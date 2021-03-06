/*-------------------------------------------------------------------------
 *
 * generic-msvc.h
 *	  Atomic operations support when using MSVC
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * NOTES:
 *
 * Documentation:
 * * Interlocked Variable Access
 *   http://msdn.microsoft.com/en-us/library/ms684122%28VS.85%29.aspx
 *
 * src/include/port/atomics/generic-msvc.h
 *
 *-------------------------------------------------------------------------
 */
#include <intrin.h>
#include <windows.h>

/* intentionally no include guards, should only be included by atomics.h */
#ifndef INSIDE_ATOMICS_H
#error "should be included via atomics.h"
#endif

/* Should work on both MSVC and Borland. */
#pragma intrinsic(_ReadWriteBarrier)
#define mdb_compiler_barrier_impl()	_ReadWriteBarrier()

#ifndef mdb_memory_barrier_impl
#define mdb_memory_barrier_impl()	MemoryBarrier()
#endif

#if defined(HAVE_ATOMICS)

#define MDB_HAVE_ATOMIC_U32_SUPPORT
typedef struct mdb_atomic_uint32
{
	volatile uint32 value;
} mdb_atomic_uint32;

#define MDB_HAVE_ATOMIC_U64_SUPPORT
typedef struct __declspec(align(8)) mdb_atomic_uint64
{
	volatile uint64 value;
} mdb_atomic_uint64;


#define MDB_HAVE_ATOMIC_COMPARE_EXCHANGE_U32
static inline bool
mdb_atomic_compare_exchange_u32_impl(volatile mdb_atomic_uint32 *ptr,
									uint32 *expected, uint32 newval)
{
	bool	ret;
	uint32	current;
	current = InterlockedCompareExchange(&ptr->value, newval, *expected);
	ret = current == *expected;
	*expected = current;
	return ret;
}

#define MDB_HAVE_ATOMIC_FETCH_ADD_U32
static inline uint32
mdb_atomic_fetch_add_u32_impl(volatile mdb_atomic_uint32 *ptr, int32 add_)
{
	return InterlockedExchangeAdd(&ptr->value, add_);
}

/*
 * The non-intrinsics versions are only available in vista upwards, so use the
 * intrinsic version. Only supported on >486, but we require XP as a minimum
 * baseline, which doesn't support the 486, so we don't need to add checks for
 * that case.
 */
#pragma intrinsic(_InterlockedCompareExchange64)

#define MDB_HAVE_ATOMIC_COMPARE_EXCHANGE_U64
static inline bool
mdb_atomic_compare_exchange_u64_impl(volatile mdb_atomic_uint64 *ptr,
									uint64 *expected, uint64 newval)
{
	bool	ret;
	uint64	current;
	current = _InterlockedCompareExchange64(&ptr->value, newval, *expected);
	ret = current == *expected;
	*expected = current;
	return ret;
}

/* Only implemented on itanium and 64bit builds */
#ifdef _WIN64
#pragma intrinsic(_InterlockedExchangeAdd64)

#define MDB_HAVE_ATOMIC_FETCH_ADD_U64
static inline uint64
mdb_atomic_fetch_add_u64_impl(volatile mdb_atomic_uint64 *ptr, int64 add_)
{
	return _InterlockedExchangeAdd64(&ptr->value, add_);
}
#endif /* _WIN64 */

#endif /* HAVE_ATOMICS */
