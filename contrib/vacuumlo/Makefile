# contrib/vacuumlo/Makefile

PGFILEDESC = "vacuumlo - removes orphaned large objects"
PGAPPICON = win32

PROGRAM = vacuumlo
OBJS	= vacuumlo.o $(WIN32RES)

MDB_CPPFLAGS = -I$(libpq_srcdir)
MDB_LIBS = $(libpq_pgport)

ifdef USE_PGXS
MDB_CONFIG = mdb_config
PGXS := $(shell $(MDB_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/vacuumlo
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
