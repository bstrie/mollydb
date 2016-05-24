-- predictability
SET synchronous_commit = on;

CREATE TABLE origin_tbl(id serial primary key, data text);
CREATE TABLE target_tbl(id serial primary key, data text);

SELECT mdb_replication_origin_create('test_decoding: regression_slot');
-- ensure duplicate creations fail
SELECT mdb_replication_origin_create('test_decoding: regression_slot');

--ensure deletions work (once)
SELECT mdb_replication_origin_create('test_decoding: temp');
SELECT mdb_replication_origin_drop('test_decoding: temp');
SELECT mdb_replication_origin_drop('test_decoding: temp');

SELECT 'init' FROM mdb_create_logical_replication_slot('regression_slot', 'test_decoding');

-- origin tx
INSERT INTO origin_tbl(data) VALUES ('will be replicated and decoded and decoded again');
INSERT INTO target_tbl(data)
SELECT data FROM mdb_logical_slot_get_changes('regression_slot', NULL, NULL, 'include-xids', '0', 'skip-empty-xacts', '1');

-- as is normal, the insert into target_tbl shows up
SELECT data FROM mdb_logical_slot_get_changes('regression_slot', NULL, NULL, 'include-xids', '0', 'skip-empty-xacts', '1');

INSERT INTO origin_tbl(data) VALUES ('will be replicated, but not decoded again');

-- mark session as replaying
SELECT mdb_replication_origin_session_setup('test_decoding: regression_slot');

-- ensure we prevent duplicate setup
SELECT mdb_replication_origin_session_setup('test_decoding: regression_slot');

SELECT '' FROM mdb_logical_emit_message(false, 'test', 'this message will not be decoded');

BEGIN;
-- setup transaction origin
SELECT mdb_replication_origin_xact_setup('0/aabbccdd', '2013-01-01 00:00');
INSERT INTO target_tbl(data)
SELECT data FROM mdb_logical_slot_get_changes('regression_slot', NULL, NULL, 'include-xids', '0', 'skip-empty-xacts', '1', 'only-local', '1');
COMMIT;

-- check replication progress for the session is correct
SELECT mdb_replication_origin_session_progress(false);
SELECT mdb_replication_origin_session_progress(true);

SELECT mdb_replication_origin_session_reset();

SELECT local_id, external_id, remote_lsn, local_lsn <> '0/0' FROM mdb_replication_origin_status;

-- check replication progress identified by name is correct
SELECT mdb_replication_origin_progress('test_decoding: regression_slot', false);
SELECT mdb_replication_origin_progress('test_decoding: regression_slot', true);

-- ensure reset requires previously setup state
SELECT mdb_replication_origin_session_reset();

-- and magically the replayed xact will be filtered!
SELECT data FROM mdb_logical_slot_get_changes('regression_slot', NULL, NULL, 'include-xids', '0', 'skip-empty-xacts', '1', 'only-local', '1');

--but new original changes still show up
INSERT INTO origin_tbl(data) VALUES ('will be replicated');
SELECT data FROM mdb_logical_slot_get_changes('regression_slot', NULL, NULL, 'include-xids', '0', 'skip-empty-xacts', '1',  'only-local', '1');

SELECT mdb_drop_replication_slot('regression_slot');
SELECT mdb_replication_origin_drop('test_decoding: regression_slot');
