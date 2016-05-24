/*-------------------------------------------------------------------------
 *
 *	  ASCII <--> UTF8
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *	  src/backend/utils/mb/conversion_procs/utf8_and_ascii/utf8_and_ascii.c
 *
 *-------------------------------------------------------------------------
 */

#include "mollydb.h"
#include "fmgr.h"
#include "mb/mdb_wchar.h"

MDB_MODULE_MAGIC;

MDB_FUNCTION_INFO_V1(ascii_to_utf8);
MDB_FUNCTION_INFO_V1(utf8_to_ascii);

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
ascii_to_utf8(MDB_FUNCTION_ARGS)
{
	unsigned char *src = (unsigned char *) MDB_GETARG_CSTRING(2);
	unsigned char *dest = (unsigned char *) MDB_GETARG_CSTRING(3);
	int			len = MDB_GETARG_INT32(4);

	CHECK_ENCODING_CONVERSION_ARGS(MDB_SQL_ASCII, MDB_UTF8);

	/* this looks wrong, but basically we're just rejecting high-bit-set */
	mdb_ascii2mic(src, dest, len);

	MDB_RETURN_VOID();
}

Datum
utf8_to_ascii(MDB_FUNCTION_ARGS)
{
	unsigned char *src = (unsigned char *) MDB_GETARG_CSTRING(2);
	unsigned char *dest = (unsigned char *) MDB_GETARG_CSTRING(3);
	int			len = MDB_GETARG_INT32(4);

	CHECK_ENCODING_CONVERSION_ARGS(MDB_UTF8, MDB_SQL_ASCII);

	/* this looks wrong, but basically we're just rejecting high-bit-set */
	mdb_mic2ascii(src, dest, len);

	MDB_RETURN_VOID();
}
