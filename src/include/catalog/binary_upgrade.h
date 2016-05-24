/*-------------------------------------------------------------------------
 *
 * binary_upgrade.h
 *	  variables used for binary upgrades
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/binary_upgrade.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef BINARY_UPGRADE_H
#define BINARY_UPGRADE_H

extern PGDLLIMPORT Oid binary_upgrade_next_mdb_type_oid;
extern PGDLLIMPORT Oid binary_upgrade_next_array_mdb_type_oid;
extern PGDLLIMPORT Oid binary_upgrade_next_toast_mdb_type_oid;

extern PGDLLIMPORT Oid binary_upgrade_next_heap_mdb_class_oid;
extern PGDLLIMPORT Oid binary_upgrade_next_index_mdb_class_oid;
extern PGDLLIMPORT Oid binary_upgrade_next_toast_mdb_class_oid;

extern PGDLLIMPORT Oid binary_upgrade_next_mdb_enum_oid;
extern PGDLLIMPORT Oid binary_upgrade_next_mdb_authid_oid;

extern PGDLLIMPORT bool binary_upgrade_record_init_privs;

#endif   /* BINARY_UPGRADE_H */
