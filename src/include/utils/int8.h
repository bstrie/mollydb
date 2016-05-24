/*-------------------------------------------------------------------------
 *
 * int8.h
 *	  Declarations for operations on 64-bit integers.
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/int8.h
 *
 * NOTES
 * These data types are supported on all 64-bit architectures, and may
 *	be supported through libraries on some 32-bit machines. If your machine
 *	is not currently supported, then please try to make it so, then post
 *	patches to the mollydb.org hackers mailing list.
 *
 *-------------------------------------------------------------------------
 */
#ifndef INT8_H
#define INT8_H

#include "fmgr.h"


extern bool scanint8(const char *str, bool errorOK, int64 *result);

extern Datum int8in(MDB_FUNCTION_ARGS);
extern Datum int8out(MDB_FUNCTION_ARGS);
extern Datum int8recv(MDB_FUNCTION_ARGS);
extern Datum int8send(MDB_FUNCTION_ARGS);

extern Datum int8eq(MDB_FUNCTION_ARGS);
extern Datum int8ne(MDB_FUNCTION_ARGS);
extern Datum int8lt(MDB_FUNCTION_ARGS);
extern Datum int8gt(MDB_FUNCTION_ARGS);
extern Datum int8le(MDB_FUNCTION_ARGS);
extern Datum int8ge(MDB_FUNCTION_ARGS);

extern Datum int84eq(MDB_FUNCTION_ARGS);
extern Datum int84ne(MDB_FUNCTION_ARGS);
extern Datum int84lt(MDB_FUNCTION_ARGS);
extern Datum int84gt(MDB_FUNCTION_ARGS);
extern Datum int84le(MDB_FUNCTION_ARGS);
extern Datum int84ge(MDB_FUNCTION_ARGS);

extern Datum int48eq(MDB_FUNCTION_ARGS);
extern Datum int48ne(MDB_FUNCTION_ARGS);
extern Datum int48lt(MDB_FUNCTION_ARGS);
extern Datum int48gt(MDB_FUNCTION_ARGS);
extern Datum int48le(MDB_FUNCTION_ARGS);
extern Datum int48ge(MDB_FUNCTION_ARGS);

extern Datum int82eq(MDB_FUNCTION_ARGS);
extern Datum int82ne(MDB_FUNCTION_ARGS);
extern Datum int82lt(MDB_FUNCTION_ARGS);
extern Datum int82gt(MDB_FUNCTION_ARGS);
extern Datum int82le(MDB_FUNCTION_ARGS);
extern Datum int82ge(MDB_FUNCTION_ARGS);

extern Datum int28eq(MDB_FUNCTION_ARGS);
extern Datum int28ne(MDB_FUNCTION_ARGS);
extern Datum int28lt(MDB_FUNCTION_ARGS);
extern Datum int28gt(MDB_FUNCTION_ARGS);
extern Datum int28le(MDB_FUNCTION_ARGS);
extern Datum int28ge(MDB_FUNCTION_ARGS);

extern Datum int8um(MDB_FUNCTION_ARGS);
extern Datum int8up(MDB_FUNCTION_ARGS);
extern Datum int8pl(MDB_FUNCTION_ARGS);
extern Datum int8mi(MDB_FUNCTION_ARGS);
extern Datum int8mul(MDB_FUNCTION_ARGS);
extern Datum int8div(MDB_FUNCTION_ARGS);
extern Datum int8abs(MDB_FUNCTION_ARGS);
extern Datum int8mod(MDB_FUNCTION_ARGS);
extern Datum int8inc(MDB_FUNCTION_ARGS);
extern Datum int8dec(MDB_FUNCTION_ARGS);
extern Datum int8inc_any(MDB_FUNCTION_ARGS);
extern Datum int8inc_float8_float8(MDB_FUNCTION_ARGS);
extern Datum int8dec_any(MDB_FUNCTION_ARGS);
extern Datum int8larger(MDB_FUNCTION_ARGS);
extern Datum int8smaller(MDB_FUNCTION_ARGS);

extern Datum int8and(MDB_FUNCTION_ARGS);
extern Datum int8or(MDB_FUNCTION_ARGS);
extern Datum int8xor(MDB_FUNCTION_ARGS);
extern Datum int8not(MDB_FUNCTION_ARGS);
extern Datum int8shl(MDB_FUNCTION_ARGS);
extern Datum int8shr(MDB_FUNCTION_ARGS);

extern Datum int84pl(MDB_FUNCTION_ARGS);
extern Datum int84mi(MDB_FUNCTION_ARGS);
extern Datum int84mul(MDB_FUNCTION_ARGS);
extern Datum int84div(MDB_FUNCTION_ARGS);

extern Datum int48pl(MDB_FUNCTION_ARGS);
extern Datum int48mi(MDB_FUNCTION_ARGS);
extern Datum int48mul(MDB_FUNCTION_ARGS);
extern Datum int48div(MDB_FUNCTION_ARGS);

extern Datum int82pl(MDB_FUNCTION_ARGS);
extern Datum int82mi(MDB_FUNCTION_ARGS);
extern Datum int82mul(MDB_FUNCTION_ARGS);
extern Datum int82div(MDB_FUNCTION_ARGS);

extern Datum int28pl(MDB_FUNCTION_ARGS);
extern Datum int28mi(MDB_FUNCTION_ARGS);
extern Datum int28mul(MDB_FUNCTION_ARGS);
extern Datum int28div(MDB_FUNCTION_ARGS);

extern Datum int48(MDB_FUNCTION_ARGS);
extern Datum int84(MDB_FUNCTION_ARGS);

extern Datum int28(MDB_FUNCTION_ARGS);
extern Datum int82(MDB_FUNCTION_ARGS);

extern Datum i8tod(MDB_FUNCTION_ARGS);
extern Datum dtoi8(MDB_FUNCTION_ARGS);

extern Datum i8tof(MDB_FUNCTION_ARGS);
extern Datum ftoi8(MDB_FUNCTION_ARGS);

extern Datum i8tooid(MDB_FUNCTION_ARGS);
extern Datum oidtoi8(MDB_FUNCTION_ARGS);

extern Datum generate_series_int8(MDB_FUNCTION_ARGS);
extern Datum generate_series_step_int8(MDB_FUNCTION_ARGS);

#endif   /* INT8_H */
