/*-------------------------------------------------------------------------
 *
 * chklocale.c
 *		Functions for handling locale-related info
 *
 *
 * Copyright (c) 1996-2016, MollyDB Global Development Group
 *
 *
 * IDENTIFICATION
 *	  src/port/chklocale.c
 *
 *-------------------------------------------------------------------------
 */

#ifndef FRONTEND
#include "mollydb.h"
#else
#include "mollydb_fe.h"
#endif

#if defined(WIN32) && (_MSC_VER >= 1900)
#include <windows.h>
#endif

#include <locale.h>
#ifdef HAVE_LANGINFO_H
#include <langinfo.h>
#endif

#include "mb/mdb_wchar.h"


/*
 * This table needs to recognize all the CODESET spellings for supported
 * backend encodings, as well as frontend-only encodings where possible
 * (the latter case is currently only needed for initdb to recognize
 * error situations).  On Windows, we rely on entries for codepage
 * numbers (CPnnn).
 *
 * Note that we search the table with mdb_strcasecmp(), so variant
 * capitalizations don't need their own entries.
 */
struct encoding_match
{
	enum mdb_enc mdb_enc_code;
	const char *system_enc_name;
};

static const struct encoding_match encoding_match_list[] = {
	{MDB_EUC_JP, "EUC-JP"},
	{MDB_EUC_JP, "eucJP"},
	{MDB_EUC_JP, "IBM-eucJP"},
	{MDB_EUC_JP, "sdeckanji"},
	{MDB_EUC_JP, "CP20932"},

	{MDB_EUC_CN, "EUC-CN"},
	{MDB_EUC_CN, "eucCN"},
	{MDB_EUC_CN, "IBM-eucCN"},
	{MDB_EUC_CN, "GB2312"},
	{MDB_EUC_CN, "dechanzi"},
	{MDB_EUC_CN, "CP20936"},

	{MDB_EUC_KR, "EUC-KR"},
	{MDB_EUC_KR, "eucKR"},
	{MDB_EUC_KR, "IBM-eucKR"},
	{MDB_EUC_KR, "deckorean"},
	{MDB_EUC_KR, "5601"},
	{MDB_EUC_KR, "CP51949"},

	{MDB_EUC_TW, "EUC-TW"},
	{MDB_EUC_TW, "eucTW"},
	{MDB_EUC_TW, "IBM-eucTW"},
	{MDB_EUC_TW, "cns11643"},
	/* No codepage for EUC-TW ? */

	{MDB_UTF8, "UTF-8"},
	{MDB_UTF8, "utf8"},
	{MDB_UTF8, "CP65001"},

	{MDB_LATIN1, "ISO-8859-1"},
	{MDB_LATIN1, "ISO8859-1"},
	{MDB_LATIN1, "iso88591"},
	{MDB_LATIN1, "CP28591"},

	{MDB_LATIN2, "ISO-8859-2"},
	{MDB_LATIN2, "ISO8859-2"},
	{MDB_LATIN2, "iso88592"},
	{MDB_LATIN2, "CP28592"},

	{MDB_LATIN3, "ISO-8859-3"},
	{MDB_LATIN3, "ISO8859-3"},
	{MDB_LATIN3, "iso88593"},
	{MDB_LATIN3, "CP28593"},

	{MDB_LATIN4, "ISO-8859-4"},
	{MDB_LATIN4, "ISO8859-4"},
	{MDB_LATIN4, "iso88594"},
	{MDB_LATIN4, "CP28594"},

	{MDB_LATIN5, "ISO-8859-9"},
	{MDB_LATIN5, "ISO8859-9"},
	{MDB_LATIN5, "iso88599"},
	{MDB_LATIN5, "CP28599"},

	{MDB_LATIN6, "ISO-8859-10"},
	{MDB_LATIN6, "ISO8859-10"},
	{MDB_LATIN6, "iso885910"},

	{MDB_LATIN7, "ISO-8859-13"},
	{MDB_LATIN7, "ISO8859-13"},
	{MDB_LATIN7, "iso885913"},

	{MDB_LATIN8, "ISO-8859-14"},
	{MDB_LATIN8, "ISO8859-14"},
	{MDB_LATIN8, "iso885914"},

	{MDB_LATIN9, "ISO-8859-15"},
	{MDB_LATIN9, "ISO8859-15"},
	{MDB_LATIN9, "iso885915"},
	{MDB_LATIN9, "CP28605"},

	{MDB_LATIN10, "ISO-8859-16"},
	{MDB_LATIN10, "ISO8859-16"},
	{MDB_LATIN10, "iso885916"},

	{MDB_KOI8R, "KOI8-R"},
	{MDB_KOI8R, "CP20866"},

	{MDB_KOI8U, "KOI8-U"},
	{MDB_KOI8U, "CP21866"},

	{MDB_WIN866, "CP866"},
	{MDB_WIN874, "CP874"},
	{MDB_WIN1250, "CP1250"},
	{MDB_WIN1251, "CP1251"},
	{MDB_WIN1251, "ansi-1251"},
	{MDB_WIN1252, "CP1252"},
	{MDB_WIN1253, "CP1253"},
	{MDB_WIN1254, "CP1254"},
	{MDB_WIN1255, "CP1255"},
	{MDB_WIN1256, "CP1256"},
	{MDB_WIN1257, "CP1257"},
	{MDB_WIN1258, "CP1258"},

	{MDB_ISO_8859_5, "ISO-8859-5"},
	{MDB_ISO_8859_5, "ISO8859-5"},
	{MDB_ISO_8859_5, "iso88595"},
	{MDB_ISO_8859_5, "CP28595"},

	{MDB_ISO_8859_6, "ISO-8859-6"},
	{MDB_ISO_8859_6, "ISO8859-6"},
	{MDB_ISO_8859_6, "iso88596"},
	{MDB_ISO_8859_6, "CP28596"},

	{MDB_ISO_8859_7, "ISO-8859-7"},
	{MDB_ISO_8859_7, "ISO8859-7"},
	{MDB_ISO_8859_7, "iso88597"},
	{MDB_ISO_8859_7, "CP28597"},

	{MDB_ISO_8859_8, "ISO-8859-8"},
	{MDB_ISO_8859_8, "ISO8859-8"},
	{MDB_ISO_8859_8, "iso88598"},
	{MDB_ISO_8859_8, "CP28598"},

	{MDB_SJIS, "SJIS"},
	{MDB_SJIS, "PCK"},
	{MDB_SJIS, "CP932"},
	{MDB_SJIS, "SHIFT_JIS"},

	{MDB_BIG5, "BIG5"},
	{MDB_BIG5, "BIG5HKSCS"},
	{MDB_BIG5, "Big5-HKSCS"},
	{MDB_BIG5, "CP950"},

	{MDB_GBK, "GBK"},
	{MDB_GBK, "CP936"},

	{MDB_UHC, "UHC"},
	{MDB_UHC, "CP949"},

	{MDB_JOHAB, "JOHAB"},
	{MDB_JOHAB, "CP1361"},

	{MDB_GB18030, "GB18030"},
	{MDB_GB18030, "CP54936"},

	{MDB_SHIFT_JIS_2004, "SJIS_2004"},

	{MDB_SQL_ASCII, "US-ASCII"},

	{MDB_SQL_ASCII, NULL}		/* end marker */
};

