# contrib/mdb_standby/Makefile

PGFILEDESC = "mdb_standby - supports creation of a warm standby"
PGAPPICON = win32

PROGRAM = mdb_standby
OBJS	= mdb_standby.o $(WIN32RES)

ifdef USE_PGXS
PG_CONFIG = mdb_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/mdb_standby
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
