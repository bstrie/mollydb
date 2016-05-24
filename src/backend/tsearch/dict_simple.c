/*-------------------------------------------------------------------------
 *
 * dict_simple.c
 *		Simple dictionary: just lowercase and check for stopword
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 *
 *
 * IDENTIFICATION
 *	  src/backend/tsearch/dict_simple.c
 *
 *-------------------------------------------------------------------------
 */
#include "mollydb.h"

#include "commands/defrem.h"
#include "tsearch/ts_locale.h"
#include "tsearch/ts_utils.h"


typedef struct
{
	StopList	stoplist;
	bool		accept;
} DictSimple;


Datum
dsimple_init(MDB_FUNCTION_ARGS)
{
	List	   *dictoptions = (List *) MDB_GETARG_POINTER(0);
	DictSimple *d = (DictSimple *) palloc0(sizeof(DictSimple));
	bool		stoploaded = false,
				acceptloaded = false;
	ListCell   *l;

	d->accept = true;			/* default */

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
		else if (mdb_strcasecmp("Accept", defel->defname) == 0)
		{
			if (acceptloaded)
				ereport(ERROR,
						(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
						 errmsg("multiple Accept parameters")));
			d->accept = defGetBoolean(defel);
			acceptloaded = true;
		}
		else
		{
			ereport(ERROR,
					(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				   errmsg("unrecognized simple dictionary parameter: \"%s\"",
						  defel->defname)));
		}
	}

	MDB_RETURN_POINTER(d);
}

Datum
dsimple_lexize(MDB_FUNCTION_ARGS)
{
	DictSimple *d = (DictSimple *) MDB_GETARG_POINTER(0);
	char	   *in = (char *) MDB_GETARG_POINTER(1);
	int32		len = MDB_GETARG_INT32(2);
	char	   *txt;
	TSLexeme   *res;

	txt = lowerstr_with_len(in, len);

	if (*txt == '\0' || searchstoplist(&(d->stoplist), txt))
	{
		/* reject as stopword */
		pfree(txt);
		res = palloc0(sizeof(TSLexeme) * 2);
		MDB_RETURN_POINTER(res);
	}
	else if (d->accept)
	{
		/* accept */
		res = palloc0(sizeof(TSLexeme) * 2);
		res[0].lexeme = txt;
		MDB_RETURN_POINTER(res);
	}
	else
	{
		/* report as unrecognized */
		pfree(txt);
		MDB_RETURN_POINTER(NULL);
	}
}
