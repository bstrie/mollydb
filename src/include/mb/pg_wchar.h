/*-------------------------------------------------------------------------
 *
 * mdb_wchar.h
 *	  multibyte-character support
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/mb/mdb_wchar.h
 *
 *	NOTES
 *		This is used both by the backend and by libpq, but should not be
 *		included by libpq client programs.  In particular, a libpq client
 *		should not assume that the encoding IDs used by the version of libpq
 *		it's linked to match up with the IDs declared here.
 *
 *-------------------------------------------------------------------------
 */
#ifndef MDB_WCHAR_H
#define MDB_WCHAR_H

/*
 * The mdb_wchar type
 */
typedef unsigned int mdb_wchar;

/*
 * Maximum byte length of multibyte characters in any backend encoding
 */
#define MAX_MULTIBYTE_CHAR_LEN	4

/*
 * various definitions for EUC
 */
#define SS2 0x8e				/* single shift 2 (JIS0201) */
#define SS3 0x8f				/* single shift 3 (JIS0212) */

/*
 * SJIS validation macros
 */
#define ISSJISHEAD(c) (((c) >= 0x81 && (c) <= 0x9f) || ((c) >= 0xe0 && (c) <= 0xfc))
#define ISSJISTAIL(c) (((c) >= 0x40 && (c) <= 0x7e) || ((c) >= 0x80 && (c) <= 0xfc))

/*----------------------------------------------------
 * MULE Internal Encoding (MIC)
 *
 * This encoding follows the design used within XEmacs; it is meant to
 * subsume many externally-defined character sets.  Each character includes
 * identification of the character set it belongs to, so the encoding is
 * general but somewhat bulky.
 *
 * Currently MollyDB supports 5 types of MULE character sets:
 *
 * 1) 1-byte ASCII characters.  Each byte is below 0x80.
 *
 * 2) "Official" single byte charsets such as ISO-8859-1 (Latin1).
 *	  Each MULE character consists of 2 bytes: LC1 + C1, where LC1 is
 *	  an identifier for the charset (in the range 0x81 to 0x8d) and C1
 *	  is the character code (in the range 0xa0 to 0xff).
 *
 * 3) "Private" single byte charsets such as SISHENG.  Each MULE
 *	  character consists of 3 bytes: LCPRV1 + LC12 + C1, where LCPRV1
 *	  is a private-charset flag, LC12 is an identifier for the charset,
 *	  and C1 is the character code (in the range 0xa0 to 0xff).
 *	  LCPRV1 is either 0x9a (if LC12 is in the range 0xa0 to 0xdf)
 *	  or 0x9b (if LC12 is in the range 0xe0 to 0xef).
 *
 * 4) "Official" multibyte charsets such as JIS X0208.  Each MULE
 *	  character consists of 3 bytes: LC2 + C1 + C2, where LC2 is
 *	  an identifier for the charset (in the range 0x90 to 0x99) and C1
 *	  and C2 form the character code (each in the range 0xa0 to 0xff).
 *
 * 5) "Private" multibyte charsets such as CNS 11643-1992 Plane 3.
 *	  Each MULE character consists of 4 bytes: LCPRV2 + LC22 + C1 + C2,
 *	  where LCPRV2 is a private-charset flag, LC22 is an identifier for
 *	  the charset, and C1 and C2 form the character code (each in the range
 *	  0xa0 to 0xff).  LCPRV2 is either 0x9c (if LC22 is in the range 0xf0
 *	  to 0xf4) or 0x9d (if LC22 is in the range 0xf5 to 0xfe).
 *
 * "Official" encodings are those that have been assigned code numbers by
 * the XEmacs project; "private" encodings have MollyDB-specific charset
 * identifiers.
 *
 * See the "XEmacs Internals Manual", available at http://www.xemacs.org,
 * for more details.  Note that for historical reasons, MollyDB'
 * private-charset flag values do not match what XEmacs says they should be,
 * so this isn't really exactly MULE (not that private charsets would be
 * interoperable anyway).
 *
 * Note that XEmacs's implementation is different from what emacs does.
 * We follow emacs's implementation, rather than XEmacs's.
 *----------------------------------------------------
 */

/*
 * Charset identifiers (also called "leading bytes" in the MULE documentation)
 */

/*
 * Charset IDs for official single byte encodings (0x81-0x8e)
 */
