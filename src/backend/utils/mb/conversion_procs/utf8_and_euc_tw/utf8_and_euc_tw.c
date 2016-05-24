/*-------------------------------------------------------------------------
 *
 *	  EUC_TW <--> UTF8
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *	  src/backend/utils/mb/conversion_procs/utf8_and_euc_tw/utf8_and_euc_tw.c
 *
 *-------------------------------------------------------------------------
 */

#include "mollydb.h"
#include "fmgr.h"
#include "mb/mdb_wchar.h"
#include "../../Unicode/euc_tw_to_utf8.map"
#include "../../Unicode/utf8_to_euc_tw.map"

MDB_MODULE_MAGIC;

MDB_FUNCTION_INFO_V1(euc_tw_to_utf8);
MDB_FUNCTION_INFO_V1(utf8_to_euc_tw);

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
euc_tw_to_utf8(MDB_FUNCTION_ARGS)
{
	unsigned char *src = (unsigned char *) MDB_GETARG_CSTRING(2);
	unsigned char *dest = (unsigned char *) MDB_GETARG_CSTRING(3);
	int			len = MDB_GETARG_INT32(4);

	CHECK_ENCODING_CONVERSION_ARGS(MDB_EUC_TW, MDB_UTF8);

	LocalToUtf(src, len, dest,
			   LUmapEUC_TW, lengthof(LUmapEUC_TW),
			   NULL, 0,
			   NULL,
			   MDB_EUC_TW);

	MDB_RETURN_VOID();
}

Datum
utf8_to_euc_tw(MDB_FUNCTION_ARGS)
{
	unsigned char *src = (unsigned char *) MDB_GETARG_CSTRING(2);
	unsigned char *dest = (unsigned char *) MDB_GETARG_CSTRING(3);
	int			len = MDB_GETARG_INT32(4);

	CHECK_ENCODING_CONVERSION_ARGS(MDB_UTF8, MDB_EUC_TW);

	UtfToLocal(src, len, dest,
			   ULmapEUC_TW, lengthof(ULmapEUC_TW),
			   NULL, 0,
			   NULL,
			   MDB_EUC_TW);

	MDB_RETURN_VOID();
}
