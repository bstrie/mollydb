/*
 * contrib/btree_gist/btree_date.c
 */
#include "mollydb.h"

#include "btree_gist.h"
#include "btree_utils_num.h"
#include "utils/date.h"

typedef struct
{
	DateADT		lower;
	DateADT		upper;
} dateKEY;

/*
** date ops
*/
MDB_FUNCTION_INFO_V1(gbt_date_compress);
MDB_FUNCTION_INFO_V1(gbt_date_fetch);
MDB_FUNCTION_INFO_V1(gbt_date_union);
MDB_FUNCTION_INFO_V1(gbt_date_picksplit);
MDB_FUNCTION_INFO_V1(gbt_date_consistent);
MDB_FUNCTION_INFO_V1(gbt_date_distance);
MDB_FUNCTION_INFO_V1(gbt_date_penalty);
MDB_FUNCTION_INFO_V1(gbt_date_same);

static bool
gbt_dategt(const void *a, const void *b)
{
	return DatumGetBool(
						DirectFunctionCall2(date_gt, DateADTGetDatum(*((const DateADT *) a)), DateADTGetDatum(*((const DateADT *) b)))
		);
}

static bool
gbt_datege(const void *a, const void *b)
{
	return DatumGetBool(
						DirectFunctionCall2(date_ge, DateADTGetDatum(*((const DateADT *) a)), DateADTGetDatum(*((const DateADT *) b)))
		);
}

static bool
gbt_dateeq(const void *a, const void *b)
{
	return DatumGetBool(
						DirectFunctionCall2(date_eq, DateADTGetDatum(*((const DateADT *) a)), DateADTGetDatum(*((const DateADT *) b)))
		);
}

static bool
gbt_datele(const void *a, const void *b)
{
	return DatumGetBool(
						DirectFunctionCall2(date_le, DateADTGetDatum(*((const DateADT *) a)), DateADTGetDatum(*((const DateADT *) b)))
		);
}

static bool
gbt_datelt(const void *a, const void *b)
{
	return DatumGetBool(
						DirectFunctionCall2(date_lt, DateADTGetDatum(*((const DateADT *) a)), DateADTGetDatum(*((const DateADT *) b)))
		);
}



static int
gbt_datekey_cmp(const void *a, const void *b)
{
	dateKEY    *ia = (dateKEY *) (((const Nsrt *) a)->t);
	dateKEY    *ib = (dateKEY *) (((const Nsrt *) b)->t);
	int			res;

	res = DatumGetInt32(DirectFunctionCall2(date_cmp, DateADTGetDatum(ia->lower), DateADTGetDatum(ib->lower)));
	if (res == 0)
		return DatumGetInt32(DirectFunctionCall2(date_cmp, DateADTGetDatum(ia->upper), DateADTGetDatum(ib->upper)));

	return res;
}

static float8
gdb_date_dist(const void *a, const void *b)
{
	/* we assume the difference can't overflow */
	Datum		diff = DirectFunctionCall2(date_mi,
									 DateADTGetDatum(*((const DateADT *) a)),
									DateADTGetDatum(*((const DateADT *) b)));

	return (float8) Abs(DatumGetInt32(diff));
}


static const gbtree_ninfo tinfo =
{
	gbt_t_date,
	sizeof(DateADT),
	8,							/* sizeof(gbtreekey8) */
	gbt_dategt,
	gbt_datege,
	gbt_dateeq,
	gbt_datele,
	gbt_datelt,
	gbt_datekey_cmp,
	gdb_date_dist
};


MDB_FUNCTION_INFO_V1(date_dist);
Datum
date_dist(MDB_FUNCTION_ARGS)
{
	/* we assume the difference can't overflow */
	Datum		diff = DirectFunctionCall2(date_mi,
										   MDB_GETARG_DATUM(0),
										   MDB_GETARG_DATUM(1));

	MDB_RETURN_INT32(Abs(DatumGetInt32(diff)));
}


/**************************************************
 * date ops
 **************************************************/