#ifdef WIN32
/*
 * On Windows, use CP<code page number> instead of the nl_langinfo() result
 *
 * Visual Studio 2012 expanded the set of valid LC_CTYPE values, so have its
 * locale machinery determine the code page.  See comments at IsoLocaleName().
 * For other compilers, follow the locale's predictable format.
 *
 * Visual Studio 2015 should still be able to do the same, but the declaration
 * of lc_codepage is missing in _locale_t, causing this code compilation to
 * fail, hence this falls back instead on GetLocaleInfoEx. VS 2015 may be an
 * exception and post-VS2015 versions should be able to handle properly the
 * codepage number using _create_locale(). So, instead of the same logic as
 * VS 2012 and VS 2013, this routine uses GetLocaleInfoEx to parse short
 * locale names like "de-DE", "fr-FR", etc. If those cannot be parsed correctly
 * process falls back to the pre-VS-2010 manual parsing done with
 * using <Language>_<Country>.<CodePage> as a base.
 *
 * Returns a malloc()'d string for the caller to free.
 */
static char *
win32_langinfo(const char *ctype)
{
	char	   *r = NULL;

#if (_MSC_VER >= 1700) && (_MSC_VER < 1900)
	_locale_t	loct = NULL;

	loct = _create_locale(LC_CTYPE, ctype);
	if (loct != NULL)
	{
		r = malloc(16);			/* excess */
		if (r != NULL)
			sprintf(r, "CP%u", loct->locinfo->lc_codepage);
		_free_locale(loct);
	}
#else
	char	   *codepage;

#if (_MSC_VER >= 1900)
	uint32		cp;
	WCHAR		wctype[LOCALE_NAME_MAX_LENGTH];

	memset(wctype, 0, sizeof(wctype));
	MultiByteToWideChar(CP_ACP, 0, ctype, -1, wctype, LOCALE_NAME_MAX_LENGTH);

	if (GetLocaleInfoEx(wctype,
			LOCALE_IDEFAULTANSICODEPAGE | LOCALE_RETURN_NUMBER,
			(LPWSTR) &cp, sizeof(cp) / sizeof(WCHAR)) > 0)
	{
		r = malloc(16);			/* excess */
		if (r != NULL)
			sprintf(r, "CP%u", cp);
	}
	else
#endif
	{

		/*
		 * Locale format on Win32 is <Language>_<Country>.<CodePage> . For
		 * example, English_United States.1252.
		 */
		codepage = strrchr(ctype, '.');
		if (codepage != NULL)
		{
			int			ln;

			codepage++;
			ln = strlen(codepage);
			r = malloc(ln + 3);
			if (r != NULL)
				sprintf(r, "CP%s", codepage);
		}

	}
#endif

	return r;
}

