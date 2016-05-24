--
-- Hot Standby tests
--
-- hs_standby_functions.sql
--

-- should fail
select txid_current();

select length(txid_current_snapshot()::text) >= 4;

select mdb_start_backup('should fail');
select mdb_switch_xlog();
select mdb_stop_backup();

-- should return no rows
select * from mdb_prepared_xacts;

-- just the startup process
select locktype, virtualxid, virtualtransaction, mode, granted
from mdb_locks where virtualxid = '1/1';

-- suicide is painless
select mdb_cancel_backend(mdb_backend_pid());
