/*
 * pgcrypto.c
 *		Various cryptographic stuff for MollyDB.
 *
 * Copyright (c) 2001 Marko Kreen
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
 * contrib/pgcrypto/pgcrypto.c
 */

#include "mollydb.h"

#include <ctype.h>

#include "parser/scansup.h"
#include "utils/builtins.h"
#include "utils/uuid.h"

#include "px.h"
#include "px-crypt.h"
#include "pgcrypto.h"

MDB_MODULE_MAGIC;

/* private stuff */

typedef int (*PFN) (const char *name, void **res);
static void *find_provider(text *name, PFN pf, char *desc, int silent);

/* SQL function: hash(bytea, text) returns bytea */
MDB_FUNCTION_INFO_V1(mdb_digest);

Datum
mdb_digest(MDB_FUNCTION_ARGS)
{
	bytea	   *arg;
	text	   *name;
	unsigned	len,
				hlen;
	PX_MD	   *md;
	bytea	   *res;

	name = MDB_GETARG_TEXT_P(1);

	/* will give error if fails */
	md = find_provider(name, (PFN) px_find_digest, "Digest", 0);

	hlen = px_md_result_size(md);

	res = (text *) palloc(hlen + VARHDRSZ);
	SET_VARSIZE(res, hlen + VARHDRSZ);

	arg = MDB_GETARG_BYTEA_P(0);
	len = VARSIZE(arg) - VARHDRSZ;

	px_md_update(md, (uint8 *) VARDATA(arg), len);
	px_md_finish(md, (uint8 *) VARDATA(res));
	px_md_free(md);

	MDB_FREE_IF_COPY(arg, 0);
	MDB_FREE_IF_COPY(name, 1);

	MDB_RETURN_BYTEA_P(res);
}

/* SQL function: hmac(data:bytea, key:bytea, type:text) returns bytea */
MDB_FUNCTION_INFO_V1(mdb_hmac);

Datum
mdb_hmac(MDB_FUNCTION_ARGS)
{
	bytea	   *arg;
	bytea	   *key;
	text	   *name;
	unsigned	len,
				hlen,
				klen;
	PX_HMAC    *h;
	bytea	   *res;

	name = MDB_GETARG_TEXT_P(2);

	/* will give error if fails */
	h = find_provider(name, (PFN) px_find_hmac, "HMAC", 0);

	hlen = px_hmac_result_size(h);

	res = (text *) palloc(hlen + VARHDRSZ);
	SET_VARSIZE(res, hlen + VARHDRSZ);

	arg = MDB_GETARG_BYTEA_P(0);
	key = MDB_GETARG_BYTEA_P(1);
	len = VARSIZE(arg) - VARHDRSZ;
	klen = VARSIZE(key) - VARHDRSZ;

	px_hmac_init(h, (uint8 *) VARDATA(key), klen);
	px_hmac_update(h, (uint8 *) VARDATA(arg), len);
	px_hmac_finish(h, (uint8 *) VARDATA(res));
	px_hmac_free(h);

	MDB_FREE_IF_COPY(arg, 0);
	MDB_FREE_IF_COPY(key, 1);
	MDB_FREE_IF_COPY(name, 2);

	MDB_RETURN_BYTEA_P(res);
}


/* SQL function: mdb_gen_salt(text) returns text */
MDB_FUNCTION_INFO_V1(mdb_gen_salt);

Datum
mdb_gen_salt(MDB_FUNCTION_ARGS)
{
	text	   *arg0 = MDB_GETARG_TEXT_PP(0);
	int			len;
	char		buf[PX_MAX_SALT_LEN + 1];

	text_to_cstring_buffer(arg0, buf, sizeof(buf));
	len = px_gen_salt(buf, buf, 0);
	if (len < 0)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				 errmsg("gen_salt: %s", px_strerror(len))));

	MDB_FREE_IF_COPY(arg0, 0);

	MDB_RETURN_TEXT_P(cstring_to_text_with_len(buf, len));
}

