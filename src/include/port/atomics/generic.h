/*-------------------------------------------------------------------------
 *
 * generic.h
 *	  Implement higher level operations based on some lower level atomic
 *	  operations.
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/port/atomics/generic.h
 *
 *-------------------------------------------------------------------------
 */

/* intentionally no include guards, should only be included by atomics.h */
#ifndef INSIDE_ATOMICS_H
#	error "should be included via atomics.h"
#endif

/*
 * If read or write barriers are undefined, we upgrade them to full memory
 * barriers.
 */
#if !defined(mdb_read_barrier_impl)
#	define mdb_read_barrier_impl mdb_memory_barrier_impl
#endif
#if !defined(mdb_write_barrier_impl)
#	define mdb_write_barrier_impl mdb_memory_barrier_impl
#endif

#ifndef MDB_HAVE_SPIN_DELAY
#define MDB_HAVE_SPIN_DELAY
#define mdb_spin_delay_impl()	((void)0)
#endif


/* provide fallback */
#if !defined(MDB_HAVE_ATOMIC_FLAG_SUPPORT) && defined(MDB_HAVE_ATOMIC_U32_SUPPORT)
#define MDB_HAVE_ATOMIC_FLAG_SUPPORT
typedef mdb_atomic_uint32 mdb_atomic_flag;
#endif

#ifndef MDB_HAVE_ATOMIC_READ_U32
#define MDB_HAVE_ATOMIC_READ_U32
static inline uint32
mdb_atomic_read_u32_impl(volatile mdb_atomic_uint32 *ptr)
{
	return *(&ptr->value);
}
#endif

#ifndef MDB_HAVE_ATOMIC_WRITE_U32
#define MDB_HAVE_ATOMIC_WRITE_U32
static inline void
mdb_atomic_write_u32_impl(volatile mdb_atomic_uint32 *ptr, uint32 val)
{
	ptr->value = val;
}
#endif

/*
 * provide fallback for test_and_set using atomic_exchange if available
 */
#if !defined(MDB_HAVE_ATOMIC_TEST_SET_FLAG) && defined(MDB_HAVE_ATOMIC_EXCHANGE_U32)

#define MDB_HAVE_ATOMIC_INIT_FLAG
static inline void
mdb_atomic_init_flag_impl(volatile mdb_atomic_flag *ptr)
{
	mdb_atomic_write_u32_impl(ptr, 0);
}

#define MDB_HAVE_ATOMIC_TEST_SET_FLAG
static inline bool
mdb_atomic_test_set_flag_impl(volatile mdb_atomic_flag *ptr)
{
	return mdb_atomic_exchange_u32_impl(ptr, &value, 1) == 0;
}

#define MDB_HAVE_ATOMIC_UNLOCKED_TEST_FLAG
static inline bool
mdb_atomic_unlocked_test_flag_impl(volatile mdb_atomic_flag *ptr)
{
	return mdb_atomic_read_u32_impl(ptr) == 0;
}


#define MDB_HAVE_ATOMIC_CLEAR_FLAG
static inline void
mdb_atomic_clear_flag_impl(volatile mdb_atomic_flag *ptr)
{
	/* XXX: release semantics suffice? */
	mdb_memory_barrier_impl();
	mdb_atomic_write_u32_impl(ptr, 0);
}

/*
 * provide fallback for test_and_set using atomic_compare_exchange if
 * available.
 */
#elif !defined(MDB_HAVE_ATOMIC_TEST_SET_FLAG) && defined(MDB_HAVE_ATOMIC_COMPARE_EXCHANGE_U32)

#define MDB_HAVE_ATOMIC_INIT_FLAG
static inline void
mdb_atomic_init_flag_impl(volatile mdb_atomic_flag *ptr)
{
	mdb_atomic_write_u32_impl(ptr, 0);
}

#define MDB_HAVE_ATOMIC_TEST_SET_FLAG
static inline bool
mdb_atomic_test_set_flag_impl(volatile mdb_atomic_flag *ptr)
{
	uint32 value = 0;
	return mdb_atomic_compare_exchange_u32_impl(ptr, &value, 1);
}

#define MDB_HAVE_ATOMIC_UNLOCKED_TEST_FLAG
static inline bool
mdb_atomic_unlocked_test_flag_impl(volatile mdb_atomic_flag *ptr)
{
	return mdb_atomic_read_u32_impl(ptr) == 0;
}

#define MDB_HAVE_ATOMIC_CLEAR_FLAG
static inline void
mdb_atomic_clear_flag_impl(volatile mdb_atomic_flag *ptr)
{
	/*
	 * Use a memory barrier + plain write if we have a native memory
	 * barrier. But don't do so if memory barriers use spinlocks - that'd lead
	 * to circularity if flags are used to implement spinlocks.
	 */
#ifndef MDB_HAVE_MEMORY_BARRIER_EMULATION
	/* XXX: release semantics suffice? */
	mdb_memory_barrier_impl();
	mdb_atomic_write_u32_impl(ptr, 0);
#else
	uint32 value = 1;
	mdb_atomic_compare_exchange_u32_impl(ptr, &value, 0);
#endif
}

