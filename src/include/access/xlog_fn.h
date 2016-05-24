/*
 * xlog_fn.h
 *
 * MollyDB transaction log SQL-callable function declarations
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/access/xlog_fn.h
 */
#ifndef XLOG_FN_H
#define XLOG_FN_H

#include "fmgr.h"

extern Datum mdb_start_backup(MDB_FUNCTION_ARGS);
extern Datum mdb_stop_backup(MDB_FUNCTION_ARGS);
extern Datum mdb_stop_backup_v2(MDB_FUNCTION_ARGS);
extern Datum mdb_switch_xlog(MDB_FUNCTION_ARGS);
extern Datum mdb_create_restore_point(MDB_FUNCTION_ARGS);
extern Datum mdb_current_xlog_location(MDB_FUNCTION_ARGS);
extern Datum mdb_current_xlog_insert_location(MDB_FUNCTION_ARGS);
extern Datum mdb_current_xlog_flush_location(MDB_FUNCTION_ARGS);
extern Datum mdb_last_xlog_receive_location(MDB_FUNCTION_ARGS);
extern Datum mdb_last_xlog_replay_location(MDB_FUNCTION_ARGS);
extern Datum mdb_last_xact_replay_timestamp(MDB_FUNCTION_ARGS);
extern Datum mdb_xlogfile_name_offset(MDB_FUNCTION_ARGS);
extern Datum mdb_xlogfile_name(MDB_FUNCTION_ARGS);
extern Datum mdb_is_in_recovery(MDB_FUNCTION_ARGS);
extern Datum mdb_xlog_replay_pause(MDB_FUNCTION_ARGS);
extern Datum mdb_xlog_replay_resume(MDB_FUNCTION_ARGS);
extern Datum mdb_is_xlog_replay_paused(MDB_FUNCTION_ARGS);
extern Datum mdb_xlog_location_diff(MDB_FUNCTION_ARGS);
extern Datum mdb_is_in_backup(MDB_FUNCTION_ARGS);
extern Datum mdb_backup_start_time(MDB_FUNCTION_ARGS);

#endif   /* XLOG_FN_H */
