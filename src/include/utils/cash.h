/*
 * src/include/utils/cash.h
 *
 *
 * cash.h
 * Written by D'Arcy J.M. Cain
 *
 * Functions to allow input and output of money normally but store
 *	and handle it as 64 bit integer.
 */

#ifndef CASH_H
#define CASH_H

#include "fmgr.h"

typedef int64 Cash;

/* Cash is pass-by-reference if and only if int64 is */
#define DatumGetCash(X)		((Cash) DatumGetInt64(X))
#define CashGetDatum(X)		Int64GetDatum(X)
#define MDB_GETARG_CASH(n)	DatumGetCash(MDB_GETARG_DATUM(n))
#define MDB_RETURN_CASH(x)	return CashGetDatum(x)

extern Datum cash_in(MDB_FUNCTION_ARGS);
extern Datum cash_out(MDB_FUNCTION_ARGS);
extern Datum cash_recv(MDB_FUNCTION_ARGS);
extern Datum cash_send(MDB_FUNCTION_ARGS);

extern Datum cash_eq(MDB_FUNCTION_ARGS);
extern Datum cash_ne(MDB_FUNCTION_ARGS);
extern Datum cash_lt(MDB_FUNCTION_ARGS);
extern Datum cash_le(MDB_FUNCTION_ARGS);
extern Datum cash_gt(MDB_FUNCTION_ARGS);
extern Datum cash_ge(MDB_FUNCTION_ARGS);
extern Datum cash_cmp(MDB_FUNCTION_ARGS);

extern Datum cash_pl(MDB_FUNCTION_ARGS);
extern Datum cash_mi(MDB_FUNCTION_ARGS);
extern Datum cash_div_cash(MDB_FUNCTION_ARGS);

extern Datum cash_mul_flt8(MDB_FUNCTION_ARGS);
extern Datum flt8_mul_cash(MDB_FUNCTION_ARGS);
extern Datum cash_div_flt8(MDB_FUNCTION_ARGS);

extern Datum cash_mul_flt4(MDB_FUNCTION_ARGS);
extern Datum flt4_mul_cash(MDB_FUNCTION_ARGS);
extern Datum cash_div_flt4(MDB_FUNCTION_ARGS);

extern Datum cash_mul_int8(MDB_FUNCTION_ARGS);
extern Datum int8_mul_cash(MDB_FUNCTION_ARGS);
extern Datum cash_div_int8(MDB_FUNCTION_ARGS);

extern Datum cash_mul_int4(MDB_FUNCTION_ARGS);
extern Datum int4_mul_cash(MDB_FUNCTION_ARGS);
extern Datum cash_div_int4(MDB_FUNCTION_ARGS);

extern Datum cash_mul_int2(MDB_FUNCTION_ARGS);
extern Datum int2_mul_cash(MDB_FUNCTION_ARGS);
extern Datum cash_div_int2(MDB_FUNCTION_ARGS);

extern Datum cashlarger(MDB_FUNCTION_ARGS);
extern Datum cashsmaller(MDB_FUNCTION_ARGS);

extern Datum cash_words(MDB_FUNCTION_ARGS);

extern Datum cash_numeric(MDB_FUNCTION_ARGS);
extern Datum numeric_cash(MDB_FUNCTION_ARGS);

extern Datum int4_cash(MDB_FUNCTION_ARGS);
extern Datum int8_cash(MDB_FUNCTION_ARGS);

#endif   /* CASH_H */
