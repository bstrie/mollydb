/*-------------------------------------------------------------------------
 *
 * version.c
 *	 Returns the MollyDB version string
 *
 * Copyright (c) 1998-2016, MollyDB Global Development Group
 *
 * IDENTIFICATION
 *
 * src/backend/utils/adt/version.c
 *
 *-------------------------------------------------------------------------
 */

#include "mollydb.h"

#include "utils/builtins.h"


Datum
mdb_version(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_TEXT_P(cstring_to_text(MDB_VERSION_STR));
}
