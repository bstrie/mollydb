/*-------------------------------------------------------------------------
 *
 * char.c
 *	  Functions for the built-in type "char" (not to be confused with
 *	  bpchar, which is the SQL CHAR(n) type).
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/utils/adt/char.c
 *
 *-------------------------------------------------------------------------
 */
#include "mollydb.h"

#include <limits.h>

#include "libpq/pqformat.h"
#include "utils/builtins.h"

/*****************************************************************************
 *	 USER I/O ROUTINES														 *
 *****************************************************************************/

/*
 *		charin			- converts "x" to 'x'
 *
 * Note that an empty input string will implicitly be converted to \0.
 */
Datum
charin(MDB_FUNCTION_ARGS)
{
	char	   *ch = MDB_GETARG_CSTRING(0);

	MDB_RETURN_CHAR(ch[0]);
}

/*
 *		charout			- converts 'x' to "x"
 *
 * Note that if the char value is \0, the resulting string will appear
 * to be empty (null-terminated after zero characters).  So this is the
 * inverse of the charin() function for such data.
 */
Datum
charout(MDB_FUNCTION_ARGS)
{
	char		ch = MDB_GETARG_CHAR(0);
	char	   *result = (char *) palloc(2);

	result[0] = ch;
	result[1] = '\0';
	MDB_RETURN_CSTRING(result);
}

/*
 *		charrecv			- converts external binary format to char
 *
 * The external representation is one byte, with no character set
 * conversion.  This is somewhat dubious, perhaps, but in many
 * cases people use char for a 1-byte binary type.
 */
Datum
charrecv(MDB_FUNCTION_ARGS)
{
	StringInfo	buf = (StringInfo) MDB_GETARG_POINTER(0);

	MDB_RETURN_CHAR(pq_getmsgbyte(buf));
}

/*
 *		charsend			- converts char to binary format
 */
Datum
charsend(MDB_FUNCTION_ARGS)
{
	char		arg1 = MDB_GETARG_CHAR(0);
	StringInfoData buf;

	pq_begintypsend(&buf);
	pq_sendbyte(&buf, arg1);
	MDB_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

/*****************************************************************************
 *	 PUBLIC ROUTINES														 *
 *****************************************************************************/

/*
 * NOTE: comparisons are done as though char is unsigned (uint8).
 * Conversions to and from integer are done as though char is signed (int8).
 *
 * You wanted consistency?
 */

Datum
chareq(MDB_FUNCTION_ARGS)
{
	char		arg1 = MDB_GETARG_CHAR(0);
	char		arg2 = MDB_GETARG_CHAR(1);

	MDB_RETURN_BOOL(arg1 == arg2);
}

Datum
charne(MDB_FUNCTION_ARGS)
{
	char		arg1 = MDB_GETARG_CHAR(0);
	char		arg2 = MDB_GETARG_CHAR(1);

	MDB_RETURN_BOOL(arg1 != arg2);
}

Datum
charlt(MDB_FUNCTION_ARGS)
{
	char		arg1 = MDB_GETARG_CHAR(0);
	char		arg2 = MDB_GETARG_CHAR(1);

	MDB_RETURN_BOOL((uint8) arg1 < (uint8) arg2);
}

Datum
charle(MDB_FUNCTION_ARGS)
{
	char		arg1 = MDB_GETARG_CHAR(0);
	char		arg2 = MDB_GETARG_CHAR(1);

	MDB_RETURN_BOOL((uint8) arg1 <= (uint8) arg2);
}

Datum
chargt(MDB_FUNCTION_ARGS)
{
	char		arg1 = MDB_GETARG_CHAR(0);
	char		arg2 = MDB_GETARG_CHAR(1);

	MDB_RETURN_BOOL((uint8) arg1 > (uint8) arg2);
}

Datum
charge(MDB_FUNCTION_ARGS)
{
	char		arg1 = MDB_GETARG_CHAR(0);
	char		arg2 = MDB_GETARG_CHAR(1);

	MDB_RETURN_BOOL((uint8) arg1 >= (uint8) arg2);
}


Datum
chartoi4(MDB_FUNCTION_ARGS)
{
	char		arg1 = MDB_GETARG_CHAR(0);

	MDB_RETURN_INT32((int32) ((int8) arg1));
}

Datum
i4tochar(MDB_FUNCTION_ARGS)
{
	int32		arg1 = MDB_GETARG_INT32(0);

	if (arg1 < SCHAR_MIN || arg1 > SCHAR_MAX)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("\"char\" out of range")));

	MDB_RETURN_CHAR((int8) arg1);
}


Datum
text_char(MDB_FUNCTION_ARGS)
{
	text	   *arg1 = MDB_GETARG_TEXT_P(0);
	char		result;

	/*
	 * An empty input string is converted to \0 (for consistency with charin).
	 * If the input is longer than one character, the excess data is silently
	 * discarded.
	 */
	if (VARSIZE(arg1) > VARHDRSZ)
		result = *(VARDATA(arg1));
	else
		result = '\0';

	MDB_RETURN_CHAR(result);
}

Datum
char_text(MDB_FUNCTION_ARGS)
{
	char		arg1 = MDB_GETARG_CHAR(0);
	text	   *result = palloc(VARHDRSZ + 1);

	/*
	 * Convert \0 to an empty string, for consistency with charout (and
	 * because the text stuff doesn't like embedded nulls all that well).
	 */
	if (arg1 != '\0')
	{
		SET_VARSIZE(result, VARHDRSZ + 1);
		*(VARDATA(result)) = arg1;
	}
	else
		SET_VARSIZE(result, VARHDRSZ);

	MDB_RETURN_TEXT_P(result);
}
