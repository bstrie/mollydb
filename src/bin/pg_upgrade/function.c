/*
 *	function.c
 *
 *	server-side function support
 *
 *	Copyright (c) 2010-2016, MollyDB Global Development Group
 *	src/bin/mdb_upgrade/function.c
 */

#include "mollydb_fe.h"

#include "mdb_upgrade.h"

#include "access/transam.h"


/*
 * get_loadable_libraries()
 *
 *	Fetch the names of all old libraries containing C-language functions.
 *	We will later check that they all exist in the new installation.
 */
void
get_loadable_libraries(void)
{
	PGresult  **ress;
	int			totaltups;
	int			dbnum;
	bool		found_public_plpython_handler = false;

	ress = (PGresult **) mdb_malloc(old_cluster.dbarr.ndbs * sizeof(PGresult *));
	totaltups = 0;

	/* Fetch all library names, removing duplicates within each DB */
	for (dbnum = 0; dbnum < old_cluster.dbarr.ndbs; dbnum++)
	{
		DbInfo	   *active_db = &old_cluster.dbarr.dbs[dbnum];
		PGconn	   *conn = connectToServer(&old_cluster, active_db->db_name);

		/*
		 * Fetch all libraries referenced in this DB.  We can't exclude the
		 * "mdb_catalog" schema because, while such functions are not
		 * explicitly dumped by mdb_dump, they do reference implicit objects
		 * that mdb_dump does dump, e.g. CREATE LANGUAGE plperl.
		 */
		ress[dbnum] = executeQueryOrDie(conn,
										"SELECT DISTINCT probin "
										"FROM	mdb_catalog.mdb_proc "
										"WHERE	prolang = 13 /* C */ AND "
										"probin IS NOT NULL AND "
										"oid >= %u;",
										FirstNormalObjectId);
		totaltups += PQntuples(ress[dbnum]);

		/*
		 * Systems that install plpython before 8.1 have
		 * plpython_call_handler() defined in the "public" schema, causing
		 * mdb_dump to dump it.  However that function still references
		 * "plpython" (no "2"), so it throws an error on restore.  This code
		 * checks for the problem function, reports affected databases to the
		 * user and explains how to remove them. 8.1 git commit:
		 * e0dedd0559f005d60c69c9772163e69c204bac69
		 * http://archives.mollydb.org/mdb-hackers/2012-03/msg01101.php
		 * http://archives.mollydb.org/mdb-bugs/2012-05/msg00206.php
		 */
		if (GET_MAJOR_VERSION(old_cluster.major_version) < 901)
		{
			PGresult   *res;

			res = executeQueryOrDie(conn,
									"SELECT 1 "
						   "FROM	mdb_catalog.mdb_proc JOIN mdb_namespace "
							 "		ON pronamespace = mdb_namespace.oid "
							   "WHERE proname = 'plpython_call_handler' AND "
									"nspname = 'public' AND "
									"prolang = 13 /* C */ AND "
									"probin = '$libdir/plpython' AND "
									"mdb_proc.oid >= %u;",
									FirstNormalObjectId);
			if (PQntuples(res) > 0)
			{
				if (!found_public_plpython_handler)
				{
					mdb_log(MDB_WARNING,
						   "\nThe old cluster has a \"plpython_call_handler\" function defined\n"
						   "in the \"public\" schema which is a duplicate of the one defined\n"
						   "in the \"mdb_catalog\" schema.  You can confirm this by executing\n"
						   "in psql:\n"
						   "\n"
						   "    \\df *.plpython_call_handler\n"
						   "\n"
						   "The \"public\" schema version of this function was created by a\n"
						   "pre-8.1 install of plpython, and must be removed for mdb_upgrade\n"
						   "to complete because it references a now-obsolete \"plpython\"\n"
						   "shared object file.  You can remove the \"public\" schema version\n"
					   "of this function by running the following command:\n"
						   "\n"
						 "    DROP FUNCTION public.plpython_call_handler()\n"
						   "\n"
						   "in each affected database:\n"
						   "\n");
				}
				mdb_log(MDB_WARNING, "    %s\n", active_db->db_name);
				found_public_plpython_handler = true;
			}
			PQclear(res);
		}

		PQfinish(conn);
	}

	if (found_public_plpython_handler)
		mdb_fatal("Remove the problem functions from the old cluster to continue.\n");

	/* Allocate what's certainly enough space */
	os_info.libraries = (char **) mdb_malloc(totaltups * sizeof(char *));

	/*
	 * Now remove duplicates across DBs.  This is pretty inefficient code, but
	 * there probably aren't enough entries to matter.
	 */
	totaltups = 0;

	for (dbnum = 0; dbnum < old_cluster.dbarr.ndbs; dbnum++)
	{
		PGresult   *res = ress[dbnum];
		int			ntups;
		int			rowno;

		ntups = PQntuples(res);
		for (rowno = 0; rowno < ntups; rowno++)
		{
			char	   *lib = PQgetvalue(res, rowno, 0);
			bool		dup = false;
			int			n;

			for (n = 0; n < totaltups; n++)
			{
				if (strcmp(lib, os_info.libraries[n]) == 0)
				{
					dup = true;
					break;
				}
			}
			if (!dup)
				os_info.libraries[totaltups++] = mdb_strdup(lib);
		}

		PQclear(res);
	}

	os_info.num_libraries = totaltups;

	mdb_free(ress);
}


