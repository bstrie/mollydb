/*-------------------------------------------------------------------------
 *
 * int8.c
 *	  Internal 64-bit integer operations
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *	  src/backend/utils/adt/int8.c
 *
 *-------------------------------------------------------------------------
 */
#include "mollydb.h"

#include <ctype.h>
#include <limits.h>
#include <math.h>

#include "funcapi.h"
#include "libpq/pqformat.h"
#include "utils/int8.h"
#include "utils/builtins.h"


#define MAXINT8LEN		25

#define SAMESIGN(a,b)	(((a) < 0) == ((b) < 0))

typedef struct
{
	int64		current;
	int64		finish;
	int64		step;
} generate_series_fctx;


/***********************************************************************
 **
 **		Routines for 64-bit integers.
 **
 ***********************************************************************/

/*----------------------------------------------------------
 * Formatting and conversion routines.
 *---------------------------------------------------------*/

/*
 * scanint8 --- try to parse a string into an int8.
 *
 * If errorOK is false, ereport a useful error message if the string is bad.
 * If errorOK is true, just return "false" for bad input.
 */
bool
scanint8(const char *str, bool errorOK, int64 *result)
{
	const char *ptr = str;
	int64		tmp = 0;
	int			sign = 1;

	/*
	 * Do our own scan, rather than relying on sscanf which might be broken
	 * for long long.
	 */

	/* skip leading spaces */
	while (*ptr && isspace((unsigned char) *ptr))
		ptr++;

	/* handle sign */
	if (*ptr == '-')
	{
		ptr++;

		/*
		 * Do an explicit check for INT64_MIN.  Ugly though this is, it's
		 * cleaner than trying to get the loop below to handle it portably.
		 */
		if (strncmp(ptr, "9223372036854775808", 19) == 0)
		{
			tmp = MDB_INT64_MIN;
			ptr += 19;
			goto gotdigits;
		}
		sign = -1;
	}
	else if (*ptr == '+')
		ptr++;

	/* require at least one digit */
	if (!isdigit((unsigned char) *ptr))
	{
		if (errorOK)
			return false;
		else
			ereport(ERROR,
					(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
					 errmsg("invalid input syntax for integer: \"%s\"",
							str)));
	}

	/* process digits */
	while (*ptr && isdigit((unsigned char) *ptr))
	{
		int64		newtmp = tmp * 10 + (*ptr++ - '0');

		if ((newtmp / 10) != tmp)		/* overflow? */
		{
			if (errorOK)
				return false;
			else
				ereport(ERROR,
						(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
					   errmsg("value \"%s\" is out of range for type bigint",
							  str)));
		}
		tmp = newtmp;
	}

gotdigits:

	/* allow trailing whitespace, but not other trailing chars */
	while (*ptr != '\0' && isspace((unsigned char) *ptr))
		ptr++;

	if (*ptr != '\0')
	{
		if (errorOK)
			return false;
		else
			ereport(ERROR,
					(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
					 errmsg("invalid input syntax for integer: \"%s\"",
							str)));
	}

	*result = (sign < 0) ? -tmp : tmp;

	return true;
}

/* int8in()
 */
Datum
int8in(MDB_FUNCTION_ARGS)
{
	char	   *str = MDB_GETARG_CSTRING(0);
	int64		result;

	(void) scanint8(str, false, &result);
	MDB_RETURN_INT64(result);
}


/* int8out()
 */
Datum
int8out(MDB_FUNCTION_ARGS)
{
	int64		val = MDB_GETARG_INT64(0);
	char		buf[MAXINT8LEN + 1];
	char	   *result;

	mdb_lltoa(val, buf);
	result = pstrdup(buf);
	MDB_RETURN_CSTRING(result);
}

/*
 *		int8recv			- converts external binary format to int8
 */
Datum
int8recv(MDB_FUNCTION_ARGS)
{
	StringInfo	buf = (StringInfo) MDB_GETARG_POINTER(0);

	MDB_RETURN_INT64(pq_getmsgint64(buf));
}

/*
 *		int8send			- converts int8 to binary format
 */
Datum
int8send(MDB_FUNCTION_ARGS)
{
	int64		arg1 = MDB_GETARG_INT64(0);
	StringInfoData buf;

	pq_begintypsend(&buf);
	pq_sendint64(&buf, arg1);
	MDB_RETURN_BYTEA_P(pq_endtypsend(&buf));
}


/*----------------------------------------------------------
 *	Relational operators for int8s, including cross-data-type comparisons.
 *---------------------------------------------------------*/

/* int8relop()
 * Is val1 relop val2?
 */
Datum
int8eq(MDB_FUNCTION_ARGS)
{
	int64		val1 = MDB_GETARG_INT64(0);
	int64		val2 = MDB_GETARG_INT64(1);

	MDB_RETURN_BOOL(val1 == val2);
}

Datum
int8ne(MDB_FUNCTION_ARGS)
{
	int64		val1 = MDB_GETARG_INT64(0);
	int64		val2 = MDB_GETARG_INT64(1);

	MDB_RETURN_BOOL(val1 != val2);
}

