# contrib/semdb/Makefile

MODULE_big = semdb
OBJS = hooks.o selinux.o uavc.o label.o dml.o \
	database.o schema.o relation.o proc.o $(WIN32RES)
DATA_built = semdb.sql
PGFILEDESC = "semdb - SELinux integration"

# Note: because we don't tell the Makefile there are any regression tests,
# we have to clean those result files explicitly
EXTRA_CLEAN = -r $(mdb_regress_clean_files) tmp/ *.pp semdb-regtest.if semdb-regtest.fc

ifdef USE_PGXS
MDB_CONFIG = mdb_config
PGXS := $(shell $(MDB_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/semdb
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif

SHLIB_LINK += -lselinux
