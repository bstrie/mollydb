/*-------------------------------------------------------------------------
 *
 * mdb_crc32c_choose.c
 *	  Choose which CRC-32C implementation to use, at runtime.
 *
 * Try to the special CRC instructions introduced in Intel SSE 4.2,
 * if available on the platform we're running on, but fall back to the
 * slicing-by-8 implementation otherwise.
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/port/mdb_crc32c_choose.c
 *
 *-------------------------------------------------------------------------
 */

#include "c.h"

#ifdef HAVE__GET_CPUID
#include <cpuid.h>
#endif

#ifdef HAVE__CPUID
#include <intrin.h>
#endif

#include "port/mdb_crc32c.h"

static bool
mdb_crc32c_sse42_available(void)
{
	unsigned int exx[4] = {0, 0, 0, 0};

#if defined(HAVE__GET_CPUID)
	__get_cpuid(1, &exx[0], &exx[1], &exx[2], &exx[3]);
#elif defined(HAVE__CPUID)
	__cpuid(exx, 1);
#else
#error cpuid instruction not available
#endif

	return (exx[2] & (1 << 20)) != 0;	/* SSE 4.2 */
}

/*
 * This gets called on the first call. It replaces the function pointer
 * so that subsequent calls are routed directly to the chosen implementation.
 */
static mdb_crc32c
mdb_comp_crc32c_choose(mdb_crc32c crc, const void *data, size_t len)
{
	if (mdb_crc32c_sse42_available())
		mdb_comp_crc32c = mdb_comp_crc32c_sse42;
	else
		mdb_comp_crc32c = mdb_comp_crc32c_sb8;

	return mdb_comp_crc32c(crc, data, len);
}

mdb_crc32c	(*mdb_comp_crc32c) (mdb_crc32c crc, const void *data, size_t len) = mdb_comp_crc32c_choose;
