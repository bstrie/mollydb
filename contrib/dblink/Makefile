# contrib/dblink/Makefile

MODULE_big = dblink
OBJS	= dblink.o $(WIN32RES)
MDB_CPPFLAGS = -I$(libpq_srcdir)
SHLIB_LINK = $(libpq)

EXTENSION = dblink
DATA = dblink--1.1.sql dblink--1.0--1.1.sql dblink--unpackaged--1.0.sql
PGFILEDESC = "dblink - connect to other MollyDB databases"

REGRESS = paths dblink
REGRESS_OPTS = --dlpath=$(top_builddir)/src/test/regress
EXTRA_CLEAN = sql/paths.sql expected/paths.out

ifdef USE_PGXS
MDB_CONFIG = mdb_config
PGXS := $(shell $(MDB_CONFIG) --pgxs)
include $(PGXS)
else
SHLIB_PREREQS = submake-libpq
subdir = contrib/dblink
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
