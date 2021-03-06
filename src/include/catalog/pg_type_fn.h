/*-------------------------------------------------------------------------
 *
 * mdb_type_fn.h
 *	 prototypes for functions in catalog/mdb_type.c
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/mdb_type_fn.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef MDB_TYPE_FN_H
#define MDB_TYPE_FN_H

#include "catalog/objectaddress.h"
#include "nodes/nodes.h"


extern ObjectAddress TypeShellMake(const char *typeName,
			  Oid typeNamespace,
			  Oid ownerId);

extern ObjectAddress TypeCreate(Oid newTypeOid,
		   const char *typeName,
		   Oid typeNamespace,
		   Oid relationOid,
		   char relationKind,
		   Oid ownerId,
		   int16 internalSize,
		   char typeType,
		   char typeCategory,
		   bool typePreferred,
		   char typDelim,
		   Oid inputProcedure,
		   Oid outputProcedure,
		   Oid receiveProcedure,
		   Oid sendProcedure,
		   Oid typmodinProcedure,
		   Oid typmodoutProcedure,
		   Oid analyzeProcedure,
		   Oid elementType,
		   bool isImplicitArray,
		   Oid arrayType,
		   Oid baseType,
		   const char *defaultTypeValue,
		   char *defaultTypeBin,
		   bool passedByValue,
		   char alignment,
		   char storage,
		   int32 typeMod,
		   int32 typNDims,
		   bool typeNotNull,
		   Oid typeCollation);

extern void GenerateTypeDependencies(Oid typeNamespace,
						 Oid typeObjectId,
						 Oid relationOid,
						 char relationKind,
						 Oid owner,
						 Oid inputProcedure,
						 Oid outputProcedure,
						 Oid receiveProcedure,
						 Oid sendProcedure,
						 Oid typmodinProcedure,
						 Oid typmodoutProcedure,
						 Oid analyzeProcedure,
						 Oid elementType,
						 bool isImplicitArray,
						 Oid baseType,
						 Oid typeCollation,
						 Node *defaultExpr,
						 bool rebuild);

extern void RenameTypeInternal(Oid typeOid, const char *newTypeName,
				   Oid typeNamespace);

extern char *makeArrayTypeName(const char *typeName, Oid typeNamespace);

extern bool moveArrayTypeName(Oid typeOid, const char *typeName,
				  Oid typeNamespace);

#endif   /* MDB_TYPE_FN_H */
