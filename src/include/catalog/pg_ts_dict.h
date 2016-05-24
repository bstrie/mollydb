/*-------------------------------------------------------------------------
 *
 * mdb_ts_dict.h
 *	definition of dictionaries for tsearch
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/mdb_ts_dict.h
 *
 * NOTES
 *		the genbki.pl script reads this file and generates .bki
 *		information from the DATA() statements.
 *
 *		XXX do NOT break up DATA() statements into multiple lines!
 *			the scripts are not as smart as you might think...
 *
 *-------------------------------------------------------------------------
 */
#ifndef MDB_TS_DICT_H
#define MDB_TS_DICT_H

#include "catalog/genbki.h"

/* ----------------
 *		mdb_ts_dict definition.  cpp turns this into
 *		typedef struct FormData_mdb_ts_dict
 * ----------------
 */
#define TSDictionaryRelationId	3600

CATALOG(mdb_ts_dict,3600)
{
	NameData	dictname;		/* dictionary name */
	Oid			dictnamespace;	/* name space */
	Oid			dictowner;		/* owner */
	Oid			dicttemplate;	/* dictionary's template */

#ifdef CATALOG_VARLEN			/* variable-length fields start here */
	text		dictinitoption; /* options passed to dict_init() */
#endif
} FormData_mdb_ts_dict;

typedef FormData_mdb_ts_dict *Form_mdb_ts_dict;

/* ----------------
 *		compiler constants for mdb_ts_dict
 * ----------------
 */
#define Natts_mdb_ts_dict				5
#define Anum_mdb_ts_dict_dictname		1
#define Anum_mdb_ts_dict_dictnamespace	2
#define Anum_mdb_ts_dict_dictowner		3
#define Anum_mdb_ts_dict_dicttemplate	4
#define Anum_mdb_ts_dict_dictinitoption	5

/* ----------------
 *		initial contents of mdb_ts_dict
 * ----------------
 */

DATA(insert OID = 3765 ( "simple" PGNSP PGUID 3727 _null_));
DESCR("simple dictionary: just lower case and check for stopword");

#endif   /* MDB_TS_DICT_H */
