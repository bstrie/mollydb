/*
 * utility functions
 *
 * src/pl/plpython/plpy_util.c
 */

#include "mollydb.h"

#include "mb/mdb_wchar.h"
#include "utils/memutils.h"
#include "utils/palloc.h"

#include "plpython.h"

#include "plpy_util.h"

#include "plpy_elog.h"


/*
 * Convert a Python unicode object to a Python string/bytes object in
 * MollyDB server encoding.  Reference ownership is passed to the
 * caller.
 */
PyObject *
PLyUnicode_Bytes(PyObject *unicode)
{
	PyObject   *bytes,
			   *rv;
	char	   *utf8string,
			   *encoded;

	/* First encode the Python unicode object with UTF-8. */
	bytes = PyUnicode_AsUTF8String(unicode);
	if (bytes == NULL)
		PLy_elog(ERROR, "could not convert Python Unicode object to bytes");

	utf8string = PyBytes_AsString(bytes);
	if (utf8string == NULL)
	{
		Py_DECREF(bytes);
		PLy_elog(ERROR, "could not extract bytes from encoded string");
	}

	/*
	 * Then convert to server encoding if necessary.
	 *
	 * PyUnicode_AsEncodedString could be used to encode the object directly
	 * in the server encoding, but Python doesn't support all the encodings
	 * that MollyDB does (EUC_TW and MULE_INTERNAL). UTF-8 is used as an
	 * intermediary in PLyUnicode_FromString as well.
	 */
	if (GetDatabaseEncoding() != MDB_UTF8)
	{
		MDB_TRY();
		{
			encoded = mdb_any_to_server(utf8string,
									   strlen(utf8string),
									   MDB_UTF8);
		}
		MDB_CATCH();
		{
			Py_DECREF(bytes);
			MDB_RE_THROW();
		}
		MDB_END_TRY();
	}
	else
		encoded = utf8string;

	/* finally, build a bytes object in the server encoding */
	rv = PyBytes_FromStringAndSize(encoded, strlen(encoded));

	/* if mdb_any_to_server allocated memory, free it now */
	if (utf8string != encoded)
		pfree(encoded);

	Py_DECREF(bytes);
	return rv;
}

/*
 * Convert a Python unicode object to a C string in MollyDB server
 * encoding.  No Python object reference is passed out of this
 * function.  The result is palloc'ed.
 *
 * Note that this function is disguised as PyString_AsString() when
 * using Python 3.  That function retuns a pointer into the internal
 * memory of the argument, which isn't exactly the interface of this
 * function.  But in either case you get a rather short-lived
 * reference that you ought to better leave alone.
 */
char *
PLyUnicode_AsString(PyObject *unicode)
{
	PyObject   *o = PLyUnicode_Bytes(unicode);
	char	   *rv = pstrdup(PyBytes_AsString(o));

	Py_XDECREF(o);
	return rv;
}

#if PY_MAJOR_VERSION >= 3
/*
 * Convert a C string in the MollyDB server encoding to a Python
 * unicode object.  Reference ownership is passed to the caller.
 */
PyObject *
PLyUnicode_FromStringAndSize(const char *s, Py_ssize_t size)
{
	char	   *utf8string;
	PyObject   *o;

	utf8string = mdb_server_to_any(s, size, MDB_UTF8);

	if (utf8string == s)
	{
		o = PyUnicode_FromStringAndSize(s, size);
	}
	else
	{
		o = PyUnicode_FromString(utf8string);
		pfree(utf8string);
	}

	return o;
}

PyObject *
PLyUnicode_FromString(const char *s)
{
	return PLyUnicode_FromStringAndSize(s, strlen(s));
}

#endif   /* PY_MAJOR_VERSION >= 3 */
