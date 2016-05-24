--
-- regproc
--

/* If objects exist, return oids */

CREATE ROLE regtestrole;

-- without schemaname

SELECT regoper('||/');
SELECT regoperator('+(int4,int4)');
SELECT regproc('now');
SELECT regprocedure('abs(numeric)');
SELECT regclass('mdb_class');
SELECT regtype('int4');

SELECT to_regoper('||/');
SELECT to_regoperator('+(int4,int4)');
SELECT to_regproc('now');
SELECT to_regprocedure('abs(numeric)');
SELECT to_regclass('mdb_class');
SELECT to_regtype('int4');

-- with schemaname

SELECT regoper('mdb_catalog.||/');
SELECT regoperator('mdb_catalog.+(int4,int4)');
SELECT regproc('mdb_catalog.now');
SELECT regprocedure('mdb_catalog.abs(numeric)');
SELECT regclass('mdb_catalog.mdb_class');
SELECT regtype('mdb_catalog.int4');

SELECT to_regoper('mdb_catalog.||/');
SELECT to_regproc('mdb_catalog.now');
SELECT to_regprocedure('mdb_catalog.abs(numeric)');
SELECT to_regclass('mdb_catalog.mdb_class');
SELECT to_regtype('mdb_catalog.int4');

-- schemaname not applicable

SELECT regrole('regtestrole');
SELECT regrole('"regtestrole"');
SELECT regnamespace('mdb_catalog');
SELECT regnamespace('"mdb_catalog"');

SELECT to_regrole('regtestrole');
SELECT to_regrole('"regtestrole"');
SELECT to_regnamespace('mdb_catalog');
SELECT to_regnamespace('"mdb_catalog"');

/* If objects don't exist, raise errors. */

DROP ROLE regtestrole;

-- without schemaname

SELECT regoper('||//');
SELECT regoperator('++(int4,int4)');
SELECT regproc('know');
SELECT regprocedure('absinthe(numeric)');
SELECT regclass('mdb_classes');
SELECT regtype('int3');

-- with schemaname

SELECT regoper('ng_catalog.||/');
SELECT regoperator('ng_catalog.+(int4,int4)');
SELECT regproc('ng_catalog.now');
SELECT regprocedure('ng_catalog.abs(numeric)');
SELECT regclass('ng_catalog.mdb_class');
SELECT regtype('ng_catalog.int4');

-- schemaname not applicable

SELECT regrole('regtestrole');
SELECT regrole('"regtestrole"');
SELECT regrole('Nonexistent');
SELECT regrole('"Nonexistent"');
SELECT regrole('foo.bar');
SELECT regnamespace('Nonexistent');
SELECT regnamespace('"Nonexistent"');
SELECT regnamespace('foo.bar');

/* If objects don't exist, return NULL with no error. */

-- without schemaname

SELECT to_regoper('||//');
SELECT to_regoperator('++(int4,int4)');
SELECT to_regproc('know');
SELECT to_regprocedure('absinthe(numeric)');
SELECT to_regclass('mdb_classes');
SELECT to_regtype('int3');

-- with schemaname

SELECT to_regoper('ng_catalog.||/');
SELECT to_regoperator('ng_catalog.+(int4,int4)');
SELECT to_regproc('ng_catalog.now');
SELECT to_regprocedure('ng_catalog.abs(numeric)');
SELECT to_regclass('ng_catalog.mdb_class');
SELECT to_regtype('ng_catalog.int4');

-- schemaname not applicable

SELECT to_regrole('regtestrole');
SELECT to_regrole('"regtestrole"');
SELECT to_regrole('foo.bar');
SELECT to_regrole('Nonexistent');
SELECT to_regrole('"Nonexistent"');
SELECT to_regrole('foo.bar');
SELECT to_regnamespace('Nonexistent');
SELECT to_regnamespace('"Nonexistent"');
SELECT to_regnamespace('foo.bar');
