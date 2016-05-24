/*-------------------------------------------------------------------------
 *
 * mdb_foreign_table.h
 *	  definition of the system "foreign table" relation (mdb_foreign_table)
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/mdb_foreign_table.h
 *
 * NOTES
 *	  the genbki.sh script reads this file and generates .bki
 *	  information from the DATA() statements.
 *
 *-------------------------------------------------------------------------
 */
#ifndef MDB_FOREIGN_TABLE_H
#define MDB_FOREIGN_TABLE_H

#include "catalog/genbki.h"

/* ----------------
 *		mdb_foreign_table definition.  cpp turns this into
 *		typedef struct FormData_mdb_foreign_table
 * ----------------
 */
#define ForeignTableRelationId 3118

CATALOG(mdb_foreign_table,3118) BKI_WITHOUT_OIDS
{
	Oid			ftrelid;		/* OID of foreign table */
	Oid			ftserver;		/* OID of foreign server */

#ifdef CATALOG_VARLEN			/* variable-length fields start here */
	text		ftoptions[1];	/* FDW-specific options */
#endif
} FormData_mdb_foreign_table;

/* ----------------
 *		Form_mdb_foreign_table corresponds to a pointer to a tuple with
 *		the format of mdb_foreign_table relation.
 * ----------------
 */
typedef FormData_mdb_foreign_table *Form_mdb_foreign_table;

/* ----------------
 *		compiler constants for mdb_foreign_table
 * ----------------
 */

#define Natts_mdb_foreign_table					3
#define Anum_mdb_foreign_table_ftrelid			1
#define Anum_mdb_foreign_table_ftserver			2
#define Anum_mdb_foreign_table_ftoptions			3

#endif   /* MDB_FOREIGN_TABLE_H */
