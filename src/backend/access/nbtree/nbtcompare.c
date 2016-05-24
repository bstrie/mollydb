/*-------------------------------------------------------------------------
 *
 * nbtcompare.c
 *	  Comparison functions for btree access method.
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/access/nbtree/nbtcompare.c
 *
 * NOTES
 *
 *	These functions are stored in mdb_amproc.  For each operator class
 *	defined on btrees, they compute
 *
 *				compare(a, b):
 *						< 0 if a < b,
 *						= 0 if a == b,
 *						> 0 if a > b.
 *
 *	The result is always an int32 regardless of the input datatype.
 *
 *	Although any negative int32 (except INT_MIN) is acceptable for reporting
 *	"<", and any positive int32 is acceptable for reporting ">", routines
 *	that work on 32-bit or wider datatypes can't just return "a - b".
 *	That could overflow and give the wrong answer.  Also, one must not
 *	return INT_MIN to report "<", since some callers will negate the result.
 *
 *	NOTE: it is critical that the comparison function impose a total order
 *	on all non-NULL values of the data type, and that the datatype's
 *	boolean comparison operators (= < >= etc) yield results consistent
 *	with the comparison routine.  Otherwise bad behavior may ensue.
 *	(For example, the comparison operators must NOT punt when faced with
 *	NAN or other funny values; you must devise some collation sequence for
 *	all such values.)  If the datatype is not trivial, this is most
 *	reliably done by having the boolean operators invoke the same
 *	three-way comparison code that the btree function does.  Therefore,
 *	this file contains only btree support for "trivial" datatypes ---
 *	all others are in the /utils/adt/ files that implement their datatypes.
 *
 *	NOTE: these routines must not leak memory, since memory allocated
 *	during an index access won't be recovered till end of query.  This
 *	primarily affects comparison routines for toastable datatypes;
 *	they have to be careful to free any detoasted copy of an input datum.
 *-------------------------------------------------------------------------
 */
#include "mollydb.h"

#include "utils/builtins.h"
#include "utils/sortsupport.h"


Datum
btboolcmp(MDB_FUNCTION_ARGS)
{
	bool		a = MDB_GETARG_BOOL(0);
	bool		b = MDB_GETARG_BOOL(1);

	MDB_RETURN_INT32((int32) a - (int32) b);
}

Datum
btint2cmp(MDB_FUNCTION_ARGS)
{
	int16		a = MDB_GETARG_INT16(0);
	int16		b = MDB_GETARG_INT16(1);

	MDB_RETURN_INT32((int32) a - (int32) b);
}

static int
btint2fastcmp(Datum x, Datum y, SortSupport ssup)
{
	int16		a = DatumGetInt16(x);
	int16		b = DatumGetInt16(y);

	return (int) a - (int) b;
}

Datum
btint2sortsupport(MDB_FUNCTION_ARGS)
{
	SortSupport ssup = (SortSupport) MDB_GETARG_POINTER(0);

	ssup->comparator = btint2fastcmp;
	MDB_RETURN_VOID();
}

Datum
btint4cmp(MDB_FUNCTION_ARGS)
{
	int32		a = MDB_GETARG_INT32(0);
	int32		b = MDB_GETARG_INT32(1);

	if (a > b)
		MDB_RETURN_INT32(1);
	else if (a == b)
		MDB_RETURN_INT32(0);
	else
		MDB_RETURN_INT32(-1);
}

static int
btint4fastcmp(Datum x, Datum y, SortSupport ssup)
{
	int32		a = DatumGetInt32(x);
	int32		b = DatumGetInt32(y);

	if (a > b)
		return 1;
	else if (a == b)
		return 0;
	else
		return -1;
}

Datum
btint4sortsupport(MDB_FUNCTION_ARGS)
{
	SortSupport ssup = (SortSupport) MDB_GETARG_POINTER(0);

	ssup->comparator = btint4fastcmp;
	MDB_RETURN_VOID();
}

Datum
btint8cmp(MDB_FUNCTION_ARGS)
{
	int64		a = MDB_GETARG_INT64(0);
	int64		b = MDB_GETARG_INT64(1);

	if (a > b)
		MDB_RETURN_INT32(1);
	else if (a == b)
		MDB_RETURN_INT32(0);
	else
		MDB_RETURN_INT32(-1);
}

static int
btint8fastcmp(Datum x, Datum y, SortSupport ssup)
{
	int64		a = DatumGetInt64(x);
	int64		b = DatumGetInt64(y);

	if (a > b)
		return 1;
	else if (a == b)
		return 0;
	else
		return -1;
}

Datum
btint8sortsupport(MDB_FUNCTION_ARGS)
{
	SortSupport ssup = (SortSupport) MDB_GETARG_POINTER(0);

	ssup->comparator = btint8fastcmp;
	MDB_RETURN_VOID();
}

