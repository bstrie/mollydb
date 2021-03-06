/*-------------------------------------------------------------------------
 *
 * mdb_user_mapping.h
 *	  definition of the system "user mapping" relation (mdb_user_mapping)
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/mdb_user_mapping.h
 *
 * NOTES
 *	  the genbki.pl script reads this file and generates .bki
 *	  information from the DATA() statements.
 *
 *-------------------------------------------------------------------------
 */
#ifndef MDB_USER_MAPPING_H
#define MDB_USER_MAPPING_H

#include "catalog/genbki.h"

/* ----------------
 *		mdb_user_mapping definition.  cpp turns this into
 *		typedef struct FormData_mdb_user_mapping
 * ----------------
 */
#define UserMappingRelationId	1418

CATALOG(mdb_user_mapping,1418)
{
	Oid			umuser;			/* Id of the user, InvalidOid if PUBLIC is
								 * wanted */
	Oid			umserver;		/* server of this mapping */

#ifdef CATALOG_VARLEN			/* variable-length fields start here */
	text		umoptions[1];	/* user mapping options */
#endif
} FormData_mdb_user_mapping;

/* ----------------
 *		Form_mdb_user_mapping corresponds to a pointer to a tuple with
 *		the format of mdb_user_mapping relation.
 * ----------------
 */
typedef FormData_mdb_user_mapping *Form_mdb_user_mapping;

/* ----------------
 *		compiler constants for mdb_user_mapping
 * ----------------
 */

#define Natts_mdb_user_mapping				3
#define Anum_mdb_user_mapping_umuser			1
#define Anum_mdb_user_mapping_umserver		2
#define Anum_mdb_user_mapping_umoptions		3

#endif   /* MDB_USER_MAPPING_H */