#define LC_ISO8859_1		0x81	/* ISO8859 Latin 1 */
#define LC_ISO8859_2		0x82	/* ISO8859 Latin 2 */
#define LC_ISO8859_3		0x83	/* ISO8859 Latin 3 */
#define LC_ISO8859_4		0x84	/* ISO8859 Latin 4 */
#define LC_TIS620			0x85	/* Thai (not supported yet) */
#define LC_ISO8859_7		0x86	/* Greek (not supported yet) */
#define LC_ISO8859_6		0x87	/* Arabic (not supported yet) */
#define LC_ISO8859_8		0x88	/* Hebrew (not supported yet) */
#define LC_JISX0201K		0x89	/* Japanese 1 byte kana */
#define LC_JISX0201R		0x8a	/* Japanese 1 byte Roman */
/* Note that 0x8b seems to be unused as of Emacs 20.7.
 * However, there might be a chance that 0x8b could be used
 * in later versions of Emacs.
 */
#define LC_KOI8_R			0x8b	/* Cyrillic KOI8-R */
#define LC_ISO8859_5		0x8c	/* ISO8859 Cyrillic */
#define LC_ISO8859_9		0x8d	/* ISO8859 Latin 5 (not supported yet) */
#define LC_ISO8859_15		0x8e	/* ISO8859 Latin 15 (not supported yet) */
/* #define CONTROL_1		0x8f	control characters (unused) */

/* Is a leading byte for "official" single byte encodings? */
#define IS_LC1(c)	((unsigned char)(c) >= 0x81 && (unsigned char)(c) <= 0x8d)

/*
 * Charset IDs for official multibyte encodings (0x90-0x99)
 * 0x9a-0x9d are free. 0x9e and 0x9f are reserved.
 */
#define LC_JISX0208_1978	0x90	/* Japanese Kanji, old JIS (not supported) */
#define LC_GB2312_80		0x91	/* Chinese */
#define LC_JISX0208			0x92	/* Japanese Kanji (JIS X 0208) */
#define LC_KS5601			0x93	/* Korean */
#define LC_JISX0212			0x94	/* Japanese Kanji (JIS X 0212) */
#define LC_CNS11643_1		0x95	/* CNS 11643-1992 Plane 1 */
#define LC_CNS11643_2		0x96	/* CNS 11643-1992 Plane 2 */
#define LC_JISX0213_1		0x97/* Japanese Kanji (JIS X 0213 Plane 1) (not
								 * supported) */
#define LC_BIG5_1			0x98	/* Plane 1 Chinese traditional (not supported) */
#define LC_BIG5_2			0x99	/* Plane 1 Chinese traditional (not supported) */

/* Is a leading byte for "official" multibyte encodings? */
#define IS_LC2(c)	((unsigned char)(c) >= 0x90 && (unsigned char)(c) <= 0x99)

/*
 * MollyDB-specific prefix bytes for "private" single byte encodings
 * (According to the MULE docs, we should be using 0x9e for this)
 */
#define LCPRV1_A		0x9a
#define LCPRV1_B		0x9b
#define IS_LCPRV1(c)	((unsigned char)(c) == LCPRV1_A || (unsigned char)(c) == LCPRV1_B)
#define IS_LCPRV1_A_RANGE(c)	\
	((unsigned char)(c) >= 0xa0 && (unsigned char)(c) <= 0xdf)
#define IS_LCPRV1_B_RANGE(c)	\
	((unsigned char)(c) >= 0xe0 && (unsigned char)(c) <= 0xef)

/*
 * MollyDB-specific prefix bytes for "private" multibyte encodings
 * (According to the MULE docs, we should be using 0x9f for this)
 */
#define LCPRV2_A		0x9c
#define LCPRV2_B		0x9d
#define IS_LCPRV2(c)	((unsigned char)(c) == LCPRV2_A || (unsigned char)(c) == LCPRV2_B)
#define IS_LCPRV2_A_RANGE(c)	\
	((unsigned char)(c) >= 0xf0 && (unsigned char)(c) <= 0xf4)
#define IS_LCPRV2_B_RANGE(c)	\
	((unsigned char)(c) >= 0xf5 && (unsigned char)(c) <= 0xfe)

/*
 * Charset IDs for private single byte encodings (0xa0-0xef)
 */
#define LC_SISHENG			0xa0/* Chinese SiSheng characters for
								 * PinYin/ZhuYin (not supported) */
#define LC_IPA				0xa1/* IPA (International Phonetic Association)
								 * (not supported) */
