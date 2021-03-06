/*
 * contrib/intarray/_int_op.c
 */
#include "mollydb.h"


#include "_int.h"

MDB_MODULE_MAGIC;

MDB_FUNCTION_INFO_V1(_int_different);
MDB_FUNCTION_INFO_V1(_int_same);
MDB_FUNCTION_INFO_V1(_int_contains);
MDB_FUNCTION_INFO_V1(_int_contained);
MDB_FUNCTION_INFO_V1(_int_overlap);
MDB_FUNCTION_INFO_V1(_int_union);
MDB_FUNCTION_INFO_V1(_int_inter);

Datum
_int_contained(MDB_FUNCTION_ARGS)
{
	/* just reverse the operands and call _int_contains */
	return DirectFunctionCall2(_int_contains,
							   MDB_GETARG_DATUM(1),
							   MDB_GETARG_DATUM(0));
}

Datum
_int_contains(MDB_FUNCTION_ARGS)
{
	/* Force copy so we can modify the arrays in-place */
	ArrayType  *a = MDB_GETARG_ARRAYTYPE_P_COPY(0);
	ArrayType  *b = MDB_GETARG_ARRAYTYPE_P_COPY(1);
	bool		res;

	CHECKARRVALID(a);
	CHECKARRVALID(b);
	PREPAREARR(a);
	PREPAREARR(b);
	res = inner_int_contains(a, b);
	pfree(a);
	pfree(b);
	MDB_RETURN_BOOL(res);
}

Datum
_int_different(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_BOOL(!DatumGetBool(
								 DirectFunctionCall2(
													 _int_same,
									   PointerGetDatum(MDB_GETARG_POINTER(0)),
										PointerGetDatum(MDB_GETARG_POINTER(1))
													 )
								 ));
}

Datum
_int_same(MDB_FUNCTION_ARGS)
{
	ArrayType  *a = MDB_GETARG_ARRAYTYPE_P_COPY(0);
	ArrayType  *b = MDB_GETARG_ARRAYTYPE_P_COPY(1);
	int			na,
				nb;
	int			n;
	int		   *da,
			   *db;
	bool		result;

	CHECKARRVALID(a);
	CHECKARRVALID(b);
	na = ARRNELEMS(a);
	nb = ARRNELEMS(b);
	da = ARRPTR(a);
	db = ARRPTR(b);

	result = FALSE;

	if (na == nb)
	{
		SORT(a);
		SORT(b);
		result = TRUE;

		for (n = 0; n < na; n++)
		{
			if (da[n] != db[n])
			{
				result = FALSE;
				break;
			}
		}
	}

	pfree(a);
	pfree(b);

	MDB_RETURN_BOOL(result);
}

/*	_int_overlap -- does a overlap b?
 */
Datum
_int_overlap(MDB_FUNCTION_ARGS)
{
	ArrayType  *a = MDB_GETARG_ARRAYTYPE_P_COPY(0);
	ArrayType  *b = MDB_GETARG_ARRAYTYPE_P_COPY(1);
	bool		result;

	CHECKARRVALID(a);
	CHECKARRVALID(b);
	if (ARRISEMPTY(a) || ARRISEMPTY(b))
		return FALSE;

	SORT(a);
	SORT(b);

	result = inner_int_overlap(a, b);

	pfree(a);
	pfree(b);

	MDB_RETURN_BOOL(result);
}

Datum
_int_union(MDB_FUNCTION_ARGS)
{
	ArrayType  *a = MDB_GETARG_ARRAYTYPE_P_COPY(0);
	ArrayType  *b = MDB_GETARG_ARRAYTYPE_P_COPY(1);
	ArrayType  *result;

	CHECKARRVALID(a);
	CHECKARRVALID(b);

	SORT(a);
	SORT(b);

	result = inner_int_union(a, b);

	pfree(a);
	pfree(b);

	MDB_RETURN_POINTER(result);
}

Datum
_int_inter(MDB_FUNCTION_ARGS)
{
	ArrayType  *a = MDB_GETARG_ARRAYTYPE_P_COPY(0);
	ArrayType  *b = MDB_GETARG_ARRAYTYPE_P_COPY(1);
	ArrayType  *result;

	CHECKARRVALID(a);
	CHECKARRVALID(b);

	SORT(a);
	SORT(b);

	result = inner_int_inter(a, b);

	pfree(a);
	pfree(b);

	MDB_RETURN_POINTER(result);
}


