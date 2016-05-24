/*
 * contrib/btree_gist/btree_oid.c
 */
#include "mollydb.h"

#include "btree_gist.h"
#include "btree_utils_num.h"

typedef struct
{
	Oid			lower;
	Oid			upper;
} oidKEY;

/*
** OID ops
*/
MDB_FUNCTION_INFO_V1(gbt_oid_compress);
MDB_FUNCTION_INFO_V1(gbt_oid_fetch);
MDB_FUNCTION_INFO_V1(gbt_oid_union);
MDB_FUNCTION_INFO_V1(gbt_oid_picksplit);
MDB_FUNCTION_INFO_V1(gbt_oid_consistent);
MDB_FUNCTION_INFO_V1(gbt_oid_distance);
MDB_FUNCTION_INFO_V1(gbt_oid_penalty);
MDB_FUNCTION_INFO_V1(gbt_oid_same);


static bool
gbt_oidgt(const void *a, const void *b)
{
	return (*((const Oid *) a) > *((const Oid *) b));
}
static bool
gbt_oidge(const void *a, const void *b)
{
	return (*((const Oid *) a) >= *((const Oid *) b));
}
static bool
gbt_oideq(const void *a, const void *b)
{
	return (*((const Oid *) a) == *((const Oid *) b));
}
static bool
gbt_oidle(const void *a, const void *b)
{
	return (*((const Oid *) a) <= *((const Oid *) b));
}
static bool
gbt_oidlt(const void *a, const void *b)
{
	return (*((const Oid *) a) < *((const Oid *) b));
}

static int
gbt_oidkey_cmp(const void *a, const void *b)
{
	oidKEY	   *ia = (oidKEY *) (((const Nsrt *) a)->t);
	oidKEY	   *ib = (oidKEY *) (((const Nsrt *) b)->t);

	if (ia->lower == ib->lower)
	{
		if (ia->upper == ib->upper)
			return 0;

		return (ia->upper > ib->upper) ? 1 : -1;
	}

	return (ia->lower > ib->lower) ? 1 : -1;
}

static float8
gbt_oid_dist(const void *a, const void *b)
{
	Oid			aa = *(const Oid *) a;
	Oid			bb = *(const Oid *) b;

	if (aa < bb)
		return (float8) (bb - aa);
	else
		return (float8) (aa - bb);
}


static const gbtree_ninfo tinfo =
{
	gbt_t_oid,
	sizeof(Oid),
	8,							/* sizeof(gbtreekey8) */
	gbt_oidgt,
	gbt_oidge,
	gbt_oideq,
	gbt_oidle,
	gbt_oidlt,
	gbt_oidkey_cmp,
	gbt_oid_dist
};


MDB_FUNCTION_INFO_V1(oid_dist);
Datum
oid_dist(MDB_FUNCTION_ARGS)
{
	Oid			a = MDB_GETARG_OID(0);
	Oid			b = MDB_GETARG_OID(1);
	Oid			res;

	if (a < b)
		res = b - a;
	else
		res = a - b;
	MDB_RETURN_OID(res);
}


/**************************************************
 * Oid ops
 **************************************************/


Datum
gbt_oid_compress(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);

	MDB_RETURN_POINTER(gbt_num_compress(entry, &tinfo));
}

Datum
gbt_oid_fetch(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);

	MDB_RETURN_POINTER(gbt_num_fetch(entry, &tinfo));
}

Datum
gbt_oid_consistent(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);
	Oid			query = MDB_GETARG_OID(1);
	StrategyNumber strategy = (StrategyNumber) MDB_GETARG_UINT16(2);

	/* Oid		subtype = MDB_GETARG_OID(3); */
	bool	   *recheck = (bool *) MDB_GETARG_POINTER(4);
	oidKEY	   *kkk = (oidKEY *) DatumGetPointer(entry->key);
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
gbt_oid_distance(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);
	Oid			query = MDB_GETARG_OID(1);

	/* Oid		subtype = MDB_GETARG_OID(3); */
	oidKEY	   *kkk = (oidKEY *) DatumGetPointer(entry->key);
	GBT_NUMKEY_R key;

	key.lower = (GBT_NUMKEY *) &kkk->lower;
	key.upper = (GBT_NUMKEY *) &kkk->upper;

	MDB_RETURN_FLOAT8(
			gbt_num_distance(&key, (void *) &query, GIST_LEAF(entry), &tinfo)
		);
}


Datum
gbt_oid_union(MDB_FUNCTION_ARGS)
{
	GistEntryVector *entryvec = (GistEntryVector *) MDB_GETARG_POINTER(0);
	void	   *out = palloc(sizeof(oidKEY));

	*(int *) MDB_GETARG_POINTER(1) = sizeof(oidKEY);
	MDB_RETURN_POINTER(gbt_num_union((void *) out, entryvec, &tinfo));
}


Datum
gbt_oid_penalty(MDB_FUNCTION_ARGS)
{
	oidKEY	   *origentry = (oidKEY *) DatumGetPointer(((GISTENTRY *) MDB_GETARG_POINTER(0))->key);
	oidKEY	   *newentry = (oidKEY *) DatumGetPointer(((GISTENTRY *) MDB_GETARG_POINTER(1))->key);
	float	   *result = (float *) MDB_GETARG_POINTER(2);

	penalty_num(result, origentry->lower, origentry->upper, newentry->lower, newentry->upper);

	MDB_RETURN_POINTER(result);
}

Datum
gbt_oid_picksplit(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_POINTER(gbt_num_picksplit(
									(GistEntryVector *) MDB_GETARG_POINTER(0),
									  (GIST_SPLITVEC *) MDB_GETARG_POINTER(1),
										&tinfo
										));
}

Datum
gbt_oid_same(MDB_FUNCTION_ARGS)
{
	oidKEY	   *b1 = (oidKEY *) MDB_GETARG_POINTER(0);
	oidKEY	   *b2 = (oidKEY *) MDB_GETARG_POINTER(1);
	bool	   *result = (bool *) MDB_GETARG_POINTER(2);

	*result = gbt_num_same((void *) b1, (void *) b2, &tinfo);
	MDB_RETURN_POINTER(result);
}
