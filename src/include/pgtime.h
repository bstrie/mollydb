/*-------------------------------------------------------------------------
 *
 * pgtime.h
 *	  MollyDB internal timezone library
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 *
 * IDENTIFICATION
 *	  src/include/pgtime.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef _PGTIME_H
#define _PGTIME_H


/*
 * The API of this library is generally similar to the corresponding
 * C library functions, except that we use mdb_time_t which (we hope) is
 * 64 bits wide, and which is most definitely signed not unsigned.
 */

typedef int64 mdb_time_t;

struct mdb_tm
{
	int			tm_sec;
	int			tm_min;
	int			tm_hour;
	int			tm_mday;
	int			tm_mon;			/* origin 0, not 1 */
	int			tm_year;		/* relative to 1900 */
	int			tm_wday;
	int			tm_yday;
	int			tm_isdst;
	long int	tm_gmtoff;
	const char *tm_zone;
};

typedef struct mdb_tz mdb_tz;
typedef struct mdb_tzenum mdb_tzenum;

/* Maximum length of a timezone name (not including trailing null) */
#define TZ_STRLEN_MAX 255

/* these functions are in localtime.c */

extern struct mdb_tm *mdb_localtime(const mdb_time_t *timep, const mdb_tz *tz);
extern struct mdb_tm *mdb_gmtime(const mdb_time_t *timep);
extern int mdb_next_dst_boundary(const mdb_time_t *timep,
					 long int *before_gmtoff,
					 int *before_isdst,
					 mdb_time_t *boundary,
					 long int *after_gmtoff,
					 int *after_isdst,
					 const mdb_tz *tz);
extern bool mdb_interpret_timezone_abbrev(const char *abbrev,
							 const mdb_time_t *timep,
							 long int *gmtoff,
							 int *isdst,
							 const mdb_tz *tz);
extern bool mdb_get_timezone_offset(const mdb_tz *tz, long int *gmtoff);
extern const char *mdb_get_timezone_name(mdb_tz *tz);
extern bool mdb_tz_acceptable(mdb_tz *tz);

/* these functions are in strftime.c */

extern size_t mdb_strftime(char *s, size_t max, const char *format,
			const struct mdb_tm * tm);

/* these functions and variables are in pgtz.c */

extern mdb_tz *session_timezone;
extern mdb_tz *log_timezone;

extern void mdb_timezone_initialize(void);
extern mdb_tz *mdb_tzset(const char *tzname);
extern mdb_tz *mdb_tzset_offset(long gmtoffset);

extern mdb_tzenum *mdb_tzenumerate_start(void);
extern mdb_tz *mdb_tzenumerate_next(mdb_tzenum *dir);
extern void mdb_tzenumerate_end(mdb_tzenum *dir);

#endif   /* _PGTIME_H */
