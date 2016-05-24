/* contrib/mdb_freespacemap/mdb_freespacemap--unpackaged--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION mdb_freespacemap FROM unpackaged" to load this file. \quit

ALTER EXTENSION mdb_freespacemap ADD function mdb_freespace(regclass,bigint);
ALTER EXTENSION mdb_freespacemap ADD function mdb_freespace(regclass);