/* SQL function: mdb_gen_salt(text, int4) returns text */
MDB_FUNCTION_INFO_V1(mdb_gen_salt_rounds);

Datum
mdb_gen_salt_rounds(MDB_FUNCTION_ARGS)
{
	text	   *arg0 = MDB_GETARG_TEXT_PP(0);
	int			rounds = MDB_GETARG_INT32(1);
	int			len;
	char		buf[PX_MAX_SALT_LEN + 1];

	text_to_cstring_buffer(arg0, buf, sizeof(buf));
	len = px_gen_salt(buf, buf, rounds);
	if (len < 0)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				 errmsg("gen_salt: %s", px_strerror(len))));

	MDB_FREE_IF_COPY(arg0, 0);

	MDB_RETURN_TEXT_P(cstring_to_text_with_len(buf, len));
}

/* SQL function: mdb_crypt(psw:text, salt:text) returns text */
MDB_FUNCTION_INFO_V1(mdb_crypt);

Datum
mdb_crypt(MDB_FUNCTION_ARGS)
{
	text	   *arg0 = MDB_GETARG_TEXT_PP(0);
	text	   *arg1 = MDB_GETARG_TEXT_PP(1);
	char	   *buf0,
			   *buf1,
			   *cres,
			   *resbuf;
	text	   *res;

	buf0 = text_to_cstring(arg0);
	buf1 = text_to_cstring(arg1);

	resbuf = palloc0(PX_MAX_CRYPT);

	cres = px_crypt(buf0, buf1, resbuf, PX_MAX_CRYPT);

	pfree(buf0);
	pfree(buf1);

	if (cres == NULL)
		ereport(ERROR,
				(errcode(ERRCODE_EXTERNAL_ROUTINE_INVOCATION_EXCEPTION),
				 errmsg("crypt(3) returned NULL")));

	res = cstring_to_text(cres);

	pfree(resbuf);

	MDB_FREE_IF_COPY(arg0, 0);
	MDB_FREE_IF_COPY(arg1, 1);

	MDB_RETURN_TEXT_P(res);
}

/* SQL function: mdb_encrypt(bytea, bytea, text) returns bytea */
MDB_FUNCTION_INFO_V1(mdb_encrypt);

Datum
mdb_encrypt(MDB_FUNCTION_ARGS)
{
	int			err;
	bytea	   *data,
			   *key,
			   *res;
	text	   *type;
	PX_Combo   *c;
	unsigned	dlen,
				klen,
				rlen;

	type = MDB_GETARG_TEXT_P(2);
	c = find_provider(type, (PFN) px_find_combo, "Cipher", 0);

	data = MDB_GETARG_BYTEA_P(0);
	key = MDB_GETARG_BYTEA_P(1);
	dlen = VARSIZE(data) - VARHDRSZ;
	klen = VARSIZE(key) - VARHDRSZ;

	rlen = px_combo_encrypt_len(c, dlen);
	res = palloc(VARHDRSZ + rlen);

	err = px_combo_init(c, (uint8 *) VARDATA(key), klen, NULL, 0);
	if (!err)
		err = px_combo_encrypt(c, (uint8 *) VARDATA(data), dlen,
							   (uint8 *) VARDATA(res), &rlen);
	px_combo_free(c);

	MDB_FREE_IF_COPY(data, 0);
	MDB_FREE_IF_COPY(key, 1);
	MDB_FREE_IF_COPY(type, 2);

	if (err)
	{
		pfree(res);
		ereport(ERROR,
				(errcode(ERRCODE_EXTERNAL_ROUTINE_INVOCATION_EXCEPTION),
				 errmsg("encrypt error: %s", px_strerror(err))));
	}

	SET_VARSIZE(res, VARHDRSZ + rlen);
	MDB_RETURN_BYTEA_P(res);
}

/* SQL function: mdb_decrypt(bytea, bytea, text) returns bytea */
MDB_FUNCTION_INFO_V1(mdb_decrypt);

