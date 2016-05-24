/*-------------------------------------------------------------------------
 *
 * pseudotypes.c
 *	  Functions for the system pseudo-types.
 *
 * A pseudo-type isn't really a type and never has any operations, but
 * we do need to supply input and output functions to satisfy the links
 * in the pseudo-type's entry in mdb_type.  In most cases the functions
 * just throw an error if invoked.  (XXX the error messages here cover
 * the most common case, but might be confusing in some contexts.  Can
 * we do better?)
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/utils/adt/pseudotypes.c
 *
 *-------------------------------------------------------------------------
 */
#include "mollydb.h"

#include "libpq/pqformat.h"
#include "utils/array.h"
#include "utils/builtins.h"
#include "utils/rangetypes.h"


/*
 * cstring_in		- input routine for pseudo-type CSTRING.
 *
 * We might as well allow this to support constructs like "foo_in('blah')".
 */
Datum
cstring_in(MDB_FUNCTION_ARGS)
{
	char	   *str = MDB_GETARG_CSTRING(0);

	MDB_RETURN_CSTRING(pstrdup(str));
}

/*
 * cstring_out		- output routine for pseudo-type CSTRING.
 *
 * We allow this mainly so that "SELECT some_output_function(...)" does
 * what the user will expect.
 */
Datum
cstring_out(MDB_FUNCTION_ARGS)
{
	char	   *str = MDB_GETARG_CSTRING(0);

	MDB_RETURN_CSTRING(pstrdup(str));
}

/*
 * cstring_recv		- binary input routine for pseudo-type CSTRING.
 */
Datum
cstring_recv(MDB_FUNCTION_ARGS)
{
	StringInfo	buf = (StringInfo) MDB_GETARG_POINTER(0);
	char	   *str;
	int			nbytes;

	str = pq_getmsgtext(buf, buf->len - buf->cursor, &nbytes);
	MDB_RETURN_CSTRING(str);
}

/*
 * cstring_send		- binary output routine for pseudo-type CSTRING.
 */
Datum
cstring_send(MDB_FUNCTION_ARGS)
{
	char	   *str = MDB_GETARG_CSTRING(0);
	StringInfoData buf;

	pq_begintypsend(&buf);
	pq_sendtext(&buf, str, strlen(str));
	MDB_RETURN_BYTEA_P(pq_endtypsend(&buf));
}


/*
 * any_in		- input routine for pseudo-type ANY.
 */
Datum
any_in(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot accept a value of type any")));

	MDB_RETURN_VOID();			/* keep compiler quiet */
}

/*
 * any_out		- output routine for pseudo-type ANY.
 */
Datum
any_out(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot display a value of type any")));

	MDB_RETURN_VOID();			/* keep compiler quiet */
}


/*
 * anyarray_in		- input routine for pseudo-type ANYARRAY.
 */
Datum
anyarray_in(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot accept a value of type anyarray")));

	MDB_RETURN_VOID();			/* keep compiler quiet */
}

/*
 * anyarray_out		- output routine for pseudo-type ANYARRAY.
 *
 * We may as well allow this, since array_out will in fact work.
 */
Datum
anyarray_out(MDB_FUNCTION_ARGS)
{
	return array_out(fcinfo);
}

/*
 * anyarray_recv		- binary input routine for pseudo-type ANYARRAY.
 *
 * XXX this could actually be made to work, since the incoming array
 * data will contain the element type OID.  Need to think through
 * type-safety issues before allowing it, however.
 */
Datum
anyarray_recv(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot accept a value of type anyarray")));

	MDB_RETURN_VOID();			/* keep compiler quiet */
}

/*
 * anyarray_send		- binary output routine for pseudo-type ANYARRAY.
 *
 * We may as well allow this, since array_send will in fact work.
 */
Datum
anyarray_send(MDB_FUNCTION_ARGS)
{
	return array_send(fcinfo);
}


/*
 * anyenum_in		- input routine for pseudo-type ANYENUM.
 */
Datum
anyenum_in(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot accept a value of type anyenum")));

	MDB_RETURN_VOID();			/* keep compiler quiet */
}

/*
 * anyenum_out		- output routine for pseudo-type ANYENUM.
 *
 * We may as well allow this, since enum_out will in fact work.
 */
Datum
anyenum_out(MDB_FUNCTION_ARGS)
{
	return enum_out(fcinfo);
}

/*
 * anyrange_in		- input routine for pseudo-type ANYRANGE.
 */
Datum
anyrange_in(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot accept a value of type anyrange")));

	MDB_RETURN_VOID();			/* keep compiler quiet */
}

