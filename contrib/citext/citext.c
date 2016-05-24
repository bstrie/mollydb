/*
 * contrib/citext/citext.c
 */
#include "mollydb.h"

#include "access/hash.h"
#include "catalog/mdb_collation.h"
#include "utils/builtins.h"
#include "utils/formatting.h"

#ifdef MDB_MODULE_MAGIC
MDB_MODULE_MAGIC;
#endif

/*
 *		====================
 *		FORWARD DECLARATIONS
 *		====================
 */

static int32 citextcmp(text *left, text *right, Oid collid);

/*
 *		=================
 *		UTILITY FUNCTIONS
 *		=================
 */

/*
 * citextcmp()
 * Internal comparison function for citext strings.
 * Returns int32 negative, zero, or positive.
 */
static int32
citextcmp(text *left, text *right, Oid collid)
{
	char	   *lcstr,
			   *rcstr;
	int32		result;

	/*
	 * We must do our str_tolower calls with DEFAULT_COLLATION_OID, not the
	 * input collation as you might expect.  This is so that the behavior of
	 * citext's equality and hashing functions is not collation-dependent.  We
	 * should change this once the core infrastructure is able to cope with
	 * collation-dependent equality and hashing functions.
	 */

	lcstr = str_tolower(VARDATA_ANY(left), VARSIZE_ANY_EXHDR(left), DEFAULT_COLLATION_OID);
	rcstr = str_tolower(VARDATA_ANY(right), VARSIZE_ANY_EXHDR(right), DEFAULT_COLLATION_OID);

	result = varstr_cmp(lcstr, strlen(lcstr),
						rcstr, strlen(rcstr),
						collid);

	pfree(lcstr);
	pfree(rcstr);

	return result;
}

/*
 *		==================
 *		INDEXING FUNCTIONS
 *		==================
 */

MDB_FUNCTION_INFO_V1(citext_cmp);

Datum
citext_cmp(MDB_FUNCTION_ARGS)
{
	text	   *left = MDB_GETARG_TEXT_PP(0);
	text	   *right = MDB_GETARG_TEXT_PP(1);
	int32		result;

	result = citextcmp(left, right, MDB_GET_COLLATION());

	MDB_FREE_IF_COPY(left, 0);
	MDB_FREE_IF_COPY(right, 1);

	MDB_RETURN_INT32(result);
}

MDB_FUNCTION_INFO_V1(citext_hash);

Datum
citext_hash(MDB_FUNCTION_ARGS)
{
	text	   *txt = MDB_GETARG_TEXT_PP(0);
	char	   *str;
	Datum		result;

	str = str_tolower(VARDATA_ANY(txt), VARSIZE_ANY_EXHDR(txt), DEFAULT_COLLATION_OID);
	result = hash_any((unsigned char *) str, strlen(str));
	pfree(str);

	/* Avoid leaking memory for toasted inputs */
	MDB_FREE_IF_COPY(txt, 0);

	MDB_RETURN_DATUM(result);
}

/*
 *		==================
 *		OPERATOR FUNCTIONS
 *		==================
 */

MDB_FUNCTION_INFO_V1(citext_eq);

Datum
citext_eq(MDB_FUNCTION_ARGS)
{
	text	   *left = MDB_GETARG_TEXT_PP(0);
	text	   *right = MDB_GETARG_TEXT_PP(1);
	char	   *lcstr,
			   *rcstr;
	bool		result;

	/* We can't compare lengths in advance of downcasing ... */

	lcstr = str_tolower(VARDATA_ANY(left), VARSIZE_ANY_EXHDR(left), DEFAULT_COLLATION_OID);
	rcstr = str_tolower(VARDATA_ANY(right), VARSIZE_ANY_EXHDR(right), DEFAULT_COLLATION_OID);

	/*
	 * Since we only care about equality or not-equality, we can avoid all the
	 * expense of strcoll() here, and just do bitwise comparison.
	 */
	result = (strcmp(lcstr, rcstr) == 0);

	pfree(lcstr);
	pfree(rcstr);
	MDB_FREE_IF_COPY(left, 0);
	MDB_FREE_IF_COPY(right, 1);

	MDB_RETURN_BOOL(result);
}