Datum
mdb_decrypt(MDB_FUNCTION_ARGS)
{
	int			err;
	bytea	   *data,
			   *key,
			   *res;
	text	   *type;
	PX_Combo   *c;
	unsigned	dlen,
				klen,
				rlen;

	type = MDB_GETARG_TEXT_P(2);
	c = find_provider(type, (PFN) px_find_combo, "Cipher", 0);

	data = MDB_GETARG_BYTEA_P(0);
	key = MDB_GETARG_BYTEA_P(1);
	dlen = VARSIZE(data) - VARHDRSZ;
	klen = VARSIZE(key) - VARHDRSZ;

	rlen = px_combo_decrypt_len(c, dlen);
	res = palloc(VARHDRSZ + rlen);

	err = px_combo_init(c, (uint8 *) VARDATA(key), klen, NULL, 0);
	if (!err)
		err = px_combo_decrypt(c, (uint8 *) VARDATA(data), dlen,
							   (uint8 *) VARDATA(res), &rlen);

	px_combo_free(c);

	if (err)
		ereport(ERROR,
				(errcode(ERRCODE_EXTERNAL_ROUTINE_INVOCATION_EXCEPTION),
				 errmsg("decrypt error: %s", px_strerror(err))));

	SET_VARSIZE(res, VARHDRSZ + rlen);

	MDB_FREE_IF_COPY(data, 0);
	MDB_FREE_IF_COPY(key, 1);
	MDB_FREE_IF_COPY(type, 2);

	MDB_RETURN_BYTEA_P(res);
}

/* SQL function: mdb_encrypt_iv(bytea, bytea, bytea, text) returns bytea */
MDB_FUNCTION_INFO_V1(mdb_encrypt_iv);

Datum
mdb_encrypt_iv(MDB_FUNCTION_ARGS)
{
	int			err;
	bytea	   *data,
			   *key,
			   *iv,
			   *res;
	text	   *type;
	PX_Combo   *c;
	unsigned	dlen,
				klen,
				ivlen,
				rlen;

	type = MDB_GETARG_TEXT_P(3);
	c = find_provider(type, (PFN) px_find_combo, "Cipher", 0);

	data = MDB_GETARG_BYTEA_P(0);
	key = MDB_GETARG_BYTEA_P(1);
	iv = MDB_GETARG_BYTEA_P(2);
	dlen = VARSIZE(data) - VARHDRSZ;
	klen = VARSIZE(key) - VARHDRSZ;
	ivlen = VARSIZE(iv) - VARHDRSZ;

	rlen = px_combo_encrypt_len(c, dlen);
	res = palloc(VARHDRSZ + rlen);

	err = px_combo_init(c, (uint8 *) VARDATA(key), klen,
						(uint8 *) VARDATA(iv), ivlen);
	if (!err)
		err = px_combo_encrypt(c, (uint8 *) VARDATA(data), dlen,
							   (uint8 *) VARDATA(res), &rlen);

	px_combo_free(c);

	if (err)
		ereport(ERROR,
				(errcode(ERRCODE_EXTERNAL_ROUTINE_INVOCATION_EXCEPTION),
				 errmsg("encrypt_iv error: %s", px_strerror(err))));

	SET_VARSIZE(res, VARHDRSZ + rlen);

	MDB_FREE_IF_COPY(data, 0);
	MDB_FREE_IF_COPY(key, 1);
	MDB_FREE_IF_COPY(iv, 2);
	MDB_FREE_IF_COPY(type, 3);

	MDB_RETURN_BYTEA_P(res);
}

/* SQL function: mdb_decrypt_iv(bytea, bytea, bytea, text) returns bytea */
MDB_FUNCTION_INFO_V1(mdb_decrypt_iv);

