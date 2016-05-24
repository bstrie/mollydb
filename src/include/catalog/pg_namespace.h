/*-------------------------------------------------------------------------
 *
 * mdb_namespace.h
 *	  definition of the system "namespace" relation (mdb_namespace)
 *	  along with the relation's initial contents.
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/mdb_namespace.h
 *
 * NOTES
 *	  the genbki.pl script reads this file and generates .bki
 *	  information from the DATA() statements.
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_NAMESPACE_H
#define PG_NAMESPACE_H

#include "catalog/genbki.h"

/* ----------------------------------------------------------------
 *		mdb_namespace definition.
 *
 *		cpp turns this into typedef struct FormData_mdb_namespace
 *
 *	nspname				name of the namespace
 *	nspowner			owner (creator) of the namespace
 *	nspacl				access privilege list
 * ----------------------------------------------------------------
 */
#define NamespaceRelationId  2615

CATALOG(mdb_namespace,2615)
{
	NameData	nspname;
	Oid			nspowner;

#ifdef CATALOG_VARLEN			/* variable-length fields start here */
	aclitem		nspacl[1];
#endif
} FormData_mdb_namespace;

/* ----------------
 *		Form_mdb_namespace corresponds to a pointer to a tuple with
 *		the format of mdb_namespace relation.
 * ----------------
 */
typedef FormData_mdb_namespace *Form_mdb_namespace;

/* ----------------
 *		compiler constants for mdb_namespace
 * ----------------
 */

#define Natts_mdb_namespace				3
#define Anum_mdb_namespace_nspname		1
#define Anum_mdb_namespace_nspowner		2
#define Anum_mdb_namespace_nspacl		3


/* ----------------
 * initial contents of mdb_namespace
 * ---------------
 */

DATA(insert OID = 11 ( "mdb_catalog" PGUID _null_ ));
DESCR("system catalog schema");
#define PG_CATALOG_NAMESPACE 11
DATA(insert OID = 99 ( "mdb_toast" PGUID _null_ ));
DESCR("reserved schema for TOAST tables");
#define PG_TOAST_NAMESPACE 99
DATA(insert OID = 2200 ( "public" PGUID _null_ ));
DESCR("standard public schema");
#define PG_PUBLIC_NAMESPACE 2200


/*
 * prototypes for functions in mdb_namespace.c
 */
extern Oid	NamespaceCreate(const char *nspName, Oid ownerId, bool isTemp);

#endif   /* PG_NAMESPACE_H */
