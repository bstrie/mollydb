/*
 * src/backend/port/dynloader/win32.h
 */
#ifndef PORT_PROTOS_H
#define PORT_PROTOS_H

#include "utils/dynamic_loader.h"		/* pgrminclude ignore */

#define mdb_dlopen(f)	dlopen((f), 1)
#define mdb_dlsym		dlsym
#define mdb_dlclose		dlclose
#define mdb_dlerror		dlerror

char	   *dlerror(void);
int			dlclose(void *handle);
void	   *dlsym(void *handle, const char *symbol);
void	   *dlopen(const char *path, int mode);

#endif   /* PORT_PROTOS_H */
