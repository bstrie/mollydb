/*-------------------------------------------------------------------------
 *
 *	  UTF8 and Cyrillic
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *	  src/backend/utils/mb/conversion_procs/utf8_and_cyrillic/utf8_and_cyrillic.c
 *
 *-------------------------------------------------------------------------
 */

#include "mollydb.h"
#include "fmgr.h"
#include "mb/mdb_wchar.h"
#include "../../Unicode/utf8_to_koi8r.map"
#include "../../Unicode/koi8r_to_utf8.map"
#include "../../Unicode/utf8_to_koi8u.map"
#include "../../Unicode/koi8u_to_utf8.map"

MDB_MODULE_MAGIC;

MDB_FUNCTION_INFO_V1(utf8_to_koi8r);
MDB_FUNCTION_INFO_V1(koi8r_to_utf8);

MDB_FUNCTION_INFO_V1(utf8_to_koi8u);
MDB_FUNCTION_INFO_V1(koi8u_to_utf8);

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
utf8_to_koi8r(MDB_FUNCTION_ARGS)
{
	unsigned char *src = (unsigned char *) MDB_GETARG_CSTRING(2);
	unsigned char *dest = (unsigned char *) MDB_GETARG_CSTRING(3);
	int			len = MDB_GETARG_INT32(4);

	CHECK_ENCODING_CONVERSION_ARGS(MDB_UTF8, MDB_KOI8R);

	UtfToLocal(src, len, dest,
			   ULmapKOI8R, lengthof(ULmapKOI8R),
			   NULL, 0,
			   NULL,
			   MDB_KOI8R);

	MDB_RETURN_VOID();
}

Datum
koi8r_to_utf8(MDB_FUNCTION_ARGS)
{
	unsigned char *src = (unsigned char *) MDB_GETARG_CSTRING(2);
	unsigned char *dest = (unsigned char *) MDB_GETARG_CSTRING(3);
	int			len = MDB_GETARG_INT32(4);

	CHECK_ENCODING_CONVERSION_ARGS(MDB_KOI8R, MDB_UTF8);

	LocalToUtf(src, len, dest,
			   LUmapKOI8R, lengthof(LUmapKOI8R),
			   NULL, 0,
			   NULL,
			   MDB_KOI8R);

	MDB_RETURN_VOID();
}

Datum
utf8_to_koi8u(MDB_FUNCTION_ARGS)
{
	unsigned char *src = (unsigned char *) MDB_GETARG_CSTRING(2);
	unsigned char *dest = (unsigned char *) MDB_GETARG_CSTRING(3);
	int			len = MDB_GETARG_INT32(4);

	CHECK_ENCODING_CONVERSION_ARGS(MDB_UTF8, MDB_KOI8U);

	UtfToLocal(src, len, dest,
			   ULmapKOI8U, lengthof(ULmapKOI8U),
			   NULL, 0,
			   NULL,
			   MDB_KOI8U);

	MDB_RETURN_VOID();
}

Datum
koi8u_to_utf8(MDB_FUNCTION_ARGS)
{
	unsigned char *src = (unsigned char *) MDB_GETARG_CSTRING(2);
	unsigned char *dest = (unsigned char *) MDB_GETARG_CSTRING(3);
	int			len = MDB_GETARG_INT32(4);

	CHECK_ENCODING_CONVERSION_ARGS(MDB_KOI8U, MDB_UTF8);

	LocalToUtf(src, len, dest,
			   LUmapKOI8U, lengthof(LUmapKOI8U),
			   NULL, 0,
			   NULL,
			   MDB_KOI8U);

	MDB_RETURN_VOID();
}
