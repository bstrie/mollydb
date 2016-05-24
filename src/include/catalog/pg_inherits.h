/*-------------------------------------------------------------------------
 *
 * mdb_inherits.h
 *	  definition of the system "inherits" relation (mdb_inherits)
 *	  along with the relation's initial contents.
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/mdb_inherits.h
 *
 * NOTES
 *	  the genbki.pl script reads this file and generates .bki
 *	  information from the DATA() statements.
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_INHERITS_H
#define PG_INHERITS_H

#include "catalog/genbki.h"

/* ----------------
 *		mdb_inherits definition.  cpp turns this into
 *		typedef struct FormData_mdb_inherits
 * ----------------
 */
#define InheritsRelationId	2611

CATALOG(mdb_inherits,2611) BKI_WITHOUT_OIDS
{
	Oid			inhrelid;
	Oid			inhparent;
	int32		inhseqno;
} FormData_mdb_inherits;

/* ----------------
 *		Form_mdb_inherits corresponds to a pointer to a tuple with
 *		the format of mdb_inherits relation.
 * ----------------
 */
typedef FormData_mdb_inherits *Form_mdb_inherits;

/* ----------------
 *		compiler constants for mdb_inherits
 * ----------------
 */
#define Natts_mdb_inherits				3
#define Anum_mdb_inherits_inhrelid		1
#define Anum_mdb_inherits_inhparent		2
#define Anum_mdb_inherits_inhseqno		3

/* ----------------
 *		mdb_inherits has no initial contents
 * ----------------
 */

#endif   /* PG_INHERITS_H */
