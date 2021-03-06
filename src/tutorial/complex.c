/*
 * src/tutorial/complex.c
 *
 ******************************************************************************
  This file contains routines that can be bound to a MollyDB backend and
  called by the backend in the process of processing queries.  The calling
  format for these routines is dictated by MollyDB architecture.
******************************************************************************/

#include "mollydb.h"

#include "fmgr.h"
#include "libpq/pqformat.h"		/* needed for send/recv functions */

MDB_MODULE_MAGIC;

typedef struct Complex
{
	double		x;
	double		y;
}	Complex;


/*****************************************************************************
 * Input/Output functions
 *****************************************************************************/

MDB_FUNCTION_INFO_V1(complex_in);

Datum
complex_in(MDB_FUNCTION_ARGS)
{
	char	   *str = MDB_GETARG_CSTRING(0);
	double		x,
				y;
	Complex    *result;

	if (sscanf(str, " ( %lf , %lf )", &x, &y) != 2)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for complex: \"%s\"",
						str)));

	result = (Complex *) palloc(sizeof(Complex));
	result->x = x;
	result->y = y;
	MDB_RETURN_POINTER(result);
}

MDB_FUNCTION_INFO_V1(complex_out);

Datum
complex_out(MDB_FUNCTION_ARGS)
{
	Complex    *complex = (Complex *) MDB_GETARG_POINTER(0);
	char	   *result;

	result = psprintf("(%g,%g)", complex->x, complex->y);
	MDB_RETURN_CSTRING(result);
}

/*****************************************************************************
 * Binary Input/Output functions
 *
 * These are optional.
 *****************************************************************************/

MDB_FUNCTION_INFO_V1(complex_recv);

Datum
complex_recv(MDB_FUNCTION_ARGS)
{
	StringInfo	buf = (StringInfo) MDB_GETARG_POINTER(0);
	Complex    *result;

	result = (Complex *) palloc(sizeof(Complex));
	result->x = pq_getmsgfloat8(buf);
	result->y = pq_getmsgfloat8(buf);
	MDB_RETURN_POINTER(result);
}

MDB_FUNCTION_INFO_V1(complex_send);

Datum
complex_send(MDB_FUNCTION_ARGS)
{
	Complex    *complex = (Complex *) MDB_GETARG_POINTER(0);
	StringInfoData buf;

	pq_begintypsend(&buf);
	pq_sendfloat8(&buf, complex->x);
	pq_sendfloat8(&buf, complex->y);
	MDB_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

/*****************************************************************************
 * New Operators
 *
 * A practical Complex datatype would provide much more than this, of course.
 *****************************************************************************/

MDB_FUNCTION_INFO_V1(complex_add);

Datum
complex_add(MDB_FUNCTION_ARGS)
{
	Complex    *a = (Complex *) MDB_GETARG_POINTER(0);
	Complex    *b = (Complex *) MDB_GETARG_POINTER(1);
	Complex    *result;

	result = (Complex *) palloc(sizeof(Complex));
	result->x = a->x + b->x;
	result->y = a->y + b->y;
	MDB_RETURN_POINTER(result);
}


/*****************************************************************************
 * Operator class for defining B-tree index
 *
 * It's essential that the comparison operators and support function for a
 * B-tree index opclass always agree on the relative ordering of any two
 * data values.  Experience has shown that it's depressingly easy to write
 * unintentionally inconsistent functions.  One way to reduce the odds of
 * making a mistake is to make all the functions simple wrappers around
 * an internal three-way-comparison function, as we do here.
 *****************************************************************************/

#define Mag(c)	((c)->x*(c)->x + (c)->y*(c)->y)

static int
complex_abs_cmp_internal(Complex * a, Complex * b)
{
	double		amag = Mag(a),
				bmag = Mag(b);

	if (amag < bmag)
		return -1;
	if (amag > bmag)
		return 1;
	return 0;
}


MDB_FUNCTION_INFO_V1(complex_abs_lt);

Datum
complex_abs_lt(MDB_FUNCTION_ARGS)
{
	Complex    *a = (Complex *) MDB_GETARG_POINTER(0);
	Complex    *b = (Complex *) MDB_GETARG_POINTER(1);

	MDB_RETURN_BOOL(complex_abs_cmp_internal(a, b) < 0);
}

MDB_FUNCTION_INFO_V1(complex_abs_le);

Datum
complex_abs_le(MDB_FUNCTION_ARGS)
{
	Complex    *a = (Complex *) MDB_GETARG_POINTER(0);
	Complex    *b = (Complex *) MDB_GETARG_POINTER(1);

	MDB_RETURN_BOOL(complex_abs_cmp_internal(a, b) <= 0);
}

MDB_FUNCTION_INFO_V1(complex_abs_eq);

Datum
complex_abs_eq(MDB_FUNCTION_ARGS)
{
	Complex    *a = (Complex *) MDB_GETARG_POINTER(0);
	Complex    *b = (Complex *) MDB_GETARG_POINTER(1);

	MDB_RETURN_BOOL(complex_abs_cmp_internal(a, b) == 0);
}

MDB_FUNCTION_INFO_V1(complex_abs_ge);

Datum
complex_abs_ge(MDB_FUNCTION_ARGS)
{
	Complex    *a = (Complex *) MDB_GETARG_POINTER(0);
	Complex    *b = (Complex *) MDB_GETARG_POINTER(1);

	MDB_RETURN_BOOL(complex_abs_cmp_internal(a, b) >= 0);
}

MDB_FUNCTION_INFO_V1(complex_abs_gt);

Datum
complex_abs_gt(MDB_FUNCTION_ARGS)
{
	Complex    *a = (Complex *) MDB_GETARG_POINTER(0);
	Complex    *b = (Complex *) MDB_GETARG_POINTER(1);

	MDB_RETURN_BOOL(complex_abs_cmp_internal(a, b) > 0);
}

MDB_FUNCTION_INFO_V1(complex_abs_cmp);

Datum
complex_abs_cmp(MDB_FUNCTION_ARGS)
{
	Complex    *a = (Complex *) MDB_GETARG_POINTER(0);
	Complex    *b = (Complex *) MDB_GETARG_POINTER(1);

	MDB_RETURN_INT32(complex_abs_cmp_internal(a, b));
}
