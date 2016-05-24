/*-------------------------------------------------------------------------
 *
 * nabstime.h
 *	  Definitions for the "new" abstime code.
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/nabstime.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NABSTIME_H
#define NABSTIME_H

#include <limits.h>

#include "fmgr.h"
#include "pgtime.h"


/* ----------------------------------------------------------------
 *
 *				time types + support macros
 *
 * ----------------------------------------------------------------
 */

/*
 * Although time_t generally is a long int on 64 bit systems, these two
 * types must be 4 bytes, because that's what mdb_type.h assumes. They
 * should be yanked (long) before 2038 and be replaced by timestamp and
 * interval.
 */
typedef int32 AbsoluteTime;
typedef int32 RelativeTime;

typedef struct
{
	int32		status;
	AbsoluteTime data[2];
} TimeIntervalData;

typedef TimeIntervalData *TimeInterval;

/*
 * Macros for fmgr-callable functions.
 */
#define DatumGetAbsoluteTime(X)  ((AbsoluteTime) DatumGetInt32(X))
#define DatumGetRelativeTime(X)  ((RelativeTime) DatumGetInt32(X))
#define DatumGetTimeInterval(X)  ((TimeInterval) DatumGetPointer(X))

#define AbsoluteTimeGetDatum(X)  Int32GetDatum(X)
#define RelativeTimeGetDatum(X)  Int32GetDatum(X)
#define TimeIntervalGetDatum(X)  PointerGetDatum(X)

#define MDB_GETARG_ABSOLUTETIME(n)  DatumGetAbsoluteTime(MDB_GETARG_DATUM(n))
#define MDB_GETARG_RELATIVETIME(n)  DatumGetRelativeTime(MDB_GETARG_DATUM(n))
#define MDB_GETARG_TIMEINTERVAL(n)  DatumGetTimeInterval(MDB_GETARG_DATUM(n))

#define MDB_RETURN_ABSOLUTETIME(x)  return AbsoluteTimeGetDatum(x)
#define MDB_RETURN_RELATIVETIME(x)  return RelativeTimeGetDatum(x)
#define MDB_RETURN_TIMEINTERVAL(x)  return TimeIntervalGetDatum(x)

/*
 * Reserved values
 * Epoch is Unix system time zero, but needs to be kept as a reserved
 *	value rather than converting to time since timezone calculations
 *	might move it away from 1970-01-01 00:00:00Z - tgl 97/02/20
 *
 * Pre-v6.1 code had large decimal numbers for reserved values.
 * These were chosen as special 32-bit bit patterns,
 *	so redefine them explicitly using these bit patterns. - tgl 97/02/24
 */
#define INVALID_ABSTIME ((AbsoluteTime) 0x7FFFFFFE)		/* 2147483647 (2^31 - 1) */
#define NOEND_ABSTIME	((AbsoluteTime) 0x7FFFFFFC)		/* 2147483645 (2^31 - 3) */
#define NOSTART_ABSTIME ((AbsoluteTime) INT_MIN)		/* -2147483648 */

#define INVALID_RELTIME ((RelativeTime) 0x7FFFFFFE)		/* 2147483647 (2^31 - 1) */

#define AbsoluteTimeIsValid(time) \
	((bool) ((time) != INVALID_ABSTIME))

/*
 * Because NOSTART_ABSTIME is defined as INT_MIN, there can't be any
 * AbsoluteTime values less than it.  Therefore, we can code the test
 * "time > NOSTART_ABSTIME" as "time != NOSTART_ABSTIME", which avoids
 * compiler bugs on some platforms.  --- tgl & az, 11/2000
 */
#define AbsoluteTimeIsReal(time) \
	((bool) (((AbsoluteTime) (time)) < NOEND_ABSTIME && \
			  ((AbsoluteTime) (time)) != NOSTART_ABSTIME))

#define RelativeTimeIsValid(time) \
	((bool) (((RelativeTime) (time)) != INVALID_RELTIME))


/*
 * nabstime.c prototypes
 */
