/*
 * pgcrypto.h
 *		Header file for pgcrypto.
 *
 * Copyright (c) 2000 Marko Kreen
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *	  notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *	  notice, this list of conditions and the following disclaimer in the
 *	  documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * contrib/pgcrypto/pgcrypto.h
 */

#ifndef _MDB_CRYPTO_H
#define _MDB_CRYPTO_H

#include "fmgr.h"

/* exported functions */
Datum		mdb_digest(MDB_FUNCTION_ARGS);
Datum		mdb_hmac(MDB_FUNCTION_ARGS);
Datum		mdb_gen_salt(MDB_FUNCTION_ARGS);
Datum		mdb_gen_salt_rounds(MDB_FUNCTION_ARGS);
Datum		mdb_crypt(MDB_FUNCTION_ARGS);
Datum		mdb_encrypt(MDB_FUNCTION_ARGS);
Datum		mdb_decrypt(MDB_FUNCTION_ARGS);
Datum		mdb_encrypt_iv(MDB_FUNCTION_ARGS);
Datum		mdb_decrypt_iv(MDB_FUNCTION_ARGS);
Datum		mdb_random_bytes(MDB_FUNCTION_ARGS);
Datum		mdb_random_uuid(MDB_FUNCTION_ARGS);

#endif
