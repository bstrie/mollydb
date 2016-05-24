/*-------------------------------------------------------------------------
 *
 * mollydb_ext.h
 *
 *	   This file contains declarations of things that are visible everywhere
 *	in MollyDB *and* are visible to clients of frontend interface libraries.
 *	For example, the Oid type is part of the API of libpq and other libraries.
 *
 *	   Declarations which are specific to a particular interface should
 *	go in the header file for that interface (such as libpq-fe.h).  This
 *	file is only for fundamental MollyDB declarations.
 *
 *	   User-written C functions don't count as "external to MollyDB."
 *	Those function much as local modifications to the backend itself, and
 *	use header files that are otherwise internal to MollyDB to interface
 *	with the backend.
 *
 * src/include/mollydb_ext.h
 *
 *-------------------------------------------------------------------------
 */

#ifndef POSTGRES_EXT_H
#define POSTGRES_EXT_H

#include "mdb_config_ext.h"

/*
 * Object ID is a fundamental type in MollyDB.
 */
typedef unsigned int Oid;

#ifdef __cplusplus
#define InvalidOid		(Oid(0))
#else
#define InvalidOid		((Oid) 0)
#endif

#define OID_MAX  UINT_MAX
/* you will need to include <limits.h> to use the above #define */

/* Define a signed 64-bit integer type for use in client API declarations. */
typedef MDB_INT64_TYPE mdb_int64;


/*
 * Identifiers of error message fields.  Kept here to keep common
 * between frontend and backend, and also to export them to libpq
 * applications.
 */
#define MDB_DIAG_SEVERITY		'S'
#define MDB_DIAG_SQLSTATE		'C'
#define MDB_DIAG_MESSAGE_PRIMARY 'M'
#define MDB_DIAG_MESSAGE_DETAIL	'D'
#define MDB_DIAG_MESSAGE_HINT	'H'
#define MDB_DIAG_STATEMENT_POSITION 'P'
#define MDB_DIAG_INTERNAL_POSITION 'p'
#define MDB_DIAG_INTERNAL_QUERY	'q'
#define MDB_DIAG_CONTEXT			'W'
#define MDB_DIAG_SCHEMA_NAME		's'
#define MDB_DIAG_TABLE_NAME		't'
#define MDB_DIAG_COLUMN_NAME		'c'
#define MDB_DIAG_DATATYPE_NAME	'd'
#define MDB_DIAG_CONSTRAINT_NAME 'n'
#define MDB_DIAG_SOURCE_FILE		'F'
#define MDB_DIAG_SOURCE_LINE		'L'
#define MDB_DIAG_SOURCE_FUNCTION 'R'

#endif   /* POSTGRES_EXT_H */
