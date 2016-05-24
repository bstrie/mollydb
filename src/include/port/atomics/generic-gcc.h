/*-------------------------------------------------------------------------
 *
 * generic-gcc.h
 *	  Atomic operations, implemented using gcc (or compatible) intrinsics.
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * NOTES:
 *
 * Documentation:
 * * Legacy __sync Built-in Functions for Atomic Memory Access
 *   http://gcc.gnu.org/onlinedocs/gcc-4.8.2/gcc/_005f_005fsync-Builtins.html
 * * Built-in functions for memory model aware atomic operations
 *   http://gcc.gnu.org/onlinedocs/gcc-4.8.2/gcc/_005f_005fatomic-Builtins.html
 *
 * src/include/port/atomics/generic-gcc.h
 *
 *-------------------------------------------------------------------------
 */

/* intentionally no include guards, should only be included by atomics.h */
#ifndef INSIDE_ATOMICS_H
#error "should be included via atomics.h"
#endif

/*
 * An empty asm block should be a sufficient compiler barrier.
 */
#define mdb_compiler_barrier_impl()	__asm__ __volatile__("" ::: "memory")

/*
 * If we're on GCC 4.1.0 or higher, we should be able to get a memory barrier
 * out of this compiler built-in.  But we prefer to rely on platform specific
 * definitions where possible, and use this only as a fallback.
 */
#if !defined(mdb_memory_barrier_impl)
#	if defined(HAVE_GCC__ATOMIC_INT32_CAS)
#		define mdb_memory_barrier_impl()		__atomic_thread_fence(__ATOMIC_SEQ_CST)
#	elif (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1))
#		define mdb_memory_barrier_impl()		__sync_synchronize()
#	endif
#endif /* !defined(mdb_memory_barrier_impl) */

#if !defined(mdb_read_barrier_impl) && defined(HAVE_GCC__ATOMIC_INT32_CAS)
/* acquire semantics include read barrier semantics */
#		define mdb_read_barrier_impl()		__atomic_thread_fence(__ATOMIC_ACQUIRE)
#endif

#if !defined(mdb_write_barrier_impl) && defined(HAVE_GCC__ATOMIC_INT32_CAS)
/* release semantics include write barrier semantics */
#		define mdb_write_barrier_impl()		__atomic_thread_fence(__ATOMIC_RELEASE)
#endif


#ifdef HAVE_ATOMICS

/* generic gcc based atomic flag implementation */
#if !defined(PG_HAVE_ATOMIC_FLAG_SUPPORT) \
	&& (defined(HAVE_GCC__SYNC_INT32_TAS) || defined(HAVE_GCC__SYNC_CHAR_TAS))

#define PG_HAVE_ATOMIC_FLAG_SUPPORT
typedef struct mdb_atomic_flag
{
	/* some platforms only have a 8 bit wide TAS */
#ifdef HAVE_GCC__SYNC_CHAR_TAS
	volatile char value;
#else
	/* but an int works on more platforms */
	volatile int value;
#endif
} mdb_atomic_flag;

#endif /* !ATOMIC_FLAG_SUPPORT && SYNC_INT32_TAS */

/* generic gcc based atomic uint32 implementation */
#if !defined(PG_HAVE_ATOMIC_U32_SUPPORT) \
	&& (defined(HAVE_GCC__ATOMIC_INT32_CAS) || defined(HAVE_GCC__SYNC_INT32_CAS))

#define PG_HAVE_ATOMIC_U32_SUPPORT
typedef struct mdb_atomic_uint32
{
	volatile uint32 value;
} mdb_atomic_uint32;

#endif /* defined(HAVE_GCC__ATOMIC_INT32_CAS) || defined(HAVE_GCC__SYNC_INT32_CAS) */

/* generic gcc based atomic uint64 implementation */
#if !defined(PG_HAVE_ATOMIC_U64_SUPPORT) \
	&& !defined(PG_DISABLE_64_BIT_ATOMICS) \
	&& (defined(HAVE_GCC__ATOMIC_INT64_CAS) || defined(HAVE_GCC__SYNC_INT64_CAS))

#define PG_HAVE_ATOMIC_U64_SUPPORT

typedef struct mdb_atomic_uint64
{
	volatile uint64 value mdb_attribute_aligned(8);
} mdb_atomic_uint64;

#endif /* defined(HAVE_GCC__ATOMIC_INT64_CAS) || defined(HAVE_GCC__SYNC_INT64_CAS) */

#ifdef PG_HAVE_ATOMIC_FLAG_SUPPORT

#if defined(HAVE_GCC__SYNC_CHAR_TAS) || defined(HAVE_GCC__SYNC_INT32_TAS)

