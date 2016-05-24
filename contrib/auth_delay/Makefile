# contrib/auth_delay/Makefile

MODULES = auth_delay
PGFILEDESC = "auth_delay - delay authentication failure reports"

ifdef USE_PGXS
MDB_CONFIG = mdb_config
PGXS := $(shell $(MDB_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/auth_delay
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
