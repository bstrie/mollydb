/*
 * contrib/btree_gist/btree_int8.c
 */
#include "mollydb.h"

#include "btree_gist.h"
#include "btree_utils_num.h"

typedef struct int64key
{
	int64		lower;
	int64		upper;
} int64KEY;

/*
** int64 ops
*/
MDB_FUNCTION_INFO_V1(gbt_int8_compress);
MDB_FUNCTION_INFO_V1(gbt_int8_fetch);
MDB_FUNCTION_INFO_V1(gbt_int8_union);
MDB_FUNCTION_INFO_V1(gbt_int8_picksplit);
MDB_FUNCTION_INFO_V1(gbt_int8_consistent);
MDB_FUNCTION_INFO_V1(gbt_int8_distance);
MDB_FUNCTION_INFO_V1(gbt_int8_penalty);
MDB_FUNCTION_INFO_V1(gbt_int8_same);


static bool
gbt_int8gt(const void *a, const void *b)
{
	return (*((const int64 *) a) > *((const int64 *) b));
}
static bool
gbt_int8ge(const void *a, const void *b)
{
	return (*((const int64 *) a) >= *((const int64 *) b));
}
static bool
gbt_int8eq(const void *a, const void *b)
{
	return (*((const int64 *) a) == *((const int64 *) b));
}
static bool
gbt_int8le(const void *a, const void *b)
{
	return (*((const int64 *) a) <= *((const int64 *) b));
}
static bool
gbt_int8lt(const void *a, const void *b)
{
	return (*((const int64 *) a) < *((const int64 *) b));
}

static int
gbt_int8key_cmp(const void *a, const void *b)
{
	int64KEY   *ia = (int64KEY *) (((const Nsrt *) a)->t);
	int64KEY   *ib = (int64KEY *) (((const Nsrt *) b)->t);

	if (ia->lower == ib->lower)
	{
		if (ia->upper == ib->upper)
			return 0;

		return (ia->upper > ib->upper) ? 1 : -1;
	}

	return (ia->lower > ib->lower) ? 1 : -1;
}

static float8
gbt_int8_dist(const void *a, const void *b)
{
	return GET_FLOAT_DISTANCE(int64, a, b);
}


static const gbtree_ninfo tinfo =
{
	gbt_t_int8,
	sizeof(int64),
	16,							/* sizeof(gbtreekey16) */
	gbt_int8gt,
	gbt_int8ge,
	gbt_int8eq,
	gbt_int8le,
	gbt_int8lt,
	gbt_int8key_cmp,
	gbt_int8_dist
};


MDB_FUNCTION_INFO_V1(int8_dist);
Datum
int8_dist(MDB_FUNCTION_ARGS)
{
	int64		a = MDB_GETARG_INT64(0);
	int64		b = MDB_GETARG_INT64(1);
	int64		r;
	int64		ra;

	r = a - b;
	ra = Abs(r);

	/* Overflow check. */
	if (ra < 0 || (!SAMESIGN(a, b) && !SAMESIGN(r, a)))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint out of range")));

	MDB_RETURN_INT64(ra);
}


/**************************************************
 * int64 ops
 **************************************************/


Datum
gbt_int8_compress(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);

	MDB_RETURN_POINTER(gbt_num_compress(entry, &tinfo));
}

Datum
gbt_int8_fetch(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);

	MDB_RETURN_POINTER(gbt_num_fetch(entry, &tinfo));
}

Datum
gbt_int8_consistent(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);
	int64		query = MDB_GETARG_INT64(1);
	StrategyNumber strategy = (StrategyNumber) MDB_GETARG_UINT16(2);

	/* Oid		subtype = MDB_GETARG_OID(3); */
	bool	   *recheck = (bool *) MDB_GETARG_POINTER(4);
	int64KEY   *kkk = (int64KEY *) DatumGetPointer(entry->key);
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
gbt_int8_distance(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);
	int64		query = MDB_GETARG_INT64(1);

	/* Oid		subtype = MDB_GETARG_OID(3); */
	int64KEY   *kkk = (int64KEY *) DatumGetPointer(entry->key);
	GBT_NUMKEY_R key;

	key.lower = (GBT_NUMKEY *) &kkk->lower;
	key.upper = (GBT_NUMKEY *) &kkk->upper;

	MDB_RETURN_FLOAT8(
			gbt_num_distance(&key, (void *) &query, GIST_LEAF(entry), &tinfo)
		);
}


Datum
gbt_int8_union(MDB_FUNCTION_ARGS)
{
	GistEntryVector *entryvec = (GistEntryVector *) MDB_GETARG_POINTER(0);
	void	   *out = palloc(sizeof(int64KEY));

	*(int *) MDB_GETARG_POINTER(1) = sizeof(int64KEY);
	MDB_RETURN_POINTER(gbt_num_union((void *) out, entryvec, &tinfo));
}


Datum
gbt_int8_penalty(MDB_FUNCTION_ARGS)
{
	int64KEY   *origentry = (int64KEY *) DatumGetPointer(((GISTENTRY *) MDB_GETARG_POINTER(0))->key);
	int64KEY   *newentry = (int64KEY *) DatumGetPointer(((GISTENTRY *) MDB_GETARG_POINTER(1))->key);
	float	   *result = (float *) MDB_GETARG_POINTER(2);

	penalty_num(result, origentry->lower, origentry->upper, newentry->lower, newentry->upper);

	MDB_RETURN_POINTER(result);
}

Datum
gbt_int8_picksplit(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_POINTER(gbt_num_picksplit(
									(GistEntryVector *) MDB_GETARG_POINTER(0),
									  (GIST_SPLITVEC *) MDB_GETARG_POINTER(1),
										&tinfo
										));
}

Datum
gbt_int8_same(MDB_FUNCTION_ARGS)
{
	int64KEY   *b1 = (int64KEY *) MDB_GETARG_POINTER(0);
	int64KEY   *b2 = (int64KEY *) MDB_GETARG_POINTER(1);
	bool	   *result = (bool *) MDB_GETARG_POINTER(2);

	*result = gbt_num_same((void *) b1, (void *) b2, &tinfo);
	MDB_RETURN_POINTER(result);
}