Datum
int8lt(MDB_FUNCTION_ARGS)
{
	int64		val1 = MDB_GETARG_INT64(0);
	int64		val2 = MDB_GETARG_INT64(1);

	MDB_RETURN_BOOL(val1 < val2);
}

Datum
int8gt(MDB_FUNCTION_ARGS)
{
	int64		val1 = MDB_GETARG_INT64(0);
	int64		val2 = MDB_GETARG_INT64(1);

	MDB_RETURN_BOOL(val1 > val2);
}

Datum
int8le(MDB_FUNCTION_ARGS)
{
	int64		val1 = MDB_GETARG_INT64(0);
	int64		val2 = MDB_GETARG_INT64(1);

	MDB_RETURN_BOOL(val1 <= val2);
}

Datum
int8ge(MDB_FUNCTION_ARGS)
{
	int64		val1 = MDB_GETARG_INT64(0);
	int64		val2 = MDB_GETARG_INT64(1);

	MDB_RETURN_BOOL(val1 >= val2);
}

/* int84relop()
 * Is 64-bit val1 relop 32-bit val2?
 */
Datum
int84eq(MDB_FUNCTION_ARGS)
{
	int64		val1 = MDB_GETARG_INT64(0);
	int32		val2 = MDB_GETARG_INT32(1);

	MDB_RETURN_BOOL(val1 == val2);
}

Datum
int84ne(MDB_FUNCTION_ARGS)
{
	int64		val1 = MDB_GETARG_INT64(0);
	int32		val2 = MDB_GETARG_INT32(1);

	MDB_RETURN_BOOL(val1 != val2);
}

Datum
int84lt(MDB_FUNCTION_ARGS)
{
	int64		val1 = MDB_GETARG_INT64(0);
	int32		val2 = MDB_GETARG_INT32(1);

	MDB_RETURN_BOOL(val1 < val2);
}

Datum
int84gt(MDB_FUNCTION_ARGS)
{
	int64		val1 = MDB_GETARG_INT64(0);
	int32		val2 = MDB_GETARG_INT32(1);

	MDB_RETURN_BOOL(val1 > val2);
}

Datum
int84le(MDB_FUNCTION_ARGS)
{
	int64		val1 = MDB_GETARG_INT64(0);
	int32		val2 = MDB_GETARG_INT32(1);

	MDB_RETURN_BOOL(val1 <= val2);
}

Datum
int84ge(MDB_FUNCTION_ARGS)
{
	int64		val1 = MDB_GETARG_INT64(0);
	int32		val2 = MDB_GETARG_INT32(1);

	MDB_RETURN_BOOL(val1 >= val2);
}

/* int48relop()
 * Is 32-bit val1 relop 64-bit val2?
 */
Datum
int48eq(MDB_FUNCTION_ARGS)
{
	int32		val1 = MDB_GETARG_INT32(0);
	int64		val2 = MDB_GETARG_INT64(1);

	MDB_RETURN_BOOL(val1 == val2);
}

Datum
int48ne(MDB_FUNCTION_ARGS)
{
	int32		val1 = MDB_GETARG_INT32(0);
	int64		val2 = MDB_GETARG_INT64(1);

	MDB_RETURN_BOOL(val1 != val2);
}

Datum
int48lt(MDB_FUNCTION_ARGS)
{
	int32		val1 = MDB_GETARG_INT32(0);
	int64		val2 = MDB_GETARG_INT64(1);

	MDB_RETURN_BOOL(val1 < val2);
}

Datum
int48gt(MDB_FUNCTION_ARGS)
{
	int32		val1 = MDB_GETARG_INT32(0);
	int64		val2 = MDB_GETARG_INT64(1);

	MDB_RETURN_BOOL(val1 > val2);
}

Datum
int48le(MDB_FUNCTION_ARGS)
{
	int32		val1 = MDB_GETARG_INT32(0);
	int64		val2 = MDB_GETARG_INT64(1);

	MDB_RETURN_BOOL(val1 <= val2);
}

Datum
int48ge(MDB_FUNCTION_ARGS)
{
	int32		val1 = MDB_GETARG_INT32(0);
	int64		val2 = MDB_GETARG_INT64(1);

	MDB_RETURN_BOOL(val1 >= val2);
}

/* int82relop()
 * Is 64-bit val1 relop 16-bit val2?
 */
Datum
int82eq(MDB_FUNCTION_ARGS)
{
	int64		val1 = MDB_GETARG_INT64(0);
	int16		val2 = MDB_GETARG_INT16(1);

	MDB_RETURN_BOOL(val1 == val2);
}

Datum
int82ne(MDB_FUNCTION_ARGS)
{
	int64		val1 = MDB_GETARG_INT64(0);
	int16		val2 = MDB_GETARG_INT16(1);

	MDB_RETURN_BOOL(val1 != val2);
}

