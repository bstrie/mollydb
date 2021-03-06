/*-------------------------------------------------------------------------
 *
 * mdb_enum.h
 *	  definition of the system "enum" relation (mdb_enum)
 *	  along with the relation's initial contents.
 *
 *
 * Copyright (c) 2006-2016, MollyDB Global Development Group
 *
 * src/include/catalog/mdb_enum.h
 *
 * NOTES
 *	  the genbki.pl script reads this file and generates .bki
 *	  information from the DATA() statements.
 *
 *	  XXX do NOT break up DATA() statements into multiple lines!
 *		  the scripts are not as smart as you might think...
 *
 *-------------------------------------------------------------------------
 */
#ifndef MDB_ENUM_H
#define MDB_ENUM_H

#include "catalog/genbki.h"
#include "nodes/mdb_list.h"

/* ----------------
 *		mdb_enum definition.  cpp turns this into
 *		typedef struct FormData_mdb_enum
 * ----------------
 */
#define EnumRelationId	3501

CATALOG(mdb_enum,3501)
{
	Oid			enumtypid;		/* OID of owning enum type */
	float4		enumsortorder;	/* sort position of this enum value */
	NameData	enumlabel;		/* text representation of enum value */
} FormData_mdb_enum;

/* ----------------
 *		Form_mdb_enum corresponds to a pointer to a tuple with
 *		the format of mdb_enum relation.
 * ----------------
 */
typedef FormData_mdb_enum *Form_mdb_enum;

/* ----------------
 *		compiler constants for mdb_enum
 * ----------------
 */
#define Natts_mdb_enum					3
#define Anum_mdb_enum_enumtypid			1
#define Anum_mdb_enum_enumsortorder		2
#define Anum_mdb_enum_enumlabel			3

/* ----------------
 *		mdb_enum has no initial contents
 * ----------------
 */

/*
 * prototypes for functions in mdb_enum.c
 */
extern void EnumValuesCreate(Oid enumTypeOid, List *vals);
extern void EnumValuesDelete(Oid enumTypeOid);
extern void AddEnumLabel(Oid enumTypeOid, const char *newVal,
			 const char *neighbor, bool newValIsAfter,
			 bool skipIfExists);

#endif   /* MDB_ENUM_H */
