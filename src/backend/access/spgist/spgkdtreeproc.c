/*-------------------------------------------------------------------------
 *
 * spgkdtreeproc.c
 *	  implementation of k-d tree over points for SP-GiST
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *			src/backend/access/spgist/spgkdtreeproc.c
 *
 *-------------------------------------------------------------------------
 */

#include "mollydb.h"

#include "access/spgist.h"
#include "access/stratnum.h"
#include "catalog/mdb_type.h"
#include "utils/builtins.h"
#include "utils/geo_decls.h"


Datum
smdb_kd_config(MDB_FUNCTION_ARGS)
{
	/* spgConfigIn *cfgin = (spgConfigIn *) MDB_GETARG_POINTER(0); */
	spgConfigOut *cfg = (spgConfigOut *) MDB_GETARG_POINTER(1);

	cfg->prefixType = FLOAT8OID;
	cfg->labelType = VOIDOID;	/* we don't need node labels */
	cfg->canReturnData = true;
	cfg->longValuesOK = false;
	MDB_RETURN_VOID();
}

static int
getSide(double coord, bool isX, Point *tst)
{
	double		tstcoord = (isX) ? tst->x : tst->y;

	if (coord == tstcoord)
		return 0;
	else if (coord > tstcoord)
		return 1;
	else
		return -1;
}

Datum
smdb_kd_choose(MDB_FUNCTION_ARGS)
{
	spgChooseIn *in = (spgChooseIn *) MDB_GETARG_POINTER(0);
	spgChooseOut *out = (spgChooseOut *) MDB_GETARG_POINTER(1);
	Point	   *inPoint = DatumGetPointP(in->datum);
	double		coord;

	if (in->allTheSame)
		elog(ERROR, "allTheSame should not occur for k-d trees");

	Assert(in->hasPrefix);
	coord = DatumGetFloat8(in->prefixDatum);

	Assert(in->nNodes == 2);

	out->resultType = spgMatchNode;
	out->result.matchNode.nodeN =
		(getSide(coord, in->level % 2, inPoint) > 0) ? 0 : 1;
	out->result.matchNode.levelAdd = 1;
	out->result.matchNode.restDatum = PointPGetDatum(inPoint);

	MDB_RETURN_VOID();
}

typedef struct SortedPoint
{
	Point	   *p;
	int			i;
} SortedPoint;

static int
x_cmp(const void *a, const void *b)
{
	SortedPoint *pa = (SortedPoint *) a;
	SortedPoint *pb = (SortedPoint *) b;

	if (pa->p->x == pb->p->x)
		return 0;
	return (pa->p->x > pb->p->x) ? 1 : -1;
}

static int
y_cmp(const void *a, const void *b)
{
	SortedPoint *pa = (SortedPoint *) a;
	SortedPoint *pb = (SortedPoint *) b;

	if (pa->p->y == pb->p->y)
		return 0;
	return (pa->p->y > pb->p->y) ? 1 : -1;
}


Datum
smdb_kd_picksplit(MDB_FUNCTION_ARGS)
{
	spgPickSplitIn *in = (spgPickSplitIn *) MDB_GETARG_POINTER(0);
	spgPickSplitOut *out = (spgPickSplitOut *) MDB_GETARG_POINTER(1);
	int			i;
	int			middle;
	SortedPoint *sorted;
	double		coord;

	sorted = palloc(sizeof(*sorted) * in->nTuples);
	for (i = 0; i < in->nTuples; i++)
	{
		sorted[i].p = DatumGetPointP(in->datums[i]);
		sorted[i].i = i;
	}

	qsort(sorted, in->nTuples, sizeof(*sorted),
		  (in->level % 2) ? x_cmp : y_cmp);
	middle = in->nTuples >> 1;
	coord = (in->level % 2) ? sorted[middle].p->x : sorted[middle].p->y;

	out->hasPrefix = true;
	out->prefixDatum = Float8GetDatum(coord);

	out->nNodes = 2;
	out->nodeLabels = NULL;		/* we don't need node labels */

	out->mapTuplesToNodes = palloc(sizeof(int) * in->nTuples);
	out->leafTupleDatums = palloc(sizeof(Datum) * in->nTuples);

	/*
	 * Note: points that have coordinates exactly equal to coord may get
	 * classified into either node, depending on where they happen to fall in
	 * the sorted list.  This is okay as long as the inner_consistent function
	 * descends into both sides for such cases.  This is better than the
	 * alternative of trying to have an exact boundary, because it keeps the
	 * tree balanced even when we have many instances of the same point value.
	 * So we should never trigger the allTheSame logic.
	 */
	for (i = 0; i < in->nTuples; i++)
	{
		Point	   *p = sorted[i].p;
		int			n = sorted[i].i;

		out->mapTuplesToNodes[n] = (i < middle) ? 0 : 1;
		out->leafTupleDatums[n] = PointPGetDatum(p);
	}

	MDB_RETURN_VOID();
}

