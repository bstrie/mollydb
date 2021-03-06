/* -------------------------------------------------------------------------
 *
 * contrib/semdb/schema.c
 *
 * Routines corresponding to schema objects
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
#include "catalog/mdb_database.h"
#include "catalog/mdb_namespace.h"
#include "commands/seclabel.h"
#include "lib/stringinfo.h"
#include "miscadmin.h"
#include "utils/builtins.h"
#include "utils/fmgroids.h"
#include "utils/lsyscache.h"
#include "utils/tqual.h"

#include "semdb.h"

/*
 * semdb_schema_post_create
 *
 * This routine assigns a default security label on a newly defined
 * schema.
 */
void
semdb_schema_post_create(Oid namespaceId)
{
	Relation	rel;
	ScanKeyData skey;
	SysScanDesc sscan;
	HeapTuple	tuple;
	char	   *tcontext;
	char	   *ncontext;
	const char *nsp_name;
	ObjectAddress object;
	Form_mdb_namespace nspForm;
	StringInfoData audit_name;

	/*
	 * Compute a default security label when we create a new schema object
	 * under the working database.
	 *
	 * XXX - uncoming version of libselinux supports to take object name to
	 * handle special treatment on default security label; such as special
	 * label on "mdb_temp" schema.
	 */
	rel = heap_open(NamespaceRelationId, AccessShareLock);

	ScanKeyInit(&skey,
				ObjectIdAttributeNumber,
				BTEqualStrategyNumber, F_OIDEQ,
				ObjectIdGetDatum(namespaceId));

	sscan = systable_beginscan(rel, NamespaceOidIndexId, true,
							   SnapshotSelf, 1, &skey);
	tuple = systable_getnext(sscan);
	if (!HeapTupleIsValid(tuple))
		elog(ERROR, "catalog lookup failed for namespace %u", namespaceId);

	nspForm = (Form_mdb_namespace) GETSTRUCT(tuple);
	nsp_name = NameStr(nspForm->nspname);
	if (strncmp(nsp_name, "mdb_temp_", 8) == 0)
		nsp_name = "mdb_temp";
	else if (strncmp(nsp_name, "mdb_toast_temp_", 14) == 0)
		nsp_name = "mdb_toast_temp";

	tcontext = semdb_get_label(DatabaseRelationId, MyDatabaseId, 0);
	ncontext = semdb_compute_create(semdb_get_client_label(),
									  tcontext,
									  SEMDB_CLASS_DB_SCHEMA,
									  nsp_name);

	/*
	 * check db_schema:{create}
	 */
	initStringInfo(&audit_name);
	appendStringInfo(&audit_name, "%s", quote_identifier(nsp_name));
	semdb_avc_check_perms_label(ncontext,
								  SEMDB_CLASS_DB_SCHEMA,
								  SEMDB_DB_SCHEMA__CREATE,
								  audit_name.data,
								  true);
	systable_endscan(sscan);
	heap_close(rel, AccessShareLock);

	/*
	 * Assign the default security label on a new procedure
	 */
	object.classId = NamespaceRelationId;
	object.objectId = namespaceId;
	object.objectSubId = 0;
	SetSecurityLabel(&object, SEPGSQL_LABEL_TAG, ncontext);

	pfree(ncontext);
	pfree(tcontext);
}

/*
 * semdb_schema_drop
 *
 * It checks privileges to drop the supplied schema object.
 */
void
semdb_schema_drop(Oid namespaceId)
{
	ObjectAddress object;
	char	   *audit_name;

	/*
	 * check db_schema:{drop} permission
	 */
	object.classId = NamespaceRelationId;
	object.objectId = namespaceId;
	object.objectSubId = 0;
	audit_name = getObjectIdentity(&object);

	semdb_avc_check_perms(&object,
							SEMDB_CLASS_DB_SCHEMA,
							SEMDB_DB_SCHEMA__DROP,
							audit_name,
							true);
	pfree(audit_name);
}

/*
 * semdb_schema_relabel
 *
 * It checks privileges to relabel the supplied schema
 * by the `seclabel'.
 */
void
semdb_schema_relabel(Oid namespaceId, const char *seclabel)
{
	ObjectAddress object;
	char	   *audit_name;

	object.classId = NamespaceRelationId;
	object.objectId = namespaceId;
	object.objectSubId = 0;
	audit_name = getObjectIdentity(&object);

	/*
	 * check db_schema:{setattr relabelfrom} permission
	 */
	semdb_avc_check_perms(&object,
							SEMDB_CLASS_DB_SCHEMA,
							SEMDB_DB_SCHEMA__SETATTR |
							SEMDB_DB_SCHEMA__RELABELFROM,
							audit_name,
							true);

	/*
	 * check db_schema:{relabelto} permission
	 */
	semdb_avc_check_perms_label(seclabel,
								  SEMDB_CLASS_DB_SCHEMA,
								  SEMDB_DB_SCHEMA__RELABELTO,
								  audit_name,
								  true);
	pfree(audit_name);
}

/*
 * semdb_schema_check_perms
 *
 * utility routine to check db_schema:{xxx} permissions
 */
static bool
check_schema_perms(Oid namespaceId, uint32 required, bool abort_on_violation)
{
	ObjectAddress object;
	char	   *audit_name;
	bool		result;

	object.classId = NamespaceRelationId;
	object.objectId = namespaceId;
	object.objectSubId = 0;
	audit_name = getObjectIdentity(&object);

	result = semdb_avc_check_perms(&object,
									 SEMDB_CLASS_DB_SCHEMA,
									 required,
									 audit_name,
									 abort_on_violation);
	pfree(audit_name);

	return result;
}

/* db_schema:{setattr} permission */
void
semdb_schema_setattr(Oid namespaceId)
{
	check_schema_perms(namespaceId, SEMDB_DB_SCHEMA__SETATTR, true);
}

/* db_schema:{search} permission */
bool
semdb_schema_search(Oid namespaceId, bool abort_on_violation)
{
	return check_schema_perms(namespaceId,
							  SEMDB_DB_SCHEMA__SEARCH,
							  abort_on_violation);
}

void
semdb_schema_add_name(Oid namespaceId)
{
	check_schema_perms(namespaceId, SEMDB_DB_SCHEMA__ADD_NAME, true);
}

void
semdb_schema_remove_name(Oid namespaceId)
{
	check_schema_perms(namespaceId, SEMDB_DB_SCHEMA__REMOVE_NAME, true);
}

void
semdb_schema_rename(Oid namespaceId)
{
	check_schema_perms(namespaceId,
					   SEMDB_DB_SCHEMA__ADD_NAME |
					   SEMDB_DB_SCHEMA__REMOVE_NAME,
					   true);
}
