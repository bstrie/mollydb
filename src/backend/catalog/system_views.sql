/*
 * MollyDB System Views
 *
 * Copyright (c) 1996-2016, MollyDB Global Development Group
 *
 * src/backend/catalog/system_views.sql
 *
 * Note: this file is read in single-user -j mode, which means that the
 * command terminator is semicolon-newline-newline; whenever the backend
 * sees that, it stops and executes what it's got.  If you write a lot of
 * statements without empty lines between, they'll all get quoted to you
 * in any error message about one of them, so don't do that.  Also, you
 * cannot write a semicolon immediately followed by an empty line in a
 * string literal (including a function body!) or a multiline comment.
 */

CREATE VIEW mdb_roles AS
    SELECT
        rolname,
        rolsuper,
        rolinherit,
        rolcreaterole,
        rolcreatedb,
        rolcanlogin,
        rolreplication,
        rolconnlimit,
        '********'::text as rolpassword,
        rolvaliduntil,
        rolbypassrls,
        setconfig as rolconfig,
        mdb_authid.oid
    FROM mdb_authid LEFT JOIN mdb_db_role_setting s
    ON (mdb_authid.oid = setrole AND setdatabase = 0);

CREATE VIEW mdb_shadow AS
    SELECT
        rolname AS usename,
        mdb_authid.oid AS usesysid,
        rolcreatedb AS usecreatedb,
        rolsuper AS usesuper,
        rolreplication AS userepl,
        rolbypassrls AS usebypassrls,
        rolpassword AS passwd,
        rolvaliduntil::abstime AS valuntil,
        setconfig AS useconfig
    FROM mdb_authid LEFT JOIN mdb_db_role_setting s
    ON (mdb_authid.oid = setrole AND setdatabase = 0)
    WHERE rolcanlogin;

REVOKE ALL on mdb_shadow FROM public;

CREATE VIEW mdb_group AS
    SELECT
        rolname AS groname,
        oid AS grosysid,
        ARRAY(SELECT member FROM mdb_auth_members WHERE roleid = oid) AS grolist
    FROM mdb_authid
    WHERE NOT rolcanlogin;

CREATE VIEW mdb_user AS
    SELECT
        usename,
        usesysid,
        usecreatedb,
        usesuper,
        userepl,
        usebypassrls,
        '********'::text as passwd,
        valuntil,
        useconfig
    FROM mdb_shadow;

CREATE VIEW mdb_policies AS
    SELECT
        N.nspname AS schemaname,
        C.relname AS tablename,
        pol.polname AS policyname,
        CASE
            WHEN pol.polroles = '{0}' THEN
                string_to_array('public', '')
            ELSE
                ARRAY
                (
                    SELECT rolname
                    FROM mdb_catalog.mdb_authid
                    WHERE oid = ANY (pol.polroles) ORDER BY 1
                )
        END AS roles,
        CASE pol.polcmd
            WHEN 'r' THEN 'SELECT'
            WHEN 'a' THEN 'INSERT'
            WHEN 'w' THEN 'UPDATE'
            WHEN 'd' THEN 'DELETE'
            WHEN '*' THEN 'ALL'
        END AS cmd,
        mdb_catalog.mdb_get_expr(pol.polqual, pol.polrelid) AS qual,
        mdb_catalog.mdb_get_expr(pol.polwithcheck, pol.polrelid) AS with_check
    FROM mdb_catalog.mdb_policy pol
    JOIN mdb_catalog.mdb_class C ON (C.oid = pol.polrelid)
    LEFT JOIN mdb_catalog.mdb_namespace N ON (N.oid = C.relnamespace);

CREATE VIEW mdb_rules AS
    SELECT
        N.nspname AS schemaname,
        C.relname AS tablename,
        R.rulename AS rulename,
        mdb_get_ruledef(R.oid) AS definition
    FROM (mdb_rewrite R JOIN mdb_class C ON (C.oid = R.ev_class))
        LEFT JOIN mdb_namespace N ON (N.oid = C.relnamespace)
    WHERE R.rulename != '_RETURN';

CREATE VIEW mdb_views AS
    SELECT
        N.nspname AS schemaname,
        C.relname AS viewname,
        mdb_get_userbyid(C.relowner) AS viewowner,
        mdb_get_viewdef(C.oid) AS definition
    FROM mdb_class C LEFT JOIN mdb_namespace N ON (N.oid = C.relnamespace)
    WHERE C.relkind = 'v';

CREATE VIEW mdb_tables AS
    SELECT
        N.nspname AS schemaname,
        C.relname AS tablename,
        mdb_get_userbyid(C.relowner) AS tableowner,
        T.spcname AS tablespace,
        C.relhasindex AS hasindexes,
        C.relhasrules AS hasrules,
        C.relhastriggers AS hastriggers,
        C.relrowsecurity AS rowsecurity
    FROM mdb_class C LEFT JOIN mdb_namespace N ON (N.oid = C.relnamespace)
         LEFT JOIN mdb_tablespace T ON (T.oid = C.reltablespace)
    WHERE C.relkind = 'r';

CREATE VIEW mdb_matviews AS
    SELECT
        N.nspname AS schemaname,
        C.relname AS matviewname,
        mdb_get_userbyid(C.relowner) AS matviewowner,
        T.spcname AS tablespace,
        C.relhasindex AS hasindexes,
        C.relispopulated AS ispopulated,
        mdb_get_viewdef(C.oid) AS definition
    FROM mdb_class C LEFT JOIN mdb_namespace N ON (N.oid = C.relnamespace)
         LEFT JOIN mdb_tablespace T ON (T.oid = C.reltablespace)
    WHERE C.relkind = 'm';

