/* -------------------------------------------------------------------------
 *
 * contrib/semdb/semdb.h
 *
 * Definitions corresponding to SE-MollyDB
 *
 * Copyright (c) 2010-2016, MollyDB Global Development Group
 *
 * -------------------------------------------------------------------------
 */
#ifndef SEPGSQL_H
#define SEPGSQL_H

#include "catalog/objectaddress.h"
#include "fmgr.h"

#include <selinux/selinux.h>
#include <selinux/avc.h>

/*
 * SE-MollyDB Label Tag
 */
#define SEPGSQL_LABEL_TAG			"selinux"

/*
 * SE-MollyDB performing mode
 */
#define SEPGSQL_MODE_DEFAULT		1
#define SEPGSQL_MODE_PERMISSIVE		2
#define SEPGSQL_MODE_INTERNAL		3
#define SEPGSQL_MODE_DISABLED		4

/*
 * Internally used code of object classes
 */
#define SEMDB_CLASS_PROCESS			0
#define SEMDB_CLASS_FILE				1
#define SEMDB_CLASS_DIR				2
#define SEMDB_CLASS_LNK_FILE			3
#define SEMDB_CLASS_CHR_FILE			4
#define SEMDB_CLASS_BLK_FILE			5
#define SEMDB_CLASS_SOCK_FILE		6
#define SEMDB_CLASS_FIFO_FILE		7
#define SEMDB_CLASS_DB_DATABASE		8
#define SEMDB_CLASS_DB_SCHEMA		9
#define SEMDB_CLASS_DB_TABLE			10
#define SEMDB_CLASS_DB_SEQUENCE		11
#define SEMDB_CLASS_DB_PROCEDURE		12
#define SEMDB_CLASS_DB_COLUMN		13
#define SEMDB_CLASS_DB_TUPLE			14
#define SEMDB_CLASS_DB_BLOB			15
#define SEMDB_CLASS_DB_LANGUAGE		16
#define SEMDB_CLASS_DB_VIEW			17
#define SEMDB_CLASS_MAX				18

/*
 * Internally used code of access vectors
 */
#define SEMDB_PROCESS__TRANSITION			(1<<0)
#define SEMDB_PROCESS__DYNTRANSITION			(1<<1)
#define SEMDB_PROCESS__SETCURRENT			(1<<2)

#define SEMDB_FILE__READ						(1<<0)
#define SEMDB_FILE__WRITE					(1<<1)
#define SEMDB_FILE__CREATE					(1<<2)
#define SEMDB_FILE__GETATTR					(1<<3)
#define SEMDB_FILE__UNLINK					(1<<4)
#define SEMDB_FILE__RENAME					(1<<5)
#define SEMDB_FILE__APPEND					(1<<6)

#define SEMDB_DIR__READ						(SEMDB_FILE__READ)
#define SEMDB_DIR__WRITE						(SEMDB_FILE__WRITE)
#define SEMDB_DIR__CREATE					(SEMDB_FILE__CREATE)
#define SEMDB_DIR__GETATTR					(SEMDB_FILE__GETATTR)
#define SEMDB_DIR__UNLINK					(SEMDB_FILE__UNLINK)
#define SEMDB_DIR__RENAME					(SEMDB_FILE__RENAME)
#define SEMDB_DIR__SEARCH					(1<<6)
#define SEMDB_DIR__ADD_NAME					(1<<7)
#define SEMDB_DIR__REMOVE_NAME				(1<<8)
#define SEMDB_DIR__RMDIR						(1<<9)
#define SEMDB_DIR__REPARENT					(1<<10)

#define SEMDB_LNK_FILE__READ					(SEMDB_FILE__READ)
#define SEMDB_LNK_FILE__WRITE				(SEMDB_FILE__WRITE)
#define SEMDB_LNK_FILE__CREATE				(SEMDB_FILE__CREATE)
#define SEMDB_LNK_FILE__GETATTR				(SEMDB_FILE__GETATTR)
#define SEMDB_LNK_FILE__UNLINK				(SEMDB_FILE__UNLINK)
#define SEMDB_LNK_FILE__RENAME				(SEMDB_FILE__RENAME)

