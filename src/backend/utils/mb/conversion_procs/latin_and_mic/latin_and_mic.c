/*-------------------------------------------------------------------------
 *
 *	  LATINn and MULE_INTERNAL
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *	  src/backend/utils/mb/conversion_procs/latin_and_mic/latin_and_mic.c
 *
 *-------------------------------------------------------------------------
 */

#include "mollydb.h"
#include "fmgr.h"
#include "mb/mdb_wchar.h"

MDB_MODULE_MAGIC;

MDB_FUNCTION_INFO_V1(latin1_to_mic);
MDB_FUNCTION_INFO_V1(mic_to_latin1);
MDB_FUNCTION_INFO_V1(latin3_to_mic);
MDB_FUNCTION_INFO_V1(mic_to_latin3);
MDB_FUNCTION_INFO_V1(latin4_to_mic);
MDB_FUNCTION_INFO_V1(mic_to_latin4);

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
latin1_to_mic(MDB_FUNCTION_ARGS)
{
	unsigned char *src = (unsigned char *) MDB_GETARG_CSTRING(2);
	unsigned char *dest = (unsigned char *) MDB_GETARG_CSTRING(3);
	int			len = MDB_GETARG_INT32(4);

	CHECK_ENCODING_CONVERSION_ARGS(MDB_LATIN1, MDB_MULE_INTERNAL);

	latin2mic(src, dest, len, LC_ISO8859_1, MDB_LATIN1);

	MDB_RETURN_VOID();
}

Datum
mic_to_latin1(MDB_FUNCTION_ARGS)
{
	unsigned char *src = (unsigned char *) MDB_GETARG_CSTRING(2);
	unsigned char *dest = (unsigned char *) MDB_GETARG_CSTRING(3);
	int			len = MDB_GETARG_INT32(4);

	CHECK_ENCODING_CONVERSION_ARGS(MDB_MULE_INTERNAL, MDB_LATIN1);

	mic2latin(src, dest, len, LC_ISO8859_1, MDB_LATIN1);

	MDB_RETURN_VOID();
}

Datum
latin3_to_mic(MDB_FUNCTION_ARGS)
{
	unsigned char *src = (unsigned char *) MDB_GETARG_CSTRING(2);
	unsigned char *dest = (unsigned char *) MDB_GETARG_CSTRING(3);
	int			len = MDB_GETARG_INT32(4);

	CHECK_ENCODING_CONVERSION_ARGS(MDB_LATIN3, MDB_MULE_INTERNAL);

	latin2mic(src, dest, len, LC_ISO8859_3, MDB_LATIN3);

	MDB_RETURN_VOID();
}

Datum
mic_to_latin3(MDB_FUNCTION_ARGS)
{
	unsigned char *src = (unsigned char *) MDB_GETARG_CSTRING(2);
	unsigned char *dest = (unsigned char *) MDB_GETARG_CSTRING(3);
	int			len = MDB_GETARG_INT32(4);

	CHECK_ENCODING_CONVERSION_ARGS(MDB_MULE_INTERNAL, MDB_LATIN3);

	mic2latin(src, dest, len, LC_ISO8859_3, MDB_LATIN3);

	MDB_RETURN_VOID();
}

Datum
latin4_to_mic(MDB_FUNCTION_ARGS)
{
	unsigned char *src = (unsigned char *) MDB_GETARG_CSTRING(2);
	unsigned char *dest = (unsigned char *) MDB_GETARG_CSTRING(3);
	int			len = MDB_GETARG_INT32(4);

	CHECK_ENCODING_CONVERSION_ARGS(MDB_LATIN4, MDB_MULE_INTERNAL);

	latin2mic(src, dest, len, LC_ISO8859_4, MDB_LATIN4);

	MDB_RETURN_VOID();
}

Datum
mic_to_latin4(MDB_FUNCTION_ARGS)
{
	unsigned char *src = (unsigned char *) MDB_GETARG_CSTRING(2);
	unsigned char *dest = (unsigned char *) MDB_GETARG_CSTRING(3);
	int			len = MDB_GETARG_INT32(4);

	CHECK_ENCODING_CONVERSION_ARGS(MDB_MULE_INTERNAL, MDB_LATIN4);

	mic2latin(src, dest, len, LC_ISO8859_4, MDB_LATIN4);

	MDB_RETURN_VOID();
}
