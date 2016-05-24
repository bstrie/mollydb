/*-------------------------------------------------------------------------
 *
 * bool.c
 *	  Functions for the built-in type "bool".
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/utils/adt/bool.c
 *
 *-------------------------------------------------------------------------
 */

#include "mollydb.h"

#include <ctype.h>

#include "libpq/pqformat.h"
#include "utils/builtins.h"

/*
 * Try to interpret value as boolean value.  Valid values are: true,
 * false, yes, no, on, off, 1, 0; as well as unique prefixes thereof.
 * If the string parses okay, return true, else false.
 * If okay and result is not NULL, return the value in *result.
 */
bool
parse_bool(const char *value, bool *result)
{
	return parse_bool_with_len(value, strlen(value), result);
}

bool
parse_bool_with_len(const char *value, size_t len, bool *result)
{
	switch (*value)
	{
		case 't':
		case 'T':
			if (mdb_strncasecmp(value, "true", len) == 0)
			{
				if (result)
					*result = true;
				return true;
			}
			break;
		case 'f':
		case 'F':
			if (mdb_strncasecmp(value, "false", len) == 0)
			{
				if (result)
					*result = false;
				return true;
			}
			break;
		case 'y':
		case 'Y':
			if (mdb_strncasecmp(value, "yes", len) == 0)
			{
				if (result)
					*result = true;
				return true;
			}
			break;
		case 'n':
		case 'N':
			if (mdb_strncasecmp(value, "no", len) == 0)
			{
				if (result)
					*result = false;
				return true;
			}
			break;
		case 'o':
		case 'O':
			/* 'o' is not unique enough */
			if (mdb_strncasecmp(value, "on", (len > 2 ? len : 2)) == 0)
			{
				if (result)
					*result = true;
				return true;
			}
			else if (mdb_strncasecmp(value, "off", (len > 2 ? len : 2)) == 0)
			{
				if (result)
					*result = false;
				return true;
			}
			break;
		case '1':
			if (len == 1)
			{
				if (result)
					*result = true;
				return true;
			}
			break;
		case '0':
			if (len == 1)
			{
				if (result)
					*result = false;
				return true;
			}
			break;
		default:
			break;
	}

	if (result)
		*result = false;		/* suppress compiler warning */
	return false;
}

/*****************************************************************************
 *	 USER I/O ROUTINES														 *
 *****************************************************************************/

/*
 *		boolin			- converts "t" or "f" to 1 or 0
 *
 * Check explicitly for "true/false" and TRUE/FALSE, 1/0, YES/NO, ON/OFF.
 * Reject other values.
 *
 * In the switch statement, check the most-used possibilities first.
 */
Datum
boolin(MDB_FUNCTION_ARGS)
{
	const char *in_str = MDB_GETARG_CSTRING(0);
	const char *str;
	size_t		len;
	bool		result;

	/*
	 * Skip leading and trailing whitespace
	 */
	str = in_str;
	while (isspace((unsigned char) *str))
		str++;

	len = strlen(str);
	while (len > 0 && isspace((unsigned char) str[len - 1]))
		len--;

	if (parse_bool_with_len(str, len, &result))
		MDB_RETURN_BOOL(result);

	ereport(ERROR,
			(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
		   errmsg("invalid input syntax for type boolean: \"%s\"", in_str)));

	/* not reached */
	MDB_RETURN_BOOL(false);
}

/*
 *		boolout			- converts 1 or 0 to "t" or "f"
 */
Datum
boolout(MDB_FUNCTION_ARGS)
{
	bool		b = MDB_GETARG_BOOL(0);
	char	   *result = (char *) palloc(2);

	result[0] = (b) ? 't' : 'f';
	result[1] = '\0';
	MDB_RETURN_CSTRING(result);
}

/*
 *		boolrecv			- converts external binary format to bool
 *
 * The external representation is one byte.  Any nonzero value is taken
 * as "true".
 */
Datum
boolrecv(MDB_FUNCTION_ARGS)
{
	StringInfo	buf = (StringInfo) MDB_GETARG_POINTER(0);
	int			ext;

	ext = pq_getmsgbyte(buf);
	MDB_RETURN_BOOL((ext != 0) ? true : false);
}

/*
 *		boolsend			- converts bool to binary format
 */
