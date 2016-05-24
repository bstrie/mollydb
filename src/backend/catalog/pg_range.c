/*-------------------------------------------------------------------------
 *
 * mdb_range.c
 *	  routines to support manipulation of the mdb_range relation
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/catalog/mdb_range.c
 *
 *-------------------------------------------------------------------------
 */
#include "mollydb.h"

#include "access/genam.h"
#include "access/heapam.h"
#include "access/htup_details.h"
#include "catalog/dependency.h"
#include "catalog/indexing.h"
#include "catalog/mdb_collation.h"
#include "catalog/mdb_opclass.h"
#include "catalog/mdb_proc.h"
#include "catalog/mdb_range.h"
#include "catalog/mdb_type.h"
#include "utils/fmgroids.h"
#include "utils/rel.h"
#include "utils/tqual.h"


/*
 * RangeCreate
 *		Create an entry in mdb_range.
 */
void
RangeCreate(Oid rangeTypeOid, Oid rangeSubType, Oid rangeCollation,
			Oid rangeSubOpclass, RegProcedure rangeCanonical,
			RegProcedure rangeSubDiff)
{
	Relation	mdb_range;
	Datum		values[Natts_mdb_range];
	bool		nulls[Natts_mdb_range];
	HeapTuple	tup;
	ObjectAddress myself;
	ObjectAddress referenced;

	mdb_range = heap_open(RangeRelationId, RowExclusiveLock);

	memset(nulls, 0, sizeof(nulls));

	values[Anum_mdb_range_rngtypid - 1] = ObjectIdGetDatum(rangeTypeOid);
	values[Anum_mdb_range_rngsubtype - 1] = ObjectIdGetDatum(rangeSubType);
	values[Anum_mdb_range_rngcollation - 1] = ObjectIdGetDatum(rangeCollation);
	values[Anum_mdb_range_rngsubopc - 1] = ObjectIdGetDatum(rangeSubOpclass);
	values[Anum_mdb_range_rngcanonical - 1] = ObjectIdGetDatum(rangeCanonical);
	values[Anum_mdb_range_rngsubdiff - 1] = ObjectIdGetDatum(rangeSubDiff);

	tup = heap_form_tuple(RelationGetDescr(mdb_range), values, nulls);

	simple_heap_insert(mdb_range, tup);
	CatalogUpdateIndexes(mdb_range, tup);
	heap_freetuple(tup);

	/* record type's dependencies on range-related items */

	myself.classId = TypeRelationId;
	myself.objectId = rangeTypeOid;
	myself.objectSubId = 0;

	referenced.classId = TypeRelationId;
	referenced.objectId = rangeSubType;
	referenced.objectSubId = 0;
	recordDependencyOn(&myself, &referenced, DEPENDENCY_NORMAL);

	referenced.classId = OperatorClassRelationId;
	referenced.objectId = rangeSubOpclass;
	referenced.objectSubId = 0;
	recordDependencyOn(&myself, &referenced, DEPENDENCY_NORMAL);

	if (OidIsValid(rangeCollation))
	{
		referenced.classId = CollationRelationId;
		referenced.objectId = rangeCollation;
		referenced.objectSubId = 0;
		recordDependencyOn(&myself, &referenced, DEPENDENCY_NORMAL);
	}

	if (OidIsValid(rangeCanonical))
	{
		referenced.classId = ProcedureRelationId;
		referenced.objectId = rangeCanonical;
		referenced.objectSubId = 0;
		recordDependencyOn(&myself, &referenced, DEPENDENCY_NORMAL);
	}

	if (OidIsValid(rangeSubDiff))
	{
		referenced.classId = ProcedureRelationId;
		referenced.objectId = rangeSubDiff;
		referenced.objectSubId = 0;
		recordDependencyOn(&myself, &referenced, DEPENDENCY_NORMAL);
	}

	heap_close(mdb_range, RowExclusiveLock);
}


/*
 * RangeDelete
 *		Remove the mdb_range entry for the specified type.
 */
void
RangeDelete(Oid rangeTypeOid)
{
	Relation	mdb_range;
	ScanKeyData key[1];
	SysScanDesc scan;
	HeapTuple	tup;

	mdb_range = heap_open(RangeRelationId, RowExclusiveLock);

	ScanKeyInit(&key[0],
				Anum_mdb_range_rngtypid,
				BTEqualStrategyNumber, F_OIDEQ,
				ObjectIdGetDatum(rangeTypeOid));

	scan = systable_beginscan(mdb_range, RangeTypidIndexId, true,
							  NULL, 1, key);

	while (HeapTupleIsValid(tup = systable_getnext(scan)))
	{
		simple_heap_delete(mdb_range, &tup->t_self);
	}

	systable_endscan(scan);

	heap_close(mdb_range, RowExclusiveLock);
}
