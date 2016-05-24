/*-------------------------------------------------------------------------
 *
 * mdb_auth_members.h
 *	  definition of the system "authorization identifier members" relation
 *	  (mdb_auth_members) along with the relation's initial contents.
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/mdb_auth_members.h
 *
 * NOTES
 *	  the genbki.pl script reads this file and generates .bki
 *	  information from the DATA() statements.
 *
 *-------------------------------------------------------------------------
 */
#ifndef MDB_AUTH_MEMBERS_H
#define MDB_AUTH_MEMBERS_H

#include "catalog/genbki.h"

/* ----------------
 *		mdb_auth_members definition.  cpp turns this into
 *		typedef struct FormData_mdb_auth_members
 * ----------------
 */
#define AuthMemRelationId	1261
#define AuthMemRelation_Rowtype_Id	2843

CATALOG(mdb_auth_members,1261) BKI_SHARED_RELATION BKI_WITHOUT_OIDS BKI_ROWTYPE_OID(2843) BKI_SCHEMA_MACRO
{
	Oid			roleid;			/* ID of a role */
	Oid			member;			/* ID of a member of that role */
	Oid			grantor;		/* who granted the membership */
	bool		admin_option;	/* granted with admin option? */
} FormData_mdb_auth_members;

/* ----------------
 *		Form_mdb_auth_members corresponds to a pointer to a tuple with
 *		the format of mdb_auth_members relation.
 * ----------------
 */
typedef FormData_mdb_auth_members *Form_mdb_auth_members;

/* ----------------
 *		compiler constants for mdb_auth_members
 * ----------------
 */
#define Natts_mdb_auth_members				4
#define Anum_mdb_auth_members_roleid			1
#define Anum_mdb_auth_members_member			2
#define Anum_mdb_auth_members_grantor		3
#define Anum_mdb_auth_members_admin_option	4

#endif   /* MDB_AUTH_MEMBERS_H */
