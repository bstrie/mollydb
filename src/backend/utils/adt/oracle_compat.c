/*-------------------------------------------------------------------------
 * oracle_compat.c
 *	Oracle compatible functions.
 *
 * Copyright (c) 1996-2016, MollyDB Global Development Group
 *
 *	Author: Edmund Mergl <E.Mergl@bawue.de>
 *	Multibyte enhancement: Tatsuo Ishii <ishii@mollydb.org>
 *
 *
 * IDENTIFICATION
 *	src/backend/utils/adt/oracle_compat.c
 *
 *-------------------------------------------------------------------------
 */
#include "mollydb.h"

#include "utils/builtins.h"
#include "utils/formatting.h"
#include "mb/mdb_wchar.h"


static text *dotrim(const char *string, int stringlen,
	   const char *set, int setlen,
	   bool doltrim, bool dortrim);


/********************************************************************
 *
 * lower
 *
 * Syntax:
 *
 *	 text lower(text string)
 *
 * Purpose:
 *
 *	 Returns string, with all letters forced to lowercase.
 *
 ********************************************************************/

Datum
lower(MDB_FUNCTION_ARGS)
{
	text	   *in_string = MDB_GETARG_TEXT_PP(0);
	char	   *out_string;
	text	   *result;

	out_string = str_tolower(VARDATA_ANY(in_string),
							 VARSIZE_ANY_EXHDR(in_string),
							 MDB_GET_COLLATION());
	result = cstring_to_text(out_string);
	pfree(out_string);

	MDB_RETURN_TEXT_P(result);
}


/********************************************************************
 *
 * upper
 *
 * Syntax:
 *
 *	 text upper(text string)
 *
 * Purpose:
 *
 *	 Returns string, with all letters forced to uppercase.
 *
 ********************************************************************/

Datum
upper(MDB_FUNCTION_ARGS)
{
	text	   *in_string = MDB_GETARG_TEXT_PP(0);
	char	   *out_string;
	text	   *result;

	out_string = str_toupper(VARDATA_ANY(in_string),
							 VARSIZE_ANY_EXHDR(in_string),
							 MDB_GET_COLLATION());
	result = cstring_to_text(out_string);
	pfree(out_string);

	MDB_RETURN_TEXT_P(result);
}


/********************************************************************
 *
 * initcap
 *
 * Syntax:
 *
 *	 text initcap(text string)
 *
 * Purpose:
 *
 *	 Returns string, with first letter of each word in uppercase, all
 *	 other letters in lowercase. A word is defined as a sequence of
 *	 alphanumeric characters, delimited by non-alphanumeric
 *	 characters.
 *
 ********************************************************************/

Datum
initcap(MDB_FUNCTION_ARGS)
{
	text	   *in_string = MDB_GETARG_TEXT_PP(0);
	char	   *out_string;
	text	   *result;

	out_string = str_initcap(VARDATA_ANY(in_string),
							 VARSIZE_ANY_EXHDR(in_string),
							 MDB_GET_COLLATION());
	result = cstring_to_text(out_string);
	pfree(out_string);

	MDB_RETURN_TEXT_P(result);
}


/********************************************************************
 *
 * lpad
 *
 * Syntax:
 *
 *	 text lpad(text string1, int4 len, text string2)
 *
 * Purpose:
 *
 *	 Returns string1, left-padded to length len with the sequence of
 *	 characters in string2.  If len is less than the length of string1,
 *	 instead truncate (on the right) to len.
 *
 ********************************************************************/

