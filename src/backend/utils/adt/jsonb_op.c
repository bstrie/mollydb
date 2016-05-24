/*-------------------------------------------------------------------------
 *
 * jsonb_op.c
 *	 Special operators for jsonb only, used by various index access methods
 *
 * Copyright (c) 2014-2016, MollyDB Global Development Group
 *
 *
 * IDENTIFICATION
 *	  src/backend/utils/adt/jsonb_op.c
 *
 *-------------------------------------------------------------------------
 */
#include "mollydb.h"

#include "catalog/mdb_type.h"
#include "miscadmin.h"
#include "utils/jsonb.h"

Datum
jsonb_exists(MDB_FUNCTION_ARGS)
{
	Jsonb	   *jb = MDB_GETARG_JSONB(0);
	text	   *key = MDB_GETARG_TEXT_PP(1);
	JsonbValue	kval;
	JsonbValue *v = NULL;

	/*
	 * We only match Object keys (which are naturally always Strings), or
	 * string elements in arrays.  In particular, we do not match non-string
	 * scalar elements.  Existence of a key/element is only considered at the
	 * top level.  No recursion occurs.
	 */
	kval.type = jbvString;
	kval.val.string.val = VARDATA_ANY(key);
	kval.val.string.len = VARSIZE_ANY_EXHDR(key);

	v = findJsonbValueFromContainer(&jb->root,
									JB_FOBJECT | JB_FARRAY,
									&kval);

	MDB_RETURN_BOOL(v != NULL);
}

Datum
jsonb_exists_any(MDB_FUNCTION_ARGS)
{
	Jsonb	   *jb = MDB_GETARG_JSONB(0);
	ArrayType  *keys = MDB_GETARG_ARRAYTYPE_P(1);
	int			i;
	Datum	   *key_datums;
	bool	   *key_nulls;
	int			elem_count;

	deconstruct_array(keys, TEXTOID, -1, false, 'i', &key_datums, &key_nulls,
					  &elem_count);

	for (i = 0; i < elem_count; i++)
	{
		JsonbValue	strVal;

		if (key_nulls[i])
			continue;

		strVal.type = jbvString;
		strVal.val.string.val = VARDATA(key_datums[i]);
		strVal.val.string.len = VARSIZE(key_datums[i]) - VARHDRSZ;

		if (findJsonbValueFromContainer(&jb->root,
										JB_FOBJECT | JB_FARRAY,
										&strVal) != NULL)
			MDB_RETURN_BOOL(true);
	}

	MDB_RETURN_BOOL(false);
}

Datum
jsonb_exists_all(MDB_FUNCTION_ARGS)
{
	Jsonb	   *jb = MDB_GETARG_JSONB(0);
	ArrayType  *keys = MDB_GETARG_ARRAYTYPE_P(1);
	int			i;
	Datum	   *key_datums;
	bool	   *key_nulls;
	int			elem_count;

	deconstruct_array(keys, TEXTOID, -1, false, 'i', &key_datums, &key_nulls,
					  &elem_count);

	for (i = 0; i < elem_count; i++)
	{
		JsonbValue	strVal;

		if (key_nulls[i])
			continue;

		strVal.type = jbvString;
		strVal.val.string.val = VARDATA(key_datums[i]);
		strVal.val.string.len = VARSIZE(key_datums[i]) - VARHDRSZ;

		if (findJsonbValueFromContainer(&jb->root,
										JB_FOBJECT | JB_FARRAY,
										&strVal) == NULL)
			MDB_RETURN_BOOL(false);
	}

	MDB_RETURN_BOOL(true);
}

Datum
jsonb_contains(MDB_FUNCTION_ARGS)
{
	Jsonb	   *val = MDB_GETARG_JSONB(0);
	Jsonb	   *tmpl = MDB_GETARG_JSONB(1);

	JsonbIterator *it1,
			   *it2;

	if (JB_ROOT_IS_OBJECT(val) != JB_ROOT_IS_OBJECT(tmpl))
		MDB_RETURN_BOOL(false);

	it1 = JsonbIteratorInit(&val->root);
	it2 = JsonbIteratorInit(&tmpl->root);

	MDB_RETURN_BOOL(JsonbDeepContains(&it1, &it2));
}

Datum
jsonb_contained(MDB_FUNCTION_ARGS)
{
	/* Commutator of "contains" */
	Jsonb	   *tmpl = MDB_GETARG_JSONB(0);
	Jsonb	   *val = MDB_GETARG_JSONB(1);

	JsonbIterator *it1,
			   *it2;

	if (JB_ROOT_IS_OBJECT(val) != JB_ROOT_IS_OBJECT(tmpl))
		MDB_RETURN_BOOL(false);

	it1 = JsonbIteratorInit(&val->root);
	it2 = JsonbIteratorInit(&tmpl->root);

	MDB_RETURN_BOOL(JsonbDeepContains(&it1, &it2));
}

