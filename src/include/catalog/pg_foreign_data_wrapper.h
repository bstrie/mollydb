/*-------------------------------------------------------------------------
 *
 * mdb_foreign_data_wrapper.h
 *	  definition of the system "foreign-data wrapper" relation (mdb_foreign_data_wrapper)
 *	  along with the relation's initial contents.
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/mdb_foreign_data_wrapper.h
 *
 * NOTES
 *	  the genbki.pl script reads this file and generates .bki
 *	  information from the DATA() statements.
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_FOREIGN_DATA_WRAPPER_H
#define PG_FOREIGN_DATA_WRAPPER_H

#include "catalog/genbki.h"

/* ----------------
 *		mdb_foreign_data_wrapper definition.  cpp turns this into
 *		typedef struct FormData_mdb_foreign_data_wrapper
 * ----------------
 */
#define ForeignDataWrapperRelationId	2328

CATALOG(mdb_foreign_data_wrapper,2328)
{
	NameData	fdwname;		/* foreign-data wrapper name */
	Oid			fdwowner;		/* FDW owner */
	Oid			fdwhandler;		/* handler function, or 0 if none */
	Oid			fdwvalidator;	/* option validation function, or 0 if none */

#ifdef CATALOG_VARLEN			/* variable-length fields start here */
	aclitem		fdwacl[1];		/* access permissions */
	text		fdwoptions[1];	/* FDW options */
#endif
} FormData_mdb_foreign_data_wrapper;

/* ----------------
 *		Form_mdb_fdw corresponds to a pointer to a tuple with
 *		the format of mdb_fdw relation.
 * ----------------
 */
typedef FormData_mdb_foreign_data_wrapper *Form_mdb_foreign_data_wrapper;

/* ----------------
 *		compiler constants for mdb_fdw
 * ----------------
 */

#define Natts_mdb_foreign_data_wrapper				6
#define Anum_mdb_foreign_data_wrapper_fdwname		1
#define Anum_mdb_foreign_data_wrapper_fdwowner		2
#define Anum_mdb_foreign_data_wrapper_fdwhandler		3
#define Anum_mdb_foreign_data_wrapper_fdwvalidator	4
#define Anum_mdb_foreign_data_wrapper_fdwacl			5
#define Anum_mdb_foreign_data_wrapper_fdwoptions		6

#endif   /* PG_FOREIGN_DATA_WRAPPER_H */