extern Datum abstimein(MDB_FUNCTION_ARGS);
extern Datum abstimeout(MDB_FUNCTION_ARGS);
extern Datum abstimerecv(MDB_FUNCTION_ARGS);
extern Datum abstimesend(MDB_FUNCTION_ARGS);

extern Datum abstimeeq(MDB_FUNCTION_ARGS);
extern Datum abstimene(MDB_FUNCTION_ARGS);
extern Datum abstimelt(MDB_FUNCTION_ARGS);
extern Datum abstimegt(MDB_FUNCTION_ARGS);
extern Datum abstimele(MDB_FUNCTION_ARGS);
extern Datum abstimege(MDB_FUNCTION_ARGS);
extern Datum abstime_finite(MDB_FUNCTION_ARGS);

extern Datum timestamp_abstime(MDB_FUNCTION_ARGS);
extern Datum abstime_timestamp(MDB_FUNCTION_ARGS);
extern Datum timestamptz_abstime(MDB_FUNCTION_ARGS);
extern Datum abstime_timestamptz(MDB_FUNCTION_ARGS);

extern Datum reltimein(MDB_FUNCTION_ARGS);
extern Datum reltimeout(MDB_FUNCTION_ARGS);
extern Datum reltimerecv(MDB_FUNCTION_ARGS);
extern Datum reltimesend(MDB_FUNCTION_ARGS);
extern Datum tintervalin(MDB_FUNCTION_ARGS);
extern Datum tintervalout(MDB_FUNCTION_ARGS);
extern Datum tintervalrecv(MDB_FUNCTION_ARGS);
extern Datum tintervalsend(MDB_FUNCTION_ARGS);
extern Datum interval_reltime(MDB_FUNCTION_ARGS);
extern Datum reltime_interval(MDB_FUNCTION_ARGS);
extern Datum mktinterval(MDB_FUNCTION_ARGS);
extern Datum timepl(MDB_FUNCTION_ARGS);
extern Datum timemi(MDB_FUNCTION_ARGS);

extern Datum intinterval(MDB_FUNCTION_ARGS);
extern Datum tintervalrel(MDB_FUNCTION_ARGS);
extern Datum timenow(MDB_FUNCTION_ARGS);
extern Datum reltimeeq(MDB_FUNCTION_ARGS);
extern Datum reltimene(MDB_FUNCTION_ARGS);
extern Datum reltimelt(MDB_FUNCTION_ARGS);
extern Datum reltimegt(MDB_FUNCTION_ARGS);
extern Datum reltimele(MDB_FUNCTION_ARGS);
extern Datum reltimege(MDB_FUNCTION_ARGS);
extern Datum tintervalsame(MDB_FUNCTION_ARGS);
extern Datum tintervaleq(MDB_FUNCTION_ARGS);
extern Datum tintervalne(MDB_FUNCTION_ARGS);
extern Datum tintervallt(MDB_FUNCTION_ARGS);
extern Datum tintervalgt(MDB_FUNCTION_ARGS);
extern Datum tintervalle(MDB_FUNCTION_ARGS);
extern Datum tintervalge(MDB_FUNCTION_ARGS);
extern Datum tintervalleneq(MDB_FUNCTION_ARGS);
extern Datum tintervallenne(MDB_FUNCTION_ARGS);
extern Datum tintervallenlt(MDB_FUNCTION_ARGS);
extern Datum tintervallengt(MDB_FUNCTION_ARGS);
extern Datum tintervallenle(MDB_FUNCTION_ARGS);
extern Datum tintervallenge(MDB_FUNCTION_ARGS);
extern Datum tintervalct(MDB_FUNCTION_ARGS);
extern Datum tintervalov(MDB_FUNCTION_ARGS);
extern Datum tintervalstart(MDB_FUNCTION_ARGS);
extern Datum tintervalend(MDB_FUNCTION_ARGS);
extern Datum timeofday(MDB_FUNCTION_ARGS);

/* non-fmgr-callable support routines */
extern AbsoluteTime GetCurrentAbsoluteTime(void);
extern void abstime2tm(AbsoluteTime time, int *tzp, struct mdb_tm * tm, char **tzn);

#endif   /* NABSTIME_H */