Datum
jsonb_ne(MDB_FUNCTION_ARGS)
{
	Jsonb	   *jba = MDB_GETARG_JSONB(0);
	Jsonb	   *jbb = MDB_GETARG_JSONB(1);
	bool		res;

	res = (compareJsonbContainers(&jba->root, &jbb->root) != 0);

	MDB_FREE_IF_COPY(jba, 0);
	MDB_FREE_IF_COPY(jbb, 1);
	MDB_RETURN_BOOL(res);
}

/*
 * B-Tree operator class operators, support function
 */
Datum
jsonb_lt(MDB_FUNCTION_ARGS)
{
	Jsonb	   *jba = MDB_GETARG_JSONB(0);
	Jsonb	   *jbb = MDB_GETARG_JSONB(1);
	bool		res;

	res = (compareJsonbContainers(&jba->root, &jbb->root) < 0);

	MDB_FREE_IF_COPY(jba, 0);
	MDB_FREE_IF_COPY(jbb, 1);
	MDB_RETURN_BOOL(res);
}

Datum
jsonb_gt(MDB_FUNCTION_ARGS)
{
	Jsonb	   *jba = MDB_GETARG_JSONB(0);
	Jsonb	   *jbb = MDB_GETARG_JSONB(1);
	bool		res;

	res = (compareJsonbContainers(&jba->root, &jbb->root) > 0);

	MDB_FREE_IF_COPY(jba, 0);
	MDB_FREE_IF_COPY(jbb, 1);
	MDB_RETURN_BOOL(res);
}

Datum
jsonb_le(MDB_FUNCTION_ARGS)
{
	Jsonb	   *jba = MDB_GETARG_JSONB(0);
	Jsonb	   *jbb = MDB_GETARG_JSONB(1);
	bool		res;

	res = (compareJsonbContainers(&jba->root, &jbb->root) <= 0);

	MDB_FREE_IF_COPY(jba, 0);
	MDB_FREE_IF_COPY(jbb, 1);
	MDB_RETURN_BOOL(res);
}

Datum
jsonb_ge(MDB_FUNCTION_ARGS)
{
	Jsonb	   *jba = MDB_GETARG_JSONB(0);
	Jsonb	   *jbb = MDB_GETARG_JSONB(1);
	bool		res;

	res = (compareJsonbContainers(&jba->root, &jbb->root) >= 0);

	MDB_FREE_IF_COPY(jba, 0);
	MDB_FREE_IF_COPY(jbb, 1);
	MDB_RETURN_BOOL(res);
}

Datum
jsonb_eq(MDB_FUNCTION_ARGS)
{
	Jsonb	   *jba = MDB_GETARG_JSONB(0);
	Jsonb	   *jbb = MDB_GETARG_JSONB(1);
	bool		res;

	res = (compareJsonbContainers(&jba->root, &jbb->root) == 0);

	MDB_FREE_IF_COPY(jba, 0);
	MDB_FREE_IF_COPY(jbb, 1);
	MDB_RETURN_BOOL(res);
}

Datum
jsonb_cmp(MDB_FUNCTION_ARGS)
{
	Jsonb	   *jba = MDB_GETARG_JSONB(0);
	Jsonb	   *jbb = MDB_GETARG_JSONB(1);
	int			res;

	res = compareJsonbContainers(&jba->root, &jbb->root);

	MDB_FREE_IF_COPY(jba, 0);
	MDB_FREE_IF_COPY(jbb, 1);
	MDB_RETURN_INT32(res);
}

/*
 * Hash operator class jsonb hashing function
 */
Datum
jsonb_hash(MDB_FUNCTION_ARGS)
{
	Jsonb	   *jb = MDB_GETARG_JSONB(0);
	JsonbIterator *it;
	JsonbValue	v;
	JsonbIteratorToken r;
	uint32		hash = 0;

	if (JB_ROOT_COUNT(jb) == 0)
		MDB_RETURN_INT32(0);

	it = JsonbIteratorInit(&jb->root);

	while ((r = JsonbIteratorNext(&it, &v, false)) != WJB_DONE)
	{
		switch (r)
		{
				/* Rotation is left to JsonbHashScalarValue() */
			case WJB_BEGIN_ARRAY:
				hash ^= JB_FARRAY;
				break;
			case WJB_BEGIN_OBJECT:
				hash ^= JB_FOBJECT;
				break;
			case WJB_KEY:
			case WJB_VALUE:
			case WJB_ELEM:
				JsonbHashScalarValue(&v, &hash);
				break;
			case WJB_END_ARRAY:
			case WJB_END_OBJECT:
				break;
			default:
				elog(ERROR, "invalid JsonbIteratorNext rc: %d", (int) r);
		}
	}

	MDB_FREE_IF_COPY(jb, 0);
	MDB_RETURN_INT32(hash);
}
