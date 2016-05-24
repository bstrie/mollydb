/*-------------------------------------------------------------------------
 *
 * timestamp.h
 *	  Definitions for the SQL "timestamp" and "interval" types.
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/timestamp.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include "datatype/timestamp.h"
#include "fmgr.h"
#include "pgtime.h"


/*
 * Macros for fmgr-callable functions.
 *
 * For Timestamp, we make use of the same support routines as for int64
 * or float8.  Therefore Timestamp is pass-by-reference if and only if
 * int64 or float8 is!
 */
#ifdef HAVE_INT64_TIMESTAMP

#define DatumGetTimestamp(X)  ((Timestamp) DatumGetInt64(X))
#define DatumGetTimestampTz(X)	((TimestampTz) DatumGetInt64(X))
#define DatumGetIntervalP(X)  ((Interval *) DatumGetPointer(X))

#define TimestampGetDatum(X) Int64GetDatum(X)
#define TimestampTzGetDatum(X) Int64GetDatum(X)
#define IntervalPGetDatum(X) PointerGetDatum(X)

#define MDB_GETARG_TIMESTAMP(n) DatumGetTimestamp(MDB_GETARG_DATUM(n))
#define MDB_GETARG_TIMESTAMPTZ(n) DatumGetTimestampTz(MDB_GETARG_DATUM(n))
#define MDB_GETARG_INTERVAL_P(n) DatumGetIntervalP(MDB_GETARG_DATUM(n))

#define MDB_RETURN_TIMESTAMP(x) return TimestampGetDatum(x)
#define MDB_RETURN_TIMESTAMPTZ(x) return TimestampTzGetDatum(x)
#define MDB_RETURN_INTERVAL_P(x) return IntervalPGetDatum(x)
#else							/* !HAVE_INT64_TIMESTAMP */

#define DatumGetTimestamp(X)  ((Timestamp) DatumGetFloat8(X))
#define DatumGetTimestampTz(X)	((TimestampTz) DatumGetFloat8(X))
#define DatumGetIntervalP(X)  ((Interval *) DatumGetPointer(X))

#define TimestampGetDatum(X) Float8GetDatum(X)
#define TimestampTzGetDatum(X) Float8GetDatum(X)
#define IntervalPGetDatum(X) PointerGetDatum(X)

#define MDB_GETARG_TIMESTAMP(n) DatumGetTimestamp(MDB_GETARG_DATUM(n))
#define MDB_GETARG_TIMESTAMPTZ(n) DatumGetTimestampTz(MDB_GETARG_DATUM(n))
#define MDB_GETARG_INTERVAL_P(n) DatumGetIntervalP(MDB_GETARG_DATUM(n))

#define MDB_RETURN_TIMESTAMP(x) return TimestampGetDatum(x)
#define MDB_RETURN_TIMESTAMPTZ(x) return TimestampTzGetDatum(x)
#define MDB_RETURN_INTERVAL_P(x) return IntervalPGetDatum(x)
#endif   /* HAVE_INT64_TIMESTAMP */


#define TIMESTAMP_MASK(b) (1 << (b))
#define INTERVAL_MASK(b) (1 << (b))

/* Macros to handle packing and unpacking the typmod field for intervals */
#define INTERVAL_FULL_RANGE (0x7FFF)
#define INTERVAL_RANGE_MASK (0x7FFF)
#define INTERVAL_FULL_PRECISION (0xFFFF)
#define INTERVAL_PRECISION_MASK (0xFFFF)
#define INTERVAL_TYPMOD(p,r) ((((r) & INTERVAL_RANGE_MASK) << 16) | ((p) & INTERVAL_PRECISION_MASK))
#define INTERVAL_PRECISION(t) ((t) & INTERVAL_PRECISION_MASK)
#define INTERVAL_RANGE(t) (((t) >> 16) & INTERVAL_RANGE_MASK)

#ifdef HAVE_INT64_TIMESTAMP
#define TimestampTzPlusMilliseconds(tz,ms) ((tz) + ((ms) * (int64) 1000))
#else
#define TimestampTzPlusMilliseconds(tz,ms) ((tz) + ((ms) / 1000.0))
#endif


/* Set at postmaster start */
extern TimestampTz PgStartTime;

/* Set at configuration reload */
extern TimestampTz PgReloadTime;


/*
 * timestamp.c prototypes
 */

