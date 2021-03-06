/*-------------------------------------------------------------------------
 *
 * int.c
 *	  Functions for the built-in integer types (except int8).
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/utils/adt/int.c
 *
 *-------------------------------------------------------------------------
 */
/*
 * OLD COMMENTS
 *		I/O routines:
 *		 int2in, int2out, int2recv, int2send
 *		 int4in, int4out, int4recv, int4send
 *		 int2vectorin, int2vectorout, int2vectorrecv, int2vectorsend
 *		Boolean operators:
 *		 inteq, intne, intlt, intle, intgt, intge
 *		Arithmetic operators:
 *		 intpl, intmi, int4mul, intdiv
 *
 *		Arithmetic operators:
 *		 intmod
 */
#include "mollydb.h"

#include <ctype.h>
#include <limits.h>

#include "catalog/mdb_type.h"
#include "funcapi.h"
#include "libpq/pqformat.h"
#include "utils/array.h"
#include "utils/builtins.h"


#define SAMESIGN(a,b)	(((a) < 0) == ((b) < 0))

#define Int2VectorSize(n)	(offsetof(int2vector, values) + (n) * sizeof(int16))

typedef struct
{
	int32		current;
	int32		finish;
	int32		step;
} generate_series_fctx;


/*****************************************************************************
 *	 USER I/O ROUTINES														 *
 *****************************************************************************/

/*
 *		int2in			- converts "num" to short
 */
Datum
int2in(MDB_FUNCTION_ARGS)
{
	char	   *num = MDB_GETARG_CSTRING(0);

	MDB_RETURN_INT16(mdb_atoi(num, sizeof(int16), '\0'));
}

/*
 *		int2out			- converts short to "num"
 */
Datum
int2out(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	char	   *result = (char *) palloc(7);	/* sign, 5 digits, '\0' */

	mdb_itoa(arg1, result);
	MDB_RETURN_CSTRING(result);
}

/*
 *		int2recv			- converts external binary format to int2
 */
Datum
int2recv(MDB_FUNCTION_ARGS)
{
	StringInfo	buf = (StringInfo) MDB_GETARG_POINTER(0);

	MDB_RETURN_INT16((int16) pq_getmsgint(buf, sizeof(int16)));
}

/*
 *		int2send			- converts int2 to binary format
 */
Datum
int2send(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	StringInfoData buf;

	pq_begintypsend(&buf);
	pq_sendint(&buf, arg1, sizeof(int16));
	MDB_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

/*
 * construct int2vector given a raw array of int2s
 *
 * If int2s is NULL then caller must fill values[] afterward
 */
int2vector *
buildint2vector(const int16 *int2s, int n)
{
	int2vector *result;

	result = (int2vector *) palloc0(Int2VectorSize(n));

	if (n > 0 && int2s)
		memcpy(result->values, int2s, n * sizeof(int16));

	/*
	 * Attach standard array header.  For historical reasons, we set the index
	 * lower bound to 0 not 1.
	 */
	SET_VARSIZE(result, Int2VectorSize(n));
	result->ndim = 1;
	result->dataoffset = 0;		/* never any nulls */
	result->elemtype = INT2OID;
	result->dim1 = n;
	result->lbound1 = 0;

	return result;
}

/*
 *		int2vectorin			- converts "num num ..." to internal form
 */
Datum
int2vectorin(MDB_FUNCTION_ARGS)
{
	char	   *intString = MDB_GETARG_CSTRING(0);
	int2vector *result;
	int			n;

	result = (int2vector *) palloc0(Int2VectorSize(FUNC_MAX_ARGS));

	for (n = 0; *intString && n < FUNC_MAX_ARGS; n++)
	{
		while (*intString && isspace((unsigned char) *intString))
			intString++;
		if (*intString == '\0')
			break;
		result->values[n] = mdb_atoi(intString, sizeof(int16), ' ');
		while (*intString && !isspace((unsigned char) *intString))
			intString++;
	}
	while (*intString && isspace((unsigned char) *intString))
		intString++;
	if (*intString)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				 errmsg("int2vector has too many elements")));

	SET_VARSIZE(result, Int2VectorSize(n));
	result->ndim = 1;
	result->dataoffset = 0;		/* never any nulls */
	result->elemtype = INT2OID;
	result->dim1 = n;
	result->lbound1 = 0;

	MDB_RETURN_POINTER(result);
}