#define SEMDB_CHR_FILE__READ					(SEMDB_FILE__READ)
#define SEMDB_CHR_FILE__WRITE				(SEMDB_FILE__WRITE)
#define SEMDB_CHR_FILE__CREATE				(SEMDB_FILE__CREATE)
#define SEMDB_CHR_FILE__GETATTR				(SEMDB_FILE__GETATTR)
#define SEMDB_CHR_FILE__UNLINK				(SEMDB_FILE__UNLINK)
#define SEMDB_CHR_FILE__RENAME				(SEMDB_FILE__RENAME)

#define SEMDB_BLK_FILE__READ					(SEMDB_FILE__READ)
#define SEMDB_BLK_FILE__WRITE				(SEMDB_FILE__WRITE)
#define SEMDB_BLK_FILE__CREATE				(SEMDB_FILE__CREATE)
#define SEMDB_BLK_FILE__GETATTR				(SEMDB_FILE__GETATTR)
#define SEMDB_BLK_FILE__UNLINK				(SEMDB_FILE__UNLINK)
#define SEMDB_BLK_FILE__RENAME				(SEMDB_FILE__RENAME)

#define SEMDB_SOCK_FILE__READ				(SEMDB_FILE__READ)
#define SEMDB_SOCK_FILE__WRITE				(SEMDB_FILE__WRITE)
#define SEMDB_SOCK_FILE__CREATE				(SEMDB_FILE__CREATE)
#define SEMDB_SOCK_FILE__GETATTR				(SEMDB_FILE__GETATTR)
#define SEMDB_SOCK_FILE__UNLINK				(SEMDB_FILE__UNLINK)
#define SEMDB_SOCK_FILE__RENAME				(SEMDB_FILE__RENAME)

#define SEMDB_FIFO_FILE__READ				(SEMDB_FILE__READ)
#define SEMDB_FIFO_FILE__WRITE				(SEMDB_FILE__WRITE)
#define SEMDB_FIFO_FILE__CREATE				(SEMDB_FILE__CREATE)
#define SEMDB_FIFO_FILE__GETATTR				(SEMDB_FILE__GETATTR)
#define SEMDB_FIFO_FILE__UNLINK				(SEMDB_FILE__UNLINK)
#define SEMDB_FIFO_FILE__RENAME				(SEMDB_FILE__RENAME)

#define SEMDB_DB_DATABASE__CREATE			(1<<0)
#define SEMDB_DB_DATABASE__DROP				(1<<1)
#define SEMDB_DB_DATABASE__GETATTR			(1<<2)
#define SEMDB_DB_DATABASE__SETATTR			(1<<3)
#define SEMDB_DB_DATABASE__RELABELFROM		(1<<4)
#define SEMDB_DB_DATABASE__RELABELTO			(1<<5)
#define SEMDB_DB_DATABASE__ACCESS			(1<<6)
#define SEMDB_DB_DATABASE__LOAD_MODULE		(1<<7)

#define SEMDB_DB_SCHEMA__CREATE				(SEMDB_DB_DATABASE__CREATE)
#define SEMDB_DB_SCHEMA__DROP				(SEMDB_DB_DATABASE__DROP)
#define SEMDB_DB_SCHEMA__GETATTR				(SEMDB_DB_DATABASE__GETATTR)
#define SEMDB_DB_SCHEMA__SETATTR				(SEMDB_DB_DATABASE__SETATTR)
#define SEMDB_DB_SCHEMA__RELABELFROM			(SEMDB_DB_DATABASE__RELABELFROM)
#define SEMDB_DB_SCHEMA__RELABELTO			(SEMDB_DB_DATABASE__RELABELTO)
#define SEMDB_DB_SCHEMA__SEARCH				(1<<6)
#define SEMDB_DB_SCHEMA__ADD_NAME			(1<<7)
#define SEMDB_DB_SCHEMA__REMOVE_NAME			(1<<8)

