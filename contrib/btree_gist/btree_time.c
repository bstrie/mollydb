/*
 * contrib/btree_gist/btree_time.c
 */
#include "mollydb.h"

#include "btree_gist.h"
#include "btree_utils_num.h"
#include "utils/date.h"
#include "utils/timestamp.h"

typedef struct
{
	TimeADT		lower;
	TimeADT		upper;
} timeKEY;

/*
** time ops
*/
MDB_FUNCTION_INFO_V1(gbt_time_compress);
MDB_FUNCTION_INFO_V1(gbt_timetz_compress);
MDB_FUNCTION_INFO_V1(gbt_time_fetch);
MDB_FUNCTION_INFO_V1(gbt_time_union);
MDB_FUNCTION_INFO_V1(gbt_time_picksplit);
MDB_FUNCTION_INFO_V1(gbt_time_consistent);
MDB_FUNCTION_INFO_V1(gbt_time_distance);
MDB_FUNCTION_INFO_V1(gbt_timetz_consistent);
MDB_FUNCTION_INFO_V1(gbt_time_penalty);
MDB_FUNCTION_INFO_V1(gbt_time_same);


#ifdef USE_FLOAT8_BYVAL
#define TimeADTGetDatumFast(X) TimeADTGetDatum(X)
#else
#define TimeADTGetDatumFast(X) PointerGetDatum(&(X))
#endif


static bool
gbt_timegt(const void *a, const void *b)
{
	const TimeADT *aa = (const TimeADT *) a;
	const TimeADT *bb = (const TimeADT *) b;

	return DatumGetBool(DirectFunctionCall2(time_gt,
											TimeADTGetDatumFast(*aa),
											TimeADTGetDatumFast(*bb)));
}

static bool
gbt_timege(const void *a, const void *b)
{
	const TimeADT *aa = (const TimeADT *) a;
	const TimeADT *bb = (const TimeADT *) b;

	return DatumGetBool(DirectFunctionCall2(time_ge,
											TimeADTGetDatumFast(*aa),
											TimeADTGetDatumFast(*bb)));
}

static bool
gbt_timeeq(const void *a, const void *b)
{
	const TimeADT *aa = (const TimeADT *) a;
	const TimeADT *bb = (const TimeADT *) b;

	return DatumGetBool(DirectFunctionCall2(time_eq,
											TimeADTGetDatumFast(*aa),
											TimeADTGetDatumFast(*bb)));
}

static bool
gbt_timele(const void *a, const void *b)
{
	const TimeADT *aa = (const TimeADT *) a;
	const TimeADT *bb = (const TimeADT *) b;

	return DatumGetBool(DirectFunctionCall2(time_le,
											TimeADTGetDatumFast(*aa),
											TimeADTGetDatumFast(*bb)));
}

static bool
gbt_timelt(const void *a, const void *b)
{
	const TimeADT *aa = (const TimeADT *) a;
	const TimeADT *bb = (const TimeADT *) b;

	return DatumGetBool(DirectFunctionCall2(time_lt,
											TimeADTGetDatumFast(*aa),
											TimeADTGetDatumFast(*bb)));
}



static int
gbt_timekey_cmp(const void *a, const void *b)
{
	timeKEY    *ia = (timeKEY *) (((const Nsrt *) a)->t);
	timeKEY    *ib = (timeKEY *) (((const Nsrt *) b)->t);
	int			res;

	res = DatumGetInt32(DirectFunctionCall2(time_cmp, TimeADTGetDatumFast(ia->lower), TimeADTGetDatumFast(ib->lower)));
	if (res == 0)
		return DatumGetInt32(DirectFunctionCall2(time_cmp, TimeADTGetDatumFast(ia->upper), TimeADTGetDatumFast(ib->upper)));

	return res;
}

static float8
gbt_time_dist(const void *a, const void *b)
{
	const TimeADT *aa = (const TimeADT *) a;
	const TimeADT *bb = (const TimeADT *) b;
	Interval   *i;

	i = DatumGetIntervalP(DirectFunctionCall2(time_mi_time,
											  TimeADTGetDatumFast(*aa),
											  TimeADTGetDatumFast(*bb)));
	return (float8) Abs(INTERVAL_TO_SEC(i));
}


static const gbtree_ninfo tinfo =
{
	gbt_t_time,
	sizeof(TimeADT),
	16,							/* sizeof(gbtreekey16) */
	gbt_timegt,
	gbt_timege,
	gbt_timeeq,
	gbt_timele,
	gbt_timelt,
	gbt_timekey_cmp,
	gbt_time_dist
};


MDB_FUNCTION_INFO_V1(time_dist);
Datum
time_dist(MDB_FUNCTION_ARGS)
{
	Datum		diff = DirectFunctionCall2(time_mi_time,
										   MDB_GETARG_DATUM(0),
										   MDB_GETARG_DATUM(1));

	MDB_RETURN_INTERVAL_P(abs_interval(DatumGetIntervalP(diff)));
}


/**************************************************
 * time ops
 **************************************************/



Datum
gbt_time_compress(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);

	MDB_RETURN_POINTER(gbt_num_compress(entry, &tinfo));
}