/*
 *		int2vectorout		- converts internal form to "num num ..."
 */
Datum
int2vectorout(MDB_FUNCTION_ARGS)
{
	int2vector *int2Array = (int2vector *) MDB_GETARG_POINTER(0);
	int			num,
				nnums = int2Array->dim1;
	char	   *rp;
	char	   *result;

	/* assumes sign, 5 digits, ' ' */
	rp = result = (char *) palloc(nnums * 7 + 1);
	for (num = 0; num < nnums; num++)
	{
		if (num != 0)
			*rp++ = ' ';
		mdb_itoa(int2Array->values[num], rp);
		while (*++rp != '\0')
			;
	}
	*rp = '\0';
	MDB_RETURN_CSTRING(result);
}

/*
 *		int2vectorrecv			- converts external binary format to int2vector
 */
Datum
int2vectorrecv(MDB_FUNCTION_ARGS)
{
	StringInfo	buf = (StringInfo) MDB_GETARG_POINTER(0);
	FunctionCallInfoData locfcinfo;
	int2vector *result;

	/*
	 * Normally one would call array_recv() using DirectFunctionCall3, but
	 * that does not work since array_recv wants to cache some data using
	 * fcinfo->flinfo->fn_extra.  So we need to pass it our own flinfo
	 * parameter.
	 */
	InitFunctionCallInfoData(locfcinfo, fcinfo->flinfo, 3,
							 InvalidOid, NULL, NULL);

	locfcinfo.arg[0] = PointerGetDatum(buf);
	locfcinfo.arg[1] = ObjectIdGetDatum(INT2OID);
	locfcinfo.arg[2] = Int32GetDatum(-1);
	locfcinfo.argnull[0] = false;
	locfcinfo.argnull[1] = false;
	locfcinfo.argnull[2] = false;

	result = (int2vector *) DatumGetPointer(array_recv(&locfcinfo));

	Assert(!locfcinfo.isnull);

	/* sanity checks: int2vector must be 1-D, 0-based, no nulls */
	if (ARR_NDIM(result) != 1 ||
		ARR_HASNULL(result) ||
		ARR_ELEMTYPE(result) != INT2OID ||
		ARR_LBOUND(result)[0] != 0)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_BINARY_REPRESENTATION),
				 errmsg("invalid int2vector data")));

	/* check length for consistency with int2vectorin() */
	if (ARR_DIMS(result)[0] > FUNC_MAX_ARGS)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				 errmsg("oidvector has too many elements")));

	MDB_RETURN_POINTER(result);
}

/*
 *		int2vectorsend			- converts int2vector to binary format
 */
Datum
int2vectorsend(MDB_FUNCTION_ARGS)
{
	return array_send(fcinfo);
}

/*
 * We don't have a complete set of int2vector support routines,
 * but we need int2vectoreq for catcache indexing.
 */
Datum
int2vectoreq(MDB_FUNCTION_ARGS)
{
	int2vector *a = (int2vector *) MDB_GETARG_POINTER(0);
	int2vector *b = (int2vector *) MDB_GETARG_POINTER(1);

	if (a->dim1 != b->dim1)
		MDB_RETURN_BOOL(false);
	MDB_RETURN_BOOL(memcmp(a->values, b->values, a->dim1 * sizeof(int16)) == 0);
}


/*****************************************************************************
 *	 PUBLIC ROUTINES														 *
 *****************************************************************************/

/*
 *		int4in			- converts "num" to int4
 */
Datum
int4in(MDB_FUNCTION_ARGS)
{
	char	   *num = MDB_GETARG_CSTRING(0);

	MDB_RETURN_INT32(mdb_atoi(num, sizeof(int32), '\0'));
}

/*
 *		int4out			- converts int4 to "num"
 */
