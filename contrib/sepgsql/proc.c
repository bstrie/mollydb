/* -------------------------------------------------------------------------
 *
 * contrib/semdb/proc.c
 *
 * Routines corresponding to procedure objects
 *
 * Copyright (c) 2010-2016, MollyDB Global Development Group
 *
 * -------------------------------------------------------------------------
 */
#include "mollydb.h"

#include "access/genam.h"
#include "access/heapam.h"
#include "access/htup_details.h"
#include "access/sysattr.h"
#include "catalog/dependency.h"
#include "catalog/indexing.h"
#include "catalog/mdb_namespace.h"
#include "catalog/mdb_proc.h"
#include "catalog/mdb_type.h"
#include "commands/seclabel.h"
#include "lib/stringinfo.h"
#include "utils/builtins.h"
#include "utils/fmgroids.h"
#include "utils/lsyscache.h"
#include "utils/syscache.h"
#include "utils/tqual.h"

#include "semdb.h"

/*
 * semdb_proc_post_create
 *
 * This routine assigns a default security label on a newly defined
 * procedure.
 */
void
semdb_proc_post_create(Oid functionId)
{
	Relation	rel;
	ScanKeyData skey;
	SysScanDesc sscan;
	HeapTuple	tuple;
	char	   *nsp_name;
	char	   *scontext;
	char	   *tcontext;
	char	   *ncontext;
	uint32		required;
	int			i;
	StringInfoData audit_name;
	ObjectAddress object;
	Form_mdb_proc proForm;

	/*
	 * Fetch namespace of the new procedure. Because mdb_proc entry is not
	 * visible right now, we need to scan the catalog using SnapshotSelf.
	 */
	rel = heap_open(ProcedureRelationId, AccessShareLock);

	ScanKeyInit(&skey,
				ObjectIdAttributeNumber,
				BTEqualStrategyNumber, F_OIDEQ,
				ObjectIdGetDatum(functionId));

	sscan = systable_beginscan(rel, ProcedureOidIndexId, true,
							   SnapshotSelf, 1, &skey);

	tuple = systable_getnext(sscan);
	if (!HeapTupleIsValid(tuple))
		elog(ERROR, "catalog lookup failed for proc %u", functionId);

	proForm = (Form_mdb_proc) GETSTRUCT(tuple);

	/*
	 * check db_schema:{add_name} permission of the namespace
	 */
	object.classId = NamespaceRelationId;
	object.objectId = proForm->pronamespace;
	object.objectSubId = 0;
	semdb_avc_check_perms(&object,
							SEMDB_CLASS_DB_SCHEMA,
							SEMDB_DB_SCHEMA__ADD_NAME,
							getObjectIdentity(&object),
							true);

	/*
	 * XXX - db_language:{implement} also should be checked here
	 */


	/*
	 * Compute a default security label when we create a new procedure object
	 * under the specified namespace.
	 */
	scontext = semdb_get_client_label();
	tcontext = semdb_get_label(NamespaceRelationId,
								 proForm->pronamespace, 0);
	ncontext = semdb_compute_create(scontext, tcontext,
									  SEMDB_CLASS_DB_PROCEDURE,
									  NameStr(proForm->proname));

	/*
	 * check db_procedure:{create (install)} permission
	 */
	initStringInfo(&audit_name);
	nsp_name = get_namespace_name(proForm->pronamespace);
	appendStringInfo(&audit_name, "%s(",
			quote_qualified_identifier(nsp_name, NameStr(proForm->proname)));
	for (i = 0; i < proForm->pronargs; i++)
	{
		if (i > 0)
			appendStringInfoChar(&audit_name, ',');

		object.classId = TypeRelationId;
		object.objectId = proForm->proargtypes.values[i];
		object.objectSubId = 0;
		appendStringInfoString(&audit_name, getObjectIdentity(&object));
	}
	appendStringInfoChar(&audit_name, ')');

	required = SEMDB_DB_PROCEDURE__CREATE;
	if (proForm->proleakproof)
		required |= SEMDB_DB_PROCEDURE__INSTALL;

	semdb_avc_check_perms_label(ncontext,
								  SEMDB_CLASS_DB_PROCEDURE,
								  required,
								  audit_name.data,
								  true);

	/*
	 * Assign the default security label on a new procedure
	 */
	object.classId = ProcedureRelationId;
	object.objectId = functionId;
	object.objectSubId = 0;
	SetSecurityLabel(&object, SEPGSQL_LABEL_TAG, ncontext);

	/*
	 * Cleanup
	 */
	systable_endscan(sscan);
	heap_close(rel, AccessShareLock);

	pfree(audit_name.data);
	pfree(tcontext);
	pfree(ncontext);
}

/*
 * semdb_proc_drop
 *
 * It checks privileges to drop the supplied function.
 */
