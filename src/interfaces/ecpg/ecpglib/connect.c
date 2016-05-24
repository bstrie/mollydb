/* src/interfaces/ecpg/ecpglib/connect.c */

#define POSTGRES_ECMDB_INTERNAL
#include "mollydb_fe.h"

#include "ecpg-pthread-win32.h"
#include "ecpgtype.h"
#include "ecpglib.h"
#include "ecpgerrno.h"
#include "extern.h"
#include "sqlca.h"

#ifdef ENABLE_THREAD_SAFETY
static pthread_mutex_t connections_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_key_t actual_connection_key;
static pthread_once_t actual_connection_key_once = PTHREAD_ONCE_INIT;
#endif
static struct connection *actual_connection = NULL;
static struct connection *all_connections = NULL;

#ifdef ENABLE_THREAD_SAFETY
static void
ecmdb_actual_connection_init(void)
{
	pthread_key_create(&actual_connection_key, NULL);
}

void
ecmdb_pthreads_init(void)
{
	pthread_once(&actual_connection_key_once, ecmdb_actual_connection_init);
}
#endif

static struct connection *
ecmdb_get_connection_nr(const char *connection_name)
{
	struct connection *ret = NULL;

	if ((connection_name == NULL) || (strcmp(connection_name, "CURRENT") == 0))
	{
#ifdef ENABLE_THREAD_SAFETY
		ret = pthread_getspecific(actual_connection_key);

		/*
		 * if no connection in TSD for this thread, get the global default
		 * connection and hope the user knows what they're doing (i.e. using
		 * their own mutex to protect that connection from concurrent accesses
		 */
		/* if !ret then  we  got the connection from TSD */
		if (NULL == ret)
			/* no TSD connection, going for global */
			ret = actual_connection;
#else
		ret = actual_connection;
#endif
	}
	else
	{
		struct connection *con;

		for (con = all_connections; con != NULL; con = con->next)
		{
			if (strcmp(connection_name, con->name) == 0)
				break;
		}
		ret = con;
	}

	return (ret);
}

struct connection *
ecmdb_get_connection(const char *connection_name)
{
	struct connection *ret = NULL;

	if ((connection_name == NULL) || (strcmp(connection_name, "CURRENT") == 0))
	{
#ifdef ENABLE_THREAD_SAFETY
		ret = pthread_getspecific(actual_connection_key);

		/*
		 * if no connection in TSD for this thread, get the global default
		 * connection and hope the user knows what they're doing (i.e. using
		 * their own mutex to protect that connection from concurrent accesses
		 */
		/* if !ret then  we  got the connection from TSD */
		if (NULL == ret)
			/* no TSD connection here either, using global */
			ret = actual_connection;
#else
		ret = actual_connection;
#endif
	}
	else
	{
#ifdef ENABLE_THREAD_SAFETY
		pthread_mutex_lock(&connections_mutex);
#endif

		ret = ecmdb_get_connection_nr(connection_name);

#ifdef ENABLE_THREAD_SAFETY
		pthread_mutex_unlock(&connections_mutex);
#endif
	}

	return (ret);
}

static void
ecmdb_finish(struct connection * act)
{
	if (act != NULL)
	{
		struct ECPGtype_information_cache *cache,
				   *ptr;

		ecmdb_deallocate_all_conn(0, ECMDB_COMPAT_PGSQL, act);
		PQfinish(act->connection);

		/*
		 * no need to lock connections_mutex - we're always called by
		 * ECPGdisconnect or ECPGconnect, which are holding the lock
		 */

		/* remove act from the list */
		if (act == all_connections)
			all_connections = act->next;
		else
		{
			struct connection *con;

			for (con = all_connections; con->next && con->next != act; con = con->next);
			if (con->next)
				con->next = act->next;
		}

#ifdef ENABLE_THREAD_SAFETY
		if (pthread_getspecific(actual_connection_key) == act)
			pthread_setspecific(actual_connection_key, all_connections);
#endif
		if (actual_connection == act)
			actual_connection = all_connections;

		ecmdb_log("ecmdb_finish: connection %s closed\n", act->name ? act->name : "(null)");

		for (cache = act->cache_head; cache; ptr = cache, cache = cache->next, ecmdb_free(ptr));
		ecmdb_free(act->name);
		ecmdb_free(act);
		/* delete cursor variables when last connection gets closed */
		if (all_connections == NULL)
		{
			struct var_list *iv_ptr;

			for (; ivlist; iv_ptr = ivlist, ivlist = ivlist->next, ecmdb_free(iv_ptr));
		}
	}
	else
		ecmdb_log("ecmdb_finish: called an extra time\n");
}

