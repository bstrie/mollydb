/*-------------------------------------------------------------------------
 *
 * regc_mdb_locale.c
 *	  ctype functions adapted to work on mdb_wchar (a/k/a chr),
 *	  and functions to cache the results of wholesale ctype probing.
 *
 * This file is #included by regcomp.c; it's not meant to compile standalone.
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *	  src/backend/regex/regc_mdb_locale.c
 *
 *-------------------------------------------------------------------------
 */

#include "catalog/mdb_collation.h"
#include "utils/mdb_locale.h"

/*
 * To provide as much functionality as possible on a variety of platforms,
 * without going so far as to implement everything from scratch, we use
 * several implementation strategies depending on the situation:
 *
 * 1. In C/POSIX collations, we use hard-wired code.  We can't depend on
 * the <ctype.h> functions since those will obey LC_CTYPE.  Note that these
 * collations don't give a fig about multibyte characters.
 *
 * 2. In the "default" collation (which is supposed to obey LC_CTYPE):
 *
 * 2a. When working in UTF8 encoding, we use the <wctype.h> functions if
 * available.  This assumes that every platform uses Unicode codepoints
 * directly as the wchar_t representation of Unicode.  On some platforms
 * wchar_t is only 16 bits wide, so we have to punt for codepoints > 0xFFFF.
 *
 * 2b. In all other encodings, or on machines that lack <wctype.h>, we use
 * the <ctype.h> functions for mdb_wchar values up to 255, and punt for values
 * above that.  This is only 100% correct in single-byte encodings such as
 * LATINn.  However, non-Unicode multibyte encodings are mostly Far Eastern
 * character sets for which the properties being tested here aren't very
 * relevant for higher code values anyway.  The difficulty with using the
 * <wctype.h> functions with non-Unicode multibyte encodings is that we can
 * have no certainty that the platform's wchar_t representation matches
 * what we do in mdb_wchar conversions.
 *
 * 3. Other collations are only supported on platforms that HAVE_LOCALE_T.
 * Here, we use the locale_t-extended forms of the <wctype.h> and <ctype.h>
 * functions, under exactly the same cases as #2.
 *
 * There is one notable difference between cases 2 and 3: in the "default"
 * collation we force ASCII letters to follow ASCII upcase/downcase rules,
 * while in a non-default collation we just let the library functions do what
 * they will.  The case where this matters is treatment of I/i in Turkish,
 * and the behavior is meant to match the upper()/lower() SQL functions.
 *
 * We store the active collation setting in static variables.  In principle
 * it could be passed down to here via the regex library's "struct vars" data
 * structure; but that would require somewhat invasive changes in the regex
 * library, and right now there's no real benefit to be gained from that.
 *
 * NB: the coding here assumes mdb_wchar is an unsigned type.
 */

typedef enum
{
	MDB_REGEX_LOCALE_C,			/* C locale (encoding independent) */
	MDB_REGEX_LOCALE_WIDE,		/* Use <wctype.h> functions */
	MDB_REGEX_LOCALE_1BYTE,		/* Use <ctype.h> functions */
	MDB_REGEX_LOCALE_WIDE_L,		/* Use locale_t <wctype.h> functions */
	MDB_REGEX_LOCALE_1BYTE_L		/* Use locale_t <ctype.h> functions */
} MDB_Locale_Strategy;

static MDB_Locale_Strategy mdb_regex_strategy;
static mdb_locale_t mdb_regex_locale;
static Oid	mdb_regex_collation;

/*
 * Hard-wired character properties for C locale
 */
#define MDB_ISDIGIT	0x01
#define MDB_ISALPHA	0x02
#define MDB_ISALNUM	(MDB_ISDIGIT | MDB_ISALPHA)
#define MDB_ISUPPER	0x04
#define MDB_ISLOWER	0x08
#define MDB_ISGRAPH	0x10
#define MDB_ISPRINT	0x20
#define MDB_ISPUNCT	0x40
#define MDB_ISSPACE	0x80

