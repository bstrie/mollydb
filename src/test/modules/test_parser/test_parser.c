/*-------------------------------------------------------------------------
 *
 * test_parser.c
 *	  Simple example of a text search parser
 *
 * Copyright (c) 2007-2016, MollyDB Global Development Group
 *
 * IDENTIFICATION
 *	  src/test/modules/test_parser/test_parser.c
 *
 *-------------------------------------------------------------------------
 */
#include "mollydb.h"

#include "fmgr.h"

MDB_MODULE_MAGIC;

/*
 * types
 */

/* self-defined type */
typedef struct
{
	char	   *buffer;			/* text to parse */
	int			len;			/* length of the text in buffer */
	int			pos;			/* position of the parser */
} ParserState;

/* copy-paste from wparser.h of tsearch2 */
typedef struct
{
	int			lexid;
	char	   *alias;
	char	   *descr;
} LexDescr;

/*
 * functions
 */
MDB_FUNCTION_INFO_V1(testprs_start);
MDB_FUNCTION_INFO_V1(testprs_getlexeme);
MDB_FUNCTION_INFO_V1(testprs_end);
MDB_FUNCTION_INFO_V1(testprs_lextype);

Datum
testprs_start(MDB_FUNCTION_ARGS)
{
	ParserState *pst = (ParserState *) palloc0(sizeof(ParserState));

	pst->buffer = (char *) MDB_GETARG_POINTER(0);
	pst->len = MDB_GETARG_INT32(1);
	pst->pos = 0;

	MDB_RETURN_POINTER(pst);
}

Datum
testprs_getlexeme(MDB_FUNCTION_ARGS)
{
	ParserState *pst = (ParserState *) MDB_GETARG_POINTER(0);
	char	  **t = (char **) MDB_GETARG_POINTER(1);
	int		   *tlen = (int *) MDB_GETARG_POINTER(2);
	int			startpos = pst->pos;
	int			type;

	*t = pst->buffer + pst->pos;

	if (pst->pos < pst->len &&
		(pst->buffer)[pst->pos] == ' ')
	{
		/* blank type */
		type = 12;
		/* go to the next non-space character */
		while (pst->pos < pst->len &&
			   (pst->buffer)[pst->pos] == ' ')
			(pst->pos)++;
	}
	else
	{
		/* word type */
		type = 3;
		/* go to the next space character */
		while (pst->pos < pst->len &&
			   (pst->buffer)[pst->pos] != ' ')
			(pst->pos)++;
	}

	*tlen = pst->pos - startpos;

	/* we are finished if (*tlen == 0) */
	if (*tlen == 0)
		type = 0;

	MDB_RETURN_INT32(type);
}

Datum
testprs_end(MDB_FUNCTION_ARGS)
{
	ParserState *pst = (ParserState *) MDB_GETARG_POINTER(0);

	pfree(pst);
	MDB_RETURN_VOID();
}

Datum
testprs_lextype(MDB_FUNCTION_ARGS)
{
	/*
	 * Remarks: - we have to return the blanks for headline reason - we use
	 * the same lexids like Teodor in the default word parser; in this way we
	 * can reuse the headline function of the default word parser.
	 */
	LexDescr   *descr = (LexDescr *) palloc(sizeof(LexDescr) * (2 + 1));

	/* there are only two types in this parser */
	descr[0].lexid = 3;
	descr[0].alias = pstrdup("word");
	descr[0].descr = pstrdup("Word");
	descr[1].lexid = 12;
	descr[1].alias = pstrdup("blank");
	descr[1].descr = pstrdup("Space symbols");
	descr[2].lexid = 0;

	MDB_RETURN_POINTER(descr);
}