CREATE VIEW mdb_indexes AS
    SELECT
        N.nspname AS schemaname,
        C.relname AS tablename,
        I.relname AS indexname,
        T.spcname AS tablespace,
        mdb_get_indexdef(I.oid) AS indexdef
    FROM mdb_index X JOIN mdb_class C ON (C.oid = X.indrelid)
         JOIN mdb_class I ON (I.oid = X.indexrelid)
         LEFT JOIN mdb_namespace N ON (N.oid = C.relnamespace)
         LEFT JOIN mdb_tablespace T ON (T.oid = I.reltablespace)
    WHERE C.relkind IN ('r', 'm') AND I.relkind = 'i';

CREATE VIEW mdb_stats WITH (security_barrier) AS
    SELECT
        nspname AS schemaname,
        relname AS tablename,
        attname AS attname,
        stainherit AS inherited,
        stanullfrac AS null_frac,
        stawidth AS avg_width,
        stadistinct AS n_distinct,
        CASE
            WHEN stakind1 = 1 THEN stavalues1
            WHEN stakind2 = 1 THEN stavalues2
            WHEN stakind3 = 1 THEN stavalues3
            WHEN stakind4 = 1 THEN stavalues4
            WHEN stakind5 = 1 THEN stavalues5
        END AS most_common_vals,
        CASE
            WHEN stakind1 = 1 THEN stanumbers1
            WHEN stakind2 = 1 THEN stanumbers2
            WHEN stakind3 = 1 THEN stanumbers3
            WHEN stakind4 = 1 THEN stanumbers4
            WHEN stakind5 = 1 THEN stanumbers5
        END AS most_common_freqs,
        CASE
            WHEN stakind1 = 2 THEN stavalues1
            WHEN stakind2 = 2 THEN stavalues2
            WHEN stakind3 = 2 THEN stavalues3
            WHEN stakind4 = 2 THEN stavalues4
            WHEN stakind5 = 2 THEN stavalues5
        END AS histogram_bounds,
        CASE
            WHEN stakind1 = 3 THEN stanumbers1[1]
            WHEN stakind2 = 3 THEN stanumbers2[1]
            WHEN stakind3 = 3 THEN stanumbers3[1]
            WHEN stakind4 = 3 THEN stanumbers4[1]
            WHEN stakind5 = 3 THEN stanumbers5[1]
        END AS correlation,
        CASE
            WHEN stakind1 = 4 THEN stavalues1
            WHEN stakind2 = 4 THEN stavalues2
            WHEN stakind3 = 4 THEN stavalues3
            WHEN stakind4 = 4 THEN stavalues4
            WHEN stakind5 = 4 THEN stavalues5
        END AS most_common_elems,
        CASE
            WHEN stakind1 = 4 THEN stanumbers1
            WHEN stakind2 = 4 THEN stanumbers2
            WHEN stakind3 = 4 THEN stanumbers3
            WHEN stakind4 = 4 THEN stanumbers4
            WHEN stakind5 = 4 THEN stanumbers5
        END AS most_common_elem_freqs,
        CASE
            WHEN stakind1 = 5 THEN stanumbers1
            WHEN stakind2 = 5 THEN stanumbers2
            WHEN stakind3 = 5 THEN stanumbers3
            WHEN stakind4 = 5 THEN stanumbers4
            WHEN stakind5 = 5 THEN stanumbers5
        END AS elem_count_histogram
    FROM mdb_statistic s JOIN mdb_class c ON (c.oid = s.starelid)
         JOIN mdb_attribute a ON (c.oid = attrelid AND attnum = s.staattnum)
         LEFT JOIN mdb_namespace n ON (n.oid = c.relnamespace)
    WHERE NOT attisdropped
    AND has_column_privilege(c.oid, a.attnum, 'select')
    AND (c.relrowsecurity = false OR NOT row_security_active(c.oid));

REVOKE ALL on mdb_statistic FROM public;

CREATE VIEW mdb_locks AS
    SELECT * FROM mdb_lock_status() AS L;

CREATE VIEW mdb_cursors AS
    SELECT * FROM mdb_cursor() AS C;

CREATE VIEW mdb_available_extensions AS
    SELECT E.name, E.default_version, X.extversion AS installed_version,
           E.comment
      FROM mdb_available_extensions() AS E
           LEFT JOIN mdb_extension AS X ON E.name = X.extname;

CREATE VIEW mdb_available_extension_versions AS
    SELECT E.name, E.version, (X.extname IS NOT NULL) AS installed,
           E.superuser, E.relocatable, E.schema, E.requires, E.comment
      FROM mdb_available_extension_versions() AS E
           LEFT JOIN mdb_extension AS X
             ON E.name = X.extname AND E.version = X.extversion;

CREATE VIEW mdb_prepared_xacts AS
    SELECT P.transaction, P.gid, P.prepared,
           U.rolname AS owner, D.datname AS database
    FROM mdb_prepared_xact() AS P
         LEFT JOIN mdb_authid U ON P.ownerid = U.oid
         LEFT JOIN mdb_database D ON P.dbid = D.oid;

CREATE VIEW mdb_prepared_statements AS
    SELECT * FROM mdb_prepared_statement() AS P;

CREATE VIEW mdb_seclabels AS
SELECT
	l.objoid, l.classoid, l.objsubid,
	CASE WHEN rel.relkind = 'r' THEN 'table'::text
		 WHEN rel.relkind = 'v' THEN 'view'::text
		 WHEN rel.relkind = 'm' THEN 'materialized view'::text
		 WHEN rel.relkind = 'S' THEN 'sequence'::text
		 WHEN rel.relkind = 'f' THEN 'foreign table'::text END AS objtype,
	rel.relnamespace AS objnamespace,
	CASE WHEN mdb_table_is_visible(rel.oid)
	     THEN quote_ident(rel.relname)
	     ELSE quote_ident(nsp.nspname) || '.' || quote_ident(rel.relname)
	     END AS objname,
	l.provider, l.label
FROM
	mdb_seclabel l
	JOIN mdb_class rel ON l.classoid = rel.tableoid AND l.objoid = rel.oid
	JOIN mdb_namespace nsp ON rel.relnamespace = nsp.oid