Datum
int4out(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	char	   *result = (char *) palloc(12);	/* sign, 10 digits, '\0' */

	mdb_ltoa(arg1, result);
	MDB_RETURN_CSTRING(result);
}

/*
 *		int4recv			- converts external binary format to int4
 */
Datum
int4recv(MDB_FUNCTION_ARGS)
{
	StringInfo	buf = (StringInfo) MDB_GETARG_POINTER(0);

	MDB_RETURN_INT32((int32) pq_getmsgint(buf, sizeof(int32)));
}

/*
 *		int4send			- converts int4 to binary format
 */
Datum
int4send(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	StringInfoData buf;

	pq_begintypsend(&buf);
	pq_sendint(&buf, arg1, sizeof(int32));
	MDB_RETURN_BYTEA_P(pq_endtypsend(&buf));
}


/*
 *		===================
 *		CONVERSION ROUTINES
 *		===================
 */

Datum
i2toi4(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);

	MDB_RETURN_INT32((int32) arg1);
}

Datum
i4toi2(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);

	if (arg1 < SHRT_MIN || arg1 > SHRT_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("smallint out of range")));

	MDB_RETURN_INT16((int16) arg1);
}

/* Cast int4 -> bool */
Datum
int4_bool(MDB_FUNCTION_ARGS)
{
	if (MDB_GETARG_INT32(0) == 0)
		MDB_RETURN_BOOL(false);
	else
		MDB_RETURN_BOOL(true);
}

/* Cast bool -> int4 */
Datum
bool_int4(MDB_FUNCTION_ARGS)
{
	if (MDB_GETARG_BOOL(0) == false)
		MDB_RETURN_INT32(0);
	else
		MDB_RETURN_INT32(1);
}

/*
 *		============================
 *		COMPARISON OPERATOR ROUTINES
 *		============================
 */

/*
 *		inteq			- returns 1 iff arg1 == arg2
 *		intne			- returns 1 iff arg1 != arg2
 *		intlt			- returns 1 iff arg1 < arg2
 *		intle			- returns 1 iff arg1 <= arg2
 *		intgt			- returns 1 iff arg1 > arg2
 *		intge			- returns 1 iff arg1 >= arg2
 */

Datum
int4eq(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	int32		arg2 = MDB_GETARG_INT32(1);

	MDB_RETURN_BOOL(arg1 == arg2);
}

Datum
int4ne(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	int32		arg2 = MDB_GETARG_INT32(1);

	MDB_RETURN_BOOL(arg1 != arg2);
}

Datum
int4lt(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	int32		arg2 = MDB_GETARG_INT32(1);

	MDB_RETURN_BOOL(arg1 < arg2);
}

Datum
int4le(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	int32		arg2 = MDB_GETARG_INT32(1);

	MDB_RETURN_BOOL(arg1 <= arg2);
}

Datum
int4gt(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	int32		arg2 = MDB_GETARG_INT32(1);

	MDB_RETURN_BOOL(arg1 > arg2);
}

Datum
int4ge(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	int32		arg2 = MDB_GETARG_INT32(1);

	MDB_RETURN_BOOL(arg1 >= arg2);
}

Datum
int2eq(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	int16		arg2 = MDB_GETARG_INT16(1);

	MDB_RETURN_BOOL(arg1 == arg2);
}

Datum
int2ne(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	int16		arg2 = MDB_GETARG_INT16(1);

	MDB_RETURN_BOOL(arg1 != arg2);
}

Datum
int2lt(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	int16		arg2 = MDB_GETARG_INT16(1);

	MDB_RETURN_BOOL(arg1 < arg2);
}

Datum
int2le(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	int16		arg2 = MDB_GETARG_INT16(1);

	MDB_RETURN_BOOL(arg1 <= arg2);
}

Datum
int2gt(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	int16		arg2 = MDB_GETARG_INT16(1);

	MDB_RETURN_BOOL(arg1 > arg2);
}

Datum
int2ge(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	int16		arg2 = MDB_GETARG_INT16(1);

	MDB_RETURN_BOOL(arg1 >= arg2);
}