bool
ECPGsetcommit(int lineno, const char *mode, const char *connection_name)
{
	struct connection *con = ecmdb_get_connection(connection_name);
	PGresult   *results;

	if (!ecmdb_init(con, connection_name, lineno))
		return (false);

	ecmdb_log("ECPGsetcommit on line %d: action \"%s\"; connection \"%s\"\n", lineno, mode, con->name);

	if (con->autocommit && strncmp(mode, "off", strlen("off")) == 0)
	{
		if (PQtransactionStatus(con->connection) == PQTRANS_IDLE)
		{
			results = PQexec(con->connection, "begin transaction");
			if (!ecmdb_check_PQresult(results, lineno, con->connection, ECMDB_COMPAT_PGSQL))
				return false;
			PQclear(results);
		}
		con->autocommit = false;
	}
	else if (!con->autocommit && strncmp(mode, "on", strlen("on")) == 0)
	{
		if (PQtransactionStatus(con->connection) != PQTRANS_IDLE)
		{
			results = PQexec(con->connection, "commit");
			if (!ecmdb_check_PQresult(results, lineno, con->connection, ECMDB_COMPAT_PGSQL))
				return false;
			PQclear(results);
		}
		con->autocommit = true;
	}

	return true;
}

bool
ECPGsetconn(int lineno, const char *connection_name)
{
	struct connection *con = ecmdb_get_connection(connection_name);

	if (!ecmdb_init(con, connection_name, lineno))
		return (false);

#ifdef ENABLE_THREAD_SAFETY
	pthread_setspecific(actual_connection_key, con);
#else
	actual_connection = con;
#endif
	return true;
}


static void
ECPGnoticeReceiver(void *arg, const PGresult *result)
{
	char	   *sqlstate = PQresultErrorField(result, MDB_DIAG_SQLSTATE);
	char	   *message = PQresultErrorField(result, MDB_DIAG_MESSAGE_PRIMARY);
	struct sqlca_t *sqlca = ECPGget_sqlca();
	int			sqlcode;

	if (sqlca == NULL)
	{
		ecmdb_log("out of memory");
		return;
	}

	(void) arg;					/* keep the compiler quiet */
	if (sqlstate == NULL)
		sqlstate = ECMDB_SQLSTATE_ECMDB_INTERNAL_ERROR;

	if (message == NULL)		/* Shouldn't happen, but need to be sure */
		message = ecmdb_gettext("empty message text");

	/* these are not warnings */
	if (strncmp(sqlstate, "00", 2) == 0)
		return;

	ecmdb_log("ECPGnoticeReceiver: %s\n", message);

	/* map to SQLCODE for backward compatibility */
	if (strcmp(sqlstate, ECMDB_SQLSTATE_INVALID_CURSOR_NAME) == 0)
		sqlcode = ECMDB_WARNING_UNKNOWN_PORTAL;
	else if (strcmp(sqlstate, ECMDB_SQLSTATE_ACTIVE_SQL_TRANSACTION) == 0)
		sqlcode = ECMDB_WARNING_IN_TRANSACTION;
	else if (strcmp(sqlstate, ECMDB_SQLSTATE_NO_ACTIVE_SQL_TRANSACTION) == 0)
		sqlcode = ECMDB_WARNING_NO_TRANSACTION;
	else if (strcmp(sqlstate, ECMDB_SQLSTATE_DUPLICATE_CURSOR) == 0)
		sqlcode = ECMDB_WARNING_PORTAL_EXISTS;
	else
		sqlcode = 0;

	strncpy(sqlca->sqlstate, sqlstate, sizeof(sqlca->sqlstate));
	sqlca->sqlcode = sqlcode;
	sqlca->sqlwarn[2] = 'W';
	sqlca->sqlwarn[0] = 'W';

	strncpy(sqlca->sqlerrm.sqlerrmc, message, sizeof(sqlca->sqlerrm.sqlerrmc));
	sqlca->sqlerrm.sqlerrmc[sizeof(sqlca->sqlerrm.sqlerrmc) - 1] = 0;
	sqlca->sqlerrm.sqlerrml = strlen(sqlca->sqlerrm.sqlerrmc);

	ecmdb_log("raising sqlcode %d\n", sqlcode);
}

