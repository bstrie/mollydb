/* src/tutorial/funcs_new.c */

/******************************************************************************
  These are user-defined functions that can be bound to a MollyDB backend
  and called by MollyDB to execute SQL functions of the same name.

  The calling format for these functions is defined by the CREATE FUNCTION
  SQL statement that binds them to the backend.

  NOTE: this file shows examples of "new style" function call conventions.
  See funcs.c for examples of "old style".
*****************************************************************************/

#include "mollydb.h"			/* general MollyDB declarations */

#include "executor/executor.h"	/* for GetAttributeByName() */
#include "utils/geo_decls.h"	/* for point type */

MDB_MODULE_MAGIC;


/* By Value */

MDB_FUNCTION_INFO_V1(add_one);

Datum
add_one(MDB_FUNCTION_ARGS)
{
	int32		arg = MDB_GETARG_INT32(0);

	MDB_RETURN_INT32(arg + 1);
}

/* By Reference, Fixed Length */

MDB_FUNCTION_INFO_V1(add_one_float8);

Datum
add_one_float8(MDB_FUNCTION_ARGS)
{
	/* The macros for FLOAT8 hide its pass-by-reference nature */
	float8		arg = MDB_GETARG_FLOAT8(0);

	MDB_RETURN_FLOAT8(arg + 1.0);
}

MDB_FUNCTION_INFO_V1(makepoint);

Datum
makepoint(MDB_FUNCTION_ARGS)
{
	Point	   *pointx = MDB_GETARG_POINT_P(0);
	Point	   *pointy = MDB_GETARG_POINT_P(1);
	Point	   *new_point = (Point *) palloc(sizeof(Point));

	new_point->x = pointx->x;
	new_point->y = pointy->y;

	MDB_RETURN_POINT_P(new_point);
}

/* By Reference, Variable Length */

MDB_FUNCTION_INFO_V1(copytext);

Datum
copytext(MDB_FUNCTION_ARGS)
{
	text	   *t = MDB_GETARG_TEXT_P(0);

	/*
	 * VARSIZE is the total size of the struct in bytes.
	 */
	text	   *new_t = (text *) palloc(VARSIZE(t));

	SET_VARSIZE(new_t, VARSIZE(t));

	/*
	 * VARDATA is a pointer to the data region of the struct.
	 */
	memcpy((void *) VARDATA(new_t),		/* destination */
		   (void *) VARDATA(t), /* source */
		   VARSIZE(t) - VARHDRSZ);		/* how many bytes */
	MDB_RETURN_TEXT_P(new_t);
}

MDB_FUNCTION_INFO_V1(concat_text);

Datum
concat_text(MDB_FUNCTION_ARGS)
{
	text	   *arg1 = MDB_GETARG_TEXT_P(0);
	text	   *arg2 = MDB_GETARG_TEXT_P(1);
	int32		arg1_size = VARSIZE(arg1) - VARHDRSZ;
	int32		arg2_size = VARSIZE(arg2) - VARHDRSZ;
	int32		new_text_size = arg1_size + arg2_size + VARHDRSZ;
	text	   *new_text = (text *) palloc(new_text_size);

	SET_VARSIZE(new_text, new_text_size);
	memcpy(VARDATA(new_text), VARDATA(arg1), arg1_size);
	memcpy(VARDATA(new_text) + arg1_size, VARDATA(arg2), arg2_size);
	MDB_RETURN_TEXT_P(new_text);
}

/* Composite types */

MDB_FUNCTION_INFO_V1(c_overpaid);

Datum
c_overpaid(MDB_FUNCTION_ARGS)
{
	HeapTupleHeader t = MDB_GETARG_HEAPTUPLEHEADER(0);
	int32		limit = MDB_GETARG_INT32(1);
	bool		isnull;
	int32		salary;

	salary = DatumGetInt32(GetAttributeByName(t, "salary", &isnull));
	if (isnull)
		MDB_RETURN_BOOL(false);

	/*
	 * Alternatively, we might prefer to do MDB_RETURN_NULL() for null salary
	 */

	MDB_RETURN_BOOL(salary > limit);
}