Datum
int82lt(MDB_FUNCTION_ARGS)
{
	int64		val1 = MDB_GETARG_INT64(0);
	int16		val2 = MDB_GETARG_INT16(1);

	MDB_RETURN_BOOL(val1 < val2);
}

Datum
int82gt(MDB_FUNCTION_ARGS)
{
	int64		val1 = MDB_GETARG_INT64(0);
	int16		val2 = MDB_GETARG_INT16(1);

	MDB_RETURN_BOOL(val1 > val2);
}

Datum
int82le(MDB_FUNCTION_ARGS)
{
	int64		val1 = MDB_GETARG_INT64(0);
	int16		val2 = MDB_GETARG_INT16(1);

	MDB_RETURN_BOOL(val1 <= val2);
}

Datum
int82ge(MDB_FUNCTION_ARGS)
{
	int64		val1 = MDB_GETARG_INT64(0);
	int16		val2 = MDB_GETARG_INT16(1);

	MDB_RETURN_BOOL(val1 >= val2);
}

/* int28relop()
 * Is 16-bit val1 relop 64-bit val2?
 */
Datum
int28eq(MDB_FUNCTION_ARGS)
{
	int16		val1 = MDB_GETARG_INT16(0);
	int64		val2 = MDB_GETARG_INT64(1);

	MDB_RETURN_BOOL(val1 == val2);
}

Datum
int28ne(MDB_FUNCTION_ARGS)
{
	int16		val1 = MDB_GETARG_INT16(0);
	int64		val2 = MDB_GETARG_INT64(1);

	MDB_RETURN_BOOL(val1 != val2);
}

Datum
int28lt(MDB_FUNCTION_ARGS)
{
	int16		val1 = MDB_GETARG_INT16(0);
	int64		val2 = MDB_GETARG_INT64(1);

	MDB_RETURN_BOOL(val1 < val2);
}

Datum
int28gt(MDB_FUNCTION_ARGS)
{
	int16		val1 = MDB_GETARG_INT16(0);
	int64		val2 = MDB_GETARG_INT64(1);

	MDB_RETURN_BOOL(val1 > val2);
}

Datum
int28le(MDB_FUNCTION_ARGS)
{
	int16		val1 = MDB_GETARG_INT16(0);
	int64		val2 = MDB_GETARG_INT64(1);

	MDB_RETURN_BOOL(val1 <= val2);
}

Datum
int28ge(MDB_FUNCTION_ARGS)
{
	int16		val1 = MDB_GETARG_INT16(0);
	int64		val2 = MDB_GETARG_INT64(1);

	MDB_RETURN_BOOL(val1 >= val2);
}


/*----------------------------------------------------------
 *	Arithmetic operators on 64-bit integers.
 *---------------------------------------------------------*/

Datum
int8um(MDB_FUNCTION_ARGS)
{
	int64		arg = MDB_GETARG_INT64(0);
	int64		result;

	result = -arg;
	/* overflow check (needed for INT64_MIN) */
	if (arg != 0 && SAMESIGN(result, arg))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint out of range")));
	MDB_RETURN_INT64(result);
}

Datum
int8up(MDB_FUNCTION_ARGS)
{
	int64		arg = MDB_GETARG_INT64(0);

	MDB_RETURN_INT64(arg);
}

Datum
int8pl(MDB_FUNCTION_ARGS)
{
	int64		arg1 = MDB_GETARG_INT64(0);
	int64		arg2 = MDB_GETARG_INT64(1);
	int64		result;

	result = arg1 + arg2;

	/*
	 * Overflow check.  If the inputs are of different signs then their sum
	 * cannot overflow.  If the inputs are of the same sign, their sum had
	 * better be that sign too.
	 */
	if (SAMESIGN(arg1, arg2) && !SAMESIGN(result, arg1))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint out of range")));
	MDB_RETURN_INT64(result);
}

Datum
int8mi(MDB_FUNCTION_ARGS)
{
	int64		arg1 = MDB_GETARG_INT64(0);
	int64		arg2 = MDB_GETARG_INT64(1);
	int64		result;

	result = arg1 - arg2;

	/*
	 * Overflow check.  If the inputs are of the same sign then their
	 * difference cannot overflow.  If they are of different signs then the
	 * result should be of the same sign as the first input.
	 */
	if (!SAMESIGN(arg1, arg2) && !SAMESIGN(result, arg1))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint out of range")));
	MDB_RETURN_INT64(result);
}

Datum
int8mul(MDB_FUNCTION_ARGS)
{
	int64		arg1 = MDB_GETARG_INT64(0);
	int64		arg2 = MDB_GETARG_INT64(1);
	int64		result;

	result = arg1 * arg2;

	/*
	 * Overflow check.  We basically check to see if result / arg2 gives arg1
	 * again.  There are two cases where this fails: arg2 = 0 (which cannot
	 * overflow) and arg1 = INT64_MIN, arg2 = -1 (where the division itself
	 * will overflow and thus incorrectly match).
	 *
	 * Since the division is likely much more expensive than the actual
	 * multiplication, we'd like to skip it where possible.  The best bang for
	 * the buck seems to be to check whether both inputs are in the int32
	 * range; if so, no overflow is possible.
	 */
	if (arg1 != (int64) ((int32) arg1) || arg2 != (int64) ((int32) arg2))
	{
		if (arg2 != 0 &&
			((arg2 == -1 && arg1 < 0 && result < 0) ||
			 result / arg2 != arg1))
			ereport(ERROR,
					(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
					 errmsg("bigint out of range")));
	}
	MDB_RETURN_INT64(result);
}