void
semdb_proc_drop(Oid functionId)
{
	ObjectAddress object;
	char	   *audit_name;

	/*
	 * check db_schema:{remove_name} permission
	 */
	object.classId = NamespaceRelationId;
	object.objectId = get_func_namespace(functionId);
	object.objectSubId = 0;
	audit_name = getObjectIdentity(&object);

	semdb_avc_check_perms(&object,
							SEMDB_CLASS_DB_SCHEMA,
							SEMDB_DB_SCHEMA__REMOVE_NAME,
							audit_name,
							true);
	pfree(audit_name);

	/*
	 * check db_procedure:{drop} permission
	 */
	object.classId = ProcedureRelationId;
	object.objectId = functionId;
	object.objectSubId = 0;
	audit_name = getObjectIdentity(&object);

	semdb_avc_check_perms(&object,
							SEMDB_CLASS_DB_PROCEDURE,
							SEMDB_DB_PROCEDURE__DROP,
							audit_name,
							true);
	pfree(audit_name);
}

/*
 * semdb_proc_relabel
 *
 * It checks privileges to relabel the supplied function
 * by the `seclabel'.
 */
void
semdb_proc_relabel(Oid functionId, const char *seclabel)
{
	ObjectAddress object;
	char	   *audit_name;

	object.classId = ProcedureRelationId;
	object.objectId = functionId;
	object.objectSubId = 0;
	audit_name = getObjectIdentity(&object);

	/*
	 * check db_procedure:{setattr relabelfrom} permission
	 */
	semdb_avc_check_perms(&object,
							SEMDB_CLASS_DB_PROCEDURE,
							SEMDB_DB_PROCEDURE__SETATTR |
							SEMDB_DB_PROCEDURE__RELABELFROM,
							audit_name,
							true);

	/*
	 * check db_procedure:{relabelto} permission
	 */
	semdb_avc_check_perms_label(seclabel,
								  SEMDB_CLASS_DB_PROCEDURE,
								  SEMDB_DB_PROCEDURE__RELABELTO,
								  audit_name,
								  true);
	pfree(audit_name);
}

/*
 * semdb_proc_setattr
 *
 * It checks privileges to alter the supplied function.
 */
void
semdb_proc_setattr(Oid functionId)
{
	Relation	rel;
	ScanKeyData skey;
	SysScanDesc sscan;
	HeapTuple	oldtup;
	HeapTuple	newtup;
	Form_mdb_proc oldform;
	Form_mdb_proc newform;
	uint32		required;
	ObjectAddress object;
	char	   *audit_name;

	/*
	 * Fetch newer catalog
	 */
	rel = heap_open(ProcedureRelationId, AccessShareLock);

	ScanKeyInit(&skey,
				ObjectIdAttributeNumber,
				BTEqualStrategyNumber, F_OIDEQ,
				ObjectIdGetDatum(functionId));

	sscan = systable_beginscan(rel, ProcedureOidIndexId, true,
							   SnapshotSelf, 1, &skey);
	newtup = systable_getnext(sscan);
	if (!HeapTupleIsValid(newtup))
		elog(ERROR, "catalog lookup failed for function %u", functionId);
	newform = (Form_mdb_proc) GETSTRUCT(newtup);

	/*
	 * Fetch older catalog
	 */
	oldtup = SearchSysCache1(PROCOID, ObjectIdGetDatum(functionId));
	if (!HeapTupleIsValid(oldtup))
		elog(ERROR, "cache lookup failed for function %u", functionId);
	oldform = (Form_mdb_proc) GETSTRUCT(oldtup);

	/*
	 * Does this ALTER command takes operation to namespace?
	 */
	if (newform->pronamespace != oldform->pronamespace)
	{
		semdb_schema_remove_name(oldform->pronamespace);
		semdb_schema_add_name(oldform->pronamespace);
	}
	if (strcmp(NameStr(newform->proname), NameStr(oldform->proname)) != 0)
		semdb_schema_rename(oldform->pronamespace);

	/*
	 * check db_procedure:{setattr (install)} permission
	 */
	required = SEMDB_DB_PROCEDURE__SETATTR;
	if (!oldform->proleakproof && newform->proleakproof)
		required |= SEMDB_DB_PROCEDURE__INSTALL;

	object.classId = ProcedureRelationId;
	object.objectId = functionId;
	object.objectSubId = 0;
	audit_name = getObjectIdentity(&object);

	semdb_avc_check_perms(&object,
							SEMDB_CLASS_DB_PROCEDURE,
							required,
							audit_name,
							true);
	/* cleanups */
	pfree(audit_name);

	ReleaseSysCache(oldtup);
	systable_endscan(sscan);
	heap_close(rel, AccessShareLock);
}

/*
 * semdb_proc_execute
 *
 * It checks privileges to execute the supplied function
 */
void
semdb_proc_execute(Oid functionId)
{
	ObjectAddress object;
	char	   *audit_name;

	/*
	 * check db_procedure:{execute} permission
	 */
	object.classId = ProcedureRelationId;
	object.objectId = functionId;
	object.objectSubId = 0;
	audit_name = getObjectIdentity(&object);
	semdb_avc_check_perms(&object,
							SEMDB_CLASS_DB_PROCEDURE,
							SEMDB_DB_PROCEDURE__EXECUTE,
							audit_name,
							true);
	pfree(audit_name);
}