Datum
lpad(MDB_FUNCTION_ARGS)
{
	text	   *string1 = MDB_GETARG_TEXT_PP(0);
	int32		len = MDB_GETARG_INT32(1);
	text	   *string2 = MDB_GETARG_TEXT_PP(2);
	text	   *ret;
	char	   *ptr1,
			   *ptr2,
			   *ptr2start,
			   *ptr2end,
			   *ptr_ret;
	int			m,
				s1len,
				s2len;

	int			bytelen;

	/* Negative len is silently taken as zero */
	if (len < 0)
		len = 0;

	s1len = VARSIZE_ANY_EXHDR(string1);
	if (s1len < 0)
		s1len = 0;				/* shouldn't happen */

	s2len = VARSIZE_ANY_EXHDR(string2);
	if (s2len < 0)
		s2len = 0;				/* shouldn't happen */

	s1len = mdb_mbstrlen_with_len(VARDATA_ANY(string1), s1len);

	if (s1len > len)
		s1len = len;			/* truncate string1 to len chars */

	if (s2len <= 0)
		len = s1len;			/* nothing to pad with, so don't pad */

	bytelen = mdb_database_encoding_max_length() * len;

	/* check for integer overflow */
	if (len != 0 && bytelen / mdb_database_encoding_max_length() != len)
		ereport(ERROR,
				(errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
				 errmsg("requested length too large")));

	ret = (text *) palloc(VARHDRSZ + bytelen);

	m = len - s1len;

	ptr2 = ptr2start = VARDATA_ANY(string2);
	ptr2end = ptr2 + s2len;
	ptr_ret = VARDATA(ret);

	while (m--)
	{
		int			mlen = mdb_mblen(ptr2);

		memcpy(ptr_ret, ptr2, mlen);
		ptr_ret += mlen;
		ptr2 += mlen;
		if (ptr2 == ptr2end)	/* wrap around at end of s2 */
			ptr2 = ptr2start;
	}

	ptr1 = VARDATA_ANY(string1);

	while (s1len--)
	{
		int			mlen = mdb_mblen(ptr1);

		memcpy(ptr_ret, ptr1, mlen);
		ptr_ret += mlen;
		ptr1 += mlen;
	}

	SET_VARSIZE(ret, ptr_ret - (char *) ret);

	MDB_RETURN_TEXT_P(ret);
}


/********************************************************************
 *
 * rpad
 *
 * Syntax:
 *
 *	 text rpad(text string1, int4 len, text string2)
 *
 * Purpose:
 *
 *	 Returns string1, right-padded to length len with the sequence of
 *	 characters in string2.  If len is less than the length of string1,
 *	 instead truncate (on the right) to len.
 *
 ********************************************************************/

Datum
rpad(MDB_FUNCTION_ARGS)
{
	text	   *string1 = MDB_GETARG_TEXT_PP(0);
	int32		len = MDB_GETARG_INT32(1);
	text	   *string2 = MDB_GETARG_TEXT_PP(2);
	text	   *ret;
	char	   *ptr1,
			   *ptr2,
			   *ptr2start,
			   *ptr2end,
			   *ptr_ret;
	int			m,
				s1len,
				s2len;

	int			bytelen;

	/* Negative len is silently taken as zero */
	if (len < 0)
		len = 0;

	s1len = VARSIZE_ANY_EXHDR(string1);
	if (s1len < 0)
		s1len = 0;				/* shouldn't happen */

	s2len = VARSIZE_ANY_EXHDR(string2);
	if (s2len < 0)
		s2len = 0;				/* shouldn't happen */

	s1len = mdb_mbstrlen_with_len(VARDATA_ANY(string1), s1len);

	if (s1len > len)
		s1len = len;			/* truncate string1 to len chars */

	if (s2len <= 0)
		len = s1len;			/* nothing to pad with, so don't pad */

	bytelen = mdb_database_encoding_max_length() * len;

	/* Check for integer overflow */
	if (len != 0 && bytelen / mdb_database_encoding_max_length() != len)
		ereport(ERROR,
				(errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
				 errmsg("requested length too large")));

	ret = (text *) palloc(VARHDRSZ + bytelen);
	m = len - s1len;

	ptr1 = VARDATA_ANY(string1);
	ptr_ret = VARDATA(ret);

	while (s1len--)
	{
		int			mlen = mdb_mblen(ptr1);

		memcpy(ptr_ret, ptr1, mlen);
		ptr_ret += mlen;
		ptr1 += mlen;
	}

	ptr2 = ptr2start = VARDATA_ANY(string2);
	ptr2end = ptr2 + s2len;

	while (m--)
	{
		int			mlen = mdb_mblen(ptr2);

		memcpy(ptr_ret, ptr2, mlen);
		ptr_ret += mlen;
		ptr2 += mlen;
		if (ptr2 == ptr2end)	/* wrap around at end of s2 */
			ptr2 = ptr2start;
	}

	SET_VARSIZE(ret, ptr_ret - (char *) ret);

	MDB_RETURN_TEXT_P(ret);
}


