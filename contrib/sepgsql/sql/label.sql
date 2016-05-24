--
-- Regression Tests for Label Management
--

--
-- Setup
--
CREATE TABLE t1 (a int, b text);
INSERT INTO t1 VALUES (1, 'aaa'), (2, 'bbb'), (3, 'ccc');
SELECT * INTO t2 FROM t1 WHERE a % 2 = 0;

CREATE FUNCTION f1 () RETURNS text
    AS 'SELECT semdb_getcon()'
    LANGUAGE sql;

CREATE FUNCTION f2 () RETURNS text
    AS 'SELECT semdb_getcon()'
    LANGUAGE sql;
SECURITY LABEL ON FUNCTION f2()
    IS 'system_u:object_r:semdb_trusted_proc_exec_t:s0';

CREATE FUNCTION f3 () RETURNS text
    AS 'BEGIN
          RAISE EXCEPTION ''an exception from f3()'';
          RETURN NULL;
        END;' LANGUAGE plmdb;
SECURITY LABEL ON FUNCTION f3()
    IS 'system_u:object_r:semdb_trusted_proc_exec_t:s0';

CREATE FUNCTION f4 () RETURNS text
    AS 'SELECT semdb_getcon()'
    LANGUAGE sql;
SECURITY LABEL ON FUNCTION f4()
    IS 'system_u:object_r:semdb_nosuch_trusted_proc_exec_t:s0';

CREATE FUNCTION f5 (text) RETURNS bool
	AS 'SELECT semdb_setcon($1)'
    LANGUAGE sql;
SECURITY LABEL ON FUNCTION f5(text)
    IS 'system_u:object_r:semdb_regtest_trusted_proc_exec_t:s0';

CREATE TABLE auth_tbl(uname text, credential text, label text);
INSERT INTO auth_tbl
    VALUES ('foo', 'acbd18db4cc2f85cedef654fccc4a4d8', 'semdb_regtest_foo_t:s0'),
           ('var', 'b2145aac704ce76dbe1ac7adac535b23', 'semdb_regtest_var_t:s0'),
           ('baz', 'b2145aac704ce76dbe1ac7adac535b23', 'semdb_regtest_baz_t:s0');
SECURITY LABEL ON TABLE auth_tbl
    IS 'system_u:object_r:semdb_secret_table_t:s0';

CREATE FUNCTION auth_func(text, text) RETURNS bool
    LANGUAGE sql
    AS 'SELECT semdb_setcon(regexp_replace(semdb_getcon(), ''_r:.*$'', ''_r:'' || label))
        FROM auth_tbl WHERE uname = $1 AND credential = $2';
SECURITY LABEL ON FUNCTION auth_func(text,text)
    IS 'system_u:object_r:semdb_regtest_trusted_proc_exec_t:s0';

CREATE TABLE foo_tbl(a int, b text);
INSERT INTO foo_tbl VALUES (1, 'aaa'), (2,'bbb'), (3,'ccc'), (4,'ddd');
SECURITY LABEL ON TABLE foo_tbl
	IS 'system_u:object_r:semdb_regtest_foo_table_t:s0';

CREATE TABLE var_tbl(x int, y text);
INSERT INTO var_tbl VALUES (2,'xxx'), (3,'yyy'), (4,'zzz'), (5,'xyz');
SECURITY LABEL ON TABLE var_tbl
	IS 'system_u:object_r:semdb_regtest_var_table_t:s0';

--
-- Tests for default labeling behavior
--
-- @SECURITY-CONTEXT=unconfined_u:unconfined_r:semdb_regtest_user_t:s0
CREATE TABLE t3 (s int, t text);
INSERT INTO t3 VALUES (1, 'sss'), (2, 'ttt'), (3, 'uuu');

-- @SECURITY-CONTEXT=unconfined_u:unconfined_r:semdb_regtest_dba_t:s0
CREATE TABLE t4 (m int, n text);
INSERT INTO t4 VALUES (1,'mmm'), (2,'nnn'), (3,'ooo');

SELECT objtype, objname, label FROM pg_seclabels
    WHERE provider = 'selinux' AND objtype = 'table' AND objname in ('t1', 't2', 't3');
SELECT objtype, objname, label FROM pg_seclabels
    WHERE provider = 'selinux' AND objtype = 'column' AND (objname like 't3.%' OR objname like 't4.%');

--
-- Tests for SECURITY LABEL
--
-- @SECURITY-CONTEXT=unconfined_u:unconfined_r:semdb_regtest_dba_t:s0
SECURITY LABEL ON TABLE t1
    IS 'system_u:object_r:semdb_ro_table_t:s0';	-- ok
SECURITY LABEL ON TABLE t2
    IS 'invalid security context';			-- be failed
SECURITY LABEL ON COLUMN t2
    IS 'system_u:object_r:semdb_ro_table_t:s0';	-- be failed
SECURITY LABEL ON COLUMN t2.b
    IS 'system_u:object_r:semdb_ro_table_t:s0';	-- ok

--
-- Tests for Trusted Procedures
--
-- @SECURITY-CONTEXT=unconfined_u:unconfined_r:semdb_regtest_user_t:s0
SET semdb.debug_audit = true;
SET client_min_messages = log;
SELECT f1();			-- normal procedure
SELECT f2();			-- trusted procedure
SELECT f3();			-- trusted procedure that raises an error
SELECT f4();			-- failed on domain transition
SELECT semdb_getcon();	-- client's label must be restored

--
-- Test for Dynamic Domain Transition
--

