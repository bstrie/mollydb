/*-------------------------------------------------------------------------
 *
 * dict_snowball.c
 *		Snowball dictionary
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 *
 * IDENTIFICATION
 *	  src/backend/snowball/dict_snowball.c
 *
 *-------------------------------------------------------------------------
 */
#include "mollydb.h"

#include "commands/defrem.h"
#include "tsearch/ts_locale.h"
#include "tsearch/ts_utils.h"

/* Some platforms define MAXINT and/or MININT, causing conflicts */
#ifdef MAXINT
#undef MAXINT
#endif
#ifdef MININT
#undef MININT
#endif

/* Now we can include the original Snowball header.h */
#include "snowball/libstemmer/header.h"
#include "snowball/libstemmer/stem_ISO_8859_1_danish.h"
#include "snowball/libstemmer/stem_ISO_8859_1_dutch.h"
#include "snowball/libstemmer/stem_ISO_8859_1_english.h"
#include "snowball/libstemmer/stem_ISO_8859_1_finnish.h"
#include "snowball/libstemmer/stem_ISO_8859_1_french.h"
#include "snowball/libstemmer/stem_ISO_8859_1_german.h"
#include "snowball/libstemmer/stem_ISO_8859_1_hungarian.h"
#include "snowball/libstemmer/stem_ISO_8859_1_italian.h"
#include "snowball/libstemmer/stem_ISO_8859_1_norwegian.h"
#include "snowball/libstemmer/stem_ISO_8859_1_porter.h"
#include "snowball/libstemmer/stem_ISO_8859_1_portuguese.h"
#include "snowball/libstemmer/stem_ISO_8859_1_spanish.h"
#include "snowball/libstemmer/stem_ISO_8859_1_swedish.h"
#include "snowball/libstemmer/stem_ISO_8859_2_romanian.h"
#include "snowball/libstemmer/stem_KOI8_R_russian.h"
#include "snowball/libstemmer/stem_UTF_8_danish.h"
#include "snowball/libstemmer/stem_UTF_8_dutch.h"
#include "snowball/libstemmer/stem_UTF_8_english.h"
#include "snowball/libstemmer/stem_UTF_8_finnish.h"
#include "snowball/libstemmer/stem_UTF_8_french.h"
#include "snowball/libstemmer/stem_UTF_8_german.h"
#include "snowball/libstemmer/stem_UTF_8_hungarian.h"
#include "snowball/libstemmer/stem_UTF_8_italian.h"
#include "snowball/libstemmer/stem_UTF_8_norwegian.h"
#include "snowball/libstemmer/stem_UTF_8_porter.h"
#include "snowball/libstemmer/stem_UTF_8_portuguese.h"
#include "snowball/libstemmer/stem_UTF_8_romanian.h"
#include "snowball/libstemmer/stem_UTF_8_russian.h"
#include "snowball/libstemmer/stem_UTF_8_spanish.h"
#include "snowball/libstemmer/stem_UTF_8_swedish.h"
#include "snowball/libstemmer/stem_UTF_8_turkish.h"

MDB_MODULE_MAGIC;

MDB_FUNCTION_INFO_V1(dsnowball_init);

MDB_FUNCTION_INFO_V1(dsnowball_lexize);

/* List of supported modules */
typedef struct stemmer_module
{
	const char *name;
	mdb_enc		enc;
	struct SN_env *(*create) (void);
	void		(*close) (struct SN_env *);
	int			(*stem) (struct SN_env *);
} stemmer_module;

