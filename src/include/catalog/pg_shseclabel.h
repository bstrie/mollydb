/* -------------------------------------------------------------------------
 *
 * mdb_shseclabel.h
 *	  definition of the system "security label" relation (mdb_shseclabel)
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * -------------------------------------------------------------------------
 */
#ifndef MDB_SHSECLABEL_H
#define MDB_SHSECLABEL_H

#include "catalog/genbki.h"

/* ----------------
 *		mdb_shseclabel definition. cpp turns this into
 *		typedef struct FormData_mdb_shseclabel
 * ----------------
 */
#define SharedSecLabelRelationId			3592
#define SharedSecLabelRelation_Rowtype_Id	4066

CATALOG(mdb_shseclabel,3592) BKI_SHARED_RELATION BKI_ROWTYPE_OID(4066) BKI_WITHOUT_OIDS BKI_SCHEMA_MACRO
{
	Oid			objoid;			/* OID of the shared object itself */
	Oid			classoid;		/* OID of table containing the shared object */

#ifdef CATALOG_VARLEN			/* variable-length fields start here */
	text provider BKI_FORCE_NOT_NULL;	/* name of label provider */
	text label	BKI_FORCE_NOT_NULL;		/* security label of the object */
#endif
} FormData_mdb_shseclabel;

typedef FormData_mdb_shseclabel *Form_mdb_shseclabel;

/* ----------------
 *		compiler constants for mdb_shseclabel
 * ----------------
 */
#define Natts_mdb_shseclabel				4
#define Anum_mdb_shseclabel_objoid		1
#define Anum_mdb_shseclabel_classoid		2
#define Anum_mdb_shseclabel_provider		3
#define Anum_mdb_shseclabel_label		4

#endif   /* MDB_SHSECLABEL_H */
