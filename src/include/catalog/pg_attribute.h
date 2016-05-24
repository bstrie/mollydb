/*-------------------------------------------------------------------------
 *
 * mdb_attribute.h
 *	  definition of the system "attribute" relation (mdb_attribute)
 *	  along with the relation's initial contents.
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/mdb_attribute.h
 *
 * NOTES
 *	  the genbki.pl script reads this file and generates .bki
 *	  information from the DATA() statements.
 *
 *-------------------------------------------------------------------------
 */
#ifndef MDB_ATTRIBUTE_H
#define MDB_ATTRIBUTE_H

#include "catalog/genbki.h"

/* ----------------
 *		mdb_attribute definition.  cpp turns this into
 *		typedef struct FormData_mdb_attribute
 *
 *		If you change the following, make sure you change the structs for
 *		system attributes in catalog/heap.c also.
 *		You may need to change catalog/genbki.pl as well.
 * ----------------
 */
#define AttributeRelationId  1249
#define AttributeRelation_Rowtype_Id  75

CATALOG(mdb_attribute,1249) BKI_BOOTSTRAP BKI_WITHOUT_OIDS BKI_ROWTYPE_OID(75) BKI_SCHEMA_MACRO
{
	Oid			attrelid;		/* OID of relation containing this attribute */
	NameData	attname;		/* name of attribute */

	/*
	 * atttypid is the OID of the instance in Catalog Class mdb_type that
	 * defines the data type of this attribute (e.g. int4).  Information in
	 * that instance is redundant with the attlen, attbyval, and attalign
	 * attributes of this instance, so they had better match or MollyDB will
	 * fail.
	 */
	Oid			atttypid;

	/*
	 * attstattarget is the target number of statistics datapoints to collect
	 * during VACUUM ANALYZE of this column.  A zero here indicates that we do
	 * not wish to collect any stats about this column. A "-1" here indicates
	 * that no value has been explicitly set for this column, so ANALYZE
	 * should use the default setting.
	 */
	int32		attstattarget;

	/*
	 * attlen is a copy of the typlen field from mdb_type for this attribute.
	 * See atttypid comments above.
	 */
	int16		attlen;

	/*
	 * attnum is the "attribute number" for the attribute:	A value that
	 * uniquely identifies this attribute within its class. For user
	 * attributes, Attribute numbers are greater than 0 and not greater than
	 * the number of attributes in the class. I.e. if the Class mdb_class says
	 * that Class XYZ has 10 attributes, then the user attribute numbers in
	 * Class mdb_attribute must be 1-10.
	 *
	 * System attributes have attribute numbers less than 0 that are unique
	 * within the class, but not constrained to any particular range.
	 *
	 * Note that (attnum - 1) is often used as the index to an array.
	 */
	int16		attnum;

	/*
	 * attndims is the declared number of dimensions, if an array type,
	 * otherwise zero.
	 */
	int32		attndims;

	/*
	 * fastgetattr() uses attcacheoff to cache byte offsets of attributes in
	 * heap tuples.  The value actually stored in mdb_attribute (-1) indicates
	 * no cached value.  But when we copy these tuples into a tuple
	 * descriptor, we may then update attcacheoff in the copies. This speeds
	 * up the attribute walking process.
	 */
	int32		attcacheoff;

	/*
	 * atttypmod records type-specific data supplied at table creation time
	 * (for example, the max length of a varchar field).  It is passed to
	 * type-specific input and output functions as the third argument. The
	 * value will generally be -1 for types that do not need typmod.
	 */
	int32		atttypmod;

	/*
	 * attbyval is a copy of the typbyval field from mdb_type for this
	 * attribute.  See atttypid comments above.
	 */
	bool		attbyval;

	/*----------
	 * attstorage tells for VARLENA attributes, what the heap access
	 * methods can do to it if a given tuple doesn't fit into a page.
	 * Possible values are
	 *		'p': Value must be stored plain always
	 *		'e': Value can be stored in "secondary" relation (if relation
	 *			 has one, see mdb_class.reltoastrelid)
	 *		'm': Value can be stored compressed inline
	 *		'x': Value can be stored compressed inline or in "secondary"
	 * Note that 'm' fields can also be moved out to secondary storage,
	 * but only as a last resort ('e' and 'x' fields are moved first).
	 *----------
	 */
	char		attstorage;

	/*
	 * attalign is a copy of the typalign field from mdb_type for this
	 * attribute.  See atttypid comments above.
	 */
	char		attalign;

	/* This flag represents the "NOT NULL" constraint */
	bool		attnotnull;

	/* Has DEFAULT value or not */
	bool		atthasdef;

	/* Is dropped (ie, logically invisible) or not */
	bool		attisdropped;

	/*
	 * This flag specifies whether this column has ever had a local
	 * definition.  It is set for normal non-inherited columns, but also for
	 * columns that are inherited from parents if also explicitly listed in
	 * CREATE TABLE INHERITS.  It is also set when inheritance is removed from
	 * a table with ALTER TABLE NO INHERIT.  If the flag is set, the column is
	 * not dropped by a parent's DROP COLUMN even if this causes the column's
	 * attinhcount to become zero.
	 */
	bool		attislocal;

	/* Number of times inherited from direct parent relation(s) */
	int32		attinhcount;

	/* attribute's collation */
	Oid			attcollation;

#ifdef CATALOG_VARLEN			/* variable-length fields start here */
	/* NOTE: The following fields are not present in tuple descriptors. */

	/* Column-level access permissions */
	aclitem		attacl[1];

	/* Column-level options */
	text		attoptions[1];

	/* Column-level FDW options */
	text		attfdwoptions[1];
#endif
} FormData_mdb_attribute;

