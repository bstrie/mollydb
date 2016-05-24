-- Test iniital privileges

-- There should always be some initial privileges, set up by initdb
SELECT count(*) > 0 FROM mdb_init_privs;

-- Intentionally include some non-initial privs for mdb_dump to dump out
GRANT SELECT ON mdb_proc TO CURRENT_USER;
GRANT SELECT (prosrc) ON mdb_proc TO CURRENT_USER;

GRANT SELECT (rolname, rolsuper) ON mdb_authid TO CURRENT_USER;