MDB_FUNCTION_INFO_V1(intset);
MDB_FUNCTION_INFO_V1(icount);
MDB_FUNCTION_INFO_V1(sort);
MDB_FUNCTION_INFO_V1(sort_asc);
MDB_FUNCTION_INFO_V1(sort_desc);
MDB_FUNCTION_INFO_V1(uniq);
MDB_FUNCTION_INFO_V1(idx);
MDB_FUNCTION_INFO_V1(subarray);
MDB_FUNCTION_INFO_V1(intarray_push_elem);
MDB_FUNCTION_INFO_V1(intarray_push_array);
MDB_FUNCTION_INFO_V1(intarray_del_elem);
MDB_FUNCTION_INFO_V1(intset_union_elem);
MDB_FUNCTION_INFO_V1(intset_subtract);

Datum
intset(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_POINTER(int_to_intset(MDB_GETARG_INT32(0)));
}

Datum
icount(MDB_FUNCTION_ARGS)
{
	ArrayType  *a = MDB_GETARG_ARRAYTYPE_P(0);
	int32		count = ARRNELEMS(a);

	MDB_FREE_IF_COPY(a, 0);
	MDB_RETURN_INT32(count);
}

Datum
sort(MDB_FUNCTION_ARGS)
{
	ArrayType  *a = MDB_GETARG_ARRAYTYPE_P_COPY(0);
	text	   *dirstr = (fcinfo->nargs == 2) ? MDB_GETARG_TEXT_P(1) : NULL;
	int32		dc = (dirstr) ? VARSIZE(dirstr) - VARHDRSZ : 0;
	char	   *d = (dirstr) ? VARDATA(dirstr) : NULL;
	int			dir = -1;

	CHECKARRVALID(a);
	if (ARRNELEMS(a) < 2)
		MDB_RETURN_POINTER(a);

	if (dirstr == NULL || (dc == 3
						   && (d[0] == 'A' || d[0] == 'a')
						   && (d[1] == 'S' || d[1] == 's')
						   && (d[2] == 'C' || d[2] == 'c')))
		dir = 1;
	else if (dc == 4
			 && (d[0] == 'D' || d[0] == 'd')
			 && (d[1] == 'E' || d[1] == 'e')
			 && (d[2] == 'S' || d[2] == 's')
			 && (d[3] == 'C' || d[3] == 'c'))
		dir = 0;
	if (dir == -1)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				 errmsg("second parameter must be \"ASC\" or \"DESC\"")));
	QSORT(a, dir);
	MDB_RETURN_POINTER(a);
}

Datum
sort_asc(MDB_FUNCTION_ARGS)
{
	ArrayType  *a = MDB_GETARG_ARRAYTYPE_P_COPY(0);

	CHECKARRVALID(a);
	QSORT(a, 1);
	MDB_RETURN_POINTER(a);
}

Datum
sort_desc(MDB_FUNCTION_ARGS)
{
	ArrayType  *a = MDB_GETARG_ARRAYTYPE_P_COPY(0);

	CHECKARRVALID(a);
	QSORT(a, 0);
	MDB_RETURN_POINTER(a);
}

Datum
uniq(MDB_FUNCTION_ARGS)
{
	ArrayType  *a = MDB_GETARG_ARRAYTYPE_P_COPY(0);

	CHECKARRVALID(a);
	if (ARRNELEMS(a) < 2)
		MDB_RETURN_POINTER(a);
	a = _int_unique(a);
	MDB_RETURN_POINTER(a);
}

Datum
idx(MDB_FUNCTION_ARGS)
{
	ArrayType  *a = MDB_GETARG_ARRAYTYPE_P(0);
	int32		result;

	CHECKARRVALID(a);
	result = ARRNELEMS(a);
	if (result)
		result = intarray_match_first(a, MDB_GETARG_INT32(1));
	MDB_FREE_IF_COPY(a, 0);
	MDB_RETURN_INT32(result);
}