/* this contains some quick hacks, needs to be cleaned up, but it works */
bool
ECPGconnect(int lineno, int c, const char *name, const char *user, const char *passwd, const char *connection_name, int autocommit)
{
	struct sqlca_t *sqlca = ECPGget_sqlca();
	enum COMPAT_MODE compat = c;
	struct connection *this;
	int			i,
				connect_params = 0;
	char	   *dbname = name ? ecmdb_strdup(name, lineno) : NULL,
			   *host = NULL,
			   *tmp,
			   *port = NULL,
			   *realname = NULL,
			   *options = NULL;
	const char **conn_keywords;
	const char **conn_values;

	if (sqlca == NULL)
	{
		ecmdb_raise(lineno, ECMDB_OUT_OF_MEMORY,
				   ECMDB_SQLSTATE_ECMDB_OUT_OF_MEMORY, NULL);
		ecmdb_free(dbname);
		return false;
	}

	ecmdb_init_sqlca(sqlca);

	/*
	 * clear auto_mem structure because some error handling functions might
	 * access it
	 */
	ecmdb_clear_auto_mem();

	if (INFORMIX_MODE(compat))
	{
		char	   *envname;

		/*
		 * Informix uses an environment variable DBPATH that overrides the
		 * connection parameters given here. We do the same with MDB_DBPATH as
		 * the syntax is different.
		 */
		envname = getenv("MDB_DBPATH");
		if (envname)
		{
			ecmdb_free(dbname);
			dbname = ecmdb_strdup(envname, lineno);
		}

	}

	if (dbname == NULL && connection_name == NULL)
		connection_name = "DEFAULT";

#if ENABLE_THREAD_SAFETY
	ecmdb_pthreads_init();
#endif

	/* check if the identifier is unique */
	if (ecmdb_get_connection(connection_name))
	{
		ecmdb_free(dbname);
		ecmdb_log("ECPGconnect: connection identifier %s is already in use\n",
				 connection_name);
		return false;
	}

	if ((this = (struct connection *) ecmdb_alloc(sizeof(struct connection), lineno)) == NULL)
	{
		ecmdb_free(dbname);
		return false;
	}

	if (dbname != NULL)
	{
		/* get the detail information from dbname */
		if (strncmp(dbname, "tcp:", 4) == 0 || strncmp(dbname, "unix:", 5) == 0)
		{
			int			offset = 0;

			/*
			 * only allow protocols tcp and unix
			 */
			if (strncmp(dbname, "tcp:", 4) == 0)
				offset = 4;
			else if (strncmp(dbname, "unix:", 5) == 0)
				offset = 5;

			if (strncmp(dbname + offset, "mollydb://", strlen("mollydb://")) == 0)
			{

				/*------
				 * new style:
				 *	<tcp|unix>:mollydb://server[:port|:/unixsocket/path:]
				 *	[/db-name][?options]
				 *------
				 */
				offset += strlen("mollydb://");

				tmp = strrchr(dbname + offset, '?');
				if (tmp != NULL)	/* options given */
				{
					options = ecmdb_strdup(tmp + 1, lineno);
					*tmp = '\0';
				}

				tmp = last_dir_separator(dbname + offset);
				if (tmp != NULL)	/* database name given */
				{
					if (tmp[1] != '\0') /* non-empty database name */
					{
						realname = ecmdb_strdup(tmp + 1, lineno);
						connect_params++;
					}
					*tmp = '\0';
				}

				tmp = strrchr(dbname + offset, ':');
				if (tmp != NULL)	/* port number or Unix socket path given */
				{
					char	   *tmp2;

					*tmp = '\0';
					if ((tmp2 = strchr(tmp + 1, ':')) != NULL)
					{
						*tmp2 = '\0';
						host = ecmdb_strdup(tmp + 1, lineno);
						connect_params++;
						if (strncmp(dbname, "unix:", 5) != 0)
						{
							ecmdb_log("ECPGconnect: socketname %s given for TCP connection on line %d\n", host, lineno);
							ecmdb_raise(lineno, ECMDB_CONNECT, ECMDB_SQLSTATE_SQLCLIENT_UNABLE_TO_ESTABLISH_SQLCONNECTION, realname ? realname : ecmdb_gettext("<DEFAULT>"));
							if (host)
								ecmdb_free(host);

							/*
							 * port not set yet if (port) ecmdb_free(port);
							 */
							if (options)
								ecmdb_free(options);
							if (realname)
								ecmdb_free(realname);
							if (dbname)
								ecmdb_free(dbname);
							free(this);
							return false;
						}
					}
					else
					{
						port = ecmdb_strdup(tmp + 1, lineno);
						connect_params++;
					}
				}

				if (strncmp(dbname, "unix:", 5) == 0)
				{
					if (strcmp(dbname + offset, "localhost") != 0 && strcmp(dbname + offset, "127.0.0.1") != 0)
					{
						ecmdb_log("ECPGconnect: non-localhost access via sockets on line %d\n", lineno);
						ecmdb_raise(lineno, ECMDB_CONNECT, ECMDB_SQLSTATE_SQLCLIENT_UNABLE_TO_ESTABLISH_SQLCONNECTION, realname ? realname : ecmdb_gettext("<DEFAULT>"));
						if (host)
							ecmdb_free(host);
						if (port)
							ecmdb_free(port);
						if (options)
							ecmdb_free(options);
						if (realname)
							ecmdb_free(realname);
						if (dbname)
							ecmdb_free(dbname);
						free(this);
						return false;
					}
				}
				else
				{
					if (*(dbname + offset) != '\0')
					{
						host = ecmdb_strdup(dbname + offset, lineno);
						connect_params++;
					}
				}

			}
		}
		else
		{
			/* old style: dbname[@server][:port] */
			tmp = strrchr(dbname, ':');
			if (tmp != NULL)	/* port number given */
			{
				port = ecmdb_strdup(tmp + 1, lineno);
				connect_params++;
				*tmp = '\0';
			}

			tmp = strrchr(dbname, '@');
			if (tmp != NULL)	/* host name given */
			{
				host = ecmdb_strdup(tmp + 1, lineno);
				connect_params++;
				*tmp = '\0';
			}

			if (strlen(dbname) > 0)
			{
				realname = ecmdb_strdup(dbname, lineno);
				connect_params++;
			}
			else
				realname = NULL;
		}
	}
	else
		realname = NULL;

	/* add connection to our list */
#ifdef ENABLE_THREAD_SAFETY
	pthread_mutex_lock(&connections_mutex);
#endif
	if (connection_name != NULL)
		this->name = ecmdb_strdup(connection_name, lineno);
	else
		this->name = ecmdb_strdup(realname, lineno);

	this->cache_head = NULL;
	this->prep_stmts = NULL;

	if (all_connections == NULL)
		this->next = NULL;
	else
		this->next = all_connections;

	all_connections = this;
#ifdef ENABLE_THREAD_SAFETY
	pthread_setspecific(actual_connection_key, all_connections);
#endif
	actual_connection = all_connections;

	ecmdb_log("ECPGconnect: opening database %s on %s port %s %s%s %s%s\n",
			 realname ? realname : "<DEFAULT>",
			 host ? host : "<DEFAULT>",
			 port ? (ecmdb_internal_regression_mode ? "<REGRESSION_PORT>" : port) : "<DEFAULT>",
			 options ? "with options " : "", options ? options : "",
			 (user && strlen(user) > 0) ? "for user " : "", user ? user : "");

	if (options)
		for (i = 0; options[i]; i++)
			/* count options */
			if (options[i] == '=')
				connect_params++;

	if (user && strlen(user) > 0)
		connect_params++;
	if (passwd && strlen(passwd) > 0)
		connect_params++;

	/* allocate enough space for all connection parameters */
	conn_keywords = (const char **) ecmdb_alloc((connect_params + 1) * sizeof(char *), lineno);
	conn_values = (const char **) ecmdb_alloc(connect_params * sizeof(char *), lineno);
	if (conn_keywords == NULL || conn_values == NULL)
	{
		if (host)
			ecmdb_free(host);
		if (port)
			ecmdb_free(port);
		if (options)
			ecmdb_free(options);
		if (realname)
			ecmdb_free(realname);
		if (dbname)
			ecmdb_free(dbname);
		if (conn_keywords)
			ecmdb_free(conn_keywords);
		if (conn_values)
			ecmdb_free(conn_values);
		free(this);
		return false;
	}

	i = 0;
	if (realname)
	{
		conn_keywords[i] = "dbname";
		conn_values[i] = realname;
		i++;
	}
	if (host)
	{
		conn_keywords[i] = "host";
		conn_values[i] = host;
		i++;
	}
	if (port)
	{
		conn_keywords[i] = "port";
		conn_values[i] = port;
		i++;
	}
	if (user && strlen(user) > 0)
	{
		conn_keywords[i] = "user";
		conn_values[i] = user;
		i++;
	}
	if (passwd && strlen(passwd) > 0)
	{
		conn_keywords[i] = "password";
		conn_values[i] = passwd;
		i++;
	}
	if (options)
	{
		char	   *str;

		/* options look like this "option1 = value1 option2 = value2 ... */
		/* we have to break up the string into single options */
		for (str = options; *str;)
		{
			int			e,
						a;
			char	   *token1,
					   *token2;

			for (token1 = str; *token1 && *token1 == ' '; token1++);
			for (e = 0; token1[e] && token1[e] != '='; e++);
			if (token1[e])		/* found "=" */
			{
				token1[e] = '\0';
				for (token2 = token1 + e + 1; *token2 && *token2 == ' '; token2++);
				for (a = 0; token2[a] && token2[a] != '&'; a++);
				if (token2[a])	/* found "&" => another option follows */
				{
					token2[a] = '\0';
					str = token2 + a + 1;
				}
				else
					str = token2 + a;

				conn_keywords[i] = token1;
				conn_values[i] = token2;
				i++;
			}
			else
				/* the parser should not be able to create this invalid option */
				str = token1 + e;
		}

	}
	conn_keywords[i] = NULL;	/* terminator */

	this->connection = PQconnectdbParams(conn_keywords, conn_values, 0);

	if (host)
		ecmdb_free(host);
	if (port)
		ecmdb_free(port);
	if (options)
		ecmdb_free(options);
	if (dbname)
		ecmdb_free(dbname);
	ecmdb_free(conn_values);
	ecmdb_free(conn_keywords);

	if (PQstatus(this->connection) == CONNECTION_BAD)
	{
		const char *errmsg = PQerrorMessage(this->connection);
		const char *db = realname ? realname : ecmdb_gettext("<DEFAULT>");

		ecmdb_log("ECPGconnect: could not open database: %s\n", errmsg);

		ecmdb_finish(this);
#ifdef ENABLE_THREAD_SAFETY
		pthread_mutex_unlock(&connections_mutex);
#endif

		ecmdb_raise(lineno, ECMDB_CONNECT, ECMDB_SQLSTATE_SQLCLIENT_UNABLE_TO_ESTABLISH_SQLCONNECTION, db);
		if (realname)
			ecmdb_free(realname);

		return false;
	}

	if (realname)
		ecmdb_free(realname);

#ifdef ENABLE_THREAD_SAFETY
	pthread_mutex_unlock(&connections_mutex);
#endif

	this->autocommit = autocommit;

	PQsetNoticeReceiver(this->connection, &ECPGnoticeReceiver, (void *) this);

	return true;
}

