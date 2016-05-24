/* contrib/mdb_buffercache/mdb_buffercache--1.1.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION mdb_buffercache" to load this file. \quit

-- Register the function.
CREATE FUNCTION mdb_buffercache_pages()
RETURNS SETOF RECORD
AS 'MODULE_PATHNAME', 'mdb_buffercache_pages'
LANGUAGE C;

-- Create a view for convenient access.
CREATE VIEW mdb_buffercache AS
	SELECT P.* FROM mdb_buffercache_pages() AS P
	(bufferid integer, relfilenode oid, reltablespace oid, reldatabase oid,
	 relforknumber int2, relblocknumber int8, isdirty bool, usagecount int2,
	 pinning_backends int4);

-- Don't want these to be available to public.
REVOKE ALL ON FUNCTION mdb_buffercache_pages() FROM PUBLIC;
REVOKE ALL ON mdb_buffercache FROM PUBLIC;
