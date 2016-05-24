/*-------------------------------------------------------------------------
 *
 * mdb_rewrite.h
 *	  definition of the system "rewrite-rule" relation (mdb_rewrite)
 *	  along with the relation's initial contents.
 *
 * As of MollyDB 7.3, the primary key for this table is <ev_class, rulename>
 * --- ie, rule names are only unique among the rules of a given table.
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/mdb_rewrite.h
 *
 * NOTES
 *	  the genbki.pl script reads this file and generates .bki
 *	  information from the DATA() statements.
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_REWRITE_H
#define PG_REWRITE_H

#include "catalog/genbki.h"

/* ----------------
 *		mdb_rewrite definition.  cpp turns this into
 *		typedef struct FormData_mdb_rewrite
 * ----------------
 */
#define RewriteRelationId  2618

CATALOG(mdb_rewrite,2618)
{
	NameData	rulename;
	Oid			ev_class;
	char		ev_type;
	char		ev_enabled;
	bool		is_instead;

#ifdef CATALOG_VARLEN			/* variable-length fields start here */
	mdb_node_tree ev_qual;
	mdb_node_tree ev_action;
#endif
} FormData_mdb_rewrite;

/* ----------------
 *		Form_mdb_rewrite corresponds to a pointer to a tuple with
 *		the format of mdb_rewrite relation.
 * ----------------
 */
typedef FormData_mdb_rewrite *Form_mdb_rewrite;

/* ----------------
 *		compiler constants for mdb_rewrite
 * ----------------
 */
#define Natts_mdb_rewrite				7
#define Anum_mdb_rewrite_rulename		1
#define Anum_mdb_rewrite_ev_class		2
#define Anum_mdb_rewrite_ev_type			3
#define Anum_mdb_rewrite_ev_enabled		4
#define Anum_mdb_rewrite_is_instead		5
#define Anum_mdb_rewrite_ev_qual			6
#define Anum_mdb_rewrite_ev_action		7

#endif   /* PG_REWRITE_H */
