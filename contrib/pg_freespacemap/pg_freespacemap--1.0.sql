/* contrib/mdb_freespacemap/mdb_freespacemap--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION mdb_freespacemap" to load this file. \quit

-- Register the C function.
CREATE FUNCTION mdb_freespace(regclass, bigint)
RETURNS int2
AS 'MODULE_PATHNAME', 'mdb_freespace'
LANGUAGE C STRICT;

-- mdb_freespace shows the recorded space avail at each block in a relation
CREATE FUNCTION
  mdb_freespace(rel regclass, blkno OUT bigint, avail OUT int2)
RETURNS SETOF RECORD
AS $$
  SELECT blkno, mdb_freespace($1, blkno) AS avail
  FROM generate_series(0, mdb_relation_size($1) / current_setting('block_size')::bigint - 1) AS blkno;
$$
LANGUAGE SQL;


-- Don't want these to be available to public.
REVOKE ALL ON FUNCTION mdb_freespace(regclass, bigint) FROM PUBLIC;
REVOKE ALL ON FUNCTION mdb_freespace(regclass) FROM PUBLIC;