static const stemmer_module stemmer_modules[] =
{
	/*
	 * Stemmers list from Snowball distribution
	 */
	{"danish", MDB_LATIN1, danish_ISO_8859_1_create_env, danish_ISO_8859_1_close_env, danish_ISO_8859_1_stem},
	{"dutch", MDB_LATIN1, dutch_ISO_8859_1_create_env, dutch_ISO_8859_1_close_env, dutch_ISO_8859_1_stem},
	{"english", MDB_LATIN1, english_ISO_8859_1_create_env, english_ISO_8859_1_close_env, english_ISO_8859_1_stem},
	{"finnish", MDB_LATIN1, finnish_ISO_8859_1_create_env, finnish_ISO_8859_1_close_env, finnish_ISO_8859_1_stem},
	{"french", MDB_LATIN1, french_ISO_8859_1_create_env, french_ISO_8859_1_close_env, french_ISO_8859_1_stem},
	{"german", MDB_LATIN1, german_ISO_8859_1_create_env, german_ISO_8859_1_close_env, german_ISO_8859_1_stem},
	{"hungarian", MDB_LATIN1, hungarian_ISO_8859_1_create_env, hungarian_ISO_8859_1_close_env, hungarian_ISO_8859_1_stem},
	{"italian", MDB_LATIN1, italian_ISO_8859_1_create_env, italian_ISO_8859_1_close_env, italian_ISO_8859_1_stem},
	{"norwegian", MDB_LATIN1, norwegian_ISO_8859_1_create_env, norwegian_ISO_8859_1_close_env, norwegian_ISO_8859_1_stem},
	{"porter", MDB_LATIN1, porter_ISO_8859_1_create_env, porter_ISO_8859_1_close_env, porter_ISO_8859_1_stem},
	{"portuguese", MDB_LATIN1, portuguese_ISO_8859_1_create_env, portuguese_ISO_8859_1_close_env, portuguese_ISO_8859_1_stem},
	{"spanish", MDB_LATIN1, spanish_ISO_8859_1_create_env, spanish_ISO_8859_1_close_env, spanish_ISO_8859_1_stem},
	{"swedish", MDB_LATIN1, swedish_ISO_8859_1_create_env, swedish_ISO_8859_1_close_env, swedish_ISO_8859_1_stem},
	{"romanian", MDB_LATIN2, romanian_ISO_8859_2_create_env, romanian_ISO_8859_2_close_env, romanian_ISO_8859_2_stem},
	{"russian", MDB_KOI8R, russian_KOI8_R_create_env, russian_KOI8_R_close_env, russian_KOI8_R_stem},
	{"danish", MDB_UTF8, danish_UTF_8_create_env, danish_UTF_8_close_env, danish_UTF_8_stem},
	{"dutch", MDB_UTF8, dutch_UTF_8_create_env, dutch_UTF_8_close_env, dutch_UTF_8_stem},
	{"english", MDB_UTF8, english_UTF_8_create_env, english_UTF_8_close_env, english_UTF_8_stem},
	{"finnish", MDB_UTF8, finnish_UTF_8_create_env, finnish_UTF_8_close_env, finnish_UTF_8_stem},
	{"french", MDB_UTF8, french_UTF_8_create_env, french_UTF_8_close_env, french_UTF_8_stem},
	{"german", MDB_UTF8, german_UTF_8_create_env, german_UTF_8_close_env, german_UTF_8_stem},
	{"hungarian", MDB_UTF8, hungarian_UTF_8_create_env, hungarian_UTF_8_close_env, hungarian_UTF_8_stem},
	{"italian", MDB_UTF8, italian_UTF_8_create_env, italian_UTF_8_close_env, italian_UTF_8_stem},
	{"norwegian", MDB_UTF8, norwegian_UTF_8_create_env, norwegian_UTF_8_close_env, norwegian_UTF_8_stem},
	{"porter", MDB_UTF8, porter_UTF_8_create_env, porter_UTF_8_close_env, porter_UTF_8_stem},
	{"portuguese", MDB_UTF8, portuguese_UTF_8_create_env, portuguese_UTF_8_close_env, portuguese_UTF_8_stem},
	{"romanian", MDB_UTF8, romanian_UTF_8_create_env, romanian_UTF_8_close_env, romanian_UTF_8_stem},
	{"russian", MDB_UTF8, russian_UTF_8_create_env, russian_UTF_8_close_env, russian_UTF_8_stem},
	{"spanish", MDB_UTF8, spanish_UTF_8_create_env, spanish_UTF_8_close_env, spanish_UTF_8_stem},
	{"swedish", MDB_UTF8, swedish_UTF_8_create_env, swedish_UTF_8_close_env, swedish_UTF_8_stem},
	{"turkish", MDB_UTF8, turkish_UTF_8_create_env, turkish_UTF_8_close_env, turkish_UTF_8_stem},

	/*
	 * Stemmer with MDB_SQL_ASCII encoding should be valid for any server
	 * encoding
	 */
	{"english", MDB_SQL_ASCII, english_ISO_8859_1_create_env, english_ISO_8859_1_close_env, english_ISO_8859_1_stem},

	{NULL, 0, NULL, NULL, NULL} /* list end marker */
};


typedef struct DictSnowball
{
	struct SN_env *z;
	StopList	stoplist;
	bool		needrecode;		/* needs recoding before/after call stem */
	int			(*stem) (struct SN_env * z);

	/*
	 * snowball saves alloced memory between calls, so we should run it in our
	 * private memory context. Note, init function is executed in long lived
	 * context, so we just remember CurrentMemoryContext
	 */
	MemoryContext dictCtx;
} DictSnowball;