#elif !defined(MDB_HAVE_ATOMIC_TEST_SET_FLAG)
#	error "No mdb_atomic_test_and_set provided"
#endif /* !defined(MDB_HAVE_ATOMIC_TEST_SET_FLAG) */


#ifndef MDB_HAVE_ATOMIC_INIT_U32
#define MDB_HAVE_ATOMIC_INIT_U32
static inline void
mdb_atomic_init_u32_impl(volatile mdb_atomic_uint32 *ptr, uint32 val_)
{
	mdb_atomic_write_u32_impl(ptr, val_);
}
#endif

#if !defined(MDB_HAVE_ATOMIC_EXCHANGE_U32) && defined(MDB_HAVE_ATOMIC_COMPARE_EXCHANGE_U32)
#define MDB_HAVE_ATOMIC_EXCHANGE_U32
static inline uint32
mdb_atomic_exchange_u32_impl(volatile mdb_atomic_uint32 *ptr, uint32 xchg_)
{
	uint32 old;
	while (true)
	{
		old = mdb_atomic_read_u32_impl(ptr);
		if (mdb_atomic_compare_exchange_u32_impl(ptr, &old, xchg_))
			break;
	}
	return old;
}
#endif

#if !defined(MDB_HAVE_ATOMIC_FETCH_ADD_U32) && defined(MDB_HAVE_ATOMIC_COMPARE_EXCHANGE_U32)
#define MDB_HAVE_ATOMIC_FETCH_ADD_U32
static inline uint32
mdb_atomic_fetch_add_u32_impl(volatile mdb_atomic_uint32 *ptr, int32 add_)
{
	uint32 old;
	while (true)
	{
		old = mdb_atomic_read_u32_impl(ptr);
		if (mdb_atomic_compare_exchange_u32_impl(ptr, &old, old + add_))
			break;
	}
	return old;
}
#endif

#if !defined(MDB_HAVE_ATOMIC_FETCH_SUB_U32) && defined(MDB_HAVE_ATOMIC_COMPARE_EXCHANGE_U32)
#define MDB_HAVE_ATOMIC_FETCH_SUB_U32
static inline uint32
mdb_atomic_fetch_sub_u32_impl(volatile mdb_atomic_uint32 *ptr, int32 sub_)
{
	return mdb_atomic_fetch_add_u32_impl(ptr, -sub_);
}
#endif

#if !defined(MDB_HAVE_ATOMIC_FETCH_AND_U32) && defined(MDB_HAVE_ATOMIC_COMPARE_EXCHANGE_U32)
#define MDB_HAVE_ATOMIC_FETCH_AND_U32
static inline uint32
mdb_atomic_fetch_and_u32_impl(volatile mdb_atomic_uint32 *ptr, uint32 and_)
{
	uint32 old;
	while (true)
	{
		old = mdb_atomic_read_u32_impl(ptr);
		if (mdb_atomic_compare_exchange_u32_impl(ptr, &old, old & and_))
			break;
	}
	return old;
}
#endif

#if !defined(MDB_HAVE_ATOMIC_FETCH_OR_U32) && defined(MDB_HAVE_ATOMIC_COMPARE_EXCHANGE_U32)
#define MDB_HAVE_ATOMIC_FETCH_OR_U32
static inline uint32
mdb_atomic_fetch_or_u32_impl(volatile mdb_atomic_uint32 *ptr, uint32 or_)
{
	uint32 old;
	while (true)
	{
		old = mdb_atomic_read_u32_impl(ptr);
		if (mdb_atomic_compare_exchange_u32_impl(ptr, &old, old | or_))
			break;
	}
	return old;
}
#endif

#if !defined(MDB_HAVE_ATOMIC_ADD_FETCH_U32) && defined(MDB_HAVE_ATOMIC_FETCH_ADD_U32)
#define MDB_HAVE_ATOMIC_ADD_FETCH_U32
static inline uint32
mdb_atomic_add_fetch_u32_impl(volatile mdb_atomic_uint32 *ptr, int32 add_)
{
	return mdb_atomic_fetch_add_u32_impl(ptr, add_) + add_;
}
#endif

#if !defined(MDB_HAVE_ATOMIC_SUB_FETCH_U32) && defined(MDB_HAVE_ATOMIC_FETCH_SUB_U32)
#define MDB_HAVE_ATOMIC_SUB_FETCH_U32
static inline uint32
mdb_atomic_sub_fetch_u32_impl(volatile mdb_atomic_uint32 *ptr, int32 sub_)
{
	return mdb_atomic_fetch_sub_u32_impl(ptr, sub_) - sub_;
}
#endif

#ifdef MDB_HAVE_ATOMIC_U64_SUPPORT

#if !defined(MDB_HAVE_ATOMIC_EXCHANGE_U64) && defined(MDB_HAVE_ATOMIC_COMPARE_EXCHANGE_U64)
#define MDB_HAVE_ATOMIC_EXCHANGE_U64
static inline uint64
mdb_atomic_exchange_u64_impl(volatile mdb_atomic_uint64 *ptr, uint64 xchg_)
{
	uint64 old;
	while (true)
	{
		old = ptr->value;
		if (mdb_atomic_compare_exchange_u64_impl(ptr, &old, xchg_))
			break;
	}
	return old;
}
#endif