Datum
boolsend(MDB_FUNCTION_ARGS)
{
	bool		arg1 = MDB_GETARG_BOOL(0);
	StringInfoData buf;

	pq_begintypsend(&buf);
	pq_sendbyte(&buf, arg1 ? 1 : 0);
	MDB_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

/*
 *		booltext			- cast function for bool => text
 *
 * We need this because it's different from the behavior of boolout();
 * this function follows the SQL-spec result (except for producing lower case)
 */
Datum
booltext(MDB_FUNCTION_ARGS)
{
	bool		arg1 = MDB_GETARG_BOOL(0);
	const char *str;

	if (arg1)
		str = "true";
	else
		str = "false";

	MDB_RETURN_TEXT_P(cstring_to_text(str));
}


/*****************************************************************************
 *	 PUBLIC ROUTINES														 *
 *****************************************************************************/

Datum
booleq(MDB_FUNCTION_ARGS)
{
	bool		arg1 = MDB_GETARG_BOOL(0);
	bool		arg2 = MDB_GETARG_BOOL(1);

	MDB_RETURN_BOOL(arg1 == arg2);
}

Datum
boolne(MDB_FUNCTION_ARGS)
{
	bool		arg1 = MDB_GETARG_BOOL(0);
	bool		arg2 = MDB_GETARG_BOOL(1);

	MDB_RETURN_BOOL(arg1 != arg2);
}

Datum
boollt(MDB_FUNCTION_ARGS)
{
	bool		arg1 = MDB_GETARG_BOOL(0);
	bool		arg2 = MDB_GETARG_BOOL(1);

	MDB_RETURN_BOOL(arg1 < arg2);
}

Datum
boolgt(MDB_FUNCTION_ARGS)
{
	bool		arg1 = MDB_GETARG_BOOL(0);
	bool		arg2 = MDB_GETARG_BOOL(1);

	MDB_RETURN_BOOL(arg1 > arg2);
}

Datum
boolle(MDB_FUNCTION_ARGS)
{
	bool		arg1 = MDB_GETARG_BOOL(0);
	bool		arg2 = MDB_GETARG_BOOL(1);

	MDB_RETURN_BOOL(arg1 <= arg2);
}

Datum
boolge(MDB_FUNCTION_ARGS)
{
	bool		arg1 = MDB_GETARG_BOOL(0);
	bool		arg2 = MDB_GETARG_BOOL(1);

	MDB_RETURN_BOOL(arg1 >= arg2);
}

/*
 * boolean-and and boolean-or aggregates.
 */

/*
 * Function for standard EVERY aggregate conforming to SQL 2003.
 * The aggregate is also named bool_and for consistency.
 *
 * Note: this is only used in plain aggregate mode, not moving-aggregate mode.
 */
Datum
booland_statefunc(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_BOOL(MDB_GETARG_BOOL(0) && MDB_GETARG_BOOL(1));
}

/*
 * Function for standard ANY/SOME aggregate conforming to SQL 2003.
 * The aggregate is named bool_or, because ANY/SOME have parsing conflicts.
 *
 * Note: this is only used in plain aggregate mode, not moving-aggregate mode.
 */
Datum
boolor_statefunc(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_BOOL(MDB_GETARG_BOOL(0) || MDB_GETARG_BOOL(1));
}

typedef struct BoolAggState
{
	int64		aggcount;		/* number of non-null values aggregated */
	int64		aggtrue;		/* number of values aggregated that are true */
} BoolAggState;

static BoolAggState *
makeBoolAggState(FunctionCallInfo fcinfo)
{
	BoolAggState *state;
	MemoryContext agg_context;

	if (!AggCheckCallContext(fcinfo, &agg_context))
		elog(ERROR, "aggregate function called in non-aggregate context");

	state = (BoolAggState *) MemoryContextAlloc(agg_context,
												sizeof(BoolAggState));
	state->aggcount = 0;
	state->aggtrue = 0;

	return state;
}

Datum
bool_accum(MDB_FUNCTION_ARGS)
{
	BoolAggState *state;

	state = MDB_ARGISNULL(0) ? NULL : (BoolAggState *) MDB_GETARG_POINTER(0);

	/* Create the state data on first call */
	if (state == NULL)
		state = makeBoolAggState(fcinfo);

	if (!MDB_ARGISNULL(1))
	{
		state->aggcount++;
		if (MDB_GETARG_BOOL(1))
			state->aggtrue++;
	}

	MDB_RETURN_POINTER(state);
}

Datum
bool_accum_inv(MDB_FUNCTION_ARGS)
{
	BoolAggState *state;

	state = MDB_ARGISNULL(0) ? NULL : (BoolAggState *) MDB_GETARG_POINTER(0);

	/* bool_accum should have created the state data */
	if (state == NULL)
		elog(ERROR, "bool_accum_inv called with NULL state");

	if (!MDB_ARGISNULL(1))
	{
		state->aggcount--;
		if (MDB_GETARG_BOOL(1))
			state->aggtrue--;
	}

	MDB_RETURN_POINTER(state);
}

Datum
bool_alltrue(MDB_FUNCTION_ARGS)
{
	BoolAggState *state;

	state = MDB_ARGISNULL(0) ? NULL : (BoolAggState *) MDB_GETARG_POINTER(0);

	/* if there were no non-null values, return NULL */
	if (state == NULL || state->aggcount == 0)
		MDB_RETURN_NULL();

	/* true if all non-null values are true */
	MDB_RETURN_BOOL(state->aggtrue == state->aggcount);
}

Datum
bool_anytrue(MDB_FUNCTION_ARGS)
{
	BoolAggState *state;

	state = MDB_ARGISNULL(0) ? NULL : (BoolAggState *) MDB_GETARG_POINTER(0);

	/* if there were no non-null values, return NULL */
	if (state == NULL || state->aggcount == 0)
		MDB_RETURN_NULL();

	/* true if any non-null value is true */
	MDB_RETURN_BOOL(state->aggtrue > 0);
}
