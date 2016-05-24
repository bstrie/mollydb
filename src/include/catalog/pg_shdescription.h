/*-------------------------------------------------------------------------
 *
 * mdb_shdescription.h
 *	  definition of the system "shared description" relation
 *	  (mdb_shdescription)
 *
 * NOTE: an object is identified by the OID of the row that primarily
 * defines the object, plus the OID of the table that that row appears in.
 * For example, a database is identified by the OID of its mdb_database row
 * plus the mdb_class OID of table mdb_database.  This allows unique
 * identification of objects without assuming that OIDs are unique
 * across tables.
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/mdb_shdescription.h
 *
 * NOTES
 *		the genbki.pl script reads this file and generates .bki
 *		information from the DATA() statements.
 *
 *		XXX do NOT break up DATA() statements into multiple lines!
 *			the scripts are not as smart as you might think...
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_SHDESCRIPTION_H
#define PG_SHDESCRIPTION_H

#include "catalog/genbki.h"

/* ----------------
 *		mdb_shdescription definition.    cpp turns this into
 *		typedef struct FormData_mdb_shdescription
 * ----------------
 */
#define SharedDescriptionRelationId  2396

CATALOG(mdb_shdescription,2396) BKI_SHARED_RELATION BKI_WITHOUT_OIDS
{
	Oid			objoid;			/* OID of object itself */
	Oid			classoid;		/* OID of table containing object */

#ifdef CATALOG_VARLEN			/* variable-length fields start here */
	text description BKI_FORCE_NOT_NULL;		/* description of object */
#endif
} FormData_mdb_shdescription;

/* ----------------
 *		Form_mdb_shdescription corresponds to a pointer to a tuple with
 *		the format of mdb_shdescription relation.
 * ----------------
 */
typedef FormData_mdb_shdescription *Form_mdb_shdescription;

/* ----------------
 *		compiler constants for mdb_shdescription
 * ----------------
 */
#define Natts_mdb_shdescription			3
#define Anum_mdb_shdescription_objoid		1
#define Anum_mdb_shdescription_classoid	2
#define Anum_mdb_shdescription_description 3

/* ----------------
 *		initial contents of mdb_shdescription
 * ----------------
 */

/*
 *	Because the contents of this table are taken from the other *.h files,
 *	there is no initialization here.  The initial contents are extracted
 *	by genbki.pl and loaded during initdb.
 */

#endif   /* PG_SHDESCRIPTION_H */
