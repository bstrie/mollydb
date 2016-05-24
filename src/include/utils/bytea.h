/*-------------------------------------------------------------------------
 *
 * bytea.h
 *	  Declarations for BYTEA data type support.
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/bytea.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef BYTEA_H
#define BYTEA_H

#include "fmgr.h"


typedef enum
{
	BYTEA_OUTPUT_ESCAPE,
	BYTEA_OUTPUT_HEX
}	ByteaOutputType;

extern int	bytea_output;		/* ByteaOutputType, but int for GUC enum */

/* functions are in utils/adt/varlena.c */
extern Datum byteain(MDB_FUNCTION_ARGS);
extern Datum byteaout(MDB_FUNCTION_ARGS);
extern Datum bytearecv(MDB_FUNCTION_ARGS);
extern Datum byteasend(MDB_FUNCTION_ARGS);
extern Datum byteaoctetlen(MDB_FUNCTION_ARGS);
extern Datum byteaGetByte(MDB_FUNCTION_ARGS);
extern Datum byteaGetBit(MDB_FUNCTION_ARGS);
extern Datum byteaSetByte(MDB_FUNCTION_ARGS);
extern Datum byteaSetBit(MDB_FUNCTION_ARGS);
extern Datum byteaeq(MDB_FUNCTION_ARGS);
extern Datum byteane(MDB_FUNCTION_ARGS);
extern Datum bytealt(MDB_FUNCTION_ARGS);
extern Datum byteale(MDB_FUNCTION_ARGS);
extern Datum byteagt(MDB_FUNCTION_ARGS);
extern Datum byteage(MDB_FUNCTION_ARGS);
extern Datum byteacmp(MDB_FUNCTION_ARGS);
extern Datum bytea_sortsupport(MDB_FUNCTION_ARGS);
extern Datum byteacat(MDB_FUNCTION_ARGS);
extern Datum byteapos(MDB_FUNCTION_ARGS);
extern Datum bytea_substr(MDB_FUNCTION_ARGS);
extern Datum bytea_substr_no_len(MDB_FUNCTION_ARGS);
extern Datum byteaoverlay(MDB_FUNCTION_ARGS);
extern Datum byteaoverlay_no_len(MDB_FUNCTION_ARGS);

#endif   /* BYTEA_H */
