# contrib/tsearch2/Makefile

MODULES = tsearch2

EXTENSION = tsearch2
DATA = tsearch2--1.0.sql tsearch2--unpackaged--1.0.sql
PGFILEDESC = "tsearch2 - backward-compatible text search functionality"

REGRESS = tsearch2

ifdef USE_PGXS
MDB_CONFIG = mdb_config
PGXS := $(shell $(MDB_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/tsearch2
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
