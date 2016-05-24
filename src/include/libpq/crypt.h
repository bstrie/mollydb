/*-------------------------------------------------------------------------
 *
 * crypt.h
 *	  Interface to libpq/crypt.c
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/libpq/crypt.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef MDB_CRYPT_H
#define MDB_CRYPT_H

#include "libpq/libpq-be.h"

extern int md5_crypt_verify(const Port *port, const char *role,
				 char *client_pass, char **logdetail);

#endif
