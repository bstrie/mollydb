/*-------------------------------------------------------------------------
 *
 * uuid.h
 *	  Header file for the "uuid" ADT. In C, we use the name mdb_uuid_t,
 *	  to avoid conflicts with any uuid_t type that might be defined by
 *	  the system headers.
 *
 * Copyright (c) 2007-2016, MollyDB Global Development Group
 *
 * src/include/utils/uuid.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef UUID_H
#define UUID_H

/* guid size in bytes */
#define UUID_LEN 16

/* opaque struct; defined in uuid.c */
typedef struct mdb_uuid_t mdb_uuid_t;

/* fmgr interface macros */
#define UUIDPGetDatum(X)		PointerGetDatum(X)
#define MDB_RETURN_UUID_P(X)		return UUIDPGetDatum(X)
#define DatumGetUUIDP(X)		((mdb_uuid_t *) DatumGetPointer(X))
#define MDB_GETARG_UUID_P(X)		DatumGetUUIDP(MDB_GETARG_DATUM(X))

#endif   /* UUID_H */
