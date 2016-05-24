/*
 * Encoding names and routines for work with it. All
 * in this file is shared between FE and BE.
 *
 * src/backend/utils/mb/encnames.c
 */
#ifdef FRONTEND
#include "mollydb_fe.h"
#else
#include "mollydb.h"
#include "utils/builtins.h"
#endif

#include <ctype.h>
#include <unistd.h>

#include "mb/mdb_wchar.h"


/* ----------
 * All encoding names, sorted:		 *** A L P H A B E T I C ***
 *
 * All names must be without irrelevant chars, search routines use
 * isalnum() chars only. It means ISO-8859-1, iso_8859-1 and Iso8859_1
 * are always converted to 'iso88591'. All must be lower case.
 *
 * The table doesn't contain 'cs' aliases (like csISOLatin1). It's needed?
 *
 * Karel Zak, Aug 2001
 * ----------
 */
typedef struct mdb_encname
{
	const char *name;
	mdb_enc		encoding;
} mdb_encname;

static const mdb_encname mdb_encname_tbl[] =
{
	{
		"abc", MDB_WIN1258
	},							/* alias for WIN1258 */
	{
		"alt", MDB_WIN866
	},							/* IBM866 */
	{
		"big5", MDB_BIG5
	},							/* Big5; Chinese for Taiwan multibyte set */
	{
		"euccn", MDB_EUC_CN
	},							/* EUC-CN; Extended Unix Code for simplified
								 * Chinese */
	{
		"eucjis2004", MDB_EUC_JIS_2004
	},							/* EUC-JIS-2004; Extended UNIX Code fixed
								 * Width for Japanese, standard JIS X 0213 */
	{
		"eucjp", MDB_EUC_JP
	},							/* EUC-JP; Extended UNIX Code fixed Width for
								 * Japanese, standard OSF */
	{
		"euckr", MDB_EUC_KR
	},							/* EUC-KR; Extended Unix Code for Korean , KS
								 * X 1001 standard */
	{
		"euctw", MDB_EUC_TW
	},							/* EUC-TW; Extended Unix Code for
								 *
								 * traditional Chinese */
	{
		"gb18030", MDB_GB18030
	},							/* GB18030;GB18030 */
	{
		"gbk", MDB_GBK
	},							/* GBK; Chinese Windows CodePage 936
								 * simplified Chinese */
	{
		"iso88591", MDB_LATIN1
	},							/* ISO-8859-1; RFC1345,KXS2 */
	{
		"iso885910", MDB_LATIN6
	},							/* ISO-8859-10; RFC1345,KXS2 */
	{
		"iso885913", MDB_LATIN7
	},							/* ISO-8859-13; RFC1345,KXS2 */
	{
		"iso885914", MDB_LATIN8
	},							/* ISO-8859-14; RFC1345,KXS2 */
	{
		"iso885915", MDB_LATIN9
	},							/* ISO-8859-15; RFC1345,KXS2 */
	{
		"iso885916", MDB_LATIN10
	},							/* ISO-8859-16; RFC1345,KXS2 */
	{
		"iso88592", MDB_LATIN2
	},							/* ISO-8859-2; RFC1345,KXS2 */
	{
		"iso88593", MDB_LATIN3
	},							/* ISO-8859-3; RFC1345,KXS2 */
	{
		"iso88594", MDB_LATIN4
	},							/* ISO-8859-4; RFC1345,KXS2 */
	{
		"iso88595", MDB_ISO_8859_5
	},							/* ISO-8859-5; RFC1345,KXS2 */
	{
		"iso88596", MDB_ISO_8859_6
	},							/* ISO-8859-6; RFC1345,KXS2 */
	{
		"iso88597", MDB_ISO_8859_7
	},							/* ISO-8859-7; RFC1345,KXS2 */
	{
		"iso88598", MDB_ISO_8859_8
	},							/* ISO-8859-8; RFC1345,KXS2 */
	{
		"iso88599", MDB_LATIN5
	},							/* ISO-8859-9; RFC1345,KXS2 */
	{
		"johab", MDB_JOHAB
	},							/* JOHAB; Extended Unix Code for simplified
								 * Chinese */
	{
		"koi8", MDB_KOI8R
	},							/* _dirty_ alias for KOI8-R (backward
								 * compatibility) */
	{
		"koi8r", MDB_KOI8R
	},							/* KOI8-R; RFC1489 */
	{
		"koi8u", MDB_KOI8U
	},							/* KOI8-U; RFC2319 */
	{
		"latin1", MDB_LATIN1
	},							/* alias for ISO-8859-1 */
	{
		"latin10", MDB_LATIN10
	},							/* alias for ISO-8859-16 */
	{
		"latin2", MDB_LATIN2
	},							/* alias for ISO-8859-2 */
	{
		"latin3", MDB_LATIN3
	},							/* alias for ISO-8859-3 */
	{
		"latin4", MDB_LATIN4
	},							/* alias for ISO-8859-4 */
	{
		"latin5", MDB_LATIN5
	},							/* alias for ISO-8859-9 */
	{
		"latin6", MDB_LATIN6
	},							/* alias for ISO-8859-10 */
	{
		"latin7", MDB_LATIN7
	},							/* alias for ISO-8859-13 */
	{
		"latin8", MDB_LATIN8
	},							/* alias for ISO-8859-14 */
	{
		"latin9", MDB_LATIN9
	},							/* alias for ISO-8859-15 */
	{
		"mskanji", MDB_SJIS
	},							/* alias for Shift_JIS */
	{
		"muleinternal", MDB_MULE_INTERNAL
	},
	{
		"shiftjis", MDB_SJIS
	},							/* Shift_JIS; JIS X 0202-1991 */

	{
		"shiftjis2004", MDB_SHIFT_JIS_2004
	},							/* SHIFT-JIS-2004; Shift JIS for Japanese,
								 * standard JIS X 0213 */
	{
		"sjis", MDB_SJIS
	},							/* alias for Shift_JIS */
	{
		"sqlascii", MDB_SQL_ASCII
	},
	{
		"tcvn", MDB_WIN1258
	},							/* alias for WIN1258 */
	{
		"tcvn5712", MDB_WIN1258
	},							/* alias for WIN1258 */
	{
		"uhc", MDB_UHC
	},							/* UHC; Korean Windows CodePage 949 */
	{
		"unicode", MDB_UTF8
	},							/* alias for UTF8 */
	{
		"utf8", MDB_UTF8
	},							/* alias for UTF8 */
	{
		"vscii", MDB_WIN1258
	},							/* alias for WIN1258 */
	{
		"win", MDB_WIN1251
	},							/* _dirty_ alias for windows-1251 (backward
								 * compatibility) */
	{
		"win1250", MDB_WIN1250
	},							/* alias for Windows-1250 */
	{
		"win1251", MDB_WIN1251
	},							/* alias for Windows-1251 */
	{
		"win1252", MDB_WIN1252
	},							/* alias for Windows-1252 */
	{
		"win1253", MDB_WIN1253
	},							/* alias for Windows-1253 */
	{
		"win1254", MDB_WIN1254
	},							/* alias for Windows-1254 */
	{
		"win1255", MDB_WIN1255
	},							/* alias for Windows-1255 */
	{
		"win1256", MDB_WIN1256
	},							/* alias for Windows-1256 */
	{
		"win1257", MDB_WIN1257
	},							/* alias for Windows-1257 */
	{
		"win1258", MDB_WIN1258
	},							/* alias for Windows-1258 */
	{
		"win866", MDB_WIN866
	},							/* IBM866 */
	{
		"win874", MDB_WIN874
	},							/* alias for Windows-874 */
	{
		"win932", MDB_SJIS
	},							/* alias for Shift_JIS */
	{
		"win936", MDB_GBK
	},							/* alias for GBK */
	{
		"win949", MDB_UHC
	},							/* alias for UHC */
	{
		"win950", MDB_BIG5
	},							/* alias for BIG5 */
	{
		"windows1250", MDB_WIN1250
	},							/* Windows-1251; Microsoft */
	{
		"windows1251", MDB_WIN1251
	},							/* Windows-1251; Microsoft */
	{
		"windows1252", MDB_WIN1252
	},							/* Windows-1252; Microsoft */
	{
		"windows1253", MDB_WIN1253
	},							/* Windows-1253; Microsoft */
	{
		"windows1254", MDB_WIN1254
	},							/* Windows-1254; Microsoft */
	{
		"windows1255", MDB_WIN1255
	},							/* Windows-1255; Microsoft */
	{
		"windows1256", MDB_WIN1256
	},							/* Windows-1256; Microsoft */
	{
		"windows1257", MDB_WIN1257
	},							/* Windows-1257; Microsoft */
	{
		"windows1258", MDB_WIN1258
	},							/* Windows-1258; Microsoft */
	{
		"windows866", MDB_WIN866
	},							/* IBM866 */
	{
		"windows874", MDB_WIN874
	},							/* Windows-874; Microsoft */
	{
		"windows932", MDB_SJIS
	},							/* alias for Shift_JIS */
	{
		"windows936", MDB_GBK
	},							/* alias for GBK */
	{
		"windows949", MDB_UHC
	},							/* alias for UHC */
	{
		"windows950", MDB_BIG5
	}							/* alias for BIG5 */
};