/********************************************************************
 *
 * btrim
 *
 * Syntax:
 *
 *	 text btrim(text string, text set)
 *
 * Purpose:
 *
 *	 Returns string with characters removed from the front and back
 *	 up to the first character not in set.
 *
 ********************************************************************/

Datum
btrim(MDB_FUNCTION_ARGS)
{
	text	   *string = MDB_GETARG_TEXT_PP(0);
	text	   *set = MDB_GETARG_TEXT_PP(1);
	text	   *ret;

	ret = dotrim(VARDATA_ANY(string), VARSIZE_ANY_EXHDR(string),
				 VARDATA_ANY(set), VARSIZE_ANY_EXHDR(set),
				 true, true);

	MDB_RETURN_TEXT_P(ret);
}

/********************************************************************
 *
 * btrim1 --- btrim with set fixed as ' '
 *
 ********************************************************************/

Datum
btrim1(MDB_FUNCTION_ARGS)
{
	text	   *string = MDB_GETARG_TEXT_PP(0);
	text	   *ret;

	ret = dotrim(VARDATA_ANY(string), VARSIZE_ANY_EXHDR(string),
				 " ", 1,
				 true, true);

	MDB_RETURN_TEXT_P(ret);
}

/*
 * Common implementation for btrim, ltrim, rtrim
 */
static text *
dotrim(const char *string, int stringlen,
	   const char *set, int setlen,
	   bool doltrim, bool dortrim)
{
	int			i;

	/* Nothing to do if either string or set is empty */
	if (stringlen > 0 && setlen > 0)
	{
		if (mdb_database_encoding_max_length() > 1)
		{
			/*
			 * In the multibyte-encoding case, build arrays of pointers to
			 * character starts, so that we can avoid inefficient checks in
			 * the inner loops.
			 */
			const char **stringchars;
			const char **setchars;
			int		   *stringmblen;
			int		   *setmblen;
			int			stringnchars;
			int			setnchars;
			int			resultndx;
			int			resultnchars;
			const char *p;
			int			len;
			int			mblen;
			const char *str_pos;
			int			str_len;

			stringchars = (const char **) palloc(stringlen * sizeof(char *));
			stringmblen = (int *) palloc(stringlen * sizeof(int));
			stringnchars = 0;
			p = string;
			len = stringlen;
			while (len > 0)
			{
				stringchars[stringnchars] = p;
				stringmblen[stringnchars] = mblen = mdb_mblen(p);
				stringnchars++;
				p += mblen;
				len -= mblen;
			}

			setchars = (const char **) palloc(setlen * sizeof(char *));
			setmblen = (int *) palloc(setlen * sizeof(int));
			setnchars = 0;
			p = set;
			len = setlen;
			while (len > 0)
			{
				setchars[setnchars] = p;
				setmblen[setnchars] = mblen = mdb_mblen(p);
				setnchars++;
				p += mblen;
				len -= mblen;
			}

			resultndx = 0;		/* index in stringchars[] */
			resultnchars = stringnchars;

			if (doltrim)
			{
				while (resultnchars > 0)
				{
					str_pos = stringchars[resultndx];
					str_len = stringmblen[resultndx];
					for (i = 0; i < setnchars; i++)
					{
						if (str_len == setmblen[i] &&
							memcmp(str_pos, setchars[i], str_len) == 0)
							break;
					}
					if (i >= setnchars)
						break;	/* no match here */
					string += str_len;
					stringlen -= str_len;
					resultndx++;
					resultnchars--;
				}
			}

			if (dortrim)
			{
				while (resultnchars > 0)
				{
					str_pos = stringchars[resultndx + resultnchars - 1];
					str_len = stringmblen[resultndx + resultnchars - 1];
					for (i = 0; i < setnchars; i++)
					{
						if (str_len == setmblen[i] &&
							memcmp(str_pos, setchars[i], str_len) == 0)
							break;
					}
					if (i >= setnchars)
						break;	/* no match here */
					stringlen -= str_len;
					resultnchars--;
				}
			}

			pfree(stringchars);
			pfree(stringmblen);
			pfree(setchars);
			pfree(setmblen);
		}
		else
		{
			/*
			 * In the single-byte-encoding case, we don't need such overhead.
			 */
			if (doltrim)
			{
				while (stringlen > 0)
				{
					char		str_ch = *string;

					for (i = 0; i < setlen; i++)
					{
						if (str_ch == set[i])
							break;
					}
					if (i >= setlen)
						break;	/* no match here */
					string++;
					stringlen--;
				}
			}

			if (dortrim)
			{
				while (stringlen > 0)
				{
					char		str_ch = string[stringlen - 1];

					for (i = 0; i < setlen; i++)
					{
						if (str_ch == set[i])
							break;
					}
					if (i >= setlen)
						break;	/* no match here */
					stringlen--;
				}
			}
		}
	}

	/* Return selected portion of string */
	return cstring_to_text_with_len(string, stringlen);
}

