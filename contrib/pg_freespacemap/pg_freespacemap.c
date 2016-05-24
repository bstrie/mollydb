/*-------------------------------------------------------------------------
 *
 * mdb_freespacemap.c
 *	  display contents of a free space map
 *
 *	  contrib/mdb_freespacemap/mdb_freespacemap.c
 *-------------------------------------------------------------------------
 */
#include "mollydb.h"

#include "funcapi.h"
#include "storage/freespace.h"

MDB_MODULE_MAGIC;

/*
 * Returns the amount of free space on a given page, according to the
 * free space map.
 */
MDB_FUNCTION_INFO_V1(mdb_freespace);

Datum
mdb_freespace(MDB_FUNCTION_ARGS)
{
	Oid			relid = MDB_GETARG_OID(0);
	int64		blkno = MDB_GETARG_INT64(1);
	int16		freespace;
	Relation	rel;

	rel = relation_open(relid, AccessShareLock);

	if (blkno < 0 || blkno > MaxBlockNumber)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				 errmsg("invalid block number")));

	freespace = GetRecordedFreeSpace(rel, blkno);

	relation_close(rel, AccessShareLock);
	MDB_RETURN_INT16(freespace);
}
