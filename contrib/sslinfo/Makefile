# contrib/sslinfo/Makefile

MODULE_big = sslinfo
OBJS = sslinfo.o $(WIN32RES)

EXTENSION = sslinfo
DATA = sslinfo--1.0--1.1.sql sslinfo--1.1.sql \
	sslinfo--unpackaged--1.0.sql
PGFILEDESC = "sslinfo - information about client SSL certificate"

ifdef USE_PGXS
MDB_CONFIG = mdb_config
PGXS := $(shell $(MDB_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/sslinfo
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif

SHLIB_LINK += $(filter -lssl -lcrypto -lssleay32 -leay32, $(LIBS))
