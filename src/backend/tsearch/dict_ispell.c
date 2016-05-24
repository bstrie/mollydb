/*-------------------------------------------------------------------------
 *
 * dict_ispell.c
 *		Ispell dictionary interface
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 *
 *
 * IDENTIFICATION
 *	  src/backend/tsearch/dict_ispell.c
 *
 *-------------------------------------------------------------------------
 */
#include "mollydb.h"

#include "commands/defrem.h"
#include "tsearch/dicts/spell.h"
#include "tsearch/ts_locale.h"
#include "tsearch/ts_utils.h"


typedef struct
{
	StopList	stoplist;
	IspellDict	obj;
} DictISpell;

Datum
dispell_init(MDB_FUNCTION_ARGS)
{
	List	   *dictoptions = (List *) MDB_GETARG_POINTER(0);
	DictISpell *d;
	bool		affloaded = false,
				dictloaded = false,
				stoploaded = false;
	ListCell   *l;

	d = (DictISpell *) palloc0(sizeof(DictISpell));

	NIStartBuild(&(d->obj));

	foreach(l, dictoptions)
	{
		DefElem    *defel = (DefElem *) lfirst(l);

		if (mdb_strcasecmp(defel->defname, "DictFile") == 0)
		{
			if (dictloaded)
				ereport(ERROR,
						(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
						 errmsg("multiple DictFile parameters")));
			NIImportDictionary(&(d->obj),
							 get_tsearch_config_filename(defGetString(defel),
														 "dict"));
			dictloaded = true;
		}
		else if (mdb_strcasecmp(defel->defname, "AffFile") == 0)
		{
			if (affloaded)
				ereport(ERROR,
						(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
						 errmsg("multiple AffFile parameters")));
			NIImportAffixes(&(d->obj),
							get_tsearch_config_filename(defGetString(defel),
														"affix"));
			affloaded = true;
		}
		else if (mdb_strcasecmp(defel->defname, "StopWords") == 0)
		{
			if (stoploaded)
				ereport(ERROR,
						(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
						 errmsg("multiple StopWords parameters")));
			readstoplist(defGetString(defel), &(d->stoplist), lowerstr);
			stoploaded = true;
		}
		else
		{
			ereport(ERROR,
					(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
					 errmsg("unrecognized Ispell parameter: \"%s\"",
							defel->defname)));
		}
	}

	if (affloaded && dictloaded)
	{
		NISortDictionary(&(d->obj));
		NISortAffixes(&(d->obj));
	}
	else if (!affloaded)
	{
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				 errmsg("missing AffFile parameter")));
	}
	else
	{
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				 errmsg("missing DictFile parameter")));
	}

	NIFinishBuild(&(d->obj));

	MDB_RETURN_POINTER(d);
}

Datum
dispell_lexize(MDB_FUNCTION_ARGS)
{
	DictISpell *d = (DictISpell *) MDB_GETARG_POINTER(0);
	char	   *in = (char *) MDB_GETARG_POINTER(1);
	int32		len = MDB_GETARG_INT32(2);
	char	   *txt;
	TSLexeme   *res;
	TSLexeme   *ptr,
			   *cptr;

	if (len <= 0)
		MDB_RETURN_POINTER(NULL);

	txt = lowerstr_with_len(in, len);
	res = NINormalizeWord(&(d->obj), txt);

	if (res == NULL)
		MDB_RETURN_POINTER(NULL);

	cptr = res;
	for (ptr = cptr; ptr->lexeme; ptr++)
	{
		if (searchstoplist(&(d->stoplist), ptr->lexeme))
		{
			pfree(ptr->lexeme);
			ptr->lexeme = NULL;
		}
		else
		{
			if (cptr != ptr)
				memcpy(cptr, ptr, sizeof(TSLexeme));
			cptr++;
		}
	}
	cptr->lexeme = NULL;

	MDB_RETURN_POINTER(res);
}