Datum
int8div(MDB_FUNCTION_ARGS)
{
	int64		arg1 = MDB_GETARG_INT64(0);
	int64		arg2 = MDB_GETARG_INT64(1);
	int64		result;

	if (arg2 == 0)
	{
		ereport(ERROR,
				(errcode(ERRCODE_DIVISION_BY_ZERO),
				 errmsg("division by zero")));
		/* ensure compiler realizes we mustn't reach the division (gcc bug) */
		MDB_RETURN_NULL();
	}

	/*
	 * INT64_MIN / -1 is problematic, since the result can't be represented on
	 * a two's-complement machine.  Some machines produce INT64_MIN, some
	 * produce zero, some throw an exception.  We can dodge the problem by
	 * recognizing that division by -1 is the same as negation.
	 */
	if (arg2 == -1)
	{
		result = -arg1;
		/* overflow check (needed for INT64_MIN) */
		if (arg1 != 0 && SAMESIGN(result, arg1))
			ereport(ERROR,
					(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
					 errmsg("bigint out of range")));
		MDB_RETURN_INT64(result);
	}

	/* No overflow is possible */

	result = arg1 / arg2;

	MDB_RETURN_INT64(result);
}

/* int8abs()
 * Absolute value
 */
Datum
int8abs(MDB_FUNCTION_ARGS)
{
	int64		arg1 = MDB_GETARG_INT64(0);
	int64		result;

	result = (arg1 < 0) ? -arg1 : arg1;
	/* overflow check (needed for INT64_MIN) */
	if (result < 0)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint out of range")));
	MDB_RETURN_INT64(result);
}

/* int8mod()
 * Modulo operation.
 */
Datum
int8mod(MDB_FUNCTION_ARGS)
{
	int64		arg1 = MDB_GETARG_INT64(0);
	int64		arg2 = MDB_GETARG_INT64(1);

	if (arg2 == 0)
	{
		ereport(ERROR,
				(errcode(ERRCODE_DIVISION_BY_ZERO),
				 errmsg("division by zero")));
		/* ensure compiler realizes we mustn't reach the division (gcc bug) */
		MDB_RETURN_NULL();
	}

	/*
	 * Some machines throw a floating-point exception for INT64_MIN % -1,
	 * which is a bit silly since the correct answer is perfectly
	 * well-defined, namely zero.
	 */
	if (arg2 == -1)
		MDB_RETURN_INT64(0);

	/* No overflow is possible */

	MDB_RETURN_INT64(arg1 % arg2);
}


Datum
int8inc(MDB_FUNCTION_ARGS)
{
	/*
	 * When int8 is pass-by-reference, we provide this special case to avoid
	 * palloc overhead for COUNT(): when called as an aggregate, we know that
	 * the argument is modifiable local storage, so just update it in-place.
	 * (If int8 is pass-by-value, then of course this is useless as well as
	 * incorrect, so just ifdef it out.)
	 */
#ifndef USE_FLOAT8_BYVAL		/* controls int8 too */
	if (AggCheckCallContext(fcinfo, NULL))
	{
		int64	   *arg = (int64 *) MDB_GETARG_POINTER(0);
		int64		result;

		result = *arg + 1;
		/* Overflow check */
		if (result < 0 && *arg > 0)
			ereport(ERROR,
					(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
					 errmsg("bigint out of range")));

		*arg = result;
		MDB_RETURN_POINTER(arg);
	}
	else
#endif
	{
		/* Not called as an aggregate, so just do it the dumb way */
		int64		arg = MDB_GETARG_INT64(0);
		int64		result;

		result = arg + 1;
		/* Overflow check */
		if (result < 0 && arg > 0)
			ereport(ERROR,
					(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
					 errmsg("bigint out of range")));

		MDB_RETURN_INT64(result);
	}
}

Datum
int8dec(MDB_FUNCTION_ARGS)
{
	/*
	 * When int8 is pass-by-reference, we provide this special case to avoid
	 * palloc overhead for COUNT(): when called as an aggregate, we know that
	 * the argument is modifiable local storage, so just update it in-place.
	 * (If int8 is pass-by-value, then of course this is useless as well as
	 * incorrect, so just ifdef it out.)
	 */
#ifndef USE_FLOAT8_BYVAL		/* controls int8 too */
	if (AggCheckCallContext(fcinfo, NULL))
	{
		int64	   *arg = (int64 *) MDB_GETARG_POINTER(0);
		int64		result;

		result = *arg - 1;
		/* Overflow check */
		if (result > 0 && *arg < 0)
			ereport(ERROR,
					(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
					 errmsg("bigint out of range")));

		*arg = result;
		MDB_RETURN_POINTER(arg);
	}
	else
#endif
	{
		/* Not called as an aggregate, so just do it the dumb way */
		int64		arg = MDB_GETARG_INT64(0);
		int64		result;

		result = arg - 1;
		/* Overflow check */
		if (result > 0 && arg < 0)
			ereport(ERROR,
					(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
					 errmsg("bigint out of range")));

		MDB_RETURN_INT64(result);
	}
}


