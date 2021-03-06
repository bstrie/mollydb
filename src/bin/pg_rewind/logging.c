/*-------------------------------------------------------------------------
 *
 * logging.c
 *	 logging functions
 *
 *	Copyright (c) 2010-2016, MollyDB Global Development Group
 *
 *-------------------------------------------------------------------------
 */
#include "mollydb_fe.h"

#include <unistd.h>
#include <time.h>

#include "mdb_rewind.h"
#include "logging.h"

#include "pgtime.h"

/* Progress counters */
uint64		fetch_size;
uint64		fetch_done;

static mdb_time_t last_progress_report = 0;

#define QUERY_ALLOC			8192

#define MESSAGE_WIDTH		60

static
mdb_attribute_printf(2, 0)
void
mdb_log_v(eLogType type, const char *fmt, va_list ap)
{
	char		message[QUERY_ALLOC];

	vsnprintf(message, sizeof(message), fmt, ap);

	switch (type)
	{
		case MDB_DEBUG:
			if (debug)
				printf("%s", _(message));
			break;

		case MDB_PROGRESS:
			if (showprogress)
				printf("%s", _(message));
			break;

		case MDB_WARNING:
			printf("%s", _(message));
			break;

		case MDB_FATAL:
			printf("\n%s", _(message));
			printf("%s", _("Failure, exiting\n"));
			exit(1);
			break;

		default:
			break;
	}
	fflush(stdout);
}


void
mdb_log(eLogType type, const char *fmt,...)
{
	va_list		args;

	va_start(args, fmt);
	mdb_log_v(type, fmt, args);
	va_end(args);
}


/*
 * Print an error message, and exit.
 */
void
mdb_fatal(const char *fmt,...)
{
	va_list		args;

	va_start(args, fmt);
	mdb_log_v(MDB_FATAL, fmt, args);
	va_end(args);
	/* should not get here, mdb_log_v() exited already */
	exit(1);
}


/*
 * Print a progress report based on the global variables.
 *
 * Progress report is written at maximum once per second, unless the
 * force parameter is set to true.
 */
void
progress_report(bool force)
{
	int			percent;
	char		fetch_done_str[32];
	char		fetch_size_str[32];
	mdb_time_t	now;

	if (!showprogress)
		return;

	now = time(NULL);
	if (now == last_progress_report && !force)
		return;					/* Max once per second */

	last_progress_report = now;
	percent = fetch_size ? (int) ((fetch_done) * 100 / fetch_size) : 0;

	/*
	 * Avoid overflowing past 100% or the full size. This may make the total
	 * size number change as we approach the end of the backup (the estimate
	 * will always be wrong if WAL is included), but that's better than having
	 * the done column be bigger than the total.
	 */
	if (percent > 100)
		percent = 100;
	if (fetch_done > fetch_size)
		fetch_size = fetch_done;

	/*
	 * Separate step to keep platform-dependent format code out of
	 * translatable strings.  And we only test for INT64_FORMAT availability
	 * in snprintf, not fprintf.
	 */
	snprintf(fetch_done_str, sizeof(fetch_done_str), INT64_FORMAT,
			 fetch_done / 1024);
	snprintf(fetch_size_str, sizeof(fetch_size_str), INT64_FORMAT,
			 fetch_size / 1024);

	mdb_log(MDB_PROGRESS, "%*s/%s kB (%d%%) copied",
		   (int) strlen(fetch_size_str), fetch_done_str, fetch_size_str,
		   percent);
	printf("\r");
}