Datum
int24eq(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	int32		arg2 = MDB_GETARG_INT32(1);

	MDB_RETURN_BOOL(arg1 == arg2);
}

Datum
int24ne(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	int32		arg2 = MDB_GETARG_INT32(1);

	MDB_RETURN_BOOL(arg1 != arg2);
}

Datum
int24lt(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	int32		arg2 = MDB_GETARG_INT32(1);

	MDB_RETURN_BOOL(arg1 < arg2);
}

Datum
int24le(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	int32		arg2 = MDB_GETARG_INT32(1);

	MDB_RETURN_BOOL(arg1 <= arg2);
}

Datum
int24gt(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	int32		arg2 = MDB_GETARG_INT32(1);

	MDB_RETURN_BOOL(arg1 > arg2);
}

Datum
int24ge(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	int32		arg2 = MDB_GETARG_INT32(1);

	MDB_RETURN_BOOL(arg1 >= arg2);
}

Datum
int42eq(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	int16		arg2 = MDB_GETARG_INT16(1);

	MDB_RETURN_BOOL(arg1 == arg2);
}

Datum
int42ne(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	int16		arg2 = MDB_GETARG_INT16(1);

	MDB_RETURN_BOOL(arg1 != arg2);
}

Datum
int42lt(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	int16		arg2 = MDB_GETARG_INT16(1);

	MDB_RETURN_BOOL(arg1 < arg2);
}

Datum
int42le(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	int16		arg2 = MDB_GETARG_INT16(1);

	MDB_RETURN_BOOL(arg1 <= arg2);
}

Datum
int42gt(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	int16		arg2 = MDB_GETARG_INT16(1);

	MDB_RETURN_BOOL(arg1 > arg2);
}

Datum
int42ge(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	int16		arg2 = MDB_GETARG_INT16(1);

	MDB_RETURN_BOOL(arg1 >= arg2);
}

/*
 *		int[24]pl		- returns arg1 + arg2
 *		int[24]mi		- returns arg1 - arg2
 *		int[24]mul		- returns arg1 * arg2
 *		int[24]div		- returns arg1 / arg2
 */

Datum
int4um(MDB_FUNCTION_ARGS)
{
	int32		arg = MDB_GETARG_INT32(0);
	int32		result;

	result = -arg;
	/* overflow check (needed for INT_MIN) */
	if (arg != 0 && SAMESIGN(result, arg))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("integer out of range")));
	MDB_RETURN_INT32(result);
}

Datum
int4up(MDB_FUNCTION_ARGS)
{
	int32		arg = MDB_GETARG_INT32(0);

	MDB_RETURN_INT32(arg);
}

Datum
int4pl(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	int32		arg2 = MDB_GETARG_INT32(1);
	int32		result;

	result = arg1 + arg2;

	/*
	 * Overflow check.  If the inputs are of different signs then their sum
	 * cannot overflow.  If the inputs are of the same sign, their sum had
	 * better be that sign too.
	 */
	if (SAMESIGN(arg1, arg2) && !SAMESIGN(result, arg1))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("integer out of range")));
	MDB_RETURN_INT32(result);
}

Datum
int4mi(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	int32		arg2 = MDB_GETARG_INT32(1);
	int32		result;

	result = arg1 - arg2;

	/*
	 * Overflow check.  If the inputs are of the same sign then their
	 * difference cannot overflow.  If they are of different signs then the
	 * result should be of the same sign as the first input.
	 */
	if (!SAMESIGN(arg1, arg2) && !SAMESIGN(result, arg1))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("integer out of range")));
	MDB_RETURN_INT32(result);
}