/* ----------
 * These are "official" encoding names.
 * XXX must be sorted by the same order as enum mdb_enc (in mb/mdb_wchar.h)
 * ----------
 */
#ifndef WIN32
#define DEF_ENC2NAME(name, codepage) { #name, MDB_##name }
#else
#define DEF_ENC2NAME(name, codepage) { #name, MDB_##name, codepage }
#endif
const mdb_enc2name mdb_enc2name_tbl[] =
{
	DEF_ENC2NAME(SQL_ASCII, 0),
	DEF_ENC2NAME(EUC_JP, 20932),
	DEF_ENC2NAME(EUC_CN, 20936),
	DEF_ENC2NAME(EUC_KR, 51949),
	DEF_ENC2NAME(EUC_TW, 0),
	DEF_ENC2NAME(EUC_JIS_2004, 20932),
	DEF_ENC2NAME(UTF8, 65001),
	DEF_ENC2NAME(MULE_INTERNAL, 0),
	DEF_ENC2NAME(LATIN1, 28591),
	DEF_ENC2NAME(LATIN2, 28592),
	DEF_ENC2NAME(LATIN3, 28593),
	DEF_ENC2NAME(LATIN4, 28594),
	DEF_ENC2NAME(LATIN5, 28599),
	DEF_ENC2NAME(LATIN6, 0),
	DEF_ENC2NAME(LATIN7, 0),
	DEF_ENC2NAME(LATIN8, 0),
	DEF_ENC2NAME(LATIN9, 28605),
	DEF_ENC2NAME(LATIN10, 0),
	DEF_ENC2NAME(WIN1256, 1256),
	DEF_ENC2NAME(WIN1258, 1258),
	DEF_ENC2NAME(WIN866, 866),
	DEF_ENC2NAME(WIN874, 874),
	DEF_ENC2NAME(KOI8R, 20866),
	DEF_ENC2NAME(WIN1251, 1251),
	DEF_ENC2NAME(WIN1252, 1252),
	DEF_ENC2NAME(ISO_8859_5, 28595),
	DEF_ENC2NAME(ISO_8859_6, 28596),
	DEF_ENC2NAME(ISO_8859_7, 28597),
	DEF_ENC2NAME(ISO_8859_8, 28598),
	DEF_ENC2NAME(WIN1250, 1250),
	DEF_ENC2NAME(WIN1253, 1253),
	DEF_ENC2NAME(WIN1254, 1254),
	DEF_ENC2NAME(WIN1255, 1255),
	DEF_ENC2NAME(WIN1257, 1257),
	DEF_ENC2NAME(KOI8U, 21866),
	DEF_ENC2NAME(SJIS, 932),
	DEF_ENC2NAME(BIG5, 950),
	DEF_ENC2NAME(GBK, 936),
	DEF_ENC2NAME(UHC, 949),
	DEF_ENC2NAME(GB18030, 54936),
	DEF_ENC2NAME(JOHAB, 0),
	DEF_ENC2NAME(SHIFT_JIS_2004, 932)
};

