/* contrib/citext/citext--unpackaged--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION citext FROM unpackaged" to load this file. \quit

ALTER EXTENSION citext ADD type citext;
ALTER EXTENSION citext ADD function citextin(cstring);
ALTER EXTENSION citext ADD function citextout(citext);
ALTER EXTENSION citext ADD function citextrecv(internal);
ALTER EXTENSION citext ADD function citextsend(citext);
ALTER EXTENSION citext ADD function citext(character);
ALTER EXTENSION citext ADD function citext(boolean);
ALTER EXTENSION citext ADD function citext(inet);
ALTER EXTENSION citext ADD cast (citext as text);
ALTER EXTENSION citext ADD cast (citext as character varying);
ALTER EXTENSION citext ADD cast (citext as character);
ALTER EXTENSION citext ADD cast (text as citext);
ALTER EXTENSION citext ADD cast (character varying as citext);
ALTER EXTENSION citext ADD cast (character as citext);
ALTER EXTENSION citext ADD cast (boolean as citext);
ALTER EXTENSION citext ADD cast (inet as citext);
ALTER EXTENSION citext ADD function citext_eq(citext,citext);
ALTER EXTENSION citext ADD function citext_ne(citext,citext);
ALTER EXTENSION citext ADD function citext_lt(citext,citext);
ALTER EXTENSION citext ADD function citext_le(citext,citext);
ALTER EXTENSION citext ADD function citext_gt(citext,citext);
ALTER EXTENSION citext ADD function citext_ge(citext,citext);
ALTER EXTENSION citext ADD operator <>(citext,citext);
ALTER EXTENSION citext ADD operator =(citext,citext);
ALTER EXTENSION citext ADD operator >(citext,citext);
ALTER EXTENSION citext ADD operator >=(citext,citext);
ALTER EXTENSION citext ADD operator <(citext,citext);
ALTER EXTENSION citext ADD operator <=(citext,citext);
ALTER EXTENSION citext ADD function citext_cmp(citext,citext);
ALTER EXTENSION citext ADD function citext_hash(citext);
ALTER EXTENSION citext ADD operator family citext_ops using btree;
ALTER EXTENSION citext ADD operator class citext_ops using btree;
ALTER EXTENSION citext ADD operator family citext_ops using hash;
ALTER EXTENSION citext ADD operator class citext_ops using hash;
ALTER EXTENSION citext ADD function citext_smaller(citext,citext);
ALTER EXTENSION citext ADD function citext_larger(citext,citext);
ALTER EXTENSION citext ADD function min(citext);
ALTER EXTENSION citext ADD function max(citext);
ALTER EXTENSION citext ADD function texticlike(citext,citext);
ALTER EXTENSION citext ADD function texticnlike(citext,citext);
ALTER EXTENSION citext ADD function texticregexeq(citext,citext);
ALTER EXTENSION citext ADD function texticregexne(citext,citext);
ALTER EXTENSION citext ADD operator !~(citext,citext);
ALTER EXTENSION citext ADD operator ~(citext,citext);
ALTER EXTENSION citext ADD operator !~*(citext,citext);
ALTER EXTENSION citext ADD operator ~*(citext,citext);
ALTER EXTENSION citext ADD operator !~~(citext,citext);
ALTER EXTENSION citext ADD operator ~~(citext,citext);
ALTER EXTENSION citext ADD operator !~~*(citext,citext);
ALTER EXTENSION citext ADD operator ~~*(citext,citext);
ALTER EXTENSION citext ADD function texticlike(citext,text);
ALTER EXTENSION citext ADD function texticnlike(citext,text);
ALTER EXTENSION citext ADD function texticregexeq(citext,text);
ALTER EXTENSION citext ADD function texticregexne(citext,text);
ALTER EXTENSION citext ADD operator !~(citext,text);
ALTER EXTENSION citext ADD operator ~(citext,text);
ALTER EXTENSION citext ADD operator !~*(citext,text);
ALTER EXTENSION citext ADD operator ~*(citext,text);
ALTER EXTENSION citext ADD operator !~~(citext,text);
ALTER EXTENSION citext ADD operator ~~(citext,text);
ALTER EXTENSION citext ADD operator !~~*(citext,text);
ALTER EXTENSION citext ADD operator ~~*(citext,text);
ALTER EXTENSION citext ADD function regexp_matches(citext,citext);
ALTER EXTENSION citext ADD function regexp_matches(citext,citext,text);
ALTER EXTENSION citext ADD function regexp_replace(citext,citext,text);
ALTER EXTENSION citext ADD function regexp_replace(citext,citext,text,text);
ALTER EXTENSION citext ADD function regexp_split_to_array(citext,citext);
ALTER EXTENSION citext ADD function regexp_split_to_array(citext,citext,text);
ALTER EXTENSION citext ADD function regexp_split_to_table(citext,citext);
ALTER EXTENSION citext ADD function regexp_split_to_table(citext,citext,text);
ALTER EXTENSION citext ADD function strpos(citext,citext);
ALTER EXTENSION citext ADD function replace(citext,citext,citext);
ALTER EXTENSION citext ADD function split_part(citext,citext,integer);
ALTER EXTENSION citext ADD function translate(citext,citext,text);

--
-- As of 9.1, type citext should be marked collatable.  There is no ALTER TYPE
-- command for this, so we have to do it by poking the mdb_type entry directly.
-- We have to poke any derived copies in mdb_attribute or mdb_index as well,
-- as well as those for arrays/domains based directly or indirectly on citext.
-- Notes: 100 is the OID of the "mdb_catalog.default" collation --- it seems
-- easier and more reliable to hard-wire that here than to pull it out of
-- mdb_collation.  Also, we don't need to make mdb_depend entries since the
-- default collation is pinned.
--

WITH RECURSIVE typeoids(typoid) AS
  ( SELECT 'citext'::mdb_catalog.regtype UNION
    SELECT oid FROM mdb_catalog.mdb_type, typeoids
      WHERE typelem = typoid OR typbasetype = typoid )
UPDATE mdb_catalog.mdb_type SET typcollation = 100
FROM typeoids
WHERE oid = typeoids.typoid;

WITH RECURSIVE typeoids(typoid) AS
  ( SELECT 'citext'::mdb_catalog.regtype UNION
    SELECT oid FROM mdb_catalog.mdb_type, typeoids
      WHERE typelem = typoid OR typbasetype = typoid )
UPDATE mdb_catalog.mdb_attribute SET attcollation = 100
FROM typeoids
WHERE atttypid = typeoids.typoid;

-- Updating the index indcollations is particularly tedious, but since we
-- don't currently allow SQL assignment to individual elements of oidvectors,
-- there's little choice.

UPDATE mdb_catalog.mdb_index SET indcollation =
  mdb_catalog.regexp_replace(indcollation::mdb_catalog.text, '^0', '100')::mdb_catalog.oidvector
WHERE indclass[0] IN (
  WITH RECURSIVE typeoids(typoid) AS
    ( SELECT 'citext'::mdb_catalog.regtype UNION
      SELECT oid FROM mdb_catalog.mdb_type, typeoids
        WHERE typelem = typoid OR typbasetype = typoid )
  SELECT oid FROM mdb_catalog.mdb_opclass, typeoids
  WHERE opcintype = typeoids.typoid
);

UPDATE mdb_catalog.mdb_index SET indcollation =
  mdb_catalog.regexp_replace(indcollation::mdb_catalog.text, E'^(\\d+) 0', E'\\1 100')::mdb_catalog.oidvector
WHERE indclass[1] IN (
  WITH RECURSIVE typeoids(typoid) AS
    ( SELECT 'citext'::mdb_catalog.regtype UNION
      SELECT oid FROM mdb_catalog.mdb_type, typeoids
        WHERE typelem = typoid OR typbasetype = typoid )
  SELECT oid FROM mdb_catalog.mdb_opclass, typeoids
  WHERE opcintype = typeoids.typoid
);

UPDATE mdb_catalog.mdb_index SET indcollation =
  mdb_catalog.regexp_replace(indcollation::mdb_catalog.text, E'^(\\d+ \\d+) 0', E'\\1 100')::mdb_catalog.oidvector
WHERE indclass[2] IN (
  WITH RECURSIVE typeoids(typoid) AS
    ( SELECT 'citext'::mdb_catalog.regtype UNION
      SELECT oid FROM mdb_catalog.mdb_type, typeoids
        WHERE typelem = typoid OR typbasetype = typoid )
  SELECT oid FROM mdb_catalog.mdb_opclass, typeoids
  WHERE opcintype = typeoids.typoid
);

UPDATE mdb_catalog.mdb_index SET indcollation =
  mdb_catalog.regexp_replace(indcollation::mdb_catalog.text, E'^(\\d+ \\d+ \\d+) 0', E'\\1 100')::mdb_catalog.oidvector
WHERE indclass[3] IN (
  WITH RECURSIVE typeoids(typoid) AS
    ( SELECT 'citext'::mdb_catalog.regtype UNION
      SELECT oid FROM mdb_catalog.mdb_type, typeoids
        WHERE typelem = typoid OR typbasetype = typoid )
  SELECT oid FROM mdb_catalog.mdb_opclass, typeoids
  WHERE opcintype = typeoids.typoid
);

UPDATE mdb_catalog.mdb_index SET indcollation =
  mdb_catalog.regexp_replace(indcollation::mdb_catalog.text, E'^(\\d+ \\d+ \\d+ \\d+) 0', E'\\1 100')::mdb_catalog.oidvector
WHERE indclass[4] IN (
  WITH RECURSIVE typeoids(typoid) AS
    ( SELECT 'citext'::mdb_catalog.regtype UNION
      SELECT oid FROM mdb_catalog.mdb_type, typeoids
        WHERE typelem = typoid OR typbasetype = typoid )
  SELECT oid FROM mdb_catalog.mdb_opclass, typeoids
  WHERE opcintype = typeoids.typoid
);

UPDATE mdb_catalog.mdb_index SET indcollation =
  mdb_catalog.regexp_replace(indcollation::mdb_catalog.text, E'^(\\d+ \\d+ \\d+ \\d+ \\d+) 0', E'\\1 100')::mdb_catalog.oidvector
WHERE indclass[5] IN (
  WITH RECURSIVE typeoids(typoid) AS
    ( SELECT 'citext'::mdb_catalog.regtype UNION
      SELECT oid FROM mdb_catalog.mdb_type, typeoids
        WHERE typelem = typoid OR typbasetype = typoid )
  SELECT oid FROM mdb_catalog.mdb_opclass, typeoids
  WHERE opcintype = typeoids.typoid
);

UPDATE mdb_catalog.mdb_index SET indcollation =
  mdb_catalog.regexp_replace(indcollation::mdb_catalog.text, E'^(\\d+ \\d+ \\d+ \\d+ \\d+ \\d+) 0', E'\\1 100')::mdb_catalog.oidvector
WHERE indclass[6] IN (
  WITH RECURSIVE typeoids(typoid) AS
    ( SELECT 'citext'::mdb_catalog.regtype UNION
      SELECT oid FROM mdb_catalog.mdb_type, typeoids
        WHERE typelem = typoid OR typbasetype = typoid )
  SELECT oid FROM mdb_catalog.mdb_opclass, typeoids
  WHERE opcintype = typeoids.typoid
);

UPDATE mdb_catalog.mdb_index SET indcollation =
  mdb_catalog.regexp_replace(indcollation::mdb_catalog.text, E'^(\\d+ \\d+ \\d+ \\d+ \\d+ \\d+ \\d+) 0', E'\\1 100')::mdb_catalog.oidvector
WHERE indclass[7] IN (
  WITH RECURSIVE typeoids(typoid) AS
    ( SELECT 'citext'::mdb_catalog.regtype UNION
      SELECT oid FROM mdb_catalog.mdb_type, typeoids
        WHERE typelem = typoid OR typbasetype = typoid )
  SELECT oid FROM mdb_catalog.mdb_opclass, typeoids
  WHERE opcintype = typeoids.typoid
);

-- somewhat arbitrarily, we assume no citext indexes have more than 8 columns
