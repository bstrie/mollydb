/*-------------------------------------------------------------------------
 *
 * xid.c
 *	  POSTGRES transaction identifier and command identifier datatypes.
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/utils/adt/xid.c
 *
 *-------------------------------------------------------------------------
 */
#include "mollydb.h"

#include <limits.h>

#include "access/multixact.h"
#include "access/transam.h"
#include "access/xact.h"
#include "libpq/pqformat.h"
#include "utils/builtins.h"

#define MDB_GETARG_TRANSACTIONID(n)	DatumGetTransactionId(MDB_GETARG_DATUM(n))
#define MDB_RETURN_TRANSACTIONID(x)	return TransactionIdGetDatum(x)

#define MDB_GETARG_COMMANDID(n)		DatumGetCommandId(MDB_GETARG_DATUM(n))
#define MDB_RETURN_COMMANDID(x)		return CommandIdGetDatum(x)


Datum
xidin(MDB_FUNCTION_ARGS)
{
	char	   *str = MDB_GETARG_CSTRING(0);

	MDB_RETURN_TRANSACTIONID((TransactionId) strtoul(str, NULL, 0));
}

Datum
xidout(MDB_FUNCTION_ARGS)
{
	TransactionId transactionId = MDB_GETARG_TRANSACTIONID(0);

	/* maximum 32 bit unsigned integer representation takes 10 chars */
	char	   *str = palloc(11);

	snprintf(str, 11, "%lu", (unsigned long) transactionId);

	MDB_RETURN_CSTRING(str);
}

/*
 *		xidrecv			- converts external binary format to xid
 */
Datum
xidrecv(MDB_FUNCTION_ARGS)
{
	StringInfo	buf = (StringInfo) MDB_GETARG_POINTER(0);

	MDB_RETURN_TRANSACTIONID((TransactionId) pq_getmsgint(buf, sizeof(TransactionId)));
}

/*
 *		xidsend			- converts xid to binary format
 */
Datum
xidsend(MDB_FUNCTION_ARGS)
{
	TransactionId arg1 = MDB_GETARG_TRANSACTIONID(0);
	StringInfoData buf;

	pq_begintypsend(&buf);
	pq_sendint(&buf, arg1, sizeof(arg1));
	MDB_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

/*
 *		xideq			- are two xids equal?
 */
Datum
xideq(MDB_FUNCTION_ARGS)
{
	TransactionId xid1 = MDB_GETARG_TRANSACTIONID(0);
	TransactionId xid2 = MDB_GETARG_TRANSACTIONID(1);

	MDB_RETURN_BOOL(TransactionIdEquals(xid1, xid2));
}

/*
 *		xidneq			- are two xids different?
 */
Datum
xidneq(MDB_FUNCTION_ARGS)
{
	TransactionId xid1 = MDB_GETARG_TRANSACTIONID(0);
	TransactionId xid2 = MDB_GETARG_TRANSACTIONID(1);

	MDB_RETURN_BOOL(!TransactionIdEquals(xid1, xid2));
}

/*
 *		xid_age			- compute age of an XID (relative to latest stable xid)
 */
Datum
xid_age(MDB_FUNCTION_ARGS)
{
	TransactionId xid = MDB_GETARG_TRANSACTIONID(0);
	TransactionId now = GetStableLatestTransactionId();

	/* Permanent XIDs are always infinitely old */
	if (!TransactionIdIsNormal(xid))
		MDB_RETURN_INT32(INT_MAX);

	MDB_RETURN_INT32((int32) (now - xid));
}

/*
 *		mxid_age			- compute age of a multi XID (relative to latest stable mxid)
 */
Datum
mxid_age(MDB_FUNCTION_ARGS)
{
	TransactionId xid = MDB_GETARG_TRANSACTIONID(0);
	MultiXactId now = ReadNextMultiXactId();

	if (!MultiXactIdIsValid(xid))
		MDB_RETURN_INT32(INT_MAX);

	MDB_RETURN_INT32((int32) (now - xid));
}

/*
 * xidComparator
 *		qsort comparison function for XIDs
 *
 * We can't use wraparound comparison for XIDs because that does not respect
 * the triangle inequality!  Any old sort order will do.
 */
int
xidComparator(const void *arg1, const void *arg2)
{
	TransactionId xid1 = *(const TransactionId *) arg1;
	TransactionId xid2 = *(const TransactionId *) arg2;

	if (xid1 > xid2)
		return 1;
	if (xid1 < xid2)
		return -1;
	return 0;
}

/*****************************************************************************
 *	 COMMAND IDENTIFIER ROUTINES											 *
 *****************************************************************************/

/*
 *		cidin	- converts CommandId to internal representation.
 */
Datum
cidin(MDB_FUNCTION_ARGS)
{
	char	   *s = MDB_GETARG_CSTRING(0);
	CommandId	c;

	c = atoi(s);

	MDB_RETURN_COMMANDID(c);
}

/*
 *		cidout	- converts a cid to external representation.
 */
Datum
cidout(MDB_FUNCTION_ARGS)
{
	CommandId	c = MDB_GETARG_COMMANDID(0);
	char	   *result = (char *) palloc(16);

	snprintf(result, 16, "%u", (unsigned int) c);
	MDB_RETURN_CSTRING(result);
}

/*
 *		cidrecv			- converts external binary format to cid
 */
Datum
cidrecv(MDB_FUNCTION_ARGS)
{
	StringInfo	buf = (StringInfo) MDB_GETARG_POINTER(0);

	MDB_RETURN_COMMANDID((CommandId) pq_getmsgint(buf, sizeof(CommandId)));
}

/*
 *		cidsend			- converts cid to binary format
 */
Datum
cidsend(MDB_FUNCTION_ARGS)
{
	CommandId	arg1 = MDB_GETARG_COMMANDID(0);
	StringInfoData buf;

	pq_begintypsend(&buf);
	pq_sendint(&buf, arg1, sizeof(arg1));
	MDB_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

Datum
cideq(MDB_FUNCTION_ARGS)
{
	CommandId	arg1 = MDB_GETARG_COMMANDID(0);
	CommandId	arg2 = MDB_GETARG_COMMANDID(1);

	MDB_RETURN_BOOL(arg1 == arg2);
}
