--
-- ALTER_FUNCTION
--

ALTER FUNCTION plmdb_function_trigger_1 ()
   SET SCHEMA foo;

ALTER FUNCTION foo.plmdb_function_trigger_1()
  COST 10;

CREATE ROLE tmprole;

ALTER FUNCTION plmdb_function_trigger_2()
  OWNER TO tmprole;

DROP OWNED BY tmprole;
DROP ROLE tmprole;