/*
 * These functions are exactly like int8inc/int8dec but are used for
 * aggregates that count only non-null values.  Since the functions are
 * declared strict, the null checks happen before we ever get here, and all we
 * need do is increment the state value.  We could actually make these mdb_proc
 * entries point right at int8inc/int8dec, but then the opr_sanity regression
 * test would complain about mismatched entries for a built-in function.
 */

Datum
int8inc_any(MDB_FUNCTION_ARGS)
{
	return int8inc(fcinfo);
}

Datum
int8inc_float8_float8(MDB_FUNCTION_ARGS)
{
	return int8inc(fcinfo);
}

Datum
int8dec_any(MDB_FUNCTION_ARGS)
{
	return int8dec(fcinfo);
}


Datum
int8larger(MDB_FUNCTION_ARGS)
{
	int64		arg1 = MDB_GETARG_INT64(0);
	int64		arg2 = MDB_GETARG_INT64(1);
	int64		result;

	result = ((arg1 > arg2) ? arg1 : arg2);

	MDB_RETURN_INT64(result);
}

Datum
int8smaller(MDB_FUNCTION_ARGS)
{
	int64		arg1 = MDB_GETARG_INT64(0);
	int64		arg2 = MDB_GETARG_INT64(1);
	int64		result;

	result = ((arg1 < arg2) ? arg1 : arg2);

	MDB_RETURN_INT64(result);
}

Datum
int84pl(MDB_FUNCTION_ARGS)
{
	int64		arg1 = MDB_GETARG_INT64(0);
	int32		arg2 = MDB_GETARG_INT32(1);
	int64		result;

	result = arg1 + arg2;

	/*
	 * Overflow check.  If the inputs are of different signs then their sum
	 * cannot overflow.  If the inputs are of the same sign, their sum had
	 * better be that sign too.
	 */
	if (SAMESIGN(arg1, arg2) && !SAMESIGN(result, arg1))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint out of range")));
	MDB_RETURN_INT64(result);
}

Datum
int84mi(MDB_FUNCTION_ARGS)
{
	int64		arg1 = MDB_GETARG_INT64(0);
	int32		arg2 = MDB_GETARG_INT32(1);
	int64		result;

	result = arg1 - arg2;

	/*
	 * Overflow check.  If the inputs are of the same sign then their
	 * difference cannot overflow.  If they are of different signs then the
	 * result should be of the same sign as the first input.
	 */
	if (!SAMESIGN(arg1, arg2) && !SAMESIGN(result, arg1))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint out of range")));
	MDB_RETURN_INT64(result);
}

Datum
int84mul(MDB_FUNCTION_ARGS)
{
	int64		arg1 = MDB_GETARG_INT64(0);
	int32		arg2 = MDB_GETARG_INT32(1);
	int64		result;

	result = arg1 * arg2;

	/*
	 * Overflow check.  We basically check to see if result / arg1 gives arg2
	 * again.  There is one case where this fails: arg1 = 0 (which cannot
	 * overflow).
	 *
	 * Since the division is likely much more expensive than the actual
	 * multiplication, we'd like to skip it where possible.  The best bang for
	 * the buck seems to be to check whether both inputs are in the int32
	 * range; if so, no overflow is possible.
	 */
	if (arg1 != (int64) ((int32) arg1) &&
		result / arg1 != arg2)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint out of range")));
	MDB_RETURN_INT64(result);
}

Datum
int84div(MDB_FUNCTION_ARGS)
{
	int64		arg1 = MDB_GETARG_INT64(0);
	int32		arg2 = MDB_GETARG_INT32(1);
	int64		result;

	if (arg2 == 0)
	{
		ereport(ERROR,
				(errcode(ERRCODE_DIVISION_BY_ZERO),
				 errmsg("division by zero")));
		/* ensure compiler realizes we mustn't reach the division (gcc bug) */
		MDB_RETURN_NULL();
	}

	/*
	 * INT64_MIN / -1 is problematic, since the result can't be represented on
	 * a two's-complement machine.  Some machines produce INT64_MIN, some
	 * produce zero, some throw an exception.  We can dodge the problem by
	 * recognizing that division by -1 is the same as negation.
	 */
	if (arg2 == -1)
	{
		result = -arg1;
		/* overflow check (needed for INT64_MIN) */
		if (arg1 != 0 && SAMESIGN(result, arg1))
			ereport(ERROR,
					(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
					 errmsg("bigint out of range")));
		MDB_RETURN_INT64(result);
	}

	/* No overflow is possible */

	result = arg1 / arg2;

	MDB_RETURN_INT64(result);
}

