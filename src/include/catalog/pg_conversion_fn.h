/*-------------------------------------------------------------------------
 *
 * mdb_conversion_fn.h
 *	 prototypes for functions in catalog/mdb_conversion.c
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/mdb_conversion_fn.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef MDB_CONVERSION_FN_H
#define MDB_CONVERSION_FN_H


#include "catalog/objectaddress.h"

extern ObjectAddress ConversionCreate(const char *conname, Oid connamespace,
				 Oid conowner,
				 int32 conforencoding, int32 contoencoding,
				 Oid conproc, bool def);
extern void RemoveConversionById(Oid conversionOid);
extern Oid	FindDefaultConversion(Oid connamespace, int32 for_encoding, int32 to_encoding);

#endif   /* MDB_CONVERSION_FN_H */
