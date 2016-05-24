/*
 * contrib/ltree/_ltree_op.c
 *
 *
 * op function for ltree[]
 * Teodor Sigaev <teodor@stack.net>
 */
#include "mollydb.h"

#include <ctype.h>

#include "ltree.h"

MDB_FUNCTION_INFO_V1(_ltree_isparent);
MDB_FUNCTION_INFO_V1(_ltree_r_isparent);
MDB_FUNCTION_INFO_V1(_ltree_risparent);
MDB_FUNCTION_INFO_V1(_ltree_r_risparent);
MDB_FUNCTION_INFO_V1(_ltq_regex);
MDB_FUNCTION_INFO_V1(_ltq_rregex);
MDB_FUNCTION_INFO_V1(_lt_q_regex);
MDB_FUNCTION_INFO_V1(_lt_q_rregex);
MDB_FUNCTION_INFO_V1(_ltxtq_exec);
MDB_FUNCTION_INFO_V1(_ltxtq_rexec);

MDB_FUNCTION_INFO_V1(_ltree_extract_isparent);
MDB_FUNCTION_INFO_V1(_ltree_extract_risparent);
MDB_FUNCTION_INFO_V1(_ltq_extract_regex);
MDB_FUNCTION_INFO_V1(_ltxtq_extract_exec);

MDB_FUNCTION_INFO_V1(_lca);

typedef Datum (*PGCALL2) (MDB_FUNCTION_ARGS);

#define NEXTVAL(x) ( (ltree*)( (char*)(x) + INTALIGN( VARSIZE(x) ) ) )

static bool
array_iterator(ArrayType *la, PGCALL2 callback, void *param, ltree **found)
{
	int			num = ArrayGetNItems(ARR_NDIM(la), ARR_DIMS(la));
	ltree	   *item = (ltree *) ARR_DATA_PTR(la);

	if (ARR_NDIM(la) > 1)
		ereport(ERROR,
				(errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR),
				 errmsg("array must be one-dimensional")));
	if (array_contains_nulls(la))
		ereport(ERROR,
				(errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
				 errmsg("array must not contain nulls")));

	if (found)
		*found = NULL;
	while (num > 0)
	{
		if (DatumGetBool(DirectFunctionCall2(callback,
							 PointerGetDatum(item), PointerGetDatum(param))))
		{

			if (found)
				*found = item;
			return true;
		}
		num--;
		item = NEXTVAL(item);
	}

	return false;
}

Datum
_ltree_isparent(MDB_FUNCTION_ARGS)
{
	ArrayType  *la = MDB_GETARG_ARRAYTYPE_P(0);
	ltree	   *query = MDB_GETARG_LTREE(1);
	bool		res = array_iterator(la, ltree_isparent, (void *) query, NULL);

	MDB_FREE_IF_COPY(la, 0);
	MDB_FREE_IF_COPY(query, 1);
	MDB_RETURN_BOOL(res);
}

Datum
_ltree_r_isparent(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_DATUM(DirectFunctionCall2(_ltree_isparent,
										MDB_GETARG_DATUM(1),
										MDB_GETARG_DATUM(0)
										));
}

Datum
_ltree_risparent(MDB_FUNCTION_ARGS)
{
	ArrayType  *la = MDB_GETARG_ARRAYTYPE_P(0);
	ltree	   *query = MDB_GETARG_LTREE(1);
	bool		res = array_iterator(la, ltree_risparent, (void *) query, NULL);

	MDB_FREE_IF_COPY(la, 0);
	MDB_FREE_IF_COPY(query, 1);
	MDB_RETURN_BOOL(res);
}

Datum
_ltree_r_risparent(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_DATUM(DirectFunctionCall2(_ltree_risparent,
										MDB_GETARG_DATUM(1),
										MDB_GETARG_DATUM(0)
										));
}

Datum
_ltq_regex(MDB_FUNCTION_ARGS)
{
	ArrayType  *la = MDB_GETARG_ARRAYTYPE_P(0);
	lquery	   *query = MDB_GETARG_LQUERY(1);
	bool		res = array_iterator(la, ltq_regex, (void *) query, NULL);

	MDB_FREE_IF_COPY(la, 0);
	MDB_FREE_IF_COPY(query, 1);
	MDB_RETURN_BOOL(res);
}

Datum
_ltq_rregex(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_DATUM(DirectFunctionCall2(_ltq_regex,
										MDB_GETARG_DATUM(1),
										MDB_GETARG_DATUM(0)
										));
}

Datum
_lt_q_regex(MDB_FUNCTION_ARGS)
{
	ArrayType  *_tree = MDB_GETARG_ARRAYTYPE_P(0);
	ArrayType  *_query = MDB_GETARG_ARRAYTYPE_P(1);
	lquery	   *query = (lquery *) ARR_DATA_PTR(_query);
	bool		res = false;
	int			num = ArrayGetNItems(ARR_NDIM(_query), ARR_DIMS(_query));

	if (ARR_NDIM(_query) > 1)
		ereport(ERROR,
				(errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR),
				 errmsg("array must be one-dimensional")));
	if (array_contains_nulls(_query))
		ereport(ERROR,
				(errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
				 errmsg("array must not contain nulls")));

	while (num > 0)
	{
		if (array_iterator(_tree, ltq_regex, (void *) query, NULL))
		{
			res = true;
			break;
		}
		num--;
		query = (lquery *) NEXTVAL(query);
	}

	MDB_FREE_IF_COPY(_tree, 0);
	MDB_FREE_IF_COPY(_query, 1);
	MDB_RETURN_BOOL(res);
}