Datum
int48pl(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	int64		arg2 = MDB_GETARG_INT64(1);
	int64		result;

	result = arg1 + arg2;

	/*
	 * Overflow check.  If the inputs are of different signs then their sum
	 * cannot overflow.  If the inputs are of the same sign, their sum had
	 * better be that sign too.
	 */
	if (SAMESIGN(arg1, arg2) && !SAMESIGN(result, arg1))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint out of range")));
	MDB_RETURN_INT64(result);
}

Datum
int48mi(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	int64		arg2 = MDB_GETARG_INT64(1);
	int64		result;

	result = arg1 - arg2;

	/*
	 * Overflow check.  If the inputs are of the same sign then their
	 * difference cannot overflow.  If they are of different signs then the
	 * result should be of the same sign as the first input.
	 */
	if (!SAMESIGN(arg1, arg2) && !SAMESIGN(result, arg1))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint out of range")));
	MDB_RETURN_INT64(result);
}

Datum
int48mul(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	int64		arg2 = MDB_GETARG_INT64(1);
	int64		result;

	result = arg1 * arg2;

	/*
	 * Overflow check.  We basically check to see if result / arg2 gives arg1
	 * again.  There is one case where this fails: arg2 = 0 (which cannot
	 * overflow).
	 *
	 * Since the division is likely much more expensive than the actual
	 * multiplication, we'd like to skip it where possible.  The best bang for
	 * the buck seems to be to check whether both inputs are in the int32
	 * range; if so, no overflow is possible.
	 */
	if (arg2 != (int64) ((int32) arg2) &&
		result / arg2 != arg1)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint out of range")));
	MDB_RETURN_INT64(result);
}

Datum
int48div(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	int64		arg2 = MDB_GETARG_INT64(1);

	if (arg2 == 0)
	{
		ereport(ERROR,
				(errcode(ERRCODE_DIVISION_BY_ZERO),
				 errmsg("division by zero")));
		/* ensure compiler realizes we mustn't reach the division (gcc bug) */
		MDB_RETURN_NULL();
	}

	/* No overflow is possible */
	MDB_RETURN_INT64((int64) arg1 / arg2);
}

Datum
int82pl(MDB_FUNCTION_ARGS)
{
	int64		arg1 = MDB_GETARG_INT64(0);
	int16		arg2 = MDB_GETARG_INT16(1);
	int64		result;

	result = arg1 + arg2;

	/*
	 * Overflow check.  If the inputs are of different signs then their sum
	 * cannot overflow.  If the inputs are of the same sign, their sum had
	 * better be that sign too.
	 */
	if (SAMESIGN(arg1, arg2) && !SAMESIGN(result, arg1))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint out of range")));
	MDB_RETURN_INT64(result);
}

Datum
int82mi(MDB_FUNCTION_ARGS)
{
	int64		arg1 = MDB_GETARG_INT64(0);
	int16		arg2 = MDB_GETARG_INT16(1);
	int64		result;

	result = arg1 - arg2;

	/*
	 * Overflow check.  If the inputs are of the same sign then their
	 * difference cannot overflow.  If they are of different signs then the
	 * result should be of the same sign as the first input.
	 */
	if (!SAMESIGN(arg1, arg2) && !SAMESIGN(result, arg1))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint out of range")));
	MDB_RETURN_INT64(result);
}

Datum
int82mul(MDB_FUNCTION_ARGS)
{
	int64		arg1 = MDB_GETARG_INT64(0);
	int16		arg2 = MDB_GETARG_INT16(1);
	int64		result;

	result = arg1 * arg2;

	/*
	 * Overflow check.  We basically check to see if result / arg1 gives arg2
	 * again.  There is one case where this fails: arg1 = 0 (which cannot
	 * overflow).
	 *
	 * Since the division is likely much more expensive than the actual
	 * multiplication, we'd like to skip it where possible.  The best bang for
	 * the buck seems to be to check whether both inputs are in the int32
	 * range; if so, no overflow is possible.
	 */
	if (arg1 != (int64) ((int32) arg1) &&
		result / arg1 != arg2)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint out of range")));
	MDB_RETURN_INT64(result);
}

Datum
int82div(MDB_FUNCTION_ARGS)
{
	int64		arg1 = MDB_GETARG_INT64(0);
	int16		arg2 = MDB_GETARG_INT16(1);
	int64		result;

	if (arg2 == 0)
	{
		ereport(ERROR,
				(errcode(ERRCODE_DIVISION_BY_ZERO),
				 errmsg("division by zero")));
		/* ensure compiler realizes we mustn't reach the division (gcc bug) */
		MDB_RETURN_NULL();
	}

	/*
	 * INT64_MIN / -1 is problematic, since the result can't be represented on
	 * a two's-complement machine.  Some machines produce INT64_MIN, some
	 * produce zero, some throw an exception.  We can dodge the problem by
	 * recognizing that division by -1 is the same as negation.
	 */
	if (arg2 == -1)
	{
		result = -arg1;
		/* overflow check (needed for INT64_MIN) */
		if (arg1 != 0 && SAMESIGN(result, arg1))
			ereport(ERROR,
					(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
					 errmsg("bigint out of range")));
		MDB_RETURN_INT64(result);
	}

	/* No overflow is possible */

	result = arg1 / arg2;

	MDB_RETURN_INT64(result);
}

