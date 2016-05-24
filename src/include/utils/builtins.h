/*-------------------------------------------------------------------------
 *
 * builtins.h
 *	  Declarations for operations on built-in types.
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/builtins.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef BUILTINS_H
#define BUILTINS_H

#include "fmgr.h"
#include "nodes/parsenodes.h"
#include "utils/sortsupport.h"

/*
 *		Defined in adt/
 */

/* acl.c */
extern Datum has_any_column_privilege_name_name(MDB_FUNCTION_ARGS);
extern Datum has_any_column_privilege_name_id(MDB_FUNCTION_ARGS);
extern Datum has_any_column_privilege_id_name(MDB_FUNCTION_ARGS);
extern Datum has_any_column_privilege_id_id(MDB_FUNCTION_ARGS);
extern Datum has_any_column_privilege_name(MDB_FUNCTION_ARGS);
extern Datum has_any_column_privilege_id(MDB_FUNCTION_ARGS);
extern Datum has_column_privilege_name_name_name(MDB_FUNCTION_ARGS);
extern Datum has_column_privilege_name_name_attnum(MDB_FUNCTION_ARGS);
extern Datum has_column_privilege_name_id_name(MDB_FUNCTION_ARGS);
extern Datum has_column_privilege_name_id_attnum(MDB_FUNCTION_ARGS);
extern Datum has_column_privilege_id_name_name(MDB_FUNCTION_ARGS);
extern Datum has_column_privilege_id_name_attnum(MDB_FUNCTION_ARGS);
extern Datum has_column_privilege_id_id_name(MDB_FUNCTION_ARGS);
extern Datum has_column_privilege_id_id_attnum(MDB_FUNCTION_ARGS);
extern Datum has_column_privilege_name_name(MDB_FUNCTION_ARGS);
extern Datum has_column_privilege_name_attnum(MDB_FUNCTION_ARGS);
extern Datum has_column_privilege_id_name(MDB_FUNCTION_ARGS);
extern Datum has_column_privilege_id_attnum(MDB_FUNCTION_ARGS);
extern Datum has_table_privilege_name_name(MDB_FUNCTION_ARGS);
extern Datum has_table_privilege_name_id(MDB_FUNCTION_ARGS);
extern Datum has_table_privilege_id_name(MDB_FUNCTION_ARGS);
extern Datum has_table_privilege_id_id(MDB_FUNCTION_ARGS);
extern Datum has_table_privilege_name(MDB_FUNCTION_ARGS);
extern Datum has_table_privilege_id(MDB_FUNCTION_ARGS);
extern Datum has_sequence_privilege_name_name(MDB_FUNCTION_ARGS);
extern Datum has_sequence_privilege_name_id(MDB_FUNCTION_ARGS);
extern Datum has_sequence_privilege_id_name(MDB_FUNCTION_ARGS);
extern Datum has_sequence_privilege_id_id(MDB_FUNCTION_ARGS);
extern Datum has_sequence_privilege_name(MDB_FUNCTION_ARGS);
extern Datum has_sequence_privilege_id(MDB_FUNCTION_ARGS);
extern Datum has_database_privilege_name_name(MDB_FUNCTION_ARGS);
extern Datum has_database_privilege_name_id(MDB_FUNCTION_ARGS);
extern Datum has_database_privilege_id_name(MDB_FUNCTION_ARGS);
extern Datum has_database_privilege_id_id(MDB_FUNCTION_ARGS);
extern Datum has_database_privilege_name(MDB_FUNCTION_ARGS);
extern Datum has_database_privilege_id(MDB_FUNCTION_ARGS);
extern Datum has_foreign_data_wrapper_privilege_name_name(MDB_FUNCTION_ARGS);
extern Datum has_foreign_data_wrapper_privilege_name_id(MDB_FUNCTION_ARGS);
extern Datum has_foreign_data_wrapper_privilege_id_name(MDB_FUNCTION_ARGS);
extern Datum has_foreign_data_wrapper_privilege_id_id(MDB_FUNCTION_ARGS);
extern Datum has_foreign_data_wrapper_privilege_name(MDB_FUNCTION_ARGS);
extern Datum has_foreign_data_wrapper_privilege_id(MDB_FUNCTION_ARGS);
extern Datum has_function_privilege_name_name(MDB_FUNCTION_ARGS);
extern Datum has_function_privilege_name_id(MDB_FUNCTION_ARGS);
extern Datum has_function_privilege_id_name(MDB_FUNCTION_ARGS);
extern Datum has_function_privilege_id_id(MDB_FUNCTION_ARGS);
extern Datum has_function_privilege_name(MDB_FUNCTION_ARGS);
extern Datum has_function_privilege_id(MDB_FUNCTION_ARGS);
extern Datum has_language_privilege_name_name(MDB_FUNCTION_ARGS);
extern Datum has_language_privilege_name_id(MDB_FUNCTION_ARGS);
extern Datum has_language_privilege_id_name(MDB_FUNCTION_ARGS);
extern Datum has_language_privilege_id_id(MDB_FUNCTION_ARGS);
extern Datum has_language_privilege_name(MDB_FUNCTION_ARGS);
extern Datum has_language_privilege_id(MDB_FUNCTION_ARGS);
extern Datum has_schema_privilege_name_name(MDB_FUNCTION_ARGS);
extern Datum has_schema_privilege_name_id(MDB_FUNCTION_ARGS);
extern Datum has_schema_privilege_id_name(MDB_FUNCTION_ARGS);
extern Datum has_schema_privilege_id_id(MDB_FUNCTION_ARGS);
extern Datum has_schema_privilege_name(MDB_FUNCTION_ARGS);
extern Datum has_schema_privilege_id(MDB_FUNCTION_ARGS);
extern Datum has_server_privilege_name_name(MDB_FUNCTION_ARGS);
extern Datum has_server_privilege_name_id(MDB_FUNCTION_ARGS);
extern Datum has_server_privilege_id_name(MDB_FUNCTION_ARGS);
extern Datum has_server_privilege_id_id(MDB_FUNCTION_ARGS);
extern Datum has_server_privilege_name(MDB_FUNCTION_ARGS);
extern Datum has_server_privilege_id(MDB_FUNCTION_ARGS);
extern Datum has_tablespace_privilege_name_name(MDB_FUNCTION_ARGS);
extern Datum has_tablespace_privilege_name_id(MDB_FUNCTION_ARGS);
extern Datum has_tablespace_privilege_id_name(MDB_FUNCTION_ARGS);
extern Datum has_tablespace_privilege_id_id(MDB_FUNCTION_ARGS);
extern Datum has_tablespace_privilege_name(MDB_FUNCTION_ARGS);
extern Datum has_tablespace_privilege_id(MDB_FUNCTION_ARGS);
extern Datum has_type_privilege_name_name(MDB_FUNCTION_ARGS);
extern Datum has_type_privilege_name_id(MDB_FUNCTION_ARGS);
extern Datum has_type_privilege_id_name(MDB_FUNCTION_ARGS);
extern Datum has_type_privilege_id_id(MDB_FUNCTION_ARGS);
extern Datum has_type_privilege_name(MDB_FUNCTION_ARGS);
extern Datum has_type_privilege_id(MDB_FUNCTION_ARGS);
extern Datum mdb_has_role_name_name(MDB_FUNCTION_ARGS);
extern Datum mdb_has_role_name_id(MDB_FUNCTION_ARGS);
extern Datum mdb_has_role_id_name(MDB_FUNCTION_ARGS);
extern Datum mdb_has_role_id_id(MDB_FUNCTION_ARGS);
extern Datum mdb_has_role_name(MDB_FUNCTION_ARGS);
extern Datum mdb_has_role_id(MDB_FUNCTION_ARGS);

/* bool.c */
extern Datum boolin(MDB_FUNCTION_ARGS);
extern Datum boolout(MDB_FUNCTION_ARGS);
extern Datum boolrecv(MDB_FUNCTION_ARGS);
extern Datum boolsend(MDB_FUNCTION_ARGS);
extern Datum booltext(MDB_FUNCTION_ARGS);
extern Datum booleq(MDB_FUNCTION_ARGS);
extern Datum boolne(MDB_FUNCTION_ARGS);
extern Datum boollt(MDB_FUNCTION_ARGS);
extern Datum boolgt(MDB_FUNCTION_ARGS);
extern Datum boolle(MDB_FUNCTION_ARGS);
extern Datum boolge(MDB_FUNCTION_ARGS);
extern Datum booland_statefunc(MDB_FUNCTION_ARGS);
extern Datum boolor_statefunc(MDB_FUNCTION_ARGS);
extern Datum bool_accum(MDB_FUNCTION_ARGS);
extern Datum bool_accum_inv(MDB_FUNCTION_ARGS);
extern Datum bool_alltrue(MDB_FUNCTION_ARGS);
extern Datum bool_anytrue(MDB_FUNCTION_ARGS);
extern bool parse_bool(const char *value, bool *result);
extern bool parse_bool_with_len(const char *value, size_t len, bool *result);

/* char.c */
extern Datum charin(MDB_FUNCTION_ARGS);
extern Datum charout(MDB_FUNCTION_ARGS);
extern Datum charrecv(MDB_FUNCTION_ARGS);
extern Datum charsend(MDB_FUNCTION_ARGS);
extern Datum chareq(MDB_FUNCTION_ARGS);
extern Datum charne(MDB_FUNCTION_ARGS);
extern Datum charlt(MDB_FUNCTION_ARGS);
extern Datum charle(MDB_FUNCTION_ARGS);
extern Datum chargt(MDB_FUNCTION_ARGS);
extern Datum charge(MDB_FUNCTION_ARGS);
extern Datum chartoi4(MDB_FUNCTION_ARGS);
extern Datum i4tochar(MDB_FUNCTION_ARGS);
extern Datum text_char(MDB_FUNCTION_ARGS);
extern Datum char_text(MDB_FUNCTION_ARGS);

/* domains.c */
extern Datum domain_in(MDB_FUNCTION_ARGS);
extern Datum domain_recv(MDB_FUNCTION_ARGS);
extern void domain_check(Datum value, bool isnull, Oid domainType,
			 void **extra, MemoryContext mcxt);
extern int	errdatatype(Oid datatypeOid);
extern int	errdomainconstraint(Oid datatypeOid, const char *conname);

/* encode.c */
extern Datum binary_encode(MDB_FUNCTION_ARGS);
extern Datum binary_decode(MDB_FUNCTION_ARGS);
extern unsigned hex_encode(const char *src, unsigned len, char *dst);
extern unsigned hex_decode(const char *src, unsigned len, char *dst);