WHERE
	l.objsubid = 0
UNION ALL
SELECT
	l.objoid, l.classoid, l.objsubid,
	'column'::text AS objtype,
	rel.relnamespace AS objnamespace,
	CASE WHEN mdb_table_is_visible(rel.oid)
	     THEN quote_ident(rel.relname)
	     ELSE quote_ident(nsp.nspname) || '.' || quote_ident(rel.relname)
	     END || '.' || att.attname AS objname,
	l.provider, l.label
FROM
	mdb_seclabel l
	JOIN mdb_class rel ON l.classoid = rel.tableoid AND l.objoid = rel.oid
	JOIN mdb_attribute att
	     ON rel.oid = att.attrelid AND l.objsubid = att.attnum
	JOIN mdb_namespace nsp ON rel.relnamespace = nsp.oid
WHERE
	l.objsubid != 0
UNION ALL
SELECT
	l.objoid, l.classoid, l.objsubid,
	CASE WHEN pro.proisagg = true THEN 'aggregate'::text
	     WHEN pro.proisagg = false THEN 'function'::text
	END AS objtype,
	pro.pronamespace AS objnamespace,
	CASE WHEN mdb_function_is_visible(pro.oid)
	     THEN quote_ident(pro.proname)
	     ELSE quote_ident(nsp.nspname) || '.' || quote_ident(pro.proname)
	END || '(' || mdb_catalog.mdb_get_function_arguments(pro.oid) || ')' AS objname,
	l.provider, l.label
FROM
	mdb_seclabel l
	JOIN mdb_proc pro ON l.classoid = pro.tableoid AND l.objoid = pro.oid
	JOIN mdb_namespace nsp ON pro.pronamespace = nsp.oid
WHERE
	l.objsubid = 0
UNION ALL
SELECT
	l.objoid, l.classoid, l.objsubid,
	CASE WHEN typ.typtype = 'd' THEN 'domain'::text
	ELSE 'type'::text END AS objtype,
	typ.typnamespace AS objnamespace,
	CASE WHEN mdb_type_is_visible(typ.oid)
	THEN quote_ident(typ.typname)
	ELSE quote_ident(nsp.nspname) || '.' || quote_ident(typ.typname)
	END AS objname,
	l.provider, l.label
FROM
	mdb_seclabel l
	JOIN mdb_type typ ON l.classoid = typ.tableoid AND l.objoid = typ.oid
	JOIN mdb_namespace nsp ON typ.typnamespace = nsp.oid
WHERE
	l.objsubid = 0
UNION ALL
SELECT
	l.objoid, l.classoid, l.objsubid,
	'large object'::text AS objtype,
	NULL::oid AS objnamespace,
	l.objoid::text AS objname,
	l.provider, l.label
FROM
	mdb_seclabel l
	JOIN mdb_largeobject_metadata lom ON l.objoid = lom.oid
WHERE
	l.classoid = 'mdb_catalog.mdb_largeobject'::regclass AND l.objsubid = 0
UNION ALL
SELECT
	l.objoid, l.classoid, l.objsubid,
	'language'::text AS objtype,
	NULL::oid AS objnamespace,
	quote_ident(lan.lanname) AS objname,
	l.provider, l.label
FROM
	mdb_seclabel l
	JOIN mdb_language lan ON l.classoid = lan.tableoid AND l.objoid = lan.oid
WHERE
	l.objsubid = 0
UNION ALL
SELECT
	l.objoid, l.classoid, l.objsubid,
	'schema'::text AS objtype,
	nsp.oid AS objnamespace,
	quote_ident(nsp.nspname) AS objname,
	l.provider, l.label
FROM
	mdb_seclabel l
	JOIN mdb_namespace nsp ON l.classoid = nsp.tableoid AND l.objoid = nsp.oid
WHERE
	l.objsubid = 0
UNION ALL
SELECT
	l.objoid, l.classoid, l.objsubid,
	'event trigger'::text AS objtype,
	NULL::oid AS objnamespace,
	quote_ident(evt.evtname) AS objname,
	l.provider, l.label
FROM
	mdb_seclabel l
	JOIN mdb_event_trigger evt ON l.classoid = evt.tableoid
		AND l.objoid = evt.oid
WHERE
	l.objsubid = 0
UNION ALL
SELECT
	l.objoid, l.classoid, 0::int4 AS objsubid,
	'database'::text AS objtype,
	NULL::oid AS objnamespace,
	quote_ident(dat.datname) AS objname,
	l.provider, l.label
FROM
	mdb_shseclabel l
	JOIN mdb_database dat ON l.classoid = dat.tableoid AND l.objoid = dat.oid
UNION ALL
SELECT
	l.objoid, l.classoid, 0::int4 AS objsubid,
	'tablespace'::text AS objtype,
	NULL::oid AS objnamespace,
	quote_ident(spc.spcname) AS objname,
	l.provider, l.label
FROM
	mdb_shseclabel l
	JOIN mdb_tablespace spc ON l.classoid = spc.tableoid AND l.objoid = spc.oid
UNION ALL
SELECT
	l.objoid, l.classoid, 0::int4 AS objsubid,
	'role'::text AS objtype,
	NULL::oid AS objnamespace,
	quote_ident(rol.rolname) AS objname,
	l.provider, l.label
FROM
	mdb_shseclabel l
	JOIN mdb_authid rol ON l.classoid = rol.tableoid AND l.objoid = rol.oid;

CREATE VIEW mdb_settings AS
    SELECT * FROM mdb_show_all_settings() AS A;

CREATE RULE mdb_settings_u AS
    ON UPDATE TO mdb_settings
    WHERE new.name = old.name DO
    SELECT set_config(old.name, new.setting, 'f');

CREATE RULE mdb_settings_n AS
    ON UPDATE TO mdb_settings
    DO INSTEAD NOTHING;

GRANT SELECT, UPDATE ON mdb_settings TO PUBLIC;

