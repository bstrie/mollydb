/* src/test/modules/test_mdb_dump/test_mdb_dump--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION test_mdb_dump" to load this file. \quit

CREATE TABLE regress_mdb_dump_table (
	col1 int,
	col2 int
);

GRANT SELECT ON regress_mdb_dump_table TO dump_test;
GRANT SELECT(col1) ON regress_mdb_dump_table TO public;

GRANT SELECT(col2) ON regress_mdb_dump_table TO dump_test;
REVOKE SELECT(col2) ON regress_mdb_dump_table FROM dump_test;