/********************************************************************
 *
 * byteatrim
 *
 * Syntax:
 *
 *	 bytea byteatrim(byta string, bytea set)
 *
 * Purpose:
 *
 *	 Returns string with characters removed from the front and back
 *	 up to the first character not in set.
 *
 * Cloned from btrim and modified as required.
 ********************************************************************/

Datum
byteatrim(MDB_FUNCTION_ARGS)
{
	bytea	   *string = MDB_GETARG_BYTEA_PP(0);
	bytea	   *set = MDB_GETARG_BYTEA_PP(1);
	bytea	   *ret;
	char	   *ptr,
			   *end,
			   *ptr2,
			   *ptr2start,
			   *end2;
	int			m,
				stringlen,
				setlen;

	stringlen = VARSIZE_ANY_EXHDR(string);
	setlen = VARSIZE_ANY_EXHDR(set);

	if (stringlen <= 0 || setlen <= 0)
		MDB_RETURN_BYTEA_P(string);

	m = stringlen;
	ptr = VARDATA_ANY(string);
	end = ptr + stringlen - 1;
	ptr2start = VARDATA_ANY(set);
	end2 = ptr2start + setlen - 1;

	while (m > 0)
	{
		ptr2 = ptr2start;
		while (ptr2 <= end2)
		{
			if (*ptr == *ptr2)
				break;
			++ptr2;
		}
		if (ptr2 > end2)
			break;
		ptr++;
		m--;
	}

	while (m > 0)
	{
		ptr2 = ptr2start;
		while (ptr2 <= end2)
		{
			if (*end == *ptr2)
				break;
			++ptr2;
		}
		if (ptr2 > end2)
			break;
		end--;
		m--;
	}

	ret = (bytea *) palloc(VARHDRSZ + m);
	SET_VARSIZE(ret, VARHDRSZ + m);
	memcpy(VARDATA(ret), ptr, m);

	MDB_RETURN_BYTEA_P(ret);
}

/********************************************************************
 *
 * ltrim
 *
 * Syntax:
 *
 *	 text ltrim(text string, text set)
 *
 * Purpose:
 *
 *	 Returns string with initial characters removed up to the first
 *	 character not in set.
 *
 ********************************************************************/

