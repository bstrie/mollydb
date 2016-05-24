# contrib/mdb_prewarm/Makefile

MODULE_big = mdb_prewarm
OBJS = mdb_prewarm.o $(WIN32RES)

EXTENSION = mdb_prewarm
DATA = mdb_prewarm--1.0.sql
PGFILEDESC = "mdb_prewarm - preload relation data into system buffer cache"

ifdef USE_PGXS
PG_CONFIG = mdb_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/mdb_prewarm
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