/*
 * anyrange_out		- output routine for pseudo-type ANYRANGE.
 *
 * We may as well allow this, since range_out will in fact work.
 */
Datum
anyrange_out(MDB_FUNCTION_ARGS)
{
	return range_out(fcinfo);
}

/*
 * void_in		- input routine for pseudo-type VOID.
 *
 * We allow this so that PL functions can return VOID without any special
 * hack in the PL handler.  Whatever value the PL thinks it's returning
 * will just be ignored.
 */
Datum
void_in(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_VOID();			/* you were expecting something different? */
}

/*
 * void_out		- output routine for pseudo-type VOID.
 *
 * We allow this so that "SELECT function_returning_void(...)" works.
 */
Datum
void_out(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_CSTRING(pstrdup(""));
}

/*
 * void_recv	- binary input routine for pseudo-type VOID.
 *
 * Note that since we consume no bytes, an attempt to send anything but
 * an empty string will result in an "invalid message format" error.
 */
Datum
void_recv(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_VOID();
}

/*
 * void_send	- binary output routine for pseudo-type VOID.
 *
 * We allow this so that "SELECT function_returning_void(...)" works
 * even when binary output is requested.
 */
Datum
void_send(MDB_FUNCTION_ARGS)
{
	StringInfoData buf;

	/* send an empty string */
	pq_begintypsend(&buf);
	MDB_RETURN_BYTEA_P(pq_endtypsend(&buf));
}


/*
 * trigger_in		- input routine for pseudo-type TRIGGER.
 */
Datum
trigger_in(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot accept a value of type trigger")));

	MDB_RETURN_VOID();			/* keep compiler quiet */
}

/*
 * trigger_out		- output routine for pseudo-type TRIGGER.
 */
Datum
trigger_out(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot display a value of type trigger")));

	MDB_RETURN_VOID();			/* keep compiler quiet */
}


/*
 * event_trigger_in - input routine for pseudo-type event_trigger.
 */
Datum
event_trigger_in(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot accept a value of type event_trigger")));

	MDB_RETURN_VOID();			/* keep compiler quiet */
}

/*
 * event_trigger_out - output routine for pseudo-type event_trigger.
 */
Datum
event_trigger_out(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot display a value of type event_trigger")));

	MDB_RETURN_VOID();			/* keep compiler quiet */
}


/*
 * language_handler_in		- input routine for pseudo-type LANGUAGE_HANDLER.
 */
Datum
language_handler_in(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot accept a value of type language_handler")));

	MDB_RETURN_VOID();			/* keep compiler quiet */
}

/*
 * language_handler_out		- output routine for pseudo-type LANGUAGE_HANDLER.
 */
Datum
language_handler_out(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot display a value of type language_handler")));

	MDB_RETURN_VOID();			/* keep compiler quiet */
}


/*
 * fdw_handler_in		- input routine for pseudo-type FDW_HANDLER.
 */
Datum
fdw_handler_in(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot accept a value of type fdw_handler")));

	MDB_RETURN_VOID();			/* keep compiler quiet */
}

/*
 * fdw_handler_out		- output routine for pseudo-type FDW_HANDLER.
 */
Datum
fdw_handler_out(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot display a value of type fdw_handler")));

	MDB_RETURN_VOID();			/* keep compiler quiet */
}


/*
 * index_am_handler_in		- input routine for pseudo-type INDEX_AM_HANDLER.
 */
Datum
index_am_handler_in(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot accept a value of type index_am_handler")));

	MDB_RETURN_VOID();			/* keep compiler quiet */
}

/*
 * index_am_handler_out		- output routine for pseudo-type INDEX_AM_HANDLER.
 */
Datum
index_am_handler_out(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot display a value of type index_am_handler")));

	MDB_RETURN_VOID();			/* keep compiler quiet */
}


/*
 * tsm_handler_in		- input routine for pseudo-type TSM_HANDLER.
 */
Datum
tsm_handler_in(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot accept a value of type tsm_handler")));

	MDB_RETURN_VOID();			/* keep compiler quiet */
}

/*
 * tsm_handler_out		- output routine for pseudo-type TSM_HANDLER.
 */
Datum
tsm_handler_out(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot display a value of type tsm_handler")));

	MDB_RETURN_VOID();			/* keep compiler quiet */
}


/*
 * internal_in		- input routine for pseudo-type INTERNAL.
 */
Datum
internal_in(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot accept a value of type internal")));

	MDB_RETURN_VOID();			/* keep compiler quiet */
}

/*
 * internal_out		- output routine for pseudo-type INTERNAL.
 */
Datum
internal_out(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot display a value of type internal")));

	MDB_RETURN_VOID();			/* keep compiler quiet */
}


