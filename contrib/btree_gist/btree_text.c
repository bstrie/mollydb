/*
 * contrib/btree_gist/btree_text.c
 */
#include "mollydb.h"

#include "btree_gist.h"
#include "btree_utils_var.h"
#include "utils/builtins.h"

/*
** Text ops
*/
MDB_FUNCTION_INFO_V1(gbt_text_compress);
MDB_FUNCTION_INFO_V1(gbt_bpchar_compress);
MDB_FUNCTION_INFO_V1(gbt_text_union);
MDB_FUNCTION_INFO_V1(gbt_text_picksplit);
MDB_FUNCTION_INFO_V1(gbt_text_consistent);
MDB_FUNCTION_INFO_V1(gbt_bpchar_consistent);
MDB_FUNCTION_INFO_V1(gbt_text_penalty);
MDB_FUNCTION_INFO_V1(gbt_text_same);


/* define for comparison */

static bool
gbt_textgt(const void *a, const void *b, Oid collation)
{
	return DatumGetBool(DirectFunctionCall2Coll(text_gt,
												collation,
												PointerGetDatum(a),
												PointerGetDatum(b)));
}

static bool
gbt_textge(const void *a, const void *b, Oid collation)
{
	return DatumGetBool(DirectFunctionCall2Coll(text_ge,
												collation,
												PointerGetDatum(a),
												PointerGetDatum(b)));
}

static bool
gbt_texteq(const void *a, const void *b, Oid collation)
{
	return DatumGetBool(DirectFunctionCall2Coll(texteq,
												collation,
												PointerGetDatum(a),
												PointerGetDatum(b)));
}

static bool
gbt_textle(const void *a, const void *b, Oid collation)
{
	return DatumGetBool(DirectFunctionCall2Coll(text_le,
												collation,
												PointerGetDatum(a),
												PointerGetDatum(b)));
}

static bool
gbt_textlt(const void *a, const void *b, Oid collation)
{
	return DatumGetBool(DirectFunctionCall2Coll(text_lt,
												collation,
												PointerGetDatum(a),
												PointerGetDatum(b)));
}

static int32
gbt_textcmp(const void *a, const void *b, Oid collation)
{
	return DatumGetInt32(DirectFunctionCall2Coll(bttextcmp,
												 collation,
												 PointerGetDatum(a),
												 PointerGetDatum(b)));
}

static gbtree_vinfo tinfo =
{
	gbt_t_text,
	0,
	FALSE,
	gbt_textgt,
	gbt_textge,
	gbt_texteq,
	gbt_textle,
	gbt_textlt,
	gbt_textcmp,
	NULL
};


/**************************************************
 * Text ops
 **************************************************/


Datum
gbt_text_compress(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);

	if (tinfo.eml == 0)
	{
		tinfo.eml = mdb_database_encoding_max_length();
	}

	MDB_RETURN_POINTER(gbt_var_compress(entry, &tinfo));
}

Datum
gbt_bpchar_compress(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);
	GISTENTRY  *retval;

	if (tinfo.eml == 0)
	{
		tinfo.eml = mdb_database_encoding_max_length();
	}

	if (entry->leafkey)
	{

		Datum		d = DirectFunctionCall1(rtrim1, entry->key);
		GISTENTRY	trim;

		gistentryinit(trim, d,
					  entry->rel, entry->page,
					  entry->offset, TRUE);
		retval = gbt_var_compress(&trim, &tinfo);
	}
	else
		retval = entry;

	MDB_RETURN_POINTER(retval);
}



Datum
gbt_text_consistent(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);
	void	   *query = (void *) DatumGetTextP(MDB_GETARG_DATUM(1));
	StrategyNumber strategy = (StrategyNumber) MDB_GETARG_UINT16(2);

	/* Oid		subtype = MDB_GETARG_OID(3); */
	bool	   *recheck = (bool *) MDB_GETARG_POINTER(4);
	bool		retval;
	GBT_VARKEY *key = (GBT_VARKEY *) DatumGetPointer(entry->key);
	GBT_VARKEY_R r = gbt_var_key_readable(key);

	/* All cases served by this function are exact */
	*recheck = false;

	if (tinfo.eml == 0)
	{
		tinfo.eml = mdb_database_encoding_max_length();
	}

	retval = gbt_var_consistent(&r, query, strategy, MDB_GET_COLLATION(),
								GIST_LEAF(entry), &tinfo);

	MDB_RETURN_BOOL(retval);
}


Datum
gbt_bpchar_consistent(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);
	void	   *query = (void *) DatumGetPointer(MDB_DETOAST_DATUM(MDB_GETARG_DATUM(1)));
	StrategyNumber strategy = (StrategyNumber) MDB_GETARG_UINT16(2);

	/* Oid		subtype = MDB_GETARG_OID(3); */
	bool	   *recheck = (bool *) MDB_GETARG_POINTER(4);
	bool		retval;
	GBT_VARKEY *key = (GBT_VARKEY *) DatumGetPointer(entry->key);
	GBT_VARKEY_R r = gbt_var_key_readable(key);
	void	   *trim = (void *) DatumGetPointer(DirectFunctionCall1(rtrim1, PointerGetDatum(query)));

	/* All cases served by this function are exact */
	*recheck = false;

	if (tinfo.eml == 0)
	{
		tinfo.eml = mdb_database_encoding_max_length();
	}

	retval = gbt_var_consistent(&r, trim, strategy, MDB_GET_COLLATION(),
								GIST_LEAF(entry), &tinfo);
	MDB_RETURN_BOOL(retval);
}


Datum
gbt_text_union(MDB_FUNCTION_ARGS)
{
	GistEntryVector *entryvec = (GistEntryVector *) MDB_GETARG_POINTER(0);
	int32	   *size = (int *) MDB_GETARG_POINTER(1);

	MDB_RETURN_POINTER(gbt_var_union(entryvec, size, MDB_GET_COLLATION(),
									&tinfo));
}


Datum
gbt_text_picksplit(MDB_FUNCTION_ARGS)
{
	GistEntryVector *entryvec = (GistEntryVector *) MDB_GETARG_POINTER(0);
	GIST_SPLITVEC *v = (GIST_SPLITVEC *) MDB_GETARG_POINTER(1);

	gbt_var_picksplit(entryvec, v, MDB_GET_COLLATION(),
					  &tinfo);
	MDB_RETURN_POINTER(v);
}

Datum
gbt_text_same(MDB_FUNCTION_ARGS)
{
	Datum		d1 = MDB_GETARG_DATUM(0);
	Datum		d2 = MDB_GETARG_DATUM(1);
	bool	   *result = (bool *) MDB_GETARG_POINTER(2);

	*result = gbt_var_same(d1, d2, MDB_GET_COLLATION(), &tinfo);
	MDB_RETURN_POINTER(result);
}


Datum
gbt_text_penalty(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *o = (GISTENTRY *) MDB_GETARG_POINTER(0);
	GISTENTRY  *n = (GISTENTRY *) MDB_GETARG_POINTER(1);
	float	   *result = (float *) MDB_GETARG_POINTER(2);

	MDB_RETURN_POINTER(gbt_var_penalty(result, o, n, MDB_GET_COLLATION(),
									  &tinfo));
}
