# contrib/tsm_system_time/Makefile

MODULE_big = tsm_system_time
OBJS = tsm_system_time.o $(WIN32RES)
PGFILEDESC = "tsm_system_time - TABLESAMPLE method which accepts time in milliseconds as a limit"

EXTENSION = tsm_system_time
DATA = tsm_system_time--1.0.sql

REGRESS = tsm_system_time

SHLIB_LINK += $(filter -lm, $(LIBS))

ifdef USE_PGXS
MDB_CONFIG = mdb_config
PGXS := $(shell $(MDB_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/tsm_system_time
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
