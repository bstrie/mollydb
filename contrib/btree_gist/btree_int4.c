/*
 * contrib/btree_gist/btree_int4.c
 */
#include "mollydb.h"

#include "btree_gist.h"
#include "btree_utils_num.h"

typedef struct int32key
{
	int32		lower;
	int32		upper;
} int32KEY;

/*
** int32 ops
*/
MDB_FUNCTION_INFO_V1(gbt_int4_compress);
MDB_FUNCTION_INFO_V1(gbt_int4_fetch);
MDB_FUNCTION_INFO_V1(gbt_int4_union);
MDB_FUNCTION_INFO_V1(gbt_int4_picksplit);
MDB_FUNCTION_INFO_V1(gbt_int4_consistent);
MDB_FUNCTION_INFO_V1(gbt_int4_distance);
MDB_FUNCTION_INFO_V1(gbt_int4_penalty);
MDB_FUNCTION_INFO_V1(gbt_int4_same);


static bool
gbt_int4gt(const void *a, const void *b)
{
	return (*((const int32 *) a) > *((const int32 *) b));
}
static bool
gbt_int4ge(const void *a, const void *b)
{
	return (*((const int32 *) a) >= *((const int32 *) b));
}
static bool
gbt_int4eq(const void *a, const void *b)
{
	return (*((const int32 *) a) == *((const int32 *) b));
}
static bool
gbt_int4le(const void *a, const void *b)
{
	return (*((const int32 *) a) <= *((const int32 *) b));
}
static bool
gbt_int4lt(const void *a, const void *b)
{
	return (*((const int32 *) a) < *((const int32 *) b));
}

static int
gbt_int4key_cmp(const void *a, const void *b)
{
	int32KEY   *ia = (int32KEY *) (((const Nsrt *) a)->t);
	int32KEY   *ib = (int32KEY *) (((const Nsrt *) b)->t);

	if (ia->lower == ib->lower)
	{
		if (ia->upper == ib->upper)
			return 0;

		return (ia->upper > ib->upper) ? 1 : -1;
	}

	return (ia->lower > ib->lower) ? 1 : -1;
}

static float8
gbt_int4_dist(const void *a, const void *b)
{
	return GET_FLOAT_DISTANCE(int32, a, b);
}


static const gbtree_ninfo tinfo =
{
	gbt_t_int4,
	sizeof(int32),
	8,							/* sizeof(gbtreekey8) */
	gbt_int4gt,
	gbt_int4ge,
	gbt_int4eq,
	gbt_int4le,
	gbt_int4lt,
	gbt_int4key_cmp,
	gbt_int4_dist
};


MDB_FUNCTION_INFO_V1(int4_dist);
Datum
int4_dist(MDB_FUNCTION_ARGS)
{
	int32		a = MDB_GETARG_INT32(0);
	int32		b = MDB_GETARG_INT32(1);
	int32		r;
	int32		ra;

	r = a - b;
	ra = Abs(r);

	/* Overflow check. */
	if (ra < 0 || (!SAMESIGN(a, b) && !SAMESIGN(r, a)))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("integer out of range")));

	MDB_RETURN_INT32(ra);
}


/**************************************************
 * int32 ops
 **************************************************/


Datum
gbt_int4_compress(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);

	MDB_RETURN_POINTER(gbt_num_compress(entry, &tinfo));
}

Datum
gbt_int4_fetch(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);

	MDB_RETURN_POINTER(gbt_num_fetch(entry, &tinfo));
}

Datum
gbt_int4_consistent(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);
	int32		query = MDB_GETARG_INT32(1);
	StrategyNumber strategy = (StrategyNumber) MDB_GETARG_UINT16(2);

	/* Oid		subtype = MDB_GETARG_OID(3); */
	bool	   *recheck = (bool *) MDB_GETARG_POINTER(4);
	int32KEY   *kkk = (int32KEY *) DatumGetPointer(entry->key);
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
gbt_int4_distance(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);
	int32		query = MDB_GETARG_INT32(1);

	/* Oid		subtype = MDB_GETARG_OID(3); */
	int32KEY   *kkk = (int32KEY *) DatumGetPointer(entry->key);
	GBT_NUMKEY_R key;

	key.lower = (GBT_NUMKEY *) &kkk->lower;
	key.upper = (GBT_NUMKEY *) &kkk->upper;

	MDB_RETURN_FLOAT8(
			gbt_num_distance(&key, (void *) &query, GIST_LEAF(entry), &tinfo)
		);
}


Datum
gbt_int4_union(MDB_FUNCTION_ARGS)
{
	GistEntryVector *entryvec = (GistEntryVector *) MDB_GETARG_POINTER(0);
	void	   *out = palloc(sizeof(int32KEY));

	*(int *) MDB_GETARG_POINTER(1) = sizeof(int32KEY);
	MDB_RETURN_POINTER(gbt_num_union((void *) out, entryvec, &tinfo));
}


Datum
gbt_int4_penalty(MDB_FUNCTION_ARGS)
{
	int32KEY   *origentry = (int32KEY *) DatumGetPointer(((GISTENTRY *) MDB_GETARG_POINTER(0))->key);
	int32KEY   *newentry = (int32KEY *) DatumGetPointer(((GISTENTRY *) MDB_GETARG_POINTER(1))->key);
	float	   *result = (float *) MDB_GETARG_POINTER(2);

	penalty_num(result, origentry->lower, origentry->upper, newentry->lower, newentry->upper);

	MDB_RETURN_POINTER(result);
}

Datum
gbt_int4_picksplit(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_POINTER(gbt_num_picksplit(
									(GistEntryVector *) MDB_GETARG_POINTER(0),
									  (GIST_SPLITVEC *) MDB_GETARG_POINTER(1),
										&tinfo
										));
}

Datum
gbt_int4_same(MDB_FUNCTION_ARGS)
{
	int32KEY   *b1 = (int32KEY *) MDB_GETARG_POINTER(0);
	int32KEY   *b2 = (int32KEY *) MDB_GETARG_POINTER(1);
	bool	   *result = (bool *) MDB_GETARG_POINTER(2);

	*result = gbt_num_same((void *) b1, (void *) b2, &tinfo);
	MDB_RETURN_POINTER(result);
}
