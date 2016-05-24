/*-------------------------------------------------------------------------
 *
 * pgstatfuncs.c
 *	  Functions for accessing the statistics collector data
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/utils/adt/pgstatfuncs.c
 *
 *-------------------------------------------------------------------------
 */
#include "mollydb.h"

#include "access/htup_details.h"
#include "catalog/mdb_type.h"
#include "funcapi.h"
#include "libpq/ip.h"
#include "miscadmin.h"
#include "pgstat.h"
#include "storage/proc.h"
#include "storage/procarray.h"
#include "utils/acl.h"
#include "utils/builtins.h"
#include "utils/inet.h"
#include "utils/timestamp.h"

#define UINT32_ACCESS_ONCE(var)      ((uint32)(*((volatile uint32 *)&(var))))

/* bogus ... these externs should be in a header file */
extern Datum mdb_stat_get_numscans(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_tuples_returned(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_tuples_fetched(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_tuples_inserted(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_tuples_updated(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_tuples_deleted(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_tuples_hot_updated(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_live_tuples(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_dead_tuples(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_mod_since_analyze(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_blocks_fetched(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_blocks_hit(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_last_vacuum_time(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_last_autovacuum_time(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_last_analyze_time(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_last_autoanalyze_time(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_vacuum_count(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_autovacuum_count(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_analyze_count(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_autoanalyze_count(MDB_FUNCTION_ARGS);

extern Datum mdb_stat_get_function_calls(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_function_total_time(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_function_self_time(MDB_FUNCTION_ARGS);

extern Datum mdb_stat_get_backend_idset(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_activity(MDB_FUNCTION_ARGS);
extern Datum mdb_backend_pid(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_backend_pid(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_backend_dbid(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_backend_userid(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_backend_activity(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_backend_wait_event_type(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_backend_wait_event(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_backend_activity_start(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_backend_xact_start(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_backend_start(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_backend_client_addr(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_backend_client_port(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_progress_info(MDB_FUNCTION_ARGS);

extern Datum mdb_stat_get_db_numbackends(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_db_xact_commit(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_db_xact_rollback(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_db_blocks_fetched(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_db_blocks_hit(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_db_tuples_returned(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_db_tuples_fetched(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_db_tuples_inserted(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_db_tuples_updated(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_db_tuples_deleted(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_db_conflict_tablespace(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_db_conflict_lock(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_db_conflict_snapshot(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_db_conflict_bufferpin(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_db_conflict_startup_deadlock(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_db_conflict_all(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_db_deadlocks(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_db_stat_reset_time(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_db_temp_files(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_db_temp_bytes(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_db_blk_read_time(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_db_blk_write_time(MDB_FUNCTION_ARGS);

extern Datum mdb_stat_get_archiver(MDB_FUNCTION_ARGS);

extern Datum mdb_stat_get_bgwriter_timed_checkpoints(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_bgwriter_requested_checkpoints(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_checkpoint_write_time(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_checkpoint_sync_time(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_bgwriter_buf_written_checkpoints(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_bgwriter_buf_written_clean(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_bgwriter_maxwritten_clean(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_bgwriter_stat_reset_time(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_buf_written_backend(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_buf_fsync_backend(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_buf_alloc(MDB_FUNCTION_ARGS);

extern Datum mdb_stat_get_xact_numscans(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_xact_tuples_returned(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_xact_tuples_fetched(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_xact_tuples_inserted(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_xact_tuples_updated(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_xact_tuples_deleted(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_xact_tuples_hot_updated(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_xact_blocks_fetched(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_xact_blocks_hit(MDB_FUNCTION_ARGS);

extern Datum mdb_stat_get_xact_function_calls(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_xact_function_total_time(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_get_xact_function_self_time(MDB_FUNCTION_ARGS);

extern Datum mdb_stat_get_snapshot_timestamp(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_clear_snapshot(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_reset(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_reset_shared(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_reset_single_table_counters(MDB_FUNCTION_ARGS);
extern Datum mdb_stat_reset_single_function_counters(MDB_FUNCTION_ARGS);

/* Global bgwriter statistics, from bgwriter.c */
extern PgStat_MsgBgWriter bgwriterStats;

Datum
mdb_stat_get_numscans(MDB_FUNCTION_ARGS)
{
	Oid			relid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatTabEntry *tabentry;

	if ((tabentry = pgstat_fetch_stat_tabentry(relid)) == NULL)
		result = 0;
	else
		result = (int64) (tabentry->numscans);

	MDB_RETURN_INT64(result);
}


Datum
mdb_stat_get_tuples_returned(MDB_FUNCTION_ARGS)
{
	Oid			relid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatTabEntry *tabentry;

	if ((tabentry = pgstat_fetch_stat_tabentry(relid)) == NULL)
		result = 0;
	else
		result = (int64) (tabentry->tuples_returned);

	MDB_RETURN_INT64(result);
}


Datum
mdb_stat_get_tuples_fetched(MDB_FUNCTION_ARGS)
{
	Oid			relid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatTabEntry *tabentry;

	if ((tabentry = pgstat_fetch_stat_tabentry(relid)) == NULL)
		result = 0;
	else
		result = (int64) (tabentry->tuples_fetched);

	MDB_RETURN_INT64(result);
}


Datum
mdb_stat_get_tuples_inserted(MDB_FUNCTION_ARGS)
{
	Oid			relid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatTabEntry *tabentry;

	if ((tabentry = pgstat_fetch_stat_tabentry(relid)) == NULL)
		result = 0;
	else
		result = (int64) (tabentry->tuples_inserted);

	MDB_RETURN_INT64(result);
}


Datum
mdb_stat_get_tuples_updated(MDB_FUNCTION_ARGS)
{
	Oid			relid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatTabEntry *tabentry;

	if ((tabentry = pgstat_fetch_stat_tabentry(relid)) == NULL)
		result = 0;
	else
		result = (int64) (tabentry->tuples_updated);

	MDB_RETURN_INT64(result);
}


Datum
mdb_stat_get_tuples_deleted(MDB_FUNCTION_ARGS)
{
	Oid			relid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatTabEntry *tabentry;

	if ((tabentry = pgstat_fetch_stat_tabentry(relid)) == NULL)
		result = 0;
	else
		result = (int64) (tabentry->tuples_deleted);

	MDB_RETURN_INT64(result);
}


Datum
mdb_stat_get_tuples_hot_updated(MDB_FUNCTION_ARGS)
{
	Oid			relid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatTabEntry *tabentry;

	if ((tabentry = pgstat_fetch_stat_tabentry(relid)) == NULL)
		result = 0;
	else
		result = (int64) (tabentry->tuples_hot_updated);

	MDB_RETURN_INT64(result);
}


Datum
mdb_stat_get_live_tuples(MDB_FUNCTION_ARGS)
{
	Oid			relid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatTabEntry *tabentry;

	if ((tabentry = pgstat_fetch_stat_tabentry(relid)) == NULL)
		result = 0;
	else
		result = (int64) (tabentry->n_live_tuples);

	MDB_RETURN_INT64(result);
}


Datum
mdb_stat_get_dead_tuples(MDB_FUNCTION_ARGS)
{
	Oid			relid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatTabEntry *tabentry;

	if ((tabentry = pgstat_fetch_stat_tabentry(relid)) == NULL)
		result = 0;
	else
		result = (int64) (tabentry->n_dead_tuples);

	MDB_RETURN_INT64(result);
}


Datum
mdb_stat_get_mod_since_analyze(MDB_FUNCTION_ARGS)
{
	Oid			relid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatTabEntry *tabentry;

	if ((tabentry = pgstat_fetch_stat_tabentry(relid)) == NULL)
		result = 0;
	else
		result = (int64) (tabentry->changes_since_analyze);

	MDB_RETURN_INT64(result);
}


Datum
mdb_stat_get_blocks_fetched(MDB_FUNCTION_ARGS)
{
	Oid			relid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatTabEntry *tabentry;

	if ((tabentry = pgstat_fetch_stat_tabentry(relid)) == NULL)
		result = 0;
	else
		result = (int64) (tabentry->blocks_fetched);

	MDB_RETURN_INT64(result);
}


Datum
mdb_stat_get_blocks_hit(MDB_FUNCTION_ARGS)
{
	Oid			relid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatTabEntry *tabentry;

	if ((tabentry = pgstat_fetch_stat_tabentry(relid)) == NULL)
		result = 0;
	else
		result = (int64) (tabentry->blocks_hit);

	MDB_RETURN_INT64(result);
}

Datum
mdb_stat_get_last_vacuum_time(MDB_FUNCTION_ARGS)
{
	Oid			relid = MDB_GETARG_OID(0);
	TimestampTz result;
	PgStat_StatTabEntry *tabentry;

	if ((tabentry = pgstat_fetch_stat_tabentry(relid)) == NULL)
		result = 0;
	else
		result = tabentry->vacuum_timestamp;

	if (result == 0)
		MDB_RETURN_NULL();
	else
		MDB_RETURN_TIMESTAMPTZ(result);
}

Datum
mdb_stat_get_last_autovacuum_time(MDB_FUNCTION_ARGS)
{
	Oid			relid = MDB_GETARG_OID(0);
	TimestampTz result;
	PgStat_StatTabEntry *tabentry;

	if ((tabentry = pgstat_fetch_stat_tabentry(relid)) == NULL)
		result = 0;
	else
		result = tabentry->autovac_vacuum_timestamp;

	if (result == 0)
		MDB_RETURN_NULL();
	else
		MDB_RETURN_TIMESTAMPTZ(result);
}

Datum
mdb_stat_get_last_analyze_time(MDB_FUNCTION_ARGS)
{
	Oid			relid = MDB_GETARG_OID(0);
	TimestampTz result;
	PgStat_StatTabEntry *tabentry;

	if ((tabentry = pgstat_fetch_stat_tabentry(relid)) == NULL)
		result = 0;
	else
		result = tabentry->analyze_timestamp;

	if (result == 0)
		MDB_RETURN_NULL();
	else
		MDB_RETURN_TIMESTAMPTZ(result);
}

Datum
mdb_stat_get_last_autoanalyze_time(MDB_FUNCTION_ARGS)
{
	Oid			relid = MDB_GETARG_OID(0);
	TimestampTz result;
	PgStat_StatTabEntry *tabentry;

	if ((tabentry = pgstat_fetch_stat_tabentry(relid)) == NULL)
		result = 0;
	else
		result = tabentry->autovac_analyze_timestamp;

	if (result == 0)
		MDB_RETURN_NULL();
	else
		MDB_RETURN_TIMESTAMPTZ(result);
}

Datum
mdb_stat_get_vacuum_count(MDB_FUNCTION_ARGS)
{
	Oid			relid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatTabEntry *tabentry;

	if ((tabentry = pgstat_fetch_stat_tabentry(relid)) == NULL)
		result = 0;
	else
		result = (int64) (tabentry->vacuum_count);

	MDB_RETURN_INT64(result);
}

Datum
mdb_stat_get_autovacuum_count(MDB_FUNCTION_ARGS)
{
	Oid			relid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatTabEntry *tabentry;

	if ((tabentry = pgstat_fetch_stat_tabentry(relid)) == NULL)
		result = 0;
	else
		result = (int64) (tabentry->autovac_vacuum_count);

	MDB_RETURN_INT64(result);
}

Datum
mdb_stat_get_analyze_count(MDB_FUNCTION_ARGS)
{
	Oid			relid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatTabEntry *tabentry;

	if ((tabentry = pgstat_fetch_stat_tabentry(relid)) == NULL)
		result = 0;
	else
		result = (int64) (tabentry->analyze_count);

	MDB_RETURN_INT64(result);
}

Datum
mdb_stat_get_autoanalyze_count(MDB_FUNCTION_ARGS)
{
	Oid			relid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatTabEntry *tabentry;

	if ((tabentry = pgstat_fetch_stat_tabentry(relid)) == NULL)
		result = 0;
	else
		result = (int64) (tabentry->autovac_analyze_count);

	MDB_RETURN_INT64(result);
}

Datum
mdb_stat_get_function_calls(MDB_FUNCTION_ARGS)
{
	Oid			funcid = MDB_GETARG_OID(0);
	PgStat_StatFuncEntry *funcentry;

	if ((funcentry = pgstat_fetch_stat_funcentry(funcid)) == NULL)
		MDB_RETURN_NULL();
	MDB_RETURN_INT64(funcentry->f_numcalls);
}

Datum
mdb_stat_get_function_total_time(MDB_FUNCTION_ARGS)
{
	Oid			funcid = MDB_GETARG_OID(0);
	PgStat_StatFuncEntry *funcentry;

	if ((funcentry = pgstat_fetch_stat_funcentry(funcid)) == NULL)
		MDB_RETURN_NULL();
	/* convert counter from microsec to millisec for display */
	MDB_RETURN_FLOAT8(((double) funcentry->f_total_time) / 1000.0);
}

Datum
mdb_stat_get_function_self_time(MDB_FUNCTION_ARGS)
{
	Oid			funcid = MDB_GETARG_OID(0);
	PgStat_StatFuncEntry *funcentry;

	if ((funcentry = pgstat_fetch_stat_funcentry(funcid)) == NULL)
		MDB_RETURN_NULL();
	/* convert counter from microsec to millisec for display */
	MDB_RETURN_FLOAT8(((double) funcentry->f_self_time) / 1000.0);
}

Datum
mdb_stat_get_backend_idset(MDB_FUNCTION_ARGS)
{
	FuncCallContext *funcctx;
	int		   *fctx;
	int32		result;

	/* stuff done only on the first call of the function */
	if (SRF_IS_FIRSTCALL())
	{
		/* create a function context for cross-call persistence */
		funcctx = SRF_FIRSTCALL_INIT();

		fctx = MemoryContextAlloc(funcctx->multi_call_memory_ctx,
								  2 * sizeof(int));
		funcctx->user_fctx = fctx;

		fctx[0] = 0;
		fctx[1] = pgstat_fetch_stat_numbackends();
	}

	/* stuff done on every call of the function */
	funcctx = SRF_PERCALL_SETUP();
	fctx = funcctx->user_fctx;

	fctx[0] += 1;
	result = fctx[0];

	if (result <= fctx[1])
	{
		/* do when there is more left to send */
		SRF_RETURN_NEXT(funcctx, Int32GetDatum(result));
	}
	else
	{
		/* do when there is no more left */
		SRF_RETURN_DONE(funcctx);
	}
}

/*
 * Returns command progress information for the named command.
 */
Datum
mdb_stat_get_progress_info(MDB_FUNCTION_ARGS)
{
#define MDB_STAT_GET_PROGRESS_COLS	PGSTAT_NUM_PROGRESS_PARAM + 3
	int			num_backends = pgstat_fetch_stat_numbackends();
	int			curr_backend;
	char	   *cmd = text_to_cstring(MDB_GETARG_TEXT_PP(0));
	ProgressCommandType	cmdtype;
	TupleDesc	tupdesc;
	Tuplestorestate *tupstore;
	ReturnSetInfo *rsinfo = (ReturnSetInfo *) fcinfo->resultinfo;
	MemoryContext per_query_ctx;
	MemoryContext oldcontext;

	/* check to see if caller supports us returning a tuplestore */
	if (rsinfo == NULL || !IsA(rsinfo, ReturnSetInfo))
		ereport(ERROR,
				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
				 errmsg("set-valued function called in context that cannot accept a set")));
	if (!(rsinfo->allowedModes & SFRM_Materialize))
		ereport(ERROR,
				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
				 errmsg("materialize mode required, but it is not " \
						"allowed in this context")));

	/* Build a tuple descriptor for our result type */
	if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE)
		elog(ERROR, "return type must be a row type");

	/* Translate command name into command type code. */
	if (mdb_strcasecmp(cmd, "VACUUM") == 0)
		cmdtype = PROGRESS_COMMAND_VACUUM;
	else
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				 errmsg("invalid command name: \"%s\"", cmd)));

	per_query_ctx = rsinfo->econtext->ecxt_per_query_memory;
	oldcontext = MemoryContextSwitchTo(per_query_ctx);

	tupstore = tuplestore_begin_heap(true, false, work_mem);
	rsinfo->returnMode = SFRM_Materialize;
	rsinfo->setResult = tupstore;
	rsinfo->setDesc = tupdesc;
	MemoryContextSwitchTo(oldcontext);

	/* 1-based index */
	for (curr_backend = 1; curr_backend <= num_backends; curr_backend++)
	{
		LocalPgBackendStatus   *local_beentry;
		PgBackendStatus		   *beentry;
		Datum		values[MDB_STAT_GET_PROGRESS_COLS];
		bool		nulls[MDB_STAT_GET_PROGRESS_COLS];
		int			i;

		MemSet(values, 0, sizeof(values));
		MemSet(nulls, 0, sizeof(nulls));

		local_beentry = pgstat_fetch_stat_local_beentry(curr_backend);

		if (!local_beentry)
			continue;

		beentry = &local_beentry->backendStatus;

		/*
		 * Report values for only those backends which are running the given
		 * command.
		 */
		if (!beentry || beentry->st_progress_command != cmdtype)
			continue;

		/* Value available to all callers */
		values[0] = Int32GetDatum(beentry->st_procpid);
		values[1] = ObjectIdGetDatum(beentry->st_databaseid);

		/* show rest of the values including relid only to role members */
		if (has_privs_of_role(GetUserId(), beentry->st_userid))
		{
			values[2] = ObjectIdGetDatum(beentry->st_progress_command_target);
			for(i = 0; i < PGSTAT_NUM_PROGRESS_PARAM; i++)
				values[i+3] = Int64GetDatum(beentry->st_progress_param[i]);
		}
		else
		{
			nulls[2] = true;
			for (i = 0; i < PGSTAT_NUM_PROGRESS_PARAM; i++)
				nulls[i+3] = true;
		}

		tuplestore_putvalues(tupstore, tupdesc, values, nulls);
	}

	/* clean up and return the tuplestore */
	tuplestore_donestoring(tupstore);

	return (Datum) 0;
}

/*
 * Returns activity of PG backends.
 */
Datum
mdb_stat_get_activity(MDB_FUNCTION_ARGS)
{
#define MDB_STAT_GET_ACTIVITY_COLS	23
	int			num_backends = pgstat_fetch_stat_numbackends();
	int			curr_backend;
	int			pid = MDB_ARGISNULL(0) ? -1 : MDB_GETARG_INT32(0);
	ReturnSetInfo *rsinfo = (ReturnSetInfo *) fcinfo->resultinfo;
	TupleDesc	tupdesc;
	Tuplestorestate *tupstore;
	MemoryContext per_query_ctx;
	MemoryContext oldcontext;

	/* check to see if caller supports us returning a tuplestore */
	if (rsinfo == NULL || !IsA(rsinfo, ReturnSetInfo))
		ereport(ERROR,
				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
				 errmsg("set-valued function called in context that cannot accept a set")));
	if (!(rsinfo->allowedModes & SFRM_Materialize))
		ereport(ERROR,
				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
				 errmsg("materialize mode required, but it is not " \
						"allowed in this context")));

	/* Build a tuple descriptor for our result type */
	if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE)
		elog(ERROR, "return type must be a row type");

	per_query_ctx = rsinfo->econtext->ecxt_per_query_memory;
	oldcontext = MemoryContextSwitchTo(per_query_ctx);

	tupstore = tuplestore_begin_heap(true, false, work_mem);
	rsinfo->returnMode = SFRM_Materialize;
	rsinfo->setResult = tupstore;
	rsinfo->setDesc = tupdesc;

	MemoryContextSwitchTo(oldcontext);

	/* 1-based index */
	for (curr_backend = 1; curr_backend <= num_backends; curr_backend++)
	{
		/* for each row */
		Datum		values[MDB_STAT_GET_ACTIVITY_COLS];
		bool		nulls[MDB_STAT_GET_ACTIVITY_COLS];
		LocalPgBackendStatus *local_beentry;
		PgBackendStatus *beentry;
		PGPROC	   *proc;
		const char *wait_event_type;
		const char *wait_event;

		MemSet(values, 0, sizeof(values));
		MemSet(nulls, 0, sizeof(nulls));

		if (pid != -1)
		{
			/* Skip any which are not the one we're looking for. */
			PgBackendStatus *be = pgstat_fetch_stat_beentry(curr_backend);

			if (!be || be->st_procpid != pid)
				continue;

		}

		/* Get the next one in the list */
		local_beentry = pgstat_fetch_stat_local_beentry(curr_backend);
		if (!local_beentry)
			continue;

		beentry = &local_beentry->backendStatus;
		if (!beentry)
		{
			int			i;

			for (i = 0; i < sizeof(nulls) / sizeof(nulls[0]); i++)
				nulls[i] = true;

			nulls[5] = false;
			values[5] = CStringGetTextDatum("<backend information not available>");

			tuplestore_putvalues(tupstore, tupdesc, values, nulls);
			continue;
		}

		/* Values available to all callers */
		values[0] = ObjectIdGetDatum(beentry->st_databaseid);
		values[1] = Int32GetDatum(beentry->st_procpid);
		values[2] = ObjectIdGetDatum(beentry->st_userid);
		if (beentry->st_appname)
			values[3] = CStringGetTextDatum(beentry->st_appname);
		else
			nulls[3] = true;

		if (TransactionIdIsValid(local_beentry->backend_xid))
			values[15] = TransactionIdGetDatum(local_beentry->backend_xid);
		else
			nulls[15] = true;

		if (TransactionIdIsValid(local_beentry->backend_xmin))
			values[16] = TransactionIdGetDatum(local_beentry->backend_xmin);
		else
			nulls[16] = true;

		if (beentry->st_ssl)
		{
			values[17] = BoolGetDatum(true);	/* ssl */
			values[18] = CStringGetTextDatum(beentry->st_sslstatus->ssl_version);
			values[19] = CStringGetTextDatum(beentry->st_sslstatus->ssl_cipher);
			values[20] = Int32GetDatum(beentry->st_sslstatus->ssl_bits);
			values[21] = BoolGetDatum(beentry->st_sslstatus->ssl_compression);
			values[22] = CStringGetTextDatum(beentry->st_sslstatus->ssl_clientdn);
		}
		else
		{
			values[17] = BoolGetDatum(false);	/* ssl */
			nulls[18] = nulls[19] = nulls[20] = nulls[21] = nulls[22] = true;
		}

		/* Values only available to role member */
		if (has_privs_of_role(GetUserId(), beentry->st_userid))
		{
			SockAddr	zero_clientaddr;

			switch (beentry->st_state)
			{
				case STATE_IDLE:
					values[4] = CStringGetTextDatum("idle");
					break;
				case STATE_RUNNING:
					values[4] = CStringGetTextDatum("active");
					break;
				case STATE_IDLEINTRANSACTION:
					values[4] = CStringGetTextDatum("idle in transaction");
					break;
				case STATE_FASTPATH:
					values[4] = CStringGetTextDatum("fastpath function call");
					break;
				case STATE_IDLEINTRANSACTION_ABORTED:
					values[4] = CStringGetTextDatum("idle in transaction (aborted)");
					break;
				case STATE_DISABLED:
					values[4] = CStringGetTextDatum("disabled");
					break;
				case STATE_UNDEFINED:
					nulls[4] = true;
					break;
			}

			values[5] = CStringGetTextDatum(beentry->st_activity);

			proc = BackendPidGetProc(beentry->st_procpid);
			if (proc != NULL)
			{
				uint32	raw_wait_event;

				raw_wait_event = UINT32_ACCESS_ONCE(proc->wait_event_info);
				wait_event_type = pgstat_get_wait_event_type(raw_wait_event);
				wait_event = pgstat_get_wait_event(raw_wait_event);

			}
			else
			{
				wait_event_type = NULL;
				wait_event = NULL;
			}

			if (wait_event_type)
				values[6] = CStringGetTextDatum(wait_event_type);
			else
				nulls[6] = true;

			if (wait_event)
				values[7] = CStringGetTextDatum(wait_event);
			else
				nulls[7] = true;

			if (beentry->st_xact_start_timestamp != 0)
				values[8] = TimestampTzGetDatum(beentry->st_xact_start_timestamp);
			else
				nulls[8] = true;

			if (beentry->st_activity_start_timestamp != 0)
				values[9] = TimestampTzGetDatum(beentry->st_activity_start_timestamp);
			else
				nulls[9] = true;

			if (beentry->st_proc_start_timestamp != 0)
				values[10] = TimestampTzGetDatum(beentry->st_proc_start_timestamp);
			else
				nulls[10] = true;

			if (beentry->st_state_start_timestamp != 0)
				values[11] = TimestampTzGetDatum(beentry->st_state_start_timestamp);
			else
				nulls[11] = true;

			/* A zeroed client addr means we don't know */
			memset(&zero_clientaddr, 0, sizeof(zero_clientaddr));
			if (memcmp(&(beentry->st_clientaddr), &zero_clientaddr,
					   sizeof(zero_clientaddr)) == 0)
			{
				nulls[12] = true;
				nulls[13] = true;
				nulls[14] = true;
			}
			else
			{
				if (beentry->st_clientaddr.addr.ss_family == AF_INET
#ifdef HAVE_IPV6
					|| beentry->st_clientaddr.addr.ss_family == AF_INET6
#endif
					)
				{
					char		remote_host[NI_MAXHOST];
					char		remote_port[NI_MAXSERV];
					int			ret;

					remote_host[0] = '\0';
					remote_port[0] = '\0';
					ret = mdb_getnameinfo_all(&beentry->st_clientaddr.addr,
											 beentry->st_clientaddr.salen,
											 remote_host, sizeof(remote_host),
											 remote_port, sizeof(remote_port),
											 NI_NUMERICHOST | NI_NUMERICSERV);
					if (ret == 0)
					{
						clean_ipv6_addr(beentry->st_clientaddr.addr.ss_family, remote_host);
						values[12] = DirectFunctionCall1(inet_in,
											   CStringGetDatum(remote_host));
						if (beentry->st_clienthostname &&
							beentry->st_clienthostname[0])
							values[13] = CStringGetTextDatum(beentry->st_clienthostname);
						else
							nulls[13] = true;
						values[14] = Int32GetDatum(atoi(remote_port));
					}
					else
					{
						nulls[12] = true;
						nulls[13] = true;
						nulls[14] = true;
					}
				}
				else if (beentry->st_clientaddr.addr.ss_family == AF_UNIX)
				{
					/*
					 * Unix sockets always reports NULL for host and -1 for
					 * port, so it's possible to tell the difference to
					 * connections we have no permissions to view, or with
					 * errors.
					 */
					nulls[12] = true;
					nulls[13] = true;
					values[14] = DatumGetInt32(-1);
				}
				else
				{
					/* Unknown address type, should never happen */
					nulls[12] = true;
					nulls[13] = true;
					nulls[14] = true;
				}
			}
		}
		else
		{
			/* No permissions to view data about this session */
			values[5] = CStringGetTextDatum("<insufficient privilege>");
			nulls[4] = true;
			nulls[6] = true;
			nulls[7] = true;
			nulls[8] = true;
			nulls[9] = true;
			nulls[10] = true;
			nulls[11] = true;
			nulls[12] = true;
			nulls[13] = true;
			nulls[14] = true;
		}

		tuplestore_putvalues(tupstore, tupdesc, values, nulls);

		/* If only a single backend was requested, and we found it, break. */
		if (pid != -1)
			break;
	}

	/* clean up and return the tuplestore */
	tuplestore_donestoring(tupstore);

	return (Datum) 0;
}


Datum
mdb_backend_pid(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_INT32(MyProcPid);
}


Datum
mdb_stat_get_backend_pid(MDB_FUNCTION_ARGS)
{
	int32		beid = MDB_GETARG_INT32(0);
	PgBackendStatus *beentry;

	if ((beentry = pgstat_fetch_stat_beentry(beid)) == NULL)
		MDB_RETURN_NULL();

	MDB_RETURN_INT32(beentry->st_procpid);
}


Datum
mdb_stat_get_backend_dbid(MDB_FUNCTION_ARGS)
{
	int32		beid = MDB_GETARG_INT32(0);
	PgBackendStatus *beentry;

	if ((beentry = pgstat_fetch_stat_beentry(beid)) == NULL)
		MDB_RETURN_NULL();

	MDB_RETURN_OID(beentry->st_databaseid);
}


Datum
mdb_stat_get_backend_userid(MDB_FUNCTION_ARGS)
{
	int32		beid = MDB_GETARG_INT32(0);
	PgBackendStatus *beentry;

	if ((beentry = pgstat_fetch_stat_beentry(beid)) == NULL)
		MDB_RETURN_NULL();

	MDB_RETURN_OID(beentry->st_userid);
}


Datum
mdb_stat_get_backend_activity(MDB_FUNCTION_ARGS)
{
	int32		beid = MDB_GETARG_INT32(0);
	PgBackendStatus *beentry;
	const char *activity;

	if ((beentry = pgstat_fetch_stat_beentry(beid)) == NULL)
		activity = "<backend information not available>";
	else if (!has_privs_of_role(GetUserId(), beentry->st_userid))
		activity = "<insufficient privilege>";
	else if (*(beentry->st_activity) == '\0')
		activity = "<command string not enabled>";
	else
		activity = beentry->st_activity;

	MDB_RETURN_TEXT_P(cstring_to_text(activity));
}

Datum
mdb_stat_get_backend_wait_event_type(MDB_FUNCTION_ARGS)
{
	int32		beid = MDB_GETARG_INT32(0);
	PgBackendStatus *beentry;
	PGPROC	   *proc;
	const char *wait_event_type = NULL;

	if ((beentry = pgstat_fetch_stat_beentry(beid)) == NULL)
		wait_event_type = "<backend information not available>";
	else if (!has_privs_of_role(GetUserId(), beentry->st_userid))
		wait_event_type = "<insufficient privilege>";
	else if ((proc = BackendPidGetProc(beentry->st_procpid)) != NULL)
		wait_event_type = pgstat_get_wait_event_type(proc->wait_event_info);

	if (!wait_event_type)
		MDB_RETURN_NULL();

	MDB_RETURN_TEXT_P(cstring_to_text(wait_event_type));
}

Datum
mdb_stat_get_backend_wait_event(MDB_FUNCTION_ARGS)
{
	int32		beid = MDB_GETARG_INT32(0);
	PgBackendStatus *beentry;
	PGPROC	   *proc;
	const char *wait_event = NULL;

	if ((beentry = pgstat_fetch_stat_beentry(beid)) == NULL)
		wait_event = "<backend information not available>";
	else if (!has_privs_of_role(GetUserId(), beentry->st_userid))
		wait_event = "<insufficient privilege>";
	else if ((proc = BackendPidGetProc(beentry->st_procpid)) != NULL)
		wait_event = pgstat_get_wait_event(proc->wait_event_info);

	if (!wait_event)
		MDB_RETURN_NULL();

	MDB_RETURN_TEXT_P(cstring_to_text(wait_event));
}


Datum
mdb_stat_get_backend_activity_start(MDB_FUNCTION_ARGS)
{
	int32		beid = MDB_GETARG_INT32(0);
	TimestampTz result;
	PgBackendStatus *beentry;

	if ((beentry = pgstat_fetch_stat_beentry(beid)) == NULL)
		MDB_RETURN_NULL();

	if (!has_privs_of_role(GetUserId(), beentry->st_userid))
		MDB_RETURN_NULL();

	result = beentry->st_activity_start_timestamp;

	/*
	 * No time recorded for start of current query -- this is the case if the
	 * user hasn't enabled query-level stats collection.
	 */
	if (result == 0)
		MDB_RETURN_NULL();

	MDB_RETURN_TIMESTAMPTZ(result);
}


Datum
mdb_stat_get_backend_xact_start(MDB_FUNCTION_ARGS)
{
	int32		beid = MDB_GETARG_INT32(0);
	TimestampTz result;
	PgBackendStatus *beentry;

	if ((beentry = pgstat_fetch_stat_beentry(beid)) == NULL)
		MDB_RETURN_NULL();

	if (!has_privs_of_role(GetUserId(), beentry->st_userid))
		MDB_RETURN_NULL();

	result = beentry->st_xact_start_timestamp;

	if (result == 0)			/* not in a transaction */
		MDB_RETURN_NULL();

	MDB_RETURN_TIMESTAMPTZ(result);
}


Datum
mdb_stat_get_backend_start(MDB_FUNCTION_ARGS)
{
	int32		beid = MDB_GETARG_INT32(0);
	TimestampTz result;
	PgBackendStatus *beentry;

	if ((beentry = pgstat_fetch_stat_beentry(beid)) == NULL)
		MDB_RETURN_NULL();

	if (!has_privs_of_role(GetUserId(), beentry->st_userid))
		MDB_RETURN_NULL();

	result = beentry->st_proc_start_timestamp;

	if (result == 0)			/* probably can't happen? */
		MDB_RETURN_NULL();

	MDB_RETURN_TIMESTAMPTZ(result);
}


Datum
mdb_stat_get_backend_client_addr(MDB_FUNCTION_ARGS)
{
	int32		beid = MDB_GETARG_INT32(0);
	PgBackendStatus *beentry;
	SockAddr	zero_clientaddr;
	char		remote_host[NI_MAXHOST];
	int			ret;

	if ((beentry = pgstat_fetch_stat_beentry(beid)) == NULL)
		MDB_RETURN_NULL();

	if (!has_privs_of_role(GetUserId(), beentry->st_userid))
		MDB_RETURN_NULL();

	/* A zeroed client addr means we don't know */
	memset(&zero_clientaddr, 0, sizeof(zero_clientaddr));
	if (memcmp(&(beentry->st_clientaddr), &zero_clientaddr,
			   sizeof(zero_clientaddr)) == 0)
		MDB_RETURN_NULL();

	switch (beentry->st_clientaddr.addr.ss_family)
	{
		case AF_INET:
#ifdef HAVE_IPV6
		case AF_INET6:
#endif
			break;
		default:
			MDB_RETURN_NULL();
	}

	remote_host[0] = '\0';
	ret = mdb_getnameinfo_all(&beentry->st_clientaddr.addr,
							 beentry->st_clientaddr.salen,
							 remote_host, sizeof(remote_host),
							 NULL, 0,
							 NI_NUMERICHOST | NI_NUMERICSERV);
	if (ret != 0)
		MDB_RETURN_NULL();

	clean_ipv6_addr(beentry->st_clientaddr.addr.ss_family, remote_host);

	MDB_RETURN_INET_P(DirectFunctionCall1(inet_in,
										 CStringGetDatum(remote_host)));
}

Datum
mdb_stat_get_backend_client_port(MDB_FUNCTION_ARGS)
{
	int32		beid = MDB_GETARG_INT32(0);
	PgBackendStatus *beentry;
	SockAddr	zero_clientaddr;
	char		remote_port[NI_MAXSERV];
	int			ret;

	if ((beentry = pgstat_fetch_stat_beentry(beid)) == NULL)
		MDB_RETURN_NULL();

	if (!has_privs_of_role(GetUserId(), beentry->st_userid))
		MDB_RETURN_NULL();

	/* A zeroed client addr means we don't know */
	memset(&zero_clientaddr, 0, sizeof(zero_clientaddr));
	if (memcmp(&(beentry->st_clientaddr), &zero_clientaddr,
			   sizeof(zero_clientaddr)) == 0)
		MDB_RETURN_NULL();

	switch (beentry->st_clientaddr.addr.ss_family)
	{
		case AF_INET:
#ifdef HAVE_IPV6
		case AF_INET6:
#endif
			break;
		case AF_UNIX:
			MDB_RETURN_INT32(-1);
		default:
			MDB_RETURN_NULL();
	}

	remote_port[0] = '\0';
	ret = mdb_getnameinfo_all(&beentry->st_clientaddr.addr,
							 beentry->st_clientaddr.salen,
							 NULL, 0,
							 remote_port, sizeof(remote_port),
							 NI_NUMERICHOST | NI_NUMERICSERV);
	if (ret != 0)
		MDB_RETURN_NULL();

	MDB_RETURN_DATUM(DirectFunctionCall1(int4in,
										CStringGetDatum(remote_port)));
}


Datum
mdb_stat_get_db_numbackends(MDB_FUNCTION_ARGS)
{
	Oid			dbid = MDB_GETARG_OID(0);
	int32		result;
	int			tot_backends = pgstat_fetch_stat_numbackends();
	int			beid;

	result = 0;
	for (beid = 1; beid <= tot_backends; beid++)
	{
		PgBackendStatus *beentry = pgstat_fetch_stat_beentry(beid);

		if (beentry && beentry->st_databaseid == dbid)
			result++;
	}

	MDB_RETURN_INT32(result);
}


Datum
mdb_stat_get_db_xact_commit(MDB_FUNCTION_ARGS)
{
	Oid			dbid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatDBEntry *dbentry;

	if ((dbentry = pgstat_fetch_stat_dbentry(dbid)) == NULL)
		result = 0;
	else
		result = (int64) (dbentry->n_xact_commit);

	MDB_RETURN_INT64(result);
}


Datum
mdb_stat_get_db_xact_rollback(MDB_FUNCTION_ARGS)
{
	Oid			dbid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatDBEntry *dbentry;

	if ((dbentry = pgstat_fetch_stat_dbentry(dbid)) == NULL)
		result = 0;
	else
		result = (int64) (dbentry->n_xact_rollback);

	MDB_RETURN_INT64(result);
}


Datum
mdb_stat_get_db_blocks_fetched(MDB_FUNCTION_ARGS)
{
	Oid			dbid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatDBEntry *dbentry;

	if ((dbentry = pgstat_fetch_stat_dbentry(dbid)) == NULL)
		result = 0;
	else
		result = (int64) (dbentry->n_blocks_fetched);

	MDB_RETURN_INT64(result);
}


Datum
mdb_stat_get_db_blocks_hit(MDB_FUNCTION_ARGS)
{
	Oid			dbid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatDBEntry *dbentry;

	if ((dbentry = pgstat_fetch_stat_dbentry(dbid)) == NULL)
		result = 0;
	else
		result = (int64) (dbentry->n_blocks_hit);

	MDB_RETURN_INT64(result);
}


Datum
mdb_stat_get_db_tuples_returned(MDB_FUNCTION_ARGS)
{
	Oid			dbid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatDBEntry *dbentry;

	if ((dbentry = pgstat_fetch_stat_dbentry(dbid)) == NULL)
		result = 0;
	else
		result = (int64) (dbentry->n_tuples_returned);

	MDB_RETURN_INT64(result);
}


Datum
mdb_stat_get_db_tuples_fetched(MDB_FUNCTION_ARGS)
{
	Oid			dbid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatDBEntry *dbentry;

	if ((dbentry = pgstat_fetch_stat_dbentry(dbid)) == NULL)
		result = 0;
	else
		result = (int64) (dbentry->n_tuples_fetched);

	MDB_RETURN_INT64(result);
}


Datum
mdb_stat_get_db_tuples_inserted(MDB_FUNCTION_ARGS)
{
	Oid			dbid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatDBEntry *dbentry;

	if ((dbentry = pgstat_fetch_stat_dbentry(dbid)) == NULL)
		result = 0;
	else
		result = (int64) (dbentry->n_tuples_inserted);

	MDB_RETURN_INT64(result);
}


Datum
mdb_stat_get_db_tuples_updated(MDB_FUNCTION_ARGS)
{
	Oid			dbid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatDBEntry *dbentry;

	if ((dbentry = pgstat_fetch_stat_dbentry(dbid)) == NULL)
		result = 0;
	else
		result = (int64) (dbentry->n_tuples_updated);

	MDB_RETURN_INT64(result);
}


Datum
mdb_stat_get_db_tuples_deleted(MDB_FUNCTION_ARGS)
{
	Oid			dbid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatDBEntry *dbentry;

	if ((dbentry = pgstat_fetch_stat_dbentry(dbid)) == NULL)
		result = 0;
	else
		result = (int64) (dbentry->n_tuples_deleted);

	MDB_RETURN_INT64(result);
}

Datum
mdb_stat_get_db_stat_reset_time(MDB_FUNCTION_ARGS)
{
	Oid			dbid = MDB_GETARG_OID(0);
	TimestampTz result;
	PgStat_StatDBEntry *dbentry;

	if ((dbentry = pgstat_fetch_stat_dbentry(dbid)) == NULL)
		result = 0;
	else
		result = dbentry->stat_reset_timestamp;

	if (result == 0)
		MDB_RETURN_NULL();
	else
		MDB_RETURN_TIMESTAMPTZ(result);
}

Datum
mdb_stat_get_db_temp_files(MDB_FUNCTION_ARGS)
{
	Oid			dbid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatDBEntry *dbentry;

	if ((dbentry = pgstat_fetch_stat_dbentry(dbid)) == NULL)
		result = 0;
	else
		result = dbentry->n_temp_files;

	MDB_RETURN_INT64(result);
}


Datum
mdb_stat_get_db_temp_bytes(MDB_FUNCTION_ARGS)
{
	Oid			dbid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatDBEntry *dbentry;

	if ((dbentry = pgstat_fetch_stat_dbentry(dbid)) == NULL)
		result = 0;
	else
		result = dbentry->n_temp_bytes;

	MDB_RETURN_INT64(result);
}

Datum
mdb_stat_get_db_conflict_tablespace(MDB_FUNCTION_ARGS)
{
	Oid			dbid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatDBEntry *dbentry;

	if ((dbentry = pgstat_fetch_stat_dbentry(dbid)) == NULL)
		result = 0;
	else
		result = (int64) (dbentry->n_conflict_tablespace);

	MDB_RETURN_INT64(result);
}

Datum
mdb_stat_get_db_conflict_lock(MDB_FUNCTION_ARGS)
{
	Oid			dbid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatDBEntry *dbentry;

	if ((dbentry = pgstat_fetch_stat_dbentry(dbid)) == NULL)
		result = 0;
	else
		result = (int64) (dbentry->n_conflict_lock);

	MDB_RETURN_INT64(result);
}

Datum
mdb_stat_get_db_conflict_snapshot(MDB_FUNCTION_ARGS)
{
	Oid			dbid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatDBEntry *dbentry;

	if ((dbentry = pgstat_fetch_stat_dbentry(dbid)) == NULL)
		result = 0;
	else
		result = (int64) (dbentry->n_conflict_snapshot);

	MDB_RETURN_INT64(result);
}

Datum
mdb_stat_get_db_conflict_bufferpin(MDB_FUNCTION_ARGS)
{
	Oid			dbid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatDBEntry *dbentry;

	if ((dbentry = pgstat_fetch_stat_dbentry(dbid)) == NULL)
		result = 0;
	else
		result = (int64) (dbentry->n_conflict_bufferpin);

	MDB_RETURN_INT64(result);
}

Datum
mdb_stat_get_db_conflict_startup_deadlock(MDB_FUNCTION_ARGS)
{
	Oid			dbid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatDBEntry *dbentry;

	if ((dbentry = pgstat_fetch_stat_dbentry(dbid)) == NULL)
		result = 0;
	else
		result = (int64) (dbentry->n_conflict_startup_deadlock);

	MDB_RETURN_INT64(result);
}

Datum
mdb_stat_get_db_conflict_all(MDB_FUNCTION_ARGS)
{
	Oid			dbid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatDBEntry *dbentry;

	if ((dbentry = pgstat_fetch_stat_dbentry(dbid)) == NULL)
		result = 0;
	else
		result = (int64) (
						  dbentry->n_conflict_tablespace +
						  dbentry->n_conflict_lock +
						  dbentry->n_conflict_snapshot +
						  dbentry->n_conflict_bufferpin +
						  dbentry->n_conflict_startup_deadlock);

	MDB_RETURN_INT64(result);
}

Datum
mdb_stat_get_db_deadlocks(MDB_FUNCTION_ARGS)
{
	Oid			dbid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_StatDBEntry *dbentry;

	if ((dbentry = pgstat_fetch_stat_dbentry(dbid)) == NULL)
		result = 0;
	else
		result = (int64) (dbentry->n_deadlocks);

	MDB_RETURN_INT64(result);
}

Datum
mdb_stat_get_db_blk_read_time(MDB_FUNCTION_ARGS)
{
	Oid			dbid = MDB_GETARG_OID(0);
	double		result;
	PgStat_StatDBEntry *dbentry;

	/* convert counter from microsec to millisec for display */
	if ((dbentry = pgstat_fetch_stat_dbentry(dbid)) == NULL)
		result = 0;
	else
		result = ((double) dbentry->n_block_read_time) / 1000.0;

	MDB_RETURN_FLOAT8(result);
}

Datum
mdb_stat_get_db_blk_write_time(MDB_FUNCTION_ARGS)
{
	Oid			dbid = MDB_GETARG_OID(0);
	double		result;
	PgStat_StatDBEntry *dbentry;

	/* convert counter from microsec to millisec for display */
	if ((dbentry = pgstat_fetch_stat_dbentry(dbid)) == NULL)
		result = 0;
	else
		result = ((double) dbentry->n_block_write_time) / 1000.0;

	MDB_RETURN_FLOAT8(result);
}

Datum
mdb_stat_get_bgwriter_timed_checkpoints(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_INT64(pgstat_fetch_global()->timed_checkpoints);
}

Datum
mdb_stat_get_bgwriter_requested_checkpoints(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_INT64(pgstat_fetch_global()->requested_checkpoints);
}

Datum
mdb_stat_get_bgwriter_buf_written_checkpoints(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_INT64(pgstat_fetch_global()->buf_written_checkpoints);
}

Datum
mdb_stat_get_bgwriter_buf_written_clean(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_INT64(pgstat_fetch_global()->buf_written_clean);
}

Datum
mdb_stat_get_bgwriter_maxwritten_clean(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_INT64(pgstat_fetch_global()->maxwritten_clean);
}

Datum
mdb_stat_get_checkpoint_write_time(MDB_FUNCTION_ARGS)
{
	/* time is already in msec, just convert to double for presentation */
	MDB_RETURN_FLOAT8((double) pgstat_fetch_global()->checkpoint_write_time);
}

Datum
mdb_stat_get_checkpoint_sync_time(MDB_FUNCTION_ARGS)
{
	/* time is already in msec, just convert to double for presentation */
	MDB_RETURN_FLOAT8((double) pgstat_fetch_global()->checkpoint_sync_time);
}

Datum
mdb_stat_get_bgwriter_stat_reset_time(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_TIMESTAMPTZ(pgstat_fetch_global()->stat_reset_timestamp);
}

Datum
mdb_stat_get_buf_written_backend(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_INT64(pgstat_fetch_global()->buf_written_backend);
}

Datum
mdb_stat_get_buf_fsync_backend(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_INT64(pgstat_fetch_global()->buf_fsync_backend);
}

Datum
mdb_stat_get_buf_alloc(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_INT64(pgstat_fetch_global()->buf_alloc);
}

Datum
mdb_stat_get_xact_numscans(MDB_FUNCTION_ARGS)
{
	Oid			relid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_TableStatus *tabentry;

	if ((tabentry = find_tabstat_entry(relid)) == NULL)
		result = 0;
	else
		result = (int64) (tabentry->t_counts.t_numscans);

	MDB_RETURN_INT64(result);
}

Datum
mdb_stat_get_xact_tuples_returned(MDB_FUNCTION_ARGS)
{
	Oid			relid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_TableStatus *tabentry;

	if ((tabentry = find_tabstat_entry(relid)) == NULL)
		result = 0;
	else
		result = (int64) (tabentry->t_counts.t_tuples_returned);

	MDB_RETURN_INT64(result);
}

Datum
mdb_stat_get_xact_tuples_fetched(MDB_FUNCTION_ARGS)
{
	Oid			relid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_TableStatus *tabentry;

	if ((tabentry = find_tabstat_entry(relid)) == NULL)
		result = 0;
	else
		result = (int64) (tabentry->t_counts.t_tuples_fetched);

	MDB_RETURN_INT64(result);
}

Datum
mdb_stat_get_xact_tuples_inserted(MDB_FUNCTION_ARGS)
{
	Oid			relid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_TableStatus *tabentry;
	PgStat_TableXactStatus *trans;

	if ((tabentry = find_tabstat_entry(relid)) == NULL)
		result = 0;
	else
	{
		result = tabentry->t_counts.t_tuples_inserted;
		/* live subtransactions' counts aren't in t_tuples_inserted yet */
		for (trans = tabentry->trans; trans != NULL; trans = trans->upper)
			result += trans->tuples_inserted;
	}

	MDB_RETURN_INT64(result);
}

Datum
mdb_stat_get_xact_tuples_updated(MDB_FUNCTION_ARGS)
{
	Oid			relid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_TableStatus *tabentry;
	PgStat_TableXactStatus *trans;

	if ((tabentry = find_tabstat_entry(relid)) == NULL)
		result = 0;
	else
	{
		result = tabentry->t_counts.t_tuples_updated;
		/* live subtransactions' counts aren't in t_tuples_updated yet */
		for (trans = tabentry->trans; trans != NULL; trans = trans->upper)
			result += trans->tuples_updated;
	}

	MDB_RETURN_INT64(result);
}

Datum
mdb_stat_get_xact_tuples_deleted(MDB_FUNCTION_ARGS)
{
	Oid			relid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_TableStatus *tabentry;
	PgStat_TableXactStatus *trans;

	if ((tabentry = find_tabstat_entry(relid)) == NULL)
		result = 0;
	else
	{
		result = tabentry->t_counts.t_tuples_deleted;
		/* live subtransactions' counts aren't in t_tuples_deleted yet */
		for (trans = tabentry->trans; trans != NULL; trans = trans->upper)
			result += trans->tuples_deleted;
	}

	MDB_RETURN_INT64(result);
}

Datum
mdb_stat_get_xact_tuples_hot_updated(MDB_FUNCTION_ARGS)
{
	Oid			relid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_TableStatus *tabentry;

	if ((tabentry = find_tabstat_entry(relid)) == NULL)
		result = 0;
	else
		result = (int64) (tabentry->t_counts.t_tuples_hot_updated);

	MDB_RETURN_INT64(result);
}

Datum
mdb_stat_get_xact_blocks_fetched(MDB_FUNCTION_ARGS)
{
	Oid			relid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_TableStatus *tabentry;

	if ((tabentry = find_tabstat_entry(relid)) == NULL)
		result = 0;
	else
		result = (int64) (tabentry->t_counts.t_blocks_fetched);

	MDB_RETURN_INT64(result);
}

Datum
mdb_stat_get_xact_blocks_hit(MDB_FUNCTION_ARGS)
{
	Oid			relid = MDB_GETARG_OID(0);
	int64		result;
	PgStat_TableStatus *tabentry;

	if ((tabentry = find_tabstat_entry(relid)) == NULL)
		result = 0;
	else
		result = (int64) (tabentry->t_counts.t_blocks_hit);

	MDB_RETURN_INT64(result);
}

Datum
mdb_stat_get_xact_function_calls(MDB_FUNCTION_ARGS)
{
	Oid			funcid = MDB_GETARG_OID(0);
	PgStat_BackendFunctionEntry *funcentry;

	if ((funcentry = find_funcstat_entry(funcid)) == NULL)
		MDB_RETURN_NULL();
	MDB_RETURN_INT64(funcentry->f_counts.f_numcalls);
}

Datum
mdb_stat_get_xact_function_total_time(MDB_FUNCTION_ARGS)
{
	Oid			funcid = MDB_GETARG_OID(0);
	PgStat_BackendFunctionEntry *funcentry;

	if ((funcentry = find_funcstat_entry(funcid)) == NULL)
		MDB_RETURN_NULL();
	MDB_RETURN_FLOAT8(INSTR_TIME_GET_MILLISEC(funcentry->f_counts.f_total_time));
}

Datum
mdb_stat_get_xact_function_self_time(MDB_FUNCTION_ARGS)
{
	Oid			funcid = MDB_GETARG_OID(0);
	PgStat_BackendFunctionEntry *funcentry;

	if ((funcentry = find_funcstat_entry(funcid)) == NULL)
		MDB_RETURN_NULL();
	MDB_RETURN_FLOAT8(INSTR_TIME_GET_MILLISEC(funcentry->f_counts.f_self_time));
}


/* Get the timestamp of the current statistics snapshot */
Datum
mdb_stat_get_snapshot_timestamp(MDB_FUNCTION_ARGS)
{
	MDB_RETURN_TIMESTAMPTZ(pgstat_fetch_global()->stats_timestamp);
}

/* Discard the active statistics snapshot */
Datum
mdb_stat_clear_snapshot(MDB_FUNCTION_ARGS)
{
	pgstat_clear_snapshot();

	MDB_RETURN_VOID();
}


/* Reset all counters for the current database */
Datum
mdb_stat_reset(MDB_FUNCTION_ARGS)
{
	pgstat_reset_counters();

	MDB_RETURN_VOID();
}

/* Reset some shared cluster-wide counters */
Datum
mdb_stat_reset_shared(MDB_FUNCTION_ARGS)
{
	char	   *target = text_to_cstring(MDB_GETARG_TEXT_PP(0));

	pgstat_reset_shared_counters(target);

	MDB_RETURN_VOID();
}

/* Reset a single counter in the current database */
Datum
mdb_stat_reset_single_table_counters(MDB_FUNCTION_ARGS)
{
	Oid			taboid = MDB_GETARG_OID(0);

	pgstat_reset_single_counter(taboid, RESET_TABLE);

	MDB_RETURN_VOID();
}

Datum
mdb_stat_reset_single_function_counters(MDB_FUNCTION_ARGS)
{
	Oid			funcoid = MDB_GETARG_OID(0);

	pgstat_reset_single_counter(funcoid, RESET_FUNCTION);

	MDB_RETURN_VOID();
}

Datum
mdb_stat_get_archiver(MDB_FUNCTION_ARGS)
{
	TupleDesc	tupdesc;
	Datum		values[7];
	bool		nulls[7];
	PgStat_ArchiverStats *archiver_stats;

	/* Initialise values and NULL flags arrays */
	MemSet(values, 0, sizeof(values));
	MemSet(nulls, 0, sizeof(nulls));

	/* Initialise attributes information in the tuple descriptor */
	tupdesc = CreateTemplateTupleDesc(7, false);
	TupleDescInitEntry(tupdesc, (AttrNumber) 1, "archived_count",
					   INT8OID, -1, 0);
	TupleDescInitEntry(tupdesc, (AttrNumber) 2, "last_archived_wal",
					   TEXTOID, -1, 0);
	TupleDescInitEntry(tupdesc, (AttrNumber) 3, "last_archived_time",
					   TIMESTAMPTZOID, -1, 0);
	TupleDescInitEntry(tupdesc, (AttrNumber) 4, "failed_count",
					   INT8OID, -1, 0);
	TupleDescInitEntry(tupdesc, (AttrNumber) 5, "last_failed_wal",
					   TEXTOID, -1, 0);
	TupleDescInitEntry(tupdesc, (AttrNumber) 6, "last_failed_time",
					   TIMESTAMPTZOID, -1, 0);
	TupleDescInitEntry(tupdesc, (AttrNumber) 7, "stats_reset",
					   TIMESTAMPTZOID, -1, 0);

	BlessTupleDesc(tupdesc);

	/* Get statistics about the archiver process */
	archiver_stats = pgstat_fetch_stat_archiver();

	/* Fill values and NULLs */
	values[0] = Int64GetDatum(archiver_stats->archived_count);
	if (*(archiver_stats->last_archived_wal) == '\0')
		nulls[1] = true;
	else
		values[1] = CStringGetTextDatum(archiver_stats->last_archived_wal);

	if (archiver_stats->last_archived_timestamp == 0)
		nulls[2] = true;
	else
		values[2] = TimestampTzGetDatum(archiver_stats->last_archived_timestamp);

	values[3] = Int64GetDatum(archiver_stats->failed_count);
	if (*(archiver_stats->last_failed_wal) == '\0')
		nulls[4] = true;
	else
		values[4] = CStringGetTextDatum(archiver_stats->last_failed_wal);

	if (archiver_stats->last_failed_timestamp == 0)
		nulls[5] = true;
	else
		values[5] = TimestampTzGetDatum(archiver_stats->last_failed_timestamp);

	if (archiver_stats->stat_reset_timestamp == 0)
		nulls[6] = true;
	else
		values[6] = TimestampTzGetDatum(archiver_stats->stat_reset_timestamp);

	/* Returns the record as Datum */
	MDB_RETURN_DATUM(HeapTupleGetDatum(
								   heap_form_tuple(tupdesc, values, nulls)));
}