Datum
btint48cmp(MDB_FUNCTION_ARGS)
{
	int32		a = MDB_GETARG_INT32(0);
	int64		b = MDB_GETARG_INT64(1);

	if (a > b)
		MDB_RETURN_INT32(1);
	else if (a == b)
		MDB_RETURN_INT32(0);
	else
		MDB_RETURN_INT32(-1);
}

Datum
btint84cmp(MDB_FUNCTION_ARGS)
{
	int64		a = MDB_GETARG_INT64(0);
	int32		b = MDB_GETARG_INT32(1);

	if (a > b)
		MDB_RETURN_INT32(1);
	else if (a == b)
		MDB_RETURN_INT32(0);
	else
		MDB_RETURN_INT32(-1);
}

Datum
btint24cmp(MDB_FUNCTION_ARGS)
{
	int16		a = MDB_GETARG_INT16(0);
	int32		b = MDB_GETARG_INT32(1);

	if (a > b)
		MDB_RETURN_INT32(1);
	else if (a == b)
		MDB_RETURN_INT32(0);
	else
		MDB_RETURN_INT32(-1);
}

Datum
btint42cmp(MDB_FUNCTION_ARGS)
{
	int32		a = MDB_GETARG_INT32(0);
	int16		b = MDB_GETARG_INT16(1);

	if (a > b)
		MDB_RETURN_INT32(1);
	else if (a == b)
		MDB_RETURN_INT32(0);
	else
		MDB_RETURN_INT32(-1);
}

Datum
btint28cmp(MDB_FUNCTION_ARGS)
{
	int16		a = MDB_GETARG_INT16(0);
	int64		b = MDB_GETARG_INT64(1);

	if (a > b)
		MDB_RETURN_INT32(1);
	else if (a == b)
		MDB_RETURN_INT32(0);
	else
		MDB_RETURN_INT32(-1);
}

Datum
btint82cmp(MDB_FUNCTION_ARGS)
{
	int64		a = MDB_GETARG_INT64(0);
	int16		b = MDB_GETARG_INT16(1);

	if (a > b)
		MDB_RETURN_INT32(1);
	else if (a == b)
		MDB_RETURN_INT32(0);
	else
		MDB_RETURN_INT32(-1);
}

Datum
btoidcmp(MDB_FUNCTION_ARGS)
{
	Oid			a = MDB_GETARG_OID(0);
	Oid			b = MDB_GETARG_OID(1);

	if (a > b)
		MDB_RETURN_INT32(1);
	else if (a == b)
		MDB_RETURN_INT32(0);
	else
		MDB_RETURN_INT32(-1);
}

static int
btoidfastcmp(Datum x, Datum y, SortSupport ssup)
{
	Oid			a = DatumGetObjectId(x);
	Oid			b = DatumGetObjectId(y);

	if (a > b)
		return 1;
	else if (a == b)
		return 0;
	else
		return -1;
}

Datum
btoidsortsupport(MDB_FUNCTION_ARGS)
{
	SortSupport ssup = (SortSupport) MDB_GETARG_POINTER(0);

	ssup->comparator = btoidfastcmp;
	MDB_RETURN_VOID();
}

Datum
btoidvectorcmp(MDB_FUNCTION_ARGS)
{
	oidvector  *a = (oidvector *) MDB_GETARG_POINTER(0);
	oidvector  *b = (oidvector *) MDB_GETARG_POINTER(1);
	int			i;

	/* We arbitrarily choose to sort first by vector length */
	if (a->dim1 != b->dim1)
		MDB_RETURN_INT32(a->dim1 - b->dim1);

	for (i = 0; i < a->dim1; i++)
	{
		if (a->values[i] != b->values[i])
		{
			if (a->values[i] > b->values[i])
				MDB_RETURN_INT32(1);
			else
				MDB_RETURN_INT32(-1);
		}
	}
	MDB_RETURN_INT32(0);
}

Datum
btcharcmp(MDB_FUNCTION_ARGS)
{
	char		a = MDB_GETARG_CHAR(0);
	char		b = MDB_GETARG_CHAR(1);

	/* Be careful to compare chars as unsigned */
	MDB_RETURN_INT32((int32) ((uint8) a) - (int32) ((uint8) b));
}

Datum
btnamecmp(MDB_FUNCTION_ARGS)
{
	Name		a = MDB_GETARG_NAME(0);
	Name		b = MDB_GETARG_NAME(1);

	MDB_RETURN_INT32(strncmp(NameStr(*a), NameStr(*b), NAMEDATALEN));
}

static int
btnamefastcmp(Datum x, Datum y, SortSupport ssup)
{
	Name		a = DatumGetName(x);
	Name		b = DatumGetName(y);

	return strncmp(NameStr(*a), NameStr(*b), NAMEDATALEN);
}

Datum
btnamesortsupport(MDB_FUNCTION_ARGS)
{
	SortSupport ssup = (SortSupport) MDB_GETARG_POINTER(0);

	ssup->comparator = btnamefastcmp;
	MDB_RETURN_VOID();
}
