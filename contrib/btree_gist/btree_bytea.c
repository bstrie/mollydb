/*
 * contrib/btree_gist/btree_bytea.c
 */
#include "mollydb.h"

#include "btree_gist.h"
#include "btree_utils_var.h"
#include "utils/bytea.h"


/*
** Bytea ops
*/
MDB_FUNCTION_INFO_V1(gbt_bytea_compress);
MDB_FUNCTION_INFO_V1(gbt_bytea_union);
MDB_FUNCTION_INFO_V1(gbt_bytea_picksplit);
MDB_FUNCTION_INFO_V1(gbt_bytea_consistent);
MDB_FUNCTION_INFO_V1(gbt_bytea_penalty);
MDB_FUNCTION_INFO_V1(gbt_bytea_same);


/* define for comparison */

static bool
gbt_byteagt(const void *a, const void *b, Oid collation)
{
	return DatumGetBool(DirectFunctionCall2(byteagt,
											PointerGetDatum(a),
											PointerGetDatum(b)));
}

static bool
gbt_byteage(const void *a, const void *b, Oid collation)
{
	return DatumGetBool(DirectFunctionCall2(byteage,
											PointerGetDatum(a),
											PointerGetDatum(b)));
}

static bool
gbt_byteaeq(const void *a, const void *b, Oid collation)
{
	return DatumGetBool(DirectFunctionCall2(byteaeq,
											PointerGetDatum(a),
											PointerGetDatum(b)));
}

static bool
gbt_byteale(const void *a, const void *b, Oid collation)
{
	return DatumGetBool(DirectFunctionCall2(byteale,
											PointerGetDatum(a),
											PointerGetDatum(b)));
}

static bool
gbt_bytealt(const void *a, const void *b, Oid collation)
{
	return DatumGetBool(DirectFunctionCall2(bytealt,
											PointerGetDatum(a),
											PointerGetDatum(b)));
}

static int32
gbt_byteacmp(const void *a, const void *b, Oid collation)
{
	return DatumGetInt32(DirectFunctionCall2(byteacmp,
											 PointerGetDatum(a),
											 PointerGetDatum(b)));
}


static const gbtree_vinfo tinfo =
{
	gbt_t_bytea,
	0,
	TRUE,
	gbt_byteagt,
	gbt_byteage,
	gbt_byteaeq,
	gbt_byteale,
	gbt_bytealt,
	gbt_byteacmp,
	NULL
};


/**************************************************
 * Text ops
 **************************************************/


Datum
gbt_bytea_compress(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);

	MDB_RETURN_POINTER(gbt_var_compress(entry, &tinfo));
}



Datum
gbt_bytea_consistent(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);
	void	   *query = (void *) DatumGetByteaP(MDB_GETARG_DATUM(1));
	StrategyNumber strategy = (StrategyNumber) MDB_GETARG_UINT16(2);

	/* Oid		subtype = MDB_GETARG_OID(3); */
	bool	   *recheck = (bool *) MDB_GETARG_POINTER(4);
	bool		retval;
	GBT_VARKEY *key = (GBT_VARKEY *) DatumGetPointer(entry->key);
	GBT_VARKEY_R r = gbt_var_key_readable(key);

	/* All cases served by this function are exact */
	*recheck = false;

	retval = gbt_var_consistent(&r, query, strategy, MDB_GET_COLLATION(),
								GIST_LEAF(entry), &tinfo);
	MDB_RETURN_BOOL(retval);
}



Datum
gbt_bytea_union(MDB_FUNCTION_ARGS)
{
	GistEntryVector *entryvec = (GistEntryVector *) MDB_GETARG_POINTER(0);
	int32	   *size = (int *) MDB_GETARG_POINTER(1);

	MDB_RETURN_POINTER(gbt_var_union(entryvec, size, MDB_GET_COLLATION(),
									&tinfo));
}


Datum
gbt_bytea_picksplit(MDB_FUNCTION_ARGS)
{
	GistEntryVector *entryvec = (GistEntryVector *) MDB_GETARG_POINTER(0);
	GIST_SPLITVEC *v = (GIST_SPLITVEC *) MDB_GETARG_POINTER(1);

	gbt_var_picksplit(entryvec, v, MDB_GET_COLLATION(),
					  &tinfo);
	MDB_RETURN_POINTER(v);
}

Datum
gbt_bytea_same(MDB_FUNCTION_ARGS)
{
	Datum		d1 = MDB_GETARG_DATUM(0);
	Datum		d2 = MDB_GETARG_DATUM(1);
	bool	   *result = (bool *) MDB_GETARG_POINTER(2);

	*result = gbt_var_same(d1, d2, MDB_GET_COLLATION(), &tinfo);
	MDB_RETURN_POINTER(result);
}


Datum
gbt_bytea_penalty(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *o = (GISTENTRY *) MDB_GETARG_POINTER(0);
	GISTENTRY  *n = (GISTENTRY *) MDB_GETARG_POINTER(1);
	float	   *result = (float *) MDB_GETARG_POINTER(2);

	MDB_RETURN_POINTER(gbt_var_penalty(result, o, n, MDB_GET_COLLATION(),
									  &tinfo));
}
