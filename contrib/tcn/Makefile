# contrib/tcn/Makefile

MODULES = tcn

EXTENSION = tcn
DATA = tcn--1.0.sql
PGFILEDESC = "tcn - trigger function notifying listeners"

ifdef USE_PGXS
MDB_CONFIG = mdb_config
PGXS := $(shell $(MDB_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/tcn
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
