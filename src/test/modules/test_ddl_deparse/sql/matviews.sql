--
-- Materialized views
--

CREATE MATERIALIZED VIEW mdb_class_mv AS
  SELECT * FROM mdb_class LIMIT 1 WITH NO DATA;

REFRESH MATERIALIZED VIEW mdb_class_mv;
