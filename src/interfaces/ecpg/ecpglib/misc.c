/* src/interfaces/ecpg/ecpglib/misc.c */

#define POSTGRES_ECMDB_INTERNAL
#include "mollydb_fe.h"

#include <limits.h>
#include <unistd.h>
#include "ecpg-pthread-win32.h"
#include "ecpgtype.h"
#include "ecpglib.h"
#include "ecpgerrno.h"
#include "extern.h"
#include "sqlca.h"
#include "pgtypes_numeric.h"
#include "pgtypes_date.h"
#include "pgtypes_timestamp.h"
#include "pgtypes_interval.h"
#include "mdb_config_paths.h"

#ifdef HAVE_LONG_LONG_INT
#ifndef LONG_LONG_MIN
#ifdef LLONG_MIN
#define LONG_LONG_MIN LLONG_MIN
#else
#define LONG_LONG_MIN LONGLONG_MIN
#endif   /* LLONG_MIN */
#endif   /* LONG_LONG_MIN */
#endif   /* HAVE_LONG_LONG_INT */

bool		ecmdb_internal_regression_mode = false;

static struct sqlca_t sqlca_init =
{
	{
		'S', 'Q', 'L', 'C', 'A', ' ', ' ', ' '
	},
	sizeof(struct sqlca_t),
	0,
	{
		0,
		{
			0
		}
	},
	{
		'N', 'O', 'T', ' ', 'S', 'E', 'T', ' '
	},
	{
		0, 0, 0, 0, 0, 0
	},
	{
		0, 0, 0, 0, 0, 0, 0, 0
	},
	{
		'0', '0', '0', '0', '0'
	}
};

#ifdef ENABLE_THREAD_SAFETY
static pthread_key_t sqlca_key;
static pthread_once_t sqlca_key_once = PTHREAD_ONCE_INIT;
#else
static struct sqlca_t sqlca =
{
	{
		'S', 'Q', 'L', 'C', 'A', ' ', ' ', ' '
	},
	sizeof(struct sqlca_t),
	0,
	{
		0,
		{
			0
		}
	},
	{
		'N', 'O', 'T', ' ', 'S', 'E', 'T', ' '
	},
	{
		0, 0, 0, 0, 0, 0
	},
	{
		0, 0, 0, 0, 0, 0, 0, 0
	},
	{
		'0', '0', '0', '0', '0'
	}
};
#endif

#ifdef ENABLE_THREAD_SAFETY
static pthread_mutex_t debug_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t debug_init_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif
static int	simple_debug = 0;
static FILE *debugstream = NULL;

void
ecmdb_init_sqlca(struct sqlca_t * sqlca)
{
	memcpy((char *) sqlca, (char *) &sqlca_init, sizeof(struct sqlca_t));
}

bool
ecmdb_init(const struct connection * con, const char *connection_name, const int lineno)
{
	struct sqlca_t *sqlca = ECPGget_sqlca();

	if (sqlca == NULL)
	{
		ecmdb_raise(lineno, ECMDB_OUT_OF_MEMORY, ECMDB_SQLSTATE_ECMDB_OUT_OF_MEMORY,
				   NULL);
		return (false);
	}

	ecmdb_init_sqlca(sqlca);
	if (con == NULL)
	{
		ecmdb_raise(lineno, ECMDB_NO_CONN, ECMDB_SQLSTATE_CONNECTION_DOES_NOT_EXIST,
				   connection_name ? connection_name : ecmdb_gettext("NULL"));
		return (false);
	}

	return (true);
}

#ifdef ENABLE_THREAD_SAFETY
static void
ecmdb_sqlca_key_destructor(void *arg)
{
	free(arg);					/* sqlca structure allocated in ECPGget_sqlca */
}

static void
ecmdb_sqlca_key_init(void)
{
	pthread_key_create(&sqlca_key, ecmdb_sqlca_key_destructor);
}
#endif