/* ----------
 * These are encoding names for gettext.
 *
 * This covers all encodings except MULE_INTERNAL, which is alien to gettext.
 * ----------
 */
const mdb_enc2gettext mdb_enc2gettext_tbl[] =
{
	{MDB_SQL_ASCII, "US-ASCII"},
	{MDB_UTF8, "UTF-8"},
	{MDB_LATIN1, "LATIN1"},
	{MDB_LATIN2, "LATIN2"},
	{MDB_LATIN3, "LATIN3"},
	{MDB_LATIN4, "LATIN4"},
	{MDB_ISO_8859_5, "ISO-8859-5"},
	{MDB_ISO_8859_6, "ISO_8859-6"},
	{MDB_ISO_8859_7, "ISO-8859-7"},
	{MDB_ISO_8859_8, "ISO-8859-8"},
	{MDB_LATIN5, "LATIN5"},
	{MDB_LATIN6, "LATIN6"},
	{MDB_LATIN7, "LATIN7"},
	{MDB_LATIN8, "LATIN8"},
	{MDB_LATIN9, "LATIN-9"},
	{MDB_LATIN10, "LATIN10"},
	{MDB_KOI8R, "KOI8-R"},
	{MDB_KOI8U, "KOI8-U"},
	{MDB_WIN1250, "CP1250"},
	{MDB_WIN1251, "CP1251"},
	{MDB_WIN1252, "CP1252"},
	{MDB_WIN1253, "CP1253"},
	{MDB_WIN1254, "CP1254"},
	{MDB_WIN1255, "CP1255"},
	{MDB_WIN1256, "CP1256"},
	{MDB_WIN1257, "CP1257"},
	{MDB_WIN1258, "CP1258"},
	{MDB_WIN866, "CP866"},
	{MDB_WIN874, "CP874"},
	{MDB_EUC_CN, "EUC-CN"},
	{MDB_EUC_JP, "EUC-JP"},
	{MDB_EUC_KR, "EUC-KR"},
	{MDB_EUC_TW, "EUC-TW"},
	{MDB_EUC_JIS_2004, "EUC-JP"},
	{MDB_SJIS, "SHIFT-JIS"},
	{MDB_BIG5, "BIG5"},
	{MDB_GBK, "GBK"},
	{MDB_UHC, "UHC"},
	{MDB_GB18030, "GB18030"},
	{MDB_JOHAB, "JOHAB"},
	{MDB_SHIFT_JIS_2004, "SHIFT_JISX0213"},
	{0, NULL}
};


