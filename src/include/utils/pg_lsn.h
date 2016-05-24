/*-------------------------------------------------------------------------
 *
 * mdb_lsn.h
 *		Declarations for operations on log sequence numbers (LSNs) of
 *		MollyDB.
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/mdb_lsn.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef MDB_LSN_H
#define MDB_LSN_H

#include "fmgr.h"
#include "access/xlogdefs.h"

extern Datum mdb_lsn_in(MDB_FUNCTION_ARGS);
extern Datum mdb_lsn_out(MDB_FUNCTION_ARGS);
extern Datum mdb_lsn_recv(MDB_FUNCTION_ARGS);
extern Datum mdb_lsn_send(MDB_FUNCTION_ARGS);

extern Datum mdb_lsn_eq(MDB_FUNCTION_ARGS);
extern Datum mdb_lsn_ne(MDB_FUNCTION_ARGS);
extern Datum mdb_lsn_lt(MDB_FUNCTION_ARGS);
extern Datum mdb_lsn_gt(MDB_FUNCTION_ARGS);
extern Datum mdb_lsn_le(MDB_FUNCTION_ARGS);
extern Datum mdb_lsn_ge(MDB_FUNCTION_ARGS);
extern Datum mdb_lsn_cmp(MDB_FUNCTION_ARGS);
extern Datum mdb_lsn_hash(MDB_FUNCTION_ARGS);

extern Datum mdb_lsn_mi(MDB_FUNCTION_ARGS);

#define DatumGetLSN(X) ((XLogRecPtr) DatumGetInt64(X))
#define LSNGetDatum(X) (Int64GetDatum((int64) (X)))

#define MDB_GETARG_LSN(n)	 DatumGetLSN(MDB_GETARG_DATUM(n))
#define MDB_RETURN_LSN(x)	 return LSNGetDatum(x)

#endif   /* MDB_LSN_H */
