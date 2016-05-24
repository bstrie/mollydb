# contrib/lo/Makefile

MODULES = lo

EXTENSION = lo
DATA = lo--1.0.sql lo--unpackaged--1.0.sql
PGFILEDESC = "lo - management for large objects"

ifdef USE_PGXS
MDB_CONFIG = mdb_config
PGXS := $(shell $(MDB_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/lo
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
