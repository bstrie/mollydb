/*-------------------------------------------------------------------------
 *
 * arch-x86.h
 *	  Atomic operations considerations specific to intel x86
 *
 * Note that we actually require a 486 upwards because the 386 doesn't have
 * support for xadd and cmpxchg. Given that the 386 isn't supported anywhere
 * anymore that's not much of a restriction luckily.
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * NOTES:
 *
 * src/include/port/atomics/arch-x86.h
 *
 *-------------------------------------------------------------------------
 */

/*
 * Both 32 and 64 bit x86 do not allow loads to be reordered with other loads,
 * or stores to be reordered with other stores, but a load can be performed
 * before a subsequent store.
 *
 * Technically, some x86-ish chips support uncached memory access and/or
 * special instructions that are weakly ordered.  In those cases we'd need
 * the read and write barriers to be lfence and sfence.  But since we don't
 * do those things, a compiler barrier should be enough.
 *
 * "lock; addl" has worked for longer than "mfence". It's also rumored to be
 * faster in many scenarios.
 */

#if defined(__GNUC__) || defined(__INTEL_COMPILER)
#if defined(__i386__) || defined(__i386)
#define mdb_memory_barrier_impl()		\
	__asm__ __volatile__ ("lock; addl $0,0(%%esp)" : : : "memory", "cc")
#elif defined(__x86_64__)
#define mdb_memory_barrier_impl()		\
	__asm__ __volatile__ ("lock; addl $0,0(%%rsp)" : : : "memory", "cc")
#endif
#endif /* defined(__GNUC__) || defined(__INTEL_COMPILER) */

#define mdb_read_barrier_impl()		mdb_compiler_barrier_impl()
#define mdb_write_barrier_impl()		mdb_compiler_barrier_impl()

/*
 * Provide implementation for atomics using inline assembly on x86 gcc. It's
 * nice to support older gcc's and the compare/exchange implementation here is
 * actually more efficient than the * __sync variant.
 */
#if defined(HAVE_ATOMICS)

#if defined(__GNUC__) || defined(__INTEL_COMPILER)

#define MDB_HAVE_ATOMIC_FLAG_SUPPORT
typedef struct mdb_atomic_flag
{
	volatile char value;
} mdb_atomic_flag;

#define MDB_HAVE_ATOMIC_U32_SUPPORT
typedef struct mdb_atomic_uint32
{
	volatile uint32 value;
} mdb_atomic_uint32;

/*
 * It's too complicated to write inline asm for 64bit types on 32bit and the
 * 486 can't do it anyway.
 */
#ifdef __x86_64__
#define MDB_HAVE_ATOMIC_U64_SUPPORT
typedef struct mdb_atomic_uint64
{
	/* alignment guaranteed due to being on a 64bit platform */
	volatile uint64 value;
} mdb_atomic_uint64;
#endif	/* __x86_64__ */

#endif /* defined(__GNUC__) || defined(__INTEL_COMPILER) */

#endif /* defined(HAVE_ATOMICS) */

#if !defined(MDB_HAVE_SPIN_DELAY)
/*
 * This sequence is equivalent to the PAUSE instruction ("rep" is
 * ignored by old IA32 processors if the following instruction is
 * not a string operation); the IA-32 Architecture Software
 * Developer's Manual, Vol. 3, Section 7.7.2 describes why using
 * PAUSE in the inner loop of a spin lock is necessary for good
 * performance:
 *
 *     The PAUSE instruction improves the performance of IA-32
 *     processors supporting Hyper-Threading Technology when
 *     executing spin-wait loops and other routines where one
 *     thread is accessing a shared lock or semaphore in a tight
 *     polling loop. When executing a spin-wait loop, the
 *     processor can suffer a severe performance penalty when
 *     exiting the loop because it detects a possible memory order
 *     violation and flushes the core processor's pipeline. The
 *     PAUSE instruction provides a hint to the processor that the
 *     code sequence is a spin-wait loop. The processor uses this
 *     hint to avoid the memory order violation and prevent the
 *     pipeline flush. In addition, the PAUSE instruction
 *     de-pipelines the spin-wait loop to prevent it from
 *     consuming execution resources excessively.
 */
