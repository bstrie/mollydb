/*-------------------------------------------------------------------------
 *
 * dict_int.c
 *	  Text search dictionary for integers
 *
 * Copyright (c) 2007-2016, MollyDB Global Development Group
 *
 * IDENTIFICATION
 *	  contrib/dict_int/dict_int.c
 *
 *-------------------------------------------------------------------------
 */
#include "mollydb.h"

#include "commands/defrem.h"
#include "tsearch/ts_public.h"

MDB_MODULE_MAGIC;

typedef struct
{
	int			maxlen;
	bool		rejectlong;
} DictInt;


MDB_FUNCTION_INFO_V1(dintdict_init);
MDB_FUNCTION_INFO_V1(dintdict_lexize);

Datum
dintdict_init(MDB_FUNCTION_ARGS)
{
	List	   *dictoptions = (List *) MDB_GETARG_POINTER(0);
	DictInt    *d;
	ListCell   *l;

	d = (DictInt *) palloc0(sizeof(DictInt));
	d->maxlen = 6;
	d->rejectlong = false;

	foreach(l, dictoptions)
	{
		DefElem    *defel = (DefElem *) lfirst(l);

		if (mdb_strcasecmp(defel->defname, "MAXLEN") == 0)
		{
			d->maxlen = atoi(defGetString(defel));
		}
		else if (mdb_strcasecmp(defel->defname, "REJECTLONG") == 0)
		{
			d->rejectlong = defGetBoolean(defel);
		}
		else
		{
			ereport(ERROR,
					(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
					 errmsg("unrecognized intdict parameter: \"%s\"",
							defel->defname)));
		}
	}

	MDB_RETURN_POINTER(d);
}

Datum
dintdict_lexize(MDB_FUNCTION_ARGS)
{
	DictInt    *d = (DictInt *) MDB_GETARG_POINTER(0);
	char	   *in = (char *) MDB_GETARG_POINTER(1);
	char	   *txt = pnstrdup(in, MDB_GETARG_INT32(2));
	TSLexeme   *res = palloc0(sizeof(TSLexeme) * 2);

	res[1].lexeme = NULL;
	if (MDB_GETARG_INT32(2) > d->maxlen)
	{
		if (d->rejectlong)
		{
			/* reject by returning void array */
			pfree(txt);
			res[0].lexeme = NULL;
		}
		else
		{
			/* trim integer */
			txt[d->maxlen] = '\0';
			res[0].lexeme = txt;
		}
	}
	else
	{
		res[0].lexeme = txt;
	}

	MDB_RETURN_POINTER(res);
}
