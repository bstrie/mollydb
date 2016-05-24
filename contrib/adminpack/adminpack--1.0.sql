/* contrib/adminpack/adminpack--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION adminpack" to load this file. \quit

/* ***********************************************
 * Administrative functions for MollyDB
 * *********************************************** */

/* generic file access functions */

CREATE FUNCTION mdb_catalog.mdb_file_write(text, text, bool)
RETURNS bigint
AS 'MODULE_PATHNAME', 'mdb_file_write'
LANGUAGE C VOLATILE STRICT;

CREATE FUNCTION mdb_catalog.mdb_file_rename(text, text, text)
RETURNS bool
AS 'MODULE_PATHNAME', 'mdb_file_rename'
LANGUAGE C VOLATILE;

CREATE FUNCTION mdb_catalog.mdb_file_rename(text, text)
RETURNS bool
AS 'SELECT mdb_catalog.mdb_file_rename($1, $2, NULL::mdb_catalog.text);'
LANGUAGE SQL VOLATILE STRICT;

CREATE FUNCTION mdb_catalog.mdb_file_unlink(text)
RETURNS bool
AS 'MODULE_PATHNAME', 'mdb_file_unlink'
LANGUAGE C VOLATILE STRICT;

CREATE FUNCTION mdb_catalog.mdb_logdir_ls()
RETURNS setof record
AS 'MODULE_PATHNAME', 'mdb_logdir_ls'
LANGUAGE C VOLATILE STRICT;


/* Renaming of existing backend functions for pgAdmin compatibility */

CREATE FUNCTION mdb_catalog.mdb_file_read(text, bigint, bigint)
RETURNS text
AS 'mdb_read_file'
LANGUAGE INTERNAL VOLATILE STRICT;

CREATE FUNCTION mdb_catalog.mdb_file_length(text)
RETURNS bigint
AS 'SELECT size FROM mdb_catalog.mdb_stat_file($1)'
LANGUAGE SQL VOLATILE STRICT;

CREATE FUNCTION mdb_catalog.mdb_logfile_rotate()
RETURNS int4
AS 'mdb_rotate_logfile'
LANGUAGE INTERNAL VOLATILE STRICT;