Datum
gbt_timetz_compress(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);
	GISTENTRY  *retval;

	if (entry->leafkey)
	{
		timeKEY    *r = (timeKEY *) palloc(sizeof(timeKEY));
		TimeTzADT  *tz = DatumGetTimeTzADTP(entry->key);
		TimeADT		tmp;

		retval = palloc(sizeof(GISTENTRY));

		/* We are using the time + zone only to compress */
#ifdef HAVE_INT64_TIMESTAMP
		tmp = tz->time + (tz->zone * INT64CONST(1000000));
#else
		tmp = (tz->time + tz->zone);
#endif
		r->lower = r->upper = tmp;
		gistentryinit(*retval, PointerGetDatum(r),
					  entry->rel, entry->page,
					  entry->offset, FALSE);
	}
	else
		retval = entry;
	MDB_RETURN_POINTER(retval);
}

Datum
gbt_time_fetch(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);

	MDB_RETURN_POINTER(gbt_num_fetch(entry, &tinfo));
}

Datum
gbt_time_consistent(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);
	TimeADT		query = MDB_GETARG_TIMEADT(1);
	StrategyNumber strategy = (StrategyNumber) MDB_GETARG_UINT16(2);

	/* Oid		subtype = MDB_GETARG_OID(3); */
	bool	   *recheck = (bool *) MDB_GETARG_POINTER(4);
	timeKEY    *kkk = (timeKEY *) DatumGetPointer(entry->key);
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
gbt_time_distance(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);
	TimeADT		query = MDB_GETARG_TIMEADT(1);

	/* Oid		subtype = MDB_GETARG_OID(3); */
	timeKEY    *kkk = (timeKEY *) DatumGetPointer(entry->key);
	GBT_NUMKEY_R key;

	key.lower = (GBT_NUMKEY *) &kkk->lower;
	key.upper = (GBT_NUMKEY *) &kkk->upper;

	MDB_RETURN_FLOAT8(
			gbt_num_distance(&key, (void *) &query, GIST_LEAF(entry), &tinfo)
		);
}

Datum
gbt_timetz_consistent(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);
	TimeTzADT  *query = MDB_GETARG_TIMETZADT_P(1);
	StrategyNumber strategy = (StrategyNumber) MDB_GETARG_UINT16(2);

	/* Oid		subtype = MDB_GETARG_OID(3); */
	bool	   *recheck = (bool *) MDB_GETARG_POINTER(4);
	timeKEY    *kkk = (timeKEY *) DatumGetPointer(entry->key);
	TimeADT		qqq;
	GBT_NUMKEY_R key;

	/* All cases served by this function are inexact */
	*recheck = true;

#ifdef HAVE_INT64_TIMESTAMP
	qqq = query->time + (query->zone * INT64CONST(1000000));
#else
	qqq = (query->time + query->zone);
#endif

	key.lower = (GBT_NUMKEY *) &kkk->lower;
	key.upper = (GBT_NUMKEY *) &kkk->upper;

	MDB_RETURN_BOOL(
				   gbt_num_consistent(&key, (void *) &qqq, &strategy, GIST_LEAF(entry), &tinfo)
		);
}


Datum
gbt_time_union(MDB_FUNCTION_ARGS)
{
	GistEntryVector *entryvec = (GistEntryVector *) MDB_GETARG_POINTER(0);
	void	   *out = palloc(sizeof(timeKEY));

	*(int *) MDB_GETARG_POINTER(1) = sizeof(timeKEY);
	MDB_RETURN_POINTER(gbt_num_union((void *) out, entryvec, &tinfo));
}


Datum
gbt_time_penalty(MDB_FUNCTION_ARGS)
{
	timeKEY    *origentry = (timeKEY *) DatumGetPointer(((GISTENTRY *) MDB_GETARG_POINTER(0))->key);
	timeKEY    *newentry = (timeKEY *) DatumGetPointer(((GISTENTRY *) MDB_GETARG_POINTER(1))->key);
	float	   *result = (float *) MDB_GETARG_POINTER(2);
	Interval   *intr;
	double		res;
	double		res2;

	intr = DatumGetIntervalP(DirectFunctionCall2(
												 time_mi_time,
										TimeADTGetDatumFast(newentry->upper),
									 TimeADTGetDatumFast(origentry->upper)));
	res = INTERVAL_TO_SEC(intr);
	res = Max(res, 0);

	intr = DatumGetIntervalP(DirectFunctionCall2(
												 time_mi_time,
									   TimeADTGetDatumFast(origentry->lower),
									  TimeADTGetDatumFast(newentry->lower)));
	res2 = INTERVAL_TO_SEC(intr);
	res2 = Max(res2, 0);

	res += res2;

	*result = 0.0;

	if (res > 0)
	{
		intr = DatumGetIntervalP(DirectFunctionCall2(
													 time_mi_time,
									   TimeADTGetDatumFast(origentry->upper),
									 TimeADTGetDatumFast(origentry->lower)));
		*result += FLT_MIN;
		*result += (float) (res / (res + INTERVAL_TO_SEC(intr)));
		*result *= (FLT_MAX / (((GISTENTRY *) MDB_GETARG_POINTER(0))->rel->rd_att->natts + 1));
	}

	MDB_RETURN_POINTER(result);
}


Datum
gbt_time_picksplit(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_POINTER(gbt_num_picksplit(
									(GistEntryVector *) MDB_GETARG_POINTER(0),
									  (GIST_SPLITVEC *) MDB_GETARG_POINTER(1),
										&tinfo
										));
}

Datum
gbt_time_same(MDB_FUNCTION_ARGS)
{
	timeKEY    *b1 = (timeKEY *) MDB_GETARG_POINTER(0);
	timeKEY    *b2 = (timeKEY *) MDB_GETARG_POINTER(1);
	bool	   *result = (bool *) MDB_GETARG_POINTER(2);

	*result = gbt_num_same((void *) b1, (void *) b2, &tinfo);
	MDB_RETURN_POINTER(result);
}
