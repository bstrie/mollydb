/*-------------------------------------------------------------------------
 *
 * date.h
 *	  Definitions for the SQL "date" and "time" types.
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/date.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef DATE_H
#define DATE_H

#include <math.h>

#include "fmgr.h"


typedef int32 DateADT;

#ifdef HAVE_INT64_TIMESTAMP
typedef int64 TimeADT;
#else
typedef float8 TimeADT;
#endif

typedef struct
{
	TimeADT		time;			/* all time units other than months and years */
	int32		zone;			/* numeric time zone, in seconds */
} TimeTzADT;

/*
 * Infinity and minus infinity must be the max and min values of DateADT.
 */
#define DATEVAL_NOBEGIN		((DateADT) MDB_INT32_MIN)
#define DATEVAL_NOEND		((DateADT) MDB_INT32_MAX)

#define DATE_NOBEGIN(j)		((j) = DATEVAL_NOBEGIN)
#define DATE_IS_NOBEGIN(j)	((j) == DATEVAL_NOBEGIN)
#define DATE_NOEND(j)		((j) = DATEVAL_NOEND)
#define DATE_IS_NOEND(j)	((j) == DATEVAL_NOEND)
#define DATE_NOT_FINITE(j)	(DATE_IS_NOBEGIN(j) || DATE_IS_NOEND(j))

/*
 * Macros for fmgr-callable functions.
 *
 * For TimeADT, we make use of the same support routines as for float8 or int64.
 * Therefore TimeADT is pass-by-reference if and only if float8 or int64 is!
 */
#ifdef HAVE_INT64_TIMESTAMP

#define MAX_TIME_PRECISION 6

#define DatumGetDateADT(X)	  ((DateADT) DatumGetInt32(X))
#define DatumGetTimeADT(X)	  ((TimeADT) DatumGetInt64(X))
#define DatumGetTimeTzADTP(X) ((TimeTzADT *) DatumGetPointer(X))

#define DateADTGetDatum(X)	  Int32GetDatum(X)
#define TimeADTGetDatum(X)	  Int64GetDatum(X)
#define TimeTzADTPGetDatum(X) PointerGetDatum(X)
#else							/* !HAVE_INT64_TIMESTAMP */

#define MAX_TIME_PRECISION 10

/* round off to MAX_TIME_PRECISION decimal places */
#define TIME_PREC_INV 10000000000.0
#define TIMEROUND(j) (rint(((double) (j)) * TIME_PREC_INV) / TIME_PREC_INV)

#define DatumGetDateADT(X)	  ((DateADT) DatumGetInt32(X))
#define DatumGetTimeADT(X)	  ((TimeADT) DatumGetFloat8(X))
#define DatumGetTimeTzADTP(X) ((TimeTzADT *) DatumGetPointer(X))

#define DateADTGetDatum(X)	  Int32GetDatum(X)
#define TimeADTGetDatum(X)	  Float8GetDatum(X)
#define TimeTzADTPGetDatum(X) PointerGetDatum(X)
#endif   /* HAVE_INT64_TIMESTAMP */

#define MDB_GETARG_DATEADT(n)	 DatumGetDateADT(MDB_GETARG_DATUM(n))
#define MDB_GETARG_TIMEADT(n)	 DatumGetTimeADT(MDB_GETARG_DATUM(n))
#define MDB_GETARG_TIMETZADT_P(n) DatumGetTimeTzADTP(MDB_GETARG_DATUM(n))

#define MDB_RETURN_DATEADT(x)	 return DateADTGetDatum(x)
#define MDB_RETURN_TIMEADT(x)	 return TimeADTGetDatum(x)
#define MDB_RETURN_TIMETZADT_P(x) return TimeTzADTPGetDatum(x)


/* date.c */
extern double date2timestamp_no_overflow(DateADT dateVal);
extern void EncodeSpecialDate(DateADT dt, char *str);