Datum
subarray(MDB_FUNCTION_ARGS)
{
	ArrayType  *a = MDB_GETARG_ARRAYTYPE_P(0);
	int32		start = MDB_GETARG_INT32(1);
	int32		len = (fcinfo->nargs == 3) ? MDB_GETARG_INT32(2) : 0;
	int32		end = 0;
	int32		c;
	ArrayType  *result;

	start = (start > 0) ? start - 1 : start;

	CHECKARRVALID(a);
	if (ARRISEMPTY(a))
	{
		MDB_FREE_IF_COPY(a, 0);
		MDB_RETURN_POINTER(new_intArrayType(0));
	}

	c = ARRNELEMS(a);

	if (start < 0)
		start = c + start;

	if (len < 0)
		end = c + len;
	else if (len == 0)
		end = c;
	else
		end = start + len;

	if (end > c)
		end = c;

	if (start < 0)
		start = 0;

	if (start >= end || end <= 0)
	{
		MDB_FREE_IF_COPY(a, 0);
		MDB_RETURN_POINTER(new_intArrayType(0));
	}

	result = new_intArrayType(end - start);
	if (end - start > 0)
		memcpy(ARRPTR(result), ARRPTR(a) + start, (end - start) * sizeof(int32));
	MDB_FREE_IF_COPY(a, 0);
	MDB_RETURN_POINTER(result);
}

Datum
intarray_push_elem(MDB_FUNCTION_ARGS)
{
	ArrayType  *a = MDB_GETARG_ARRAYTYPE_P(0);
	ArrayType  *result;

	result = intarray_add_elem(a, MDB_GETARG_INT32(1));
	MDB_FREE_IF_COPY(a, 0);
	MDB_RETURN_POINTER(result);
}

Datum
intarray_push_array(MDB_FUNCTION_ARGS)
{
	ArrayType  *a = MDB_GETARG_ARRAYTYPE_P(0);
	ArrayType  *b = MDB_GETARG_ARRAYTYPE_P(1);
	ArrayType  *result;

	result = intarray_concat_arrays(a, b);
	MDB_FREE_IF_COPY(a, 0);
	MDB_FREE_IF_COPY(b, 1);
	MDB_RETURN_POINTER(result);
}

Datum
intarray_del_elem(MDB_FUNCTION_ARGS)
{
	ArrayType  *a = MDB_GETARG_ARRAYTYPE_P_COPY(0);
	int32		elem = MDB_GETARG_INT32(1);
	int32		c;
	int32	   *aa;
	int32		n = 0,
				i;

	CHECKARRVALID(a);
	if (!ARRISEMPTY(a))
	{
		c = ARRNELEMS(a);
		aa = ARRPTR(a);
		for (i = 0; i < c; i++)
		{
			if (aa[i] != elem)
			{
				if (i > n)
					aa[n++] = aa[i];
				else
					n++;
			}
		}
		a = resize_intArrayType(a, n);
	}
	MDB_RETURN_POINTER(a);
}

Datum
intset_union_elem(MDB_FUNCTION_ARGS)
{
	ArrayType  *a = MDB_GETARG_ARRAYTYPE_P(0);
	ArrayType  *result;

	result = intarray_add_elem(a, MDB_GETARG_INT32(1));
	MDB_FREE_IF_COPY(a, 0);
	QSORT(result, 1);
	MDB_RETURN_POINTER(_int_unique(result));
}

Datum
intset_subtract(MDB_FUNCTION_ARGS)
{
	ArrayType  *a = MDB_GETARG_ARRAYTYPE_P_COPY(0);
	ArrayType  *b = MDB_GETARG_ARRAYTYPE_P_COPY(1);
	ArrayType  *result;
	int32		ca;
	int32		cb;
	int32	   *aa,
			   *bb,
			   *r;
	int32		n = 0,
				i = 0,
				k = 0;

	CHECKARRVALID(a);
	CHECKARRVALID(b);

	QSORT(a, 1);
	a = _int_unique(a);
	ca = ARRNELEMS(a);
	QSORT(b, 1);
	b = _int_unique(b);
	cb = ARRNELEMS(b);
	result = new_intArrayType(ca);
	aa = ARRPTR(a);
	bb = ARRPTR(b);
	r = ARRPTR(result);
	while (i < ca)
	{
		if (k == cb || aa[i] < bb[k])
			r[n++] = aa[i++];
		else if (aa[i] == bb[k])
		{
			i++;
			k++;
		}
		else
			k++;
	}
	result = resize_intArrayType(result, n);
	pfree(a);
	pfree(b);
	MDB_RETURN_POINTER(result);
}
