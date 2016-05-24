---
--- CREATE_TRIGGER
---

CREATE FUNCTION plmdb_function_trigger_1()
  RETURNS TRIGGER
  LANGUAGE plmdb
AS $$
BEGIN
  RETURN NEW;
END;
$$;

CREATE TRIGGER trigger_1
  BEFORE INSERT OR UPDATE
  ON datatype_table
  FOR EACH ROW
  EXECUTE PROCEDURE plmdb_function_trigger_1();