extern Datum date_in(MDB_FUNCTION_ARGS);
extern Datum date_out(MDB_FUNCTION_ARGS);
extern Datum date_recv(MDB_FUNCTION_ARGS);
extern Datum date_send(MDB_FUNCTION_ARGS);
extern Datum make_date(MDB_FUNCTION_ARGS);
extern Datum date_eq(MDB_FUNCTION_ARGS);
extern Datum date_ne(MDB_FUNCTION_ARGS);
extern Datum date_lt(MDB_FUNCTION_ARGS);
extern Datum date_le(MDB_FUNCTION_ARGS);
extern Datum date_gt(MDB_FUNCTION_ARGS);
extern Datum date_ge(MDB_FUNCTION_ARGS);
extern Datum date_cmp(MDB_FUNCTION_ARGS);
extern Datum date_sortsupport(MDB_FUNCTION_ARGS);
extern Datum date_finite(MDB_FUNCTION_ARGS);
extern Datum date_larger(MDB_FUNCTION_ARGS);
extern Datum date_smaller(MDB_FUNCTION_ARGS);
extern Datum date_mi(MDB_FUNCTION_ARGS);
extern Datum date_pli(MDB_FUNCTION_ARGS);
extern Datum date_mii(MDB_FUNCTION_ARGS);
extern Datum date_eq_timestamp(MDB_FUNCTION_ARGS);
extern Datum date_ne_timestamp(MDB_FUNCTION_ARGS);
extern Datum date_lt_timestamp(MDB_FUNCTION_ARGS);
extern Datum date_le_timestamp(MDB_FUNCTION_ARGS);
extern Datum date_gt_timestamp(MDB_FUNCTION_ARGS);
extern Datum date_ge_timestamp(MDB_FUNCTION_ARGS);
extern Datum date_cmp_timestamp(MDB_FUNCTION_ARGS);
extern Datum date_eq_timestamptz(MDB_FUNCTION_ARGS);
extern Datum date_ne_timestamptz(MDB_FUNCTION_ARGS);
extern Datum date_lt_timestamptz(MDB_FUNCTION_ARGS);
extern Datum date_le_timestamptz(MDB_FUNCTION_ARGS);
extern Datum date_gt_timestamptz(MDB_FUNCTION_ARGS);
extern Datum date_ge_timestamptz(MDB_FUNCTION_ARGS);
extern Datum date_cmp_timestamptz(MDB_FUNCTION_ARGS);
extern Datum timestamp_eq_date(MDB_FUNCTION_ARGS);
extern Datum timestamp_ne_date(MDB_FUNCTION_ARGS);
extern Datum timestamp_lt_date(MDB_FUNCTION_ARGS);
extern Datum timestamp_le_date(MDB_FUNCTION_ARGS);
extern Datum timestamp_gt_date(MDB_FUNCTION_ARGS);
extern Datum timestamp_ge_date(MDB_FUNCTION_ARGS);
extern Datum timestamp_cmp_date(MDB_FUNCTION_ARGS);
extern Datum timestamptz_eq_date(MDB_FUNCTION_ARGS);
extern Datum timestamptz_ne_date(MDB_FUNCTION_ARGS);
extern Datum timestamptz_lt_date(MDB_FUNCTION_ARGS);
extern Datum timestamptz_le_date(MDB_FUNCTION_ARGS);
extern Datum timestamptz_gt_date(MDB_FUNCTION_ARGS);
extern Datum timestamptz_ge_date(MDB_FUNCTION_ARGS);
extern Datum timestamptz_cmp_date(MDB_FUNCTION_ARGS);
extern Datum date_pl_interval(MDB_FUNCTION_ARGS);
extern Datum date_mi_interval(MDB_FUNCTION_ARGS);
extern Datum date_timestamp(MDB_FUNCTION_ARGS);
extern Datum timestamp_date(MDB_FUNCTION_ARGS);
extern Datum date_timestamptz(MDB_FUNCTION_ARGS);
extern Datum timestamptz_date(MDB_FUNCTION_ARGS);
extern Datum datetime_timestamp(MDB_FUNCTION_ARGS);
extern Datum abstime_date(MDB_FUNCTION_ARGS);

