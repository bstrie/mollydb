/*-------------------------------------------------------------------------
 *
 * atomics.c
 *	   Non-Inline parts of the atomics implementation
 *
 * Portions Copyright (c) 2013-2016, MollyDB Global Development Group
 *
 *
 * IDENTIFICATION
 *	  src/backend/port/atomics.c
 *
 *-------------------------------------------------------------------------
 */
#include "mollydb.h"

#include "miscadmin.h"
#include "port/atomics.h"
#include "storage/spin.h"

#ifdef PG_HAVE_MEMORY_BARRIER_EMULATION
#ifdef WIN32
#error "barriers are required (and provided) on WIN32 platforms"
#endif
#include <sys/types.h>
#include <signal.h>
#endif

#ifdef PG_HAVE_MEMORY_BARRIER_EMULATION
void
mdb_spinlock_barrier(void)
{
	/*
	 * NB: we have to be reentrant here, some barriers are placed in signal
	 * handlers.
	 *
	 * We use kill(0) for the fallback barrier as we assume that kernels on
	 * systems old enough to require fallback barrier support will include an
	 * appropriate barrier while checking the existence of the postmaster
	 * pid.
	 */
	(void) kill(PostmasterPid, 0);
}
#endif

#ifdef PG_HAVE_COMPILER_BARRIER_EMULATION
void
mdb_extern_compiler_barrier(void)
{
	/* do nothing */
}
#endif


#ifdef PG_HAVE_ATOMIC_FLAG_SIMULATION

void
mdb_atomic_init_flag_impl(volatile mdb_atomic_flag *ptr)
{
	StaticAssertStmt(sizeof(ptr->sema) >= sizeof(slock_t),
					 "size mismatch of atomic_flag vs slock_t");

#ifndef HAVE_SPINLOCKS

	/*
	 * NB: If we're using semaphore based TAS emulation, be careful to use a
	 * separate set of semaphores. Otherwise we'd get in trouble if an atomic
	 * var would be manipulated while spinlock is held.
	 */
	s_init_lock_sema((slock_t *) &ptr->sema, true);
#else
	SpinLockInit((slock_t *) &ptr->sema);
#endif
}

bool
mdb_atomic_test_set_flag_impl(volatile mdb_atomic_flag *ptr)
{
	return TAS((slock_t *) &ptr->sema);
}

void
mdb_atomic_clear_flag_impl(volatile mdb_atomic_flag *ptr)
{
	S_UNLOCK((slock_t *) &ptr->sema);
}

#endif   /* PG_HAVE_ATOMIC_FLAG_SIMULATION */

#ifdef PG_HAVE_ATOMIC_U32_SIMULATION
void
mdb_atomic_init_u32_impl(volatile mdb_atomic_uint32 *ptr, uint32 val_)
{
	StaticAssertStmt(sizeof(ptr->sema) >= sizeof(slock_t),
					 "size mismatch of atomic_flag vs slock_t");

	/*
	 * If we're using semaphore based atomic flags, be careful about nested
	 * usage of atomics while a spinlock is held.
	 */
#ifndef HAVE_SPINLOCKS
	s_init_lock_sema((slock_t *) &ptr->sema, true);
#else
	SpinLockInit((slock_t *) &ptr->sema);
#endif
	ptr->value = val_;
}

bool
mdb_atomic_compare_exchange_u32_impl(volatile mdb_atomic_uint32 *ptr,
									uint32 *expected, uint32 newval)
{
	bool		ret;

	/*
	 * Do atomic op under a spinlock. It might look like we could just skip
	 * the cmpxchg if the lock isn't available, but that'd just emulate a
	 * 'weak' compare and swap. I.e. one that allows spurious failures. Since
	 * several algorithms rely on a strong variant and that is efficiently
	 * implementable on most major architectures let's emulate it here as
	 * well.
	 */
	SpinLockAcquire((slock_t *) &ptr->sema);

	/* perform compare/exchange logic */
	ret = ptr->value == *expected;
	*expected = ptr->value;
	if (ret)
		ptr->value = newval;

	/* and release lock */
	SpinLockRelease((slock_t *) &ptr->sema);

	return ret;
}

uint32
mdb_atomic_fetch_add_u32_impl(volatile mdb_atomic_uint32 *ptr, int32 add_)
{
	uint32		oldval;

	SpinLockAcquire((slock_t *) &ptr->sema);
	oldval = ptr->value;
	ptr->value += add_;
	SpinLockRelease((slock_t *) &ptr->sema);
	return oldval;
}

#endif   /* PG_HAVE_ATOMIC_U32_SIMULATION */