Datum
int28pl(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	int64		arg2 = MDB_GETARG_INT64(1);
	int64		result;

	result = arg1 + arg2;

	/*
	 * Overflow check.  If the inputs are of different signs then their sum
	 * cannot overflow.  If the inputs are of the same sign, their sum had
	 * better be that sign too.
	 */
	if (SAMESIGN(arg1, arg2) && !SAMESIGN(result, arg1))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint out of range")));
	MDB_RETURN_INT64(result);
}

Datum
int28mi(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	int64		arg2 = MDB_GETARG_INT64(1);
	int64		result;

	result = arg1 - arg2;

	/*
	 * Overflow check.  If the inputs are of the same sign then their
	 * difference cannot overflow.  If they are of different signs then the
	 * result should be of the same sign as the first input.
	 */
	if (!SAMESIGN(arg1, arg2) && !SAMESIGN(result, arg1))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint out of range")));
	MDB_RETURN_INT64(result);
}

Datum
int28mul(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	int64		arg2 = MDB_GETARG_INT64(1);
	int64		result;

	result = arg1 * arg2;

	/*
	 * Overflow check.  We basically check to see if result / arg2 gives arg1
	 * again.  There is one case where this fails: arg2 = 0 (which cannot
	 * overflow).
	 *
	 * Since the division is likely much more expensive than the actual
	 * multiplication, we'd like to skip it where possible.  The best bang for
	 * the buck seems to be to check whether both inputs are in the int32
	 * range; if so, no overflow is possible.
	 */
	if (arg2 != (int64) ((int32) arg2) &&
		result / arg2 != arg1)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint out of range")));
	MDB_RETURN_INT64(result);
}

Datum
int28div(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	int64		arg2 = MDB_GETARG_INT64(1);

	if (arg2 == 0)
	{
		ereport(ERROR,
				(errcode(ERRCODE_DIVISION_BY_ZERO),
				 errmsg("division by zero")));
		/* ensure compiler realizes we mustn't reach the division (gcc bug) */
		MDB_RETURN_NULL();
	}

	/* No overflow is possible */
	MDB_RETURN_INT64((int64) arg1 / arg2);
}

/* Binary arithmetics
 *
 *		int8and		- returns arg1 & arg2
 *		int8or		- returns arg1 | arg2
 *		int8xor		- returns arg1 # arg2
 *		int8not		- returns ~arg1
 *		int8shl		- returns arg1 << arg2
 *		int8shr		- returns arg1 >> arg2
 */

Datum
int8and(MDB_FUNCTION_ARGS)
{
	int64		arg1 = MDB_GETARG_INT64(0);
	int64		arg2 = MDB_GETARG_INT64(1);

	MDB_RETURN_INT64(arg1 & arg2);
}

Datum
int8or(MDB_FUNCTION_ARGS)
{
	int64		arg1 = MDB_GETARG_INT64(0);
	int64		arg2 = MDB_GETARG_INT64(1);

	MDB_RETURN_INT64(arg1 | arg2);
}

Datum
int8xor(MDB_FUNCTION_ARGS)
{
	int64		arg1 = MDB_GETARG_INT64(0);
	int64		arg2 = MDB_GETARG_INT64(1);

	MDB_RETURN_INT64(arg1 ^ arg2);
}

Datum
int8not(MDB_FUNCTION_ARGS)
{
	int64		arg1 = MDB_GETARG_INT64(0);

	MDB_RETURN_INT64(~arg1);
}

Datum
int8shl(MDB_FUNCTION_ARGS)
{
	int64		arg1 = MDB_GETARG_INT64(0);
	int32		arg2 = MDB_GETARG_INT32(1);

	MDB_RETURN_INT64(arg1 << arg2);
}

Datum
int8shr(MDB_FUNCTION_ARGS)
{
	int64		arg1 = MDB_GETARG_INT64(0);
	int32		arg2 = MDB_GETARG_INT32(1);

	MDB_RETURN_INT64(arg1 >> arg2);
}

/*----------------------------------------------------------
 *	Conversion operators.
 *---------------------------------------------------------*/

Datum
int48(MDB_FUNCTION_ARGS)
{
	int32		arg = MDB_GETARG_INT32(0);

	MDB_RETURN_INT64((int64) arg);
}

Datum
int84(MDB_FUNCTION_ARGS)
{
	int64		arg = MDB_GETARG_INT64(0);
	int32		result;

	result = (int32) arg;

	/* Test for overflow by reverse-conversion. */
	if ((int64) result != arg)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("integer out of range")));

	MDB_RETURN_INT32(result);
}

