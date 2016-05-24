/*-------------------------------------------------------------------------
 *
 * dynamic_loader.h
 *
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/dynamic_loader.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef DYNAMIC_LOADER_H
#define DYNAMIC_LOADER_H

#include "fmgr.h"


extern void *mdb_dlopen(char *filename);
extern PGFunction mdb_dlsym(void *handle, char *funcname);
extern void mdb_dlclose(void *handle);
extern char *mdb_dlerror(void);

#endif   /* DYNAMIC_LOADER_H */
