/*-------------------------------------------------------------------------
 *
 *	  EUC_JP <--> UTF8
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *	  src/backend/utils/mb/conversion_procs/utf8_and_euc_jp/utf8_and_euc_jp.c
 *
 *-------------------------------------------------------------------------
 */

#include "mollydb.h"
#include "fmgr.h"
#include "mb/mdb_wchar.h"
#include "../../Unicode/euc_jp_to_utf8.map"
#include "../../Unicode/utf8_to_euc_jp.map"

MDB_MODULE_MAGIC;

MDB_FUNCTION_INFO_V1(euc_jp_to_utf8);
MDB_FUNCTION_INFO_V1(utf8_to_euc_jp);

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
euc_jp_to_utf8(MDB_FUNCTION_ARGS)
{
	unsigned char *src = (unsigned char *) MDB_GETARG_CSTRING(2);
	unsigned char *dest = (unsigned char *) MDB_GETARG_CSTRING(3);
	int			len = MDB_GETARG_INT32(4);

	CHECK_ENCODING_CONVERSION_ARGS(MDB_EUC_JP, MDB_UTF8);

	LocalToUtf(src, len, dest,
			   LUmapEUC_JP, lengthof(LUmapEUC_JP),
			   NULL, 0,
			   NULL,
			   MDB_EUC_JP);

	MDB_RETURN_VOID();
}

Datum
utf8_to_euc_jp(MDB_FUNCTION_ARGS)
{
	unsigned char *src = (unsigned char *) MDB_GETARG_CSTRING(2);
	unsigned char *dest = (unsigned char *) MDB_GETARG_CSTRING(3);
	int			len = MDB_GETARG_INT32(4);

	CHECK_ENCODING_CONVERSION_ARGS(MDB_UTF8, MDB_EUC_JP);

	UtfToLocal(src, len, dest,
			   ULmapEUC_JP, lengthof(ULmapEUC_JP),
			   NULL, 0,
			   NULL,
			   MDB_EUC_JP);

	MDB_RETURN_VOID();
}
