/*-------------------------------------------------------------------------
 *
 * mdb_ts_config.h
 *	definition of configuration of tsearch
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/mdb_ts_config.h
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
#ifndef PG_TS_CONFIG_H
#define PG_TS_CONFIG_H

#include "catalog/genbki.h"

/* ----------------
 *		mdb_ts_config definition.  cpp turns this into
 *		typedef struct FormData_mdb_ts_config
 * ----------------
 */
#define TSConfigRelationId	3602

CATALOG(mdb_ts_config,3602)
{
	NameData	cfgname;		/* name of configuration */
	Oid			cfgnamespace;	/* name space */
	Oid			cfgowner;		/* owner */
	Oid			cfgparser;		/* OID of parser (in mdb_ts_parser) */
} FormData_mdb_ts_config;

typedef FormData_mdb_ts_config *Form_mdb_ts_config;

/* ----------------
 *		compiler constants for mdb_ts_config
 * ----------------
 */
#define Natts_mdb_ts_config				4
#define Anum_mdb_ts_config_cfgname		1
#define Anum_mdb_ts_config_cfgnamespace	2
#define Anum_mdb_ts_config_cfgowner		3
#define Anum_mdb_ts_config_cfgparser		4

/* ----------------
 *		initial contents of mdb_ts_config
 * ----------------
 */
DATA(insert OID = 3748 ( "simple" PGNSP PGUID 3722 ));
DESCR("simple configuration");

#endif   /* PG_TS_CONFIG_H */
