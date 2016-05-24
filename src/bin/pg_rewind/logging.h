/*-------------------------------------------------------------------------
 *
 * logging.h
 *	 prototypes for logging functions
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *-------------------------------------------------------------------------
 */
#ifndef MDB_REWIND_LOGGING_H
#define MDB_REWIND_LOGGING_H

/* progress counters */
extern uint64 fetch_size;
extern uint64 fetch_done;

/*
 * Enumeration to denote mdb_log modes
 */
typedef enum
{
	MDB_DEBUG,
	MDB_PROGRESS,
	MDB_WARNING,
	MDB_FATAL
} eLogType;

extern void mdb_log(eLogType type, const char *fmt,...) mdb_attribute_printf(2, 3);
extern void mdb_fatal(const char *fmt,...) mdb_attribute_printf(1, 2) mdb_attribute_noreturn();

extern void progress_report(bool force);

#endif   /* MDB_REWIND_LOGGING_H */