Datum
ltrim(MDB_FUNCTION_ARGS)
{
	text	   *string = MDB_GETARG_TEXT_PP(0);
	text	   *set = MDB_GETARG_TEXT_PP(1);
	text	   *ret;

	ret = dotrim(VARDATA_ANY(string), VARSIZE_ANY_EXHDR(string),
				 VARDATA_ANY(set), VARSIZE_ANY_EXHDR(set),
				 true, false);

	MDB_RETURN_TEXT_P(ret);
}

/********************************************************************
 *
 * ltrim1 --- ltrim with set fixed as ' '
 *
 ********************************************************************/

Datum
ltrim1(MDB_FUNCTION_ARGS)
{
	text	   *string = MDB_GETARG_TEXT_PP(0);
	text	   *ret;

	ret = dotrim(VARDATA_ANY(string), VARSIZE_ANY_EXHDR(string),
				 " ", 1,
				 true, false);

	MDB_RETURN_TEXT_P(ret);
}

/********************************************************************
 *
 * rtrim
 *
 * Syntax:
 *
 *	 text rtrim(text string, text set)
 *
 * Purpose:
 *
 *	 Returns string with final characters removed after the last
 *	 character not in set.
 *
 ********************************************************************/

Datum
rtrim(MDB_FUNCTION_ARGS)
{
	text	   *string = MDB_GETARG_TEXT_PP(0);
	text	   *set = MDB_GETARG_TEXT_PP(1);
	text	   *ret;

	ret = dotrim(VARDATA_ANY(string), VARSIZE_ANY_EXHDR(string),
				 VARDATA_ANY(set), VARSIZE_ANY_EXHDR(set),
				 false, true);

	MDB_RETURN_TEXT_P(ret);
}

/********************************************************************
 *
 * rtrim1 --- rtrim with set fixed as ' '
 *
 ********************************************************************/

Datum
rtrim1(MDB_FUNCTION_ARGS)
{
	text	   *string = MDB_GETARG_TEXT_PP(0);
	text	   *ret;

	ret = dotrim(VARDATA_ANY(string), VARSIZE_ANY_EXHDR(string),
				 " ", 1,
				 false, true);

	MDB_RETURN_TEXT_P(ret);
}


/********************************************************************
 *
 * translate
 *
 * Syntax:
 *
 *	 text translate(text string, text from, text to)
 *
 * Purpose:
 *
 *	 Returns string after replacing all occurrences of characters in from
 *	 with the corresponding character in to.  If from is longer than to,
 *	 occurrences of the extra characters in from are deleted.
 *	 Improved by Edwin Ramirez <ramirez@doc.mssm.edu>.
 *
 ********************************************************************/

Datum
translate(MDB_FUNCTION_ARGS)
{
	text	   *string = MDB_GETARG_TEXT_PP(0);
	text	   *from = MDB_GETARG_TEXT_PP(1);
	text	   *to = MDB_GETARG_TEXT_PP(2);
	text	   *result;
	char	   *from_ptr,
			   *to_ptr;
	char	   *source,
			   *target;
	int			m,
				fromlen,
				tolen,
				retlen,
				i;
	int			worst_len;
	int			len;
	int			source_len;
	int			from_index;

	m = VARSIZE_ANY_EXHDR(string);
	if (m <= 0)
		MDB_RETURN_TEXT_P(string);
	source = VARDATA_ANY(string);

	fromlen = VARSIZE_ANY_EXHDR(from);
	from_ptr = VARDATA_ANY(from);
	tolen = VARSIZE_ANY_EXHDR(to);
	to_ptr = VARDATA_ANY(to);

	/*
	 * The worst-case expansion is to substitute a max-length character for a
	 * single-byte character at each position of the string.
	 */
	worst_len = mdb_database_encoding_max_length() * m;

	/* check for integer overflow */
	if (worst_len / mdb_database_encoding_max_length() != m)
		ereport(ERROR,
				(errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
				 errmsg("requested length too large")));

	result = (text *) palloc(worst_len + VARHDRSZ);
	target = VARDATA(result);
	retlen = 0;

	while (m > 0)
	{
		source_len = mdb_mblen(source);
		from_index = 0;

		for (i = 0; i < fromlen; i += len)
		{
			len = mdb_mblen(&from_ptr[i]);
			if (len == source_len &&
				memcmp(source, &from_ptr[i], len) == 0)
				break;

			from_index++;
		}
		if (i < fromlen)
		{
			/* substitute */
			char	   *p = to_ptr;

			for (i = 0; i < from_index; i++)
			{
				p += mdb_mblen(p);
				if (p >= (to_ptr + tolen))
					break;
			}
			if (p < (to_ptr + tolen))
			{
				len = mdb_mblen(p);
				memcpy(target, p, len);
				target += len;
				retlen += len;
			}

		}
		else
		{
			/* no match, so copy */
			memcpy(target, source, source_len);
			target += source_len;
			retlen += source_len;
		}

		source += source_len;
		m -= source_len;
	}

	SET_VARSIZE(result, retlen + VARHDRSZ);

	/*
	 * The function result is probably much bigger than needed, if we're using
	 * a multibyte encoding, but it's not worth reallocating it; the result
	 * probably won't live long anyway.
	 */

	MDB_RETURN_TEXT_P(result);
}

