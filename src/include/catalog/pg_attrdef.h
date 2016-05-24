/*-------------------------------------------------------------------------
 *
 * mdb_attrdef.h
 *	  definition of the system "attribute defaults" relation (mdb_attrdef)
 *	  along with the relation's initial contents.
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/mdb_attrdef.h
 *
 * NOTES
 *	  the genbki.pl script reads this file and generates .bki
 *	  information from the DATA() statements.
 *
 *-------------------------------------------------------------------------
 */
#ifndef MDB_ATTRDEF_H
#define MDB_ATTRDEF_H

#include "catalog/genbki.h"

/* ----------------
 *		mdb_attrdef definition.  cpp turns this into
 *		typedef struct FormData_mdb_attrdef
 * ----------------
 */
#define AttrDefaultRelationId  2604

CATALOG(mdb_attrdef,2604)
{
	Oid			adrelid;		/* OID of table containing attribute */
	int16		adnum;			/* attnum of attribute */

#ifdef CATALOG_VARLEN			/* variable-length fields start here */
	mdb_node_tree adbin;			/* nodeToString representation of default */
	text		adsrc;			/* human-readable representation of default */
#endif
} FormData_mdb_attrdef;

/* ----------------
 *		Form_mdb_attrdef corresponds to a pointer to a tuple with
 *		the format of mdb_attrdef relation.
 * ----------------
 */
typedef FormData_mdb_attrdef *Form_mdb_attrdef;

/* ----------------
 *		compiler constants for mdb_attrdef
 * ----------------
 */
#define Natts_mdb_attrdef				4
#define Anum_mdb_attrdef_adrelid			1
#define Anum_mdb_attrdef_adnum			2
#define Anum_mdb_attrdef_adbin			3
#define Anum_mdb_attrdef_adsrc			4

#endif   /* MDB_ATTRDEF_H */