extern Datum time_in(MDB_FUNCTION_ARGS);
extern Datum time_out(MDB_FUNCTION_ARGS);
extern Datum time_recv(MDB_FUNCTION_ARGS);
extern Datum time_send(MDB_FUNCTION_ARGS);
extern Datum timetypmodin(MDB_FUNCTION_ARGS);
extern Datum timetypmodout(MDB_FUNCTION_ARGS);
extern Datum make_time(MDB_FUNCTION_ARGS);
extern Datum time_transform(MDB_FUNCTION_ARGS);
extern Datum time_scale(MDB_FUNCTION_ARGS);
extern Datum time_eq(MDB_FUNCTION_ARGS);
extern Datum time_ne(MDB_FUNCTION_ARGS);
extern Datum time_lt(MDB_FUNCTION_ARGS);
extern Datum time_le(MDB_FUNCTION_ARGS);
extern Datum time_gt(MDB_FUNCTION_ARGS);
extern Datum time_ge(MDB_FUNCTION_ARGS);
extern Datum time_cmp(MDB_FUNCTION_ARGS);
extern Datum time_hash(MDB_FUNCTION_ARGS);
extern Datum overlaps_time(MDB_FUNCTION_ARGS);
extern Datum time_larger(MDB_FUNCTION_ARGS);
extern Datum time_smaller(MDB_FUNCTION_ARGS);
extern Datum time_mi_time(MDB_FUNCTION_ARGS);
extern Datum timestamp_time(MDB_FUNCTION_ARGS);
extern Datum timestamptz_time(MDB_FUNCTION_ARGS);
extern Datum time_interval(MDB_FUNCTION_ARGS);
extern Datum interval_time(MDB_FUNCTION_ARGS);
extern Datum time_pl_interval(MDB_FUNCTION_ARGS);
extern Datum time_mi_interval(MDB_FUNCTION_ARGS);
extern Datum time_part(MDB_FUNCTION_ARGS);

extern Datum timetz_in(MDB_FUNCTION_ARGS);
extern Datum timetz_out(MDB_FUNCTION_ARGS);
extern Datum timetz_recv(MDB_FUNCTION_ARGS);
extern Datum timetz_send(MDB_FUNCTION_ARGS);
extern Datum timetztypmodin(MDB_FUNCTION_ARGS);
extern Datum timetztypmodout(MDB_FUNCTION_ARGS);
extern Datum timetz_scale(MDB_FUNCTION_ARGS);
extern Datum timetz_eq(MDB_FUNCTION_ARGS);
extern Datum timetz_ne(MDB_FUNCTION_ARGS);
extern Datum timetz_lt(MDB_FUNCTION_ARGS);
extern Datum timetz_le(MDB_FUNCTION_ARGS);
extern Datum timetz_gt(MDB_FUNCTION_ARGS);
extern Datum timetz_ge(MDB_FUNCTION_ARGS);
extern Datum timetz_cmp(MDB_FUNCTION_ARGS);
extern Datum timetz_hash(MDB_FUNCTION_ARGS);
extern Datum overlaps_timetz(MDB_FUNCTION_ARGS);
extern Datum timetz_larger(MDB_FUNCTION_ARGS);
extern Datum timetz_smaller(MDB_FUNCTION_ARGS);
extern Datum timetz_time(MDB_FUNCTION_ARGS);
extern Datum time_timetz(MDB_FUNCTION_ARGS);
extern Datum timestamptz_timetz(MDB_FUNCTION_ARGS);
extern Datum datetimetz_timestamptz(MDB_FUNCTION_ARGS);
extern Datum timetz_part(MDB_FUNCTION_ARGS);
extern Datum timetz_zone(MDB_FUNCTION_ARGS);
extern Datum timetz_izone(MDB_FUNCTION_ARGS);
extern Datum timetz_pl_interval(MDB_FUNCTION_ARGS);
extern Datum timetz_mi_interval(MDB_FUNCTION_ARGS);

#endif   /* DATE_H */
