/*-------------------------------------------------------------------------
 *
 * mdb_operator_fn.h
*	 prototypes for functions in catalog/mdb_operator.c
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/mdb_operator_fn.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef MDB_OPERATOR_FN_H
#define MDB_OPERATOR_FN_H

#include "catalog/objectaddress.h"
#include "nodes/mdb_list.h"

extern ObjectAddress OperatorCreate(const char *operatorName,
			   Oid operatorNamespace,
			   Oid leftTypeId,
			   Oid rightTypeId,
			   Oid procedureId,
			   List *commutatorName,
			   List *negatorName,
			   Oid restrictionId,
			   Oid joinId,
			   bool canMerge,
			   bool canHash);

extern ObjectAddress makeOperatorDependencies(HeapTuple tuple, bool isUpdate);

extern void OperatorUpd(Oid baseId, Oid commId, Oid negId, bool isDelete);

#endif   /* MDB_OPERATOR_FN_H */
