/*-------------------------------------------------------------------------
 *
 * dict.c
 *		Standard interface to dictionary
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 *
 *
 * IDENTIFICATION
 *	  src/backend/tsearch/dict.c
 *
 *-------------------------------------------------------------------------
 */
#include "mollydb.h"

#include "catalog/mdb_type.h"
#include "tsearch/ts_cache.h"
#include "tsearch/ts_utils.h"
#include "utils/builtins.h"


/*
 * Lexize one word by dictionary, mostly debug function
 */
Datum
ts_lexize(MDB_FUNCTION_ARGS)
{
	Oid			dictId = MDB_GETARG_OID(0);
	text	   *in = MDB_GETARG_TEXT_P(1);
	ArrayType  *a;
	TSDictionaryCacheEntry *dict;
	TSLexeme   *res,
			   *ptr;
	Datum	   *da;
	DictSubState dstate = {false, false, NULL};

	dict = lookup_ts_dictionary_cache(dictId);

	res = (TSLexeme *) DatumGetPointer(FunctionCall4(&dict->lexize,
											 PointerGetDatum(dict->dictData),
												PointerGetDatum(VARDATA(in)),
									   Int32GetDatum(VARSIZE(in) - VARHDRSZ),
												  PointerGetDatum(&dstate)));

	if (dstate.getnext)
	{
		dstate.isend = true;
		ptr = (TSLexeme *) DatumGetPointer(FunctionCall4(&dict->lexize,
											 PointerGetDatum(dict->dictData),
												PointerGetDatum(VARDATA(in)),
									   Int32GetDatum(VARSIZE(in) - VARHDRSZ),
												  PointerGetDatum(&dstate)));
		if (ptr != NULL)
			res = ptr;
	}

	if (!res)
		MDB_RETURN_NULL();

	ptr = res;
	while (ptr->lexeme)
		ptr++;
	da = (Datum *) palloc(sizeof(Datum) * (ptr - res));
	ptr = res;
	while (ptr->lexeme)
	{
		da[ptr - res] = CStringGetTextDatum(ptr->lexeme);
		ptr++;
	}

	a = construct_array(da,
						ptr - res,
						TEXTOID,
						-1,
						false,
						'i');

	ptr = res;
	while (ptr->lexeme)
	{
		pfree(DatumGetPointer(da[ptr - res]));
		pfree(ptr->lexeme);
		ptr++;
	}
	pfree(res);
	pfree(da);

	MDB_RETURN_POINTER(a);
}
