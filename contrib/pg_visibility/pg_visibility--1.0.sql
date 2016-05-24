/* contrib/mdb_visibility/mdb_visibility--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION mdb_visibility" to load this file. \quit

-- Show visibility map information.
CREATE FUNCTION mdb_visibility_map(regclass, blkno bigint,
								  all_visible OUT boolean,
								  all_frozen OUT boolean)
RETURNS record
AS 'MODULE_PATHNAME', 'mdb_visibility_map'
LANGUAGE C STRICT;

-- Show visibility map and page-level visibility information.
CREATE FUNCTION mdb_visibility(regclass, blkno bigint,
							  all_visible OUT boolean,
							  all_frozen OUT boolean,
							  pd_all_visible OUT boolean)
RETURNS record
AS 'MODULE_PATHNAME', 'mdb_visibility'
LANGUAGE C STRICT;

-- Show visibility map information for each block in a relation.
CREATE FUNCTION mdb_visibility_map(regclass, blkno OUT bigint,
								  all_visible OUT boolean,
								  all_frozen OUT boolean)
RETURNS SETOF record
AS 'MODULE_PATHNAME', 'mdb_visibility_map_rel'
LANGUAGE C STRICT;

-- Show visibility map and page-level visibility information for each block.
CREATE FUNCTION mdb_visibility(regclass, blkno OUT bigint,
							  all_visible OUT boolean,
							  all_frozen OUT boolean,
							  pd_all_visible OUT boolean)
RETURNS SETOF record
AS 'MODULE_PATHNAME', 'mdb_visibility_rel'
LANGUAGE C STRICT;

-- Show summary of visibility map bits for a relation.
CREATE FUNCTION mdb_visibility_map_summary(regclass,
    OUT all_visible bigint, OUT all_frozen bigint)
RETURNS record
AS 'MODULE_PATHNAME', 'mdb_visibility_map_summary'
LANGUAGE C STRICT;

-- Don't want these to be available to public.
REVOKE ALL ON FUNCTION mdb_visibility_map(regclass, bigint) FROM PUBLIC;
REVOKE ALL ON FUNCTION mdb_visibility(regclass, bigint) FROM PUBLIC;
REVOKE ALL ON FUNCTION mdb_visibility_map(regclass) FROM PUBLIC;
REVOKE ALL ON FUNCTION mdb_visibility(regclass) FROM PUBLIC;
REVOKE ALL ON FUNCTION mdb_visibility_map_summary(regclass) FROM PUBLIC;
