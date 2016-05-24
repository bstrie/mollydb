/*-------------------------------------------------------------------------
 *
 * mdb_db_role_setting.h
 *	definition of configuration settings
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/mdb_db_role_setting.h
 *
 * NOTES
 *		the genbki.pl script reads this file and generates .bki
 *		information from the DATA() statements.
 *
 *		XXX do NOT break up DATA() statements into multiple lines!
 *			the scripts are not as smart as you might think...
 *
 *-------------------------------------------------------------------------
 */
#ifndef MDB_DB_ROLE_SETTING_H
#define MDB_DB_ROLE_SETTING_H

#include "utils/guc.h"
#include "utils/relcache.h"
#include "utils/snapshot.h"

/* ----------------
 *		mdb_db_role_setting definition.  cpp turns this into
 *		typedef struct FormData_mdb_db_role_setting
 * ----------------
 */
#define DbRoleSettingRelationId 2964

CATALOG(mdb_db_role_setting,2964) BKI_SHARED_RELATION BKI_WITHOUT_OIDS
{
	Oid			setdatabase;	/* database */
	Oid			setrole;		/* role */

#ifdef CATALOG_VARLEN			/* variable-length fields start here */
	text		setconfig[1];	/* GUC settings to apply at login */
#endif
} FormData_mdb_db_role_setting;

typedef FormData_mdb_db_role_setting *Form_mdb_db_role_setting;

/* ----------------
 *		compiler constants for mdb_db_role_setting
 * ----------------
 */
#define Natts_mdb_db_role_setting				3
#define Anum_mdb_db_role_setting_setdatabase		1
#define Anum_mdb_db_role_setting_setrole			2
#define Anum_mdb_db_role_setting_setconfig		3

/* ----------------
 *		initial contents of mdb_db_role_setting are NOTHING
 * ----------------
 */

/*
 * prototypes for functions in mdb_db_role_setting.h
 */
extern void AlterSetting(Oid databaseid, Oid roleid, VariableSetStmt *setstmt);
extern void DropSetting(Oid databaseid, Oid roleid);
extern void ApplySetting(Snapshot snapshot, Oid databaseid, Oid roleid,
			 Relation relsetting, GucSource source);

#endif   /* MDB_DB_ROLE_SETTING_H */