#define LC_VISCII_LOWER		0xa2/* Vietnamese VISCII1.1 lower-case (not
								 * supported) */
#define LC_VISCII_UPPER		0xa3/* Vietnamese VISCII1.1 upper-case (not
								 * supported) */
#define LC_ARABIC_DIGIT		0xa4	/* Arabic digit (not supported) */
#define LC_ARABIC_1_COLUMN	0xa5	/* Arabic 1-column (not supported) */
#define LC_ASCII_RIGHT_TO_LEFT	0xa6	/* ASCII (left half of ISO8859-1) with
										 * right-to-left direction (not
										 * supported) */
#define LC_LAO				0xa7/* Lao characters (ISO10646 0E80..0EDF) (not
								 * supported) */
#define LC_ARABIC_2_COLUMN	0xa8	/* Arabic 1-column (not supported) */

/*
 * Charset IDs for private multibyte encodings (0xf0-0xff)
 */
#define LC_INDIAN_1_COLUMN	0xf0/* Indian charset for 1-column width glyphs
								 * (not supported) */
#define LC_TIBETAN_1_COLUMN 0xf1/* Tibetan 1-column width glyphs (not
								 * supported) */
#define LC_UNICODE_SUBSET_2 0xf2/* Unicode characters of the range
								 * U+2500..U+33FF. (not supported) */
#define LC_UNICODE_SUBSET_3 0xf3/* Unicode characters of the range
								 * U+E000..U+FFFF. (not supported) */
#define LC_UNICODE_SUBSET	0xf4/* Unicode characters of the range
								 * U+0100..U+24FF. (not supported) */
#define LC_ETHIOPIC			0xf5	/* Ethiopic characters (not supported) */
#define LC_CNS11643_3		0xf6	/* CNS 11643-1992 Plane 3 */
#define LC_CNS11643_4		0xf7	/* CNS 11643-1992 Plane 4 */
#define LC_CNS11643_5		0xf8	/* CNS 11643-1992 Plane 5 */
#define LC_CNS11643_6		0xf9	/* CNS 11643-1992 Plane 6 */
#define LC_CNS11643_7		0xfa	/* CNS 11643-1992 Plane 7 */
#define LC_INDIAN_2_COLUMN	0xfb/* Indian charset for 2-column width glyphs
								 * (not supported) */
#define LC_TIBETAN			0xfc	/* Tibetan (not supported) */
/* #define FREE				0xfd	free (unused) */
/* #define FREE				0xfe	free (unused) */
/* #define FREE				0xff	free (unused) */

/*----------------------------------------------------
 * end of MULE stuff
 *----------------------------------------------------
 */

/*
 * MollyDB encoding identifiers
 *
 * WARNING: the order of this enum must be same as order of entries
 *			in the mdb_enc2name_tbl[] array (in mb/encnames.c), and
 *			in the mdb_wchar_table[] array (in mb/wchar.c)!
 *
 *			If you add some encoding don't forget to check
 *			MDB_ENCODING_BE_LAST macro.
 *
 * MDB_SQL_ASCII is default encoding and must be = 0.
 *
 * XXX	We must avoid renumbering any backend encoding until libpq's major
 * version number is increased beyond 5; it turns out that the backend
 * encoding IDs are effectively part of libpq's ABI as far as 8.2 initdb and
 * psql are concerned.
 */