#define SEMDB_DB_TABLE__CREATE				(SEMDB_DB_DATABASE__CREATE)
#define SEMDB_DB_TABLE__DROP					(SEMDB_DB_DATABASE__DROP)
#define SEMDB_DB_TABLE__GETATTR				(SEMDB_DB_DATABASE__GETATTR)
#define SEMDB_DB_TABLE__SETATTR				(SEMDB_DB_DATABASE__SETATTR)
#define SEMDB_DB_TABLE__RELABELFROM			(SEMDB_DB_DATABASE__RELABELFROM)
#define SEMDB_DB_TABLE__RELABELTO			(SEMDB_DB_DATABASE__RELABELTO)
#define SEMDB_DB_TABLE__SELECT				(1<<6)
#define SEMDB_DB_TABLE__UPDATE				(1<<7)
#define SEMDB_DB_TABLE__INSERT				(1<<8)
#define SEMDB_DB_TABLE__DELETE				(1<<9)
#define SEMDB_DB_TABLE__LOCK					(1<<10)

#define SEMDB_DB_SEQUENCE__CREATE			(SEMDB_DB_DATABASE__CREATE)
#define SEMDB_DB_SEQUENCE__DROP				(SEMDB_DB_DATABASE__DROP)
#define SEMDB_DB_SEQUENCE__GETATTR			(SEMDB_DB_DATABASE__GETATTR)
#define SEMDB_DB_SEQUENCE__SETATTR			(SEMDB_DB_DATABASE__SETATTR)
#define SEMDB_DB_SEQUENCE__RELABELFROM		(SEMDB_DB_DATABASE__RELABELFROM)
#define SEMDB_DB_SEQUENCE__RELABELTO			(SEMDB_DB_DATABASE__RELABELTO)
#define SEMDB_DB_SEQUENCE__GET_VALUE			(1<<6)
#define SEMDB_DB_SEQUENCE__NEXT_VALUE		(1<<7)
#define SEMDB_DB_SEQUENCE__SET_VALUE			(1<<8)

#define SEMDB_DB_PROCEDURE__CREATE			(SEMDB_DB_DATABASE__CREATE)
#define SEMDB_DB_PROCEDURE__DROP				(SEMDB_DB_DATABASE__DROP)
#define SEMDB_DB_PROCEDURE__GETATTR			(SEMDB_DB_DATABASE__GETATTR)
#define SEMDB_DB_PROCEDURE__SETATTR			(SEMDB_DB_DATABASE__SETATTR)
#define SEMDB_DB_PROCEDURE__RELABELFROM		(SEMDB_DB_DATABASE__RELABELFROM)
#define SEMDB_DB_PROCEDURE__RELABELTO		(SEMDB_DB_DATABASE__RELABELTO)
#define SEMDB_DB_PROCEDURE__EXECUTE			(1<<6)
#define SEMDB_DB_PROCEDURE__ENTRYPOINT		(1<<7)
#define SEMDB_DB_PROCEDURE__INSTALL			(1<<8)

#define SEMDB_DB_COLUMN__CREATE				(SEMDB_DB_DATABASE__CREATE)
#define SEMDB_DB_COLUMN__DROP				(SEMDB_DB_DATABASE__DROP)
#define SEMDB_DB_COLUMN__GETATTR				(SEMDB_DB_DATABASE__GETATTR)
#define SEMDB_DB_COLUMN__SETATTR				(SEMDB_DB_DATABASE__SETATTR)
#define SEMDB_DB_COLUMN__RELABELFROM			(SEMDB_DB_DATABASE__RELABELFROM)
#define SEMDB_DB_COLUMN__RELABELTO			(SEMDB_DB_DATABASE__RELABELTO)
#define SEMDB_DB_COLUMN__SELECT				(1<<6)
#define SEMDB_DB_COLUMN__UPDATE				(1<<7)
#define SEMDB_DB_COLUMN__INSERT				(1<<8)

