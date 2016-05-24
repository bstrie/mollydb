/*-------------------------------------------------------------------------
 *
 * mdb_visibility.c
 *	  display visibility map information and page-level visibility bits
 *
 *	  contrib/mdb_visibility/mdb_visibility.c
 *-------------------------------------------------------------------------
 */
#include "mollydb.h"

#include "access/htup_details.h"
#include "access/visibilitymap.h"
#include "catalog/mdb_type.h"
#include "funcapi.h"
#include "miscadmin.h"
#include "storage/bufmgr.h"
#include "utils/rel.h"

MDB_MODULE_MAGIC;

typedef struct vbits
{
	BlockNumber	next;
	BlockNumber	count;
	uint8		bits[FLEXIBLE_ARRAY_MEMBER];
} vbits;

MDB_FUNCTION_INFO_V1(mdb_visibility_map);
MDB_FUNCTION_INFO_V1(mdb_visibility_map_rel);
MDB_FUNCTION_INFO_V1(mdb_visibility);
MDB_FUNCTION_INFO_V1(mdb_visibility_rel);
MDB_FUNCTION_INFO_V1(mdb_visibility_map_summary);

static TupleDesc mdb_visibility_tupdesc(bool include_blkno, bool include_pd);
static vbits *collect_visibility_data(Oid relid, bool include_pd);

/*
 * Visibility map information for a single block of a relation.
 */
Datum
mdb_visibility_map(MDB_FUNCTION_ARGS)
{
	Oid			relid = MDB_GETARG_OID(0);
	int64		blkno = MDB_GETARG_INT64(1);
	int32		mapbits;
	Relation	rel;
	Buffer		vmbuffer = InvalidBuffer;
	TupleDesc	tupdesc;
	Datum		values[2];
	bool		nulls[2];

	rel = relation_open(relid, AccessShareLock);

	if (blkno < 0 || blkno > MaxBlockNumber)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				 errmsg("invalid block number")));

	tupdesc = mdb_visibility_tupdesc(false, false);
	MemSet(nulls, 0, sizeof(nulls));

	mapbits = (int32) visibilitymap_get_status(rel, blkno, &vmbuffer);
	if (vmbuffer != InvalidBuffer)
		ReleaseBuffer(vmbuffer);
	values[0] = BoolGetDatum((mapbits & VISIBILITYMAP_ALL_VISIBLE) != 0);
	values[1] = BoolGetDatum((mapbits & VISIBILITYMAP_ALL_FROZEN) != 0);

	relation_close(rel, AccessShareLock);

	MDB_RETURN_DATUM(HeapTupleGetDatum(heap_form_tuple(tupdesc, values, nulls)));
}

/*
 * Visibility map information for a single block of a relation, plus the
 * page-level information for the same block.
 */
Datum
mdb_visibility(MDB_FUNCTION_ARGS)
{
	Oid			relid = MDB_GETARG_OID(0);
	int64		blkno = MDB_GETARG_INT64(1);
	int32		mapbits;
	Relation	rel;
	Buffer		vmbuffer = InvalidBuffer;
	Buffer		buffer;
	Page		page;
	TupleDesc	tupdesc;
	Datum		values[3];
	bool		nulls[3];

	rel = relation_open(relid, AccessShareLock);

	if (blkno < 0 || blkno > MaxBlockNumber)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				 errmsg("invalid block number")));

	tupdesc = mdb_visibility_tupdesc(false, true);
	MemSet(nulls, 0, sizeof(nulls));

	mapbits = (int32) visibilitymap_get_status(rel, blkno, &vmbuffer);
	if (vmbuffer != InvalidBuffer)
		ReleaseBuffer(vmbuffer);
	values[0] = BoolGetDatum((mapbits & VISIBILITYMAP_ALL_VISIBLE) != 0);
	values[1] = BoolGetDatum((mapbits & VISIBILITYMAP_ALL_FROZEN) != 0);

	buffer = ReadBuffer(rel, blkno);
	LockBuffer(buffer, BUFFER_LOCK_SHARE);

	page = BufferGetPage(buffer);
	values[2] = BoolGetDatum(PageIsAllVisible(page));

	UnlockReleaseBuffer(buffer);

	relation_close(rel, AccessShareLock);

	MDB_RETURN_DATUM(HeapTupleGetDatum(heap_form_tuple(tupdesc, values, nulls)));
}

