/*-------------------------------------------------------------------------
 *
 * name.c
 *	  Functions for the built-in type "name".
 *
 * name replaces char16 and is carefully implemented so that it
 * is a string of physical length NAMEDATALEN.
 * DO NOT use hard-coded constants anywhere
 * always use NAMEDATALEN as the symbolic constant!   - jolly 8/21/95
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/utils/adt/name.c
 *
 *-------------------------------------------------------------------------
 */
#include "mollydb.h"

#include "catalog/namespace.h"
#include "catalog/mdb_type.h"
#include "libpq/pqformat.h"
#include "mb/mdb_wchar.h"
#include "miscadmin.h"
#include "utils/array.h"
#include "utils/builtins.h"
#include "utils/lsyscache.h"


/*****************************************************************************
 *	 USER I/O ROUTINES (none)												 *
 *****************************************************************************/


/*
 *		namein	- converts "..." to internal representation
 *
 *		Note:
 *				[Old] Currently if strlen(s) < NAMEDATALEN, the extra chars are nulls
 *				Now, always NULL terminated
 */
Datum
namein(MDB_FUNCTION_ARGS)
{
	char	   *s = MDB_GETARG_CSTRING(0);
	Name		result;
	int			len;

	len = strlen(s);

	/* Truncate oversize input */
	if (len >= NAMEDATALEN)
		len = mdb_mbcliplen(s, len, NAMEDATALEN - 1);

	/* We use palloc0 here to ensure result is zero-padded */
	result = (Name) palloc0(NAMEDATALEN);
	memcpy(NameStr(*result), s, len);

	MDB_RETURN_NAME(result);
}

/*
 *		nameout - converts internal representation to "..."
 */
Datum
nameout(MDB_FUNCTION_ARGS)
{
	Name		s = MDB_GETARG_NAME(0);

	MDB_RETURN_CSTRING(pstrdup(NameStr(*s)));
}

/*
 *		namerecv			- converts external binary format to name
 */
Datum
namerecv(MDB_FUNCTION_ARGS)
{
	StringInfo	buf = (StringInfo) MDB_GETARG_POINTER(0);
	Name		result;
	char	   *str;
	int			nbytes;

	str = pq_getmsgtext(buf, buf->len - buf->cursor, &nbytes);
	if (nbytes >= NAMEDATALEN)
		ereport(ERROR,
				(errcode(ERRCODE_NAME_TOO_LONG),
				 errmsg("identifier too long"),
				 errdetail("Identifier must be less than %d characters.",
						   NAMEDATALEN)));
	result = (NameData *) palloc0(NAMEDATALEN);
	memcpy(result, str, nbytes);
	pfree(str);
	MDB_RETURN_NAME(result);
}

/*
 *		namesend			- converts name to binary format
 */
Datum
namesend(MDB_FUNCTION_ARGS)
{
	Name		s = MDB_GETARG_NAME(0);
	StringInfoData buf;

	pq_begintypsend(&buf);
	pq_sendtext(&buf, NameStr(*s), strlen(NameStr(*s)));
	MDB_RETURN_BYTEA_P(pq_endtypsend(&buf));
}


/*****************************************************************************
 *	 PUBLIC ROUTINES														 *
 *****************************************************************************/

/*
 *		nameeq	- returns 1 iff arguments are equal
 *		namene	- returns 1 iff arguments are not equal
 *
 *		BUGS:
 *				Assumes that "xy\0\0a" should be equal to "xy\0b".
 *				If not, can do the comparison backwards for efficiency.
 *
 *		namelt	- returns 1 iff a < b
 *		namele	- returns 1 iff a <= b
 *		namegt	- returns 1 iff a > b
 *		namege	- returns 1 iff a >= b
 *
 */
Datum
nameeq(MDB_FUNCTION_ARGS)
{
	Name		arg1 = MDB_GETARG_NAME(0);
	Name		arg2 = MDB_GETARG_NAME(1);

	MDB_RETURN_BOOL(strncmp(NameStr(*arg1), NameStr(*arg2), NAMEDATALEN) == 0);
}

Datum
namene(MDB_FUNCTION_ARGS)
{
	Name		arg1 = MDB_GETARG_NAME(0);
	Name		arg2 = MDB_GETARG_NAME(1);

	MDB_RETURN_BOOL(strncmp(NameStr(*arg1), NameStr(*arg2), NAMEDATALEN) != 0);
}

Datum
namelt(MDB_FUNCTION_ARGS)
{
	Name		arg1 = MDB_GETARG_NAME(0);
	Name		arg2 = MDB_GETARG_NAME(1);

	MDB_RETURN_BOOL(strncmp(NameStr(*arg1), NameStr(*arg2), NAMEDATALEN) < 0);
}

