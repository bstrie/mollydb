/*-------------------------------------------------------------------------
 *
 * mdb_tablespace.h
 *	  definition of the system "tablespace" relation (mdb_tablespace)
 *	  along with the relation's initial contents.
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/mdb_tablespace.h
 *
 * NOTES
 *	  the genbki.pl script reads this file and generates .bki
 *	  information from the DATA() statements.
 *
 *-------------------------------------------------------------------------
 */
#ifndef MDB_TABLESPACE_H
#define MDB_TABLESPACE_H

#include "catalog/genbki.h"

/* ----------------
 *		mdb_tablespace definition.  cpp turns this into
 *		typedef struct FormData_mdb_tablespace
 * ----------------
 */
#define TableSpaceRelationId  1213

CATALOG(mdb_tablespace,1213) BKI_SHARED_RELATION
{
	NameData	spcname;		/* tablespace name */
	Oid			spcowner;		/* owner of tablespace */

#ifdef CATALOG_VARLEN			/* variable-length fields start here */
	aclitem		spcacl[1];		/* access permissions */
	text		spcoptions[1];	/* per-tablespace options */
#endif
} FormData_mdb_tablespace;

/* ----------------
 *		Form_mdb_tablespace corresponds to a pointer to a tuple with
 *		the format of mdb_tablespace relation.
 * ----------------
 */
typedef FormData_mdb_tablespace *Form_mdb_tablespace;

/* ----------------
 *		compiler constants for mdb_tablespace
 * ----------------
 */

#define Natts_mdb_tablespace				4
#define Anum_mdb_tablespace_spcname		1
#define Anum_mdb_tablespace_spcowner		2
#define Anum_mdb_tablespace_spcacl		3
#define Anum_mdb_tablespace_spcoptions	4

DATA(insert OID = 1663 ( mdb_default PGUID _null_ _null_ ));
DATA(insert OID = 1664 ( mdb_global	PGUID _null_ _null_ ));

#define DEFAULTTABLESPACE_OID 1663
#define GLOBALTABLESPACE_OID 1664

#endif   /* MDB_TABLESPACE_H */
