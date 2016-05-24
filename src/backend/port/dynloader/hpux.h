/*-------------------------------------------------------------------------
 *
 * dynloader.h
 *	  dynamic loader for HP-UX using the shared library mechanism
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/port/dynloader/hpux.h
 *
 *	NOTES
 *		all functions are defined here -- it's impossible to trace the
 *		shl_* routines from the bundled HP-UX debugger.
 *
 *-------------------------------------------------------------------------
 */
/* System includes */
#include "fmgr.h"

extern void *mdb_dlopen(char *filename);
extern PGFunction mdb_dlsym(void *handle, char *funcname);
extern void mdb_dlclose(void *handle);
extern char *mdb_dlerror(void);
