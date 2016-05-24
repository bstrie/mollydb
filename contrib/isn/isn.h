/*-------------------------------------------------------------------------
 *
 * isn.h
 *	  MollyDB type definitions for ISNs (ISBN, ISMN, ISSN, EAN13, UPC)
 *
 * Author:	German Mendez Bravo (Kronuz)
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 *
 * IDENTIFICATION
 *	  contrib/isn/isn.h
 *
 *-------------------------------------------------------------------------
 */

#ifndef ISN_H
#define ISN_H

#include "fmgr.h"

#undef ISN_DEBUG
#define ISN_WEAK_MODE

/*
 *	uint64 is the internal storage format for ISNs.
 */
typedef uint64 ean13;

#define EAN13_FORMAT UINT64_FORMAT

#define MDB_GETARG_EAN13(n) MDB_GETARG_INT64(n)
#define MDB_RETURN_EAN13(x) MDB_RETURN_INT64(x)

extern Datum isn_out(MDB_FUNCTION_ARGS);
extern Datum ean13_out(MDB_FUNCTION_ARGS);
extern Datum ean13_in(MDB_FUNCTION_ARGS);
extern Datum isbn_in(MDB_FUNCTION_ARGS);
extern Datum ismn_in(MDB_FUNCTION_ARGS);
extern Datum issn_in(MDB_FUNCTION_ARGS);
extern Datum upc_in(MDB_FUNCTION_ARGS);

extern Datum isbn_cast_from_ean13(MDB_FUNCTION_ARGS);
extern Datum ismn_cast_from_ean13(MDB_FUNCTION_ARGS);
extern Datum issn_cast_from_ean13(MDB_FUNCTION_ARGS);
extern Datum upc_cast_from_ean13(MDB_FUNCTION_ARGS);

extern Datum is_valid(MDB_FUNCTION_ARGS);
extern Datum make_valid(MDB_FUNCTION_ARGS);

extern Datum accept_weak_input(MDB_FUNCTION_ARGS);
extern Datum weak_input_status(MDB_FUNCTION_ARGS);

extern void initialize(void);

#endif   /* ISN_H */
