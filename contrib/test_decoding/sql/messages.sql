-- predictability
SET synchronous_commit = on;

SELECT 'init' FROM mdb_create_logical_replication_slot('regression_slot', 'test_decoding');

SELECT 'msg1' FROM mdb_logical_emit_message(true, 'test', 'msg1');
SELECT 'msg2' FROM mdb_logical_emit_message(false, 'test', 'msg2');

BEGIN;
SELECT 'msg3' FROM mdb_logical_emit_message(true, 'test', 'msg3');
SELECT 'msg4' FROM mdb_logical_emit_message(false, 'test', 'msg4');
ROLLBACK;

BEGIN;
SELECT 'msg5' FROM mdb_logical_emit_message(true, 'test', 'msg5');
SELECT 'msg6' FROM mdb_logical_emit_message(false, 'test', 'msg6');
SELECT 'msg7' FROM mdb_logical_emit_message(true, 'test', 'msg7');
COMMIT;

SELECT 'ignorethis' FROM mdb_logical_emit_message(true, 'test', 'czechtastic');

SELECT data FROM mdb_logical_slot_get_changes('regression_slot', NULL, NULL, 'force-binary', '0', 'skip-empty-xacts', '1');

-- test db filtering
\set prevdb :DBNAME
\c template1

SELECT 'otherdb1' FROM mdb_logical_emit_message(false, 'test', 'otherdb1');
SELECT 'otherdb2' FROM mdb_logical_emit_message(true, 'test', 'otherdb2');

\c :prevdb
SELECT data FROM mdb_logical_slot_get_changes('regression_slot', NULL, NULL, 'force-binary', '0', 'skip-empty-xacts', '1');

SELECT 'cleanup' FROM mdb_drop_replication_slot('regression_slot');
