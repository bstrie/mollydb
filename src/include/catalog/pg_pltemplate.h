/*-------------------------------------------------------------------------
 *
 * mdb_pltemplate.h
 *	  definition of the system "PL template" relation (mdb_pltemplate)
 *	  along with the relation's initial contents.
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/mdb_pltemplate.h
 *
 * NOTES
 *	  the genbki.pl script reads this file and generates .bki
 *	  information from the DATA() statements.
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_PLTEMPLATE_H
#define PG_PLTEMPLATE_H

#include "catalog/genbki.h"

/* ----------------
 *		mdb_pltemplate definition.  cpp turns this into
 *		typedef struct FormData_mdb_pltemplate
 * ----------------
 */
#define PLTemplateRelationId	1136

CATALOG(mdb_pltemplate,1136) BKI_SHARED_RELATION BKI_WITHOUT_OIDS
{
	NameData	tmplname;		/* name of PL */
	bool		tmpltrusted;	/* PL is trusted? */
	bool		tmpldbacreate;	/* PL is installable by db owner? */

#ifdef CATALOG_VARLEN			/* variable-length fields start here */
	text tmplhandler BKI_FORCE_NOT_NULL;		/* name of call handler
												 * function */
	text		tmplinline;		/* name of anonymous-block handler, or NULL */
	text		tmplvalidator;	/* name of validator function, or NULL */
	text tmpllibrary BKI_FORCE_NOT_NULL;		/* path of shared library */
	aclitem		tmplacl[1];		/* access privileges for template */
#endif
} FormData_mdb_pltemplate;

/* ----------------
 *		Form_mdb_pltemplate corresponds to a pointer to a row with
 *		the format of mdb_pltemplate relation.
 * ----------------
 */
typedef FormData_mdb_pltemplate *Form_mdb_pltemplate;

/* ----------------
 *		compiler constants for mdb_pltemplate
 * ----------------
 */
#define Natts_mdb_pltemplate					8
#define Anum_mdb_pltemplate_tmplname			1
#define Anum_mdb_pltemplate_tmpltrusted		2
#define Anum_mdb_pltemplate_tmpldbacreate	3
#define Anum_mdb_pltemplate_tmplhandler		4
#define Anum_mdb_pltemplate_tmplinline		5
#define Anum_mdb_pltemplate_tmplvalidator	6
#define Anum_mdb_pltemplate_tmpllibrary		7
#define Anum_mdb_pltemplate_tmplacl			8


/* ----------------
 *		initial contents of mdb_pltemplate
 * ----------------
 */

DATA(insert ( "plmdb"		t t "plmdb_call_handler" "plmdb_inline_handler" "plmdb_validator" "$libdir/plmdb" _null_ ));
DATA(insert ( "pltcl"		t t "pltcl_call_handler" _null_ _null_ "$libdir/pltcl" _null_ ));
DATA(insert ( "pltclu"		f f "pltclu_call_handler" _null_ _null_ "$libdir/pltcl" _null_ ));
DATA(insert ( "plperl"		t t "plperl_call_handler" "plperl_inline_handler" "plperl_validator" "$libdir/plperl" _null_ ));
DATA(insert ( "plperlu"		f f "plperlu_call_handler" "plperlu_inline_handler" "plperlu_validator" "$libdir/plperl" _null_ ));
DATA(insert ( "plpythonu"	f f "plpython_call_handler" "plpython_inline_handler" "plpython_validator" "$libdir/plpython2" _null_ ));
DATA(insert ( "plpython2u"	f f "plpython2_call_handler" "plpython2_inline_handler" "plpython2_validator" "$libdir/plpython2" _null_ ));
DATA(insert ( "plpython3u"	f f "plpython3_call_handler" "plpython3_inline_handler" "plpython3_validator" "$libdir/plpython3" _null_ ));

#endif   /* PG_PLTEMPLATE_H */