/* enum.c */
extern Datum enum_in(MDB_FUNCTION_ARGS);
extern Datum enum_out(MDB_FUNCTION_ARGS);
extern Datum enum_recv(MDB_FUNCTION_ARGS);
extern Datum enum_send(MDB_FUNCTION_ARGS);
extern Datum enum_lt(MDB_FUNCTION_ARGS);
extern Datum enum_le(MDB_FUNCTION_ARGS);
extern Datum enum_eq(MDB_FUNCTION_ARGS);
extern Datum enum_ne(MDB_FUNCTION_ARGS);
extern Datum enum_ge(MDB_FUNCTION_ARGS);
extern Datum enum_gt(MDB_FUNCTION_ARGS);
extern Datum enum_cmp(MDB_FUNCTION_ARGS);
extern Datum enum_smaller(MDB_FUNCTION_ARGS);
extern Datum enum_larger(MDB_FUNCTION_ARGS);
extern Datum enum_first(MDB_FUNCTION_ARGS);
extern Datum enum_last(MDB_FUNCTION_ARGS);
extern Datum enum_range_bounds(MDB_FUNCTION_ARGS);
extern Datum enum_range_all(MDB_FUNCTION_ARGS);

/* int.c */
extern Datum int2in(MDB_FUNCTION_ARGS);
extern Datum int2out(MDB_FUNCTION_ARGS);
extern Datum int2recv(MDB_FUNCTION_ARGS);
extern Datum int2send(MDB_FUNCTION_ARGS);
extern Datum int2vectorin(MDB_FUNCTION_ARGS);
extern Datum int2vectorout(MDB_FUNCTION_ARGS);
extern Datum int2vectorrecv(MDB_FUNCTION_ARGS);
extern Datum int2vectorsend(MDB_FUNCTION_ARGS);
extern Datum int2vectoreq(MDB_FUNCTION_ARGS);
extern Datum int4in(MDB_FUNCTION_ARGS);
extern Datum int4out(MDB_FUNCTION_ARGS);
extern Datum int4recv(MDB_FUNCTION_ARGS);
extern Datum int4send(MDB_FUNCTION_ARGS);
extern Datum i2toi4(MDB_FUNCTION_ARGS);
extern Datum i4toi2(MDB_FUNCTION_ARGS);
extern Datum int4_bool(MDB_FUNCTION_ARGS);
extern Datum bool_int4(MDB_FUNCTION_ARGS);
extern Datum int4eq(MDB_FUNCTION_ARGS);
extern Datum int4ne(MDB_FUNCTION_ARGS);
extern Datum int4lt(MDB_FUNCTION_ARGS);
extern Datum int4le(MDB_FUNCTION_ARGS);
extern Datum int4gt(MDB_FUNCTION_ARGS);
extern Datum int4ge(MDB_FUNCTION_ARGS);
extern Datum int2eq(MDB_FUNCTION_ARGS);
extern Datum int2ne(MDB_FUNCTION_ARGS);
extern Datum int2lt(MDB_FUNCTION_ARGS);
extern Datum int2le(MDB_FUNCTION_ARGS);
extern Datum int2gt(MDB_FUNCTION_ARGS);
extern Datum int2ge(MDB_FUNCTION_ARGS);
extern Datum int24eq(MDB_FUNCTION_ARGS);
extern Datum int24ne(MDB_FUNCTION_ARGS);
extern Datum int24lt(MDB_FUNCTION_ARGS);
extern Datum int24le(MDB_FUNCTION_ARGS);
extern Datum int24gt(MDB_FUNCTION_ARGS);
extern Datum int24ge(MDB_FUNCTION_ARGS);
extern Datum int42eq(MDB_FUNCTION_ARGS);
extern Datum int42ne(MDB_FUNCTION_ARGS);
extern Datum int42lt(MDB_FUNCTION_ARGS);
extern Datum int42le(MDB_FUNCTION_ARGS);
extern Datum int42gt(MDB_FUNCTION_ARGS);
extern Datum int42ge(MDB_FUNCTION_ARGS);
extern Datum int4um(MDB_FUNCTION_ARGS);
extern Datum int4up(MDB_FUNCTION_ARGS);
extern Datum int4pl(MDB_FUNCTION_ARGS);
extern Datum int4mi(MDB_FUNCTION_ARGS);
extern Datum int4mul(MDB_FUNCTION_ARGS);
extern Datum int4div(MDB_FUNCTION_ARGS);
extern Datum int4abs(MDB_FUNCTION_ARGS);
extern Datum int4inc(MDB_FUNCTION_ARGS);
extern Datum int2um(MDB_FUNCTION_ARGS);
extern Datum int2up(MDB_FUNCTION_ARGS);
extern Datum int2pl(MDB_FUNCTION_ARGS);
extern Datum int2mi(MDB_FUNCTION_ARGS);
extern Datum int2mul(MDB_FUNCTION_ARGS);
extern Datum int2div(MDB_FUNCTION_ARGS);
extern Datum int2abs(MDB_FUNCTION_ARGS);
extern Datum int24pl(MDB_FUNCTION_ARGS);
extern Datum int24mi(MDB_FUNCTION_ARGS);
extern Datum int24mul(MDB_FUNCTION_ARGS);
extern Datum int24div(MDB_FUNCTION_ARGS);
extern Datum int42pl(MDB_FUNCTION_ARGS);
extern Datum int42mi(MDB_FUNCTION_ARGS);
extern Datum int42mul(MDB_FUNCTION_ARGS);
extern Datum int42div(MDB_FUNCTION_ARGS);
extern Datum int4mod(MDB_FUNCTION_ARGS);
extern Datum int2mod(MDB_FUNCTION_ARGS);
extern Datum int2larger(MDB_FUNCTION_ARGS);
extern Datum int2smaller(MDB_FUNCTION_ARGS);
extern Datum int4larger(MDB_FUNCTION_ARGS);
extern Datum int4smaller(MDB_FUNCTION_ARGS);

extern Datum int4and(MDB_FUNCTION_ARGS);
extern Datum int4or(MDB_FUNCTION_ARGS);
extern Datum int4xor(MDB_FUNCTION_ARGS);
extern Datum int4not(MDB_FUNCTION_ARGS);
extern Datum int4shl(MDB_FUNCTION_ARGS);
extern Datum int4shr(MDB_FUNCTION_ARGS);
extern Datum int2and(MDB_FUNCTION_ARGS);
extern Datum int2or(MDB_FUNCTION_ARGS);
extern Datum int2xor(MDB_FUNCTION_ARGS);
extern Datum int2not(MDB_FUNCTION_ARGS);
extern Datum int2shl(MDB_FUNCTION_ARGS);
extern Datum int2shr(MDB_FUNCTION_ARGS);
extern Datum generate_series_int4(MDB_FUNCTION_ARGS);
extern Datum generate_series_step_int4(MDB_FUNCTION_ARGS);
extern int2vector *buildint2vector(const int16 *int2s, int n);

/* name.c */
extern Datum namein(MDB_FUNCTION_ARGS);
extern Datum nameout(MDB_FUNCTION_ARGS);
extern Datum namerecv(MDB_FUNCTION_ARGS);
extern Datum namesend(MDB_FUNCTION_ARGS);
extern Datum nameeq(MDB_FUNCTION_ARGS);
extern Datum namene(MDB_FUNCTION_ARGS);
extern Datum namelt(MDB_FUNCTION_ARGS);
extern Datum namele(MDB_FUNCTION_ARGS);
extern Datum namegt(MDB_FUNCTION_ARGS);
extern Datum namege(MDB_FUNCTION_ARGS);
extern int	namecpy(Name n1, Name n2);
extern int	namestrcpy(Name name, const char *str);
extern int	namestrcmp(Name name, const char *str);
extern Datum current_user(MDB_FUNCTION_ARGS);
extern Datum session_user(MDB_FUNCTION_ARGS);
extern Datum current_schema(MDB_FUNCTION_ARGS);
extern Datum current_schemas(MDB_FUNCTION_ARGS);

/* numutils.c */
extern int32 mdb_atoi(const char *s, int size, int c);
extern void mdb_itoa(int16 i, char *a);
extern void mdb_ltoa(int32 l, char *a);
extern void mdb_lltoa(int64 ll, char *a);
extern char *mdb_ltostr_zeropad(char *str, int32 value, int32 minwidth);
extern char *mdb_ltostr(char *str, int32 value);
extern uint64 mdb_strtouint64(const char *str, char **endptr, int base);

/*
 *		Per-opclass comparison functions for new btrees.  These are
 *		stored in mdb_amproc; most are defined in access/nbtree/nbtcompare.c
 */
extern Datum btboolcmp(MDB_FUNCTION_ARGS);
extern Datum btint2cmp(MDB_FUNCTION_ARGS);
extern Datum btint4cmp(MDB_FUNCTION_ARGS);
extern Datum btint8cmp(MDB_FUNCTION_ARGS);
extern Datum btfloat4cmp(MDB_FUNCTION_ARGS);
extern Datum btfloat8cmp(MDB_FUNCTION_ARGS);
extern Datum btint48cmp(MDB_FUNCTION_ARGS);
extern Datum btint84cmp(MDB_FUNCTION_ARGS);
extern Datum btint24cmp(MDB_FUNCTION_ARGS);
extern Datum btint42cmp(MDB_FUNCTION_ARGS);
extern Datum btint28cmp(MDB_FUNCTION_ARGS);
extern Datum btint82cmp(MDB_FUNCTION_ARGS);
extern Datum btfloat48cmp(MDB_FUNCTION_ARGS);
extern Datum btfloat84cmp(MDB_FUNCTION_ARGS);
extern Datum btoidcmp(MDB_FUNCTION_ARGS);
extern Datum btoidvectorcmp(MDB_FUNCTION_ARGS);
extern Datum btabstimecmp(MDB_FUNCTION_ARGS);
extern Datum btreltimecmp(MDB_FUNCTION_ARGS);
extern Datum bttintervalcmp(MDB_FUNCTION_ARGS);
extern Datum btcharcmp(MDB_FUNCTION_ARGS);
extern Datum btnamecmp(MDB_FUNCTION_ARGS);
extern Datum bttextcmp(MDB_FUNCTION_ARGS);
extern Datum bttextsortsupport(MDB_FUNCTION_ARGS);

/*
 *		Per-opclass sort support functions for new btrees.  Like the
 *		functions above, these are stored in mdb_amproc; most are defined in
 *		access/nbtree/nbtcompare.c
 */
extern Datum btint2sortsupport(MDB_FUNCTION_ARGS);
extern Datum btint4sortsupport(MDB_FUNCTION_ARGS);
extern Datum btint8sortsupport(MDB_FUNCTION_ARGS);
extern Datum btfloat4sortsupport(MDB_FUNCTION_ARGS);
extern Datum btfloat8sortsupport(MDB_FUNCTION_ARGS);
extern Datum btoidsortsupport(MDB_FUNCTION_ARGS);
extern Datum btnamesortsupport(MDB_FUNCTION_ARGS);