/********************************************************************
 *
 * ascii
 *
 * Syntax:
 *
 *	 int ascii(text string)
 *
 * Purpose:
 *
 *	 Returns the decimal representation of the first character from
 *	 string.
 *	 If the string is empty we return 0.
 *	 If the database encoding is UTF8, we return the Unicode codepoint.
 *	 If the database encoding is any other multi-byte encoding, we
 *	 return the value of the first byte if it is an ASCII character
 *	 (range 1 .. 127), or raise an error.
 *	 For all other encodings we return the value of the first byte,
 *	 (range 1..255).
 *
 ********************************************************************/

Datum
ascii(MDB_FUNCTION_ARGS)
{
	text	   *string = MDB_GETARG_TEXT_PP(0);
	int			encoding = GetDatabaseEncoding();
	unsigned char *data;

	if (VARSIZE_ANY_EXHDR(string) <= 0)
		MDB_RETURN_INT32(0);

	data = (unsigned char *) VARDATA_ANY(string);

	if (encoding == MDB_UTF8 && *data > 127)
	{
		/* return the code point for Unicode */

		int			result = 0,
					tbytes = 0,
					i;

		if (*data >= 0xF0)
		{
			result = *data & 0x07;
			tbytes = 3;
		}
		else if (*data >= 0xE0)
		{
			result = *data & 0x0F;
			tbytes = 2;
		}
		else
		{
			Assert(*data > 0xC0);
			result = *data & 0x1f;
			tbytes = 1;
		}

		Assert(tbytes > 0);

		for (i = 1; i <= tbytes; i++)
		{
			Assert((data[i] & 0xC0) == 0x80);
			result = (result << 6) + (data[i] & 0x3f);
		}

		MDB_RETURN_INT32(result);
	}
	else
	{
		if (mdb_encoding_max_length(encoding) > 1 && *data > 127)
			ereport(ERROR,
					(errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
					 errmsg("requested character too large")));


		MDB_RETURN_INT32((int32) *data);
	}
}

/********************************************************************
 *
 * chr
 *
 * Syntax:
 *
 *	 text chr(int val)
 *
 * Purpose:
 *
 *	Returns the character having the binary equivalent to val.
 *
 * For UTF8 we treat the argumwent as a Unicode code point.
 * For other multi-byte encodings we raise an error for arguments
 * outside the strict ASCII range (1..127).
 *
 * It's important that we don't ever return a value that is not valid
 * in the database encoding, so that this doesn't become a way for
 * invalid data to enter the database.
 *
 ********************************************************************/

