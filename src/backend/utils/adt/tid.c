/*-------------------------------------------------------------------------
 *
 * tid.c
 *	  Functions for the built-in type tuple id
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/utils/adt/tid.c
 *
 * NOTES
 *	  input routine largely stolen from boxin().
 *
 *-------------------------------------------------------------------------
 */
#include "mollydb.h"

#include <math.h>
#include <limits.h>

#include "access/heapam.h"
#include "access/sysattr.h"
#include "catalog/namespace.h"
#include "catalog/mdb_type.h"
#include "libpq/pqformat.h"
#include "miscadmin.h"
#include "parser/parsetree.h"
#include "utils/acl.h"
#include "utils/builtins.h"
#include "utils/rel.h"
#include "utils/snapmgr.h"
#include "utils/tqual.h"


#define DatumGetItemPointer(X)	 ((ItemPointer) DatumGetPointer(X))
#define ItemPointerGetDatum(X)	 PointerGetDatum(X)
#define MDB_GETARG_ITEMPOINTER(n) DatumGetItemPointer(MDB_GETARG_DATUM(n))
#define MDB_RETURN_ITEMPOINTER(x) return ItemPointerGetDatum(x)

#define LDELIM			'('
#define RDELIM			')'
#define DELIM			','
#define NTIDARGS		2

/* ----------------------------------------------------------------
 *		tidin
 * ----------------------------------------------------------------
 */
Datum
tidin(MDB_FUNCTION_ARGS)
{
	char	   *str = MDB_GETARG_CSTRING(0);
	char	   *p,
			   *coord[NTIDARGS];
	int			i;
	ItemPointer result;
	BlockNumber blockNumber;
	OffsetNumber offsetNumber;
	char	   *badp;
	int			hold_offset;

	for (i = 0, p = str; *p && i < NTIDARGS && *p != RDELIM; p++)
		if (*p == DELIM || (*p == LDELIM && !i))
			coord[i++] = p + 1;

	if (i < NTIDARGS)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for type tid: \"%s\"",
						str)));

	errno = 0;
	blockNumber = strtoul(coord[0], &badp, 10);
	if (errno || *badp != DELIM)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for type tid: \"%s\"",
						str)));

	hold_offset = strtol(coord[1], &badp, 10);
	if (errno || *badp != RDELIM ||
		hold_offset > USHRT_MAX || hold_offset < 0)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for type tid: \"%s\"",
						str)));

	offsetNumber = hold_offset;

	result = (ItemPointer) palloc(sizeof(ItemPointerData));

	ItemPointerSet(result, blockNumber, offsetNumber);

	MDB_RETURN_ITEMPOINTER(result);
}

/* ----------------------------------------------------------------
 *		tidout
 * ----------------------------------------------------------------
 */
Datum
tidout(MDB_FUNCTION_ARGS)
{
	ItemPointer itemPtr = MDB_GETARG_ITEMPOINTER(0);
	BlockNumber blockNumber;
	OffsetNumber offsetNumber;
	char		buf[32];

	blockNumber = BlockIdGetBlockNumber(&(itemPtr->ip_blkid));
	offsetNumber = itemPtr->ip_posid;

	/* Perhaps someday we should output this as a record. */
	snprintf(buf, sizeof(buf), "(%u,%u)", blockNumber, offsetNumber);

	MDB_RETURN_CSTRING(pstrdup(buf));
}

/*
 *		tidrecv			- converts external binary format to tid
 */
Datum
tidrecv(MDB_FUNCTION_ARGS)
{
	StringInfo	buf = (StringInfo) MDB_GETARG_POINTER(0);
	ItemPointer result;
	BlockNumber blockNumber;
	OffsetNumber offsetNumber;

	blockNumber = pq_getmsgint(buf, sizeof(blockNumber));
	offsetNumber = pq_getmsgint(buf, sizeof(offsetNumber));

	result = (ItemPointer) palloc(sizeof(ItemPointerData));

	ItemPointerSet(result, blockNumber, offsetNumber);

	MDB_RETURN_ITEMPOINTER(result);
}

/*
 *		tidsend			- converts tid to binary format
 */
