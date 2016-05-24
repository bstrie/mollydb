/* src/test/modules/test_shm_mq/test_shm_mq--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION test_shm_mq" to load this file. \quit

CREATE FUNCTION test_shm_mq(queue_size mdb_catalog.int8,
					   message mdb_catalog.text,
					   repeat_count mdb_catalog.int4 default 1,
					   num_workers mdb_catalog.int4 default 1)
    RETURNS mdb_catalog.void STRICT
	AS 'MODULE_PATHNAME' LANGUAGE C;

CREATE FUNCTION test_shm_mq_pipelined(queue_size mdb_catalog.int8,
					   message mdb_catalog.text,
					   repeat_count mdb_catalog.int4 default 1,
					   num_workers mdb_catalog.int4 default 1,
					   verify mdb_catalog.bool default true)
    RETURNS mdb_catalog.void STRICT
	AS 'MODULE_PATHNAME' LANGUAGE C;