#define SEMDB_DB_TUPLE__RELABELFROM			(SEMDB_DB_DATABASE__RELABELFROM)
#define SEMDB_DB_TUPLE__RELABELTO			(SEMDB_DB_DATABASE__RELABELTO)
#define SEMDB_DB_TUPLE__SELECT				(SEMDB_DB_DATABASE__GETATTR)
#define SEMDB_DB_TUPLE__UPDATE				(SEMDB_DB_DATABASE__SETATTR)
#define SEMDB_DB_TUPLE__INSERT				(SEMDB_DB_DATABASE__CREATE)
#define SEMDB_DB_TUPLE__DELETE				(SEMDB_DB_DATABASE__DROP)

#define SEMDB_DB_BLOB__CREATE				(SEMDB_DB_DATABASE__CREATE)
#define SEMDB_DB_BLOB__DROP					(SEMDB_DB_DATABASE__DROP)
#define SEMDB_DB_BLOB__GETATTR				(SEMDB_DB_DATABASE__GETATTR)
#define SEMDB_DB_BLOB__SETATTR				(SEMDB_DB_DATABASE__SETATTR)
#define SEMDB_DB_BLOB__RELABELFROM			(SEMDB_DB_DATABASE__RELABELFROM)
#define SEMDB_DB_BLOB__RELABELTO				(SEMDB_DB_DATABASE__RELABELTO)
#define SEMDB_DB_BLOB__READ					(1<<6)
#define SEMDB_DB_BLOB__WRITE					(1<<7)
#define SEMDB_DB_BLOB__IMPORT				(1<<8)
#define SEMDB_DB_BLOB__EXPORT				(1<<9)

#define SEMDB_DB_LANGUAGE__CREATE			(SEMDB_DB_DATABASE__CREATE)
#define SEMDB_DB_LANGUAGE__DROP				(SEMDB_DB_DATABASE__DROP)
#define SEMDB_DB_LANGUAGE__GETATTR			(SEMDB_DB_DATABASE__GETATTR)
#define SEMDB_DB_LANGUAGE__SETATTR			(SEMDB_DB_DATABASE__SETATTR)
#define SEMDB_DB_LANGUAGE__RELABELFROM		(SEMDB_DB_DATABASE__RELABELFROM)
#define SEMDB_DB_LANGUAGE__RELABELTO			(SEMDB_DB_DATABASE__RELABELTO)
#define SEMDB_DB_LANGUAGE__IMPLEMENT			(1<<6)
#define SEMDB_DB_LANGUAGE__EXECUTE			(1<<7)

#define SEMDB_DB_VIEW__CREATE				(SEMDB_DB_DATABASE__CREATE)
#define SEMDB_DB_VIEW__DROP					(SEMDB_DB_DATABASE__DROP)
#define SEMDB_DB_VIEW__GETATTR				(SEMDB_DB_DATABASE__GETATTR)
#define SEMDB_DB_VIEW__SETATTR				(SEMDB_DB_DATABASE__SETATTR)
#define SEMDB_DB_VIEW__RELABELFROM			(SEMDB_DB_DATABASE__RELABELFROM)
#define SEMDB_DB_VIEW__RELABELTO				(SEMDB_DB_DATABASE__RELABELTO)
#define SEMDB_DB_VIEW__EXPAND				(1<<6)

/*
 * hooks.c
 */
extern bool semdb_get_permissive(void);
extern bool semdb_get_debug_audit(void);

/*
 * selinux.c
 */
extern bool semdb_is_enabled(void);
extern int	semdb_get_mode(void);
extern int	semdb_set_mode(int new_mode);
extern bool semdb_getenforce(void);