bool
ECPGdisconnect(int lineno, const char *connection_name)
{
	struct sqlca_t *sqlca = ECPGget_sqlca();
	struct connection *con;

	if (sqlca == NULL)
	{
		ecmdb_raise(lineno, ECMDB_OUT_OF_MEMORY,
				   ECMDB_SQLSTATE_ECMDB_OUT_OF_MEMORY, NULL);
		return (false);
	}

#ifdef ENABLE_THREAD_SAFETY
	pthread_mutex_lock(&connections_mutex);
#endif

	if (strcmp(connection_name, "ALL") == 0)
	{
		ecmdb_init_sqlca(sqlca);
		for (con = all_connections; con;)
		{
			struct connection *f = con;

			con = con->next;
			ecmdb_finish(f);
		}
	}
	else
	{
		con = ecmdb_get_connection_nr(connection_name);

		if (!ecmdb_init(con, connection_name, lineno))
		{
#ifdef ENABLE_THREAD_SAFETY
			pthread_mutex_unlock(&connections_mutex);
#endif
			return (false);
		}
		else
			ecmdb_finish(con);
	}

#ifdef ENABLE_THREAD_SAFETY
	pthread_mutex_unlock(&connections_mutex);
#endif

	return true;
}

PGconn *
ECPGget_PGconn(const char *connection_name)
{
	struct connection *con;

	con = ecmdb_get_connection(connection_name);
	if (con == NULL)
		return NULL;

	return con->connection;
}
