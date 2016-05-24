# contrib/mdb_freespacemap/Makefile

MODULE_big = mdb_freespacemap
OBJS = mdb_freespacemap.o $(WIN32RES)

EXTENSION = mdb_freespacemap
DATA = mdb_freespacemap--1.0.sql mdb_freespacemap--unpackaged--1.0.sql
PGFILEDESC = "mdb_freespacemap - monitoring of free space map"

ifdef USE_PGXS
PG_CONFIG = mdb_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/mdb_freespacemap
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
