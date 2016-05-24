--
-- ADVISORY LOCKS
--

BEGIN;

SELECT
	mdb_advisory_xact_lock(1), mdb_advisory_xact_lock_shared(2),
	mdb_advisory_xact_lock(1, 1), mdb_advisory_xact_lock_shared(2, 2);

SELECT locktype, classid, objid, objsubid, mode, granted
	FROM mdb_locks WHERE locktype = 'advisory'
	ORDER BY classid, objid, objsubid;


-- mdb_advisory_unlock_all() shouldn't release xact locks
SELECT mdb_advisory_unlock_all();

SELECT count(*) FROM mdb_locks WHERE locktype = 'advisory';


-- can't unlock xact locks
SELECT
	mdb_advisory_unlock(1), mdb_advisory_unlock_shared(2),
	mdb_advisory_unlock(1, 1), mdb_advisory_unlock_shared(2, 2);


-- automatically release xact locks at commit
COMMIT;

SELECT count(*) FROM mdb_locks WHERE locktype = 'advisory';


BEGIN;

-- holding both session and xact locks on the same objects, xact first
SELECT
	mdb_advisory_xact_lock(1), mdb_advisory_xact_lock_shared(2),
	mdb_advisory_xact_lock(1, 1), mdb_advisory_xact_lock_shared(2, 2);

SELECT locktype, classid, objid, objsubid, mode, granted
	FROM mdb_locks WHERE locktype = 'advisory'
	ORDER BY classid, objid, objsubid;

SELECT
	mdb_advisory_lock(1), mdb_advisory_lock_shared(2),
	mdb_advisory_lock(1, 1), mdb_advisory_lock_shared(2, 2);

ROLLBACK;

SELECT locktype, classid, objid, objsubid, mode, granted
	FROM mdb_locks WHERE locktype = 'advisory'
	ORDER BY classid, objid, objsubid;


-- unlocking session locks
SELECT
	mdb_advisory_unlock(1), mdb_advisory_unlock(1),
	mdb_advisory_unlock_shared(2), mdb_advisory_unlock_shared(2),
	mdb_advisory_unlock(1, 1), mdb_advisory_unlock(1, 1),
	mdb_advisory_unlock_shared(2, 2), mdb_advisory_unlock_shared(2, 2);

SELECT count(*) FROM mdb_locks WHERE locktype = 'advisory';


BEGIN;

-- holding both session and xact locks on the same objects, session first
SELECT
	mdb_advisory_lock(1), mdb_advisory_lock_shared(2),
	mdb_advisory_lock(1, 1), mdb_advisory_lock_shared(2, 2);

SELECT locktype, classid, objid, objsubid, mode, granted
	FROM mdb_locks WHERE locktype = 'advisory'
	ORDER BY classid, objid, objsubid;

SELECT
	mdb_advisory_xact_lock(1), mdb_advisory_xact_lock_shared(2),
	mdb_advisory_xact_lock(1, 1), mdb_advisory_xact_lock_shared(2, 2);

ROLLBACK;

SELECT locktype, classid, objid, objsubid, mode, granted
	FROM mdb_locks WHERE locktype = 'advisory'
	ORDER BY classid, objid, objsubid;


-- releasing all session locks
SELECT mdb_advisory_unlock_all();

SELECT count(*) FROM mdb_locks WHERE locktype = 'advisory';


BEGIN;

-- grabbing txn locks multiple times

SELECT
	mdb_advisory_xact_lock(1), mdb_advisory_xact_lock(1),
	mdb_advisory_xact_lock_shared(2), mdb_advisory_xact_lock_shared(2),
	mdb_advisory_xact_lock(1, 1), mdb_advisory_xact_lock(1, 1),
	mdb_advisory_xact_lock_shared(2, 2), mdb_advisory_xact_lock_shared(2, 2);

SELECT locktype, classid, objid, objsubid, mode, granted
	FROM mdb_locks WHERE locktype = 'advisory'
	ORDER BY classid, objid, objsubid;

COMMIT;

SELECT count(*) FROM mdb_locks WHERE locktype = 'advisory';

-- grabbing session locks multiple times

SELECT
	mdb_advisory_lock(1), mdb_advisory_lock(1),
	mdb_advisory_lock_shared(2), mdb_advisory_lock_shared(2),
	mdb_advisory_lock(1, 1), mdb_advisory_lock(1, 1),
	mdb_advisory_lock_shared(2, 2), mdb_advisory_lock_shared(2, 2);

SELECT locktype, classid, objid, objsubid, mode, granted
	FROM mdb_locks WHERE locktype = 'advisory'
	ORDER BY classid, objid, objsubid;

SELECT
	mdb_advisory_unlock(1), mdb_advisory_unlock(1),
	mdb_advisory_unlock_shared(2), mdb_advisory_unlock_shared(2),
	mdb_advisory_unlock(1, 1), mdb_advisory_unlock(1, 1),
	mdb_advisory_unlock_shared(2, 2), mdb_advisory_unlock_shared(2, 2);

SELECT count(*) FROM mdb_locks WHERE locktype = 'advisory';

-- .. and releasing them all at once

SELECT
	mdb_advisory_lock(1), mdb_advisory_lock(1),
	mdb_advisory_lock_shared(2), mdb_advisory_lock_shared(2),
	mdb_advisory_lock(1, 1), mdb_advisory_lock(1, 1),
	mdb_advisory_lock_shared(2, 2), mdb_advisory_lock_shared(2, 2);

SELECT locktype, classid, objid, objsubid, mode, granted
	FROM mdb_locks WHERE locktype = 'advisory'
	ORDER BY classid, objid, objsubid;

SELECT mdb_advisory_unlock_all();

SELECT count(*) FROM mdb_locks WHERE locktype = 'advisory';
