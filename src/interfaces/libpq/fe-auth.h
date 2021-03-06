/*-------------------------------------------------------------------------
 *
 * fe-auth.h
 *
 *	  Definitions for network authentication routines
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/interfaces/libpq/fe-auth.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef FE_AUTH_H
#define FE_AUTH_H

#include "libpq-fe.h"
#include "libpq-int.h"


extern int	mdb_fe_sendauth(AuthRequest areq, PGconn *conn);
extern char *mdb_fe_getauthname(PQExpBuffer errorMessage);

#endif   /* FE_AUTH_H */