Datum
smdb_kd_inner_consistent(MDB_FUNCTION_ARGS)
{
	spgInnerConsistentIn *in = (spgInnerConsistentIn *) MDB_GETARG_POINTER(0);
	spgInnerConsistentOut *out = (spgInnerConsistentOut *) MDB_GETARG_POINTER(1);
	double		coord;
	int			which;
	int			i;

	Assert(in->hasPrefix);
	coord = DatumGetFloat8(in->prefixDatum);

	if (in->allTheSame)
		elog(ERROR, "allTheSame should not occur for k-d trees");

	Assert(in->nNodes == 2);

	/* "which" is a bitmask of children that satisfy all constraints */
	which = (1 << 1) | (1 << 2);

	for (i = 0; i < in->nkeys; i++)
	{
		Point	   *query = DatumGetPointP(in->scankeys[i].sk_argument);
		BOX		   *boxQuery;

		switch (in->scankeys[i].sk_strategy)
		{
			case RTLeftStrategyNumber:
				if ((in->level % 2) != 0 && FPlt(query->x, coord))
					which &= (1 << 1);
				break;
			case RTRightStrategyNumber:
				if ((in->level % 2) != 0 && FPgt(query->x, coord))
					which &= (1 << 2);
				break;
			case RTSameStrategyNumber:
				if ((in->level % 2) != 0)
				{
					if (FPlt(query->x, coord))
						which &= (1 << 1);
					else if (FPgt(query->x, coord))
						which &= (1 << 2);
				}
				else
				{
					if (FPlt(query->y, coord))
						which &= (1 << 1);
					else if (FPgt(query->y, coord))
						which &= (1 << 2);
				}
				break;
			case RTBelowStrategyNumber:
				if ((in->level % 2) == 0 && FPlt(query->y, coord))
					which &= (1 << 1);
				break;
			case RTAboveStrategyNumber:
				if ((in->level % 2) == 0 && FPgt(query->y, coord))
					which &= (1 << 2);
				break;
			case RTContainedByStrategyNumber:

				/*
				 * For this operator, the query is a box not a point.  We
				 * cheat to the extent of assuming that DatumGetPointP won't
				 * do anything that would be bad for a pointer-to-box.
				 */
				boxQuery = DatumGetBoxP(in->scankeys[i].sk_argument);

				if ((in->level % 2) != 0)
				{
					if (FPlt(boxQuery->high.x, coord))
						which &= (1 << 1);
					else if (FPgt(boxQuery->low.x, coord))
						which &= (1 << 2);
				}
				else
				{
					if (FPlt(boxQuery->high.y, coord))
						which &= (1 << 1);
					else if (FPgt(boxQuery->low.y, coord))
						which &= (1 << 2);
				}
				break;
			default:
				elog(ERROR, "unrecognized strategy number: %d",
					 in->scankeys[i].sk_strategy);
				break;
		}

		if (which == 0)
			break;				/* no need to consider remaining conditions */
	}

	/* We must descend into the children identified by which */
	out->nodeNumbers = (int *) palloc(sizeof(int) * 2);
	out->nNodes = 0;
	for (i = 1; i <= 2; i++)
	{
		if (which & (1 << i))
			out->nodeNumbers[out->nNodes++] = i - 1;
	}

	/* Set up level increments, too */
	out->levelAdds = (int *) palloc(sizeof(int) * 2);
	out->levelAdds[0] = 1;
	out->levelAdds[1] = 1;

	MDB_RETURN_VOID();
}

/*
 * smdb_kd_leaf_consistent() is the same as smdb_quad_leaf_consistent(),
 * since we support the same operators and the same leaf data type.
 * So we just borrow that function.
 */
