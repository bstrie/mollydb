# src/test/modules/brin/Makefile

EXTRA_CLEAN = ./isolation_output
EXTRA_INSTALL=contrib/pageinspect

ISOLATIONCHECKS=summarization-and-inprogress-insertion

ifdef USE_PGXS
MDB_CONFIG = mdb_config
PGXS := $(shell $(MDB_CONFIG) --pgxs)
include $(PGXS)
else
subdir = src/test/modules/brin
top_builddir = ../../../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif

check: isolation-check

isolation-check: | submake-isolation
	$(MKDIR_P) isolation_output
	$(mdb_isolation_regress_check) \
	    --outputdir=./isolation_output \
	    $(ISOLATIONCHECKS)

.PHONY: check isolation-check

submake-isolation:
	$(MAKE) -C $(top_builddir)/src/test/isolation all