Datum
gbt_date_compress(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);

	MDB_RETURN_POINTER(gbt_num_compress(entry, &tinfo));
}

Datum
gbt_date_fetch(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);

	MDB_RETURN_POINTER(gbt_num_fetch(entry, &tinfo));
}

Datum
gbt_date_consistent(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);
	DateADT		query = MDB_GETARG_DATEADT(1);
	StrategyNumber strategy = (StrategyNumber) MDB_GETARG_UINT16(2);

	/* Oid		subtype = MDB_GETARG_OID(3); */
	bool	   *recheck = (bool *) MDB_GETARG_POINTER(4);
	dateKEY    *kkk = (dateKEY *) DatumGetPointer(entry->key);
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
gbt_date_distance(MDB_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) MDB_GETARG_POINTER(0);
	DateADT		query = MDB_GETARG_DATEADT(1);

	/* Oid		subtype = MDB_GETARG_OID(3); */
	dateKEY    *kkk = (dateKEY *) DatumGetPointer(entry->key);
	GBT_NUMKEY_R key;

	key.lower = (GBT_NUMKEY *) &kkk->lower;
	key.upper = (GBT_NUMKEY *) &kkk->upper;

	MDB_RETURN_FLOAT8(
			gbt_num_distance(&key, (void *) &query, GIST_LEAF(entry), &tinfo)
		);
}


Datum
gbt_date_union(MDB_FUNCTION_ARGS)
{
	GistEntryVector *entryvec = (GistEntryVector *) MDB_GETARG_POINTER(0);
	void	   *out = palloc(sizeof(dateKEY));

	*(int *) MDB_GETARG_POINTER(1) = sizeof(dateKEY);
	MDB_RETURN_POINTER(gbt_num_union((void *) out, entryvec, &tinfo));
}


Datum
gbt_date_penalty(MDB_FUNCTION_ARGS)
{
	dateKEY    *origentry = (dateKEY *) DatumGetPointer(((GISTENTRY *) MDB_GETARG_POINTER(0))->key);
	dateKEY    *newentry = (dateKEY *) DatumGetPointer(((GISTENTRY *) MDB_GETARG_POINTER(1))->key);
	float	   *result = (float *) MDB_GETARG_POINTER(2);
	int32		diff,
				res;

	diff = DatumGetInt32(DirectFunctionCall2(
											 date_mi,
											 DateADTGetDatum(newentry->upper),
										 DateADTGetDatum(origentry->upper)));

	res = Max(diff, 0);

	diff = DatumGetInt32(DirectFunctionCall2(
											 date_mi,
										   DateADTGetDatum(origentry->lower),
										  DateADTGetDatum(newentry->lower)));

	res += Max(diff, 0);

	*result = 0.0;

	if (res > 0)
	{
		diff = DatumGetInt32(DirectFunctionCall2(
												 date_mi,
										   DateADTGetDatum(origentry->upper),
										 DateADTGetDatum(origentry->lower)));
		*result += FLT_MIN;
		*result += (float) (res / ((double) (res + diff)));
		*result *= (FLT_MAX / (((GISTENTRY *) MDB_GETARG_POINTER(0))->rel->rd_att->natts + 1));
	}

	MDB_RETURN_POINTER(result);
}


Datum
gbt_date_picksplit(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_POINTER(gbt_num_picksplit(
									(GistEntryVector *) MDB_GETARG_POINTER(0),
									  (GIST_SPLITVEC *) MDB_GETARG_POINTER(1),
										&tinfo
										));
}

Datum
gbt_date_same(MDB_FUNCTION_ARGS)
{
	dateKEY    *b1 = (dateKEY *) MDB_GETARG_POINTER(0);
	dateKEY    *b2 = (dateKEY *) MDB_GETARG_POINTER(1);
	bool	   *result = (bool *) MDB_GETARG_POINTER(2);

	*result = gbt_num_same((void *) b1, (void *) b2, &tinfo);
	MDB_RETURN_POINTER(result);
}