/*
 * ATTRIBUTE_FIXED_PART_SIZE is the size of the fixed-layout,
 * guaranteed-not-null part of a mdb_attribute row.  This is in fact as much
 * of the row as gets copied into tuple descriptors, so don't expect you
 * can access fields beyond attcollation except in a real tuple!
 */
#define ATTRIBUTE_FIXED_PART_SIZE \
	(offsetof(FormData_mdb_attribute,attcollation) + sizeof(Oid))

/* ----------------
 *		Form_mdb_attribute corresponds to a pointer to a tuple with
 *		the format of mdb_attribute relation.
 * ----------------
 */
typedef FormData_mdb_attribute *Form_mdb_attribute;

/* ----------------
 *		compiler constants for mdb_attribute
 * ----------------
 */

#define Natts_mdb_attribute				21
#define Anum_mdb_attribute_attrelid		1
#define Anum_mdb_attribute_attname		2
#define Anum_mdb_attribute_atttypid		3
#define Anum_mdb_attribute_attstattarget 4
#define Anum_mdb_attribute_attlen		5
#define Anum_mdb_attribute_attnum		6
#define Anum_mdb_attribute_attndims		7
#define Anum_mdb_attribute_attcacheoff	8
#define Anum_mdb_attribute_atttypmod		9
#define Anum_mdb_attribute_attbyval		10
#define Anum_mdb_attribute_attstorage	11
#define Anum_mdb_attribute_attalign		12
#define Anum_mdb_attribute_attnotnull	13
#define Anum_mdb_attribute_atthasdef		14
#define Anum_mdb_attribute_attisdropped	15
#define Anum_mdb_attribute_attislocal	16
#define Anum_mdb_attribute_attinhcount	17
#define Anum_mdb_attribute_attcollation	18
#define Anum_mdb_attribute_attacl		19
#define Anum_mdb_attribute_attoptions	20
#define Anum_mdb_attribute_attfdwoptions 21


/* ----------------
 *		initial contents of mdb_attribute
 *
 * The initial contents of mdb_attribute are generated at compile time by
 * genbki.pl.  Only "bootstrapped" relations need be included.
 * ----------------
 */

#endif   /* MDB_ATTRIBUTE_H */