Datum
int4mul(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	int32		arg2 = MDB_GETARG_INT32(1);
	int32		result;

	result = arg1 * arg2;

	/*
	 * Overflow check.  We basically check to see if result / arg2 gives arg1
	 * again.  There are two cases where this fails: arg2 = 0 (which cannot
	 * overflow) and arg1 = INT_MIN, arg2 = -1 (where the division itself will
	 * overflow and thus incorrectly match).
	 *
	 * Since the division is likely much more expensive than the actual
	 * multiplication, we'd like to skip it where possible.  The best bang for
	 * the buck seems to be to check whether both inputs are in the int16
	 * range; if so, no overflow is possible.
	 */
	if (!(arg1 >= (int32) SHRT_MIN && arg1 <= (int32) SHRT_MAX &&
		  arg2 >= (int32) SHRT_MIN && arg2 <= (int32) SHRT_MAX) &&
		arg2 != 0 &&
		((arg2 == -1 && arg1 < 0 && result < 0) ||
		 result / arg2 != arg1))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("integer out of range")));
	MDB_RETURN_INT32(result);
}

Datum
int4div(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	int32		arg2 = MDB_GETARG_INT32(1);
	int32		result;

	if (arg2 == 0)
	{
		ereport(ERROR,
				(errcode(ERRCODE_DIVISION_BY_ZERO),
				 errmsg("division by zero")));
		/* ensure compiler realizes we mustn't reach the division (gcc bug) */
		MDB_RETURN_NULL();
	}

	/*
	 * INT_MIN / -1 is problematic, since the result can't be represented on a
	 * two's-complement machine.  Some machines produce INT_MIN, some produce
	 * zero, some throw an exception.  We can dodge the problem by recognizing
	 * that division by -1 is the same as negation.
	 */
	if (arg2 == -1)
	{
		result = -arg1;
		/* overflow check (needed for INT_MIN) */
		if (arg1 != 0 && SAMESIGN(result, arg1))
			ereport(ERROR,
					(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
					 errmsg("integer out of range")));
		MDB_RETURN_INT32(result);
	}

	/* No overflow is possible */

	result = arg1 / arg2;

	MDB_RETURN_INT32(result);
}

Datum
int4inc(MDB_FUNCTION_ARGS)
{
	int32		arg = MDB_GETARG_INT32(0);
	int32		result;

	result = arg + 1;
	/* Overflow check */
	if (arg > 0 && result < 0)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("integer out of range")));

	MDB_RETURN_INT32(result);
}

Datum
int2um(MDB_FUNCTION_ARGS)
{
	int16		arg = MDB_GETARG_INT16(0);
	int16		result;

	result = -arg;
	/* overflow check (needed for SHRT_MIN) */
	if (arg != 0 && SAMESIGN(result, arg))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("smallint out of range")));
	MDB_RETURN_INT16(result);
}

Datum
int2up(MDB_FUNCTION_ARGS)
{
	int16		arg = MDB_GETARG_INT16(0);

	MDB_RETURN_INT16(arg);
}

Datum
int2pl(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	int16		arg2 = MDB_GETARG_INT16(1);
	int16		result;

	result = arg1 + arg2;

	/*
	 * Overflow check.  If the inputs are of different signs then their sum
	 * cannot overflow.  If the inputs are of the same sign, their sum had
	 * better be that sign too.
	 */
	if (SAMESIGN(arg1, arg2) && !SAMESIGN(result, arg1))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("smallint out of range")));
	MDB_RETURN_INT16(result);
}

Datum
int2mi(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	int16		arg2 = MDB_GETARG_INT16(1);
	int16		result;

	result = arg1 - arg2;

	/*
	 * Overflow check.  If the inputs are of the same sign then their
	 * difference cannot overflow.  If they are of different signs then the
	 * result should be of the same sign as the first input.
	 */
	if (!SAMESIGN(arg1, arg2) && !SAMESIGN(result, arg1))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("smallint out of range")));
	MDB_RETURN_INT16(result);
}

Datum
int2mul(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	int16		arg2 = MDB_GETARG_INT16(1);
	int32		result32;

	/*
	 * The most practical way to detect overflow is to do the arithmetic in
	 * int32 (so that the result can't overflow) and then do a range check.
	 */
	result32 = (int32) arg1 *(int32) arg2;

	if (result32 < SHRT_MIN || result32 > SHRT_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("smallint out of range")));

	MDB_RETURN_INT16((int16) result32);
}