Datum
chr			(MDB_FUNCTION_ARGS)
{
	uint32		cvalue = MDB_GETARG_UINT32(0);
	text	   *result;
	int			encoding = GetDatabaseEncoding();

	if (encoding == MDB_UTF8 && cvalue > 127)
	{
		/* for Unicode we treat the argument as a code point */
		int			bytes;
		unsigned char *wch;

		/*
		 * We only allow valid Unicode code points; per RFC3629 that stops at
		 * U+10FFFF, even though 4-byte UTF8 sequences can hold values up to
		 * U+1FFFFF.
		 */
		if (cvalue > 0x0010ffff)
			ereport(ERROR,
					(errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
					 errmsg("requested character too large for encoding: %d",
							cvalue)));

		if (cvalue > 0xffff)
			bytes = 4;
		else if (cvalue > 0x07ff)
			bytes = 3;
		else
			bytes = 2;

		result = (text *) palloc(VARHDRSZ + bytes);
		SET_VARSIZE(result, VARHDRSZ + bytes);
		wch = (unsigned char *) VARDATA(result);

		if (bytes == 2)
		{
			wch[0] = 0xC0 | ((cvalue >> 6) & 0x1F);
			wch[1] = 0x80 | (cvalue & 0x3F);
		}
		else if (bytes == 3)
		{
			wch[0] = 0xE0 | ((cvalue >> 12) & 0x0F);
			wch[1] = 0x80 | ((cvalue >> 6) & 0x3F);
			wch[2] = 0x80 | (cvalue & 0x3F);
		}
		else
		{
			wch[0] = 0xF0 | ((cvalue >> 18) & 0x07);
			wch[1] = 0x80 | ((cvalue >> 12) & 0x3F);
			wch[2] = 0x80 | ((cvalue >> 6) & 0x3F);
			wch[3] = 0x80 | (cvalue & 0x3F);
		}

		/*
		 * The preceding range check isn't sufficient, because UTF8 excludes
		 * Unicode "surrogate pair" codes.  Make sure what we created is valid
		 * UTF8.
		 */
		if (!mdb_utf8_islegal(wch, bytes))
			ereport(ERROR,
					(errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
					 errmsg("requested character not valid for encoding: %d",
							cvalue)));
	}
	else
	{
		bool		is_mb;

		/*
		 * Error out on arguments that make no sense or that we can't validly
		 * represent in the encoding.
		 */
		if (cvalue == 0)
			ereport(ERROR,
					(errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
					 errmsg("null character not permitted")));

		is_mb = mdb_encoding_max_length(encoding) > 1;

		if ((is_mb && (cvalue > 127)) || (!is_mb && (cvalue > 255)))
			ereport(ERROR,
					(errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
					 errmsg("requested character too large for encoding: %d",
							cvalue)));

		result = (text *) palloc(VARHDRSZ + 1);
		SET_VARSIZE(result, VARHDRSZ + 1);
		*VARDATA(result) = (char) cvalue;
	}

	MDB_RETURN_TEXT_P(result);
}

/********************************************************************
 *
 * repeat
 *
 * Syntax:
 *
 *	 text repeat(text string, int val)
 *
 * Purpose:
 *
 *	Repeat string by val.
 *
 ********************************************************************/

Datum
repeat(MDB_FUNCTION_ARGS)
{
	text	   *string = MDB_GETARG_TEXT_PP(0);
	int32		count = MDB_GETARG_INT32(1);
	text	   *result;
	int			slen,
				tlen;
	int			i;
	char	   *cp,
			   *sp;

	if (count < 0)
		count = 0;

	slen = VARSIZE_ANY_EXHDR(string);
	tlen = VARHDRSZ + (count * slen);

	/* Check for integer overflow */
	if (slen != 0 && count != 0)
	{
		int			check = count * slen;
		int			check2 = check + VARHDRSZ;

		if ((check / slen) != count || check2 <= check)
			ereport(ERROR,
					(errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
					 errmsg("requested length too large")));
	}

	result = (text *) palloc(tlen);

	SET_VARSIZE(result, tlen);
	cp = VARDATA(result);
	sp = VARDATA_ANY(string);
	for (i = 0; i < count; i++)
	{
		memcpy(cp, sp, slen);
		cp += slen;
	}

	MDB_RETURN_TEXT_P(result);
}
