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
#ifndef PG_LSN_H
#define PG_LSN_H

#include "fmgr.h"
#include "access/xlogdefs.h"

extern Datum mdb_lsn_in(PG_FUNCTION_ARGS);
extern Datum mdb_lsn_out(PG_FUNCTION_ARGS);
extern Datum mdb_lsn_recv(PG_FUNCTION_ARGS);
extern Datum mdb_lsn_send(PG_FUNCTION_ARGS);

extern Datum mdb_lsn_eq(PG_FUNCTION_ARGS);
extern Datum mdb_lsn_ne(PG_FUNCTION_ARGS);
extern Datum mdb_lsn_lt(PG_FUNCTION_ARGS);
extern Datum mdb_lsn_gt(PG_FUNCTION_ARGS);
extern Datum mdb_lsn_le(PG_FUNCTION_ARGS);
extern Datum mdb_lsn_ge(PG_FUNCTION_ARGS);
extern Datum mdb_lsn_cmp(PG_FUNCTION_ARGS);
extern Datum mdb_lsn_hash(PG_FUNCTION_ARGS);

extern Datum mdb_lsn_mi(PG_FUNCTION_ARGS);

#define DatumGetLSN(X) ((XLogRecPtr) DatumGetInt64(X))
#define LSNGetDatum(X) (Int64GetDatum((int64) (X)))

#define PG_GETARG_LSN(n)	 DatumGetLSN(PG_GETARG_DATUM(n))
#define PG_RETURN_LSN(x)	 return LSNGetDatum(x)

#endif   /* PG_LSN_H */