MDB_FUNCTION_INFO_V1(citext_ne);

Datum
citext_ne(MDB_FUNCTION_ARGS)
{
	text	   *left = MDB_GETARG_TEXT_PP(0);
	text	   *right = MDB_GETARG_TEXT_PP(1);
	char	   *lcstr,
			   *rcstr;
	bool		result;

	/* We can't compare lengths in advance of downcasing ... */

	lcstr = str_tolower(VARDATA_ANY(left), VARSIZE_ANY_EXHDR(left), DEFAULT_COLLATION_OID);
	rcstr = str_tolower(VARDATA_ANY(right), VARSIZE_ANY_EXHDR(right), DEFAULT_COLLATION_OID);

	/*
	 * Since we only care about equality or not-equality, we can avoid all the
	 * expense of strcoll() here, and just do bitwise comparison.
	 */
	result = (strcmp(lcstr, rcstr) != 0);

	pfree(lcstr);
	pfree(rcstr);
	MDB_FREE_IF_COPY(left, 0);
	MDB_FREE_IF_COPY(right, 1);

	MDB_RETURN_BOOL(result);
}

MDB_FUNCTION_INFO_V1(citext_lt);

Datum
citext_lt(MDB_FUNCTION_ARGS)
{
	text	   *left = MDB_GETARG_TEXT_PP(0);
	text	   *right = MDB_GETARG_TEXT_PP(1);
	bool		result;

	result = citextcmp(left, right, MDB_GET_COLLATION()) < 0;

	MDB_FREE_IF_COPY(left, 0);
	MDB_FREE_IF_COPY(right, 1);

	MDB_RETURN_BOOL(result);
}

MDB_FUNCTION_INFO_V1(citext_le);

Datum
citext_le(MDB_FUNCTION_ARGS)
{
	text	   *left = MDB_GETARG_TEXT_PP(0);
	text	   *right = MDB_GETARG_TEXT_PP(1);
	bool		result;

	result = citextcmp(left, right, MDB_GET_COLLATION()) <= 0;

	MDB_FREE_IF_COPY(left, 0);
	MDB_FREE_IF_COPY(right, 1);

	MDB_RETURN_BOOL(result);
}

MDB_FUNCTION_INFO_V1(citext_gt);

Datum
citext_gt(MDB_FUNCTION_ARGS)
{
	text	   *left = MDB_GETARG_TEXT_PP(0);
	text	   *right = MDB_GETARG_TEXT_PP(1);
	bool		result;

	result = citextcmp(left, right, MDB_GET_COLLATION()) > 0;

	MDB_FREE_IF_COPY(left, 0);
	MDB_FREE_IF_COPY(right, 1);

	MDB_RETURN_BOOL(result);
}

MDB_FUNCTION_INFO_V1(citext_ge);

Datum
citext_ge(MDB_FUNCTION_ARGS)
{
	text	   *left = MDB_GETARG_TEXT_PP(0);
	text	   *right = MDB_GETARG_TEXT_PP(1);
	bool		result;

	result = citextcmp(left, right, MDB_GET_COLLATION()) >= 0;

	MDB_FREE_IF_COPY(left, 0);
	MDB_FREE_IF_COPY(right, 1);

	MDB_RETURN_BOOL(result);
}

/*
 *		===================
 *		AGGREGATE FUNCTIONS
 *		===================
 */

MDB_FUNCTION_INFO_V1(citext_smaller);

Datum
citext_smaller(MDB_FUNCTION_ARGS)
{
	text	   *left = MDB_GETARG_TEXT_PP(0);
	text	   *right = MDB_GETARG_TEXT_PP(1);
	text	   *result;

	result = citextcmp(left, right, MDB_GET_COLLATION()) < 0 ? left : right;
	MDB_RETURN_TEXT_P(result);
}

MDB_FUNCTION_INFO_V1(citext_larger);

Datum
citext_larger(MDB_FUNCTION_ARGS)
{
	text	   *left = MDB_GETARG_TEXT_PP(0);
	text	   *right = MDB_GETARG_TEXT_PP(1);
	text	   *result;

	result = citextcmp(left, right, MDB_GET_COLLATION()) > 0 ? left : right;
	MDB_RETURN_TEXT_P(result);
}
