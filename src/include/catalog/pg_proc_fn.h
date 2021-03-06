/*-------------------------------------------------------------------------
 *
 * mdb_proc_fn.h
 *	 prototypes for functions in catalog/mdb_proc.c
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/mdb_proc_fn.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef MDB_PROC_FN_H
#define MDB_PROC_FN_H

#include "catalog/objectaddress.h"
#include "nodes/mdb_list.h"

extern ObjectAddress ProcedureCreate(const char *procedureName,
				Oid procNamespace,
				bool replace,
				bool returnsSet,
				Oid returnType,
				Oid proowner,
				Oid languageObjectId,
				Oid languageValidator,
				const char *prosrc,
				const char *probin,
				bool isAgg,
				bool isWindowFunc,
				bool security_definer,
				bool isLeakProof,
				bool isStrict,
				char volatility,
				char parallel,
				oidvector *parameterTypes,
				Datum allParameterTypes,
				Datum parameterModes,
				Datum parameterNames,
				List *parameterDefaults,
				Datum trftypes,
				Datum proconfig,
				float4 procost,
				float4 prorows);

extern bool function_parse_error_transpose(const char *prosrc);

extern List *oid_array_to_list(Datum datum);

#endif   /* MDB_PROC_FN_H */