struct sqlca_t *
ECPGget_sqlca(void)
{
#ifdef ENABLE_THREAD_SAFETY
	struct sqlca_t *sqlca;

	pthread_once(&sqlca_key_once, ecmdb_sqlca_key_init);

	sqlca = pthread_getspecific(sqlca_key);
	if (sqlca == NULL)
	{
		sqlca = malloc(sizeof(struct sqlca_t));
		if (sqlca == NULL)
			return NULL;
		ecmdb_init_sqlca(sqlca);
		pthread_setspecific(sqlca_key, sqlca);
	}
	return (sqlca);
#else
	return (&sqlca);
#endif
}

bool
ECPGstatus(int lineno, const char *connection_name)
{
	struct connection *con = ecmdb_get_connection(connection_name);

	if (!ecmdb_init(con, connection_name, lineno))
		return (false);

	/* are we connected? */
	if (con->connection == NULL)
	{
		ecmdb_raise(lineno, ECMDB_NOT_CONN, ECMDB_SQLSTATE_ECMDB_INTERNAL_ERROR, con->name);
		return false;
	}

	return (true);
}

PGTransactionStatusType
ECPGtransactionStatus(const char *connection_name)
{
	const struct connection *con;

	con = ecmdb_get_connection(connection_name);
	if (con == NULL)
	{
		/* transaction status is unknown */
		return PQTRANS_UNKNOWN;
	}

	return PQtransactionStatus(con->connection);

}

bool
ECPGtrans(int lineno, const char *connection_name, const char *transaction)
{
	PGresult   *res;
	struct connection *con = ecmdb_get_connection(connection_name);

	if (!ecmdb_init(con, connection_name, lineno))
		return (false);

	ecmdb_log("ECPGtrans on line %d: action \"%s\"; connection \"%s\"\n", lineno, transaction, con ? con->name : "null");

	/* if we have no connection we just simulate the command */
	if (con && con->connection)
	{
		/*
		 * If we got a transaction command but have no open transaction, we
		 * have to start one, unless we are in autocommit, where the
		 * developers have to take care themselves. However, if the command is
		 * a begin statement, we just execute it once.
		 */
		if (PQtransactionStatus(con->connection) == PQTRANS_IDLE && !con->autocommit && strncmp(transaction, "begin", 5) != 0 && strncmp(transaction, "start", 5) != 0)
		{
			res = PQexec(con->connection, "begin transaction");
			if (!ecmdb_check_PQresult(res, lineno, con->connection, ECMDB_COMPAT_PGSQL))
				return FALSE;
			PQclear(res);
		}

		res = PQexec(con->connection, transaction);
		if (!ecmdb_check_PQresult(res, lineno, con->connection, ECMDB_COMPAT_PGSQL))
			return FALSE;
		PQclear(res);
	}

	return true;
}


void
ECPGdebug(int n, FILE *dbgs)
{
#ifdef ENABLE_THREAD_SAFETY
	pthread_mutex_lock(&debug_init_mutex);
#endif

	if (n > 100)
	{
		ecmdb_internal_regression_mode = true;
		simple_debug = n - 100;
	}
	else
		simple_debug = n;

	debugstream = dbgs;

	ecmdb_log("ECPGdebug: set to %d\n", simple_debug);

#ifdef ENABLE_THREAD_SAFETY
	pthread_mutex_unlock(&debug_init_mutex);
#endif
}

void
ecmdb_log(const char *format,...)
{
	va_list		ap;
	struct sqlca_t *sqlca = ECPGget_sqlca();
	const char *intl_format;
	int			bufsize;
	char	   *fmt;

	if (!simple_debug)
		return;

	/* localize the error message string */
	intl_format = ecmdb_gettext(format);

	/*
	 * Insert PID into the format, unless ecmdb_internal_regression_mode is set
	 * (regression tests want unchanging output).
	 */
	bufsize = strlen(intl_format) + 100;
	fmt = (char *) malloc(bufsize);
	if (fmt == NULL)
		return;

	if (ecmdb_internal_regression_mode)
		snprintf(fmt, bufsize, "[NO_PID]: %s", intl_format);
	else
		snprintf(fmt, bufsize, "[%d]: %s", (int) getpid(), intl_format);

#ifdef ENABLE_THREAD_SAFETY
	pthread_mutex_lock(&debug_mutex);
#endif

	va_start(ap, format);
	vfprintf(debugstream, fmt, ap);
	va_end(ap);

	/* dump out internal sqlca variables */
	if (ecmdb_internal_regression_mode && sqlca != NULL)
	{
		fprintf(debugstream, "[NO_PID]: sqlca: code: %ld, state: %s\n",
				sqlca->sqlcode, sqlca->sqlstate);
	}

	fflush(debugstream);

#ifdef ENABLE_THREAD_SAFETY
	pthread_mutex_unlock(&debug_mutex);
#endif

	free(fmt);
}