/*
 * Visibility map information for every block in a relation.
 */
Datum
mdb_visibility_map_rel(MDB_FUNCTION_ARGS)
{
	FuncCallContext *funcctx;
	vbits	   *info;

	if (SRF_IS_FIRSTCALL())
	{
		Oid			relid = MDB_GETARG_OID(0);
		MemoryContext	oldcontext;

		funcctx = SRF_FIRSTCALL_INIT();
		oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);
		funcctx->tuple_desc = mdb_visibility_tupdesc(true, false);
		funcctx->user_fctx = collect_visibility_data(relid, false);
		MemoryContextSwitchTo(oldcontext);
	}

	funcctx = SRF_PERCALL_SETUP();
	info = (vbits *) funcctx->user_fctx;

	if (info->next < info->count)
	{
		Datum		values[3];
		bool		nulls[3];
		HeapTuple	tuple;

		MemSet(nulls, 0, sizeof(nulls));
		values[0] = Int64GetDatum(info->next);
		values[1] = BoolGetDatum((info->bits[info->next] & (1 << 0)) != 0);
		values[2] = BoolGetDatum((info->bits[info->next] & (1 << 1)) != 0);
		info->next++;

		tuple = heap_form_tuple(funcctx->tuple_desc, values, nulls);
		SRF_RETURN_NEXT(funcctx, HeapTupleGetDatum(tuple));
	}

	SRF_RETURN_DONE(funcctx);
}

/*
 * Visibility map information for every block in a relation, plus the page
 * level information for each block.
 */
Datum
mdb_visibility_rel(MDB_FUNCTION_ARGS)
{
	FuncCallContext *funcctx;
	vbits	   *info;

	if (SRF_IS_FIRSTCALL())
	{
		Oid			relid = MDB_GETARG_OID(0);
		MemoryContext	oldcontext;

		funcctx = SRF_FIRSTCALL_INIT();
		oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);
		funcctx->tuple_desc = mdb_visibility_tupdesc(true, true);
		funcctx->user_fctx = collect_visibility_data(relid, true);
		MemoryContextSwitchTo(oldcontext);
	}

	funcctx = SRF_PERCALL_SETUP();
	info = (vbits *) funcctx->user_fctx;

	if (info->next < info->count)
	{
		Datum		values[4];
		bool		nulls[4];
		HeapTuple	tuple;

		MemSet(nulls, 0, sizeof(nulls));
		values[0] = Int64GetDatum(info->next);
		values[1] = BoolGetDatum((info->bits[info->next] & (1 << 0)) != 0);
		values[2] = BoolGetDatum((info->bits[info->next] & (1 << 1)) != 0);
		values[3] = BoolGetDatum((info->bits[info->next] & (1 << 2)) != 0);
		info->next++;

		tuple = heap_form_tuple(funcctx->tuple_desc, values, nulls);
		SRF_RETURN_NEXT(funcctx, HeapTupleGetDatum(tuple));
	}

	SRF_RETURN_DONE(funcctx);
}

/*
 * Count the number of all-visible and all-frozen pages in the visibility
 * map for a particular relation.
 */