Datum
int2div(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	int16		arg2 = MDB_GETARG_INT16(1);
	int16		result;

	if (arg2 == 0)
	{
		ereport(ERROR,
				(errcode(ERRCODE_DIVISION_BY_ZERO),
				 errmsg("division by zero")));
		/* ensure compiler realizes we mustn't reach the division (gcc bug) */
		MDB_RETURN_NULL();
	}

	/*
	 * SHRT_MIN / -1 is problematic, since the result can't be represented on
	 * a two's-complement machine.  Some machines produce SHRT_MIN, some
	 * produce zero, some throw an exception.  We can dodge the problem by
	 * recognizing that division by -1 is the same as negation.
	 */
	if (arg2 == -1)
	{
		result = -arg1;
		/* overflow check (needed for SHRT_MIN) */
		if (arg1 != 0 && SAMESIGN(result, arg1))
			ereport(ERROR,
					(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
					 errmsg("smallint out of range")));
		MDB_RETURN_INT16(result);
	}

	/* No overflow is possible */

	result = arg1 / arg2;

	MDB_RETURN_INT16(result);
}

Datum
int24pl(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	int32		arg2 = MDB_GETARG_INT32(1);
	int32		result;

	result = arg1 + arg2;

	/*
	 * Overflow check.  If the inputs are of different signs then their sum
	 * cannot overflow.  If the inputs are of the same sign, their sum had
	 * better be that sign too.
	 */
	if (SAMESIGN(arg1, arg2) && !SAMESIGN(result, arg1))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("integer out of range")));
	MDB_RETURN_INT32(result);
}

Datum
int24mi(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	int32		arg2 = MDB_GETARG_INT32(1);
	int32		result;

	result = arg1 - arg2;

	/*
	 * Overflow check.  If the inputs are of the same sign then their
	 * difference cannot overflow.  If they are of different signs then the
	 * result should be of the same sign as the first input.
	 */
	if (!SAMESIGN(arg1, arg2) && !SAMESIGN(result, arg1))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("integer out of range")));
	MDB_RETURN_INT32(result);
}

Datum
int24mul(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	int32		arg2 = MDB_GETARG_INT32(1);
	int32		result;

	result = arg1 * arg2;

	/*
	 * Overflow check.  We basically check to see if result / arg2 gives arg1
	 * again.  There is one case where this fails: arg2 = 0 (which cannot
	 * overflow).
	 *
	 * Since the division is likely much more expensive than the actual
	 * multiplication, we'd like to skip it where possible.  The best bang for
	 * the buck seems to be to check whether both inputs are in the int16
	 * range; if so, no overflow is possible.
	 */
	if (!(arg2 >= (int32) SHRT_MIN && arg2 <= (int32) SHRT_MAX) &&
		result / arg2 != arg1)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("integer out of range")));
	MDB_RETURN_INT32(result);
}

Datum
int24div(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	int32		arg2 = MDB_GETARG_INT32(1);

	if (arg2 == 0)
	{
		ereport(ERROR,
				(errcode(ERRCODE_DIVISION_BY_ZERO),
				 errmsg("division by zero")));
		/* ensure compiler realizes we mustn't reach the division (gcc bug) */
		MDB_RETURN_NULL();
	}

	/* No overflow is possible */
	MDB_RETURN_INT32((int32) arg1 / arg2);
}

Datum
int42pl(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	int16		arg2 = MDB_GETARG_INT16(1);
	int32		result;

	result = arg1 + arg2;

	/*
	 * Overflow check.  If the inputs are of different signs then their sum
	 * cannot overflow.  If the inputs are of the same sign, their sum had
	 * better be that sign too.
	 */
	if (SAMESIGN(arg1, arg2) && !SAMESIGN(result, arg1))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("integer out of range")));
	MDB_RETURN_INT32(result);
}