Datum
int28(MDB_FUNCTION_ARGS)
{
	int16		arg = MDB_GETARG_INT16(0);

	MDB_RETURN_INT64((int64) arg);
}

Datum
int82(MDB_FUNCTION_ARGS)
{
	int64		arg = MDB_GETARG_INT64(0);
	int16		result;

	result = (int16) arg;

	/* Test for overflow by reverse-conversion. */
	if ((int64) result != arg)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("smallint out of range")));

	MDB_RETURN_INT16(result);
}

Datum
i8tod(MDB_FUNCTION_ARGS)
{
	int64		arg = MDB_GETARG_INT64(0);
	float8		result;

	result = arg;

	MDB_RETURN_FLOAT8(result);
}

/* dtoi8()
 * Convert float8 to 8-byte integer.
 */
Datum
dtoi8(MDB_FUNCTION_ARGS)
{
	float8		arg = MDB_GETARG_FLOAT8(0);
	int64		result;

	/* Round arg to nearest integer (but it's still in float form) */
	arg = rint(arg);

	/*
	 * Does it fit in an int64?  Avoid assuming that we have handy constants
	 * defined for the range boundaries, instead test for overflow by
	 * reverse-conversion.
	 */
	result = (int64) arg;

	if ((float8) result != arg)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint out of range")));

	MDB_RETURN_INT64(result);
}

Datum
i8tof(MDB_FUNCTION_ARGS)
{
	int64		arg = MDB_GETARG_INT64(0);
	float4		result;

	result = arg;

	MDB_RETURN_FLOAT4(result);
}

/* ftoi8()
 * Convert float4 to 8-byte integer.
 */
Datum
ftoi8(MDB_FUNCTION_ARGS)
{
	float4		arg = MDB_GETARG_FLOAT4(0);
	int64		result;
	float8		darg;

	/* Round arg to nearest integer (but it's still in float form) */
	darg = rint(arg);

	/*
	 * Does it fit in an int64?  Avoid assuming that we have handy constants
	 * defined for the range boundaries, instead test for overflow by
	 * reverse-conversion.
	 */
	result = (int64) darg;

	if ((float8) result != darg)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("bigint out of range")));

	MDB_RETURN_INT64(result);
}

Datum
i8tooid(MDB_FUNCTION_ARGS)
{
	int64		arg = MDB_GETARG_INT64(0);
	Oid			result;

	result = (Oid) arg;

	/* Test for overflow by reverse-conversion. */
	if ((int64) result != arg)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("OID out of range")));

	MDB_RETURN_OID(result);
}

Datum
oidtoi8(MDB_FUNCTION_ARGS)
{
	Oid			arg = MDB_GETARG_OID(0);

	MDB_RETURN_INT64((int64) arg);
}

/*
 * non-persistent numeric series generator
 */
Datum
generate_series_int8(MDB_FUNCTION_ARGS)
{
	return generate_series_step_int8(fcinfo);
}

Datum
generate_series_step_int8(MDB_FUNCTION_ARGS)
{
	FuncCallContext *funcctx;
	generate_series_fctx *fctx;
	int64		result;
	MemoryContext oldcontext;

	/* stuff done only on the first call of the function */
	if (SRF_IS_FIRSTCALL())
	{
		int64		start = MDB_GETARG_INT64(0);
		int64		finish = MDB_GETARG_INT64(1);
		int64		step = 1;

		/* see if we were given an explicit step size */
		if (MDB_NARGS() == 3)
			step = MDB_GETARG_INT64(2);
		if (step == 0)
			ereport(ERROR,
					(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
					 errmsg("step size cannot equal zero")));

		/* create a function context for cross-call persistence */
		funcctx = SRF_FIRSTCALL_INIT();

		/*
		 * switch to memory context appropriate for multiple function calls
		 */
		oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

		/* allocate memory for user context */
		fctx = (generate_series_fctx *) palloc(sizeof(generate_series_fctx));

		/*
		 * Use fctx to keep state from call to call. Seed current with the
		 * original start value
		 */
		fctx->current = start;
		fctx->finish = finish;
		fctx->step = step;

		funcctx->user_fctx = fctx;
		MemoryContextSwitchTo(oldcontext);
	}

	/* stuff done on every call of the function */
	funcctx = SRF_PERCALL_SETUP();

	/*
	 * get the saved state and use current as the result for this iteration
	 */
	fctx = funcctx->user_fctx;
	result = fctx->current;

	if ((fctx->step > 0 && fctx->current <= fctx->finish) ||
		(fctx->step < 0 && fctx->current >= fctx->finish))
	{
		/* increment current in preparation for next iteration */
		fctx->current += fctx->step;

		/* if next-value computation overflows, this is the final result */
		if (SAMESIGN(result, fctx->step) && !SAMESIGN(result, fctx->current))
			fctx->step = 0;

		/* do when there is more left to send */
		SRF_RETURN_NEXT(funcctx, Int64GetDatum(result));
	}
	else
		/* do when there is no more left */
		SRF_RETURN_DONE(funcctx);
}
