/*-------------------------------------------------------------------------
 *
 *	  EUC_CN <--> UTF8
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *	  src/backend/utils/mb/conversion_procs/utf8_and_euc_cn/utf8_and_euc_cn.c
 *
 *-------------------------------------------------------------------------
 */

#include "mollydb.h"
#include "fmgr.h"
#include "mb/mdb_wchar.h"
#include "../../Unicode/euc_cn_to_utf8.map"
#include "../../Unicode/utf8_to_euc_cn.map"

MDB_MODULE_MAGIC;

MDB_FUNCTION_INFO_V1(euc_cn_to_utf8);
MDB_FUNCTION_INFO_V1(utf8_to_euc_cn);

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
euc_cn_to_utf8(MDB_FUNCTION_ARGS)
{
	unsigned char *src = (unsigned char *) MDB_GETARG_CSTRING(2);
	unsigned char *dest = (unsigned char *) MDB_GETARG_CSTRING(3);
	int			len = MDB_GETARG_INT32(4);

	CHECK_ENCODING_CONVERSION_ARGS(MDB_EUC_CN, MDB_UTF8);

	LocalToUtf(src, len, dest,
			   LUmapEUC_CN, lengthof(LUmapEUC_CN),
			   NULL, 0,
			   NULL,
			   MDB_EUC_CN);

	MDB_RETURN_VOID();
}

Datum
utf8_to_euc_cn(MDB_FUNCTION_ARGS)
{
	unsigned char *src = (unsigned char *) MDB_GETARG_CSTRING(2);
	unsigned char *dest = (unsigned char *) MDB_GETARG_CSTRING(3);
	int			len = MDB_GETARG_INT32(4);

	CHECK_ENCODING_CONVERSION_ARGS(MDB_UTF8, MDB_EUC_CN);

	UtfToLocal(src, len, dest,
			   ULmapEUC_CN, lengthof(ULmapEUC_CN),
			   NULL, 0,
			   NULL,
			   MDB_EUC_CN);

	MDB_RETURN_VOID();
}
