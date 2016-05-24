/*-------------------------------------------------------------------------
 *
 * mdb_largeobject.h
 *	  definition of the system "largeobject" relation (mdb_largeobject)
 *	  along with the relation's initial contents.
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/mdb_largeobject.h
 *
 * NOTES
 *	  the genbki.pl script reads this file and generates .bki
 *	  information from the DATA() statements.
 *
 *-------------------------------------------------------------------------
 */
#ifndef MDB_LARGEOBJECT_H
#define MDB_LARGEOBJECT_H

#include "catalog/genbki.h"

/* ----------------
 *		mdb_largeobject definition.  cpp turns this into
 *		typedef struct FormData_mdb_largeobject
 * ----------------
 */
#define LargeObjectRelationId  2613

CATALOG(mdb_largeobject,2613) BKI_WITHOUT_OIDS
{
	Oid			loid;			/* Identifier of large object */
	int32		pageno;			/* Page number (starting from 0) */

	/* data has variable length, but we allow direct access; see inv_api.c */
	bytea data	BKI_FORCE_NOT_NULL;		/* Data for page (may be zero-length) */
} FormData_mdb_largeobject;

/* ----------------
 *		Form_mdb_largeobject corresponds to a pointer to a tuple with
 *		the format of mdb_largeobject relation.
 * ----------------
 */
typedef FormData_mdb_largeobject *Form_mdb_largeobject;

/* ----------------
 *		compiler constants for mdb_largeobject
 * ----------------
 */
#define Natts_mdb_largeobject			3
#define Anum_mdb_largeobject_loid		1
#define Anum_mdb_largeobject_pageno		2
#define Anum_mdb_largeobject_data		3

extern Oid	LargeObjectCreate(Oid loid);
extern void LargeObjectDrop(Oid loid);
extern bool LargeObjectExists(Oid loid);

#endif   /* MDB_LARGEOBJECT_H */