CREATE VIEW mdb_file_settings AS
   SELECT * FROM mdb_show_all_file_settings() AS A;

REVOKE ALL on mdb_file_settings FROM PUBLIC;
REVOKE EXECUTE ON FUNCTION mdb_show_all_file_settings() FROM PUBLIC;

CREATE VIEW mdb_timezone_abbrevs AS
    SELECT * FROM mdb_timezone_abbrevs();

CREATE VIEW mdb_timezone_names AS
    SELECT * FROM mdb_timezone_names();

CREATE VIEW mdb_config AS
    SELECT * FROM mdb_config();

REVOKE ALL on mdb_config FROM PUBLIC;
REVOKE EXECUTE ON FUNCTION mdb_config() FROM PUBLIC;

-- Statistics views

CREATE VIEW mdb_stat_all_tables AS
    SELECT
            C.oid AS relid,
            N.nspname AS schemaname,
            C.relname AS relname,
            mdb_stat_get_numscans(C.oid) AS seq_scan,
            mdb_stat_get_tuples_returned(C.oid) AS seq_tup_read,
            sum(mdb_stat_get_numscans(I.indexrelid))::bigint AS idx_scan,
            sum(mdb_stat_get_tuples_fetched(I.indexrelid))::bigint +
            mdb_stat_get_tuples_fetched(C.oid) AS idx_tup_fetch,
            mdb_stat_get_tuples_inserted(C.oid) AS n_tup_ins,
            mdb_stat_get_tuples_updated(C.oid) AS n_tup_upd,
            mdb_stat_get_tuples_deleted(C.oid) AS n_tup_del,
            mdb_stat_get_tuples_hot_updated(C.oid) AS n_tup_hot_upd,
            mdb_stat_get_live_tuples(C.oid) AS n_live_tup,
            mdb_stat_get_dead_tuples(C.oid) AS n_dead_tup,
            mdb_stat_get_mod_since_analyze(C.oid) AS n_mod_since_analyze,
            mdb_stat_get_last_vacuum_time(C.oid) as last_vacuum,
            mdb_stat_get_last_autovacuum_time(C.oid) as last_autovacuum,
            mdb_stat_get_last_analyze_time(C.oid) as last_analyze,
            mdb_stat_get_last_autoanalyze_time(C.oid) as last_autoanalyze,
            mdb_stat_get_vacuum_count(C.oid) AS vacuum_count,
            mdb_stat_get_autovacuum_count(C.oid) AS autovacuum_count,
            mdb_stat_get_analyze_count(C.oid) AS analyze_count,
            mdb_stat_get_autoanalyze_count(C.oid) AS autoanalyze_count
    FROM mdb_class C LEFT JOIN
         mdb_index I ON C.oid = I.indrelid
         LEFT JOIN mdb_namespace N ON (N.oid = C.relnamespace)
    WHERE C.relkind IN ('r', 't', 'm')
    GROUP BY C.oid, N.nspname, C.relname;

CREATE VIEW mdb_stat_xact_all_tables AS
    SELECT
            C.oid AS relid,
            N.nspname AS schemaname,
            C.relname AS relname,
            mdb_stat_get_xact_numscans(C.oid) AS seq_scan,
            mdb_stat_get_xact_tuples_returned(C.oid) AS seq_tup_read,
            sum(mdb_stat_get_xact_numscans(I.indexrelid))::bigint AS idx_scan,
            sum(mdb_stat_get_xact_tuples_fetched(I.indexrelid))::bigint +
            mdb_stat_get_xact_tuples_fetched(C.oid) AS idx_tup_fetch,
            mdb_stat_get_xact_tuples_inserted(C.oid) AS n_tup_ins,
            mdb_stat_get_xact_tuples_updated(C.oid) AS n_tup_upd,
            mdb_stat_get_xact_tuples_deleted(C.oid) AS n_tup_del,
            mdb_stat_get_xact_tuples_hot_updated(C.oid) AS n_tup_hot_upd
    FROM mdb_class C LEFT JOIN
         mdb_index I ON C.oid = I.indrelid
         LEFT JOIN mdb_namespace N ON (N.oid = C.relnamespace)
    WHERE C.relkind IN ('r', 't', 'm')
    GROUP BY C.oid, N.nspname, C.relname;

CREATE VIEW mdb_stat_sys_tables AS
    SELECT * FROM mdb_stat_all_tables
    WHERE schemaname IN ('mdb_catalog', 'information_schema') OR
          schemaname ~ '^mdb_toast';

CREATE VIEW mdb_stat_xact_sys_tables AS
    SELECT * FROM mdb_stat_xact_all_tables
    WHERE schemaname IN ('mdb_catalog', 'information_schema') OR
          schemaname ~ '^mdb_toast';

CREATE VIEW mdb_stat_user_tables AS
    SELECT * FROM mdb_stat_all_tables
    WHERE schemaname NOT IN ('mdb_catalog', 'information_schema') AND
          schemaname !~ '^mdb_toast';

CREATE VIEW mdb_stat_xact_user_tables AS
    SELECT * FROM mdb_stat_xact_all_tables
    WHERE schemaname NOT IN ('mdb_catalog', 'information_schema') AND
          schemaname !~ '^mdb_toast';

