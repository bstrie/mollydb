/*-------------------------------------------------------------------------
 *
 * json.h
 *	  Declarations for JSON data type support.
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/json.h
 *
 *-------------------------------------------------------------------------
 */

#ifndef JSON_H
#define JSON_H

#include "fmgr.h"
#include "lib/stringinfo.h"

/* functions in json.c */
extern Datum json_in(MDB_FUNCTION_ARGS);
extern Datum json_out(MDB_FUNCTION_ARGS);
extern Datum json_recv(MDB_FUNCTION_ARGS);
extern Datum json_send(MDB_FUNCTION_ARGS);
extern Datum array_to_json(MDB_FUNCTION_ARGS);
extern Datum array_to_json_pretty(MDB_FUNCTION_ARGS);
extern Datum row_to_json(MDB_FUNCTION_ARGS);
extern Datum row_to_json_pretty(MDB_FUNCTION_ARGS);
extern Datum to_json(MDB_FUNCTION_ARGS);

extern Datum json_agg_transfn(MDB_FUNCTION_ARGS);
extern Datum json_agg_finalfn(MDB_FUNCTION_ARGS);

extern Datum json_object_agg_finalfn(MDB_FUNCTION_ARGS);
extern Datum json_object_agg_transfn(MDB_FUNCTION_ARGS);

extern Datum json_build_object(MDB_FUNCTION_ARGS);
extern Datum json_build_object_noargs(MDB_FUNCTION_ARGS);
extern Datum json_build_array(MDB_FUNCTION_ARGS);
extern Datum json_build_array_noargs(MDB_FUNCTION_ARGS);

extern Datum json_object(MDB_FUNCTION_ARGS);
extern Datum json_object_two_arg(MDB_FUNCTION_ARGS);

extern void escape_json(StringInfo buf, const char *str);

extern Datum json_typeof(MDB_FUNCTION_ARGS);

/* functions in jsonfuncs.c */
extern Datum json_object_field(MDB_FUNCTION_ARGS);
extern Datum json_object_field_text(MDB_FUNCTION_ARGS);
extern Datum json_array_element(MDB_FUNCTION_ARGS);
extern Datum json_array_element_text(MDB_FUNCTION_ARGS);
extern Datum json_extract_path(MDB_FUNCTION_ARGS);
extern Datum json_extract_path_text(MDB_FUNCTION_ARGS);
extern Datum json_object_keys(MDB_FUNCTION_ARGS);
extern Datum json_array_length(MDB_FUNCTION_ARGS);
extern Datum json_each(MDB_FUNCTION_ARGS);
extern Datum json_each_text(MDB_FUNCTION_ARGS);
extern Datum json_array_elements(MDB_FUNCTION_ARGS);
extern Datum json_array_elements_text(MDB_FUNCTION_ARGS);
extern Datum json_populate_record(MDB_FUNCTION_ARGS);
extern Datum json_populate_recordset(MDB_FUNCTION_ARGS);
extern Datum json_to_record(MDB_FUNCTION_ARGS);
extern Datum json_to_recordset(MDB_FUNCTION_ARGS);
extern Datum json_strip_nulls(MDB_FUNCTION_ARGS);

extern Datum jsonb_object_field(MDB_FUNCTION_ARGS);
extern Datum jsonb_object_field_text(MDB_FUNCTION_ARGS);
extern Datum jsonb_array_element(MDB_FUNCTION_ARGS);
extern Datum jsonb_array_element_text(MDB_FUNCTION_ARGS);
extern Datum jsonb_extract_path(MDB_FUNCTION_ARGS);
extern Datum jsonb_extract_path_text(MDB_FUNCTION_ARGS);
extern Datum jsonb_object_keys(MDB_FUNCTION_ARGS);
extern Datum jsonb_array_length(MDB_FUNCTION_ARGS);
extern Datum jsonb_each(MDB_FUNCTION_ARGS);
extern Datum jsonb_each_text(MDB_FUNCTION_ARGS);
extern Datum jsonb_array_elements_text(MDB_FUNCTION_ARGS);
extern Datum jsonb_array_elements(MDB_FUNCTION_ARGS);
extern Datum jsonb_populate_record(MDB_FUNCTION_ARGS);
extern Datum jsonb_populate_recordset(MDB_FUNCTION_ARGS);
extern Datum jsonb_to_record(MDB_FUNCTION_ARGS);
extern Datum jsonb_to_recordset(MDB_FUNCTION_ARGS);
extern Datum jsonb_strip_nulls(MDB_FUNCTION_ARGS);

#endif   /* JSON_H */