#ifndef PG_HAVE_ATOMIC_TEST_SET_FLAG
#define PG_HAVE_ATOMIC_TEST_SET_FLAG
static inline bool
mdb_atomic_test_set_flag_impl(volatile mdb_atomic_flag *ptr)
{
	/* NB: only an acquire barrier, not a full one */
	/* some platform only support a 1 here */
	return __sync_lock_test_and_set(&ptr->value, 1) == 0;
}
#endif

#endif /* defined(HAVE_GCC__SYNC_*_TAS) */

#ifndef PG_HAVE_ATOMIC_UNLOCKED_TEST_FLAG
#define PG_HAVE_ATOMIC_UNLOCKED_TEST_FLAG
static inline bool
mdb_atomic_unlocked_test_flag_impl(volatile mdb_atomic_flag *ptr)
{
	return ptr->value == 0;
}
#endif

#ifndef PG_HAVE_ATOMIC_CLEAR_FLAG
#define PG_HAVE_ATOMIC_CLEAR_FLAG
static inline void
mdb_atomic_clear_flag_impl(volatile mdb_atomic_flag *ptr)
{
	__sync_lock_release(&ptr->value);
}
#endif

#ifndef PG_HAVE_ATOMIC_INIT_FLAG
#define PG_HAVE_ATOMIC_INIT_FLAG
static inline void
mdb_atomic_init_flag_impl(volatile mdb_atomic_flag *ptr)
{
	mdb_atomic_clear_flag_impl(ptr);
}
#endif

#endif /* defined(PG_HAVE_ATOMIC_FLAG_SUPPORT) */

/* prefer __atomic, it has a better API */
#if !defined(PG_HAVE_ATOMIC_COMPARE_EXCHANGE_U32) && defined(HAVE_GCC__ATOMIC_INT32_CAS)
#define PG_HAVE_ATOMIC_COMPARE_EXCHANGE_U32
static inline bool
mdb_atomic_compare_exchange_u32_impl(volatile mdb_atomic_uint32 *ptr,
									uint32 *expected, uint32 newval)
{
	/* FIXME: we can probably use a lower consistency model */
	return __atomic_compare_exchange_n(&ptr->value, expected, newval, false,
									   __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}
#endif

#if !defined(PG_HAVE_ATOMIC_COMPARE_EXCHANGE_U32) && defined(HAVE_GCC__SYNC_INT32_CAS)
#define PG_HAVE_ATOMIC_COMPARE_EXCHANGE_U32
static inline bool
mdb_atomic_compare_exchange_u32_impl(volatile mdb_atomic_uint32 *ptr,
									uint32 *expected, uint32 newval)
{
	bool	ret;
	uint32	current;
	current = __sync_val_compare_and_swap(&ptr->value, *expected, newval);
	ret = current == *expected;
	*expected = current;
	return ret;
}
#endif

#if !defined(PG_HAVE_ATOMIC_FETCH_ADD_U32) && defined(HAVE_GCC__SYNC_INT32_CAS)
#define PG_HAVE_ATOMIC_FETCH_ADD_U32
static inline uint32
mdb_atomic_fetch_add_u32_impl(volatile mdb_atomic_uint32 *ptr, int32 add_)
{
	return __sync_fetch_and_add(&ptr->value, add_);
}
#endif


#if !defined(PG_DISABLE_64_BIT_ATOMICS)

#if !defined(PG_HAVE_ATOMIC_COMPARE_EXCHANGE_U64) && defined(HAVE_GCC__ATOMIC_INT64_CAS)
#define PG_HAVE_ATOMIC_COMPARE_EXCHANGE_U64
static inline bool
mdb_atomic_compare_exchange_u64_impl(volatile mdb_atomic_uint64 *ptr,
									uint64 *expected, uint64 newval)
{
	return __atomic_compare_exchange_n(&ptr->value, expected, newval, false,
									   __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}
#endif

#if !defined(PG_HAVE_ATOMIC_COMPARE_EXCHANGE_U64) && defined(HAVE_GCC__SYNC_INT64_CAS)
#define PG_HAVE_ATOMIC_COMPARE_EXCHANGE_U64
static inline bool
mdb_atomic_compare_exchange_u64_impl(volatile mdb_atomic_uint64 *ptr,
									uint64 *expected, uint64 newval)
{
	bool	ret;
	uint64	current;
	current = __sync_val_compare_and_swap(&ptr->value, *expected, newval);
	ret = current == *expected;
	*expected = current;
	return ret;
}
#endif

#if !defined(PG_HAVE_ATOMIC_FETCH_ADD_U64) && defined(HAVE_GCC__SYNC_INT64_CAS)
#define PG_HAVE_ATOMIC_FETCH_ADD_U64
static inline uint64
mdb_atomic_fetch_add_u64_impl(volatile mdb_atomic_uint64 *ptr, int64 add_)
{
	return __sync_fetch_and_add(&ptr->value, add_);
}
#endif

#endif /* !defined(PG_DISABLE_64_BIT_ATOMICS) */

#endif /* defined(HAVE_ATOMICS) */
