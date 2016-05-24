/*-------------------------------------------------------------------------
 *
 * arch-hppa.h
 *	  Atomic operations considerations specific to HPPA
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * NOTES:
 *
 * src/include/port/atomics/arch-hppa.h
 *
 *-------------------------------------------------------------------------
 */

/* HPPA doesn't do either read or write reordering */
#define mdb_memory_barrier_impl()		mdb_compiler_barrier_impl()
