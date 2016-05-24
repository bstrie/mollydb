/* contrib/mdb_stat_statements/mdb_stat_statements--1.0--1.1.sql */

-- complain if script is sourced in psql, rather than via ALTER EXTENSION
\echo Use "ALTER EXTENSION mdb_stat_statements UPDATE TO '1.1'" to load this file. \quit

/* First we have to remove them from the extension */
ALTER EXTENSION mdb_stat_statements DROP VIEW mdb_stat_statements;
ALTER EXTENSION mdb_stat_statements DROP FUNCTION mdb_stat_statements();

/* Then we can drop them */
DROP VIEW mdb_stat_statements;
DROP FUNCTION mdb_stat_statements();

/* Now redefine */
CREATE FUNCTION mdb_stat_statements(
    OUT userid oid,
    OUT dbid oid,
    OUT query text,
    OUT calls int8,
    OUT total_time float8,
    OUT rows int8,
    OUT shared_blks_hit int8,
    OUT shared_blks_read int8,
    OUT shared_blks_dirtied int8,
    OUT shared_blks_written int8,
    OUT local_blks_hit int8,
    OUT local_blks_read int8,
    OUT local_blks_dirtied int8,
    OUT local_blks_written int8,
    OUT temp_blks_read int8,
    OUT temp_blks_written int8,
    OUT blk_read_time float8,
    OUT blk_write_time float8
)
RETURNS SETOF record
AS 'MODULE_PATHNAME'
LANGUAGE C;

CREATE VIEW mdb_stat_statements AS
  SELECT * FROM mdb_stat_statements();

GRANT SELECT ON mdb_stat_statements TO PUBLIC;