static const unsigned char mdb_char_properties[128] = {
	 /* NUL */ 0,
	 /* ^A */ 0,
	 /* ^B */ 0,
	 /* ^C */ 0,
	 /* ^D */ 0,
	 /* ^E */ 0,
	 /* ^F */ 0,
	 /* ^G */ 0,
	 /* ^H */ 0,
	 /* ^I */ MDB_ISSPACE,
	 /* ^J */ MDB_ISSPACE,
	 /* ^K */ MDB_ISSPACE,
	 /* ^L */ MDB_ISSPACE,
	 /* ^M */ MDB_ISSPACE,
	 /* ^N */ 0,
	 /* ^O */ 0,
	 /* ^P */ 0,
	 /* ^Q */ 0,
	 /* ^R */ 0,
	 /* ^S */ 0,
	 /* ^T */ 0,
	 /* ^U */ 0,
	 /* ^V */ 0,
	 /* ^W */ 0,
	 /* ^X */ 0,
	 /* ^Y */ 0,
	 /* ^Z */ 0,
	 /* ^[ */ 0,
	 /* ^\ */ 0,
	 /* ^] */ 0,
	 /* ^^ */ 0,
	 /* ^_ */ 0,
	 /* */ MDB_ISPRINT | MDB_ISSPACE,
	 /* !  */ MDB_ISGRAPH | MDB_ISPRINT | MDB_ISPUNCT,
	 /* "  */ MDB_ISGRAPH | MDB_ISPRINT | MDB_ISPUNCT,
	 /* #  */ MDB_ISGRAPH | MDB_ISPRINT | MDB_ISPUNCT,
	 /* $  */ MDB_ISGRAPH | MDB_ISPRINT | MDB_ISPUNCT,
	 /* %  */ MDB_ISGRAPH | MDB_ISPRINT | MDB_ISPUNCT,
	 /* &  */ MDB_ISGRAPH | MDB_ISPRINT | MDB_ISPUNCT,
	 /* '  */ MDB_ISGRAPH | MDB_ISPRINT | MDB_ISPUNCT,
	 /* (  */ MDB_ISGRAPH | MDB_ISPRINT | MDB_ISPUNCT,
	 /* )  */ MDB_ISGRAPH | MDB_ISPRINT | MDB_ISPUNCT,
	 /* *  */ MDB_ISGRAPH | MDB_ISPRINT | MDB_ISPUNCT,
	 /* +  */ MDB_ISGRAPH | MDB_ISPRINT | MDB_ISPUNCT,
	 /* ,  */ MDB_ISGRAPH | MDB_ISPRINT | MDB_ISPUNCT,
	 /* -  */ MDB_ISGRAPH | MDB_ISPRINT | MDB_ISPUNCT,
	 /* .  */ MDB_ISGRAPH | MDB_ISPRINT | MDB_ISPUNCT,
	 /* /  */ MDB_ISGRAPH | MDB_ISPRINT | MDB_ISPUNCT,
	 /* 0  */ MDB_ISDIGIT | MDB_ISGRAPH | MDB_ISPRINT,
	 /* 1  */ MDB_ISDIGIT | MDB_ISGRAPH | MDB_ISPRINT,
	 /* 2  */ MDB_ISDIGIT | MDB_ISGRAPH | MDB_ISPRINT,
	 /* 3  */ MDB_ISDIGIT | MDB_ISGRAPH | MDB_ISPRINT,
	 /* 4  */ MDB_ISDIGIT | MDB_ISGRAPH | MDB_ISPRINT,
	 /* 5  */ MDB_ISDIGIT | MDB_ISGRAPH | MDB_ISPRINT,
	 /* 6  */ MDB_ISDIGIT | MDB_ISGRAPH | MDB_ISPRINT,
	 /* 7  */ MDB_ISDIGIT | MDB_ISGRAPH | MDB_ISPRINT,
	 /* 8  */ MDB_ISDIGIT | MDB_ISGRAPH | MDB_ISPRINT,
	 /* 9  */ MDB_ISDIGIT | MDB_ISGRAPH | MDB_ISPRINT,
	 /* :  */ MDB_ISGRAPH | MDB_ISPRINT | MDB_ISPUNCT,
	 /* ;  */ MDB_ISGRAPH | MDB_ISPRINT | MDB_ISPUNCT,
	 /* <  */ MDB_ISGRAPH | MDB_ISPRINT | MDB_ISPUNCT,
	 /* =  */ MDB_ISGRAPH | MDB_ISPRINT | MDB_ISPUNCT,
	 /* >  */ MDB_ISGRAPH | MDB_ISPRINT | MDB_ISPUNCT,
	 /* ?  */ MDB_ISGRAPH | MDB_ISPRINT | MDB_ISPUNCT,
	 /* @  */ MDB_ISGRAPH | MDB_ISPRINT | MDB_ISPUNCT,
	 /* A  */ MDB_ISALPHA | MDB_ISUPPER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* B  */ MDB_ISALPHA | MDB_ISUPPER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* C  */ MDB_ISALPHA | MDB_ISUPPER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* D  */ MDB_ISALPHA | MDB_ISUPPER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* E  */ MDB_ISALPHA | MDB_ISUPPER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* F  */ MDB_ISALPHA | MDB_ISUPPER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* G  */ MDB_ISALPHA | MDB_ISUPPER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* H  */ MDB_ISALPHA | MDB_ISUPPER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* I  */ MDB_ISALPHA | MDB_ISUPPER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* J  */ MDB_ISALPHA | MDB_ISUPPER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* K  */ MDB_ISALPHA | MDB_ISUPPER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* L  */ MDB_ISALPHA | MDB_ISUPPER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* M  */ MDB_ISALPHA | MDB_ISUPPER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* N  */ MDB_ISALPHA | MDB_ISUPPER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* O  */ MDB_ISALPHA | MDB_ISUPPER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* P  */ MDB_ISALPHA | MDB_ISUPPER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* Q  */ MDB_ISALPHA | MDB_ISUPPER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* R  */ MDB_ISALPHA | MDB_ISUPPER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* S  */ MDB_ISALPHA | MDB_ISUPPER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* T  */ MDB_ISALPHA | MDB_ISUPPER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* U  */ MDB_ISALPHA | MDB_ISUPPER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* V  */ MDB_ISALPHA | MDB_ISUPPER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* W  */ MDB_ISALPHA | MDB_ISUPPER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* X  */ MDB_ISALPHA | MDB_ISUPPER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* Y  */ MDB_ISALPHA | MDB_ISUPPER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* Z  */ MDB_ISALPHA | MDB_ISUPPER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* [  */ MDB_ISGRAPH | MDB_ISPRINT | MDB_ISPUNCT,
	 /* \  */ MDB_ISGRAPH | MDB_ISPRINT | MDB_ISPUNCT,
	 /* ]  */ MDB_ISGRAPH | MDB_ISPRINT | MDB_ISPUNCT,
	 /* ^  */ MDB_ISGRAPH | MDB_ISPRINT | MDB_ISPUNCT,
	 /* _  */ MDB_ISGRAPH | MDB_ISPRINT | MDB_ISPUNCT,
	 /* `  */ MDB_ISGRAPH | MDB_ISPRINT | MDB_ISPUNCT,
	 /* a  */ MDB_ISALPHA | MDB_ISLOWER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* b  */ MDB_ISALPHA | MDB_ISLOWER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* c  */ MDB_ISALPHA | MDB_ISLOWER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* d  */ MDB_ISALPHA | MDB_ISLOWER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* e  */ MDB_ISALPHA | MDB_ISLOWER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* f  */ MDB_ISALPHA | MDB_ISLOWER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* g  */ MDB_ISALPHA | MDB_ISLOWER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* h  */ MDB_ISALPHA | MDB_ISLOWER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* i  */ MDB_ISALPHA | MDB_ISLOWER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* j  */ MDB_ISALPHA | MDB_ISLOWER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* k  */ MDB_ISALPHA | MDB_ISLOWER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* l  */ MDB_ISALPHA | MDB_ISLOWER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* m  */ MDB_ISALPHA | MDB_ISLOWER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* n  */ MDB_ISALPHA | MDB_ISLOWER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* o  */ MDB_ISALPHA | MDB_ISLOWER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* p  */ MDB_ISALPHA | MDB_ISLOWER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* q  */ MDB_ISALPHA | MDB_ISLOWER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* r  */ MDB_ISALPHA | MDB_ISLOWER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* s  */ MDB_ISALPHA | MDB_ISLOWER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* t  */ MDB_ISALPHA | MDB_ISLOWER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* u  */ MDB_ISALPHA | MDB_ISLOWER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* v  */ MDB_ISALPHA | MDB_ISLOWER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* w  */ MDB_ISALPHA | MDB_ISLOWER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* x  */ MDB_ISALPHA | MDB_ISLOWER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* y  */ MDB_ISALPHA | MDB_ISLOWER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* z  */ MDB_ISALPHA | MDB_ISLOWER | MDB_ISGRAPH | MDB_ISPRINT,
	 /* {  */ MDB_ISGRAPH | MDB_ISPRINT | MDB_ISPUNCT,
	 /* |  */ MDB_ISGRAPH | MDB_ISPRINT | MDB_ISPUNCT,
	 /* }  */ MDB_ISGRAPH | MDB_ISPRINT | MDB_ISPUNCT,
	 /* ~  */ MDB_ISGRAPH | MDB_ISPRINT | MDB_ISPUNCT,
	 /* DEL */ 0
};


/*
 * mdb_set_regex_collation: set collation for these functions to obey
 *
 * This is called when beginning compilation or execution of a regexp.
 * Since there's no need for re-entrancy of regexp operations, it's okay
 * to store the results in static variables.
 */
void
mdb_set_regex_collation(Oid collation)
{
	if (lc_ctype_is_c(collation))
	{
		/* C/POSIX collations use this path regardless of database encoding */
		mdb_regex_strategy = MDB_REGEX_LOCALE_C;
		mdb_regex_locale = 0;
		mdb_regex_collation = C_COLLATION_OID;
	}
	else
	{
		if (collation == DEFAULT_COLLATION_OID)
			mdb_regex_locale = 0;
		else if (OidIsValid(collation))
		{
			/*
			 * NB: mdb_newlocale_from_collation will fail if not HAVE_LOCALE_T;
			 * the case of mdb_regex_locale != 0 but not HAVE_LOCALE_T does not
			 * have to be considered below.
			 */
			mdb_regex_locale = mdb_newlocale_from_collation(collation);
		}
		else
		{
			/*
			 * This typically means that the parser could not resolve a
			 * conflict of implicit collations, so report it that way.
			 */
			ereport(ERROR,
					(errcode(ERRCODE_INDETERMINATE_COLLATION),
					 errmsg("could not determine which collation to use for regular expression"),
					 errhint("Use the COLLATE clause to set the collation explicitly.")));
		}

#ifdef USE_WIDE_UPPER_LOWER
		if (GetDatabaseEncoding() == MDB_UTF8)
		{
			if (mdb_regex_locale)
				mdb_regex_strategy = MDB_REGEX_LOCALE_WIDE_L;
			else
				mdb_regex_strategy = MDB_REGEX_LOCALE_WIDE;
		}
		else
#endif   /* USE_WIDE_UPPER_LOWER */
		{
			if (mdb_regex_locale)
				mdb_regex_strategy = MDB_REGEX_LOCALE_1BYTE_L;
			else
				mdb_regex_strategy = MDB_REGEX_LOCALE_1BYTE;
		}

		mdb_regex_collation = collation;
	}
}

static int
mdb_wc_isdigit(mdb_wchar c)
{
	switch (mdb_regex_strategy)
	{
		case MDB_REGEX_LOCALE_C:
			return (c <= (mdb_wchar) 127 &&
					(mdb_char_properties[c] & MDB_ISDIGIT));
		case MDB_REGEX_LOCALE_WIDE:
#ifdef USE_WIDE_UPPER_LOWER
			if (sizeof(wchar_t) >= 4 || c <= (mdb_wchar) 0xFFFF)
				return iswdigit((wint_t) c);
#endif
			/* FALL THRU */
		case MDB_REGEX_LOCALE_1BYTE:
			return (c <= (mdb_wchar) UCHAR_MAX &&
					isdigit((unsigned char) c));
		case MDB_REGEX_LOCALE_WIDE_L:
#if defined(HAVE_LOCALE_T) && defined(USE_WIDE_UPPER_LOWER)
			if (sizeof(wchar_t) >= 4 || c <= (mdb_wchar) 0xFFFF)
				return iswdigit_l((wint_t) c, mdb_regex_locale);
#endif
			/* FALL THRU */
		case MDB_REGEX_LOCALE_1BYTE_L:
#ifdef HAVE_LOCALE_T
			return (c <= (mdb_wchar) UCHAR_MAX &&
					isdigit_l((unsigned char) c, mdb_regex_locale));
#endif
			break;
	}
	return 0;					/* can't get here, but keep compiler quiet */
}

static int
mdb_wc_isalpha(mdb_wchar c)
{
	switch (mdb_regex_strategy)
	{
		case MDB_REGEX_LOCALE_C:
			return (c <= (mdb_wchar) 127 &&
					(mdb_char_properties[c] & MDB_ISALPHA));
		case MDB_REGEX_LOCALE_WIDE:
#ifdef USE_WIDE_UPPER_LOWER
			if (sizeof(wchar_t) >= 4 || c <= (mdb_wchar) 0xFFFF)
				return iswalpha((wint_t) c);
#endif
			/* FALL THRU */
		case MDB_REGEX_LOCALE_1BYTE:
			return (c <= (mdb_wchar) UCHAR_MAX &&
					isalpha((unsigned char) c));
		case MDB_REGEX_LOCALE_WIDE_L:
#if defined(HAVE_LOCALE_T) && defined(USE_WIDE_UPPER_LOWER)
			if (sizeof(wchar_t) >= 4 || c <= (mdb_wchar) 0xFFFF)
				return iswalpha_l((wint_t) c, mdb_regex_locale);
#endif
			/* FALL THRU */
		case MDB_REGEX_LOCALE_1BYTE_L:
#ifdef HAVE_LOCALE_T
			return (c <= (mdb_wchar) UCHAR_MAX &&
					isalpha_l((unsigned char) c, mdb_regex_locale));
#endif
			break;
	}
	return 0;					/* can't get here, but keep compiler quiet */
}

static int
mdb_wc_isalnum(mdb_wchar c)
{
	switch (mdb_regex_strategy)
	{
		case MDB_REGEX_LOCALE_C:
			return (c <= (mdb_wchar) 127 &&
					(mdb_char_properties[c] & MDB_ISALNUM));
		case MDB_REGEX_LOCALE_WIDE:
#ifdef USE_WIDE_UPPER_LOWER
			if (sizeof(wchar_t) >= 4 || c <= (mdb_wchar) 0xFFFF)
				return iswalnum((wint_t) c);
#endif
			/* FALL THRU */
		case MDB_REGEX_LOCALE_1BYTE:
			return (c <= (mdb_wchar) UCHAR_MAX &&
					isalnum((unsigned char) c));
		case MDB_REGEX_LOCALE_WIDE_L:
#if defined(HAVE_LOCALE_T) && defined(USE_WIDE_UPPER_LOWER)
			if (sizeof(wchar_t) >= 4 || c <= (mdb_wchar) 0xFFFF)
				return iswalnum_l((wint_t) c, mdb_regex_locale);
#endif
			/* FALL THRU */
		case MDB_REGEX_LOCALE_1BYTE_L:
#ifdef HAVE_LOCALE_T
			return (c <= (mdb_wchar) UCHAR_MAX &&
					isalnum_l((unsigned char) c, mdb_regex_locale));
#endif
			break;
	}
	return 0;					/* can't get here, but keep compiler quiet */
}

static int
mdb_wc_isupper(mdb_wchar c)
{
	switch (mdb_regex_strategy)
	{
		case MDB_REGEX_LOCALE_C:
			return (c <= (mdb_wchar) 127 &&
					(mdb_char_properties[c] & MDB_ISUPPER));
		case MDB_REGEX_LOCALE_WIDE:
#ifdef USE_WIDE_UPPER_LOWER
			if (sizeof(wchar_t) >= 4 || c <= (mdb_wchar) 0xFFFF)
				return iswupper((wint_t) c);
#endif
			/* FALL THRU */
		case MDB_REGEX_LOCALE_1BYTE:
			return (c <= (mdb_wchar) UCHAR_MAX &&
					isupper((unsigned char) c));
		case MDB_REGEX_LOCALE_WIDE_L:
#if defined(HAVE_LOCALE_T) && defined(USE_WIDE_UPPER_LOWER)
			if (sizeof(wchar_t) >= 4 || c <= (mdb_wchar) 0xFFFF)
				return iswupper_l((wint_t) c, mdb_regex_locale);
#endif
			/* FALL THRU */
		case MDB_REGEX_LOCALE_1BYTE_L:
#ifdef HAVE_LOCALE_T
			return (c <= (mdb_wchar) UCHAR_MAX &&
					isupper_l((unsigned char) c, mdb_regex_locale));
#endif
			break;
	}
	return 0;					/* can't get here, but keep compiler quiet */
}

static int
mdb_wc_islower(mdb_wchar c)
{
	switch (mdb_regex_strategy)
	{
		case MDB_REGEX_LOCALE_C:
			return (c <= (mdb_wchar) 127 &&
					(mdb_char_properties[c] & MDB_ISLOWER));
		case MDB_REGEX_LOCALE_WIDE:
#ifdef USE_WIDE_UPPER_LOWER
			if (sizeof(wchar_t) >= 4 || c <= (mdb_wchar) 0xFFFF)
				return iswlower((wint_t) c);
#endif
			/* FALL THRU */
		case MDB_REGEX_LOCALE_1BYTE:
			return (c <= (mdb_wchar) UCHAR_MAX &&
					islower((unsigned char) c));
		case MDB_REGEX_LOCALE_WIDE_L:
#if defined(HAVE_LOCALE_T) && defined(USE_WIDE_UPPER_LOWER)
			if (sizeof(wchar_t) >= 4 || c <= (mdb_wchar) 0xFFFF)
				return iswlower_l((wint_t) c, mdb_regex_locale);
#endif
			/* FALL THRU */
		case MDB_REGEX_LOCALE_1BYTE_L:
#ifdef HAVE_LOCALE_T
			return (c <= (mdb_wchar) UCHAR_MAX &&
					islower_l((unsigned char) c, mdb_regex_locale));
#endif
			break;
	}
	return 0;					/* can't get here, but keep compiler quiet */
}

static int
mdb_wc_isgraph(mdb_wchar c)
{
	switch (mdb_regex_strategy)
	{
		case MDB_REGEX_LOCALE_C:
			return (c <= (mdb_wchar) 127 &&
					(mdb_char_properties[c] & MDB_ISGRAPH));
		case MDB_REGEX_LOCALE_WIDE:
#ifdef USE_WIDE_UPPER_LOWER
			if (sizeof(wchar_t) >= 4 || c <= (mdb_wchar) 0xFFFF)
				return iswgraph((wint_t) c);
#endif
			/* FALL THRU */
		case MDB_REGEX_LOCALE_1BYTE:
			return (c <= (mdb_wchar) UCHAR_MAX &&
					isgraph((unsigned char) c));
		case MDB_REGEX_LOCALE_WIDE_L:
#if defined(HAVE_LOCALE_T) && defined(USE_WIDE_UPPER_LOWER)
			if (sizeof(wchar_t) >= 4 || c <= (mdb_wchar) 0xFFFF)
				return iswgraph_l((wint_t) c, mdb_regex_locale);
#endif
			/* FALL THRU */
		case MDB_REGEX_LOCALE_1BYTE_L:
#ifdef HAVE_LOCALE_T
			return (c <= (mdb_wchar) UCHAR_MAX &&
					isgraph_l((unsigned char) c, mdb_regex_locale));
#endif
			break;
	}
	return 0;					/* can't get here, but keep compiler quiet */
}

static int
mdb_wc_isprint(mdb_wchar c)
{
	switch (mdb_regex_strategy)
	{
		case MDB_REGEX_LOCALE_C:
			return (c <= (mdb_wchar) 127 &&
					(mdb_char_properties[c] & MDB_ISPRINT));
		case MDB_REGEX_LOCALE_WIDE:
#ifdef USE_WIDE_UPPER_LOWER
			if (sizeof(wchar_t) >= 4 || c <= (mdb_wchar) 0xFFFF)
				return iswprint((wint_t) c);
#endif
			/* FALL THRU */
		case MDB_REGEX_LOCALE_1BYTE:
			return (c <= (mdb_wchar) UCHAR_MAX &&
					isprint((unsigned char) c));
		case MDB_REGEX_LOCALE_WIDE_L:
#if defined(HAVE_LOCALE_T) && defined(USE_WIDE_UPPER_LOWER)
			if (sizeof(wchar_t) >= 4 || c <= (mdb_wchar) 0xFFFF)
				return iswprint_l((wint_t) c, mdb_regex_locale);
#endif
			/* FALL THRU */
		case MDB_REGEX_LOCALE_1BYTE_L:
#ifdef HAVE_LOCALE_T
			return (c <= (mdb_wchar) UCHAR_MAX &&
					isprint_l((unsigned char) c, mdb_regex_locale));
#endif
			break;
	}
	return 0;					/* can't get here, but keep compiler quiet */
}

static int
mdb_wc_ispunct(mdb_wchar c)
{
	switch (mdb_regex_strategy)
	{
		case MDB_REGEX_LOCALE_C:
			return (c <= (mdb_wchar) 127 &&
					(mdb_char_properties[c] & MDB_ISPUNCT));
		case MDB_REGEX_LOCALE_WIDE:
#ifdef USE_WIDE_UPPER_LOWER
			if (sizeof(wchar_t) >= 4 || c <= (mdb_wchar) 0xFFFF)
				return iswpunct((wint_t) c);
#endif
			/* FALL THRU */
		case MDB_REGEX_LOCALE_1BYTE:
			return (c <= (mdb_wchar) UCHAR_MAX &&
					ispunct((unsigned char) c));
		case MDB_REGEX_LOCALE_WIDE_L:
#if defined(HAVE_LOCALE_T) && defined(USE_WIDE_UPPER_LOWER)
			if (sizeof(wchar_t) >= 4 || c <= (mdb_wchar) 0xFFFF)
				return iswpunct_l((wint_t) c, mdb_regex_locale);
#endif
			/* FALL THRU */
		case MDB_REGEX_LOCALE_1BYTE_L:
#ifdef HAVE_LOCALE_T
			return (c <= (mdb_wchar) UCHAR_MAX &&
					ispunct_l((unsigned char) c, mdb_regex_locale));
#endif
			break;
	}
	return 0;					/* can't get here, but keep compiler quiet */
}

static int
mdb_wc_isspace(mdb_wchar c)
{
	switch (mdb_regex_strategy)
	{
		case MDB_REGEX_LOCALE_C:
			return (c <= (mdb_wchar) 127 &&
					(mdb_char_properties[c] & MDB_ISSPACE));
		case MDB_REGEX_LOCALE_WIDE:
#ifdef USE_WIDE_UPPER_LOWER
			if (sizeof(wchar_t) >= 4 || c <= (mdb_wchar) 0xFFFF)
				return iswspace((wint_t) c);
#endif
			/* FALL THRU */
		case MDB_REGEX_LOCALE_1BYTE:
			return (c <= (mdb_wchar) UCHAR_MAX &&
					isspace((unsigned char) c));
		case MDB_REGEX_LOCALE_WIDE_L:
#if defined(HAVE_LOCALE_T) && defined(USE_WIDE_UPPER_LOWER)
			if (sizeof(wchar_t) >= 4 || c <= (mdb_wchar) 0xFFFF)
				return iswspace_l((wint_t) c, mdb_regex_locale);
#endif
			/* FALL THRU */
		case MDB_REGEX_LOCALE_1BYTE_L:
#ifdef HAVE_LOCALE_T
			return (c <= (mdb_wchar) UCHAR_MAX &&
					isspace_l((unsigned char) c, mdb_regex_locale));
#endif
			break;
	}
	return 0;					/* can't get here, but keep compiler quiet */
}

static mdb_wchar
mdb_wc_toupper(mdb_wchar c)
{
	switch (mdb_regex_strategy)
	{
		case MDB_REGEX_LOCALE_C:
			if (c <= (mdb_wchar) 127)
				return mdb_ascii_toupper((unsigned char) c);
			return c;
		case MDB_REGEX_LOCALE_WIDE:
			/* force C behavior for ASCII characters, per comments above */
			if (c <= (mdb_wchar) 127)
				return mdb_ascii_toupper((unsigned char) c);
#ifdef USE_WIDE_UPPER_LOWER
			if (sizeof(wchar_t) >= 4 || c <= (mdb_wchar) 0xFFFF)
				return towupper((wint_t) c);
#endif
			/* FALL THRU */
		case MDB_REGEX_LOCALE_1BYTE:
			/* force C behavior for ASCII characters, per comments above */
			if (c <= (mdb_wchar) 127)
				return mdb_ascii_toupper((unsigned char) c);
			if (c <= (mdb_wchar) UCHAR_MAX)
				return toupper((unsigned char) c);
			return c;
		case MDB_REGEX_LOCALE_WIDE_L:
#if defined(HAVE_LOCALE_T) && defined(USE_WIDE_UPPER_LOWER)
			if (sizeof(wchar_t) >= 4 || c <= (mdb_wchar) 0xFFFF)
				return towupper_l((wint_t) c, mdb_regex_locale);
#endif
			/* FALL THRU */
		case MDB_REGEX_LOCALE_1BYTE_L:
#ifdef HAVE_LOCALE_T
			if (c <= (mdb_wchar) UCHAR_MAX)
				return toupper_l((unsigned char) c, mdb_regex_locale);
#endif
			return c;
	}
	return 0;					/* can't get here, but keep compiler quiet */
}

static mdb_wchar
mdb_wc_tolower(mdb_wchar c)
{
	switch (mdb_regex_strategy)
	{
		case MDB_REGEX_LOCALE_C:
			if (c <= (mdb_wchar) 127)
				return mdb_ascii_tolower((unsigned char) c);
			return c;
		case MDB_REGEX_LOCALE_WIDE:
			/* force C behavior for ASCII characters, per comments above */
			if (c <= (mdb_wchar) 127)
				return mdb_ascii_tolower((unsigned char) c);
#ifdef USE_WIDE_UPPER_LOWER
			if (sizeof(wchar_t) >= 4 || c <= (mdb_wchar) 0xFFFF)
				return towlower((wint_t) c);
#endif
			/* FALL THRU */
		case MDB_REGEX_LOCALE_1BYTE:
			/* force C behavior for ASCII characters, per comments above */
			if (c <= (mdb_wchar) 127)
				return mdb_ascii_tolower((unsigned char) c);
			if (c <= (mdb_wchar) UCHAR_MAX)
				return tolower((unsigned char) c);
			return c;
		case MDB_REGEX_LOCALE_WIDE_L:
#if defined(HAVE_LOCALE_T) && defined(USE_WIDE_UPPER_LOWER)
			if (sizeof(wchar_t) >= 4 || c <= (mdb_wchar) 0xFFFF)
				return towlower_l((wint_t) c, mdb_regex_locale);
#endif
			/* FALL THRU */
		case MDB_REGEX_LOCALE_1BYTE_L:
#ifdef HAVE_LOCALE_T
			if (c <= (mdb_wchar) UCHAR_MAX)
				return tolower_l((unsigned char) c, mdb_regex_locale);
#endif
			return c;
	}
	return 0;					/* can't get here, but keep compiler quiet */
}


/*
 * These functions cache the results of probing libc's ctype behavior for
 * all character codes of interest in a given encoding/collation.  The
 * result is provided as a "struct cvec", but notice that the representation
 * is a touch different from a cvec created by regc_cvec.c: we allocate the
 * chrs[] and ranges[] arrays separately from the struct so that we can
 * realloc them larger at need.  This is okay since the cvecs made here
 * should never be freed by freecvec().
 *
 * We use malloc not palloc since we mustn't lose control on out-of-memory;
 * the main regex code expects us to return a failure indication instead.
 */

typedef int (*mdb_wc_probefunc) (mdb_wchar c);

typedef struct mdb_ctype_cache
{
	mdb_wc_probefunc probefunc;	/* mdb_wc_isalpha or a sibling */
	Oid			collation;		/* collation this entry is for */
	struct cvec cv;				/* cache entry contents */
	struct mdb_ctype_cache *next;	/* chain link */
} mdb_ctype_cache;

static mdb_ctype_cache *mdb_ctype_cache_list = NULL;

/*
 * Add a chr or range to pcc->cv; return false if run out of memory
 */
static bool
store_match(mdb_ctype_cache *pcc, mdb_wchar chr1, int nchrs)
{
	chr		   *newchrs;

	if (nchrs > 1)
	{
		if (pcc->cv.nranges >= pcc->cv.rangespace)
		{
			pcc->cv.rangespace *= 2;
			newchrs = (chr *) realloc(pcc->cv.ranges,
									  pcc->cv.rangespace * sizeof(chr) * 2);
			if (newchrs == NULL)
				return false;
			pcc->cv.ranges = newchrs;
		}
		pcc->cv.ranges[pcc->cv.nranges * 2] = chr1;
		pcc->cv.ranges[pcc->cv.nranges * 2 + 1] = chr1 + nchrs - 1;
		pcc->cv.nranges++;
	}
	else
	{
		assert(nchrs == 1);
		if (pcc->cv.nchrs >= pcc->cv.chrspace)
		{
			pcc->cv.chrspace *= 2;
			newchrs = (chr *) realloc(pcc->cv.chrs,
									  pcc->cv.chrspace * sizeof(chr));
			if (newchrs == NULL)
				return false;
			pcc->cv.chrs = newchrs;
		}
		pcc->cv.chrs[pcc->cv.nchrs++] = chr1;
	}
	return true;
}

/*
 * Given a probe function (e.g., mdb_wc_isalpha) get a struct cvec for all
 * chrs satisfying the probe function.  The active collation is the one
 * previously set by mdb_set_regex_collation.  Return NULL if out of memory.
 *
 * Note that the result must not be freed or modified by caller.
 */
static struct cvec *
mdb_ctype_get_cache(mdb_wc_probefunc probefunc)
{
	mdb_ctype_cache *pcc;
	mdb_wchar	max_chr;
	mdb_wchar	cur_chr;
	int			nmatches;
	chr		   *newchrs;

	/*
	 * Do we already have the answer cached?
	 */
	for (pcc = mdb_ctype_cache_list; pcc != NULL; pcc = pcc->next)
	{
		if (pcc->probefunc == probefunc &&
			pcc->collation == mdb_regex_collation)
			return &pcc->cv;
	}

	/*
	 * Nope, so initialize some workspace ...
	 */
	pcc = (mdb_ctype_cache *) malloc(sizeof(mdb_ctype_cache));
	if (pcc == NULL)
		return NULL;
	pcc->probefunc = probefunc;
	pcc->collation = mdb_regex_collation;
	pcc->cv.nchrs = 0;
	pcc->cv.chrspace = 128;
	pcc->cv.chrs = (chr *) malloc(pcc->cv.chrspace * sizeof(chr));
	pcc->cv.nranges = 0;
	pcc->cv.rangespace = 64;
	pcc->cv.ranges = (chr *) malloc(pcc->cv.rangespace * sizeof(chr) * 2);
	if (pcc->cv.chrs == NULL || pcc->cv.ranges == NULL)
		goto out_of_memory;

	/*
	 * Decide how many character codes we ought to look through.  For C locale
	 * there's no need to go further than 127.  Otherwise, if the encoding is
	 * UTF8 go up to 0x7FF, which is a pretty arbitrary cutoff but we cannot
	 * extend it as far as we'd like (say, 0xFFFF, the end of the Basic
	 * Multilingual Plane) without creating significant performance issues due
	 * to too many characters being fed through the colormap code.  This will
	 * need redesign to fix reasonably, but at least for the moment we have
	 * all common European languages covered.  Otherwise (not C, not UTF8) go
	 * up to 255.  These limits are interrelated with restrictions discussed
	 * at the head of this file.
	 */
	switch (mdb_regex_strategy)
	{
		case MDB_REGEX_LOCALE_C:
			max_chr = (mdb_wchar) 127;
			break;
		case MDB_REGEX_LOCALE_WIDE:
		case MDB_REGEX_LOCALE_WIDE_L:
			max_chr = (mdb_wchar) 0x7FF;
			break;
		case MDB_REGEX_LOCALE_1BYTE:
		case MDB_REGEX_LOCALE_1BYTE_L:
			max_chr = (mdb_wchar) UCHAR_MAX;
			break;
		default:
			max_chr = 0;		/* can't get here, but keep compiler quiet */
			break;
	}

	/*
	 * And scan 'em ...
	 */
	nmatches = 0;				/* number of consecutive matches */

	for (cur_chr = 0; cur_chr <= max_chr; cur_chr++)
	{
		if ((*probefunc) (cur_chr))
			nmatches++;
		else if (nmatches > 0)
		{
			if (!store_match(pcc, cur_chr - nmatches, nmatches))
				goto out_of_memory;
			nmatches = 0;
		}
	}

	if (nmatches > 0)
		if (!store_match(pcc, cur_chr - nmatches, nmatches))
			goto out_of_memory;

	/*
	 * We might have allocated more memory than needed, if so free it
	 */
	if (pcc->cv.nchrs == 0)
	{
		free(pcc->cv.chrs);
		pcc->cv.chrs = NULL;
		pcc->cv.chrspace = 0;
	}
	else if (pcc->cv.nchrs < pcc->cv.chrspace)
	{
		newchrs = (chr *) realloc(pcc->cv.chrs,
								  pcc->cv.nchrs * sizeof(chr));
		if (newchrs == NULL)
			goto out_of_memory;
		pcc->cv.chrs = newchrs;
		pcc->cv.chrspace = pcc->cv.nchrs;
	}
	if (pcc->cv.nranges == 0)
	{
		free(pcc->cv.ranges);
		pcc->cv.ranges = NULL;
		pcc->cv.rangespace = 0;
	}
	else if (pcc->cv.nranges < pcc->cv.rangespace)
	{
		newchrs = (chr *) realloc(pcc->cv.ranges,
								  pcc->cv.nranges * sizeof(chr) * 2);
		if (newchrs == NULL)
			goto out_of_memory;
		pcc->cv.ranges = newchrs;
		pcc->cv.rangespace = pcc->cv.nranges;
	}

	/*
	 * Success, link it into cache chain
	 */
	pcc->next = mdb_ctype_cache_list;
	mdb_ctype_cache_list = pcc;

	return &pcc->cv;

	/*
	 * Failure, clean up
	 */
out_of_memory:
	if (pcc->cv.chrs)
		free(pcc->cv.chrs);
	if (pcc->cv.ranges)
		free(pcc->cv.ranges);
	free(pcc);

	return NULL;
}
