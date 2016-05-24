/*
 * mdb_policy.h
 *	 definition of the system "policy" relation (mdb_policy)
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 */
#ifndef PG_POLICY_H
#define PG_POLICY_H

#include "catalog/genbki.h"

/* ----------------
 *		mdb_policy definition. cpp turns this into
 *		typedef struct FormData_mdb_policy
 * ----------------
 */
#define PolicyRelationId	3256

CATALOG(mdb_policy,3256)
{
	NameData	polname;		/* Policy name. */
	Oid			polrelid;		/* Oid of the relation with policy. */
	char		polcmd;			/* One of ACL_*_CHR, or '*' for all */

#ifdef CATALOG_VARLEN
	Oid			polroles[1];	/* Roles associated with policy, not-NULL */
	mdb_node_tree polqual;		/* Policy quals. */
	mdb_node_tree polwithcheck;	/* WITH CHECK quals. */
#endif
} FormData_mdb_policy;

/* ----------------
 *		Form_mdb_policy corresponds to a pointer to a row with
 *		the format of mdb_policy relation.
 * ----------------
 */
typedef FormData_mdb_policy *Form_mdb_policy;

/* ----------------
 *		compiler constants for mdb_policy
 * ----------------
 */
#define Natts_mdb_policy				6
#define Anum_mdb_policy_polname		1
#define Anum_mdb_policy_polrelid		2
#define Anum_mdb_policy_polcmd		3
#define Anum_mdb_policy_polroles		4
#define Anum_mdb_policy_polqual		5
#define Anum_mdb_policy_polwithcheck 6

#endif   /* PG_POLICY_H */
