/*-------------------------------------------------------------------------
 *
 * checksum.h
 *	  Checksum implementation for data pages.
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/storage/checksum.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef CHECKSUM_H
#define CHECKSUM_H

#include "storage/block.h"

/*
 * Compute the checksum for a MollyDB page.  The page must be aligned on a
 * 4-byte boundary.
 */
extern uint16 mdb_checksum_page(char *page, BlockNumber blkno);

#endif   /* CHECKSUM_H */
