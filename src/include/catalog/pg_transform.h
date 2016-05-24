/*-------------------------------------------------------------------------
 *
 * mdb_transform.h
 *
 * Copyright (c) 2012-2016, MollyDB Global Development Group
 *
 * src/include/catalog/mdb_transform.h
 *
 * NOTES
 *	  the genbki.pl script reads this file and generates .bki
 *	  information from the DATA() statements.
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_TRANSFORM_H
#define PG_TRANSFORM_H

#include "catalog/genbki.h"

/* ----------------
 *		mdb_transform definition.  cpp turns this into
 *		typedef struct FormData_mdb_transform
 * ----------------
 */
#define TransformRelationId 3576

CATALOG(mdb_transform,3576)
{
	Oid			trftype;
	Oid			trflang;
	regproc		trffromsql;
	regproc		trftosql;
} FormData_mdb_transform;

typedef FormData_mdb_transform *Form_mdb_transform;

/* ----------------
 *		compiler constants for mdb_transform
 * ----------------
 */
#define Natts_mdb_transform			4
#define Anum_mdb_transform_trftype	1
#define Anum_mdb_transform_trflang	2
#define Anum_mdb_transform_trffromsql	3
#define Anum_mdb_transform_trftosql	4

#endif   /* PG_TRANSFORM_H */