extern Datum timestamp_in(MDB_FUNCTION_ARGS);
extern Datum timestamp_out(MDB_FUNCTION_ARGS);
extern Datum timestamp_recv(MDB_FUNCTION_ARGS);
extern Datum timestamp_send(MDB_FUNCTION_ARGS);
extern Datum timestamptypmodin(MDB_FUNCTION_ARGS);
extern Datum timestamptypmodout(MDB_FUNCTION_ARGS);
extern Datum timestamp_transform(MDB_FUNCTION_ARGS);
extern Datum timestamp_scale(MDB_FUNCTION_ARGS);
extern Datum timestamp_eq(MDB_FUNCTION_ARGS);
extern Datum timestamp_ne(MDB_FUNCTION_ARGS);
extern Datum timestamp_lt(MDB_FUNCTION_ARGS);
extern Datum timestamp_le(MDB_FUNCTION_ARGS);
extern Datum timestamp_ge(MDB_FUNCTION_ARGS);
extern Datum timestamp_gt(MDB_FUNCTION_ARGS);
extern Datum timestamp_finite(MDB_FUNCTION_ARGS);
extern Datum timestamp_cmp(MDB_FUNCTION_ARGS);
extern Datum timestamp_sortsupport(MDB_FUNCTION_ARGS);
extern Datum timestamp_hash(MDB_FUNCTION_ARGS);
extern Datum timestamp_smaller(MDB_FUNCTION_ARGS);
extern Datum timestamp_larger(MDB_FUNCTION_ARGS);

extern Datum timestamp_eq_timestamptz(MDB_FUNCTION_ARGS);
extern Datum timestamp_ne_timestamptz(MDB_FUNCTION_ARGS);
extern Datum timestamp_lt_timestamptz(MDB_FUNCTION_ARGS);
extern Datum timestamp_le_timestamptz(MDB_FUNCTION_ARGS);
extern Datum timestamp_gt_timestamptz(MDB_FUNCTION_ARGS);
extern Datum timestamp_ge_timestamptz(MDB_FUNCTION_ARGS);
extern Datum timestamp_cmp_timestamptz(MDB_FUNCTION_ARGS);

extern Datum make_timestamp(MDB_FUNCTION_ARGS);
extern Datum make_timestamptz(MDB_FUNCTION_ARGS);
extern Datum make_timestamptz_at_timezone(MDB_FUNCTION_ARGS);
extern Datum float8_timestamptz(MDB_FUNCTION_ARGS);

extern Datum timestamptz_eq_timestamp(MDB_FUNCTION_ARGS);
extern Datum timestamptz_ne_timestamp(MDB_FUNCTION_ARGS);
extern Datum timestamptz_lt_timestamp(MDB_FUNCTION_ARGS);
extern Datum timestamptz_le_timestamp(MDB_FUNCTION_ARGS);
extern Datum timestamptz_gt_timestamp(MDB_FUNCTION_ARGS);
extern Datum timestamptz_ge_timestamp(MDB_FUNCTION_ARGS);
extern Datum timestamptz_cmp_timestamp(MDB_FUNCTION_ARGS);

extern Datum interval_in(MDB_FUNCTION_ARGS);
extern Datum interval_out(MDB_FUNCTION_ARGS);
extern Datum interval_recv(MDB_FUNCTION_ARGS);
extern Datum interval_send(MDB_FUNCTION_ARGS);
extern Datum intervaltypmodin(MDB_FUNCTION_ARGS);
extern Datum intervaltypmodout(MDB_FUNCTION_ARGS);
extern Datum interval_transform(MDB_FUNCTION_ARGS);
extern Datum interval_scale(MDB_FUNCTION_ARGS);
extern Datum interval_eq(MDB_FUNCTION_ARGS);
extern Datum interval_ne(MDB_FUNCTION_ARGS);
extern Datum interval_lt(MDB_FUNCTION_ARGS);
extern Datum interval_le(MDB_FUNCTION_ARGS);
extern Datum interval_ge(MDB_FUNCTION_ARGS);
extern Datum interval_gt(MDB_FUNCTION_ARGS);
extern Datum interval_finite(MDB_FUNCTION_ARGS);
extern Datum interval_cmp(MDB_FUNCTION_ARGS);
extern Datum interval_hash(MDB_FUNCTION_ARGS);
extern Datum interval_smaller(MDB_FUNCTION_ARGS);
extern Datum interval_larger(MDB_FUNCTION_ARGS);
extern Datum interval_justify_interval(MDB_FUNCTION_ARGS);
extern Datum interval_justify_hours(MDB_FUNCTION_ARGS);
extern Datum interval_justify_days(MDB_FUNCTION_ARGS);
extern Datum make_interval(MDB_FUNCTION_ARGS);

extern Datum timestamp_trunc(MDB_FUNCTION_ARGS);
extern Datum interval_trunc(MDB_FUNCTION_ARGS);
extern Datum timestamp_part(MDB_FUNCTION_ARGS);
extern Datum interval_part(MDB_FUNCTION_ARGS);
extern Datum timestamp_zone_transform(MDB_FUNCTION_ARGS);
extern Datum timestamp_zone(MDB_FUNCTION_ARGS);
extern Datum timestamp_izone_transform(MDB_FUNCTION_ARGS);
extern Datum timestamp_izone(MDB_FUNCTION_ARGS);
extern Datum timestamp_timestamptz(MDB_FUNCTION_ARGS);