Datum
tidsend(MDB_FUNCTION_ARGS)
{
	ItemPointer itemPtr = MDB_GETARG_ITEMPOINTER(0);
	BlockId		blockId;
	BlockNumber blockNumber;
	OffsetNumber offsetNumber;
	StringInfoData buf;

	blockId = &(itemPtr->ip_blkid);
	blockNumber = BlockIdGetBlockNumber(blockId);
	offsetNumber = itemPtr->ip_posid;

	pq_begintypsend(&buf);
	pq_sendint(&buf, blockNumber, sizeof(blockNumber));
	pq_sendint(&buf, offsetNumber, sizeof(offsetNumber));
	MDB_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

/*****************************************************************************
 *	 PUBLIC ROUTINES														 *
 *****************************************************************************/

Datum
tideq(MDB_FUNCTION_ARGS)
{
	ItemPointer arg1 = MDB_GETARG_ITEMPOINTER(0);
	ItemPointer arg2 = MDB_GETARG_ITEMPOINTER(1);

	MDB_RETURN_BOOL(ItemPointerCompare(arg1, arg2) == 0);
}

Datum
tidne(MDB_FUNCTION_ARGS)
{
	ItemPointer arg1 = MDB_GETARG_ITEMPOINTER(0);
	ItemPointer arg2 = MDB_GETARG_ITEMPOINTER(1);

	MDB_RETURN_BOOL(ItemPointerCompare(arg1, arg2) != 0);
}

Datum
tidlt(MDB_FUNCTION_ARGS)
{
	ItemPointer arg1 = MDB_GETARG_ITEMPOINTER(0);
	ItemPointer arg2 = MDB_GETARG_ITEMPOINTER(1);

	MDB_RETURN_BOOL(ItemPointerCompare(arg1, arg2) < 0);
}

Datum
tidle(MDB_FUNCTION_ARGS)
{
	ItemPointer arg1 = MDB_GETARG_ITEMPOINTER(0);
	ItemPointer arg2 = MDB_GETARG_ITEMPOINTER(1);

	MDB_RETURN_BOOL(ItemPointerCompare(arg1, arg2) <= 0);
}

Datum
tidgt(MDB_FUNCTION_ARGS)
{
	ItemPointer arg1 = MDB_GETARG_ITEMPOINTER(0);
	ItemPointer arg2 = MDB_GETARG_ITEMPOINTER(1);

	MDB_RETURN_BOOL(ItemPointerCompare(arg1, arg2) > 0);
}

Datum
tidge(MDB_FUNCTION_ARGS)
{
	ItemPointer arg1 = MDB_GETARG_ITEMPOINTER(0);
	ItemPointer arg2 = MDB_GETARG_ITEMPOINTER(1);

	MDB_RETURN_BOOL(ItemPointerCompare(arg1, arg2) >= 0);
}

Datum
bttidcmp(MDB_FUNCTION_ARGS)
{
	ItemPointer arg1 = MDB_GETARG_ITEMPOINTER(0);
	ItemPointer arg2 = MDB_GETARG_ITEMPOINTER(1);

	MDB_RETURN_INT32(ItemPointerCompare(arg1, arg2));
}

Datum
tidlarger(MDB_FUNCTION_ARGS)
{
	ItemPointer arg1 = MDB_GETARG_ITEMPOINTER(0);
	ItemPointer arg2 = MDB_GETARG_ITEMPOINTER(1);

	MDB_RETURN_ITEMPOINTER(ItemPointerCompare(arg1, arg2) >= 0 ? arg1 : arg2);
}

Datum
tidsmaller(MDB_FUNCTION_ARGS)
{
	ItemPointer arg1 = MDB_GETARG_ITEMPOINTER(0);
	ItemPointer arg2 = MDB_GETARG_ITEMPOINTER(1);

	MDB_RETURN_ITEMPOINTER(ItemPointerCompare(arg1, arg2) <= 0 ? arg1 : arg2);
}


/*
 *	Functions to get latest tid of a specified tuple.
 *
 *	Maybe these implementations should be moved to another place
 */

static ItemPointerData Current_last_tid = {{0, 0}, 0};

void
setLastTid(const ItemPointer tid)
{
	Current_last_tid = *tid;
}

/*
 *	Handle CTIDs of views.
 *		CTID should be defined in the view and it must
 *		correspond to the CTID of a base relation.
 */
static Datum
currtid_for_view(Relation viewrel, ItemPointer tid)
{
	TupleDesc	att = RelationGetDescr(viewrel);
	RuleLock   *rulelock;
	RewriteRule *rewrite;
	int			i,
				natts = att->natts,
				tididx = -1;

	for (i = 0; i < natts; i++)
	{
		if (strcmp(NameStr(att->attrs[i]->attname), "ctid") == 0)
		{
			if (att->attrs[i]->atttypid != TIDOID)
				elog(ERROR, "ctid isn't of type TID");
			tididx = i;
			break;
		}
	}
	if (tididx < 0)
		elog(ERROR, "currtid cannot handle views with no CTID");
	rulelock = viewrel->rd_rules;
	if (!rulelock)
		elog(ERROR, "the view has no rules");
	for (i = 0; i < rulelock->numLocks; i++)
	{
		rewrite = rulelock->rules[i];
		if (rewrite->event == CMD_SELECT)
		{
			Query	   *query;
			TargetEntry *tle;

			if (list_length(rewrite->actions) != 1)
				elog(ERROR, "only one select rule is allowed in views");
			query = (Query *) linitial(rewrite->actions);
			tle = get_tle_by_resno(query->targetList, tididx + 1);
			if (tle && tle->expr && IsA(tle->expr, Var))
			{
				Var		   *var = (Var *) tle->expr;
				RangeTblEntry *rte;

				if (!IS_SPECIAL_VARNO(var->varno) &&
					var->varattno == SelfItemPointerAttributeNumber)
				{
					rte = rt_fetch(var->varno, query->rtable);
					if (rte)
					{
						heap_close(viewrel, AccessShareLock);
						return DirectFunctionCall2(currtid_byreloid, ObjectIdGetDatum(rte->relid), PointerGetDatum(tid));
					}
				}
			}
			break;
		}
	}
	elog(ERROR, "currtid cannot handle this view");
	return (Datum) 0;
}

Datum
currtid_byreloid(MDB_FUNCTION_ARGS)
{
	Oid			reloid = MDB_GETARG_OID(0);
	ItemPointer tid = MDB_GETARG_ITEMPOINTER(1);
	ItemPointer result;
	Relation	rel;
	AclResult	aclresult;
	Snapshot	snapshot;

	result = (ItemPointer) palloc(sizeof(ItemPointerData));
	if (!reloid)
	{
		*result = Current_last_tid;
		MDB_RETURN_ITEMPOINTER(result);
	}

	rel = heap_open(reloid, AccessShareLock);

	aclresult = mdb_class_aclcheck(RelationGetRelid(rel), GetUserId(),
								  ACL_SELECT);
	if (aclresult != ACLCHECK_OK)
		aclcheck_error(aclresult, ACL_KIND_CLASS,
					   RelationGetRelationName(rel));

	if (rel->rd_rel->relkind == RELKIND_VIEW)
		return currtid_for_view(rel, tid);

	ItemPointerCopy(tid, result);

	snapshot = RegisterSnapshot(GetLatestSnapshot());
	heap_get_latest_tid(rel, snapshot, result);
	UnregisterSnapshot(snapshot);

	heap_close(rel, AccessShareLock);

	MDB_RETURN_ITEMPOINTER(result);
}

Datum
currtid_byrelname(MDB_FUNCTION_ARGS)
{
	text	   *relname = MDB_GETARG_TEXT_P(0);
	ItemPointer tid = MDB_GETARG_ITEMPOINTER(1);
	ItemPointer result;
	RangeVar   *relrv;
	Relation	rel;
	AclResult	aclresult;
	Snapshot	snapshot;

	relrv = makeRangeVarFromNameList(textToQualifiedNameList(relname));
	rel = heap_openrv(relrv, AccessShareLock);

	aclresult = mdb_class_aclcheck(RelationGetRelid(rel), GetUserId(),
								  ACL_SELECT);
	if (aclresult != ACLCHECK_OK)
		aclcheck_error(aclresult, ACL_KIND_CLASS,
					   RelationGetRelationName(rel));

	if (rel->rd_rel->relkind == RELKIND_VIEW)
		return currtid_for_view(rel, tid);

	result = (ItemPointer) palloc(sizeof(ItemPointerData));
	ItemPointerCopy(tid, result);

	snapshot = RegisterSnapshot(GetLatestSnapshot());
	heap_get_latest_tid(rel, snapshot, result);
	UnregisterSnapshot(snapshot);

	heap_close(rel, AccessShareLock);

	MDB_RETURN_ITEMPOINTER(result);
}
