/* src/backend/port/dynloader/darwin.h */

#include "fmgr.h"

void	   *mdb_dlopen(char *filename);
PGFunction	mdb_dlsym(void *handle, char *funcname);
void		mdb_dlclose(void *handle);
char	   *mdb_dlerror(void);