CREATE VIEW mdb_statio_all_tables AS
    SELECT
            C.oid AS relid,
            N.nspname AS schemaname,
            C.relname AS relname,
            mdb_stat_get_blocks_fetched(C.oid) -
                    mdb_stat_get_blocks_hit(C.oid) AS heap_blks_read,
            mdb_stat_get_blocks_hit(C.oid) AS heap_blks_hit,
            sum(mdb_stat_get_blocks_fetched(I.indexrelid) -
                    mdb_stat_get_blocks_hit(I.indexrelid))::bigint AS idx_blks_read,
            sum(mdb_stat_get_blocks_hit(I.indexrelid))::bigint AS idx_blks_hit,
            mdb_stat_get_blocks_fetched(T.oid) -
                    mdb_stat_get_blocks_hit(T.oid) AS toast_blks_read,
            mdb_stat_get_blocks_hit(T.oid) AS toast_blks_hit,
            sum(mdb_stat_get_blocks_fetched(X.indexrelid) -
                    mdb_stat_get_blocks_hit(X.indexrelid))::bigint AS tidx_blks_read,
            sum(mdb_stat_get_blocks_hit(X.indexrelid))::bigint AS tidx_blks_hit
    FROM mdb_class C LEFT JOIN
            mdb_index I ON C.oid = I.indrelid LEFT JOIN
            mdb_class T ON C.reltoastrelid = T.oid LEFT JOIN
            mdb_index X ON T.oid = X.indrelid
            LEFT JOIN mdb_namespace N ON (N.oid = C.relnamespace)
    WHERE C.relkind IN ('r', 't', 'm')
    GROUP BY C.oid, N.nspname, C.relname, T.oid, X.indrelid;

CREATE VIEW mdb_statio_sys_tables AS
    SELECT * FROM mdb_statio_all_tables
    WHERE schemaname IN ('mdb_catalog', 'information_schema') OR
          schemaname ~ '^mdb_toast';

CREATE VIEW mdb_statio_user_tables AS
    SELECT * FROM mdb_statio_all_tables
    WHERE schemaname NOT IN ('mdb_catalog', 'information_schema') AND
          schemaname !~ '^mdb_toast';

CREATE VIEW mdb_stat_all_indexes AS
    SELECT
            C.oid AS relid,
            I.oid AS indexrelid,
            N.nspname AS schemaname,
            C.relname AS relname,
            I.relname AS indexrelname,
            mdb_stat_get_numscans(I.oid) AS idx_scan,
            mdb_stat_get_tuples_returned(I.oid) AS idx_tup_read,
            mdb_stat_get_tuples_fetched(I.oid) AS idx_tup_fetch
    FROM mdb_class C JOIN
            mdb_index X ON C.oid = X.indrelid JOIN
            mdb_class I ON I.oid = X.indexrelid
            LEFT JOIN mdb_namespace N ON (N.oid = C.relnamespace)
    WHERE C.relkind IN ('r', 't', 'm');

CREATE VIEW mdb_stat_sys_indexes AS
    SELECT * FROM mdb_stat_all_indexes
    WHERE schemaname IN ('mdb_catalog', 'information_schema') OR
          schemaname ~ '^mdb_toast';

CREATE VIEW mdb_stat_user_indexes AS
    SELECT * FROM mdb_stat_all_indexes
    WHERE schemaname NOT IN ('mdb_catalog', 'information_schema') AND
          schemaname !~ '^mdb_toast';

CREATE VIEW mdb_statio_all_indexes AS
    SELECT
            C.oid AS relid,
            I.oid AS indexrelid,
            N.nspname AS schemaname,
            C.relname AS relname,
            I.relname AS indexrelname,
            mdb_stat_get_blocks_fetched(I.oid) -
                    mdb_stat_get_blocks_hit(I.oid) AS idx_blks_read,
            mdb_stat_get_blocks_hit(I.oid) AS idx_blks_hit
    FROM mdb_class C JOIN
            mdb_index X ON C.oid = X.indrelid JOIN
            mdb_class I ON I.oid = X.indexrelid
            LEFT JOIN mdb_namespace N ON (N.oid = C.relnamespace)
    WHERE C.relkind IN ('r', 't', 'm');

CREATE VIEW mdb_statio_sys_indexes AS
    SELECT * FROM mdb_statio_all_indexes
    WHERE schemaname IN ('mdb_catalog', 'information_schema') OR
          schemaname ~ '^mdb_toast';

CREATE VIEW mdb_statio_user_indexes AS
    SELECT * FROM mdb_statio_all_indexes
    WHERE schemaname NOT IN ('mdb_catalog', 'information_schema') AND
          schemaname !~ '^mdb_toast';

CREATE VIEW mdb_statio_all_sequences AS
    SELECT
            C.oid AS relid,
            N.nspname AS schemaname,
            C.relname AS relname,
            mdb_stat_get_blocks_fetched(C.oid) -
                    mdb_stat_get_blocks_hit(C.oid) AS blks_read,
            mdb_stat_get_blocks_hit(C.oid) AS blks_hit
    FROM mdb_class C
            LEFT JOIN mdb_namespace N ON (N.oid = C.relnamespace)
    WHERE C.relkind = 'S';

CREATE VIEW mdb_statio_sys_sequences AS
    SELECT * FROM mdb_statio_all_sequences
    WHERE schemaname IN ('mdb_catalog', 'information_schema') OR
          schemaname ~ '^mdb_toast';

CREATE VIEW mdb_statio_user_sequences AS
    SELECT * FROM mdb_statio_all_sequences
    WHERE schemaname NOT IN ('mdb_catalog', 'information_schema') AND
          schemaname !~ '^mdb_toast';

CREATE VIEW mdb_stat_activity AS
    SELECT
            S.datid AS datid,
            D.datname AS datname,
            S.pid,
            S.usesysid,
            U.rolname AS usename,
            S.application_name,
            S.client_addr,
            S.client_hostname,
            S.client_port,
            S.backend_start,
            S.xact_start,
            S.query_start,
            S.state_change,
            S.wait_event_type,
            S.wait_event,
            S.state,
            S.backend_xid,
            s.backend_xmin,
            S.query
    FROM mdb_database D, mdb_stat_get_activity(NULL) AS S, mdb_authid U
    WHERE S.datid = D.oid AND
            S.usesysid = U.oid;

