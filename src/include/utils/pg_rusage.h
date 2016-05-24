/*-------------------------------------------------------------------------
 *
 * mdb_rusage.h
 *	  header file for resource usage measurement support routines
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/mdb_rusage.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_RUSAGE_H
#define PG_RUSAGE_H

#include <sys/time.h>

#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#else
#include "rusagestub.h"
#endif


/* State structure for mdb_rusage_init/mdb_rusage_show */
typedef struct PGRUsage
{
	struct timeval tv;
	struct rusage ru;
} PGRUsage;


extern void mdb_rusage_init(PGRUsage *ru0);
extern const char *mdb_rusage_show(const PGRUsage *ru0);

#endif   /* PG_RUSAGE_H */