/*
 * opaque_in		- input routine for pseudo-type OPAQUE.
 */
Datum
opaque_in(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot accept a value of type opaque")));

	MDB_RETURN_VOID();			/* keep compiler quiet */
}

/*
 * opaque_out		- output routine for pseudo-type OPAQUE.
 */
Datum
opaque_out(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot display a value of type opaque")));

	MDB_RETURN_VOID();			/* keep compiler quiet */
}


/*
 * anyelement_in		- input routine for pseudo-type ANYELEMENT.
 */
Datum
anyelement_in(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot accept a value of type anyelement")));

	MDB_RETURN_VOID();			/* keep compiler quiet */
}

/*
 * anyelement_out		- output routine for pseudo-type ANYELEMENT.
 */
Datum
anyelement_out(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot display a value of type anyelement")));

	MDB_RETURN_VOID();			/* keep compiler quiet */
}

/*
 * anynonarray_in		- input routine for pseudo-type ANYNONARRAY.
 */
Datum
anynonarray_in(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot accept a value of type anynonarray")));

	MDB_RETURN_VOID();			/* keep compiler quiet */
}

/*
 * anynonarray_out		- output routine for pseudo-type ANYNONARRAY.
 */
Datum
anynonarray_out(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot display a value of type anynonarray")));

	MDB_RETURN_VOID();			/* keep compiler quiet */
}

/*
 * shell_in		- input routine for "shell" types (those not yet filled in).
 */
Datum
shell_in(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot accept a value of a shell type")));

	MDB_RETURN_VOID();			/* keep compiler quiet */
}

/*
 * shell_out		- output routine for "shell" types.
 */
Datum
shell_out(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot display a value of a shell type")));

	MDB_RETURN_VOID();			/* keep compiler quiet */
}


/*
 * mdb_node_tree_in		- input routine for type MDB_NODE_TREE.
 *
 * mdb_node_tree isn't really a pseudotype --- it's real enough to be a table
 * column --- but it presently has no operations of its own, and disallows
 * input too, so its I/O functions seem to fit here as much as anywhere.
 */
Datum
mdb_node_tree_in(MDB_FUNCTION_ARGS)
{
	/*
	 * We disallow input of mdb_node_tree values because the SQL functions that
	 * operate on the type are not secure against malformed input.
	 */
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot accept a value of type %s", "mdb_node_tree")));

	MDB_RETURN_VOID();			/* keep compiler quiet */
}


/*
 * mdb_node_tree_out		- output routine for type MDB_NODE_TREE.
 *
 * The internal representation is the same as TEXT, so just pass it off.
 */
Datum
mdb_node_tree_out(MDB_FUNCTION_ARGS)
{
	return textout(fcinfo);
}

/*
 * mdb_node_tree_recv		- binary input routine for type MDB_NODE_TREE.
 */
Datum
mdb_node_tree_recv(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot accept a value of type %s", "mdb_node_tree")));

	MDB_RETURN_VOID();			/* keep compiler quiet */
}

/*
 * mdb_node_tree_send		- binary output routine for type MDB_NODE_TREE.
 */
Datum
mdb_node_tree_send(MDB_FUNCTION_ARGS)
{
	return textsend(fcinfo);
}

/*
 * mdb_ddl_command_in	- input routine for type MDB_DDL_COMMAND.
 *
 * Like mdb_node_tree, mdb_ddl_command isn't really a pseudotype; it's here for
 * the same reasons as that one.
 */
Datum
mdb_ddl_command_in(MDB_FUNCTION_ARGS)
{
	/*
	 * Disallow input of mdb_ddl_command value.
	 */
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot accept a value of type %s", "mdb_ddl_command")));

	MDB_RETURN_VOID();			/* keep compiler quiet */
}

/*
 * mdb_ddl_command_out		- output routine for type MDB_DDL_COMMAND.
 *
 * We don't have any good way to output this type directly, so punt.
 */
Datum
mdb_ddl_command_out(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot output a value of type %s", "mdb_ddl_command")));

	MDB_RETURN_VOID();
}

/*
 * mdb_ddl_command_recv		- binary input routine for type MDB_DDL_COMMAND.
 */
Datum
mdb_ddl_command_recv(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot accept a value of type %s", "mdb_ddl_command")));

	MDB_RETURN_VOID();
}

/*
 * mdb_ddl_command_send		- binary output routine for type MDB_DDL_COMMAND.
 */
Datum
mdb_ddl_command_send(MDB_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot output a value of type %s", "mdb_ddl_command")));

	MDB_RETURN_VOID();
}