CREATE VIEW mdb_stat_replication AS
    SELECT
            S.pid,
            S.usesysid,
            U.rolname AS usename,
            S.application_name,
            S.client_addr,
            S.client_hostname,
            S.client_port,
            S.backend_start,
            S.backend_xmin,
            W.state,
            W.sent_location,
            W.write_location,
            W.flush_location,
            W.replay_location,
            W.sync_priority,
            W.sync_state
    FROM mdb_stat_get_activity(NULL) AS S, mdb_authid U,
            mdb_stat_get_wal_senders() AS W
    WHERE S.usesysid = U.oid AND
            S.pid = W.pid;

CREATE VIEW mdb_stat_wal_receiver AS
    SELECT
            s.pid,
            s.status,
            s.receive_start_lsn,
            s.receive_start_tli,
            s.received_lsn,
            s.received_tli,
            s.last_msg_send_time,
            s.last_msg_receipt_time,
            s.latest_end_lsn,
            s.latest_end_time,
            s.slot_name
    FROM mdb_stat_get_wal_receiver() s
    WHERE s.pid IS NOT NULL;

CREATE VIEW mdb_stat_ssl AS
    SELECT
            S.pid,
            S.ssl,
            S.sslversion AS version,
            S.sslcipher AS cipher,
            S.sslbits AS bits,
            S.sslcompression AS compression,
            S.sslclientdn AS clientdn
    FROM mdb_stat_get_activity(NULL) AS S;

CREATE VIEW mdb_replication_slots AS
    SELECT
            L.slot_name,
            L.plugin,
            L.slot_type,
            L.datoid,
            D.datname AS database,
            L.active,
            L.active_pid,
            L.xmin,
            L.catalog_xmin,
            L.restart_lsn,
            L.confirmed_flush_lsn
    FROM mdb_get_replication_slots() AS L
            LEFT JOIN mdb_database D ON (L.datoid = D.oid);

CREATE VIEW mdb_stat_database AS
    SELECT
            D.oid AS datid,
            D.datname AS datname,
            mdb_stat_get_db_numbackends(D.oid) AS numbackends,
            mdb_stat_get_db_xact_commit(D.oid) AS xact_commit,
            mdb_stat_get_db_xact_rollback(D.oid) AS xact_rollback,
            mdb_stat_get_db_blocks_fetched(D.oid) -
                    mdb_stat_get_db_blocks_hit(D.oid) AS blks_read,
            mdb_stat_get_db_blocks_hit(D.oid) AS blks_hit,
            mdb_stat_get_db_tuples_returned(D.oid) AS tup_returned,
            mdb_stat_get_db_tuples_fetched(D.oid) AS tup_fetched,
            mdb_stat_get_db_tuples_inserted(D.oid) AS tup_inserted,
            mdb_stat_get_db_tuples_updated(D.oid) AS tup_updated,
            mdb_stat_get_db_tuples_deleted(D.oid) AS tup_deleted,
            mdb_stat_get_db_conflict_all(D.oid) AS conflicts,
            mdb_stat_get_db_temp_files(D.oid) AS temp_files,
            mdb_stat_get_db_temp_bytes(D.oid) AS temp_bytes,
            mdb_stat_get_db_deadlocks(D.oid) AS deadlocks,
            mdb_stat_get_db_blk_read_time(D.oid) AS blk_read_time,
            mdb_stat_get_db_blk_write_time(D.oid) AS blk_write_time,
            mdb_stat_get_db_stat_reset_time(D.oid) AS stats_reset
    FROM mdb_database D;

CREATE VIEW mdb_stat_database_conflicts AS
    SELECT
            D.oid AS datid,
            D.datname AS datname,
            mdb_stat_get_db_conflict_tablespace(D.oid) AS confl_tablespace,
            mdb_stat_get_db_conflict_lock(D.oid) AS confl_lock,
            mdb_stat_get_db_conflict_snapshot(D.oid) AS confl_snapshot,
            mdb_stat_get_db_conflict_bufferpin(D.oid) AS confl_bufferpin,
            mdb_stat_get_db_conflict_startup_deadlock(D.oid) AS confl_deadlock
    FROM mdb_database D;

CREATE VIEW mdb_stat_user_functions AS
    SELECT
            P.oid AS funcid,
            N.nspname AS schemaname,
            P.proname AS funcname,
            mdb_stat_get_function_calls(P.oid) AS calls,
            mdb_stat_get_function_total_time(P.oid) AS total_time,
            mdb_stat_get_function_self_time(P.oid) AS self_time
    FROM mdb_proc P LEFT JOIN mdb_namespace N ON (N.oid = P.pronamespace)
    WHERE P.prolang != 12  -- fast check to eliminate built-in functions
          AND mdb_stat_get_function_calls(P.oid) IS NOT NULL;

CREATE VIEW mdb_stat_xact_user_functions AS
    SELECT
            P.oid AS funcid,
            N.nspname AS schemaname,
            P.proname AS funcname,
            mdb_stat_get_xact_function_calls(P.oid) AS calls,
            mdb_stat_get_xact_function_total_time(P.oid) AS total_time,
            mdb_stat_get_xact_function_self_time(P.oid) AS self_time
    FROM mdb_proc P LEFT JOIN mdb_namespace N ON (N.oid = P.pronamespace)
    WHERE P.prolang != 12  -- fast check to eliminate built-in functions
          AND mdb_stat_get_xact_function_calls(P.oid) IS NOT NULL;

CREATE VIEW mdb_stat_archiver AS
    SELECT
        s.archived_count,
        s.last_archived_wal,
        s.last_archived_time,
        s.failed_count,
        s.last_failed_wal,
        s.last_failed_time,
        s.stats_reset
    FROM mdb_stat_get_archiver() s;