extern Datum timestamptz_in(MDB_FUNCTION_ARGS);
extern Datum timestamptz_out(MDB_FUNCTION_ARGS);
extern Datum timestamptz_recv(MDB_FUNCTION_ARGS);
extern Datum timestamptz_send(MDB_FUNCTION_ARGS);
extern Datum timestamptztypmodin(MDB_FUNCTION_ARGS);
extern Datum timestamptztypmodout(MDB_FUNCTION_ARGS);
extern Datum timestamptz_scale(MDB_FUNCTION_ARGS);
extern Datum timestamptz_timestamp(MDB_FUNCTION_ARGS);
extern Datum timestamptz_zone(MDB_FUNCTION_ARGS);
extern Datum timestamptz_izone(MDB_FUNCTION_ARGS);
extern Datum timestamptz_timestamptz(MDB_FUNCTION_ARGS);

extern Datum interval_um(MDB_FUNCTION_ARGS);
extern Datum interval_pl(MDB_FUNCTION_ARGS);
extern Datum interval_mi(MDB_FUNCTION_ARGS);
extern Datum interval_mul(MDB_FUNCTION_ARGS);
extern Datum mul_d_interval(MDB_FUNCTION_ARGS);
extern Datum interval_div(MDB_FUNCTION_ARGS);
extern Datum interval_accum(MDB_FUNCTION_ARGS);
extern Datum interval_combine(MDB_FUNCTION_ARGS);
extern Datum interval_accum_inv(MDB_FUNCTION_ARGS);
extern Datum interval_avg(MDB_FUNCTION_ARGS);

extern Datum timestamp_mi(MDB_FUNCTION_ARGS);
extern Datum timestamp_pl_interval(MDB_FUNCTION_ARGS);
extern Datum timestamp_mi_interval(MDB_FUNCTION_ARGS);
extern Datum timestamp_age(MDB_FUNCTION_ARGS);
extern Datum overlaps_timestamp(MDB_FUNCTION_ARGS);

extern Datum timestamptz_pl_interval(MDB_FUNCTION_ARGS);
extern Datum timestamptz_mi_interval(MDB_FUNCTION_ARGS);
extern Datum timestamptz_age(MDB_FUNCTION_ARGS);
extern Datum timestamptz_trunc(MDB_FUNCTION_ARGS);
extern Datum timestamptz_part(MDB_FUNCTION_ARGS);

extern Datum now(MDB_FUNCTION_ARGS);
extern Datum statement_timestamp(MDB_FUNCTION_ARGS);
extern Datum clock_timestamp(MDB_FUNCTION_ARGS);

extern Datum mdb_postmaster_start_time(MDB_FUNCTION_ARGS);
extern Datum mdb_conf_load_time(MDB_FUNCTION_ARGS);

extern Datum generate_series_timestamp(MDB_FUNCTION_ARGS);
extern Datum generate_series_timestamptz(MDB_FUNCTION_ARGS);

/* Internal routines (not fmgr-callable) */

extern TimestampTz GetCurrentTimestamp(void);
extern void TimestampDifference(TimestampTz start_time, TimestampTz stop_time,
					long *secs, int *microsecs);
extern bool TimestampDifferenceExceeds(TimestampTz start_time,
						   TimestampTz stop_time,
						   int msec);

/*
 * Prototypes for functions to deal with integer timestamps, when the native
 * format is float timestamps.
 */
#ifndef HAVE_INT64_TIMESTAMP
extern int64 GetCurrentIntegerTimestamp(void);
extern TimestampTz IntegerTimestampToTimestampTz(int64 timestamp);
#else
#define GetCurrentIntegerTimestamp()	GetCurrentTimestamp()
#define IntegerTimestampToTimestampTz(timestamp) (timestamp)
#endif

extern TimestampTz time_t_to_timestamptz(mdb_time_t tm);
extern mdb_time_t timestamptz_to_time_t(TimestampTz t);

extern const char *timestamptz_to_str(TimestampTz t);

extern int	tm2timestamp(struct mdb_tm * tm, fsec_t fsec, int *tzp, Timestamp *dt);
extern int timestamp2tm(Timestamp dt, int *tzp, struct mdb_tm * tm,
			 fsec_t *fsec, const char **tzn, mdb_tz *attimezone);
extern void dt2time(Timestamp dt, int *hour, int *min, int *sec, fsec_t *fsec);

extern int	interval2tm(Interval span, struct mdb_tm * tm, fsec_t *fsec);
extern int	tm2interval(struct mdb_tm * tm, fsec_t fsec, Interval *span);

extern Timestamp SetEpochTimestamp(void);
extern void GetEpochTime(struct mdb_tm * tm);

extern int	timestamp_cmp_internal(Timestamp dt1, Timestamp dt2);

/* timestamp comparison works for timestamptz also */
#define timestamptz_cmp_internal(dt1,dt2)	timestamp_cmp_internal(dt1, dt2)

extern int	isoweek2j(int year, int week);
extern void isoweek2date(int woy, int *year, int *mon, int *mday);
extern void isoweekdate2date(int isoweek, int wday, int *year, int *mon, int *mday);
extern int	date2isoweek(int year, int mon, int mday);
extern int	date2isoyear(int year, int mon, int mday);
extern int	date2isoyearday(int year, int mon, int mday);

#endif   /* TIMESTAMP_H */
