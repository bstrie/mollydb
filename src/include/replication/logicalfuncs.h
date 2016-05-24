/*-------------------------------------------------------------------------
 * logicalfuncs.h
 *	   MollyDB WAL to logical transformation support functions
 *
 * Copyright (c) 2012-2016, MollyDB Global Development Group
 *
 *-------------------------------------------------------------------------
 */
#ifndef LOGICALFUNCS_H
#define LOGICALFUNCS_H

#include "replication/logical.h"

extern int logical_read_local_xlog_page(XLogReaderState *state,
							 XLogRecPtr targetPagePtr,
							 int reqLen, XLogRecPtr targetRecPtr,
							 char *cur_page, TimeLineID *pageTLI);

extern Datum mdb_logical_slot_get_changes(MDB_FUNCTION_ARGS);
extern Datum mdb_logical_slot_get_binary_changes(MDB_FUNCTION_ARGS);
extern Datum mdb_logical_slot_peek_changes(MDB_FUNCTION_ARGS);
extern Datum mdb_logical_slot_peek_binary_changes(MDB_FUNCTION_ARGS);

extern Datum mdb_logical_emit_message_bytea(MDB_FUNCTION_ARGS);
extern Datum mdb_logical_emit_message_text(MDB_FUNCTION_ARGS);
#endif
