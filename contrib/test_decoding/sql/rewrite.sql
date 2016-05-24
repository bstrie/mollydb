-- predictability
SET synchronous_commit = on;

DROP TABLE IF EXISTS replication_example;

SELECT 'init' FROM mdb_create_logical_replication_slot('regression_slot', 'test_decoding');
CREATE TABLE replication_example(id SERIAL PRIMARY KEY, somedata int, text varchar(120));
INSERT INTO replication_example(somedata) VALUES (1);
SELECT data FROM mdb_logical_slot_get_changes('regression_slot', NULL, NULL, 'include-xids', '0', 'skip-empty-xacts', '1');

BEGIN;
INSERT INTO replication_example(somedata) VALUES (2);
ALTER TABLE replication_example ADD COLUMN testcolumn1 int;
INSERT INTO replication_example(somedata, testcolumn1) VALUES (3,  1);
COMMIT;

BEGIN;
INSERT INTO replication_example(somedata) VALUES (3);
ALTER TABLE replication_example ADD COLUMN testcolumn2 int;
INSERT INTO replication_example(somedata, testcolumn1, testcolumn2) VALUES (4,  2, 1);
COMMIT;

VACUUM FULL mdb_am;
VACUUM FULL mdb_amop;
VACUUM FULL mdb_proc;
VACUUM FULL mdb_opclass;
VACUUM FULL mdb_type;
VACUUM FULL mdb_index;
VACUUM FULL mdb_database;

-- repeated rewrites that fail
BEGIN;
CLUSTER mdb_class USING mdb_class_oid_index;
CLUSTER mdb_class USING mdb_class_oid_index;
ROLLBACK;

-- repeated rewrites that succeed
BEGIN;
CLUSTER mdb_class USING mdb_class_oid_index;
CLUSTER mdb_class USING mdb_class_oid_index;
CLUSTER mdb_class USING mdb_class_oid_index;
COMMIT;

 -- repeated rewrites in different transactions
VACUUM FULL mdb_class;
VACUUM FULL mdb_class;

INSERT INTO replication_example(somedata, testcolumn1) VALUES (5, 3);

BEGIN;
INSERT INTO replication_example(somedata, testcolumn1) VALUES (6, 4);
ALTER TABLE replication_example ADD COLUMN testcolumn3 int;
INSERT INTO replication_example(somedata, testcolumn1, testcolumn3) VALUES (7, 5, 1);
COMMIT;

-- make old files go away
CHECKPOINT;

SELECT data FROM mdb_logical_slot_get_changes('regression_slot', NULL, NULL, 'include-xids', '0', 'skip-empty-xacts', '1');
SELECT mdb_drop_replication_slot('regression_slot');

DROP TABLE IF EXISTS replication_example;
