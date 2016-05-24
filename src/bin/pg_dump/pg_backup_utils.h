/*-------------------------------------------------------------------------
 *
 * mdb_backup_utils.h
 *	Utility routines shared by mdb_dump and mdb_restore.
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/bin/mdb_dump/mdb_backup_utils.h
 *
 *-------------------------------------------------------------------------
 */

#ifndef PG_BACKUP_UTILS_H
#define PG_BACKUP_UTILS_H

typedef enum					/* bits returned by set_dump_section */
{
	DUMP_PRE_DATA = 0x01,
	DUMP_DATA = 0x02,
	DUMP_POST_DATA = 0x04,
	DUMP_UNSECTIONED = 0xff
} DumpSections;

typedef void (*on_exit_nicely_callback) (int code, void *arg);

extern const char *progname;

extern void set_dump_section(const char *arg, int *dumpSections);
extern void write_msg(const char *modulename, const char *fmt,...) mdb_attribute_printf(2, 3);
extern void vwrite_msg(const char *modulename, const char *fmt, va_list ap) mdb_attribute_printf(2, 0);
extern void on_exit_nicely(on_exit_nicely_callback function, void *arg);
extern void exit_nicely(int code) mdb_attribute_noreturn();

extern void exit_horribly(const char *modulename, const char *fmt,...) mdb_attribute_printf(2, 3) mdb_attribute_noreturn();

#endif   /* PG_BACKUP_UTILS_H */
