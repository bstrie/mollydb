/*-------------------------------------------------------------------------
 *
 *	  ASCII and MULE_INTERNAL
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *	  src/backend/utils/mb/conversion_procs/ascii_and_mic/ascii_and_mic.c
 *
 *-------------------------------------------------------------------------
 */

#include "mollydb.h"
#include "fmgr.h"
#include "mb/mdb_wchar.h"

MDB_MODULE_MAGIC;

MDB_FUNCTION_INFO_V1(ascii_to_mic);
MDB_FUNCTION_INFO_V1(mic_to_ascii);

/* ----------
 * conv_proc(
 *		INTEGER,	-- source encoding id
 *		INTEGER,	-- destination encoding id
 *		CSTRING,	-- source string (null terminated C string)
 *		CSTRING,	-- destination string (null terminated C string)
 *		INTEGER		-- source string length
 * ) returns VOID;
 * ----------
 */

Datum
ascii_to_mic(MDB_FUNCTION_ARGS)
{
	unsigned char *src = (unsigned char *) MDB_GETARG_CSTRING(2);
	unsigned char *dest = (unsigned char *) MDB_GETARG_CSTRING(3);
	int			len = MDB_GETARG_INT32(4);

	CHECK_ENCODING_CONVERSION_ARGS(MDB_SQL_ASCII, MDB_MULE_INTERNAL);

	mdb_ascii2mic(src, dest, len);

	MDB_RETURN_VOID();
}

Datum
mic_to_ascii(MDB_FUNCTION_ARGS)
{
	unsigned char *src = (unsigned char *) MDB_GETARG_CSTRING(2);
	unsigned char *dest = (unsigned char *) MDB_GETARG_CSTRING(3);
	int			len = MDB_GETARG_INT32(4);

	CHECK_ENCODING_CONVERSION_ARGS(MDB_MULE_INTERNAL, MDB_SQL_ASCII);

	mdb_mic2ascii(src, dest, len);

	MDB_RETURN_VOID();
}
