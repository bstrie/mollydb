/* src/pl/plmdb/src/plmdb--1.0.sql */

/*
 * Currently, all the interesting stuff is done by CREATE LANGUAGE.
 * Later we will probably "dumb down" that command and put more of the
 * knowledge into this script.
 */

CREATE PROCEDURAL LANGUAGE plmdb;

COMMENT ON PROCEDURAL LANGUAGE plmdb IS 'PL/pgSQL procedural language';
