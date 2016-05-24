# contrib/mdb_buffercache/Makefile

MODULE_big = mdb_buffercache
OBJS = mdb_buffercache_pages.o $(WIN32RES)

EXTENSION = mdb_buffercache
DATA = mdb_buffercache--1.1.sql mdb_buffercache--1.0--1.1.sql mdb_buffercache--unpackaged--1.0.sql
PGFILEDESC = "mdb_buffercache - monitoring of shared buffer cache in real-time"

ifdef USE_PGXS
MDB_CONFIG = mdb_config
PGXS := $(shell $(MDB_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/mdb_buffercache
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
