/*-------------------------------------------------------------------------
 *
 * mdb_collation_fn.h
 *	 prototypes for functions in catalog/mdb_collation.c
 *
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/mdb_collation_fn.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef MDB_COLLATION_FN_H
#define MDB_COLLATION_FN_H

extern Oid CollationCreate(const char *collname, Oid collnamespace,
				Oid collowner,
				int32 collencoding,
				const char *collcollate, const char *collctype);
extern void RemoveCollationById(Oid collationOid);

#endif   /* MDB_COLLATION_FN_H */
