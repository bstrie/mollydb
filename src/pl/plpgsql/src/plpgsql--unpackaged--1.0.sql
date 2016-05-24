/* src/pl/plmdb/src/plmdb--unpackaged--1.0.sql */

ALTER EXTENSION plmdb ADD PROCEDURAL LANGUAGE plmdb;
-- ALTER ADD LANGUAGE doesn't pick up the support functions, so we have to.
ALTER EXTENSION plmdb ADD FUNCTION plmdb_call_handler();
ALTER EXTENSION plmdb ADD FUNCTION plmdb_inline_handler(internal);
ALTER EXTENSION plmdb ADD FUNCTION plmdb_validator(oid);
