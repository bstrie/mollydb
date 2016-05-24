/* contrib/mdb_prewarm/mdb_prewarm--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION mdb_prewarm" to load this file. \quit

-- Register the function.
CREATE FUNCTION mdb_prewarm(regclass,
						   mode text default 'buffer',
						   fork text default 'main',
						   first_block int8 default null,
						   last_block int8 default null)
RETURNS int8
AS 'MODULE_PATHNAME', 'mdb_prewarm'
LANGUAGE C;
