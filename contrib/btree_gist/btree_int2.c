/*
 * contrib/btree_gist/btree_int2.c
 */
#include "mollydb.h"

#include "btree_gist.h"
#include "btree_utils_num.h"

typedef struct int16key
{
	int16		lower;
	int16		upper;
} int16KEY;

/*
** int16 ops
*/
MDB_FUNCTION_INFO_V1(gbt_int2_compress);
MDB_FUNCTION_INFO_V1(gbt_int2_fetch);
MDB_FUNCTION_INFO_V1(gbt_int2_union);
MDB_FUNCTION_INFO_V1(gbt_int2_picksplit);
MDB_FUNCTION_INFO_V1(gbt_int2_consistent);
MDB_FUNCTION_INFO_V1(gbt_int2_distance);
MDB_FUNCTION_INFO_V1(gbt_int2_penalty);
MDB_FUNCTION_INFO_V1(gbt_int2_same);

static bool
gbt_int2gt(const void *a, const void *b)
{
	return (*((const int16 *) a) > *((const int16 *) b));
}
static bool
gbt_int2ge(const void *a, const void *b)
{
	return (*((const int16 *) a) >= *((const int16 *) b));
}
static bool
gbt_int2eq(const void *a, const void *b)
{
	return (*((const int16 *) a) == *((const int16 *) b));
}
static bool
gbt_int2le(const void *a, const void *b)
{
	return (*((const int16 *) a) <= *((const int16 *) b));
}
static bool
gbt_int2lt(const void *a, const void *b)
{
	return (*((const int16 *) a) < *((const int16 *) b));
}

static int
gbt_int2key_cmp(const void *a, const void *b)
{
	int16KEY   *ia = (int16KEY *) (((const Nsrt *) a)->t);
	int16KEY   *ib = (int16KEY *) (((const Nsrt *) b)->t);

	if (ia->lower == ib->lower)
	{
		if (ia->upper == ib->upper)
			return 0;

		return (ia->upper > ib->upper) ? 1 : -1;
	}

	return (ia->lower > ib->lower) ? 1 : -1;
}

static float8
gbt_int2_dist(const void *a, const void *b)
{
	return GET_FLOAT_DISTANCE(int16, a, b);
}


static const gbtree_ninfo tinfo =
{
	gbt_t_int2,
	sizeof(int16),
	4,							/* sizeof(gbtreekey4) */
	gbt_int2gt,
	gbt_int2ge,
	gbt_int2eq,
	gbt_int2le,
	gbt_int2lt,
	gbt_int2key_cmp,
	gbt_int2_dist
};


MDB_FUNCTION_INFO_V1(int2_dist);
Datum
int2_dist(MDB_FUNCTION_ARGS)
{
	int16		a = MDB_GETARG_INT16(0);
	int16		b = MDB_GETARG_INT16(1);
	int16		r;
	int16		ra;

	r = a - b;
	ra = Abs(r);

	/* Overflow check. */
	if (ra < 0 || (!SAMESIGN(a, b) && !SAMESIGN(r, a)))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("smallint out of range")));

	MDB_RETURN_INT16(ra);
}


/**************************************************
 * int16 ops
 **************************************************/


Datum
gbt_int2_compress(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);

	MDB_RETURN_POINTER(gbt_num_compress(entry, &tinfo));
}

Datum
gbt_int2_fetch(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);

	MDB_RETURN_POINTER(gbt_num_fetch(entry, &tinfo));
}

Datum
gbt_int2_consistent(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);
	int16		query = MDB_GETARG_INT16(1);
	StrategyNumber strategy = (StrategyNumber) MDB_GETARG_UINT16(2);

	/* Oid		subtype = MDB_GETARG_OID(3); */
	bool	   *recheck = (bool *) MDB_GETARG_POINTER(4);
	int16KEY   *kkk = (int16KEY *) DatumGetPointer(entry->key);
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
gbt_int2_distance(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);
	int16		query = MDB_GETARG_INT16(1);

	/* Oid		subtype = MDB_GETARG_OID(3); */
	int16KEY   *kkk = (int16KEY *) DatumGetPointer(entry->key);
	GBT_NUMKEY_R key;

	key.lower = (GBT_NUMKEY *) &kkk->lower;
	key.upper = (GBT_NUMKEY *) &kkk->upper;

	MDB_RETURN_FLOAT8(
			gbt_num_distance(&key, (void *) &query, GIST_LEAF(entry), &tinfo)
		);
}


Datum
gbt_int2_union(MDB_FUNCTION_ARGS)
{
	GistEntryVector *entryvec = (GistEntryVector *) MDB_GETARG_POINTER(0);
	void	   *out = palloc(sizeof(int16KEY));

	*(int *) MDB_GETARG_POINTER(1) = sizeof(int16KEY);
	MDB_RETURN_POINTER(gbt_num_union((void *) out, entryvec, &tinfo));
}


Datum
gbt_int2_penalty(MDB_FUNCTION_ARGS)
{
	int16KEY   *origentry = (int16KEY *) DatumGetPointer(((GISTENTRY *) MDB_GETARG_POINTER(0))->key);
	int16KEY   *newentry = (int16KEY *) DatumGetPointer(((GISTENTRY *) MDB_GETARG_POINTER(1))->key);
	float	   *result = (float *) MDB_GETARG_POINTER(2);

	penalty_num(result, origentry->lower, origentry->upper, newentry->lower, newentry->upper);

	MDB_RETURN_POINTER(result);
}

Datum
gbt_int2_picksplit(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_POINTER(gbt_num_picksplit(
									(GistEntryVector *) MDB_GETARG_POINTER(0),
									  (GIST_SPLITVEC *) MDB_GETARG_POINTER(1),
										&tinfo
										));
}

Datum
gbt_int2_same(MDB_FUNCTION_ARGS)
{
	int16KEY   *b1 = (int16KEY *) MDB_GETARG_POINTER(0);
	int16KEY   *b2 = (int16KEY *) MDB_GETARG_POINTER(1);
	bool	   *result = (bool *) MDB_GETARG_POINTER(2);

	*result = gbt_num_same((void *) b1, (void *) b2, &tinfo);
	MDB_RETURN_POINTER(result);
}
