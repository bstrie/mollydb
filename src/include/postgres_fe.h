/*-------------------------------------------------------------------------
 *
 * postgres_fe.h
 *	  Primary include file for MollyDB client-side .c files
 *
 * This should be the first file included by MollyDB client libraries and
 * application programs --- but not by backend modules, which should include
 * postgres.h.
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1995, Regents of the University of California
 *
 * src/include/postgres_fe.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef POSTGRES_FE_H
#define POSTGRES_FE_H

#ifndef FRONTEND
#define FRONTEND 1
#endif

#include "c.h"

#include "common/fe_memutils.h"

#endif   /* POSTGRES_FE_H */
