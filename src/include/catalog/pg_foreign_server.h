/*-------------------------------------------------------------------------
 *
 * mdb_foreign_server.h
 *	  definition of the system "foreign server" relation (mdb_foreign_server)
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/mdb_foreign_server.h
 *
 * NOTES
 *	  the genbki.pl script reads this file and generates .bki
 *	  information from the DATA() statements.
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_FOREIGN_SERVER_H
#define PG_FOREIGN_SERVER_H

#include "catalog/genbki.h"

/* ----------------
 *		mdb_foreign_server definition.  cpp turns this into
 *		typedef struct FormData_mdb_foreign_server
 * ----------------
 */
#define ForeignServerRelationId 1417

CATALOG(mdb_foreign_server,1417)
{
	NameData	srvname;		/* foreign server name */
	Oid			srvowner;		/* server owner */
	Oid			srvfdw;			/* server FDW */

#ifdef CATALOG_VARLEN			/* variable-length fields start here */
	text		srvtype;
	text		srvversion;
	aclitem		srvacl[1];		/* access permissions */
	text		srvoptions[1];	/* FDW-specific options */
#endif
} FormData_mdb_foreign_server;

/* ----------------
 *		Form_mdb_foreign_server corresponds to a pointer to a tuple with
 *		the format of mdb_foreign_server relation.
 * ----------------
 */
typedef FormData_mdb_foreign_server *Form_mdb_foreign_server;

/* ----------------
 *		compiler constants for mdb_foreign_server
 * ----------------
 */

#define Natts_mdb_foreign_server					7
#define Anum_mdb_foreign_server_srvname			1
#define Anum_mdb_foreign_server_srvowner			2
#define Anum_mdb_foreign_server_srvfdw			3
#define Anum_mdb_foreign_server_srvtype			4
#define Anum_mdb_foreign_server_srvversion		5
#define Anum_mdb_foreign_server_srvacl			6
#define Anum_mdb_foreign_server_srvoptions		7

#endif   /* PG_FOREIGN_SERVER_H */