void
ECPGset_noind_null(enum ECPGttype type, void *ptr)
{
	switch (type)
	{
		case ECPGt_char:
		case ECPGt_unsigned_char:
		case ECPGt_string:
			*((char *) ptr) = '\0';
			break;
		case ECPGt_short:
		case ECPGt_unsigned_short:
			*((short int *) ptr) = SHRT_MIN;
			break;
		case ECPGt_int:
		case ECPGt_unsigned_int:
			*((int *) ptr) = INT_MIN;
			break;
		case ECPGt_long:
		case ECPGt_unsigned_long:
		case ECPGt_date:
			*((long *) ptr) = LONG_MIN;
			break;
#ifdef HAVE_LONG_LONG_INT
		case ECPGt_long_long:
		case ECPGt_unsigned_long_long:
			*((long long *) ptr) = LONG_LONG_MIN;
			break;
#endif   /* HAVE_LONG_LONG_INT */
		case ECPGt_float:
			memset((char *) ptr, 0xff, sizeof(float));
			break;
		case ECPGt_double:
			memset((char *) ptr, 0xff, sizeof(double));
			break;
		case ECPGt_varchar:
			*(((struct ECPGgeneric_varchar *) ptr)->arr) = 0x00;
			((struct ECPGgeneric_varchar *) ptr)->len = 0;
			break;
		case ECPGt_decimal:
			memset((char *) ptr, 0, sizeof(decimal));
			((decimal *) ptr)->sign = NUMERIC_NULL;
			break;
		case ECPGt_numeric:
			memset((char *) ptr, 0, sizeof(numeric));
			((numeric *) ptr)->sign = NUMERIC_NULL;
			break;
		case ECPGt_interval:
			memset((char *) ptr, 0xff, sizeof(interval));
			break;
		case ECPGt_timestamp:
			memset((char *) ptr, 0xff, sizeof(timestamp));
			break;
		default:
			break;
	}
}

static bool
_check(unsigned char *ptr, int length)
{
	for (length--; length >= 0; length--)
		if (ptr[length] != 0xff)
			return false;

	return true;
}

bool
ECPGis_noind_null(enum ECPGttype type, void *ptr)
{
	switch (type)
	{
		case ECPGt_char:
		case ECPGt_unsigned_char:
		case ECPGt_string:
			if (*((char *) ptr) == '\0')
				return true;
			break;
		case ECPGt_short:
		case ECPGt_unsigned_short:
			if (*((short int *) ptr) == SHRT_MIN)
				return true;
			break;
		case ECPGt_int:
		case ECPGt_unsigned_int:
			if (*((int *) ptr) == INT_MIN)
				return true;
			break;
		case ECPGt_long:
		case ECPGt_unsigned_long:
		case ECPGt_date:
			if (*((long *) ptr) == LONG_MIN)
				return true;
			break;
#ifdef HAVE_LONG_LONG_INT
		case ECPGt_long_long:
		case ECPGt_unsigned_long_long:
			if (*((long long *) ptr) == LONG_LONG_MIN)
				return true;
			break;
#endif   /* HAVE_LONG_LONG_INT */
		case ECPGt_float:
			return (_check(ptr, sizeof(float)));
			break;
		case ECPGt_double:
			return (_check(ptr, sizeof(double)));
			break;
		case ECPGt_varchar:
			if (*(((struct ECPGgeneric_varchar *) ptr)->arr) == 0x00)
				return true;
			break;
		case ECPGt_decimal:
			if (((decimal *) ptr)->sign == NUMERIC_NULL)
				return true;
			break;
		case ECPGt_numeric:
			if (((numeric *) ptr)->sign == NUMERIC_NULL)
				return true;
			break;
		case ECPGt_interval:
			return (_check(ptr, sizeof(interval)));
			break;
		case ECPGt_timestamp:
			return (_check(ptr, sizeof(timestamp)));
			break;
		default:
			break;
	}

	return false;
}