/* float.c */
extern PGDLLIMPORT int extra_float_digits;

extern double get_float8_infinity(void);
extern float get_float4_infinity(void);
extern double get_float8_nan(void);
extern float get_float4_nan(void);
extern int	is_infinite(double val);
extern double float8in_internal(char *num, char **endptr_p,
				  const char *type_name, const char *orig_string);
extern char *float8out_internal(double num);

extern Datum float4in(MDB_FUNCTION_ARGS);
extern Datum float4out(MDB_FUNCTION_ARGS);
extern Datum float4recv(MDB_FUNCTION_ARGS);
extern Datum float4send(MDB_FUNCTION_ARGS);
extern Datum float8in(MDB_FUNCTION_ARGS);
extern Datum float8out(MDB_FUNCTION_ARGS);
extern Datum float8recv(MDB_FUNCTION_ARGS);
extern Datum float8send(MDB_FUNCTION_ARGS);
extern Datum float4abs(MDB_FUNCTION_ARGS);
extern Datum float4um(MDB_FUNCTION_ARGS);
extern Datum float4up(MDB_FUNCTION_ARGS);
extern Datum float4larger(MDB_FUNCTION_ARGS);
extern Datum float4smaller(MDB_FUNCTION_ARGS);
extern Datum float8abs(MDB_FUNCTION_ARGS);
extern Datum float8um(MDB_FUNCTION_ARGS);
extern Datum float8up(MDB_FUNCTION_ARGS);
extern Datum float8larger(MDB_FUNCTION_ARGS);
extern Datum float8smaller(MDB_FUNCTION_ARGS);
extern Datum float4pl(MDB_FUNCTION_ARGS);
extern Datum float4mi(MDB_FUNCTION_ARGS);
extern Datum float4mul(MDB_FUNCTION_ARGS);
extern Datum float4div(MDB_FUNCTION_ARGS);
extern Datum float8pl(MDB_FUNCTION_ARGS);
extern Datum float8mi(MDB_FUNCTION_ARGS);
extern Datum float8mul(MDB_FUNCTION_ARGS);
extern Datum float8div(MDB_FUNCTION_ARGS);
extern Datum float4eq(MDB_FUNCTION_ARGS);
extern Datum float4ne(MDB_FUNCTION_ARGS);
extern Datum float4lt(MDB_FUNCTION_ARGS);
extern Datum float4le(MDB_FUNCTION_ARGS);
extern Datum float4gt(MDB_FUNCTION_ARGS);
extern Datum float4ge(MDB_FUNCTION_ARGS);
extern Datum float8eq(MDB_FUNCTION_ARGS);
extern Datum float8ne(MDB_FUNCTION_ARGS);
extern Datum float8lt(MDB_FUNCTION_ARGS);
extern Datum float8le(MDB_FUNCTION_ARGS);
extern Datum float8gt(MDB_FUNCTION_ARGS);
extern Datum float8ge(MDB_FUNCTION_ARGS);
extern Datum ftod(MDB_FUNCTION_ARGS);
extern Datum i4tod(MDB_FUNCTION_ARGS);
extern Datum i2tod(MDB_FUNCTION_ARGS);
extern Datum dtof(MDB_FUNCTION_ARGS);
extern Datum dtoi4(MDB_FUNCTION_ARGS);
extern Datum dtoi2(MDB_FUNCTION_ARGS);
extern Datum i4tof(MDB_FUNCTION_ARGS);
extern Datum i2tof(MDB_FUNCTION_ARGS);
extern Datum ftoi4(MDB_FUNCTION_ARGS);
extern Datum ftoi2(MDB_FUNCTION_ARGS);
extern Datum dround(MDB_FUNCTION_ARGS);
extern Datum dceil(MDB_FUNCTION_ARGS);
extern Datum dfloor(MDB_FUNCTION_ARGS);
extern Datum dsign(MDB_FUNCTION_ARGS);
extern Datum dtrunc(MDB_FUNCTION_ARGS);
extern Datum dsqrt(MDB_FUNCTION_ARGS);
extern Datum dcbrt(MDB_FUNCTION_ARGS);
extern Datum dpow(MDB_FUNCTION_ARGS);
extern Datum dexp(MDB_FUNCTION_ARGS);
extern Datum dlog1(MDB_FUNCTION_ARGS);
extern Datum dlog10(MDB_FUNCTION_ARGS);
extern Datum dacos(MDB_FUNCTION_ARGS);
extern Datum dasin(MDB_FUNCTION_ARGS);
extern Datum datan(MDB_FUNCTION_ARGS);
extern Datum datan2(MDB_FUNCTION_ARGS);
extern Datum dcos(MDB_FUNCTION_ARGS);
extern Datum dcot(MDB_FUNCTION_ARGS);
extern Datum dsin(MDB_FUNCTION_ARGS);
extern Datum dtan(MDB_FUNCTION_ARGS);
extern Datum dacosd(MDB_FUNCTION_ARGS);
extern Datum dasind(MDB_FUNCTION_ARGS);
extern Datum datand(MDB_FUNCTION_ARGS);
extern Datum datan2d(MDB_FUNCTION_ARGS);
extern Datum dcosd(MDB_FUNCTION_ARGS);
extern Datum dcotd(MDB_FUNCTION_ARGS);
extern Datum dsind(MDB_FUNCTION_ARGS);
extern Datum dtand(MDB_FUNCTION_ARGS);
extern Datum degrees(MDB_FUNCTION_ARGS);
extern Datum dpi(MDB_FUNCTION_ARGS);
extern Datum radians(MDB_FUNCTION_ARGS);
extern Datum drandom(MDB_FUNCTION_ARGS);
extern Datum setseed(MDB_FUNCTION_ARGS);
extern Datum float8_combine(MDB_FUNCTION_ARGS);
extern Datum float8_accum(MDB_FUNCTION_ARGS);
extern Datum float4_accum(MDB_FUNCTION_ARGS);
extern Datum float8_avg(MDB_FUNCTION_ARGS);
extern Datum float8_var_pop(MDB_FUNCTION_ARGS);
extern Datum float8_var_samp(MDB_FUNCTION_ARGS);
extern Datum float8_stddev_pop(MDB_FUNCTION_ARGS);
extern Datum float8_stddev_samp(MDB_FUNCTION_ARGS);
extern Datum float8_regr_accum(MDB_FUNCTION_ARGS);
extern Datum float8_regr_combine(MDB_FUNCTION_ARGS);
extern Datum float8_regr_sxx(MDB_FUNCTION_ARGS);
extern Datum float8_regr_syy(MDB_FUNCTION_ARGS);
extern Datum float8_regr_sxy(MDB_FUNCTION_ARGS);
extern Datum float8_regr_avgx(MDB_FUNCTION_ARGS);
extern Datum float8_regr_avgy(MDB_FUNCTION_ARGS);
extern Datum float8_covar_pop(MDB_FUNCTION_ARGS);
extern Datum float8_covar_samp(MDB_FUNCTION_ARGS);
extern Datum float8_corr(MDB_FUNCTION_ARGS);
extern Datum float8_regr_r2(MDB_FUNCTION_ARGS);
extern Datum float8_regr_slope(MDB_FUNCTION_ARGS);
extern Datum float8_regr_intercept(MDB_FUNCTION_ARGS);
extern Datum float48pl(MDB_FUNCTION_ARGS);
extern Datum float48mi(MDB_FUNCTION_ARGS);
extern Datum float48mul(MDB_FUNCTION_ARGS);
extern Datum float48div(MDB_FUNCTION_ARGS);
extern Datum float84pl(MDB_FUNCTION_ARGS);
extern Datum float84mi(MDB_FUNCTION_ARGS);
extern Datum float84mul(MDB_FUNCTION_ARGS);
extern Datum float84div(MDB_FUNCTION_ARGS);
extern Datum float48eq(MDB_FUNCTION_ARGS);
extern Datum float48ne(MDB_FUNCTION_ARGS);
extern Datum float48lt(MDB_FUNCTION_ARGS);
extern Datum float48le(MDB_FUNCTION_ARGS);
extern Datum float48gt(MDB_FUNCTION_ARGS);
extern Datum float48ge(MDB_FUNCTION_ARGS);
extern Datum float84eq(MDB_FUNCTION_ARGS);
extern Datum float84ne(MDB_FUNCTION_ARGS);
extern Datum float84lt(MDB_FUNCTION_ARGS);
extern Datum float84le(MDB_FUNCTION_ARGS);
extern Datum float84gt(MDB_FUNCTION_ARGS);
extern Datum float84ge(MDB_FUNCTION_ARGS);
extern Datum width_bucket_float8(MDB_FUNCTION_ARGS);

/* dbsize.c */
extern Datum mdb_tablespace_size_oid(MDB_FUNCTION_ARGS);
extern Datum mdb_tablespace_size_name(MDB_FUNCTION_ARGS);
extern Datum mdb_database_size_oid(MDB_FUNCTION_ARGS);
extern Datum mdb_database_size_name(MDB_FUNCTION_ARGS);
extern Datum mdb_relation_size(MDB_FUNCTION_ARGS);
extern Datum mdb_total_relation_size(MDB_FUNCTION_ARGS);
extern Datum mdb_size_pretty(MDB_FUNCTION_ARGS);
extern Datum mdb_size_pretty_numeric(MDB_FUNCTION_ARGS);
extern Datum mdb_size_bytes(MDB_FUNCTION_ARGS);
extern Datum mdb_table_size(MDB_FUNCTION_ARGS);
extern Datum mdb_indexes_size(MDB_FUNCTION_ARGS);
extern Datum mdb_relation_filenode(MDB_FUNCTION_ARGS);
extern Datum mdb_filenode_relation(MDB_FUNCTION_ARGS);
extern Datum mdb_relation_filepath(MDB_FUNCTION_ARGS);

