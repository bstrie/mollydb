/* contrib/mdb_trgm/mdb_trgm--1.0--1.1.sql */

-- complain if script is sourced in psql, rather than via ALTER EXTENSION
\echo Use "ALTER EXTENSION mdb_trgm UPDATE TO '1.1'" to load this file. \quit

ALTER OPERATOR FAMILY gist_trgm_ops USING gist ADD
        OPERATOR        5       mdb_catalog.~ (text, text),
        OPERATOR        6       mdb_catalog.~* (text, text);

ALTER OPERATOR FAMILY gin_trgm_ops USING gin ADD
        OPERATOR        5       mdb_catalog.~ (text, text),
        OPERATOR        6       mdb_catalog.~* (text, text);