#ifdef WIN32
#ifdef ENABLE_THREAD_SAFETY

void
win32_pthread_mutex(volatile pthread_mutex_t *mutex)
{
	if (mutex->handle == NULL)
	{
		while (InterlockedExchange((LONG *) &mutex->initlock, 1) == 1)
			Sleep(0);
		if (mutex->handle == NULL)
			mutex->handle = CreateMutex(NULL, FALSE, NULL);
		InterlockedExchange((LONG *) &mutex->initlock, 0);
	}
}

static pthread_mutex_t win32_pthread_once_lock = PTHREAD_MUTEX_INITIALIZER;

void
win32_pthread_once(volatile pthread_once_t *once, void (*fn) (void))
{
	if (!*once)
	{
		pthread_mutex_lock(&win32_pthread_once_lock);
		if (!*once)
		{
			*once = true;
			fn();
		}
		pthread_mutex_unlock(&win32_pthread_once_lock);
	}
}
#endif   /* ENABLE_THREAD_SAFETY */
#endif   /* WIN32 */

#ifdef ENABLE_NLS

char *
ecmdb_gettext(const char *msgid)
{
	static bool already_bound = false;

	if (!already_bound)
	{
		/* dgettext() preserves errno, but bindtextdomain() doesn't */
#ifdef WIN32
		int			save_errno = GetLastError();
#else
		int			save_errno = errno;
#endif
		const char *ldir;

		already_bound = true;
		/* No relocatable lookup here because the binary could be anywhere */
		ldir = getenv("PGLOCALEDIR");
		if (!ldir)
			ldir = LOCALEDIR;
		bindtextdomain(MDB_TEXTDOMAIN("ecpglib"), ldir);
#ifdef WIN32
		SetLastError(save_errno);
#else
		errno = save_errno;
#endif
	}

	return dgettext(MDB_TEXTDOMAIN("ecpglib"), msgid);
}
#endif   /* ENABLE_NLS */

struct var_list *ivlist = NULL;

void
ECPGset_var(int number, void *pointer, int lineno)
{
	struct var_list *ptr;

	for (ptr = ivlist; ptr != NULL; ptr = ptr->next)
	{
		if (ptr->number == number)
		{
			/* already known => just change pointer value */
			ptr->pointer = pointer;
			return;
		}
	}

	/* a new one has to be added */
	ptr = (struct var_list *) calloc(1L, sizeof(struct var_list));
	if (!ptr)
	{
		struct sqlca_t *sqlca = ECPGget_sqlca();

		if (sqlca == NULL)
		{
			ecmdb_raise(lineno, ECMDB_OUT_OF_MEMORY,
					   ECMDB_SQLSTATE_ECMDB_OUT_OF_MEMORY, NULL);
			return;
		}

		sqlca->sqlcode = ECMDB_OUT_OF_MEMORY;
		strncpy(sqlca->sqlstate, "YE001", sizeof(sqlca->sqlstate));
		snprintf(sqlca->sqlerrm.sqlerrmc, sizeof(sqlca->sqlerrm.sqlerrmc), "out of memory on line %d", lineno);
		sqlca->sqlerrm.sqlerrml = strlen(sqlca->sqlerrm.sqlerrmc);
		/* free all memory we have allocated for the user */
		ECPGfree_auto_mem();
	}
	else
	{
		ptr->number = number;
		ptr->pointer = pointer;
		ptr->next = ivlist;
		ivlist = ptr;
	}
}

void *
ECPGget_var(int number)
{
	struct var_list *ptr;

	for (ptr = ivlist; ptr != NULL && ptr->number != number; ptr = ptr->next);
	return (ptr) ? ptr->pointer : NULL;
}
