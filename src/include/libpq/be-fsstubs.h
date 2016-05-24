/*-------------------------------------------------------------------------
 *
 * be-fsstubs.h
 *
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/libpq/be-fsstubs.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef BE_FSSTUBS_H
#define BE_FSSTUBS_H

#include "fmgr.h"

/*
 * LO functions available via mdb_proc entries
 */
extern Datum lo_import(MDB_FUNCTION_ARGS);
extern Datum lo_import_with_oid(MDB_FUNCTION_ARGS);
extern Datum lo_export(MDB_FUNCTION_ARGS);

extern Datum lo_creat(MDB_FUNCTION_ARGS);
extern Datum lo_create(MDB_FUNCTION_ARGS);
extern Datum lo_from_bytea(MDB_FUNCTION_ARGS);

extern Datum lo_open(MDB_FUNCTION_ARGS);
extern Datum lo_close(MDB_FUNCTION_ARGS);

extern Datum loread(MDB_FUNCTION_ARGS);
extern Datum lowrite(MDB_FUNCTION_ARGS);

extern Datum lo_get(MDB_FUNCTION_ARGS);
extern Datum lo_get_fragment(MDB_FUNCTION_ARGS);
extern Datum lo_put(MDB_FUNCTION_ARGS);

extern Datum lo_lseek(MDB_FUNCTION_ARGS);
extern Datum lo_tell(MDB_FUNCTION_ARGS);
extern Datum lo_lseek64(MDB_FUNCTION_ARGS);
extern Datum lo_tell64(MDB_FUNCTION_ARGS);
extern Datum lo_unlink(MDB_FUNCTION_ARGS);
extern Datum lo_truncate(MDB_FUNCTION_ARGS);
extern Datum lo_truncate64(MDB_FUNCTION_ARGS);

/*
 * compatibility option for access control
 */
extern bool lo_compat_privileges;

/*
 * These are not fmgr-callable, but are available to C code.
 * Probably these should have had the underscore-free names,
 * but too late now...
 */
extern int	lo_read(int fd, char *buf, int len);
extern int	lo_write(int fd, const char *buf, int len);

/*
 * Cleanup LOs at xact commit/abort
 */
extern void AtEOXact_LargeObject(bool isCommit);
extern void AtEOSubXact_LargeObject(bool isCommit, SubTransactionId mySubid,
						SubTransactionId parentSubid);

#endif   /* BE_FSSTUBS_H */
