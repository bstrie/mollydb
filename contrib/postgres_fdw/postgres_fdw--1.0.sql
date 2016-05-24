/* contrib/mollydb_fdw/mollydb_fdw--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION mollydb_fdw" to load this file. \quit

CREATE FUNCTION mollydb_fdw_handler()
RETURNS fdw_handler
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT;

CREATE FUNCTION mollydb_fdw_validator(text[], oid)
RETURNS void
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT;

CREATE FOREIGN DATA WRAPPER mollydb_fdw
  HANDLER mollydb_fdw_handler
  VALIDATOR mollydb_fdw_validator;