CREATE VIEW mdb_stat_bgwriter AS
    SELECT
        mdb_stat_get_bgwriter_timed_checkpoints() AS checkpoints_timed,
        mdb_stat_get_bgwriter_requested_checkpoints() AS checkpoints_req,
        mdb_stat_get_checkpoint_write_time() AS checkpoint_write_time,
        mdb_stat_get_checkpoint_sync_time() AS checkpoint_sync_time,
        mdb_stat_get_bgwriter_buf_written_checkpoints() AS buffers_checkpoint,
        mdb_stat_get_bgwriter_buf_written_clean() AS buffers_clean,
        mdb_stat_get_bgwriter_maxwritten_clean() AS maxwritten_clean,
        mdb_stat_get_buf_written_backend() AS buffers_backend,
        mdb_stat_get_buf_fsync_backend() AS buffers_backend_fsync,
        mdb_stat_get_buf_alloc() AS buffers_alloc,
        mdb_stat_get_bgwriter_stat_reset_time() AS stats_reset;

CREATE VIEW mdb_stat_progress_vacuum AS
	SELECT
		S.pid AS pid, S.datid AS datid, D.datname AS datname,
		S.relid AS relid,
		CASE S.param1 WHEN 0 THEN 'initializing'
					  WHEN 1 THEN 'scanning heap'
					  WHEN 2 THEN 'vacuuming indexes'
					  WHEN 3 THEN 'vacuuming heap'
					  WHEN 4 THEN 'cleaning up indexes'
					  WHEN 5 THEN 'truncating heap'
					  WHEN 6 THEN 'performing final cleanup'
					  END AS phase,
		S.param2 AS heap_blks_total, S.param3 AS heap_blks_scanned,
		S.param4 AS heap_blks_vacuumed, S.param5 AS index_vacuum_count,
		S.param6 AS max_dead_tuples, S.param7 AS num_dead_tuples
    FROM mdb_stat_get_progress_info('VACUUM') AS S
		 JOIN mdb_database D ON S.datid = D.oid;

CREATE VIEW mdb_user_mappings AS
    SELECT
        U.oid       AS umid,
        S.oid       AS srvid,
        S.srvname   AS srvname,
        U.umuser    AS umuser,
        CASE WHEN U.umuser = 0 THEN
            'public'
        ELSE
            A.rolname
        END AS usename,
        CASE WHEN mdb_has_role(S.srvowner, 'USAGE') OR has_server_privilege(S.oid, 'USAGE') THEN
            U.umoptions
        ELSE
            NULL
        END AS umoptions
    FROM mdb_user_mapping U
         LEFT JOIN mdb_authid A ON (A.oid = U.umuser) JOIN
        mdb_foreign_server S ON (U.umserver = S.oid);

REVOKE ALL on mdb_user_mapping FROM public;


CREATE VIEW mdb_replication_origin_status AS
    SELECT *
    FROM mdb_show_replication_origin_status();

REVOKE ALL ON mdb_replication_origin_status FROM public;

--
-- We have a few function definitions in here, too.
-- At some point there might be enough to justify breaking them out into
-- a separate "system_functions.sql" file.
--

-- Tsearch debug function.  Defined here because it'd be pretty unwieldy
-- to put it into mdb_proc.h

CREATE FUNCTION ts_debug(IN config regconfig, IN document text,
    OUT alias text,
    OUT description text,
    OUT token text,
    OUT dictionaries regdictionary[],
    OUT dictionary regdictionary,
    OUT lexemes text[])
RETURNS SETOF record AS
$$
SELECT
    tt.alias AS alias,
    tt.description AS description,
    parse.token AS token,
    ARRAY ( SELECT m.mapdict::mdb_catalog.regdictionary
            FROM mdb_catalog.mdb_ts_config_map AS m
            WHERE m.mapcfg = $1 AND m.maptokentype = parse.tokid
            ORDER BY m.mapseqno )
    AS dictionaries,
    ( SELECT mapdict::mdb_catalog.regdictionary
      FROM mdb_catalog.mdb_ts_config_map AS m
      WHERE m.mapcfg = $1 AND m.maptokentype = parse.tokid
      ORDER BY mdb_catalog.ts_lexize(mapdict, parse.token) IS NULL, m.mapseqno
      LIMIT 1
    ) AS dictionary,
    ( SELECT mdb_catalog.ts_lexize(mapdict, parse.token)
      FROM mdb_catalog.mdb_ts_config_map AS m
      WHERE m.mapcfg = $1 AND m.maptokentype = parse.tokid
      ORDER BY mdb_catalog.ts_lexize(mapdict, parse.token) IS NULL, m.mapseqno
      LIMIT 1
    ) AS lexemes
FROM mdb_catalog.ts_parse(
        (SELECT cfgparser FROM mdb_catalog.mdb_ts_config WHERE oid = $1 ), $2
    ) AS parse,
     mdb_catalog.ts_token_type(
        (SELECT cfgparser FROM mdb_catalog.mdb_ts_config WHERE oid = $1 )
    ) AS tt
WHERE tt.tokid = parse.tokid
$$
LANGUAGE SQL STRICT STABLE PARALLEL SAFE;

COMMENT ON FUNCTION ts_debug(regconfig,text) IS
    'debug function for text search configuration';

CREATE FUNCTION ts_debug(IN document text,
    OUT alias text,
    OUT description text,
    OUT token text,
    OUT dictionaries regdictionary[],
    OUT dictionary regdictionary,
    OUT lexemes text[])
RETURNS SETOF record AS
$$
    SELECT * FROM mdb_catalog.ts_debug( mdb_catalog.get_current_ts_config(), $1);
$$
LANGUAGE SQL STRICT STABLE PARALLEL SAFE;

COMMENT ON FUNCTION ts_debug(text) IS
    'debug function for current text search configuration';

--
-- Redeclare built-in functions that need default values attached to their
-- arguments.  It's impractical to set those up directly in mdb_proc.h because
-- of the complexity and platform-dependency of the expression tree
-- representation.  (Note that internal functions still have to have entries
-- in mdb_proc.h; we are merely causing their proargnames and proargdefaults
-- to get filled in.)
--

CREATE OR REPLACE FUNCTION
  mdb_start_backup(label text, fast boolean DEFAULT false, exclusive boolean DEFAULT true)
  RETURNS mdb_lsn STRICT VOLATILE LANGUAGE internal AS 'mdb_start_backup'
  PARALLEL RESTRICTED;

