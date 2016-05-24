/*
 * contrib/btree_gist/btree_inet.c
 */
#include "mollydb.h"

#include "btree_gist.h"
#include "btree_utils_num.h"
#include "utils/builtins.h"
#include "utils/inet.h"
#include "catalog/mdb_type.h"

typedef struct inetkey
{
	double		lower;
	double		upper;
} inetKEY;

/*
** inet ops
*/
MDB_FUNCTION_INFO_V1(gbt_inet_compress);
MDB_FUNCTION_INFO_V1(gbt_inet_union);
MDB_FUNCTION_INFO_V1(gbt_inet_picksplit);
MDB_FUNCTION_INFO_V1(gbt_inet_consistent);
MDB_FUNCTION_INFO_V1(gbt_inet_penalty);
MDB_FUNCTION_INFO_V1(gbt_inet_same);


static bool
gbt_inetgt(const void *a, const void *b)
{
	return (*((const double *) a) > *((const double *) b));
}
static bool
gbt_inetge(const void *a, const void *b)
{
	return (*((const double *) a) >= *((const double *) b));
}
static bool
gbt_ineteq(const void *a, const void *b)
{
	return (*((const double *) a) == *((const double *) b));
}
static bool
gbt_inetle(const void *a, const void *b)
{
	return (*((const double *) a) <= *((const double *) b));
}
static bool
gbt_inetlt(const void *a, const void *b)
{
	return (*((const double *) a) < *((const double *) b));
}

static int
gbt_inetkey_cmp(const void *a, const void *b)
{
	inetKEY    *ia = (inetKEY *) (((const Nsrt *) a)->t);
	inetKEY    *ib = (inetKEY *) (((const Nsrt *) b)->t);

	if (ia->lower == ib->lower)
	{
		if (ia->upper == ib->upper)
			return 0;

		return (ia->upper > ib->upper) ? 1 : -1;
	}

	return (ia->lower > ib->lower) ? 1 : -1;
}


static const gbtree_ninfo tinfo =
{
	gbt_t_inet,
	sizeof(double),
	16,							/* sizeof(gbtreekey16) */
	gbt_inetgt,
	gbt_inetge,
	gbt_ineteq,
	gbt_inetle,
	gbt_inetlt,
	gbt_inetkey_cmp,
	NULL
};


/**************************************************
 * inet ops
 **************************************************/


Datum
gbt_inet_compress(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);
	GISTENTRY  *retval;

	if (entry->leafkey)
	{
		inetKEY    *r = (inetKEY *) palloc(sizeof(inetKEY));

		retval = palloc(sizeof(GISTENTRY));
		r->lower = convert_network_to_scalar(entry->key, INETOID);
		r->upper = r->lower;
		gistentryinit(*retval, PointerGetDatum(r),
					  entry->rel, entry->page,
					  entry->offset, FALSE);
	}
	else
		retval = entry;

	MDB_RETURN_POINTER(retval);
}


Datum
gbt_inet_consistent(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);
	double		query = convert_network_to_scalar(MDB_GETARG_DATUM(1), INETOID);
	StrategyNumber strategy = (StrategyNumber) MDB_GETARG_UINT16(2);

	/* Oid		subtype = MDB_GETARG_OID(3); */
	bool	   *recheck = (bool *) MDB_GETARG_POINTER(4);
	inetKEY    *kkk = (inetKEY *) DatumGetPointer(entry->key);
	GBT_NUMKEY_R key;

	/* All cases served by this function are inexact */
	*recheck = true;

	key.lower = (GBT_NUMKEY *) &kkk->lower;
	key.upper = (GBT_NUMKEY *) &kkk->upper;

	MDB_RETURN_BOOL(gbt_num_consistent(&key, (void *) &query,
									  &strategy, GIST_LEAF(entry), &tinfo));
}


Datum
gbt_inet_union(MDB_FUNCTION_ARGS)
{
	GistEntryVector *entryvec = (GistEntryVector *) MDB_GETARG_POINTER(0);
	void	   *out = palloc(sizeof(inetKEY));

	*(int *) MDB_GETARG_POINTER(1) = sizeof(inetKEY);
	MDB_RETURN_POINTER(gbt_num_union((void *) out, entryvec, &tinfo));
}


Datum
gbt_inet_penalty(MDB_FUNCTION_ARGS)
{
	inetKEY    *origentry = (inetKEY *) DatumGetPointer(((GISTENTRY *) MDB_GETARG_POINTER(0))->key);
	inetKEY    *newentry = (inetKEY *) DatumGetPointer(((GISTENTRY *) MDB_GETARG_POINTER(1))->key);
	float	   *result = (float *) MDB_GETARG_POINTER(2);

	penalty_num(result, origentry->lower, origentry->upper, newentry->lower, newentry->upper);

	MDB_RETURN_POINTER(result);

}

Datum
gbt_inet_picksplit(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_POINTER(gbt_num_picksplit(
									(GistEntryVector *) MDB_GETARG_POINTER(0),
									  (GIST_SPLITVEC *) MDB_GETARG_POINTER(1),
										&tinfo
										));
}

Datum
gbt_inet_same(MDB_FUNCTION_ARGS)
{
	inetKEY    *b1 = (inetKEY *) MDB_GETARG_POINTER(0);
	inetKEY    *b2 = (inetKEY *) MDB_GETARG_POINTER(1);
	bool	   *result = (bool *) MDB_GETARG_POINTER(2);

	*result = gbt_num_same((void *) b1, (void *) b2, &tinfo);
	MDB_RETURN_POINTER(result);
}
