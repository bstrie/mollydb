/*-------------------------------------------------------------------------
 *
 *	  WIN <--> UTF8
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *	  src/backend/utils/mb/conversion_procs/utf8_and_win/utf8_and_win.c
 *
 *-------------------------------------------------------------------------
 */

#include "mollydb.h"
#include "fmgr.h"
#include "mb/mdb_wchar.h"
#include "../../Unicode/utf8_to_win1250.map"
#include "../../Unicode/utf8_to_win1251.map"
#include "../../Unicode/utf8_to_win1252.map"
#include "../../Unicode/utf8_to_win1253.map"
#include "../../Unicode/utf8_to_win1254.map"
#include "../../Unicode/utf8_to_win1255.map"
#include "../../Unicode/utf8_to_win1256.map"
#include "../../Unicode/utf8_to_win1257.map"
#include "../../Unicode/utf8_to_win1258.map"
#include "../../Unicode/utf8_to_win866.map"
#include "../../Unicode/utf8_to_win874.map"
#include "../../Unicode/win1250_to_utf8.map"
#include "../../Unicode/win1251_to_utf8.map"
#include "../../Unicode/win1252_to_utf8.map"
#include "../../Unicode/win1253_to_utf8.map"
#include "../../Unicode/win1254_to_utf8.map"
#include "../../Unicode/win1255_to_utf8.map"
#include "../../Unicode/win1256_to_utf8.map"
#include "../../Unicode/win1257_to_utf8.map"
#include "../../Unicode/win866_to_utf8.map"
#include "../../Unicode/win874_to_utf8.map"
#include "../../Unicode/win1258_to_utf8.map"

MDB_MODULE_MAGIC;

MDB_FUNCTION_INFO_V1(win_to_utf8);
MDB_FUNCTION_INFO_V1(utf8_to_win);

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

typedef struct
{
	mdb_enc		encoding;
	const mdb_local_to_utf *map1;	/* to UTF8 map name */
	const mdb_utf_to_local *map2;	/* from UTF8 map name */
	int			size1;			/* size of map1 */
	int			size2;			/* size of map2 */
} mdb_conv_map;

static const mdb_conv_map maps[] = {
	{MDB_WIN866, LUmapWIN866, ULmapWIN866,
		lengthof(LUmapWIN866),
	lengthof(ULmapWIN866)},
	{MDB_WIN874, LUmapWIN874, ULmapWIN874,
		lengthof(LUmapWIN874),
	lengthof(ULmapWIN874)},
	{MDB_WIN1250, LUmapWIN1250, ULmapWIN1250,
		lengthof(LUmapWIN1250),
	lengthof(ULmapWIN1250)},
	{MDB_WIN1251, LUmapWIN1251, ULmapWIN1251,
		lengthof(LUmapWIN1251),
	lengthof(ULmapWIN1251)},
	{MDB_WIN1252, LUmapWIN1252, ULmapWIN1252,
		lengthof(LUmapWIN1252),
	lengthof(ULmapWIN1252)},
	{MDB_WIN1253, LUmapWIN1253, ULmapWIN1253,
		lengthof(LUmapWIN1253),
	lengthof(ULmapWIN1253)},
	{MDB_WIN1254, LUmapWIN1254, ULmapWIN1254,
		lengthof(LUmapWIN1254),
	lengthof(ULmapWIN1254)},
	{MDB_WIN1255, LUmapWIN1255, ULmapWIN1255,
		lengthof(LUmapWIN1255),
	lengthof(ULmapWIN1255)},
	{MDB_WIN1256, LUmapWIN1256, ULmapWIN1256,
		lengthof(LUmapWIN1256),
	lengthof(ULmapWIN1256)},
	{MDB_WIN1257, LUmapWIN1257, ULmapWIN1257,
		lengthof(LUmapWIN1257),
	lengthof(ULmapWIN1257)},
	{MDB_WIN1258, LUmapWIN1258, ULmapWIN1258,
		lengthof(LUmapWIN1258),
	lengthof(ULmapWIN1258)},
};

Datum
win_to_utf8(MDB_FUNCTION_ARGS)
{
	int			encoding = MDB_GETARG_INT32(0);
	unsigned char *src = (unsigned char *) MDB_GETARG_CSTRING(2);
	unsigned char *dest = (unsigned char *) MDB_GETARG_CSTRING(3);
	int			len = MDB_GETARG_INT32(4);
	int			i;

	CHECK_ENCODING_CONVERSION_ARGS(-1, MDB_UTF8);

	for (i = 0; i < lengthof(maps); i++)
	{
		if (encoding == maps[i].encoding)
		{
			LocalToUtf(src, len, dest,
					   maps[i].map1, maps[i].size1,
					   NULL, 0,
					   NULL,
					   encoding);
			MDB_RETURN_VOID();
		}
	}

	ereport(ERROR,
			(errcode(ERRCODE_INTERNAL_ERROR),
			 errmsg("unexpected encoding ID %d for WIN character sets",
					encoding)));

	MDB_RETURN_VOID();
}

Datum
utf8_to_win(MDB_FUNCTION_ARGS)
{
	int			encoding = MDB_GETARG_INT32(1);
	unsigned char *src = (unsigned char *) MDB_GETARG_CSTRING(2);
	unsigned char *dest = (unsigned char *) MDB_GETARG_CSTRING(3);
	int			len = MDB_GETARG_INT32(4);
	int			i;

	CHECK_ENCODING_CONVERSION_ARGS(MDB_UTF8, -1);

	for (i = 0; i < lengthof(maps); i++)
	{
		if (encoding == maps[i].encoding)
		{
			UtfToLocal(src, len, dest,
					   maps[i].map2, maps[i].size2,
					   NULL, 0,
					   NULL,
					   encoding);
			MDB_RETURN_VOID();
		}
	}

	ereport(ERROR,
			(errcode(ERRCODE_INTERNAL_ERROR),
			 errmsg("unexpected encoding ID %d for WIN character sets",
					encoding)));

	MDB_RETURN_VOID();
}
