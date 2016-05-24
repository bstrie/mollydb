# contrib/intagg/Makefile

EXTENSION = intagg
DATA = intagg--1.0.sql intagg--unpackaged--1.0.sql

ifdef USE_PGXS
MDB_CONFIG = mdb_config
PGXS := $(shell $(MDB_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/intagg
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
