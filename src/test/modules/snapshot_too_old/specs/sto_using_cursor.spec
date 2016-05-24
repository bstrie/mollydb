# This test provokes a "snapshot too old" error using a cursor.
#
# The sleep is needed because with a threshold of zero a statement could error
# on changes it made.  With more normal settings no external delay is needed,
# but we don't want these tests to run long enough to see that, since
# granularity is in minutes.
#
# Since results depend on the value of old_snapshot_threshold, sneak that into
# the line generated by the sleep, so that a surprising values isn't so hard
# to identify.

setup
{
    CREATE TABLE sto1 (c int NOT NULL);
    INSERT INTO sto1 SELECT generate_series(1, 1000);
    CREATE TABLE sto2 (c int NOT NULL);
}
setup
{
    VACUUM ANALYZE sto1;
}

teardown
{
    DROP TABLE sto1, sto2;
}

session "s1"
setup			{ BEGIN ISOLATION LEVEL REPEATABLE READ; }
step "s1decl"	{ DECLARE cursor1 CURSOR FOR SELECT c FROM sto1; }
step "s1f1"		{ FETCH FIRST FROM cursor1; }
step "s1sleep"	{ SELECT setting, mdb_sleep(6) FROM mdb_settings WHERE name = 'old_snapshot_threshold'; }
step "s1f2"		{ FETCH FIRST FROM cursor1; }
teardown		{ COMMIT; }

session "s2"
step "s2u"		{ UPDATE sto1 SET c = 1001 WHERE c = 1; }