Datum
namele(MDB_FUNCTION_ARGS)
{
	Name		arg1 = MDB_GETARG_NAME(0);
	Name		arg2 = MDB_GETARG_NAME(1);

	MDB_RETURN_BOOL(strncmp(NameStr(*arg1), NameStr(*arg2), NAMEDATALEN) <= 0);
}

Datum
namegt(MDB_FUNCTION_ARGS)
{
	Name		arg1 = MDB_GETARG_NAME(0);
	Name		arg2 = MDB_GETARG_NAME(1);

	MDB_RETURN_BOOL(strncmp(NameStr(*arg1), NameStr(*arg2), NAMEDATALEN) > 0);
}

Datum
namege(MDB_FUNCTION_ARGS)
{
	Name		arg1 = MDB_GETARG_NAME(0);
	Name		arg2 = MDB_GETARG_NAME(1);

	MDB_RETURN_BOOL(strncmp(NameStr(*arg1), NameStr(*arg2), NAMEDATALEN) >= 0);
}


/* (see char.c for comparison/operation routines) */

int
namecpy(Name n1, Name n2)
{
	if (!n1 || !n2)
		return -1;
	StrNCpy(NameStr(*n1), NameStr(*n2), NAMEDATALEN);
	return 0;
}

#ifdef NOT_USED
int
namecat(Name n1, Name n2)
{
	return namestrcat(n1, NameStr(*n2));		/* n2 can't be any longer than
												 * n1 */
}
#endif

#ifdef NOT_USED
int
namecmp(Name n1, Name n2)
{
	return strncmp(NameStr(*n1), NameStr(*n2), NAMEDATALEN);
}
#endif

int
namestrcpy(Name name, const char *str)
{
	if (!name || !str)
		return -1;
	StrNCpy(NameStr(*name), str, NAMEDATALEN);
	return 0;
}

#ifdef NOT_USED
int
namestrcat(Name name, const char *str)
{
	int			i;
	char	   *p,
			   *q;

	if (!name || !str)
		return -1;
	for (i = 0, p = NameStr(*name); i < NAMEDATALEN && *p; ++i, ++p)
		;
	for (q = str; i < NAMEDATALEN; ++i, ++p, ++q)
	{
		*p = *q;
		if (!*q)
			break;
	}
	return 0;
}
#endif

int
namestrcmp(Name name, const char *str)
{
	if (!name && !str)
		return 0;
	if (!name)
		return -1;				/* NULL < anything */
	if (!str)
		return 1;				/* NULL < anything */
	return strncmp(NameStr(*name), str, NAMEDATALEN);
}


/*
 * SQL-functions CURRENT_USER, SESSION_USER
 */
Datum
current_user(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_DATUM(DirectFunctionCall1(namein, CStringGetDatum(GetUserNameFromId(GetUserId(), false))));
}

Datum
session_user(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_DATUM(DirectFunctionCall1(namein, CStringGetDatum(GetUserNameFromId(GetSessionUserId(), false))));
}


/*
 * SQL-functions CURRENT_SCHEMA, CURRENT_SCHEMAS
 */
Datum
current_schema(MDB_FUNCTION_ARGS)
{
	List	   *search_path = fetch_search_path(false);
	char	   *nspname;

	if (search_path == NIL)
		MDB_RETURN_NULL();
	nspname = get_namespace_name(linitial_oid(search_path));
	list_free(search_path);
	if (!nspname)
		MDB_RETURN_NULL();		/* recently-deleted namespace? */
	MDB_RETURN_DATUM(DirectFunctionCall1(namein, CStringGetDatum(nspname)));
}

Datum
current_schemas(MDB_FUNCTION_ARGS)
{
	List	   *search_path = fetch_search_path(MDB_GETARG_BOOL(0));
	ListCell   *l;
	Datum	   *names;
	int			i;
	ArrayType  *array;

	names = (Datum *) palloc(list_length(search_path) * sizeof(Datum));
	i = 0;
	foreach(l, search_path)
	{
		char	   *nspname;

		nspname = get_namespace_name(lfirst_oid(l));
		if (nspname)			/* watch out for deleted namespace */
		{
			names[i] = DirectFunctionCall1(namein, CStringGetDatum(nspname));
			i++;
		}
	}
	list_free(search_path);

	array = construct_array(names, i,
							NAMEOID,
							NAMEDATALEN,		/* sizeof(Name) */
							false,		/* Name is not by-val */
							'c');		/* alignment of Name */

	MDB_RETURN_POINTER(array);
}