#ifndef MDB_HAVE_ATOMIC_WRITE_U64
#define MDB_HAVE_ATOMIC_WRITE_U64
static inline void
mdb_atomic_write_u64_impl(volatile mdb_atomic_uint64 *ptr, uint64 val)
{
	/*
	 * 64 bit writes aren't safe on all platforms. In the generic
	 * implementation implement them as an atomic exchange.
	 */
	mdb_atomic_exchange_u64_impl(ptr, val);
}
#endif

#ifndef MDB_HAVE_ATOMIC_READ_U64
#define MDB_HAVE_ATOMIC_READ_U64
static inline uint64
mdb_atomic_read_u64_impl(volatile mdb_atomic_uint64 *ptr)
{
	uint64 old = 0;

	/*
	 * 64 bit reads aren't safe on all platforms. In the generic
	 * implementation implement them as a compare/exchange with 0. That'll
	 * fail or succeed, but always return the old value. Possible might store
	 * a 0, but only if the prev. value also was a 0 - i.e. harmless.
	 */
	mdb_atomic_compare_exchange_u64_impl(ptr, &old, 0);

	return old;
}
#endif

#ifndef MDB_HAVE_ATOMIC_INIT_U64
#define MDB_HAVE_ATOMIC_INIT_U64
static inline void
mdb_atomic_init_u64_impl(volatile mdb_atomic_uint64 *ptr, uint64 val_)
{
	mdb_atomic_write_u64_impl(ptr, val_);
}
#endif

#if !defined(MDB_HAVE_ATOMIC_FETCH_ADD_U64) && defined(MDB_HAVE_ATOMIC_COMPARE_EXCHANGE_U64)
#define MDB_HAVE_ATOMIC_FETCH_ADD_U64
static inline uint64
mdb_atomic_fetch_add_u64_impl(volatile mdb_atomic_uint64 *ptr, int64 add_)
{
	uint64 old;
	while (true)
	{
		old = mdb_atomic_read_u64_impl(ptr);
		if (mdb_atomic_compare_exchange_u64_impl(ptr, &old, old + add_))
			break;
	}
	return old;
}
#endif

#if !defined(MDB_HAVE_ATOMIC_FETCH_SUB_U64) && defined(MDB_HAVE_ATOMIC_COMPARE_EXCHANGE_U64)
#define MDB_HAVE_ATOMIC_FETCH_SUB_U64
static inline uint64
mdb_atomic_fetch_sub_u64_impl(volatile mdb_atomic_uint64 *ptr, int64 sub_)
{
	return mdb_atomic_fetch_add_u64_impl(ptr, -sub_);
}
#endif

#if !defined(MDB_HAVE_ATOMIC_FETCH_AND_U64) && defined(MDB_HAVE_ATOMIC_COMPARE_EXCHANGE_U64)
#define MDB_HAVE_ATOMIC_FETCH_AND_U64
static inline uint64
mdb_atomic_fetch_and_u64_impl(volatile mdb_atomic_uint64 *ptr, uint64 and_)
{
	uint64 old;
	while (true)
	{
		old = mdb_atomic_read_u64_impl(ptr);
		if (mdb_atomic_compare_exchange_u64_impl(ptr, &old, old & and_))
			break;
	}
	return old;
}
#endif

#if !defined(MDB_HAVE_ATOMIC_FETCH_OR_U64) && defined(MDB_HAVE_ATOMIC_COMPARE_EXCHANGE_U64)
#define MDB_HAVE_ATOMIC_FETCH_OR_U64
static inline uint64
mdb_atomic_fetch_or_u64_impl(volatile mdb_atomic_uint64 *ptr, uint64 or_)
{
	uint64 old;
	while (true)
	{
		old = mdb_atomic_read_u64_impl(ptr);
		if (mdb_atomic_compare_exchange_u64_impl(ptr, &old, old | or_))
			break;
	}
	return old;
}
#endif

#if !defined(MDB_HAVE_ATOMIC_ADD_FETCH_U64) && defined(MDB_HAVE_ATOMIC_FETCH_ADD_U64)
#define MDB_HAVE_ATOMIC_ADD_FETCH_U64
static inline uint64
mdb_atomic_add_fetch_u64_impl(volatile mdb_atomic_uint64 *ptr, int64 add_)
{
	return mdb_atomic_fetch_add_u64_impl(ptr, add_) + add_;
}
#endif

#if !defined(MDB_HAVE_ATOMIC_SUB_FETCH_U64) && defined(MDB_HAVE_ATOMIC_FETCH_SUB_U64)
#define MDB_HAVE_ATOMIC_SUB_FETCH_U64
static inline uint64
mdb_atomic_sub_fetch_u64_impl(volatile mdb_atomic_uint64 *ptr, int64 sub_)
{
	return mdb_atomic_fetch_sub_u64_impl(ptr, sub_) - sub_;
}
#endif

#endif /* MDB_HAVE_ATOMIC_COMPARE_EXCHANGE_U64 */
