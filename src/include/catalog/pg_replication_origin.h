/*-------------------------------------------------------------------------
 *
 * mdb_replication_origin.h
 *	  Persistent replication origin registry
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/mdb_replication_origin.h
 *
 * NOTES
 *	  the genbki.pl script reads this file and generates .bki
 *	  information from the DATA() statements.
 *
 *-------------------------------------------------------------------------
 */
#ifndef MDB_REPLICATION_ORIGIN_H
#define MDB_REPLICATION_ORIGIN_H

#include "catalog/genbki.h"
#include "access/xlogdefs.h"

/* ----------------
 *		mdb_replication_origin.  cpp turns this into
 *		typedef struct FormData_mdb_replication_origin
 * ----------------
 */
#define ReplicationOriginRelationId 6000

CATALOG(mdb_replication_origin,6000) BKI_SHARED_RELATION BKI_WITHOUT_OIDS
{
	/*
	 * Locally known id that get included into WAL.
	 *
	 * This should never leave the system.
	 *
	 * Needs to fit into an uint16, so we don't waste too much space in WAL
	 * records. For this reason we don't use a normal Oid column here, since
	 * we need to handle allocation of new values manually.
	 */
	Oid			roident;

	/*
	 * Variable-length fields start here, but we allow direct access to
	 * roname.
	 */

	/* external, free-format, name */
	text roname BKI_FORCE_NOT_NULL;

#ifdef CATALOG_VARLEN			/* further variable-length fields */
#endif
} FormData_mdb_replication_origin;

typedef FormData_mdb_replication_origin *Form_mdb_replication_origin;

/* ----------------
 *		compiler constants for mdb_replication_origin
 * ----------------
 */
#define Natts_mdb_replication_origin					2
#define Anum_mdb_replication_origin_roident			1
#define Anum_mdb_replication_origin_roname			2

/* ----------------
 *		mdb_replication_origin has no initial contents
 * ----------------
 */

#endif   /* MDB_REPLICATION_ORIGIN_H */
