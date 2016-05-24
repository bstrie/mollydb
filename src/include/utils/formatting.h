/* -----------------------------------------------------------------------
 * formatting.h
 *
 * src/include/utils/formatting.h
 *
 *
 *	 Portions Copyright (c) 1999-2016, MollyDB Global Development Group
 *
 *	 The MollyDB routines for a DateTime/int/float/numeric formatting,
 *	 inspire with Oracle TO_CHAR() / TO_DATE() / TO_NUMBER() routines.
 *
 *	 Karel Zak
 *
 * -----------------------------------------------------------------------
 */

#ifndef _FORMATTING_H_
#define _FORMATTING_H_

#include "fmgr.h"


extern char *str_tolower(const char *buff, size_t nbytes, Oid collid);
extern char *str_toupper(const char *buff, size_t nbytes, Oid collid);
extern char *str_initcap(const char *buff, size_t nbytes, Oid collid);

extern char *asc_tolower(const char *buff, size_t nbytes);
extern char *asc_toupper(const char *buff, size_t nbytes);
extern char *asc_initcap(const char *buff, size_t nbytes);

extern Datum timestamp_to_char(MDB_FUNCTION_ARGS);
extern Datum timestamptz_to_char(MDB_FUNCTION_ARGS);
extern Datum interval_to_char(MDB_FUNCTION_ARGS);
extern Datum to_timestamp(MDB_FUNCTION_ARGS);
extern Datum to_date(MDB_FUNCTION_ARGS);
extern Datum numeric_to_number(MDB_FUNCTION_ARGS);
extern Datum numeric_to_char(MDB_FUNCTION_ARGS);
extern Datum int4_to_char(MDB_FUNCTION_ARGS);
extern Datum int8_to_char(MDB_FUNCTION_ARGS);
extern Datum float4_to_char(MDB_FUNCTION_ARGS);
extern Datum float8_to_char(MDB_FUNCTION_ARGS);

#endif
