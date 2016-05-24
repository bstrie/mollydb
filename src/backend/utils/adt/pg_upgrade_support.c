/*
 *	mdb_upgrade_support.c
 *
 *	server-side functions to set backend global variables
 *	to control oid and relfilenode assignment, and do other special
 *	hacks needed for mdb_upgrade.
 *
 *	Copyright (c) 2010-2016, MollyDB Global Development Group
 *	src/backend/utils/adt/mdb_upgrade_support.c
 */

#include "mollydb.h"

#include "catalog/binary_upgrade.h"
#include "catalog/namespace.h"
#include "catalog/mdb_type.h"
#include "commands/extension.h"
#include "miscadmin.h"
#include "utils/array.h"
#include "utils/builtins.h"


Datum		binary_upgrade_set_next_mdb_type_oid(PG_FUNCTION_ARGS);
Datum		binary_upgrade_set_next_array_mdb_type_oid(PG_FUNCTION_ARGS);
Datum		binary_upgrade_set_next_toast_mdb_type_oid(PG_FUNCTION_ARGS);
Datum		binary_upgrade_set_next_heap_mdb_class_oid(PG_FUNCTION_ARGS);
Datum		binary_upgrade_set_next_index_mdb_class_oid(PG_FUNCTION_ARGS);
Datum		binary_upgrade_set_next_toast_mdb_class_oid(PG_FUNCTION_ARGS);
Datum		binary_upgrade_set_next_mdb_enum_oid(PG_FUNCTION_ARGS);
Datum		binary_upgrade_set_next_mdb_authid_oid(PG_FUNCTION_ARGS);
Datum		binary_upgrade_create_empty_extension(PG_FUNCTION_ARGS);
Datum		binary_upgrade_set_record_init_privs(PG_FUNCTION_ARGS);


#define CHECK_IS_BINARY_UPGRADE									\
do {															\
	if (!IsBinaryUpgrade)										\
		ereport(ERROR,											\
				(errcode(ERRCODE_CANT_CHANGE_RUNTIME_PARAM),	\
				 (errmsg("function can only be called when server is in binary upgrade mode")))); \
} while (0)

Datum
binary_upgrade_set_next_mdb_type_oid(PG_FUNCTION_ARGS)
{
	Oid			typoid = PG_GETARG_OID(0);

	CHECK_IS_BINARY_UPGRADE;
	binary_upgrade_next_mdb_type_oid = typoid;

	PG_RETURN_VOID();
}

Datum
binary_upgrade_set_next_array_mdb_type_oid(PG_FUNCTION_ARGS)
{
	Oid			typoid = PG_GETARG_OID(0);

	CHECK_IS_BINARY_UPGRADE;
	binary_upgrade_next_array_mdb_type_oid = typoid;

	PG_RETURN_VOID();
}

Datum
binary_upgrade_set_next_toast_mdb_type_oid(PG_FUNCTION_ARGS)
{
	Oid			typoid = PG_GETARG_OID(0);

	CHECK_IS_BINARY_UPGRADE;
	binary_upgrade_next_toast_mdb_type_oid = typoid;

	PG_RETURN_VOID();
}

Datum
binary_upgrade_set_next_heap_mdb_class_oid(PG_FUNCTION_ARGS)
{
	Oid			reloid = PG_GETARG_OID(0);

	CHECK_IS_BINARY_UPGRADE;
	binary_upgrade_next_heap_mdb_class_oid = reloid;

	PG_RETURN_VOID();
}

Datum
binary_upgrade_set_next_index_mdb_class_oid(PG_FUNCTION_ARGS)
{
	Oid			reloid = PG_GETARG_OID(0);

	CHECK_IS_BINARY_UPGRADE;
	binary_upgrade_next_index_mdb_class_oid = reloid;

	PG_RETURN_VOID();
}

Datum
binary_upgrade_set_next_toast_mdb_class_oid(PG_FUNCTION_ARGS)
{
	Oid			reloid = PG_GETARG_OID(0);

	CHECK_IS_BINARY_UPGRADE;
	binary_upgrade_next_toast_mdb_class_oid = reloid;

	PG_RETURN_VOID();
}

Datum
binary_upgrade_set_next_mdb_enum_oid(PG_FUNCTION_ARGS)
{
	Oid			enumoid = PG_GETARG_OID(0);

	CHECK_IS_BINARY_UPGRADE;
	binary_upgrade_next_mdb_enum_oid = enumoid;

	PG_RETURN_VOID();
}

Datum
binary_upgrade_set_next_mdb_authid_oid(PG_FUNCTION_ARGS)
{
	Oid			authoid = PG_GETARG_OID(0);

	CHECK_IS_BINARY_UPGRADE;
	binary_upgrade_next_mdb_authid_oid = authoid;
	PG_RETURN_VOID();
}

Datum
binary_upgrade_create_empty_extension(PG_FUNCTION_ARGS)
{
	text	   *extName;
	text	   *schemaName;
	bool		relocatable;
	text	   *extVersion;
	Datum		extConfig;
	Datum		extCondition;
	List	   *requiredExtensions;

	CHECK_IS_BINARY_UPGRADE;

	/* We must check these things before dereferencing the arguments */
	if (PG_ARGISNULL(0) ||
		PG_ARGISNULL(1) ||
		PG_ARGISNULL(2) ||
		PG_ARGISNULL(3))
		elog(ERROR, "null argument to binary_upgrade_create_empty_extension is not allowed");

	extName = PG_GETARG_TEXT_PP(0);
	schemaName = PG_GETARG_TEXT_PP(1);
	relocatable = PG_GETARG_BOOL(2);
	extVersion = PG_GETARG_TEXT_PP(3);

	if (PG_ARGISNULL(4))
		extConfig = PointerGetDatum(NULL);
	else
		extConfig = PG_GETARG_DATUM(4);

	if (PG_ARGISNULL(5))
		extCondition = PointerGetDatum(NULL);
	else
		extCondition = PG_GETARG_DATUM(5);

	requiredExtensions = NIL;
	if (!PG_ARGISNULL(6))
	{
		ArrayType  *textArray = PG_GETARG_ARRAYTYPE_P(6);
		Datum	   *textDatums;
		int			ndatums;
		int			i;

		deconstruct_array(textArray,
						  TEXTOID, -1, false, 'i',
						  &textDatums, NULL, &ndatums);
		for (i = 0; i < ndatums; i++)
		{
			text	   *txtname = DatumGetTextPP(textDatums[i]);
			char	   *extName = text_to_cstring(txtname);
			Oid			extOid = get_extension_oid(extName, false);

			requiredExtensions = lappend_oid(requiredExtensions, extOid);
		}
	}

	InsertExtensionTuple(text_to_cstring(extName),
						 GetUserId(),
					   get_namespace_oid(text_to_cstring(schemaName), false),
						 relocatable,
						 text_to_cstring(extVersion),
						 extConfig,
						 extCondition,
						 requiredExtensions);

	PG_RETURN_VOID();
}

Datum
binary_upgrade_set_record_init_privs(PG_FUNCTION_ARGS)
{
	bool		record_init_privs = PG_GETARG_BOOL(0);

	CHECK_IS_BINARY_UPGRADE;
	binary_upgrade_record_init_privs = record_init_privs;

	PG_RETURN_VOID();
}