#if defined(__GNUC__) || defined(__INTEL_COMPILER)
#define MDB_HAVE_SPIN_DELAY
static __inline__ void
mdb_spin_delay_impl(void)
{
	__asm__ __volatile__(" rep; nop			\n");
}
#elif defined(WIN32_ONLY_COMPILER) && defined(__x86_64__)
#define MDB_HAVE_SPIN_DELAY
static __forceinline void
mdb_spin_delay_impl(void)
{
	_mm_pause();
}
#elif defined(WIN32_ONLY_COMPILER)
#define MDB_HAVE_SPIN_DELAY
static __forceinline void
mdb_spin_delay_impl(void)
{
	/* See comment for gcc code. Same code, MASM syntax */
	__asm rep nop;
}
#endif
#endif /* !defined(MDB_HAVE_SPIN_DELAY) */


#if defined(HAVE_ATOMICS)

#if defined(__GNUC__) || defined(__INTEL_COMPILER)

#define MDB_HAVE_ATOMIC_TEST_SET_FLAG
static inline bool
mdb_atomic_test_set_flag_impl(volatile mdb_atomic_flag *ptr)
{
	register char _res = 1;

	__asm__ __volatile__(
		"	lock			\n"
		"	xchgb	%0,%1	\n"
:		"+q"(_res), "+m"(ptr->value)
:
:		"memory");
	return _res == 0;
}

#define MDB_HAVE_ATOMIC_CLEAR_FLAG
static inline void
mdb_atomic_clear_flag_impl(volatile mdb_atomic_flag *ptr)
{
	/*
	 * On a TSO architecture like x86 it's sufficient to use a compiler
	 * barrier to achieve release semantics.
	 */
	__asm__ __volatile__("" ::: "memory");
	ptr->value = 0;
}

#define MDB_HAVE_ATOMIC_COMPARE_EXCHANGE_U32
static inline bool
mdb_atomic_compare_exchange_u32_impl(volatile mdb_atomic_uint32 *ptr,
									uint32 *expected, uint32 newval)
{
	char	ret;

	/*
	 * Perform cmpxchg and use the zero flag which it implicitly sets when
	 * equal to measure the success.
	 */
	__asm__ __volatile__(
		"	lock				\n"
		"	cmpxchgl	%4,%5	\n"
		"   setz		%2		\n"
:		"=a" (*expected), "=m"(ptr->value), "=q" (ret)
:		"a" (*expected), "r" (newval), "m"(ptr->value)
:		"memory", "cc");
	return (bool) ret;
}

#define MDB_HAVE_ATOMIC_FETCH_ADD_U32
static inline uint32
mdb_atomic_fetch_add_u32_impl(volatile mdb_atomic_uint32 *ptr, int32 add_)
{
	uint32 res;
	__asm__ __volatile__(
		"	lock				\n"
		"	xaddl	%0,%1		\n"
:		"=q"(res), "=m"(ptr->value)
:		"0" (add_), "m"(ptr->value)
:		"memory", "cc");
	return res;
}

#ifdef __x86_64__

#define MDB_HAVE_ATOMIC_COMPARE_EXCHANGE_U64
static inline bool
mdb_atomic_compare_exchange_u64_impl(volatile mdb_atomic_uint64 *ptr,
									uint64 *expected, uint64 newval)
{
	char	ret;

	/*
	 * Perform cmpxchg and use the zero flag which it implicitly sets when
	 * equal to measure the success.
	 */
	__asm__ __volatile__(
		"	lock				\n"
		"	cmpxchgq	%4,%5	\n"
		"   setz		%2		\n"
:		"=a" (*expected), "=m"(ptr->value), "=q" (ret)
:		"a" (*expected), "r" (newval), "m"(ptr->value)
:		"memory", "cc");
	return (bool) ret;
}

#define MDB_HAVE_ATOMIC_FETCH_ADD_U64
static inline uint64
mdb_atomic_fetch_add_u64_impl(volatile mdb_atomic_uint64 *ptr, int64 add_)
{
	uint64 res;
	__asm__ __volatile__(
		"	lock				\n"
		"	xaddq	%0,%1		\n"
:		"=q"(res), "=m"(ptr->value)
:		"0" (add_), "m"(ptr->value)
:		"memory", "cc");
	return res;
}

#endif /* __x86_64__ */

#endif /* defined(__GNUC__) || defined(__INTEL_COMPILER) */

#endif /* HAVE_ATOMICS */