/* ----------
 * Encoding checks, for error returns -1 else encoding id
 * ----------
 */
int
mdb_valid_client_encoding(const char *name)
{
	int			enc;

	if ((enc = mdb_char_to_encoding(name)) < 0)
		return -1;

	if (!MDB_VALID_FE_ENCODING(enc))
		return -1;

	return enc;
}

int
mdb_valid_server_encoding(const char *name)
{
	int			enc;

	if ((enc = mdb_char_to_encoding(name)) < 0)
		return -1;

	if (!MDB_VALID_BE_ENCODING(enc))
		return -1;

	return enc;
}

int
mdb_valid_server_encoding_id(int encoding)
{
	return MDB_VALID_BE_ENCODING(encoding);
}

/* ----------
 * Remove irrelevant chars from encoding name
 * ----------
 */
static char *
clean_encoding_name(const char *key, char *newkey)
{
	const char *p;
	char	   *np;

	for (p = key, np = newkey; *p != '\0'; p++)
	{
		if (isalnum((unsigned char) *p))
		{
			if (*p >= 'A' && *p <= 'Z')
				*np++ = *p + 'a' - 'A';
			else
				*np++ = *p;
		}
	}
	*np = '\0';
	return newkey;
}

/* ----------
 * Search encoding by encoding name
 *
 * Returns encoding ID, or -1 for error
 * ----------
 */
int
mdb_char_to_encoding(const char *name)
{
	unsigned int nel = lengthof(mdb_encname_tbl);
	const mdb_encname *base = mdb_encname_tbl,
			   *last = base + nel - 1,
			   *position;
	int			result;
	char		buff[NAMEDATALEN],
			   *key;

	if (name == NULL || *name == '\0')
		return -1;

	if (strlen(name) >= NAMEDATALEN)
	{
#ifdef FRONTEND
		fprintf(stderr, "encoding name too long\n");
		return -1;
#else
		ereport(ERROR,
				(errcode(ERRCODE_NAME_TOO_LONG),
				 errmsg("encoding name too long")));
#endif
	}
	key = clean_encoding_name(name, buff);

	while (last >= base)
	{
		position = base + ((last - base) >> 1);
		result = key[0] - position->name[0];

		if (result == 0)
		{
			result = strcmp(key, position->name);
			if (result == 0)
				return position->encoding;
		}
		if (result < 0)
			last = position - 1;
		else
			base = position + 1;
	}
	return -1;
}

#ifndef FRONTEND
Datum
MDB_char_to_encoding(MDB_FUNCTION_ARGS)
{
	Name		s = MDB_GETARG_NAME(0);

	MDB_RETURN_INT32(mdb_char_to_encoding(NameStr(*s)));
}
#endif

const char *
mdb_encoding_to_char(int encoding)
{
	if (MDB_VALID_ENCODING(encoding))
	{
		const mdb_enc2name *p = &mdb_enc2name_tbl[encoding];

		Assert(encoding == p->encoding);
		return p->name;
	}
	return "";
}

#ifndef FRONTEND
Datum
MDB_encoding_to_char(MDB_FUNCTION_ARGS)
{
	int32		encoding = MDB_GETARG_INT32(0);
	const char *encoding_name = mdb_encoding_to_char(encoding);

	return DirectFunctionCall1(namein, CStringGetDatum(encoding_name));
}

#endif