Datum
_lt_q_rregex(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_DATUM(DirectFunctionCall2(_lt_q_regex,
										MDB_GETARG_DATUM(1),
										MDB_GETARG_DATUM(0)
										));
}


Datum
_ltxtq_exec(MDB_FUNCTION_ARGS)
{
	ArrayType  *la = MDB_GETARG_ARRAYTYPE_P(0);
	ltxtquery  *query = MDB_GETARG_LTXTQUERY(1);
	bool		res = array_iterator(la, ltxtq_exec, (void *) query, NULL);

	MDB_FREE_IF_COPY(la, 0);
	MDB_FREE_IF_COPY(query, 1);
	MDB_RETURN_BOOL(res);
}

Datum
_ltxtq_rexec(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_DATUM(DirectFunctionCall2(_ltxtq_exec,
										MDB_GETARG_DATUM(1),
										MDB_GETARG_DATUM(0)
										));
}


Datum
_ltree_extract_isparent(MDB_FUNCTION_ARGS)
{
	ArrayType  *la = MDB_GETARG_ARRAYTYPE_P(0);
	ltree	   *query = MDB_GETARG_LTREE(1);
	ltree	   *found,
			   *item;

	if (!array_iterator(la, ltree_isparent, (void *) query, &found))
	{
		MDB_FREE_IF_COPY(la, 0);
		MDB_FREE_IF_COPY(query, 1);
		MDB_RETURN_NULL();
	}

	item = (ltree *) palloc0(VARSIZE(found));
	memcpy(item, found, VARSIZE(found));

	MDB_FREE_IF_COPY(la, 0);
	MDB_FREE_IF_COPY(query, 1);
	MDB_RETURN_POINTER(item);
}

Datum
_ltree_extract_risparent(MDB_FUNCTION_ARGS)
{
	ArrayType  *la = MDB_GETARG_ARRAYTYPE_P(0);
	ltree	   *query = MDB_GETARG_LTREE(1);
	ltree	   *found,
			   *item;

	if (!array_iterator(la, ltree_risparent, (void *) query, &found))
	{
		MDB_FREE_IF_COPY(la, 0);
		MDB_FREE_IF_COPY(query, 1);
		MDB_RETURN_NULL();
	}

	item = (ltree *) palloc0(VARSIZE(found));
	memcpy(item, found, VARSIZE(found));

	MDB_FREE_IF_COPY(la, 0);
	MDB_FREE_IF_COPY(query, 1);
	MDB_RETURN_POINTER(item);
}

Datum
_ltq_extract_regex(MDB_FUNCTION_ARGS)
{
	ArrayType  *la = MDB_GETARG_ARRAYTYPE_P(0);
	lquery	   *query = MDB_GETARG_LQUERY(1);
	ltree	   *found,
			   *item;

	if (!array_iterator(la, ltq_regex, (void *) query, &found))
	{
		MDB_FREE_IF_COPY(la, 0);
		MDB_FREE_IF_COPY(query, 1);
		MDB_RETURN_NULL();
	}

	item = (ltree *) palloc0(VARSIZE(found));
	memcpy(item, found, VARSIZE(found));

	MDB_FREE_IF_COPY(la, 0);
	MDB_FREE_IF_COPY(query, 1);
	MDB_RETURN_POINTER(item);
}

Datum
_ltxtq_extract_exec(MDB_FUNCTION_ARGS)
{
	ArrayType  *la = MDB_GETARG_ARRAYTYPE_P(0);
	ltxtquery  *query = MDB_GETARG_LTXTQUERY(1);
	ltree	   *found,
			   *item;

	if (!array_iterator(la, ltxtq_exec, (void *) query, &found))
	{
		MDB_FREE_IF_COPY(la, 0);
		MDB_FREE_IF_COPY(query, 1);
		MDB_RETURN_NULL();
	}

	item = (ltree *) palloc0(VARSIZE(found));
	memcpy(item, found, VARSIZE(found));

	MDB_FREE_IF_COPY(la, 0);
	MDB_FREE_IF_COPY(query, 1);
	MDB_RETURN_POINTER(item);
}

Datum
_lca(MDB_FUNCTION_ARGS)
{
	ArrayType  *la = MDB_GETARG_ARRAYTYPE_P(0);
	int			num = ArrayGetNItems(ARR_NDIM(la), ARR_DIMS(la));
	ltree	   *item = (ltree *) ARR_DATA_PTR(la);
	ltree	  **a,
			   *res;

	if (ARR_NDIM(la) > 1)
		ereport(ERROR,
				(errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR),
				 errmsg("array must be one-dimensional")));
	if (array_contains_nulls(la))
		ereport(ERROR,
				(errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
				 errmsg("array must not contain nulls")));

	a = (ltree **) palloc(sizeof(ltree *) * num);
	while (num > 0)
	{
		num--;
		a[num] = item;
		item = NEXTVAL(item);
	}
	res = lca_inner(a, ArrayGetNItems(ARR_NDIM(la), ARR_DIMS(la)));
	pfree(a);

	MDB_FREE_IF_COPY(la, 0);

	if (res)
		MDB_RETURN_POINTER(res);
	else
		MDB_RETURN_NULL();
}