Datum
mdb_decrypt_iv(MDB_FUNCTION_ARGS)
{
	int			err;
	bytea	   *data,
			   *key,
			   *iv,
			   *res;
	text	   *type;
	PX_Combo   *c;
	unsigned	dlen,
				klen,
				rlen,
				ivlen;

	type = MDB_GETARG_TEXT_P(3);
	c = find_provider(type, (PFN) px_find_combo, "Cipher", 0);

	data = MDB_GETARG_BYTEA_P(0);
	key = MDB_GETARG_BYTEA_P(1);
	iv = MDB_GETARG_BYTEA_P(2);
	dlen = VARSIZE(data) - VARHDRSZ;
	klen = VARSIZE(key) - VARHDRSZ;
	ivlen = VARSIZE(iv) - VARHDRSZ;

	rlen = px_combo_decrypt_len(c, dlen);
	res = palloc(VARHDRSZ + rlen);

	err = px_combo_init(c, (uint8 *) VARDATA(key), klen,
						(uint8 *) VARDATA(iv), ivlen);
	if (!err)
		err = px_combo_decrypt(c, (uint8 *) VARDATA(data), dlen,
							   (uint8 *) VARDATA(res), &rlen);

	px_combo_free(c);

	if (err)
		ereport(ERROR,
				(errcode(ERRCODE_EXTERNAL_ROUTINE_INVOCATION_EXCEPTION),
				 errmsg("decrypt_iv error: %s", px_strerror(err))));

	SET_VARSIZE(res, VARHDRSZ + rlen);

	MDB_FREE_IF_COPY(data, 0);
	MDB_FREE_IF_COPY(key, 1);
	MDB_FREE_IF_COPY(iv, 2);
	MDB_FREE_IF_COPY(type, 3);

	MDB_RETURN_BYTEA_P(res);
}

/* SQL function: mdb_random_bytes(int4) returns bytea */
MDB_FUNCTION_INFO_V1(mdb_random_bytes);

Datum
mdb_random_bytes(MDB_FUNCTION_ARGS)
{
	int			err;
	int			len = MDB_GETARG_INT32(0);
	bytea	   *res;

	if (len < 1 || len > 1024)
		ereport(ERROR,
				(errcode(ERRCODE_EXTERNAL_ROUTINE_INVOCATION_EXCEPTION),
				 errmsg("Length not in range")));

	res = palloc(VARHDRSZ + len);
	SET_VARSIZE(res, VARHDRSZ + len);

	/* generate result */
	err = px_get_random_bytes((uint8 *) VARDATA(res), len);
	if (err < 0)
		ereport(ERROR,
				(errcode(ERRCODE_EXTERNAL_ROUTINE_INVOCATION_EXCEPTION),
				 errmsg("Random generator error: %s", px_strerror(err))));

	MDB_RETURN_BYTEA_P(res);
}

/* SQL function: gen_random_uuid() returns uuid */
MDB_FUNCTION_INFO_V1(mdb_random_uuid);

Datum
mdb_random_uuid(MDB_FUNCTION_ARGS)
{
	uint8	   *buf = (uint8 *) palloc(UUID_LEN);
	int			err;

	/* generate random bits */
	err = px_get_pseudo_random_bytes(buf, UUID_LEN);
	if (err < 0)
		ereport(ERROR,
				(errcode(ERRCODE_EXTERNAL_ROUTINE_INVOCATION_EXCEPTION),
				 errmsg("Random generator error: %s", px_strerror(err))));

	/*
	 * Set magic numbers for a "version 4" (pseudorandom) UUID, see
	 * http://tools.ietf.org/html/rfc4122#section-4.4
	 */
	buf[6] = (buf[6] & 0x0f) | 0x40;	/* "version" field */
	buf[8] = (buf[8] & 0x3f) | 0x80;	/* "variant" field */

	MDB_RETURN_UUID_P((mdb_uuid_t *) buf);
}

static void *
find_provider(text *name,
			  PFN provider_lookup,
			  char *desc, int silent)
{
	void	   *res;
	char	   *buf;
	int			err;

	buf = downcase_truncate_identifier(VARDATA(name),
									   VARSIZE(name) - VARHDRSZ,
									   false);

	err = provider_lookup(buf, &res);

	if (err && !silent)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				 errmsg("Cannot use \"%s\": %s", buf, px_strerror(err))));

	pfree(buf);

	return err ? NULL : res;
}