/*
 * check_loadable_libraries()
 *
 *	Check that the new cluster contains all required libraries.
 *	We do this by actually trying to LOAD each one, thereby testing
 *	compatibility as well as presence.
 */
void
check_loadable_libraries(void)
{
	PGconn	   *conn = connectToServer(&new_cluster, "template1");
	int			libnum;
	FILE	   *script = NULL;
	bool		found = false;
	char		output_path[MAXPGPATH];

	prep_status("Checking for presence of required libraries");

	snprintf(output_path, sizeof(output_path), "loadable_libraries.txt");

	for (libnum = 0; libnum < os_info.num_libraries; libnum++)
	{
		char	   *lib = os_info.libraries[libnum];
		int			llen = strlen(lib);
		char		cmd[7 + 2 * MAXPGPATH + 1];
		PGresult   *res;

		/*
		 * In MollyDB 9.0, Python 3 support was added, and to do that, a
		 * plpython2u language was created with library name plpython2.so as a
		 * symbolic link to plpython.so.  In MollyDB 9.1, only the
		 * plpython2.so library was created, and both plpythonu and plpython2u
		 * pointing to it.  For this reason, any reference to library name
		 * "plpython" in an old PG <= 9.1 cluster must look for "plpython2" in
		 * the new cluster.
		 *
		 * For this case, we could check mdb_pltemplate, but that only works
		 * for languages, and does not help with function shared objects, so
		 * we just do a general fix.
		 */
		if (GET_MAJOR_VERSION(old_cluster.major_version) < 901 &&
			strcmp(lib, "$libdir/plpython") == 0)
		{
			lib = "$libdir/plpython2";
			llen = strlen(lib);
		}

		strcpy(cmd, "LOAD '");
		PQescapeStringConn(conn, cmd + strlen(cmd), lib, llen, NULL);
		strcat(cmd, "'");

		res = PQexec(conn, cmd);

		if (PQresultStatus(res) != PGRES_COMMAND_OK)
		{
			found = true;

			if (script == NULL && (script = fopen_priv(output_path, "w")) == NULL)
				mdb_fatal("Could not open file \"%s\": %s\n",
						 output_path, getErrorText());
			fprintf(script, "Could not load library \"%s\"\n%s\n",
					lib,
					PQerrorMessage(conn));
		}

		PQclear(res);
	}

	PQfinish(conn);

	if (found)
	{
		fclose(script);
		mdb_log(MDB_REPORT, "fatal\n");
		mdb_fatal("Your installation references loadable libraries that are missing from the\n"
				 "new installation.  You can add these libraries to the new installation,\n"
				 "or remove the functions using them from the old installation.  A list of\n"
				 "problem libraries is in the file:\n"
				 "    %s\n\n", output_path);
	}
	else
		check_ok();
}
