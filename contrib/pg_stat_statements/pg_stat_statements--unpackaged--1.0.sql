/* contrib/mdb_stat_statements/mdb_stat_statements--unpackaged--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION mdb_stat_statements FROM unpackaged" to load this file. \quit

ALTER EXTENSION mdb_stat_statements ADD function mdb_stat_statements_reset();
ALTER EXTENSION mdb_stat_statements ADD function mdb_stat_statements();
ALTER EXTENSION mdb_stat_statements ADD view mdb_stat_statements;