typedef enum mdb_enc
{
	MDB_SQL_ASCII = 0,			/* SQL/ASCII */
	MDB_EUC_JP,					/* EUC for Japanese */
	MDB_EUC_CN,					/* EUC for Chinese */
	MDB_EUC_KR,					/* EUC for Korean */
	MDB_EUC_TW,					/* EUC for Taiwan */
	MDB_EUC_JIS_2004,			/* EUC-JIS-2004 */
	MDB_UTF8,					/* Unicode UTF8 */
	MDB_MULE_INTERNAL,			/* Mule internal code */
	MDB_LATIN1,					/* ISO-8859-1 Latin 1 */
	MDB_LATIN2,					/* ISO-8859-2 Latin 2 */
	MDB_LATIN3,					/* ISO-8859-3 Latin 3 */
	MDB_LATIN4,					/* ISO-8859-4 Latin 4 */
	MDB_LATIN5,					/* ISO-8859-9 Latin 5 */
	MDB_LATIN6,					/* ISO-8859-10 Latin6 */
	MDB_LATIN7,					/* ISO-8859-13 Latin7 */
	MDB_LATIN8,					/* ISO-8859-14 Latin8 */
	MDB_LATIN9,					/* ISO-8859-15 Latin9 */
	MDB_LATIN10,					/* ISO-8859-16 Latin10 */
	MDB_WIN1256,					/* windows-1256 */
	MDB_WIN1258,					/* Windows-1258 */
	MDB_WIN866,					/* (MS-DOS CP866) */
	MDB_WIN874,					/* windows-874 */
	MDB_KOI8R,					/* KOI8-R */
	MDB_WIN1251,					/* windows-1251 */
	MDB_WIN1252,					/* windows-1252 */
	MDB_ISO_8859_5,				/* ISO-8859-5 */
	MDB_ISO_8859_6,				/* ISO-8859-6 */
	MDB_ISO_8859_7,				/* ISO-8859-7 */
	MDB_ISO_8859_8,				/* ISO-8859-8 */
	MDB_WIN1250,					/* windows-1250 */
	MDB_WIN1253,					/* windows-1253 */
	MDB_WIN1254,					/* windows-1254 */
	MDB_WIN1255,					/* windows-1255 */
	MDB_WIN1257,					/* windows-1257 */
	MDB_KOI8U,					/* KOI8-U */
	/* MDB_ENCODING_BE_LAST points to the above entry */

	/* followings are for client encoding only */
	MDB_SJIS,					/* Shift JIS (Windows-932) */
	MDB_BIG5,					/* Big5 (Windows-950) */
	MDB_GBK,						/* GBK (Windows-936) */
	MDB_UHC,						/* UHC (Windows-949) */
	MDB_GB18030,					/* GB18030 */
	MDB_JOHAB,					/* EUC for Korean JOHAB */
	MDB_SHIFT_JIS_2004,			/* Shift-JIS-2004 */
	_MDB_LAST_ENCODING_			/* mark only */

} mdb_enc;

#define MDB_ENCODING_BE_LAST MDB_KOI8U

/*
 * Please use these tests before access to mdb_encconv_tbl[]
 * or to other places...
 */
#define MDB_VALID_BE_ENCODING(_enc) \
		((_enc) >= 0 && (_enc) <= MDB_ENCODING_BE_LAST)

#define MDB_ENCODING_IS_CLIENT_ONLY(_enc) \
		((_enc) > MDB_ENCODING_BE_LAST && (_enc) < _MDB_LAST_ENCODING_)

#define MDB_VALID_ENCODING(_enc) \
		((_enc) >= 0 && (_enc) < _MDB_LAST_ENCODING_)

/* On FE are possible all encodings */
#define MDB_VALID_FE_ENCODING(_enc)	MDB_VALID_ENCODING(_enc)

/*
 * Table for mapping an encoding number to official encoding name and
 * possibly other subsidiary data.  Be careful to check encoding number
 * before accessing a table entry!
 *
 * if (MDB_VALID_ENCODING(encoding))
 *		mdb_enc2name_tbl[ encoding ];
 */
typedef struct mdb_enc2name
{
	const char *name;
	mdb_enc		encoding;
#ifdef WIN32
	unsigned	codepage;		/* codepage for WIN32 */
#endif
} mdb_enc2name;

extern const mdb_enc2name mdb_enc2name_tbl[];

/*
 * Encoding names for gettext
 */
typedef struct mdb_enc2gettext
{
	mdb_enc		encoding;
	const char *name;
} mdb_enc2gettext;

extern const mdb_enc2gettext mdb_enc2gettext_tbl[];

/*
 * mdb_wchar stuff
 */
typedef int (*mb2wchar_with_len_converter) (const unsigned char *from,
														mdb_wchar *to,
														int len);

typedef int (*wchar2mb_with_len_converter) (const mdb_wchar *from,
														unsigned char *to,
														int len);

typedef int (*mblen_converter) (const unsigned char *mbstr);

typedef int (*mbdisplaylen_converter) (const unsigned char *mbstr);

typedef bool (*mbcharacter_incrementer) (unsigned char *mbstr, int len);

typedef int (*mbverifier) (const unsigned char *mbstr, int len);

typedef struct
{
	mb2wchar_with_len_converter mb2wchar_with_len;		/* convert a multibyte
														 * string to a wchar */
	wchar2mb_with_len_converter wchar2mb_with_len;		/* convert a wchar
														 * string to a multibyte */
	mblen_converter mblen;		/* get byte length of a char */
	mbdisplaylen_converter dsplen;		/* get display width of a char */
	mbverifier	mbverify;		/* verify multibyte sequence */
	int			maxmblen;		/* max bytes for a char in this encoding */
} mdb_wchar_tbl;

