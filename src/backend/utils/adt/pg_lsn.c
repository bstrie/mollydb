/*-------------------------------------------------------------------------
 *
 * mdb_lsn.c
 *	  Operations for the mdb_lsn datatype.
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *	  src/backend/utils/adt/mdb_lsn.c
 *
 *-------------------------------------------------------------------------
 */
#include "mollydb.h"

#include "access/hash.h"
#include "funcapi.h"
#include "libpq/pqformat.h"
#include "utils/builtins.h"
#include "utils/mdb_lsn.h"

#define MAXMDB_LSNLEN			17
#define MAXMDB_LSNCOMPONENT	8

/*----------------------------------------------------------
 * Formatting and conversion routines.
 *---------------------------------------------------------*/

Datum
mdb_lsn_in(MDB_FUNCTION_ARGS)
{
	char	   *str = MDB_GETARG_CSTRING(0);
	int			len1,
				len2;
	uint32		id,
				off;
	XLogRecPtr	result;

	/* Sanity check input format. */
	len1 = strspn(str, "0123456789abcdefABCDEF");
	if (len1 < 1 || len1 > MAXMDB_LSNCOMPONENT || str[len1] != '/')
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
			   errmsg("invalid input syntax for type mdb_lsn: \"%s\"", str)));
	len2 = strspn(str + len1 + 1, "0123456789abcdefABCDEF");
	if (len2 < 1 || len2 > MAXMDB_LSNCOMPONENT || str[len1 + 1 + len2] != '\0')
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
			   errmsg("invalid input syntax for type mdb_lsn: \"%s\"", str)));

	/* Decode result. */
	id = (uint32) strtoul(str, NULL, 16);
	off = (uint32) strtoul(str + len1 + 1, NULL, 16);
	result = ((uint64) id << 32) | off;

	MDB_RETURN_LSN(result);
}

Datum
mdb_lsn_out(MDB_FUNCTION_ARGS)
{
	XLogRecPtr	lsn = MDB_GETARG_LSN(0);
	char		buf[MAXMDB_LSNLEN + 1];
	char	   *result;
	uint32		id,
				off;

	/* Decode ID and offset */
	id = (uint32) (lsn >> 32);
	off = (uint32) lsn;

	snprintf(buf, sizeof buf, "%X/%X", id, off);
	result = pstrdup(buf);
	MDB_RETURN_CSTRING(result);
}

Datum
mdb_lsn_recv(MDB_FUNCTION_ARGS)
{
	StringInfo	buf = (StringInfo) MDB_GETARG_POINTER(0);
	XLogRecPtr	result;

	result = pq_getmsgint64(buf);
	MDB_RETURN_LSN(result);
}

Datum
mdb_lsn_send(MDB_FUNCTION_ARGS)
{
	XLogRecPtr	lsn = MDB_GETARG_LSN(0);
	StringInfoData buf;

	pq_begintypsend(&buf);
	pq_sendint64(&buf, lsn);
	MDB_RETURN_BYTEA_P(pq_endtypsend(&buf));
}


/*----------------------------------------------------------
 *	Operators for MollyDB LSNs
 *---------------------------------------------------------*/

Datum
mdb_lsn_eq(MDB_FUNCTION_ARGS)
{
	XLogRecPtr	lsn1 = MDB_GETARG_LSN(0);
	XLogRecPtr	lsn2 = MDB_GETARG_LSN(1);

	MDB_RETURN_BOOL(lsn1 == lsn2);
}

Datum
mdb_lsn_ne(MDB_FUNCTION_ARGS)
{
	XLogRecPtr	lsn1 = MDB_GETARG_LSN(0);
	XLogRecPtr	lsn2 = MDB_GETARG_LSN(1);

	MDB_RETURN_BOOL(lsn1 != lsn2);
}

Datum
mdb_lsn_lt(MDB_FUNCTION_ARGS)
{
	XLogRecPtr	lsn1 = MDB_GETARG_LSN(0);
	XLogRecPtr	lsn2 = MDB_GETARG_LSN(1);

	MDB_RETURN_BOOL(lsn1 < lsn2);
}

Datum
mdb_lsn_gt(MDB_FUNCTION_ARGS)
{
	XLogRecPtr	lsn1 = MDB_GETARG_LSN(0);
	XLogRecPtr	lsn2 = MDB_GETARG_LSN(1);

	MDB_RETURN_BOOL(lsn1 > lsn2);
}

Datum
mdb_lsn_le(MDB_FUNCTION_ARGS)
{
	XLogRecPtr	lsn1 = MDB_GETARG_LSN(0);
	XLogRecPtr	lsn2 = MDB_GETARG_LSN(1);

	MDB_RETURN_BOOL(lsn1 <= lsn2);
}

Datum
mdb_lsn_ge(MDB_FUNCTION_ARGS)
{
	XLogRecPtr	lsn1 = MDB_GETARG_LSN(0);
	XLogRecPtr	lsn2 = MDB_GETARG_LSN(1);

	MDB_RETURN_BOOL(lsn1 >= lsn2);
}

/* btree index opclass support */
Datum
mdb_lsn_cmp(MDB_FUNCTION_ARGS)
{
	XLogRecPtr	a = MDB_GETARG_LSN(0);
	XLogRecPtr	b = MDB_GETARG_LSN(1);

	if (a > b)
		MDB_RETURN_INT32(1);
	else if (a == b)
		MDB_RETURN_INT32(0);
	else
		MDB_RETURN_INT32(-1);
}

/* hash index opclass support */
Datum
mdb_lsn_hash(MDB_FUNCTION_ARGS)
{
	/* We can use hashint8 directly */
	return hashint8(fcinfo);
}


/*----------------------------------------------------------
 *	Arithmetic operators on MollyDB LSNs.
 *---------------------------------------------------------*/

Datum
mdb_lsn_mi(MDB_FUNCTION_ARGS)
{
	XLogRecPtr	lsn1 = MDB_GETARG_LSN(0);
	XLogRecPtr	lsn2 = MDB_GETARG_LSN(1);
	char		buf[256];
	Datum		result;

	/* Output could be as large as plus or minus 2^63 - 1. */
	if (lsn1 < lsn2)
		snprintf(buf, sizeof buf, "-" UINT64_FORMAT, lsn2 - lsn1);
	else
		snprintf(buf, sizeof buf, UINT64_FORMAT, lsn1 - lsn2);

	/* Convert to numeric. */
	result = DirectFunctionCall3(numeric_in,
								 CStringGetDatum(buf),
								 ObjectIdGetDatum(0),
								 Int32GetDatum(-1));

	return result;
}