Datum
mdb_visibility_map_summary(MDB_FUNCTION_ARGS)
{
	Oid			relid = MDB_GETARG_OID(0);
	Relation	rel;
	BlockNumber	nblocks;
	BlockNumber	blkno;
	Buffer		vmbuffer = InvalidBuffer;
	int64		all_visible = 0;
	int64		all_frozen = 0;
	TupleDesc	tupdesc;
	Datum		values[2];
	bool		nulls[2];

	rel = relation_open(relid, AccessShareLock);
	nblocks = RelationGetNumberOfBlocks(rel);

	for (blkno = 0; blkno < nblocks; ++blkno)
	{
		int32		mapbits;

		/* Make sure we are interruptible. */
		CHECK_FOR_INTERRUPTS();

		/* Get map info. */
		mapbits = (int32) visibilitymap_get_status(rel, blkno, &vmbuffer);
		if ((mapbits & VISIBILITYMAP_ALL_VISIBLE) != 0)
			++all_visible;
		if ((mapbits & VISIBILITYMAP_ALL_FROZEN) != 0)
			++all_frozen;
	}

	/* Clean up. */
	if (vmbuffer != InvalidBuffer)
		ReleaseBuffer(vmbuffer);
	relation_close(rel, AccessShareLock);

	tupdesc = CreateTemplateTupleDesc(2, false);
	TupleDescInitEntry(tupdesc, (AttrNumber) 1, "all_visible", INT8OID, -1, 0);
	TupleDescInitEntry(tupdesc, (AttrNumber) 2, "all_frozen", INT8OID, -1, 0);
	tupdesc = BlessTupleDesc(tupdesc);

	MemSet(nulls, 0, sizeof(nulls));
	values[0] = Int64GetDatum(all_visible);
	values[1] = Int64GetDatum(all_frozen);

	MDB_RETURN_DATUM(HeapTupleGetDatum(heap_form_tuple(tupdesc, values, nulls)));
}

/*
 * Helper function to construct whichever TupleDesc we need for a particular
 * call.
 */
static TupleDesc
mdb_visibility_tupdesc(bool include_blkno, bool include_pd)
{
	TupleDesc	tupdesc;
	AttrNumber	maxattr = 2;
	AttrNumber	a = 0;

	if (include_blkno)
		++maxattr;
	if (include_pd)
		++maxattr;
	tupdesc = CreateTemplateTupleDesc(maxattr, false);
	if (include_blkno)
		TupleDescInitEntry(tupdesc, ++a, "blkno", INT8OID, -1, 0);
	TupleDescInitEntry(tupdesc, ++a, "all_visible", BOOLOID, -1, 0);
	TupleDescInitEntry(tupdesc, ++a, "all_frozen", BOOLOID, -1, 0);
	if (include_pd)
		TupleDescInitEntry(tupdesc, ++a, "pd_all_visible", BOOLOID, -1, 0);
	Assert(a == maxattr);

	return BlessTupleDesc(tupdesc);
}

/*
 * Collect visibility data about a relation.
 */
static vbits *
collect_visibility_data(Oid relid, bool include_pd)
{
	Relation	rel;
	BlockNumber	nblocks;
	vbits	   *info;
	BlockNumber	blkno;
	Buffer		vmbuffer = InvalidBuffer;
	BufferAccessStrategy	bstrategy = GetAccessStrategy(BAS_BULKREAD);

	rel = relation_open(relid, AccessShareLock);

	nblocks = RelationGetNumberOfBlocks(rel);
	info = palloc0(offsetof(vbits, bits) + nblocks);
	info->next = 0;
	info->count = nblocks;

	for (blkno = 0; blkno < nblocks; ++blkno)
	{
		int32		mapbits;

		/* Make sure we are interruptible. */
		CHECK_FOR_INTERRUPTS();

		/* Get map info. */
		mapbits = (int32) visibilitymap_get_status(rel, blkno, &vmbuffer);
		if ((mapbits & VISIBILITYMAP_ALL_VISIBLE) != 0)
			info->bits[blkno] |= (1 << 0);
		if ((mapbits & VISIBILITYMAP_ALL_FROZEN) != 0)
			info->bits[blkno] |= (1 << 1);

		/*
		 * Page-level data requires reading every block, so only get it if
		 * the caller needs it.  Use a buffer access strategy, too, to prevent
		 * cache-trashing.
		 */
		if (include_pd)
		{
			Buffer		buffer;
			Page		page;

			buffer = ReadBufferExtended(rel, MAIN_FORKNUM, blkno, RBM_NORMAL,
										bstrategy);
			LockBuffer(buffer, BUFFER_LOCK_SHARE);

			page = BufferGetPage(buffer);
			if (PageIsAllVisible(page))
				info->bits[blkno] |= (1 << 2);

			UnlockReleaseBuffer(buffer);
		}
	}

	/* Clean up. */
	if (vmbuffer != InvalidBuffer)
		ReleaseBuffer(vmbuffer);
	relation_close(rel, AccessShareLock);

	return info;
}
