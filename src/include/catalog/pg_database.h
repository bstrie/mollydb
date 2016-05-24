/*-------------------------------------------------------------------------
 *
 * mdb_database.h
 *	  definition of the system "database" relation (mdb_database)
 *	  along with the relation's initial contents.
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/mdb_database.h
 *
 * NOTES
 *	  the genbki.pl script reads this file and generates .bki
 *	  information from the DATA() statements.
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_DATABASE_H
#define PG_DATABASE_H

#include "catalog/genbki.h"

/* ----------------
 *		mdb_database definition.  cpp turns this into
 *		typedef struct FormData_mdb_database
 * ----------------
 */
#define DatabaseRelationId	1262
#define DatabaseRelation_Rowtype_Id  1248

CATALOG(mdb_database,1262) BKI_SHARED_RELATION BKI_ROWTYPE_OID(1248) BKI_SCHEMA_MACRO
{
	NameData	datname;		/* database name */
	Oid			datdba;			/* owner of database */
	int32		encoding;		/* character encoding */
	NameData	datcollate;		/* LC_COLLATE setting */
	NameData	datctype;		/* LC_CTYPE setting */
	bool		datistemplate;	/* allowed as CREATE DATABASE template? */
	bool		datallowconn;	/* new connections allowed? */
	int32		datconnlimit;	/* max connections allowed (-1=no limit) */
	Oid			datlastsysoid;	/* highest OID to consider a system OID */
	TransactionId datfrozenxid; /* all Xids < this are frozen in this DB */
	TransactionId datminmxid;	/* all multixacts in the DB are >= this */
	Oid			dattablespace;	/* default table space for this DB */

#ifdef CATALOG_VARLEN			/* variable-length fields start here */
	aclitem		datacl[1];		/* access permissions */
#endif
} FormData_mdb_database;

/* ----------------
 *		Form_mdb_database corresponds to a pointer to a tuple with
 *		the format of mdb_database relation.
 * ----------------
 */
typedef FormData_mdb_database *Form_mdb_database;

/* ----------------
 *		compiler constants for mdb_database
 * ----------------
 */
#define Natts_mdb_database				13
#define Anum_mdb_database_datname		1
#define Anum_mdb_database_datdba			2
#define Anum_mdb_database_encoding		3
#define Anum_mdb_database_datcollate		4
#define Anum_mdb_database_datctype		5
#define Anum_mdb_database_datistemplate	6
#define Anum_mdb_database_datallowconn	7
#define Anum_mdb_database_datconnlimit	8
#define Anum_mdb_database_datlastsysoid	9
#define Anum_mdb_database_datfrozenxid	10
#define Anum_mdb_database_datminmxid		11
#define Anum_mdb_database_dattablespace	12
#define Anum_mdb_database_datacl			13

DATA(insert OID = 1 (  template1 PGUID ENCODING "LC_COLLATE" "LC_CTYPE" t t -1 0 0 1 1663 _null_));
SHDESCR("default template for new databases");
#define TemplateDbOid			1

#endif   /* PG_DATABASE_H */
