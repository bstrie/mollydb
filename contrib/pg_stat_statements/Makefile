# contrib/mdb_stat_statements/Makefile

MODULE_big = mdb_stat_statements
OBJS = mdb_stat_statements.o $(WIN32RES)

EXTENSION = mdb_stat_statements
DATA = mdb_stat_statements--1.3.sql mdb_stat_statements--1.2--1.3.sql \
	mdb_stat_statements--1.1--1.2.sql mdb_stat_statements--1.0--1.1.sql \
	mdb_stat_statements--unpackaged--1.0.sql
PGFILEDESC = "mdb_stat_statements - execution statistics of SQL statements"

LDFLAGS_SL += $(filter -lm, $(LIBS))

ifdef USE_PGXS
PG_CONFIG = mdb_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/mdb_stat_statements
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
