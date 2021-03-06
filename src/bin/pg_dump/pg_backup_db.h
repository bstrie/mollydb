/*
 *	Definitions for mdb_backup_db.c
 *
 *	IDENTIFICATION
 *		src/bin/mdb_dump/mdb_backup_db.h
 */

#ifndef MDB_BACKUP_DB_H
#define MDB_BACKUP_DB_H

#include "mdb_backup.h"


extern int	ExecuteSqlCommandBuf(Archive *AHX, const char *buf, size_t bufLen);

extern void ExecuteSqlStatement(Archive *AHX, const char *query);
extern PGresult *ExecuteSqlQuery(Archive *AHX, const char *query,
				ExecStatusType status);

extern void EndDBCopyMode(Archive *AHX, const char *tocEntryTag);

extern void StartTransaction(Archive *AHX);
extern void CommitTransaction(Archive *AHX);

#endif
