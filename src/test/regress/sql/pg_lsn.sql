--
-- MDB_LSN
--

CREATE TABLE MDB_LSN_TBL (f1 mdb_lsn);

-- Largest and smallest input
INSERT INTO MDB_LSN_TBL VALUES ('0/0');
INSERT INTO MDB_LSN_TBL VALUES ('FFFFFFFF/FFFFFFFF');

-- Incorrect input
INSERT INTO MDB_LSN_TBL VALUES ('G/0');
INSERT INTO MDB_LSN_TBL VALUES ('-1/0');
INSERT INTO MDB_LSN_TBL VALUES (' 0/12345678');
INSERT INTO MDB_LSN_TBL VALUES ('ABCD/');
INSERT INTO MDB_LSN_TBL VALUES ('/ABCD');
DROP TABLE MDB_LSN_TBL;

-- Operators
SELECT '0/16AE7F8' = '0/16AE7F8'::mdb_lsn;
SELECT '0/16AE7F8'::mdb_lsn != '0/16AE7F7';
SELECT '0/16AE7F7' < '0/16AE7F8'::mdb_lsn;
SELECT '0/16AE7F8' > mdb_lsn '0/16AE7F7';
SELECT '0/16AE7F7'::mdb_lsn - '0/16AE7F8'::mdb_lsn;
SELECT '0/16AE7F8'::mdb_lsn - '0/16AE7F7'::mdb_lsn;

-- Check btree and hash opclasses
EXPLAIN (COSTS OFF)
SELECT DISTINCT (i || '/' || j)::mdb_lsn f
  FROM generate_series(1, 10) i,
       generate_series(1, 10) j,
       generate_series(1, 5) k
  WHERE i <= 10 AND j > 0 AND j <= 10
  ORDER BY f;

SELECT DISTINCT (i || '/' || j)::mdb_lsn f
  FROM generate_series(1, 10) i,
       generate_series(1, 10) j,
       generate_series(1, 5) k
  WHERE i <= 10 AND j > 0 AND j <= 10
  ORDER BY f;
