/*-------------------------------------------------------------------------
 *
 * mdb_collation.h
 *	  definition of the system "collation" relation (mdb_collation)
 *	  along with the relation's initial contents.
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *		src/include/catalog/mdb_collation.h
 *
 * NOTES
 *	  the genbki.pl script reads this file and generates .bki
 *	  information from the DATA() statements.
 *
 *-------------------------------------------------------------------------
 */
#ifndef MDB_COLLATION_H
#define MDB_COLLATION_H

#include "catalog/genbki.h"

/* ----------------
 *		mdb_collation definition.  cpp turns this into
 *		typedef struct FormData_mdb_collation
 * ----------------
 */
#define CollationRelationId  3456

CATALOG(mdb_collation,3456)
{
	NameData	collname;		/* collation name */
	Oid			collnamespace;	/* OID of namespace containing collation */
	Oid			collowner;		/* owner of collation */
	int32		collencoding;	/* encoding for this collation; -1 = "all" */
	NameData	collcollate;	/* LC_COLLATE setting */
	NameData	collctype;		/* LC_CTYPE setting */
} FormData_mdb_collation;

/* ----------------
 *		Form_mdb_collation corresponds to a pointer to a row with
 *		the format of mdb_collation relation.
 * ----------------
 */
typedef FormData_mdb_collation *Form_mdb_collation;

/* ----------------
 *		compiler constants for mdb_collation
 * ----------------
 */
#define Natts_mdb_collation				6
#define Anum_mdb_collation_collname		1
#define Anum_mdb_collation_collnamespace 2
#define Anum_mdb_collation_collowner		3
#define Anum_mdb_collation_collencoding	4
#define Anum_mdb_collation_collcollate	5
#define Anum_mdb_collation_collctype		6

/* ----------------
 *		initial contents of mdb_collation
 * ----------------
 */

DATA(insert OID = 100 ( default		PGNSP PGUID -1 "" "" ));
DESCR("database's default collation");
#define DEFAULT_COLLATION_OID	100
DATA(insert OID = 950 ( C			PGNSP PGUID -1 "C" "C" ));
DESCR("standard C collation");
#define C_COLLATION_OID			950
DATA(insert OID = 951 ( POSIX		PGNSP PGUID -1 "POSIX" "POSIX" ));
DESCR("standard POSIX collation");
#define POSIX_COLLATION_OID		951

#endif   /* MDB_COLLATION_H */