extern const mdb_wchar_tbl mdb_wchar_table[];

/*
 * Data structures for conversions between UTF-8 and other encodings
 * (UtfToLocal() and LocalToUtf()).  In these data structures, characters of
 * either encoding are represented by uint32 words; hence we can only support
 * characters up to 4 bytes long.  For example, the byte sequence 0xC2 0x89
 * would be represented by 0x0000C289, and 0xE8 0xA2 0xB4 by 0x00E8A2B4.
 *
 * Maps are arrays of these structs, which must be in order by the lookup key
 * (so that bsearch() can be used).
 *
 * UTF-8 to local code conversion map
 */
typedef struct
{
	uint32		utf;			/* UTF-8 */
	uint32		code;			/* local code */
} mdb_utf_to_local;

/*
 * local code to UTF-8 conversion map
 */
typedef struct
{
	uint32		code;			/* local code */
	uint32		utf;			/* UTF-8 */
} mdb_local_to_utf;

/*
 * UTF-8 to local code conversion map (for combined characters)
 */
typedef struct
{
	uint32		utf1;			/* UTF-8 code 1 */
	uint32		utf2;			/* UTF-8 code 2 */
	uint32		code;			/* local code */
} mdb_utf_to_local_combined;

/*
 * local code to UTF-8 conversion map (for combined characters)
 */
typedef struct
{
	uint32		code;			/* local code */
	uint32		utf1;			/* UTF-8 code 1 */
	uint32		utf2;			/* UTF-8 code 2 */
} mdb_local_to_utf_combined;

/*
 * callback function for algorithmic encoding conversions (in either direction)
 *
 * if function returns zero, it does not know how to convert the code
 */
typedef uint32 (*utf_local_conversion_func) (uint32 code);

/*
 * Support macro for encoding conversion functions to validate their
 * arguments.  (This could be made more compact if we included fmgr.h
 * here, but we don't want to do that because this header file is also
 * used by frontends.)
 */
#define CHECK_ENCODING_CONVERSION_ARGS(srcencoding,destencoding) \
	check_encoding_conversion_args(MDB_GETARG_INT32(0), \
								   MDB_GETARG_INT32(1), \
								   MDB_GETARG_INT32(4), \
								   (srcencoding), \
								   (destencoding))


/*
 * These functions are considered part of libpq's exported API and
 * are also declared in libpq-fe.h.
 */
extern int	mdb_char_to_encoding(const char *name);
extern const char *mdb_encoding_to_char(int encoding);
extern int	mdb_valid_server_encoding_id(int encoding);

/*
 * Remaining functions are not considered part of libpq's API, though many
 * of them do exist inside libpq.
 */
extern int	mdb_mb2wchar(const char *from, mdb_wchar *to);
extern int	mdb_mb2wchar_with_len(const char *from, mdb_wchar *to, int len);
extern int mdb_encoding_mb2wchar_with_len(int encoding,
							  const char *from, mdb_wchar *to, int len);
extern int	mdb_wchar2mb(const mdb_wchar *from, char *to);
extern int	mdb_wchar2mb_with_len(const mdb_wchar *from, char *to, int len);
extern int mdb_encoding_wchar2mb_with_len(int encoding,
							  const mdb_wchar *from, char *to, int len);
extern int	mdb_char_and_wchar_strcmp(const char *s1, const mdb_wchar *s2);
extern int	mdb_wchar_strncmp(const mdb_wchar *s1, const mdb_wchar *s2, size_t n);
extern int	mdb_char_and_wchar_strncmp(const char *s1, const mdb_wchar *s2, size_t n);
extern size_t mdb_wchar_strlen(const mdb_wchar *wstr);
extern int	mdb_mblen(const char *mbstr);
extern int	mdb_dsplen(const char *mbstr);
extern int	mdb_encoding_mblen(int encoding, const char *mbstr);
extern int	mdb_encoding_dsplen(int encoding, const char *mbstr);
extern int	mdb_encoding_verifymb(int encoding, const char *mbstr, int len);
extern int	mdb_mule_mblen(const unsigned char *mbstr);
extern int	mdb_mic_mblen(const unsigned char *mbstr);
extern int	mdb_mbstrlen(const char *mbstr);
extern int	mdb_mbstrlen_with_len(const char *mbstr, int len);
extern int	mdb_mbcliplen(const char *mbstr, int len, int limit);
extern int mdb_encoding_mbcliplen(int encoding, const char *mbstr,
					  int len, int limit);
