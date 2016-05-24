/*-------------------------------------------------------------------------
 *
 * mdb_conversion.h
 *	  definition of the system "conversion" relation (mdb_conversion)
 *	  along with the relation's initial contents.
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/mdb_conversion.h
 *
 * NOTES
 *	  the genbki.pl script reads this file and generates .bki
 *	  information from the DATA() statements.
 *
 *-------------------------------------------------------------------------
 */
#ifndef MDB_CONVERSION_H
#define MDB_CONVERSION_H

#include "catalog/genbki.h"

/* ----------------------------------------------------------------
 *		mdb_conversion definition.
 *
 *		cpp turns this into typedef struct FormData_mdb_namespace
 *
 *	conname				name of the conversion
 *	connamespace		name space which the conversion belongs to
 *	conowner			owner of the conversion
 *	conforencoding		FOR encoding id
 *	contoencoding		TO encoding id
 *	conproc				OID of the conversion proc
 *	condefault			TRUE if this is a default conversion
 * ----------------------------------------------------------------
 */
#define ConversionRelationId  2607

CATALOG(mdb_conversion,2607)
{
	NameData	conname;
	Oid			connamespace;
	Oid			conowner;
	int32		conforencoding;
	int32		contoencoding;
	regproc		conproc;
	bool		condefault;
} FormData_mdb_conversion;

/* ----------------
 *		Form_mdb_conversion corresponds to a pointer to a tuple with
 *		the format of mdb_conversion relation.
 * ----------------
 */
typedef FormData_mdb_conversion *Form_mdb_conversion;

/* ----------------
 *		compiler constants for mdb_conversion
 * ----------------
 */

#define Natts_mdb_conversion				7
#define Anum_mdb_conversion_conname		1
#define Anum_mdb_conversion_connamespace 2
#define Anum_mdb_conversion_conowner		3
#define Anum_mdb_conversion_conforencoding		4
#define Anum_mdb_conversion_contoencoding		5
#define Anum_mdb_conversion_conproc		6
#define Anum_mdb_conversion_condefault	7

/* ----------------
 * initial contents of mdb_conversion
 * ---------------
 */

#endif   /* MDB_CONVERSION_H */
