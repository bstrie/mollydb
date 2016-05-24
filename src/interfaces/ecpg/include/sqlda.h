#ifndef ECMDB_SQLDA_H
#define ECMDB_SQLDA_H

#ifdef _ECMDB_INFORMIX_H

#include "sqlda-compat.h"
typedef struct sqlvar_compat sqlvar_t;
typedef struct sqlda_compat sqlda_t;

#else

#include "sqlda-native.h"
typedef struct sqlvar_struct sqlvar_t;
typedef struct sqlda_struct sqlda_t;

#endif

#endif   /* ECMDB_SQLDA_H */