Datum
int42mi(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	int16		arg2 = MDB_GETARG_INT16(1);
	int32		result;

	result = arg1 - arg2;

	/*
	 * Overflow check.  If the inputs are of the same sign then their
	 * difference cannot overflow.  If they are of different signs then the
	 * result should be of the same sign as the first input.
	 */
	if (!SAMESIGN(arg1, arg2) && !SAMESIGN(result, arg1))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("integer out of range")));
	MDB_RETURN_INT32(result);
}

Datum
int42mul(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	int16		arg2 = MDB_GETARG_INT16(1);
	int32		result;

	result = arg1 * arg2;

	/*
	 * Overflow check.  We basically check to see if result / arg1 gives arg2
	 * again.  There is one case where this fails: arg1 = 0 (which cannot
	 * overflow).
	 *
	 * Since the division is likely much more expensive than the actual
	 * multiplication, we'd like to skip it where possible.  The best bang for
	 * the buck seems to be to check whether both inputs are in the int16
	 * range; if so, no overflow is possible.
	 */
	if (!(arg1 >= (int32) SHRT_MIN && arg1 <= (int32) SHRT_MAX) &&
		result / arg1 != arg2)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("integer out of range")));
	MDB_RETURN_INT32(result);
}

Datum
int42div(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	int16		arg2 = MDB_GETARG_INT16(1);
	int32		result;

	if (arg2 == 0)
	{
		ereport(ERROR,
				(errcode(ERRCODE_DIVISION_BY_ZERO),
				 errmsg("division by zero")));
		/* ensure compiler realizes we mustn't reach the division (gcc bug) */
		MDB_RETURN_NULL();
	}

	/*
	 * INT_MIN / -1 is problematic, since the result can't be represented on a
	 * two's-complement machine.  Some machines produce INT_MIN, some produce
	 * zero, some throw an exception.  We can dodge the problem by recognizing
	 * that division by -1 is the same as negation.
	 */
	if (arg2 == -1)
	{
		result = -arg1;
		/* overflow check (needed for INT_MIN) */
		if (arg1 != 0 && SAMESIGN(result, arg1))
			ereport(ERROR,
					(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
					 errmsg("integer out of range")));
		MDB_RETURN_INT32(result);
	}

	/* No overflow is possible */

	result = arg1 / arg2;

	MDB_RETURN_INT32(result);
}

Datum
int4mod(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	int32		arg2 = MDB_GETARG_INT32(1);

	if (arg2 == 0)
	{
		ereport(ERROR,
				(errcode(ERRCODE_DIVISION_BY_ZERO),
				 errmsg("division by zero")));
		/* ensure compiler realizes we mustn't reach the division (gcc bug) */
		MDB_RETURN_NULL();
	}

	/*
	 * Some machines throw a floating-point exception for INT_MIN % -1, which
	 * is a bit silly since the correct answer is perfectly well-defined,
	 * namely zero.
	 */
	if (arg2 == -1)
		MDB_RETURN_INT32(0);

	/* No overflow is possible */

	MDB_RETURN_INT32(arg1 % arg2);
}

Datum
int2mod(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	int16		arg2 = MDB_GETARG_INT16(1);

	if (arg2 == 0)
	{
		ereport(ERROR,
				(errcode(ERRCODE_DIVISION_BY_ZERO),
				 errmsg("division by zero")));
		/* ensure compiler realizes we mustn't reach the division (gcc bug) */
		MDB_RETURN_NULL();
	}

	/*
	 * Some machines throw a floating-point exception for INT_MIN % -1, which
	 * is a bit silly since the correct answer is perfectly well-defined,
	 * namely zero.  (It's not clear this ever happens when dealing with
	 * int16, but we might as well have the test for safety.)
	 */
	if (arg2 == -1)
		MDB_RETURN_INT16(0);

	/* No overflow is possible */

	MDB_RETURN_INT16(arg1 % arg2);
}


/* int[24]abs()
 * Absolute value
 */
Datum
int4abs(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	int32		result;

	result = (arg1 < 0) ? -arg1 : arg1;
	/* overflow check (needed for INT_MIN) */
	if (result < 0)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("integer out of range")));
	MDB_RETURN_INT32(result);
}