-- validation of transaction aware dynamic-transition
-- @SECURITY-CONTEXT=unconfined_u:unconfined_r:semdb_regtest_superuser_t:s0:c0.c25
SELECT semdb_setcon('unconfined_u:unconfined_r:semdb_regtest_superuser_t:s0:c0.c15');
SELECT semdb_getcon();

SELECT semdb_setcon(NULL);	-- failed to reset
SELECT semdb_getcon();

BEGIN;
SELECT semdb_setcon('unconfined_u:unconfined_r:semdb_regtest_superuser_t:s0:c0.c12');
SELECT semdb_getcon();

SAVEPOINT svpt_1;
SELECT semdb_setcon('unconfined_u:unconfined_r:semdb_regtest_superuser_t:s0:c0.c9');
SELECT semdb_getcon();

SAVEPOINT svpt_2;
SELECT semdb_setcon('unconfined_u:unconfined_r:semdb_regtest_superuser_t:s0:c0.c6');
SELECT semdb_getcon();

SAVEPOINT svpt_3;
SELECT semdb_setcon('unconfined_u:unconfined_r:semdb_regtest_superuser_t:s0:c0.c3');
SELECT semdb_getcon();

ROLLBACK TO SAVEPOINT svpt_2;
SELECT semdb_getcon();		-- should be 's0:c0.c9'

ROLLBACK TO SAVEPOINT svpt_1;
SELECT semdb_getcon();		-- should be 's0:c0.c12'

ABORT;
SELECT semdb_getcon();		-- should be 's0:c0.c15'

BEGIN;
SELECT semdb_setcon('unconfined_u:unconfined_r:semdb_regtest_superuser_t:s0:c0.c8');
SELECT semdb_getcon();

SAVEPOINT svpt_1;
SELECT semdb_setcon('unconfined_u:unconfined_r:semdb_regtest_superuser_t:s0:c0.c4');
SELECT semdb_getcon();

ROLLBACK TO SAVEPOINT svpt_1;
SELECT semdb_getcon();		-- should be 's0:c0.c8'
SELECT semdb_setcon('unconfined_u:unconfined_r:semdb_regtest_superuser_t:s0:c0.c6');

COMMIT;
SELECT semdb_getcon();		-- should be 's0:c0.c6'

-- semdb_regtest_user_t is not available dynamic-transition,
-- unless semdb_setcon() is called inside of trusted-procedure
-- @SECURITY-CONTEXT=unconfined_u:unconfined_r:semdb_regtest_user_t:s0:c0.c15

-- semdb_regtest_user_t has no permission to switch current label
SELECT semdb_setcon('unconfined_u:unconfined_r:semdb_regtest_user_t:s0');	-- failed
SELECT semdb_getcon();

-- trusted procedure allows to switch, but unavailable to override MCS rules
SELECT f5('unconfined_u:unconfined_r:semdb_regtest_user_t:s0:c0.c7');	-- OK
SELECT semdb_getcon();

SELECT f5('unconfined_u:unconfined_r:semdb_regtest_user_t:s0:c0.c31');	-- Failed
SELECT semdb_getcon();

SELECT f5(NULL);	-- Failed
SELECT semdb_getcon();

BEGIN;
SELECT f5('unconfined_u:unconfined_r:semdb_regtest_user_t:s0:c0.c3');	-- OK
SELECT semdb_getcon();

ABORT;
SELECT semdb_getcon();

--
-- Test for simulation of typical connection pooling server
--
-- @SECURITY-CONTEXT=unconfined_u:unconfined_r:semdb_regtest_pool_t:s0

-- we shouldn't allow to switch client label without trusted procedure
SELECT semdb_setcon('unconfined_u:unconfined_r:semdb_regtest_foo_t:s0');

SELECT * FROM auth_tbl;	-- failed, no permission to reference

-- switch to "foo"
SELECT auth_func('foo', 'acbd18db4cc2f85cedef654fccc4a4d8');

SELECT semdb_getcon();

SELECT * FROM foo_tbl;	-- OK

SELECT * FROM var_tbl;	-- failed

SELECT * FROM auth_tbl;	-- failed

SELECT semdb_setcon(NULL);	-- end of session
SELECT semdb_getcon();

-- the pooler cannot touch these tables directry
SELECT * FROM foo_tbl;	-- failed

SELECT * FROM var_tbl;	-- failed

-- switch to "var"
SELECT auth_func('var', 'b2145aac704ce76dbe1ac7adac535b23');

SELECT semdb_getcon();

SELECT * FROM foo_tbl;  -- failed

SELECT * FROM var_tbl;  -- OK

SELECT * FROM auth_tbl;	-- failed

SELECT semdb_setcon(NULL);    -- end of session

-- misc checks
SELECT auth_func('var', 'invalid credential');	-- not works
SELECT semdb_getcon();

--
-- Clean up
--
-- @SECURITY-CONTEXT=unconfined_u:unconfined_r:semdb_regtest_superuser_t:s0-s0:c0.c255
DROP TABLE IF EXISTS t1 CASCADE;
DROP TABLE IF EXISTS t2 CASCADE;
DROP TABLE IF EXISTS t3 CASCADE;
DROP TABLE IF EXISTS t4 CASCADE;
DROP FUNCTION IF EXISTS f1() CASCADE;
DROP FUNCTION IF EXISTS f2() CASCADE;
DROP FUNCTION IF EXISTS f3() CASCADE;
DROP FUNCTION IF EXISTS f4() CASCADE;
DROP FUNCTION IF EXISTS f5(text) CASCADE;
