# contrib/mollydb_fdw/Makefile

MODULE_big = mollydb_fdw
OBJS = mollydb_fdw.o option.o deparse.o connection.o shippable.o $(WIN32RES)
PGFILEDESC = "mollydb_fdw - foreign data wrapper for MollyDB"

MDB_CPPFLAGS = -I$(libpq_srcdir)
SHLIB_LINK = $(libpq)

EXTENSION = mollydb_fdw
DATA = mollydb_fdw--1.0.sql

REGRESS = mollydb_fdw

ifdef USE_PGXS
MDB_CONFIG = mdb_config
PGXS := $(shell $(MDB_CONFIG) --pgxs)
include $(PGXS)
else
SHLIB_PREREQS = submake-libpq
subdir = contrib/mollydb_fdw
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
