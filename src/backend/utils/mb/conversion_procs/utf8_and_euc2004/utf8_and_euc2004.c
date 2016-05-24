/*-------------------------------------------------------------------------
 *
 *	  EUC_JIS_2004 <--> UTF8
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *	  src/backend/utils/mb/conversion_procs/utf8_and_euc2004/utf8_and_euc2004.c
 *
 *-------------------------------------------------------------------------
 */

#include "mollydb.h"
#include "fmgr.h"
#include "mb/mdb_wchar.h"
#include "../../Unicode/euc_jis_2004_to_utf8.map"
#include "../../Unicode/utf8_to_euc_jis_2004.map"
#include "../../Unicode/euc_jis_2004_to_utf8_combined.map"
#include "../../Unicode/utf8_to_euc_jis_2004_combined.map"

MDB_MODULE_MAGIC;

MDB_FUNCTION_INFO_V1(euc_jis_2004_to_utf8);
MDB_FUNCTION_INFO_V1(utf8_to_euc_jis_2004);

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
euc_jis_2004_to_utf8(MDB_FUNCTION_ARGS)
{
	unsigned char *src = (unsigned char *) MDB_GETARG_CSTRING(2);
	unsigned char *dest = (unsigned char *) MDB_GETARG_CSTRING(3);
	int			len = MDB_GETARG_INT32(4);

	CHECK_ENCODING_CONVERSION_ARGS(MDB_EUC_JIS_2004, MDB_UTF8);

	LocalToUtf(src, len, dest,
			   LUmapEUC_JIS_2004, lengthof(LUmapEUC_JIS_2004),
			LUmapEUC_JIS_2004_combined, lengthof(LUmapEUC_JIS_2004_combined),
			   NULL,
			   MDB_EUC_JIS_2004);

	MDB_RETURN_VOID();
}

Datum
utf8_to_euc_jis_2004(MDB_FUNCTION_ARGS)
{
	unsigned char *src = (unsigned char *) MDB_GETARG_CSTRING(2);
	unsigned char *dest = (unsigned char *) MDB_GETARG_CSTRING(3);
	int			len = MDB_GETARG_INT32(4);

	CHECK_ENCODING_CONVERSION_ARGS(MDB_UTF8, MDB_EUC_JIS_2004);

	UtfToLocal(src, len, dest,
			   ULmapEUC_JIS_2004, lengthof(ULmapEUC_JIS_2004),
			ULmapEUC_JIS_2004_combined, lengthof(ULmapEUC_JIS_2004_combined),
			   NULL,
			   MDB_EUC_JIS_2004);

	MDB_RETURN_VOID();
}