extern int	mdb_mbcharcliplen(const char *mbstr, int len, int imit);
extern int	mdb_encoding_max_length(int encoding);
extern int	mdb_database_encoding_max_length(void);
extern mbcharacter_incrementer mdb_database_encoding_character_incrementer(void);

extern int	PrepareClientEncoding(int encoding);
extern int	SetClientEncoding(int encoding);
extern void InitializeClientEncoding(void);
extern int	mdb_get_client_encoding(void);
extern const char *mdb_get_client_encoding_name(void);

extern void SetDatabaseEncoding(int encoding);
extern int	GetDatabaseEncoding(void);
extern const char *GetDatabaseEncodingName(void);
extern void SetMessageEncoding(int encoding);
extern int	GetMessageEncoding(void);

#ifdef ENABLE_NLS
extern int	mdb_bind_textdomain_codeset(const char *domainname);
#endif

extern int	mdb_valid_client_encoding(const char *name);
extern int	mdb_valid_server_encoding(const char *name);

extern unsigned char *unicode_to_utf8(mdb_wchar c, unsigned char *utf8string);
extern mdb_wchar utf8_to_unicode(const unsigned char *c);
extern int	mdb_utf_mblen(const unsigned char *);
extern unsigned char *mdb_do_encoding_conversion(unsigned char *src, int len,
						  int src_encoding,
						  int dest_encoding);

extern char *mdb_client_to_server(const char *s, int len);
extern char *mdb_server_to_client(const char *s, int len);
extern char *mdb_any_to_server(const char *s, int len, int encoding);
extern char *mdb_server_to_any(const char *s, int len, int encoding);

extern unsigned short BIG5toCNS(unsigned short big5, unsigned char *lc);
extern unsigned short CNStoBIG5(unsigned short cns, unsigned char lc);

extern void UtfToLocal(const unsigned char *utf, int len,
		   unsigned char *iso,
		   const mdb_utf_to_local *map, int mapsize,
		   const mdb_utf_to_local_combined *cmap, int cmapsize,
		   utf_local_conversion_func conv_func,
		   int encoding);
extern void LocalToUtf(const unsigned char *iso, int len,
		   unsigned char *utf,
		   const mdb_local_to_utf *map, int mapsize,
		   const mdb_local_to_utf_combined *cmap, int cmapsize,
		   utf_local_conversion_func conv_func,
		   int encoding);

extern bool mdb_verifymbstr(const char *mbstr, int len, bool noError);
extern bool mdb_verify_mbstr(int encoding, const char *mbstr, int len,
				bool noError);
extern int mdb_verify_mbstr_len(int encoding, const char *mbstr, int len,
					bool noError);

extern void check_encoding_conversion_args(int src_encoding,
							   int dest_encoding,
							   int len,
							   int expected_src_encoding,
							   int expected_dest_encoding);

extern void report_invalid_encoding(int encoding, const char *mbstr, int len) mdb_attribute_noreturn();
extern void report_untranslatable_char(int src_encoding, int dest_encoding,
						 const char *mbstr, int len) mdb_attribute_noreturn();

extern void local2local(const unsigned char *l, unsigned char *p, int len,
			int src_encoding, int dest_encoding, const unsigned char *tab);
extern void mdb_ascii2mic(const unsigned char *l, unsigned char *p, int len);
extern void mdb_mic2ascii(const unsigned char *mic, unsigned char *p, int len);
extern void latin2mic(const unsigned char *l, unsigned char *p, int len,
		  int lc, int encoding);
extern void mic2latin(const unsigned char *mic, unsigned char *p, int len,
		  int lc, int encoding);
extern void latin2mic_with_table(const unsigned char *l, unsigned char *p,
					 int len, int lc, int encoding,
					 const unsigned char *tab);
extern void mic2latin_with_table(const unsigned char *mic, unsigned char *p,
					 int len, int lc, int encoding,
					 const unsigned char *tab);

extern bool mdb_utf8_islegal(const unsigned char *source, int length);

#ifdef WIN32
extern WCHAR *pgwin32_message_to_UTF16(const char *str, int len, int *utf16len);
#endif

#endif   /* MDB_WCHAR_H */
