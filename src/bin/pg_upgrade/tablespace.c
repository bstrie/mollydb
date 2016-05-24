/*
 *	tablespace.c
 *
 *	tablespace functions
 *
 *	Copyright (c) 2010-2016, MollyDB Global Development Group
 *	src/bin/mdb_upgrade/tablespace.c
 */

#include "mollydb_fe.h"

#include "mdb_upgrade.h"

#include <sys/types.h>

static void get_tablespace_paths(void);
static void set_tablespace_directory_suffix(ClusterInfo *cluster);


void
init_tablespaces(void)
{
	get_tablespace_paths();

	set_tablespace_directory_suffix(&old_cluster);
	set_tablespace_directory_suffix(&new_cluster);

	if (os_info.num_old_tablespaces > 0 &&
	strcmp(old_cluster.tablespace_suffix, new_cluster.tablespace_suffix) == 0)
		mdb_fatal("Cannot upgrade to/from the same system catalog version when\n"
				 "using tablespaces.\n");
}


/*
 * get_tablespace_paths()
 *
 * Scans mdb_tablespace and returns a malloc'ed array of all tablespace
 * paths. Its the caller's responsibility to free the array.
 */
static void
get_tablespace_paths(void)
{
	PGconn	   *conn = connectToServer(&old_cluster, "template1");
	PGresult   *res;
	int			tblnum;
	int			i_spclocation;
	char		query[QUERY_ALLOC];

	snprintf(query, sizeof(query),
			 "SELECT	%s "
			 "FROM	mdb_catalog.mdb_tablespace "
			 "WHERE	spcname != 'mdb_default' AND "
			 "		spcname != 'mdb_global'",
	/* 9.2 removed the spclocation column */
			 (GET_MAJOR_VERSION(old_cluster.major_version) <= 901) ?
	"spclocation" : "mdb_catalog.mdb_tablespace_location(oid) AS spclocation");

	res = executeQueryOrDie(conn, "%s", query);

	if ((os_info.num_old_tablespaces = PQntuples(res)) != 0)
		os_info.old_tablespaces = (char **) mdb_malloc(
							   os_info.num_old_tablespaces * sizeof(char *));
	else
		os_info.old_tablespaces = NULL;

	i_spclocation = PQfnumber(res, "spclocation");

	for (tblnum = 0; tblnum < os_info.num_old_tablespaces; tblnum++)
	{
		struct stat statBuf;

		os_info.old_tablespaces[tblnum] = mdb_strdup(
									 PQgetvalue(res, tblnum, i_spclocation));

		/*
		 * Check that the tablespace path exists and is a directory.
		 * Effectively, this is checking only for tables/indexes in
		 * non-existent tablespace directories.  Databases located in
		 * non-existent tablespaces already throw a backend error.
		 * Non-existent tablespace directories can occur when a data directory
		 * that contains user tablespaces is moved as part of mdb_upgrade
		 * preparation and the symbolic links are not updated.
		 */
		if (stat(os_info.old_tablespaces[tblnum], &statBuf) != 0)
		{
			if (errno == ENOENT)
				report_status(MDB_FATAL,
							  "tablespace directory \"%s\" does not exist\n",
							  os_info.old_tablespaces[tblnum]);
			else
				report_status(MDB_FATAL,
						   "cannot stat() tablespace directory \"%s\": %s\n",
					   os_info.old_tablespaces[tblnum], getErrorText());
		}
		if (!S_ISDIR(statBuf.st_mode))
			report_status(MDB_FATAL,
						  "tablespace path \"%s\" is not a directory\n",
						  os_info.old_tablespaces[tblnum]);
	}

	PQclear(res);

	PQfinish(conn);

	return;
}


static void
set_tablespace_directory_suffix(ClusterInfo *cluster)
{
	if (GET_MAJOR_VERSION(cluster->major_version) <= 804)
		cluster->tablespace_suffix = mdb_strdup("");
	else
	{
		/* This cluster has a version-specific subdirectory */

		/* The leading slash is needed to start a new directory. */
		cluster->tablespace_suffix = psprintf("/MDB_%s_%d",
											  cluster->major_version_str,
											  cluster->controldata.cat_ver);
	}
}
