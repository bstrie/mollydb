/*-------------------------------------------------------------------------
 *
 * varbit.h
 *	  Functions for the SQL datatypes BIT() and BIT VARYING().
 *
 * Code originally contributed by Adriaan Joubert.
 *
 * Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/varbit.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef VARBIT_H
#define VARBIT_H

#include <limits.h>

#include "fmgr.h"

/*
 * Modeled on struct varlena from mollydb.h, but data type is bits8.
 */
typedef struct
{
	int32		vl_len_;		/* varlena header (do not touch directly!) */
	int32		bit_len;		/* number of valid bits */
	bits8		bit_dat[FLEXIBLE_ARRAY_MEMBER]; /* bit string, most sig. byte
												 * first */
} VarBit;

/*
 * fmgr interface macros
 *
 * BIT and BIT VARYING are toastable varlena types.  They are the same
 * as far as representation goes, so we just have one set of macros.
 */
#define DatumGetVarBitP(X)		   ((VarBit *) MDB_DETOAST_DATUM(X))
#define DatumGetVarBitPCopy(X)	   ((VarBit *) MDB_DETOAST_DATUM_COPY(X))
#define VarBitPGetDatum(X)		   PointerGetDatum(X)
#define MDB_GETARG_VARBIT_P(n)	   DatumGetVarBitP(MDB_GETARG_DATUM(n))
#define MDB_GETARG_VARBIT_P_COPY(n) DatumGetVarBitPCopy(MDB_GETARG_DATUM(n))
#define MDB_RETURN_VARBIT_P(x)	   return VarBitPGetDatum(x)

/* Header overhead *in addition to* VARHDRSZ */
#define VARBITHDRSZ			sizeof(int32)
/* Number of bits in this bit string */
#define VARBITLEN(PTR)		(((VarBit *) (PTR))->bit_len)
/* Pointer to the first byte containing bit string data */
#define VARBITS(PTR)		(((VarBit *) (PTR))->bit_dat)
/* Number of bytes in the data section of a bit string */
#define VARBITBYTES(PTR)	(VARSIZE(PTR) - VARHDRSZ - VARBITHDRSZ)
/* Padding of the bit string at the end (in bits) */
#define VARBITPAD(PTR)		(VARBITBYTES(PTR)*BITS_PER_BYTE - VARBITLEN(PTR))
/* Number of bytes needed to store a bit string of a given length */
#define VARBITTOTALLEN(BITLEN)	(((BITLEN) + BITS_PER_BYTE-1)/BITS_PER_BYTE + \
								 VARHDRSZ + VARBITHDRSZ)
/*
 * Maximum number of bits.  Several code sites assume no overflow from
 * computing bitlen + X; VARBITTOTALLEN() has the largest such X.
 */
#define VARBITMAXLEN		(INT_MAX - BITS_PER_BYTE + 1)
/* pointer beyond the end of the bit string (like end() in STL containers) */
#define VARBITEND(PTR)		(((bits8 *) (PTR)) + VARSIZE(PTR))
/* Mask that will cover exactly one byte, i.e. BITS_PER_BYTE bits */
#define BITMASK 0xFF


extern Datum bit_in(MDB_FUNCTION_ARGS);
extern Datum bit_out(MDB_FUNCTION_ARGS);
extern Datum bit_recv(MDB_FUNCTION_ARGS);
extern Datum bit_send(MDB_FUNCTION_ARGS);
extern Datum bittypmodin(MDB_FUNCTION_ARGS);
extern Datum bittypmodout(MDB_FUNCTION_ARGS);
extern Datum varbit_in(MDB_FUNCTION_ARGS);
extern Datum varbit_out(MDB_FUNCTION_ARGS);
extern Datum varbit_recv(MDB_FUNCTION_ARGS);
extern Datum varbit_send(MDB_FUNCTION_ARGS);
extern Datum varbittypmodin(MDB_FUNCTION_ARGS);
extern Datum varbittypmodout(MDB_FUNCTION_ARGS);
extern Datum bit(MDB_FUNCTION_ARGS);
extern Datum varbit_transform(MDB_FUNCTION_ARGS);
extern Datum varbit(MDB_FUNCTION_ARGS);
extern Datum biteq(MDB_FUNCTION_ARGS);
extern Datum bitne(MDB_FUNCTION_ARGS);
extern Datum bitlt(MDB_FUNCTION_ARGS);
extern Datum bitle(MDB_FUNCTION_ARGS);
extern Datum bitgt(MDB_FUNCTION_ARGS);
extern Datum bitge(MDB_FUNCTION_ARGS);
extern Datum bitcmp(MDB_FUNCTION_ARGS);

/* avoid the names bitand and bitor, since they are C++ keywords */
extern Datum bit_and(MDB_FUNCTION_ARGS);
extern Datum bit_or(MDB_FUNCTION_ARGS);
extern Datum bitxor(MDB_FUNCTION_ARGS);
extern Datum bitnot(MDB_FUNCTION_ARGS);
extern Datum bitshiftleft(MDB_FUNCTION_ARGS);
extern Datum bitshiftright(MDB_FUNCTION_ARGS);
extern Datum bitcat(MDB_FUNCTION_ARGS);
extern Datum bitsubstr(MDB_FUNCTION_ARGS);
extern Datum bitsubstr_no_len(MDB_FUNCTION_ARGS);
extern Datum bitoverlay(MDB_FUNCTION_ARGS);
extern Datum bitoverlay_no_len(MDB_FUNCTION_ARGS);
extern Datum bitlength(MDB_FUNCTION_ARGS);
extern Datum bitoctetlength(MDB_FUNCTION_ARGS);
extern Datum bitfromint4(MDB_FUNCTION_ARGS);
extern Datum bittoint4(MDB_FUNCTION_ARGS);
extern Datum bitfromint8(MDB_FUNCTION_ARGS);
extern Datum bittoint8(MDB_FUNCTION_ARGS);
extern Datum bitposition(MDB_FUNCTION_ARGS);
extern Datum bitsetbit(MDB_FUNCTION_ARGS);
extern Datum bitgetbit(MDB_FUNCTION_ARGS);

#endif