static void
locate_stem_module(DictSnowball *d, char *lang)
{
	const stemmer_module *m;

	/*
	 * First, try to find exact match of stemmer module. Stemmer with
	 * MDB_SQL_ASCII encoding is treated as working with any server encoding
	 */
	for (m = stemmer_modules; m->name; m++)
	{
		if ((m->enc == MDB_SQL_ASCII || m->enc == GetDatabaseEncoding()) &&
			mdb_strcasecmp(m->name, lang) == 0)
		{
			d->stem = m->stem;
			d->z = m->create();
			d->needrecode = false;
			return;
		}
	}

	/*
	 * Second, try to find stemmer for needed language for UTF8 encoding.
	 */
	for (m = stemmer_modules; m->name; m++)
	{
		if (m->enc == MDB_UTF8 && mdb_strcasecmp(m->name, lang) == 0)
		{
			d->stem = m->stem;
			d->z = m->create();
			d->needrecode = true;
			return;
		}
	}

	ereport(ERROR,
			(errcode(ERRCODE_UNDEFINED_OBJECT),
			 errmsg("no Snowball stemmer available for language \"%s\" and encoding \"%s\"",
					lang, GetDatabaseEncodingName())));
}

Datum
dsnowball_init(MDB_FUNCTION_ARGS)
{
	List	   *dictoptions = (List *) MDB_GETARG_POINTER(0);
	DictSnowball *d;
	bool		stoploaded = false;
	ListCell   *l;

	d = (DictSnowball *) palloc0(sizeof(DictSnowball));

	foreach(l, dictoptions)
	{
		DefElem    *defel = (DefElem *) lfirst(l);

		if (mdb_strcasecmp("StopWords", defel->defname) == 0)
		{
			if (stoploaded)
				ereport(ERROR,
						(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
						 errmsg("multiple StopWords parameters")));
			readstoplist(defGetString(defel), &d->stoplist, lowerstr);
			stoploaded = true;
		}
		else if (mdb_strcasecmp("Language", defel->defname) == 0)
		{
			if (d->stem)
				ereport(ERROR,
						(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
						 errmsg("multiple Language parameters")));
			locate_stem_module(d, defGetString(defel));
		}
		else
		{
			ereport(ERROR,
					(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
					 errmsg("unrecognized Snowball parameter: \"%s\"",
							defel->defname)));
		}
	}

	if (!d->stem)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				 errmsg("missing Language parameter")));

	d->dictCtx = CurrentMemoryContext;

	MDB_RETURN_POINTER(d);
}

Datum
dsnowball_lexize(MDB_FUNCTION_ARGS)
{
	DictSnowball *d = (DictSnowball *) MDB_GETARG_POINTER(0);
	char	   *in = (char *) MDB_GETARG_POINTER(1);
	int32		len = MDB_GETARG_INT32(2);
	char	   *txt = lowerstr_with_len(in, len);
	TSLexeme   *res = palloc0(sizeof(TSLexeme) * 2);

	if (*txt == '\0' || searchstoplist(&(d->stoplist), txt))
	{
		pfree(txt);
	}
	else
	{
		MemoryContext saveCtx;

		/*
		 * recode to utf8 if stemmer is utf8 and doesn't match server encoding
		 */
		if (d->needrecode)
		{
			char	   *recoded;

			recoded = mdb_server_to_any(txt, strlen(txt), MDB_UTF8);
			if (recoded != txt)
			{
				pfree(txt);
				txt = recoded;
			}
		}

		/* see comment about d->dictCtx */
		saveCtx = MemoryContextSwitchTo(d->dictCtx);
		SN_set_current(d->z, strlen(txt), (symbol *) txt);
		d->stem(d->z);
		MemoryContextSwitchTo(saveCtx);

		if (d->z->p && d->z->l)
		{
			txt = repalloc(txt, d->z->l + 1);
			memcpy(txt, d->z->p, d->z->l);
			txt[d->z->l] = '\0';
		}

		/* back recode if needed */
		if (d->needrecode)
		{
			char	   *recoded;

			recoded = mdb_any_to_server(txt, strlen(txt), MDB_UTF8);
			if (recoded != txt)
			{
				pfree(txt);
				txt = recoded;
			}
		}

		res->lexeme = txt;
	}

	MDB_RETURN_POINTER(res);
}
