/*
 * contrib/btree_gist/btree_float4.c
 */
#include "mollydb.h"

#include "btree_gist.h"
#include "btree_utils_num.h"

typedef struct float4key
{
	float4		lower;
	float4		upper;
} float4KEY;

/*
** float4 ops
*/
MDB_FUNCTION_INFO_V1(gbt_float4_compress);
MDB_FUNCTION_INFO_V1(gbt_float4_fetch);
MDB_FUNCTION_INFO_V1(gbt_float4_union);
MDB_FUNCTION_INFO_V1(gbt_float4_picksplit);
MDB_FUNCTION_INFO_V1(gbt_float4_consistent);
MDB_FUNCTION_INFO_V1(gbt_float4_distance);
MDB_FUNCTION_INFO_V1(gbt_float4_penalty);
MDB_FUNCTION_INFO_V1(gbt_float4_same);

static bool
gbt_float4gt(const void *a, const void *b)
{
	return (*((const float4 *) a) > *((const float4 *) b));
}
static bool
gbt_float4ge(const void *a, const void *b)
{
	return (*((const float4 *) a) >= *((const float4 *) b));
}
static bool
gbt_float4eq(const void *a, const void *b)
{
	return (*((const float4 *) a) == *((const float4 *) b));
}
static bool
gbt_float4le(const void *a, const void *b)
{
	return (*((const float4 *) a) <= *((const float4 *) b));
}
static bool
gbt_float4lt(const void *a, const void *b)
{
	return (*((const float4 *) a) < *((const float4 *) b));
}

static int
gbt_float4key_cmp(const void *a, const void *b)
{
	float4KEY  *ia = (float4KEY *) (((const Nsrt *) a)->t);
	float4KEY  *ib = (float4KEY *) (((const Nsrt *) b)->t);

	if (ia->lower == ib->lower)
	{
		if (ia->upper == ib->upper)
			return 0;

		return (ia->upper > ib->upper) ? 1 : -1;
	}

	return (ia->lower > ib->lower) ? 1 : -1;
}

static float8
gbt_float4_dist(const void *a, const void *b)
{
	return GET_FLOAT_DISTANCE(float4, a, b);
}


static const gbtree_ninfo tinfo =
{
	gbt_t_float4,
	sizeof(float4),
	8,							/* sizeof(gbtreekey8) */
	gbt_float4gt,
	gbt_float4ge,
	gbt_float4eq,
	gbt_float4le,
	gbt_float4lt,
	gbt_float4key_cmp,
	gbt_float4_dist
};


MDB_FUNCTION_INFO_V1(float4_dist);
Datum
float4_dist(MDB_FUNCTION_ARGS)
{
	float4		a = MDB_GETARG_FLOAT4(0);
	float4		b = MDB_GETARG_FLOAT4(1);
	float4		r;

	r = a - b;
	CHECKFLOATVAL(r, isinf(a) || isinf(b), true);

	MDB_RETURN_FLOAT4(Abs(r));
}


/**************************************************
 * float4 ops
 **************************************************/


Datum
gbt_float4_compress(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);

	MDB_RETURN_POINTER(gbt_num_compress(entry, &tinfo));
}

Datum
gbt_float4_fetch(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);

	MDB_RETURN_POINTER(gbt_num_fetch(entry, &tinfo));
}

Datum
gbt_float4_consistent(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);
	float4		query = MDB_GETARG_FLOAT4(1);
	StrategyNumber strategy = (StrategyNumber) MDB_GETARG_UINT16(2);

	/* Oid		subtype = MDB_GETARG_OID(3); */
	bool	   *recheck = (bool *) MDB_GETARG_POINTER(4);
	float4KEY  *kkk = (float4KEY *) DatumGetPointer(entry->key);
	GBT_NUMKEY_R key;

	/* All cases served by this function are exact */
	*recheck = false;

	key.lower = (GBT_NUMKEY *) &kkk->lower;
	key.upper = (GBT_NUMKEY *) &kkk->upper;

	MDB_RETURN_BOOL(
				   gbt_num_consistent(&key, (void *) &query, &strategy, GIST_LEAF(entry), &tinfo)
		);
}


Datum
gbt_float4_distance(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);
	float4		query = MDB_GETARG_FLOAT4(1);

	/* Oid		subtype = MDB_GETARG_OID(3); */
	float4KEY  *kkk = (float4KEY *) DatumGetPointer(entry->key);
	GBT_NUMKEY_R key;

	key.lower = (GBT_NUMKEY *) &kkk->lower;
	key.upper = (GBT_NUMKEY *) &kkk->upper;

	MDB_RETURN_FLOAT8(
			gbt_num_distance(&key, (void *) &query, GIST_LEAF(entry), &tinfo)
		);
}


Datum
gbt_float4_union(MDB_FUNCTION_ARGS)
{
	GistEntryVector *entryvec = (GistEntryVector *) MDB_GETARG_POINTER(0);
	void	   *out = palloc(sizeof(float4KEY));

	*(int *) MDB_GETARG_POINTER(1) = sizeof(float4KEY);
	MDB_RETURN_POINTER(gbt_num_union((void *) out, entryvec, &tinfo));
}


Datum
gbt_float4_penalty(MDB_FUNCTION_ARGS)
{
	float4KEY  *origentry = (float4KEY *) DatumGetPointer(((GISTENTRY *) MDB_GETARG_POINTER(0))->key);
	float4KEY  *newentry = (float4KEY *) DatumGetPointer(((GISTENTRY *) MDB_GETARG_POINTER(1))->key);
	float	   *result = (float *) MDB_GETARG_POINTER(2);

	penalty_num(result, origentry->lower, origentry->upper, newentry->lower, newentry->upper);

	MDB_RETURN_POINTER(result);

}

Datum
gbt_float4_picksplit(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_POINTER(gbt_num_picksplit(
									(GistEntryVector *) MDB_GETARG_POINTER(0),
									  (GIST_SPLITVEC *) MDB_GETARG_POINTER(1),
										&tinfo
										));
}

Datum
gbt_float4_same(MDB_FUNCTION_ARGS)
{
	float4KEY  *b1 = (float4KEY *) MDB_GETARG_POINTER(0);
	float4KEY  *b2 = (float4KEY *) MDB_GETARG_POINTER(1);
	bool	   *result = (bool *) MDB_GETARG_POINTER(2);

	*result = gbt_num_same((void *) b1, (void *) b2, &tinfo);
	MDB_RETURN_POINTER(result);
}