#ifndef FRONTEND
/*
 * Given a Windows code page identifier, find the corresponding MollyDB
 * encoding.  Issue a warning and return -1 if none found.
 */
int
mdb_codepage_to_encoding(UINT cp)
{
	char		sys[16];
	int			i;

	sprintf(sys, "CP%u", cp);

	/* Check the table */
	for (i = 0; encoding_match_list[i].system_enc_name; i++)
		if (mdb_strcasecmp(sys, encoding_match_list[i].system_enc_name) == 0)
			return encoding_match_list[i].mdb_enc_code;

	ereport(WARNING,
			(errmsg("could not determine encoding for codeset \"%s\"", sys),
		   errdetail("Please report this to <mdb-bugs@mollydb.org>.")));

	return -1;
}
#endif
#endif   /* WIN32 */

#if (defined(HAVE_LANGINFO_H) && defined(CODESET)) || defined(WIN32)

/*
 * Given a setting for LC_CTYPE, return the MollyDB ID of the associated
 * encoding, if we can determine it.  Return -1 if we can't determine it.
 *
 * Pass in NULL to get the encoding for the current locale setting.
 * Pass "" to get the encoding selected by the server's environment.
 *
 * If the result is MDB_SQL_ASCII, callers should treat it as being compatible
 * with any desired encoding.
 *
 * If running in the backend and write_message is false, this function must
 * cope with the possibility that elog() and palloc() are not yet usable.
 */
int
mdb_get_encoding_from_locale(const char *ctype, bool write_message)
{
	char	   *sys;
	int			i;

	/* Get the CODESET property, and also LC_CTYPE if not passed in */
	if (ctype)
	{
		char	   *save;
		char	   *name;

		/* If locale is C or POSIX, we can allow all encodings */
		if (mdb_strcasecmp(ctype, "C") == 0 ||
			mdb_strcasecmp(ctype, "POSIX") == 0)
			return MDB_SQL_ASCII;

		save = setlocale(LC_CTYPE, NULL);
		if (!save)
			return -1;			/* setlocale() broken? */
		/* must copy result, or it might change after setlocale */
		save = strdup(save);
		if (!save)
			return -1;			/* out of memory; unlikely */

		name = setlocale(LC_CTYPE, ctype);
		if (!name)
		{
			free(save);
			return -1;			/* bogus ctype passed in? */
		}

#ifndef WIN32
		sys = nl_langinfo(CODESET);
		if (sys)
			sys = strdup(sys);
#else
		sys = win32_langinfo(name);
#endif

		setlocale(LC_CTYPE, save);
		free(save);
	}
	else
	{
		/* much easier... */
		ctype = setlocale(LC_CTYPE, NULL);
		if (!ctype)
			return -1;			/* setlocale() broken? */

		/* If locale is C or POSIX, we can allow all encodings */
		if (mdb_strcasecmp(ctype, "C") == 0 ||
			mdb_strcasecmp(ctype, "POSIX") == 0)
			return MDB_SQL_ASCII;

#ifndef WIN32
		sys = nl_langinfo(CODESET);
		if (sys)
			sys = strdup(sys);
#else
		sys = win32_langinfo(ctype);
#endif
	}

	if (!sys)
		return -1;				/* out of memory; unlikely */

	/* Check the table */
	for (i = 0; encoding_match_list[i].system_enc_name; i++)
	{
		if (mdb_strcasecmp(sys, encoding_match_list[i].system_enc_name) == 0)
		{
			free(sys);
			return encoding_match_list[i].mdb_enc_code;
		}
	}

	/* Special-case kluges for particular platforms go here */

#ifdef __darwin__

	/*
	 * Current OS X has many locales that report an empty string for CODESET,
	 * but they all seem to actually use UTF-8.
	 */
	if (strlen(sys) == 0)
	{
		free(sys);
		return MDB_UTF8;
	}
#endif

	/*
	 * We print a warning if we got a CODESET string but couldn't recognize
	 * it.  This means we need another entry in the table.
	 */
	if (write_message)
	{
#ifdef FRONTEND
		fprintf(stderr, _("could not determine encoding for locale \"%s\": codeset is \"%s\""),
				ctype, sys);
		/* keep newline separate so there's only one translatable string */
		fputc('\n', stderr);
#else
		ereport(WARNING,
				(errmsg("could not determine encoding for locale \"%s\": codeset is \"%s\"",
						ctype, sys),
		   errdetail("Please report this to <mdb-bugs@mollydb.org>.")));
#endif
	}

	free(sys);
	return -1;
}
#else							/* (HAVE_LANGINFO_H && CODESET) || WIN32 */

/*
 * stub if no multi-language platform support
 *
 * Note: we could return -1 here, but that would have the effect of
 * forcing users to specify an encoding to initdb on such platforms.
 * It seems better to silently default to SQL_ASCII.
 */
int
mdb_get_encoding_from_locale(const char *ctype, bool write_message)
{
	return MDB_SQL_ASCII;
}

#endif   /* (HAVE_LANGINFO_H && CODESET) || WIN32 */