-- legacy definition for compatibility with 9.3
CREATE OR REPLACE FUNCTION
  json_populate_record(base anyelement, from_json json, use_json_as_text boolean DEFAULT false)
  RETURNS anyelement LANGUAGE internal STABLE AS 'json_populate_record' PARALLEL SAFE;

-- legacy definition for compatibility with 9.3
CREATE OR REPLACE FUNCTION
  json_populate_recordset(base anyelement, from_json json, use_json_as_text boolean DEFAULT false)
  RETURNS SETOF anyelement LANGUAGE internal STABLE ROWS 100  AS 'json_populate_recordset' PARALLEL SAFE;

CREATE OR REPLACE FUNCTION mdb_logical_slot_get_changes(
    IN slot_name name, IN upto_lsn mdb_lsn, IN upto_nchanges int, VARIADIC options text[] DEFAULT '{}',
    OUT location mdb_lsn, OUT xid xid, OUT data text)
RETURNS SETOF RECORD
LANGUAGE INTERNAL
VOLATILE ROWS 1000 COST 1000
AS 'mdb_logical_slot_get_changes';

CREATE OR REPLACE FUNCTION mdb_logical_slot_peek_changes(
    IN slot_name name, IN upto_lsn mdb_lsn, IN upto_nchanges int, VARIADIC options text[] DEFAULT '{}',
    OUT location mdb_lsn, OUT xid xid, OUT data text)
RETURNS SETOF RECORD
LANGUAGE INTERNAL
VOLATILE ROWS 1000 COST 1000
AS 'mdb_logical_slot_peek_changes';

CREATE OR REPLACE FUNCTION mdb_logical_slot_get_binary_changes(
    IN slot_name name, IN upto_lsn mdb_lsn, IN upto_nchanges int, VARIADIC options text[] DEFAULT '{}',
    OUT location mdb_lsn, OUT xid xid, OUT data bytea)
RETURNS SETOF RECORD
LANGUAGE INTERNAL
VOLATILE ROWS 1000 COST 1000
AS 'mdb_logical_slot_get_binary_changes';

CREATE OR REPLACE FUNCTION mdb_logical_slot_peek_binary_changes(
    IN slot_name name, IN upto_lsn mdb_lsn, IN upto_nchanges int, VARIADIC options text[] DEFAULT '{}',
    OUT location mdb_lsn, OUT xid xid, OUT data bytea)
RETURNS SETOF RECORD
LANGUAGE INTERNAL
VOLATILE ROWS 1000 COST 1000
AS 'mdb_logical_slot_peek_binary_changes';

CREATE OR REPLACE FUNCTION mdb_create_physical_replication_slot(
    IN slot_name name, IN immediately_reserve boolean DEFAULT false,
    OUT slot_name name, OUT xlog_position mdb_lsn)
RETURNS RECORD
LANGUAGE INTERNAL
STRICT VOLATILE
AS 'mdb_create_physical_replication_slot';

CREATE OR REPLACE FUNCTION
  make_interval(years int4 DEFAULT 0, months int4 DEFAULT 0, weeks int4 DEFAULT 0,
                days int4 DEFAULT 0, hours int4 DEFAULT 0, mins int4 DEFAULT 0,
                secs double precision DEFAULT 0.0)
RETURNS interval
LANGUAGE INTERNAL
STRICT IMMUTABLE PARALLEL SAFE
AS 'make_interval';

CREATE OR REPLACE FUNCTION
  jsonb_set(jsonb_in jsonb, path text[] , replacement jsonb,
            create_if_missing boolean DEFAULT true)
RETURNS jsonb
LANGUAGE INTERNAL
STRICT IMMUTABLE PARALLEL SAFE
AS 'jsonb_set';

CREATE OR REPLACE FUNCTION
  parse_ident(str text, strict boolean DEFAULT true)
RETURNS text[]
LANGUAGE INTERNAL
STRICT IMMUTABLE PARALLEL SAFE
AS 'parse_ident';

CREATE OR REPLACE FUNCTION
  jsonb_insert(jsonb_in jsonb, path text[] , replacement jsonb,
            insert_after boolean DEFAULT false)
RETURNS jsonb
LANGUAGE INTERNAL
STRICT IMMUTABLE PARALLEL SAFE
AS 'jsonb_insert';

-- The default permissions for functions mean that anyone can execute them.
-- A number of functions shouldn't be executable by just anyone, but rather
-- than use explicit 'superuser()' checks in those functions, we use the GRANT
-- system to REVOKE access to those functions at initdb time.  Administrators
-- can later change who can access these functions, or leave them as only
-- available to superuser / cluster owner, if they choose.
REVOKE EXECUTE ON FUNCTION mdb_start_backup(text, boolean, boolean) FROM public;
REVOKE EXECUTE ON FUNCTION mdb_stop_backup() FROM public;
REVOKE EXECUTE ON FUNCTION mdb_stop_backup(boolean) FROM public;
REVOKE EXECUTE ON FUNCTION mdb_create_restore_point(text) FROM public;
REVOKE EXECUTE ON FUNCTION mdb_switch_xlog() FROM public;
REVOKE EXECUTE ON FUNCTION mdb_xlog_replay_pause() FROM public;
REVOKE EXECUTE ON FUNCTION mdb_xlog_replay_resume() FROM public;
REVOKE EXECUTE ON FUNCTION mdb_rotate_logfile() FROM public;
REVOKE EXECUTE ON FUNCTION mdb_reload_conf() FROM public;

REVOKE EXECUTE ON FUNCTION mdb_stat_reset() FROM public;
REVOKE EXECUTE ON FUNCTION mdb_stat_reset_shared(text) FROM public;
REVOKE EXECUTE ON FUNCTION mdb_stat_reset_single_table_counters(oid) FROM public;
REVOKE EXECUTE ON FUNCTION mdb_stat_reset_single_function_counters(oid) FROM public;
