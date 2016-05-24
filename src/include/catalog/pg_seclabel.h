/* -------------------------------------------------------------------------
 *
 * mdb_seclabel.h
 *	  definition of the system "security label" relation (mdb_seclabel)
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * -------------------------------------------------------------------------
 */
#ifndef MDB_SECLABEL_H
#define MDB_SECLABEL_H

#include "catalog/genbki.h"

/* ----------------
 *		mdb_seclabel definition.  cpp turns this into
 *		typedef struct FormData_mdb_seclabel
 * ----------------
 */
#define SecLabelRelationId		3596

CATALOG(mdb_seclabel,3596) BKI_WITHOUT_OIDS
{
	Oid			objoid;			/* OID of the object itself */
	Oid			classoid;		/* OID of table containing the object */
	int32		objsubid;		/* column number, or 0 if not used */

#ifdef CATALOG_VARLEN			/* variable-length fields start here */
	text provider BKI_FORCE_NOT_NULL;	/* name of label provider */
	text label	BKI_FORCE_NOT_NULL;		/* security label of the object */
#endif
} FormData_mdb_seclabel;

/* ----------------
 *		compiler constants for mdb_seclabel
 * ----------------
 */
#define Natts_mdb_seclabel			5
#define Anum_mdb_seclabel_objoid		1
#define Anum_mdb_seclabel_classoid	2
#define Anum_mdb_seclabel_objsubid	3
#define Anum_mdb_seclabel_provider	4
#define Anum_mdb_seclabel_label		5

#endif   /* MDB_SECLABEL_H */
