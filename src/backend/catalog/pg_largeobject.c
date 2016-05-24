/*-------------------------------------------------------------------------
 *
 * mdb_largeobject.c
 *	  routines to support manipulation of the mdb_largeobject relation
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/catalog/mdb_largeobject.c
 *
 *-------------------------------------------------------------------------
 */
#include "mollydb.h"

#include "access/genam.h"
#include "access/heapam.h"
#include "access/htup_details.h"
#include "access/sysattr.h"
#include "catalog/dependency.h"
#include "catalog/indexing.h"
#include "catalog/mdb_largeobject.h"
#include "catalog/mdb_largeobject_metadata.h"
#include "miscadmin.h"
#include "utils/acl.h"
#include "utils/fmgroids.h"
#include "utils/rel.h"
#include "utils/tqual.h"


/*
 * Create a large object having the given LO identifier.
 *
 * We create a new large object by inserting an entry into
 * mdb_largeobject_metadata without any data pages, so that the object
 * will appear to exist with size 0.
 */
Oid
LargeObjectCreate(Oid loid)
{
	Relation	mdb_lo_meta;
	HeapTuple	ntup;
	Oid			loid_new;
	Datum		values[Natts_mdb_largeobject_metadata];
	bool		nulls[Natts_mdb_largeobject_metadata];

	mdb_lo_meta = heap_open(LargeObjectMetadataRelationId,
						   RowExclusiveLock);

	/*
	 * Insert metadata of the largeobject
	 */
	memset(values, 0, sizeof(values));
	memset(nulls, false, sizeof(nulls));

	values[Anum_mdb_largeobject_metadata_lomowner - 1]
		= ObjectIdGetDatum(GetUserId());
	nulls[Anum_mdb_largeobject_metadata_lomacl - 1] = true;

	ntup = heap_form_tuple(RelationGetDescr(mdb_lo_meta),
						   values, nulls);
	if (OidIsValid(loid))
		HeapTupleSetOid(ntup, loid);

	loid_new = simple_heap_insert(mdb_lo_meta, ntup);
	Assert(!OidIsValid(loid) || loid == loid_new);

	CatalogUpdateIndexes(mdb_lo_meta, ntup);

	heap_freetuple(ntup);

	heap_close(mdb_lo_meta, RowExclusiveLock);

	return loid_new;
}

/*
 * Drop a large object having the given LO identifier.  Both the data pages
 * and metadata must be dropped.
 */
void
LargeObjectDrop(Oid loid)
{
	Relation	mdb_lo_meta;
	Relation	mdb_largeobject;
	ScanKeyData skey[1];
	SysScanDesc scan;
	HeapTuple	tuple;

	mdb_lo_meta = heap_open(LargeObjectMetadataRelationId,
						   RowExclusiveLock);

	mdb_largeobject = heap_open(LargeObjectRelationId,
							   RowExclusiveLock);

	/*
	 * Delete an entry from mdb_largeobject_metadata
	 */
	ScanKeyInit(&skey[0],
				ObjectIdAttributeNumber,
				BTEqualStrategyNumber, F_OIDEQ,
				ObjectIdGetDatum(loid));

	scan = systable_beginscan(mdb_lo_meta,
							  LargeObjectMetadataOidIndexId, true,
							  NULL, 1, skey);

	tuple = systable_getnext(scan);
	if (!HeapTupleIsValid(tuple))
		ereport(ERROR,
				(errcode(ERRCODE_UNDEFINED_OBJECT),
				 errmsg("large object %u does not exist", loid)));

	simple_heap_delete(mdb_lo_meta, &tuple->t_self);

	systable_endscan(scan);

	/*
	 * Delete all the associated entries from mdb_largeobject
	 */
	ScanKeyInit(&skey[0],
				Anum_mdb_largeobject_loid,
				BTEqualStrategyNumber, F_OIDEQ,
				ObjectIdGetDatum(loid));

	scan = systable_beginscan(mdb_largeobject,
							  LargeObjectLOidPNIndexId, true,
							  NULL, 1, skey);
	while (HeapTupleIsValid(tuple = systable_getnext(scan)))
	{
		simple_heap_delete(mdb_largeobject, &tuple->t_self);
	}

	systable_endscan(scan);

	heap_close(mdb_largeobject, RowExclusiveLock);

	heap_close(mdb_lo_meta, RowExclusiveLock);
}

/*
 * LargeObjectExists
 *
 * We don't use the system cache for large object metadata, for fear of
 * using too much local memory.
 *
 * This function always scans the system catalog using an up-to-date snapshot,
 * so it should not be used when a large object is opened in read-only mode
 * (because large objects opened in read only mode are supposed to be viewed
 * relative to the caller's snapshot, whereas in read-write mode they are
 * relative to a current snapshot).
 */
bool
LargeObjectExists(Oid loid)
{
	Relation	mdb_lo_meta;
	ScanKeyData skey[1];
	SysScanDesc sd;
	HeapTuple	tuple;
	bool		retval = false;

	ScanKeyInit(&skey[0],
				ObjectIdAttributeNumber,
				BTEqualStrategyNumber, F_OIDEQ,
				ObjectIdGetDatum(loid));

	mdb_lo_meta = heap_open(LargeObjectMetadataRelationId,
						   AccessShareLock);

	sd = systable_beginscan(mdb_lo_meta,
							LargeObjectMetadataOidIndexId, true,
							NULL, 1, skey);

	tuple = systable_getnext(sd);
	if (HeapTupleIsValid(tuple))
		retval = true;

	systable_endscan(sd);

	heap_close(mdb_lo_meta, AccessShareLock);

	return retval;
}
