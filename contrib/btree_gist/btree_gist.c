/*
 * contrib/btree_gist/btree_gist.c
 */
#include "mollydb.h"

#include "btree_gist.h"

MDB_MODULE_MAGIC;

MDB_FUNCTION_INFO_V1(gbt_decompress);
MDB_FUNCTION_INFO_V1(gbtreekey_in);
MDB_FUNCTION_INFO_V1(gbtreekey_out);

/**************************************************
 * In/Out for keys
 **************************************************/


Datum
gbtreekey_in(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("<datatype>key_in() not implemented")));

	MDB_RETURN_POINTER(NULL);
}

#include "btree_utils_var.h"
#include "utils/builtins.h"
Datum
gbtreekey_out(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("<datatype>key_out() not implemented")));
	MDB_RETURN_POINTER(NULL);
}


/*
** GiST DeCompress methods
** do not do anything.
*/
Datum
gbt_decompress(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_POINTER(MDB_GETARG_POINTER(0));
}
