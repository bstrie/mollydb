# contrib/mdb_visibility/Makefile

MODULE_big = mdb_visibility
OBJS = mdb_visibility.o $(WIN32RES)

EXTENSION = mdb_visibility
DATA = mdb_visibility--1.0.sql
PGFILEDESC = "mdb_visibility - page visibility information"

ifdef USE_PGXS
MDB_CONFIG = mdb_config
PGXS := $(shell $(MDB_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/mdb_visibility
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
