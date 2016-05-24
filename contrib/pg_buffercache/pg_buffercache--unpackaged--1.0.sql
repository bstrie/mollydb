/* contrib/mdb_buffercache/mdb_buffercache--unpackaged--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION mdb_buffercache FROM unpackaged" to load this file. \quit

ALTER EXTENSION mdb_buffercache ADD function mdb_buffercache_pages();
ALTER EXTENSION mdb_buffercache ADD view mdb_buffercache;