/* genfile.c */
extern Datum mdb_stat_file(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_file_1arg(MDB_FUNCTION_ARGS);
extern Datum mdb_read_file(MDB_FUNCTION_ARGS);
extern Datum mdb_read_file_off_len(MDB_FUNCTION_ARGS);
extern Datum mdb_read_file_all(MDB_FUNCTION_ARGS);
extern Datum mdb_read_binary_file(MDB_FUNCTION_ARGS);
extern Datum mdb_read_binary_file_off_len(MDB_FUNCTION_ARGS);
extern Datum mdb_read_binary_file_all(MDB_FUNCTION_ARGS);
extern Datum mdb_ls_dir(MDB_FUNCTION_ARGS);
extern Datum mdb_ls_dir_1arg(MDB_FUNCTION_ARGS);

/* misc.c */
extern Datum mdb_num_nulls(MDB_FUNCTION_ARGS);
extern Datum mdb_num_nonnulls(MDB_FUNCTION_ARGS);
extern Datum current_database(MDB_FUNCTION_ARGS);
extern Datum current_query(MDB_FUNCTION_ARGS);
extern Datum mdb_cancel_backend(MDB_FUNCTION_ARGS);
extern Datum mdb_terminate_backend(MDB_FUNCTION_ARGS);
extern Datum mdb_reload_conf(MDB_FUNCTION_ARGS);
extern Datum mdb_tablespace_databases(MDB_FUNCTION_ARGS);
extern Datum mdb_tablespace_location(MDB_FUNCTION_ARGS);
extern Datum mdb_rotate_logfile(MDB_FUNCTION_ARGS);
extern Datum mdb_sleep(MDB_FUNCTION_ARGS);
extern Datum mdb_get_keywords(MDB_FUNCTION_ARGS);
extern Datum mdb_typeof(MDB_FUNCTION_ARGS);
extern Datum mdb_collation_for(MDB_FUNCTION_ARGS);
extern Datum mdb_relation_is_updatable(MDB_FUNCTION_ARGS);
extern Datum mdb_column_is_updatable(MDB_FUNCTION_ARGS);
extern Datum parse_ident(MDB_FUNCTION_ARGS);

/* oid.c */
extern Datum oidin(MDB_FUNCTION_ARGS);
extern Datum oidout(MDB_FUNCTION_ARGS);
extern Datum oidrecv(MDB_FUNCTION_ARGS);
extern Datum oidsend(MDB_FUNCTION_ARGS);
extern Datum oideq(MDB_FUNCTION_ARGS);
extern Datum oidne(MDB_FUNCTION_ARGS);
extern Datum oidlt(MDB_FUNCTION_ARGS);
extern Datum oidle(MDB_FUNCTION_ARGS);
extern Datum oidge(MDB_FUNCTION_ARGS);
extern Datum oidgt(MDB_FUNCTION_ARGS);
extern Datum oidlarger(MDB_FUNCTION_ARGS);
extern Datum oidsmaller(MDB_FUNCTION_ARGS);
extern Datum oidvectorin(MDB_FUNCTION_ARGS);
extern Datum oidvectorout(MDB_FUNCTION_ARGS);
extern Datum oidvectorrecv(MDB_FUNCTION_ARGS);
extern Datum oidvectorsend(MDB_FUNCTION_ARGS);
extern Datum oidvectoreq(MDB_FUNCTION_ARGS);
extern Datum oidvectorne(MDB_FUNCTION_ARGS);
extern Datum oidvectorlt(MDB_FUNCTION_ARGS);
extern Datum oidvectorle(MDB_FUNCTION_ARGS);
extern Datum oidvectorge(MDB_FUNCTION_ARGS);
extern Datum oidvectorgt(MDB_FUNCTION_ARGS);
extern oidvector *buildoidvector(const Oid *oids, int n);
extern Oid	oidparse(Node *node);

/* orderedsetaggs.c */
extern Datum ordered_set_transition(MDB_FUNCTION_ARGS);
extern Datum ordered_set_transition_multi(MDB_FUNCTION_ARGS);
extern Datum percentile_disc_final(MDB_FUNCTION_ARGS);
extern Datum percentile_cont_float8_final(MDB_FUNCTION_ARGS);
extern Datum percentile_cont_interval_final(MDB_FUNCTION_ARGS);
extern Datum percentile_disc_multi_final(MDB_FUNCTION_ARGS);
extern Datum percentile_cont_float8_multi_final(MDB_FUNCTION_ARGS);
extern Datum percentile_cont_interval_multi_final(MDB_FUNCTION_ARGS);
extern Datum mode_final(MDB_FUNCTION_ARGS);
extern Datum hypothetical_rank_final(MDB_FUNCTION_ARGS);
extern Datum hypothetical_percent_rank_final(MDB_FUNCTION_ARGS);
extern Datum hypothetical_cume_dist_final(MDB_FUNCTION_ARGS);
extern Datum hypothetical_dense_rank_final(MDB_FUNCTION_ARGS);

/* pseudotypes.c */
extern Datum cstring_in(MDB_FUNCTION_ARGS);
extern Datum cstring_out(MDB_FUNCTION_ARGS);
extern Datum cstring_recv(MDB_FUNCTION_ARGS);
extern Datum cstring_send(MDB_FUNCTION_ARGS);
extern Datum any_in(MDB_FUNCTION_ARGS);
extern Datum any_out(MDB_FUNCTION_ARGS);
extern Datum anyarray_in(MDB_FUNCTION_ARGS);
extern Datum anyarray_out(MDB_FUNCTION_ARGS);
extern Datum anyarray_recv(MDB_FUNCTION_ARGS);
extern Datum anyarray_send(MDB_FUNCTION_ARGS);
extern Datum anynonarray_in(MDB_FUNCTION_ARGS);
extern Datum anynonarray_out(MDB_FUNCTION_ARGS);
extern Datum anyenum_in(MDB_FUNCTION_ARGS);
extern Datum anyenum_out(MDB_FUNCTION_ARGS);
extern Datum anyrange_in(MDB_FUNCTION_ARGS);
extern Datum anyrange_out(MDB_FUNCTION_ARGS);
extern Datum void_in(MDB_FUNCTION_ARGS);
extern Datum void_out(MDB_FUNCTION_ARGS);
extern Datum void_recv(MDB_FUNCTION_ARGS);
extern Datum void_send(MDB_FUNCTION_ARGS);
extern Datum trigger_in(MDB_FUNCTION_ARGS);
extern Datum trigger_out(MDB_FUNCTION_ARGS);
extern Datum event_trigger_in(MDB_FUNCTION_ARGS);
extern Datum event_trigger_out(MDB_FUNCTION_ARGS);
extern Datum language_handler_in(MDB_FUNCTION_ARGS);
extern Datum language_handler_out(MDB_FUNCTION_ARGS);
extern Datum fdw_handler_in(MDB_FUNCTION_ARGS);
extern Datum fdw_handler_out(MDB_FUNCTION_ARGS);
extern Datum index_am_handler_in(MDB_FUNCTION_ARGS);
extern Datum index_am_handler_out(MDB_FUNCTION_ARGS);
extern Datum tsm_handler_in(MDB_FUNCTION_ARGS);
extern Datum tsm_handler_out(MDB_FUNCTION_ARGS);
extern Datum internal_in(MDB_FUNCTION_ARGS);
extern Datum internal_out(MDB_FUNCTION_ARGS);
extern Datum opaque_in(MDB_FUNCTION_ARGS);
extern Datum opaque_out(MDB_FUNCTION_ARGS);
extern Datum anyelement_in(MDB_FUNCTION_ARGS);
extern Datum anyelement_out(MDB_FUNCTION_ARGS);
extern Datum shell_in(MDB_FUNCTION_ARGS);
extern Datum shell_out(MDB_FUNCTION_ARGS);
extern Datum mdb_node_tree_in(MDB_FUNCTION_ARGS);
extern Datum mdb_node_tree_out(MDB_FUNCTION_ARGS);
extern Datum mdb_node_tree_recv(MDB_FUNCTION_ARGS);
extern Datum mdb_node_tree_send(MDB_FUNCTION_ARGS);
extern Datum mdb_ddl_command_in(MDB_FUNCTION_ARGS);
extern Datum mdb_ddl_command_out(MDB_FUNCTION_ARGS);
extern Datum mdb_ddl_command_recv(MDB_FUNCTION_ARGS);
extern Datum mdb_ddl_command_send(MDB_FUNCTION_ARGS);

/* regexp.c */
extern Datum nameregexeq(MDB_FUNCTION_ARGS);
extern Datum nameregexne(MDB_FUNCTION_ARGS);
extern Datum textregexeq(MDB_FUNCTION_ARGS);
extern Datum textregexne(MDB_FUNCTION_ARGS);
extern Datum nameicregexeq(MDB_FUNCTION_ARGS);
extern Datum nameicregexne(MDB_FUNCTION_ARGS);
extern Datum texticregexeq(MDB_FUNCTION_ARGS);
extern Datum texticregexne(MDB_FUNCTION_ARGS);
extern Datum textregexsubstr(MDB_FUNCTION_ARGS);
extern Datum textregexreplace_noopt(MDB_FUNCTION_ARGS);
extern Datum textregexreplace(MDB_FUNCTION_ARGS);
extern Datum similar_escape(MDB_FUNCTION_ARGS);
extern Datum regexp_matches(MDB_FUNCTION_ARGS);
extern Datum regexp_matches_no_flags(MDB_FUNCTION_ARGS);
extern Datum regexp_split_to_table(MDB_FUNCTION_ARGS);
extern Datum regexp_split_to_table_no_flags(MDB_FUNCTION_ARGS);
extern Datum regexp_split_to_array(MDB_FUNCTION_ARGS);
extern Datum regexp_split_to_array_no_flags(MDB_FUNCTION_ARGS);
extern char *regexp_fixed_prefix(text *text_re, bool case_insensitive,
					Oid collation, bool *exact);

/* regproc.c */
extern Datum regprocin(MDB_FUNCTION_ARGS);
extern Datum regprocout(MDB_FUNCTION_ARGS);
extern Datum to_regproc(MDB_FUNCTION_ARGS);
extern Datum to_regprocedure(MDB_FUNCTION_ARGS);
extern Datum regprocrecv(MDB_FUNCTION_ARGS);
extern Datum regprocsend(MDB_FUNCTION_ARGS);
extern Datum regprocedurein(MDB_FUNCTION_ARGS);
extern Datum regprocedureout(MDB_FUNCTION_ARGS);
extern Datum regprocedurerecv(MDB_FUNCTION_ARGS);
extern Datum regproceduresend(MDB_FUNCTION_ARGS);
extern Datum regoperin(MDB_FUNCTION_ARGS);
extern Datum regoperout(MDB_FUNCTION_ARGS);
extern Datum regoperrecv(MDB_FUNCTION_ARGS);
extern Datum regopersend(MDB_FUNCTION_ARGS);
extern Datum to_regoper(MDB_FUNCTION_ARGS);
extern Datum to_regoperator(MDB_FUNCTION_ARGS);
extern Datum regoperatorin(MDB_FUNCTION_ARGS);
extern Datum regoperatorout(MDB_FUNCTION_ARGS);
extern Datum regoperatorrecv(MDB_FUNCTION_ARGS);
extern Datum regoperatorsend(MDB_FUNCTION_ARGS);
extern Datum regclassin(MDB_FUNCTION_ARGS);
extern Datum regclassout(MDB_FUNCTION_ARGS);
extern Datum regclassrecv(MDB_FUNCTION_ARGS);
extern Datum regclasssend(MDB_FUNCTION_ARGS);
extern Datum to_regclass(MDB_FUNCTION_ARGS);
extern Datum regtypein(MDB_FUNCTION_ARGS);
extern Datum regtypeout(MDB_FUNCTION_ARGS);
extern Datum regtyperecv(MDB_FUNCTION_ARGS);
extern Datum regtypesend(MDB_FUNCTION_ARGS);
extern Datum to_regtype(MDB_FUNCTION_ARGS);
extern Datum regrolein(MDB_FUNCTION_ARGS);
extern Datum regroleout(MDB_FUNCTION_ARGS);
extern Datum regrolerecv(MDB_FUNCTION_ARGS);
extern Datum regrolesend(MDB_FUNCTION_ARGS);
extern Datum to_regrole(MDB_FUNCTION_ARGS);
extern Datum regnamespacein(MDB_FUNCTION_ARGS);
extern Datum regnamespaceout(MDB_FUNCTION_ARGS);
extern Datum regnamespacerecv(MDB_FUNCTION_ARGS);
extern Datum regnamespacesend(MDB_FUNCTION_ARGS);
extern Datum to_regnamespace(MDB_FUNCTION_ARGS);
extern Datum regconfigin(MDB_FUNCTION_ARGS);
extern Datum regconfigout(MDB_FUNCTION_ARGS);
extern Datum regconfigrecv(MDB_FUNCTION_ARGS);
extern Datum regconfigsend(MDB_FUNCTION_ARGS);
extern Datum regdictionaryin(MDB_FUNCTION_ARGS);
extern Datum regdictionaryout(MDB_FUNCTION_ARGS);
extern Datum regdictionaryrecv(MDB_FUNCTION_ARGS);
extern Datum regdictionarysend(MDB_FUNCTION_ARGS);
extern Datum text_regclass(MDB_FUNCTION_ARGS);
extern List *stringToQualifiedNameList(const char *string);
extern char *format_procedure(Oid procedure_oid);
extern char *format_procedure_qualified(Oid procedure_oid);
extern void format_procedure_parts(Oid operator_oid, List **objnames,
					   List **objargs);
extern char *format_operator(Oid operator_oid);
extern char *format_operator_qualified(Oid operator_oid);
extern void format_operator_parts(Oid operator_oid, List **objnames,
					  List **objargs);

/* rowtypes.c */
extern Datum record_in(MDB_FUNCTION_ARGS);
extern Datum record_out(MDB_FUNCTION_ARGS);
extern Datum record_recv(MDB_FUNCTION_ARGS);
extern Datum record_send(MDB_FUNCTION_ARGS);
extern Datum record_eq(MDB_FUNCTION_ARGS);
extern Datum record_ne(MDB_FUNCTION_ARGS);
extern Datum record_lt(MDB_FUNCTION_ARGS);
extern Datum record_gt(MDB_FUNCTION_ARGS);
extern Datum record_le(MDB_FUNCTION_ARGS);
extern Datum record_ge(MDB_FUNCTION_ARGS);
extern Datum btrecordcmp(MDB_FUNCTION_ARGS);
extern Datum record_image_eq(MDB_FUNCTION_ARGS);
extern Datum record_image_ne(MDB_FUNCTION_ARGS);
extern Datum record_image_lt(MDB_FUNCTION_ARGS);
extern Datum record_image_gt(MDB_FUNCTION_ARGS);
extern Datum record_image_le(MDB_FUNCTION_ARGS);
extern Datum record_image_ge(MDB_FUNCTION_ARGS);
extern Datum btrecordimagecmp(MDB_FUNCTION_ARGS);

/* ruleutils.c */
extern bool quote_all_identifiers;
extern Datum mdb_get_ruledef(MDB_FUNCTION_ARGS);
extern Datum mdb_get_ruledef_ext(MDB_FUNCTION_ARGS);
extern Datum mdb_get_viewdef(MDB_FUNCTION_ARGS);
extern Datum mdb_get_viewdef_ext(MDB_FUNCTION_ARGS);
extern Datum mdb_get_viewdef_wrap(MDB_FUNCTION_ARGS);
extern Datum mdb_get_viewdef_name(MDB_FUNCTION_ARGS);
extern Datum mdb_get_viewdef_name_ext(MDB_FUNCTION_ARGS);
extern Datum mdb_get_indexdef(MDB_FUNCTION_ARGS);
extern Datum mdb_get_indexdef_ext(MDB_FUNCTION_ARGS);
extern Datum mdb_get_triggerdef(MDB_FUNCTION_ARGS);
extern Datum mdb_get_triggerdef_ext(MDB_FUNCTION_ARGS);
extern Datum mdb_get_constraintdef(MDB_FUNCTION_ARGS);
extern Datum mdb_get_constraintdef_ext(MDB_FUNCTION_ARGS);
extern Datum mdb_get_expr(MDB_FUNCTION_ARGS);
extern Datum mdb_get_expr_ext(MDB_FUNCTION_ARGS);
extern Datum mdb_get_userbyid(MDB_FUNCTION_ARGS);
extern Datum mdb_get_serial_sequence(MDB_FUNCTION_ARGS);
extern Datum mdb_get_functiondef(MDB_FUNCTION_ARGS);
extern Datum mdb_get_function_arguments(MDB_FUNCTION_ARGS);
extern Datum mdb_get_function_identity_arguments(MDB_FUNCTION_ARGS);
extern Datum mdb_get_function_result(MDB_FUNCTION_ARGS);
extern Datum mdb_get_function_arg_default(MDB_FUNCTION_ARGS);
extern const char *quote_identifier(const char *ident);
extern char *quote_qualified_identifier(const char *qualifier,
						   const char *ident);


/* tid.c */
extern Datum tidin(MDB_FUNCTION_ARGS);
extern Datum tidout(MDB_FUNCTION_ARGS);
extern Datum tidrecv(MDB_FUNCTION_ARGS);
extern Datum tidsend(MDB_FUNCTION_ARGS);
extern Datum tideq(MDB_FUNCTION_ARGS);
extern Datum tidne(MDB_FUNCTION_ARGS);
extern Datum tidlt(MDB_FUNCTION_ARGS);
extern Datum tidle(MDB_FUNCTION_ARGS);
extern Datum tidgt(MDB_FUNCTION_ARGS);
extern Datum tidge(MDB_FUNCTION_ARGS);
extern Datum bttidcmp(MDB_FUNCTION_ARGS);
extern Datum tidlarger(MDB_FUNCTION_ARGS);
extern Datum tidsmaller(MDB_FUNCTION_ARGS);
extern Datum currtid_byreloid(MDB_FUNCTION_ARGS);
extern Datum currtid_byrelname(MDB_FUNCTION_ARGS);

/* varchar.c */
extern Datum bpcharin(MDB_FUNCTION_ARGS);
extern Datum bpcharout(MDB_FUNCTION_ARGS);
extern Datum bpcharrecv(MDB_FUNCTION_ARGS);
extern Datum bpcharsend(MDB_FUNCTION_ARGS);
extern Datum bpchartypmodin(MDB_FUNCTION_ARGS);
extern Datum bpchartypmodout(MDB_FUNCTION_ARGS);
extern Datum bpchar(MDB_FUNCTION_ARGS);
extern Datum char_bpchar(MDB_FUNCTION_ARGS);
extern Datum name_bpchar(MDB_FUNCTION_ARGS);
extern Datum bpchar_name(MDB_FUNCTION_ARGS);
extern Datum bpchareq(MDB_FUNCTION_ARGS);
extern Datum bpcharne(MDB_FUNCTION_ARGS);
extern Datum bpcharlt(MDB_FUNCTION_ARGS);
extern Datum bpcharle(MDB_FUNCTION_ARGS);
extern Datum bpchargt(MDB_FUNCTION_ARGS);
extern Datum bpcharge(MDB_FUNCTION_ARGS);
extern Datum bpcharcmp(MDB_FUNCTION_ARGS);
extern Datum bpchar_sortsupport(MDB_FUNCTION_ARGS);
extern Datum bpchar_larger(MDB_FUNCTION_ARGS);
extern Datum bpchar_smaller(MDB_FUNCTION_ARGS);
extern int	bpchartruelen(char *s, int len);
extern Datum bpcharlen(MDB_FUNCTION_ARGS);
extern Datum bpcharoctetlen(MDB_FUNCTION_ARGS);
extern Datum hashbpchar(MDB_FUNCTION_ARGS);
extern Datum bpchar_pattern_lt(MDB_FUNCTION_ARGS);
extern Datum bpchar_pattern_le(MDB_FUNCTION_ARGS);
extern Datum bpchar_pattern_gt(MDB_FUNCTION_ARGS);
extern Datum bpchar_pattern_ge(MDB_FUNCTION_ARGS);
extern Datum btbpchar_pattern_cmp(MDB_FUNCTION_ARGS);
extern Datum btbpchar_pattern_sortsupport(MDB_FUNCTION_ARGS);

extern Datum varcharin(MDB_FUNCTION_ARGS);
extern Datum varcharout(MDB_FUNCTION_ARGS);
extern Datum varcharrecv(MDB_FUNCTION_ARGS);
extern Datum varcharsend(MDB_FUNCTION_ARGS);
extern Datum varchartypmodin(MDB_FUNCTION_ARGS);
extern Datum varchartypmodout(MDB_FUNCTION_ARGS);
extern Datum varchar_transform(MDB_FUNCTION_ARGS);
extern Datum varchar(MDB_FUNCTION_ARGS);

/* varlena.c */
extern text *cstring_to_text(const char *s);
extern text *cstring_to_text_with_len(const char *s, int len);
extern char *text_to_cstring(const text *t);
extern void text_to_cstring_buffer(const text *src, char *dst, size_t dst_len);

#define CStringGetTextDatum(s) PointerGetDatum(cstring_to_text(s))
#define TextDatumGetCString(d) text_to_cstring((text *) DatumGetPointer(d))

extern Datum textin(MDB_FUNCTION_ARGS);
extern Datum textout(MDB_FUNCTION_ARGS);
extern Datum textrecv(MDB_FUNCTION_ARGS);
extern Datum textsend(MDB_FUNCTION_ARGS);
extern Datum textcat(MDB_FUNCTION_ARGS);
extern Datum texteq(MDB_FUNCTION_ARGS);
extern Datum textne(MDB_FUNCTION_ARGS);
extern Datum text_lt(MDB_FUNCTION_ARGS);
extern Datum text_le(MDB_FUNCTION_ARGS);
extern Datum text_gt(MDB_FUNCTION_ARGS);
extern Datum text_ge(MDB_FUNCTION_ARGS);
extern Datum text_larger(MDB_FUNCTION_ARGS);
extern Datum text_smaller(MDB_FUNCTION_ARGS);
extern Datum text_pattern_lt(MDB_FUNCTION_ARGS);
extern Datum text_pattern_le(MDB_FUNCTION_ARGS);
extern Datum text_pattern_gt(MDB_FUNCTION_ARGS);
extern Datum text_pattern_ge(MDB_FUNCTION_ARGS);
extern Datum bttext_pattern_cmp(MDB_FUNCTION_ARGS);
extern Datum bttext_pattern_sortsupport(MDB_FUNCTION_ARGS);
extern Datum textlen(MDB_FUNCTION_ARGS);
extern Datum textoctetlen(MDB_FUNCTION_ARGS);
extern Datum textpos(MDB_FUNCTION_ARGS);
extern Datum text_substr(MDB_FUNCTION_ARGS);
extern Datum text_substr_no_len(MDB_FUNCTION_ARGS);
extern Datum textoverlay(MDB_FUNCTION_ARGS);
extern Datum textoverlay_no_len(MDB_FUNCTION_ARGS);
extern Datum name_text(MDB_FUNCTION_ARGS);
extern Datum text_name(MDB_FUNCTION_ARGS);
extern int	varstr_cmp(char *arg1, int len1, char *arg2, int len2, Oid collid);
extern void varstr_sortsupport(SortSupport ssup, Oid collid, bool bpchar);
extern int varstr_levenshtein(const char *source, int slen,
				   const char *target, int tlen,
				   int ins_c, int del_c, int sub_c,
				   bool trusted);
extern int varstr_levenshtein_less_equal(const char *source, int slen,
							  const char *target, int tlen,
							  int ins_c, int del_c, int sub_c,
							  int max_d, bool trusted);
extern List *textToQualifiedNameList(text *textval);
extern bool SplitIdentifierString(char *rawstring, char separator,
					  List **namelist);
extern bool SplitDirectoriesString(char *rawstring, char separator,
					   List **namelist);
extern Datum replace_text(MDB_FUNCTION_ARGS);
extern text *replace_text_regexp(text *src_text, void *regexp,
					text *replace_text, bool glob);
extern Datum split_text(MDB_FUNCTION_ARGS);
extern Datum text_to_array(MDB_FUNCTION_ARGS);
extern Datum array_to_text(MDB_FUNCTION_ARGS);
extern Datum text_to_array_null(MDB_FUNCTION_ARGS);
extern Datum array_to_text_null(MDB_FUNCTION_ARGS);
extern Datum to_hex32(MDB_FUNCTION_ARGS);
extern Datum to_hex64(MDB_FUNCTION_ARGS);
extern Datum md5_text(MDB_FUNCTION_ARGS);
extern Datum md5_bytea(MDB_FUNCTION_ARGS);

extern Datum unknownin(MDB_FUNCTION_ARGS);
extern Datum unknownout(MDB_FUNCTION_ARGS);
extern Datum unknownrecv(MDB_FUNCTION_ARGS);
extern Datum unknownsend(MDB_FUNCTION_ARGS);

extern Datum mdb_column_size(MDB_FUNCTION_ARGS);

extern Datum bytea_string_agg_transfn(MDB_FUNCTION_ARGS);
extern Datum bytea_string_agg_finalfn(MDB_FUNCTION_ARGS);
extern Datum string_agg_transfn(MDB_FUNCTION_ARGS);
extern Datum string_agg_finalfn(MDB_FUNCTION_ARGS);

extern Datum text_concat(MDB_FUNCTION_ARGS);
extern Datum text_concat_ws(MDB_FUNCTION_ARGS);
extern Datum text_left(MDB_FUNCTION_ARGS);
extern Datum text_right(MDB_FUNCTION_ARGS);
extern Datum text_reverse(MDB_FUNCTION_ARGS);
extern Datum text_format(MDB_FUNCTION_ARGS);
extern Datum text_format_nv(MDB_FUNCTION_ARGS);

/* version.c */
extern Datum mdb_version(MDB_FUNCTION_ARGS);

/* xid.c */
extern Datum xidin(MDB_FUNCTION_ARGS);
extern Datum xidout(MDB_FUNCTION_ARGS);
extern Datum xidrecv(MDB_FUNCTION_ARGS);
extern Datum xidsend(MDB_FUNCTION_ARGS);
extern Datum xideq(MDB_FUNCTION_ARGS);
extern Datum xidneq(MDB_FUNCTION_ARGS);
extern Datum xid_age(MDB_FUNCTION_ARGS);
extern Datum mxid_age(MDB_FUNCTION_ARGS);
extern int	xidComparator(const void *arg1, const void *arg2);
extern Datum cidin(MDB_FUNCTION_ARGS);
extern Datum cidout(MDB_FUNCTION_ARGS);
extern Datum cidrecv(MDB_FUNCTION_ARGS);
extern Datum cidsend(MDB_FUNCTION_ARGS);
extern Datum cideq(MDB_FUNCTION_ARGS);

/* like.c */
extern Datum namelike(MDB_FUNCTION_ARGS);
extern Datum namenlike(MDB_FUNCTION_ARGS);
extern Datum nameiclike(MDB_FUNCTION_ARGS);
extern Datum nameicnlike(MDB_FUNCTION_ARGS);
extern Datum textlike(MDB_FUNCTION_ARGS);
extern Datum textnlike(MDB_FUNCTION_ARGS);
extern Datum texticlike(MDB_FUNCTION_ARGS);
extern Datum texticnlike(MDB_FUNCTION_ARGS);
extern Datum bytealike(MDB_FUNCTION_ARGS);
extern Datum byteanlike(MDB_FUNCTION_ARGS);
extern Datum like_escape(MDB_FUNCTION_ARGS);
extern Datum like_escape_bytea(MDB_FUNCTION_ARGS);

/* oracle_compat.c */
extern Datum lower(MDB_FUNCTION_ARGS);
extern Datum upper(MDB_FUNCTION_ARGS);
extern Datum initcap(MDB_FUNCTION_ARGS);
extern Datum lpad(MDB_FUNCTION_ARGS);
extern Datum rpad(MDB_FUNCTION_ARGS);
extern Datum btrim(MDB_FUNCTION_ARGS);
extern Datum btrim1(MDB_FUNCTION_ARGS);
extern Datum byteatrim(MDB_FUNCTION_ARGS);
extern Datum ltrim(MDB_FUNCTION_ARGS);
extern Datum ltrim1(MDB_FUNCTION_ARGS);
extern Datum rtrim(MDB_FUNCTION_ARGS);
extern Datum rtrim1(MDB_FUNCTION_ARGS);
extern Datum translate(MDB_FUNCTION_ARGS);
extern Datum chr (MDB_FUNCTION_ARGS);
extern Datum repeat(MDB_FUNCTION_ARGS);
extern Datum ascii(MDB_FUNCTION_ARGS);

/* inet_cidr_ntop.c */
extern char *inet_cidr_ntop(int af, const void *src, int bits,
			   char *dst, size_t size);

/* inet_net_pton.c */
extern int inet_net_pton(int af, const char *src,
			  void *dst, size_t size);

/* network.c */
extern Datum inet_in(MDB_FUNCTION_ARGS);
extern Datum inet_out(MDB_FUNCTION_ARGS);
extern Datum inet_recv(MDB_FUNCTION_ARGS);
extern Datum inet_send(MDB_FUNCTION_ARGS);
extern Datum cidr_in(MDB_FUNCTION_ARGS);
extern Datum cidr_out(MDB_FUNCTION_ARGS);
extern Datum cidr_recv(MDB_FUNCTION_ARGS);
extern Datum cidr_send(MDB_FUNCTION_ARGS);
extern Datum network_cmp(MDB_FUNCTION_ARGS);
extern Datum network_lt(MDB_FUNCTION_ARGS);
extern Datum network_le(MDB_FUNCTION_ARGS);
extern Datum network_eq(MDB_FUNCTION_ARGS);
extern Datum network_ge(MDB_FUNCTION_ARGS);
extern Datum network_gt(MDB_FUNCTION_ARGS);
extern Datum network_ne(MDB_FUNCTION_ARGS);
extern Datum network_smaller(MDB_FUNCTION_ARGS);
extern Datum network_larger(MDB_FUNCTION_ARGS);
extern Datum hashinet(MDB_FUNCTION_ARGS);
extern Datum network_sub(MDB_FUNCTION_ARGS);
extern Datum network_subeq(MDB_FUNCTION_ARGS);
extern Datum network_sup(MDB_FUNCTION_ARGS);
extern Datum network_supeq(MDB_FUNCTION_ARGS);
extern Datum network_overlap(MDB_FUNCTION_ARGS);
extern Datum network_network(MDB_FUNCTION_ARGS);
extern Datum network_netmask(MDB_FUNCTION_ARGS);
extern Datum network_hostmask(MDB_FUNCTION_ARGS);
extern Datum network_masklen(MDB_FUNCTION_ARGS);
extern Datum network_family(MDB_FUNCTION_ARGS);
extern Datum network_broadcast(MDB_FUNCTION_ARGS);
extern Datum network_host(MDB_FUNCTION_ARGS);
extern Datum network_show(MDB_FUNCTION_ARGS);
extern Datum inet_abbrev(MDB_FUNCTION_ARGS);
extern Datum cidr_abbrev(MDB_FUNCTION_ARGS);
extern double convert_network_to_scalar(Datum value, Oid typid);
extern Datum inet_to_cidr(MDB_FUNCTION_ARGS);
extern Datum inet_set_masklen(MDB_FUNCTION_ARGS);
extern Datum cidr_set_masklen(MDB_FUNCTION_ARGS);
extern Datum network_scan_first(Datum in);
extern Datum network_scan_last(Datum in);
extern Datum inet_client_addr(MDB_FUNCTION_ARGS);
extern Datum inet_client_port(MDB_FUNCTION_ARGS);
extern Datum inet_server_addr(MDB_FUNCTION_ARGS);
extern Datum inet_server_port(MDB_FUNCTION_ARGS);
extern Datum inetnot(MDB_FUNCTION_ARGS);
extern Datum inetand(MDB_FUNCTION_ARGS);
extern Datum inetor(MDB_FUNCTION_ARGS);
extern Datum inetpl(MDB_FUNCTION_ARGS);
extern Datum inetmi_int8(MDB_FUNCTION_ARGS);
extern Datum inetmi(MDB_FUNCTION_ARGS);
extern void clean_ipv6_addr(int addr_family, char *addr);
extern Datum inet_same_family(MDB_FUNCTION_ARGS);
extern Datum inet_merge(MDB_FUNCTION_ARGS);

/* mac.c */
extern Datum macaddr_in(MDB_FUNCTION_ARGS);
extern Datum macaddr_out(MDB_FUNCTION_ARGS);
extern Datum macaddr_recv(MDB_FUNCTION_ARGS);
extern Datum macaddr_send(MDB_FUNCTION_ARGS);
extern Datum macaddr_cmp(MDB_FUNCTION_ARGS);
extern Datum macaddr_lt(MDB_FUNCTION_ARGS);
extern Datum macaddr_le(MDB_FUNCTION_ARGS);
extern Datum macaddr_eq(MDB_FUNCTION_ARGS);
extern Datum macaddr_ge(MDB_FUNCTION_ARGS);
extern Datum macaddr_gt(MDB_FUNCTION_ARGS);
extern Datum macaddr_ne(MDB_FUNCTION_ARGS);
extern Datum macaddr_not(MDB_FUNCTION_ARGS);
extern Datum macaddr_and(MDB_FUNCTION_ARGS);
extern Datum macaddr_or(MDB_FUNCTION_ARGS);
extern Datum macaddr_trunc(MDB_FUNCTION_ARGS);
extern Datum hashmacaddr(MDB_FUNCTION_ARGS);

/* numeric.c */
extern Datum numeric_in(MDB_FUNCTION_ARGS);
extern Datum numeric_out(MDB_FUNCTION_ARGS);
extern Datum numeric_recv(MDB_FUNCTION_ARGS);
extern Datum numeric_send(MDB_FUNCTION_ARGS);
extern Datum numerictypmodin(MDB_FUNCTION_ARGS);
extern Datum numerictypmodout(MDB_FUNCTION_ARGS);
extern Datum numeric_transform(MDB_FUNCTION_ARGS);
extern Datum numeric (MDB_FUNCTION_ARGS);
extern Datum numeric_abs(MDB_FUNCTION_ARGS);
extern Datum numeric_uminus(MDB_FUNCTION_ARGS);
extern Datum numeric_uplus(MDB_FUNCTION_ARGS);
extern Datum numeric_sign(MDB_FUNCTION_ARGS);
extern Datum numeric_round(MDB_FUNCTION_ARGS);
extern Datum numeric_trunc(MDB_FUNCTION_ARGS);
extern Datum numeric_ceil(MDB_FUNCTION_ARGS);
extern Datum numeric_floor(MDB_FUNCTION_ARGS);
extern Datum numeric_sortsupport(MDB_FUNCTION_ARGS);
extern Datum numeric_cmp(MDB_FUNCTION_ARGS);
extern Datum numeric_eq(MDB_FUNCTION_ARGS);
extern Datum numeric_ne(MDB_FUNCTION_ARGS);
extern Datum numeric_gt(MDB_FUNCTION_ARGS);
extern Datum numeric_ge(MDB_FUNCTION_ARGS);
extern Datum numeric_lt(MDB_FUNCTION_ARGS);
extern Datum numeric_le(MDB_FUNCTION_ARGS);
extern Datum numeric_add(MDB_FUNCTION_ARGS);
extern Datum numeric_sub(MDB_FUNCTION_ARGS);
extern Datum numeric_mul(MDB_FUNCTION_ARGS);
extern Datum numeric_div(MDB_FUNCTION_ARGS);
extern Datum numeric_div_trunc(MDB_FUNCTION_ARGS);
extern Datum numeric_mod(MDB_FUNCTION_ARGS);
extern Datum numeric_inc(MDB_FUNCTION_ARGS);
extern Datum numeric_smaller(MDB_FUNCTION_ARGS);
extern Datum numeric_larger(MDB_FUNCTION_ARGS);
extern Datum numeric_fac(MDB_FUNCTION_ARGS);
extern Datum numeric_sqrt(MDB_FUNCTION_ARGS);
extern Datum numeric_exp(MDB_FUNCTION_ARGS);
extern Datum numeric_ln(MDB_FUNCTION_ARGS);
extern Datum numeric_log(MDB_FUNCTION_ARGS);
extern Datum numeric_power(MDB_FUNCTION_ARGS);
extern Datum numeric_scale(MDB_FUNCTION_ARGS);
extern Datum int4_numeric(MDB_FUNCTION_ARGS);
extern Datum numeric_int4(MDB_FUNCTION_ARGS);
extern Datum int8_numeric(MDB_FUNCTION_ARGS);
extern Datum numeric_int8(MDB_FUNCTION_ARGS);
extern Datum int2_numeric(MDB_FUNCTION_ARGS);
extern Datum numeric_int2(MDB_FUNCTION_ARGS);
extern Datum float8_numeric(MDB_FUNCTION_ARGS);
extern Datum numeric_float8(MDB_FUNCTION_ARGS);
extern Datum numeric_float8_no_overflow(MDB_FUNCTION_ARGS);
extern Datum float4_numeric(MDB_FUNCTION_ARGS);
extern Datum numeric_float4(MDB_FUNCTION_ARGS);
extern Datum numeric_accum(MDB_FUNCTION_ARGS);
extern Datum numeric_combine(MDB_FUNCTION_ARGS);
extern Datum numeric_avg_accum(MDB_FUNCTION_ARGS);
extern Datum numeric_avg_combine(MDB_FUNCTION_ARGS);
extern Datum numeric_avg_serialize(MDB_FUNCTION_ARGS);
extern Datum numeric_avg_deserialize(MDB_FUNCTION_ARGS);
extern Datum numeric_serialize(MDB_FUNCTION_ARGS);
extern Datum numeric_deserialize(MDB_FUNCTION_ARGS);
extern Datum numeric_accum_inv(MDB_FUNCTION_ARGS);
extern Datum int2_accum(MDB_FUNCTION_ARGS);
extern Datum int4_accum(MDB_FUNCTION_ARGS);
extern Datum int8_accum(MDB_FUNCTION_ARGS);
extern Datum numeric_poly_combine(MDB_FUNCTION_ARGS);
extern Datum numeric_poly_serialize(MDB_FUNCTION_ARGS);
extern Datum numeric_poly_deserialize(MDB_FUNCTION_ARGS);
extern Datum int2_accum_inv(MDB_FUNCTION_ARGS);
extern Datum int4_accum_inv(MDB_FUNCTION_ARGS);
extern Datum int8_accum_inv(MDB_FUNCTION_ARGS);
extern Datum int8_avg_accum(MDB_FUNCTION_ARGS);
extern Datum int8_avg_combine(MDB_FUNCTION_ARGS);
extern Datum int8_avg_serialize(MDB_FUNCTION_ARGS);
extern Datum int8_avg_deserialize(MDB_FUNCTION_ARGS);
extern Datum numeric_avg(MDB_FUNCTION_ARGS);
extern Datum numeric_sum(MDB_FUNCTION_ARGS);
extern Datum numeric_var_pop(MDB_FUNCTION_ARGS);
extern Datum numeric_var_samp(MDB_FUNCTION_ARGS);
extern Datum numeric_stddev_pop(MDB_FUNCTION_ARGS);
extern Datum numeric_stddev_samp(MDB_FUNCTION_ARGS);
extern Datum numeric_poly_sum(MDB_FUNCTION_ARGS);
extern Datum numeric_poly_avg(MDB_FUNCTION_ARGS);
extern Datum numeric_poly_var_pop(MDB_FUNCTION_ARGS);
extern Datum numeric_poly_var_samp(MDB_FUNCTION_ARGS);
extern Datum numeric_poly_stddev_pop(MDB_FUNCTION_ARGS);
extern Datum numeric_poly_stddev_samp(MDB_FUNCTION_ARGS);
extern Datum int2_sum(MDB_FUNCTION_ARGS);
extern Datum int4_sum(MDB_FUNCTION_ARGS);
extern Datum int8_sum(MDB_FUNCTION_ARGS);
extern Datum int2_avg_accum(MDB_FUNCTION_ARGS);
extern Datum int4_avg_accum(MDB_FUNCTION_ARGS);
extern Datum int4_avg_combine(MDB_FUNCTION_ARGS);
extern Datum int2_avg_accum_inv(MDB_FUNCTION_ARGS);
extern Datum int4_avg_accum_inv(MDB_FUNCTION_ARGS);
extern Datum int8_avg_accum_inv(MDB_FUNCTION_ARGS);
extern Datum int8_avg(MDB_FUNCTION_ARGS);
extern Datum int2int4_sum(MDB_FUNCTION_ARGS);
extern Datum width_bucket_numeric(MDB_FUNCTION_ARGS);
extern Datum hash_numeric(MDB_FUNCTION_ARGS);
extern Datum generate_series_numeric(MDB_FUNCTION_ARGS);
extern Datum generate_series_step_numeric(MDB_FUNCTION_ARGS);

/* ri_triggers.c */
extern Datum RI_FKey_check_ins(MDB_FUNCTION_ARGS);
extern Datum RI_FKey_check_upd(MDB_FUNCTION_ARGS);
extern Datum RI_FKey_noaction_del(MDB_FUNCTION_ARGS);
extern Datum RI_FKey_noaction_upd(MDB_FUNCTION_ARGS);
extern Datum RI_FKey_cascade_del(MDB_FUNCTION_ARGS);
extern Datum RI_FKey_cascade_upd(MDB_FUNCTION_ARGS);
extern Datum RI_FKey_restrict_del(MDB_FUNCTION_ARGS);
extern Datum RI_FKey_restrict_upd(MDB_FUNCTION_ARGS);
extern Datum RI_FKey_setnull_del(MDB_FUNCTION_ARGS);
extern Datum RI_FKey_setnull_upd(MDB_FUNCTION_ARGS);
extern Datum RI_FKey_setdefault_del(MDB_FUNCTION_ARGS);
extern Datum RI_FKey_setdefault_upd(MDB_FUNCTION_ARGS);

/* trigfuncs.c */
extern Datum suppress_redundant_updates_trigger(MDB_FUNCTION_ARGS);

/* encoding support functions */
extern Datum getdatabaseencoding(MDB_FUNCTION_ARGS);
extern Datum database_character_set(MDB_FUNCTION_ARGS);
extern Datum mdb_client_encoding(MDB_FUNCTION_ARGS);
extern Datum MDB_encoding_to_char(MDB_FUNCTION_ARGS);
extern Datum MDB_char_to_encoding(MDB_FUNCTION_ARGS);
extern Datum MDB_character_set_name(MDB_FUNCTION_ARGS);
extern Datum MDB_character_set_id(MDB_FUNCTION_ARGS);
extern Datum mdb_convert(MDB_FUNCTION_ARGS);
extern Datum mdb_convert_to(MDB_FUNCTION_ARGS);
extern Datum mdb_convert_from(MDB_FUNCTION_ARGS);
extern Datum length_in_encoding(MDB_FUNCTION_ARGS);
extern Datum mdb_encoding_max_length_sql(MDB_FUNCTION_ARGS);

/* format_type.c */
extern Datum format_type(MDB_FUNCTION_ARGS);
extern char *format_type_be(Oid type_oid);
extern char *format_type_be_qualified(Oid type_oid);
extern char *format_type_with_typemod(Oid type_oid, int32 typemod);
extern char *format_type_with_typemod_qualified(Oid type_oid, int32 typemod);
extern Datum oidvectortypes(MDB_FUNCTION_ARGS);
extern int32 type_maximum_size(Oid type_oid, int32 typemod);

/* quote.c */
extern Datum quote_ident(MDB_FUNCTION_ARGS);
extern Datum quote_literal(MDB_FUNCTION_ARGS);
extern char *quote_literal_cstr(const char *rawstr);
extern Datum quote_nullable(MDB_FUNCTION_ARGS);

/* guc.c */
extern Datum show_config_by_name(MDB_FUNCTION_ARGS);
extern Datum show_config_by_name_missing_ok(MDB_FUNCTION_ARGS);
extern Datum set_config_by_name(MDB_FUNCTION_ARGS);
extern Datum show_all_settings(MDB_FUNCTION_ARGS);
extern Datum show_all_file_settings(MDB_FUNCTION_ARGS);

/* mdb_config.c */
extern Datum mdb_config(MDB_FUNCTION_ARGS);

/* mdb_controldata.c */
extern Datum mdb_control_checkpoint(MDB_FUNCTION_ARGS);
extern Datum mdb_control_system(MDB_FUNCTION_ARGS);
extern Datum mdb_control_init(MDB_FUNCTION_ARGS);
extern Datum mdb_control_recovery(MDB_FUNCTION_ARGS);

/* rls.c */
extern Datum row_security_active(MDB_FUNCTION_ARGS);
extern Datum row_security_active_name(MDB_FUNCTION_ARGS);

/* lockfuncs.c */
extern Datum mdb_lock_status(MDB_FUNCTION_ARGS);
extern Datum mdb_blocking_pids(MDB_FUNCTION_ARGS);
extern Datum mdb_advisory_lock_int8(MDB_FUNCTION_ARGS);
extern Datum mdb_advisory_xact_lock_int8(MDB_FUNCTION_ARGS);
extern Datum mdb_advisory_lock_shared_int8(MDB_FUNCTION_ARGS);
extern Datum mdb_advisory_xact_lock_shared_int8(MDB_FUNCTION_ARGS);
extern Datum mdb_try_advisory_lock_int8(MDB_FUNCTION_ARGS);
extern Datum mdb_try_advisory_xact_lock_int8(MDB_FUNCTION_ARGS);
extern Datum mdb_try_advisory_lock_shared_int8(MDB_FUNCTION_ARGS);
extern Datum mdb_try_advisory_xact_lock_shared_int8(MDB_FUNCTION_ARGS);
extern Datum mdb_advisory_unlock_int8(MDB_FUNCTION_ARGS);
extern Datum mdb_advisory_unlock_shared_int8(MDB_FUNCTION_ARGS);
extern Datum mdb_advisory_lock_int4(MDB_FUNCTION_ARGS);
extern Datum mdb_advisory_xact_lock_int4(MDB_FUNCTION_ARGS);
extern Datum mdb_advisory_lock_shared_int4(MDB_FUNCTION_ARGS);
extern Datum mdb_advisory_xact_lock_shared_int4(MDB_FUNCTION_ARGS);
extern Datum mdb_try_advisory_lock_int4(MDB_FUNCTION_ARGS);
extern Datum mdb_try_advisory_xact_lock_int4(MDB_FUNCTION_ARGS);
extern Datum mdb_try_advisory_lock_shared_int4(MDB_FUNCTION_ARGS);
extern Datum mdb_try_advisory_xact_lock_shared_int4(MDB_FUNCTION_ARGS);
extern Datum mdb_advisory_unlock_int4(MDB_FUNCTION_ARGS);
extern Datum mdb_advisory_unlock_shared_int4(MDB_FUNCTION_ARGS);
extern Datum mdb_advisory_unlock_all(MDB_FUNCTION_ARGS);

/* txid.c */
extern Datum txid_snapshot_in(MDB_FUNCTION_ARGS);
extern Datum txid_snapshot_out(MDB_FUNCTION_ARGS);
extern Datum txid_snapshot_recv(MDB_FUNCTION_ARGS);
extern Datum txid_snapshot_send(MDB_FUNCTION_ARGS);
extern Datum txid_current(MDB_FUNCTION_ARGS);
extern Datum txid_current_snapshot(MDB_FUNCTION_ARGS);
extern Datum txid_snapshot_xmin(MDB_FUNCTION_ARGS);
extern Datum txid_snapshot_xmax(MDB_FUNCTION_ARGS);
extern Datum txid_snapshot_xip(MDB_FUNCTION_ARGS);
extern Datum txid_visible_in_snapshot(MDB_FUNCTION_ARGS);

/* uuid.c */
extern Datum uuid_in(MDB_FUNCTION_ARGS);
extern Datum uuid_out(MDB_FUNCTION_ARGS);
extern Datum uuid_send(MDB_FUNCTION_ARGS);
extern Datum uuid_recv(MDB_FUNCTION_ARGS);
extern Datum uuid_lt(MDB_FUNCTION_ARGS);
extern Datum uuid_le(MDB_FUNCTION_ARGS);
extern Datum uuid_eq(MDB_FUNCTION_ARGS);
extern Datum uuid_ge(MDB_FUNCTION_ARGS);
extern Datum uuid_gt(MDB_FUNCTION_ARGS);
extern Datum uuid_ne(MDB_FUNCTION_ARGS);
extern Datum uuid_cmp(MDB_FUNCTION_ARGS);
extern Datum uuid_sortsupport(MDB_FUNCTION_ARGS);
extern Datum uuid_hash(MDB_FUNCTION_ARGS);

/* windowfuncs.c */
extern Datum window_row_number(MDB_FUNCTION_ARGS);
extern Datum window_rank(MDB_FUNCTION_ARGS);
extern Datum window_dense_rank(MDB_FUNCTION_ARGS);
extern Datum window_percent_rank(MDB_FUNCTION_ARGS);
extern Datum window_cume_dist(MDB_FUNCTION_ARGS);
extern Datum window_ntile(MDB_FUNCTION_ARGS);
extern Datum window_lag(MDB_FUNCTION_ARGS);
extern Datum window_lag_with_offset(MDB_FUNCTION_ARGS);
extern Datum window_lag_with_offset_and_default(MDB_FUNCTION_ARGS);
extern Datum window_lead(MDB_FUNCTION_ARGS);
extern Datum window_lead_with_offset(MDB_FUNCTION_ARGS);
extern Datum window_lead_with_offset_and_default(MDB_FUNCTION_ARGS);
extern Datum window_first_value(MDB_FUNCTION_ARGS);
extern Datum window_last_value(MDB_FUNCTION_ARGS);
extern Datum window_nth_value(MDB_FUNCTION_ARGS);

/* access/spgist/spgquadtreeproc.c */
extern Datum smdb_quad_config(MDB_FUNCTION_ARGS);
extern Datum smdb_quad_choose(MDB_FUNCTION_ARGS);
extern Datum smdb_quad_picksplit(MDB_FUNCTION_ARGS);
extern Datum smdb_quad_inner_consistent(MDB_FUNCTION_ARGS);
extern Datum smdb_quad_leaf_consistent(MDB_FUNCTION_ARGS);

/* access/spgist/spgkdtreeproc.c */
extern Datum smdb_kd_config(MDB_FUNCTION_ARGS);
extern Datum smdb_kd_choose(MDB_FUNCTION_ARGS);
extern Datum smdb_kd_picksplit(MDB_FUNCTION_ARGS);
extern Datum smdb_kd_inner_consistent(MDB_FUNCTION_ARGS);

/* access/spgist/spgtextproc.c */
extern Datum smdb_text_config(MDB_FUNCTION_ARGS);
extern Datum smdb_text_choose(MDB_FUNCTION_ARGS);
extern Datum smdb_text_picksplit(MDB_FUNCTION_ARGS);
extern Datum smdb_text_inner_consistent(MDB_FUNCTION_ARGS);
extern Datum smdb_text_leaf_consistent(MDB_FUNCTION_ARGS);

/* access/gin/ginarrayproc.c */
extern Datum ginarrayextract(MDB_FUNCTION_ARGS);
extern Datum ginarrayextract_2args(MDB_FUNCTION_ARGS);
extern Datum ginqueryarrayextract(MDB_FUNCTION_ARGS);
extern Datum ginarrayconsistent(MDB_FUNCTION_ARGS);
extern Datum ginarraytriconsistent(MDB_FUNCTION_ARGS);

/* access/tablesample/bernoulli.c */
extern Datum tsm_bernoulli_handler(MDB_FUNCTION_ARGS);

/* access/tablesample/system.c */
extern Datum tsm_system_handler(MDB_FUNCTION_ARGS);

/* access/transam/twophase.c */
extern Datum mdb_prepared_xact(MDB_FUNCTION_ARGS);

/* access/transam/multixact.c */
extern Datum mdb_get_multixact_members(MDB_FUNCTION_ARGS);

/* access/transam/committs.c */
extern Datum mdb_xact_commit_timestamp(MDB_FUNCTION_ARGS);
extern Datum mdb_last_committed_xact(MDB_FUNCTION_ARGS);

/* catalogs/dependency.c */
extern Datum mdb_describe_object(MDB_FUNCTION_ARGS);
extern Datum mdb_identify_object(MDB_FUNCTION_ARGS);
extern Datum mdb_identify_object_as_address(MDB_FUNCTION_ARGS);

/* catalog/objectaddress.c */
extern Datum mdb_get_object_address(MDB_FUNCTION_ARGS);

/* commands/constraint.c */
extern Datum unique_key_recheck(MDB_FUNCTION_ARGS);

/* commands/event_trigger.c */
extern Datum mdb_event_trigger_dropped_objects(MDB_FUNCTION_ARGS);
extern Datum mdb_event_trigger_table_rewrite_oid(MDB_FUNCTION_ARGS);
extern Datum mdb_event_trigger_table_rewrite_reason(MDB_FUNCTION_ARGS);
extern Datum mdb_event_trigger_ddl_commands(MDB_FUNCTION_ARGS);

/* commands/extension.c */
extern Datum mdb_available_extensions(MDB_FUNCTION_ARGS);
extern Datum mdb_available_extension_versions(MDB_FUNCTION_ARGS);
extern Datum mdb_extension_update_paths(MDB_FUNCTION_ARGS);
extern Datum mdb_extension_config_dump(MDB_FUNCTION_ARGS);

/* commands/prepare.c */
extern Datum mdb_prepared_statement(MDB_FUNCTION_ARGS);

/* utils/mmgr/portalmem.c */
extern Datum mdb_cursor(MDB_FUNCTION_ARGS);

#endif   /* BUILTINS_H */