extern void semdb_audit_log(bool denied,
				  const char *scontext,
				  const char *tcontext,
				  uint16 tclass,
				  uint32 audited,
				  const char *audit_name);

extern void semdb_compute_avd(const char *scontext,
					const char *tcontext,
					uint16 tclass,
					struct av_decision * avd);

extern char *semdb_compute_create(const char *scontext,
					   const char *tcontext,
					   uint16 tclass,
					   const char *objname);

extern bool semdb_check_perms(const char *scontext,
					const char *tcontext,
					uint16 tclass,
					uint32 required,
					const char *audit_name,
					bool abort_on_violation);

/*
 * uavc.c
 */
#define SEPGSQL_AVC_NOAUDIT			((void *)(-1))
extern bool semdb_avc_check_perms_label(const char *tcontext,
							  uint16 tclass,
							  uint32 required,
							  const char *audit_name,
							  bool abort_on_violation);
extern bool semdb_avc_check_perms(const ObjectAddress *tobject,
						uint16 tclass,
						uint32 required,
						const char *audit_name,
						bool abort_on_violation);
extern char *semdb_avc_trusted_proc(Oid functionId);
extern void semdb_avc_init(void);

/*
 * label.c
 */
extern char *semdb_get_client_label(void);
extern void semdb_init_client_label(void);
extern char *semdb_get_label(Oid relOid, Oid objOid, int32 subId);

extern void semdb_object_relabel(const ObjectAddress *object,
					   const char *seclabel);

extern Datum semdb_getcon(MDB_FUNCTION_ARGS);
extern Datum semdb_setcon(MDB_FUNCTION_ARGS);
extern Datum semdb_mcstrans_in(MDB_FUNCTION_ARGS);
extern Datum semdb_mcstrans_out(MDB_FUNCTION_ARGS);
extern Datum semdb_restorecon(MDB_FUNCTION_ARGS);

/*
 * dml.c
 */
extern bool semdb_dml_privileges(List *rangeTabls, bool abort_on_violation);

/*
 * database.c
 */
extern void semdb_database_post_create(Oid databaseId,
							 const char *dtemplate);
extern void semdb_database_drop(Oid databaseId);
extern void semdb_database_relabel(Oid databaseId, const char *seclabel);
extern void semdb_database_setattr(Oid databaseId);

/*
 * schema.c
 */
extern void semdb_schema_post_create(Oid namespaceId);
extern void semdb_schema_drop(Oid namespaceId);
extern void semdb_schema_relabel(Oid namespaceId, const char *seclabel);
extern void semdb_schema_setattr(Oid namespaceId);
extern bool semdb_schema_search(Oid namespaceId, bool abort_on_violation);
extern void semdb_schema_add_name(Oid namespaceId);
extern void semdb_schema_remove_name(Oid namespaceId);
extern void semdb_schema_rename(Oid namespaceId);

/*
 * relation.c
 */
extern void semdb_attribute_post_create(Oid relOid, AttrNumber attnum);
extern void semdb_attribute_drop(Oid relOid, AttrNumber attnum);
extern void semdb_attribute_relabel(Oid relOid, AttrNumber attnum,
						  const char *seclabel);
extern void semdb_attribute_setattr(Oid relOid, AttrNumber attnum);
extern void semdb_relation_post_create(Oid relOid);
extern void semdb_relation_drop(Oid relOid);
extern void semdb_relation_relabel(Oid relOid, const char *seclabel);
extern void semdb_relation_setattr(Oid relOid);

/*
 * proc.c
 */
extern void semdb_proc_post_create(Oid functionId);
extern void semdb_proc_drop(Oid functionId);
extern void semdb_proc_relabel(Oid functionId, const char *seclabel);
extern void semdb_proc_setattr(Oid functionId);
extern void semdb_proc_execute(Oid functionId);

#endif   /* SEPGSQL_H */
