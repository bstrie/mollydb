/* -------------------------------------------------------------------------
 *
 * contrib/semdb/database.c
 *
 * Routines corresponding to database objects
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
#include "catalog/mdb_database.h"
#include "catalog/indexing.h"
#include "commands/dbcommands.h"
#include "commands/seclabel.h"
#include "utils/builtins.h"
#include "utils/fmgroids.h"
#include "utils/tqual.h"
#include "semdb.h"

/*
 * semdb_database_post_create
 *
 * This routine assigns a default security label on a newly defined
 * database, and check permission needed for its creation.
 */
void
semdb_database_post_create(Oid databaseId, const char *dtemplate)
{
	Relation	rel;
	ScanKeyData skey;
	SysScanDesc sscan;
	HeapTuple	tuple;
	char	   *tcontext;
	char	   *ncontext;
	ObjectAddress object;
	Form_mdb_database datForm;
	StringInfoData audit_name;

	/*
	 * Oid of the source database is not saved in mdb_database catalog, so we
	 * collect its identifier using contextual information. If NULL, its
	 * default is "template1" according to createdb().
	 */
	if (!dtemplate)
		dtemplate = "template1";

	object.classId = DatabaseRelationId;
	object.objectId = get_database_oid(dtemplate, false);
	object.objectSubId = 0;

	tcontext = semdb_get_label(object.classId,
								 object.objectId,
								 object.objectSubId);

	/*
	 * check db_database:{getattr} permission
	 */
	initStringInfo(&audit_name);
	appendStringInfo(&audit_name, "%s", quote_identifier(dtemplate));
	semdb_avc_check_perms_label(tcontext,
								  SEMDB_CLASS_DB_DATABASE,
								  SEMDB_DB_DATABASE__GETATTR,
								  audit_name.data,
								  true);

	/*
	 * Compute a default security label of the newly created database based on
	 * a pair of security label of client and source database.
	 *
	 * XXX - uncoming version of libselinux supports to take object name to
	 * handle special treatment on default security label.
	 */
	rel = heap_open(DatabaseRelationId, AccessShareLock);

	ScanKeyInit(&skey,
				ObjectIdAttributeNumber,
				BTEqualStrategyNumber, F_OIDEQ,
				ObjectIdGetDatum(databaseId));

	sscan = systable_beginscan(rel, DatabaseOidIndexId, true,
							   SnapshotSelf, 1, &skey);
	tuple = systable_getnext(sscan);
	if (!HeapTupleIsValid(tuple))
		elog(ERROR, "catalog lookup failed for database %u", databaseId);

	datForm = (Form_mdb_database) GETSTRUCT(tuple);

	ncontext = semdb_compute_create(semdb_get_client_label(),
									  tcontext,
									  SEMDB_CLASS_DB_DATABASE,
									  NameStr(datForm->datname));

	/*
	 * check db_database:{create} permission
	 */
	resetStringInfo(&audit_name);
	appendStringInfo(&audit_name, "%s",
					 quote_identifier(NameStr(datForm->datname)));
	semdb_avc_check_perms_label(ncontext,
								  SEMDB_CLASS_DB_DATABASE,
								  SEMDB_DB_DATABASE__CREATE,
								  audit_name.data,
								  true);

	systable_endscan(sscan);
	heap_close(rel, AccessShareLock);

	/*
	 * Assign the default security label on the new database
	 */
	object.classId = DatabaseRelationId;
	object.objectId = databaseId;
	object.objectSubId = 0;

	SetSecurityLabel(&object, SEPGSQL_LABEL_TAG, ncontext);

	pfree(ncontext);
	pfree(tcontext);
}

/*
 * semdb_database_drop
 *
 * It checks privileges to drop the supplied database
 */
void
semdb_database_drop(Oid databaseId)
{
	ObjectAddress object;
	char	   *audit_name;

	/*
	 * check db_database:{drop} permission
	 */
	object.classId = DatabaseRelationId;
	object.objectId = databaseId;
	object.objectSubId = 0;
	audit_name = getObjectIdentity(&object);

	semdb_avc_check_perms(&object,
							SEMDB_CLASS_DB_DATABASE,
							SEMDB_DB_DATABASE__DROP,
							audit_name,
							true);
	pfree(audit_name);
}

/*
 * semdb_database_post_alter
 *
 * It checks privileges to alter the supplied database
 */
void
semdb_database_setattr(Oid databaseId)
{
	ObjectAddress object;
	char	   *audit_name;

	/*
	 * check db_database:{setattr} permission
	 */
	object.classId = DatabaseRelationId;
	object.objectId = databaseId;
	object.objectSubId = 0;
	audit_name = getObjectIdentity(&object);

	semdb_avc_check_perms(&object,
							SEMDB_CLASS_DB_DATABASE,
							SEMDB_DB_DATABASE__SETATTR,
							audit_name,
							true);
	pfree(audit_name);
}

/*
 * semdb_database_relabel
 *
 * It checks privileges to relabel the supplied database with the `seclabel'
 */
void
semdb_database_relabel(Oid databaseId, const char *seclabel)
{
	ObjectAddress object;
	char	   *audit_name;

	object.classId = DatabaseRelationId;
	object.objectId = databaseId;
	object.objectSubId = 0;
	audit_name = getObjectIdentity(&object);

	/*
	 * check db_database:{setattr relabelfrom} permission
	 */
	semdb_avc_check_perms(&object,
							SEMDB_CLASS_DB_DATABASE,
							SEMDB_DB_DATABASE__SETATTR |
							SEMDB_DB_DATABASE__RELABELFROM,
							audit_name,
							true);

	/*
	 * check db_database:{relabelto} permission
	 */
	semdb_avc_check_perms_label(seclabel,
								  SEMDB_CLASS_DB_DATABASE,
								  SEMDB_DB_DATABASE__RELABELTO,
								  audit_name,
								  true);
	pfree(audit_name);
}