Datum
int2abs(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	int16		result;

	result = (arg1 < 0) ? -arg1 : arg1;
	/* overflow check (needed for SHRT_MIN) */
	if (result < 0)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("smallint out of range")));
	MDB_RETURN_INT16(result);
}

Datum
int2larger(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	int16		arg2 = MDB_GETARG_INT16(1);

	MDB_RETURN_INT16((arg1 > arg2) ? arg1 : arg2);
}

Datum
int2smaller(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	int16		arg2 = MDB_GETARG_INT16(1);

	MDB_RETURN_INT16((arg1 < arg2) ? arg1 : arg2);
}

Datum
int4larger(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	int32		arg2 = MDB_GETARG_INT32(1);

	MDB_RETURN_INT32((arg1 > arg2) ? arg1 : arg2);
}

Datum
int4smaller(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	int32		arg2 = MDB_GETARG_INT32(1);

	MDB_RETURN_INT32((arg1 < arg2) ? arg1 : arg2);
}

/*
 * Bit-pushing operators
 *
 *		int[24]and		- returns arg1 & arg2
 *		int[24]or		- returns arg1 | arg2
 *		int[24]xor		- returns arg1 # arg2
 *		int[24]not		- returns ~arg1
 *		int[24]shl		- returns arg1 << arg2
 *		int[24]shr		- returns arg1 >> arg2
 */

Datum
int4and(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	int32		arg2 = MDB_GETARG_INT32(1);

	MDB_RETURN_INT32(arg1 & arg2);
}

Datum
int4or(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	int32		arg2 = MDB_GETARG_INT32(1);

	MDB_RETURN_INT32(arg1 | arg2);
}

Datum
int4xor(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	int32		arg2 = MDB_GETARG_INT32(1);

	MDB_RETURN_INT32(arg1 ^ arg2);
}

Datum
int4shl(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	int32		arg2 = MDB_GETARG_INT32(1);

	MDB_RETURN_INT32(arg1 << arg2);
}

Datum
int4shr(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);
	int32		arg2 = MDB_GETARG_INT32(1);

	MDB_RETURN_INT32(arg1 >> arg2);
}

Datum
int4not(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);

	MDB_RETURN_INT32(~arg1);
}

Datum
int2and(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	int16		arg2 = MDB_GETARG_INT16(1);

	MDB_RETURN_INT16(arg1 & arg2);
}

Datum
int2or(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	int16		arg2 = MDB_GETARG_INT16(1);

	MDB_RETURN_INT16(arg1 | arg2);
}

Datum
int2xor(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	int16		arg2 = MDB_GETARG_INT16(1);

	MDB_RETURN_INT16(arg1 ^ arg2);
}

Datum
int2not(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);

	MDB_RETURN_INT16(~arg1);
}


Datum
int2shl(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	int32		arg2 = MDB_GETARG_INT32(1);

	MDB_RETURN_INT16(arg1 << arg2);
}

Datum
int2shr(MDB_FUNCTION_ARGS)
{
	int16		arg1 = MDB_GETARG_INT16(0);
	int32		arg2 = MDB_GETARG_INT32(1);

	MDB_RETURN_INT16(arg1 >> arg2);
}

/*
 * non-persistent numeric series generator
 */
Datum
generate_series_int4(MDB_FUNCTION_ARGS)
{
	return generate_series_step_int4(fcinfo);
}

Datum
generate_series_step_int4(MDB_FUNCTION_ARGS)
{
	FuncCallContext *funcctx;
	generate_series_fctx *fctx;
	int32		result;
	MemoryContext oldcontext;

	/* stuff done only on the first call of the function */
	if (SRF_IS_FIRSTCALL())
	{
		int32		start = MDB_GETARG_INT32(0);
		int32		finish = MDB_GETARG_INT32(1);
		int32		step = 1;

		/* see if we were given an explicit step size */
		if (MDB_NARGS() == 3)
			step = MDB_GETARG_INT32(2);
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
		SRF_RETURN_NEXT(funcctx, Int32GetDatum(result));
	}
	else
		/* do when there is no more left */
		SRF_RETURN_DONE(funcctx);
}
