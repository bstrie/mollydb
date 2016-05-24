/*
 * contrib/btree_gist/btree_bit.c
 */
#include "mollydb.h"

#include "btree_gist.h"
#include "btree_utils_var.h"
#include "utils/bytea.h"
#include "utils/varbit.h"


/*
** Bit ops
*/
MDB_FUNCTION_INFO_V1(gbt_bit_compress);
MDB_FUNCTION_INFO_V1(gbt_bit_union);
MDB_FUNCTION_INFO_V1(gbt_bit_picksplit);
MDB_FUNCTION_INFO_V1(gbt_bit_consistent);
MDB_FUNCTION_INFO_V1(gbt_bit_penalty);
MDB_FUNCTION_INFO_V1(gbt_bit_same);


/* define for comparison */

static bool
gbt_bitgt(const void *a, const void *b, Oid collation)
{
	return DatumGetBool(DirectFunctionCall2(bitgt,
											PointerGetDatum(a),
											PointerGetDatum(b)));
}

static bool
gbt_bitge(const void *a, const void *b, Oid collation)
{
	return DatumGetBool(DirectFunctionCall2(bitge,
											PointerGetDatum(a),
											PointerGetDatum(b)));
}

static bool
gbt_biteq(const void *a, const void *b, Oid collation)
{
	return DatumGetBool(DirectFunctionCall2(biteq,
											PointerGetDatum(a),
											PointerGetDatum(b)));
}

static bool
gbt_bitle(const void *a, const void *b, Oid collation)
{
	return DatumGetBool(DirectFunctionCall2(bitle,
											PointerGetDatum(a),
											PointerGetDatum(b)));
}

static bool
gbt_bitlt(const void *a, const void *b, Oid collation)
{
	return DatumGetBool(DirectFunctionCall2(bitlt,
											PointerGetDatum(a),
											PointerGetDatum(b)));
}

static int32
gbt_bitcmp(const void *a, const void *b, Oid collation)
{
	return DatumGetInt32(DirectFunctionCall2(byteacmp,
											 PointerGetDatum(a),
											 PointerGetDatum(b)));
}


static bytea *
gbt_bit_xfrm(bytea *leaf)
{
	bytea	   *out = leaf;
	int			sz = VARBITBYTES(leaf) + VARHDRSZ;
	int			padded_sz = INTALIGN(sz);

	out = (bytea *) palloc(padded_sz);
	/* initialize the padding bytes to zero */
	while (sz < padded_sz)
		((char *) out)[sz++] = 0;
	SET_VARSIZE(out, padded_sz);
	memcpy((void *) VARDATA(out), (void *) VARBITS(leaf), VARBITBYTES(leaf));
	return out;
}




static GBT_VARKEY *
gbt_bit_l2n(GBT_VARKEY *leaf)
{
	GBT_VARKEY *out = leaf;
	GBT_VARKEY_R r = gbt_var_key_readable(leaf);
	bytea	   *o;

	o = gbt_bit_xfrm(r.lower);
	r.upper = r.lower = o;
	out = gbt_var_key_copy(&r);
	pfree(o);

	return out;

}

static const gbtree_vinfo tinfo =
{
	gbt_t_bit,
	0,
	TRUE,
	gbt_bitgt,
	gbt_bitge,
	gbt_biteq,
	gbt_bitle,
	gbt_bitlt,
	gbt_bitcmp,
	gbt_bit_l2n
};


/**************************************************
 * Bit ops
 **************************************************/

Datum
gbt_bit_compress(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);

	MDB_RETURN_POINTER(gbt_var_compress(entry, &tinfo));
}

Datum
gbt_bit_consistent(MDB_FUNCTION_ARGS)
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

	if (GIST_LEAF(entry))
		retval = gbt_var_consistent(&r, query, strategy, MDB_GET_COLLATION(),
									TRUE, &tinfo);
	else
	{
		bytea	   *q = gbt_bit_xfrm((bytea *) query);

		retval = gbt_var_consistent(&r, q, strategy, MDB_GET_COLLATION(),
									FALSE, &tinfo);
	}
	MDB_RETURN_BOOL(retval);
}



Datum
gbt_bit_union(MDB_FUNCTION_ARGS)
{
	GistEntryVector *entryvec = (GistEntryVector *) MDB_GETARG_POINTER(0);
	int32	   *size = (int *) MDB_GETARG_POINTER(1);

	MDB_RETURN_POINTER(gbt_var_union(entryvec, size, MDB_GET_COLLATION(),
									&tinfo));
}


Datum
gbt_bit_picksplit(MDB_FUNCTION_ARGS)
{
	GistEntryVector *entryvec = (GistEntryVector *) MDB_GETARG_POINTER(0);
	GIST_SPLITVEC *v = (GIST_SPLITVEC *) MDB_GETARG_POINTER(1);

	gbt_var_picksplit(entryvec, v, MDB_GET_COLLATION(),
					  &tinfo);
	MDB_RETURN_POINTER(v);
}

Datum
gbt_bit_same(MDB_FUNCTION_ARGS)
{
	Datum		d1 = MDB_GETARG_DATUM(0);
	Datum		d2 = MDB_GETARG_DATUM(1);
	bool	   *result = (bool *) MDB_GETARG_POINTER(2);

	*result = gbt_var_same(d1, d2, MDB_GET_COLLATION(), &tinfo);
	MDB_RETURN_POINTER(result);
}


Datum
gbt_bit_penalty(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *o = (GISTENTRY *) MDB_GETARG_POINTER(0);
	GISTENTRY  *n = (GISTENTRY *) MDB_GETARG_POINTER(1);
	float	   *result = (float *) MDB_GETARG_POINTER(2);

	MDB_RETURN_POINTER(gbt_var_penalty(result, o, n, MDB_GET_COLLATION(),
									  &tinfo));
}
