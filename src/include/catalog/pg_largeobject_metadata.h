/*-------------------------------------------------------------------------
 *
 * mdb_largeobject_metadata.h
 *	  definition of the system "largeobject_metadata" relation (mdb_largeobject_metadata)
 *	  along with the relation's initial contents.
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/mdb_largeobject_metadata.h
 *
 * NOTES
 *	  the genbki.pl script reads this file and generates .bki
 *	  information from the DATA() statements.
 *
 *-------------------------------------------------------------------------
 */
#ifndef MDB_LARGEOBJECT_METADATA_H
#define MDB_LARGEOBJECT_METADATA_H

#include "catalog/genbki.h"

/* ----------------
 *		mdb_largeobject_metadata definition. cpp turns this into
 *		typedef struct FormData_mdb_largeobject_metadata
 * ----------------
 */
#define LargeObjectMetadataRelationId  2995

CATALOG(mdb_largeobject_metadata,2995)
{
	Oid			lomowner;		/* OID of the largeobject owner */

#ifdef CATALOG_VARLEN			/* variable-length fields start here */
	aclitem		lomacl[1];		/* access permissions */
#endif
} FormData_mdb_largeobject_metadata;

/* ----------------
 *		Form_mdb_largeobject_metadata corresponds to a pointer to a tuple
 *		with the format of mdb_largeobject_metadata relation.
 * ----------------
 */
typedef FormData_mdb_largeobject_metadata *Form_mdb_largeobject_metadata;

/* ----------------
 *		compiler constants for mdb_largeobject_metadata
 * ----------------
 */
#define Natts_mdb_largeobject_metadata			2
#define Anum_mdb_largeobject_metadata_lomowner	1
#define Anum_mdb_largeobject_metadata_lomacl		2

#endif   /* MDB_LARGEOBJECT_METADATA_H */
