/*-------------------------------------------------------------------------
 *
 *	  SHIFT_JIS_2004 <--> UTF8
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *	  src/backend/utils/mb/conversion_procs/utf8_and_sjis2004/utf8_and_sjis2004.c
 *
 *-------------------------------------------------------------------------
 */

#include "mollydb.h"
#include "fmgr.h"
#include "mb/mdb_wchar.h"
#include "../../Unicode/shift_jis_2004_to_utf8.map"
#include "../../Unicode/utf8_to_shift_jis_2004.map"
#include "../../Unicode/shift_jis_2004_to_utf8_combined.map"
#include "../../Unicode/utf8_to_shift_jis_2004_combined.map"

MDB_MODULE_MAGIC;

MDB_FUNCTION_INFO_V1(shift_jis_2004_to_utf8);
MDB_FUNCTION_INFO_V1(utf8_to_shift_jis_2004);

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
shift_jis_2004_to_utf8(MDB_FUNCTION_ARGS)
{
	unsigned char *src = (unsigned char *) MDB_GETARG_CSTRING(2);
	unsigned char *dest = (unsigned char *) MDB_GETARG_CSTRING(3);
	int			len = MDB_GETARG_INT32(4);

	CHECK_ENCODING_CONVERSION_ARGS(MDB_SHIFT_JIS_2004, MDB_UTF8);

	LocalToUtf(src, len, dest,
			   LUmapSHIFT_JIS_2004, lengthof(LUmapSHIFT_JIS_2004),
		LUmapSHIFT_JIS_2004_combined, lengthof(LUmapSHIFT_JIS_2004_combined),
			   NULL,
			   MDB_SHIFT_JIS_2004);

	MDB_RETURN_VOID();
}

Datum
utf8_to_shift_jis_2004(MDB_FUNCTION_ARGS)
{
	unsigned char *src = (unsigned char *) MDB_GETARG_CSTRING(2);
	unsigned char *dest = (unsigned char *) MDB_GETARG_CSTRING(3);
	int			len = MDB_GETARG_INT32(4);

	CHECK_ENCODING_CONVERSION_ARGS(MDB_UTF8, MDB_SHIFT_JIS_2004);

	UtfToLocal(src, len, dest,
			   ULmapSHIFT_JIS_2004, lengthof(ULmapSHIFT_JIS_2004),
		ULmapSHIFT_JIS_2004_combined, lengthof(ULmapSHIFT_JIS_2004_combined),
			   NULL,
			   MDB_SHIFT_JIS_2004);

	MDB_RETURN_VOID();
}
