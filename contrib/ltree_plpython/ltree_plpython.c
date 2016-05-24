#include "mollydb.h"
#include "fmgr.h"
#include "plpython.h"
#include "ltree.h"

MDB_MODULE_MAGIC;


MDB_FUNCTION_INFO_V1(ltree_to_plpython);

Datum
ltree_to_plpython(MDB_FUNCTION_ARGS)
{
	ltree	   *in = MDB_GETARG_LTREE(0);
	int			i;
	PyObject   *list;
	ltree_level *curlevel;

	list = PyList_New(in->numlevel);

	curlevel = LTREE_FIRST(in);
	for (i = 0; i < in->numlevel; i++)
	{
		PyList_SetItem(list, i, PyString_FromStringAndSize(curlevel->name, curlevel->len));
		curlevel = LEVEL_NEXT(curlevel);
	}

	MDB_FREE_IF_COPY(in, 0);

	return PointerGetDatum(list);
}
