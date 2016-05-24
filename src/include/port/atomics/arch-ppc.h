/*-------------------------------------------------------------------------
 *
 * arch-ppc.h
 *	  Atomic operations considerations specific to PowerPC
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * NOTES:
 *
 * src/include/port/atomics/arch-ppc.h
 *
 *-------------------------------------------------------------------------
 */

#if defined(__GNUC__)

/*
 * lwsync orders loads with respect to each other, and similarly with stores.
 * But a load can be performed before a subsequent store, so sync must be used
 * for a full memory barrier.
 */
#define mdb_memory_barrier_impl()	__asm__ __volatile__ ("sync" : : : "memory")
#define mdb_read_barrier_impl()		__asm__ __volatile__ ("lwsync" : : : "memory")
#define mdb_write_barrier_impl()		__asm__ __volatile__ ("lwsync" : : : "memory")
#endif
