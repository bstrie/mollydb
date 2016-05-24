/* src/test/modules/test_ddl_deparse/test_ddl_deparse--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION test_ddl_deparse" to load this file. \quit

CREATE FUNCTION get_command_type(mdb_ddl_command)
  RETURNS text IMMUTABLE STRICT
  AS 'MODULE_PATHNAME' LANGUAGE C;

CREATE FUNCTION get_command_tag(mdb_ddl_command)
  RETURNS text IMMUTABLE STRICT
  AS 'MODULE_PATHNAME' LANGUAGE C;

CREATE FUNCTION get_altertable_subcmdtypes(mdb_ddl_command)
  RETURNS text[] IMMUTABLE STRICT
  AS 'MODULE_PATHNAME' LANGUAGE C;
