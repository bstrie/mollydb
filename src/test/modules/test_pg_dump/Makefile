# src/test/modules/test_rls_hooks/Makefile

MODULE = test_mdb_dump
PGFILEDESC = "test_mdb_dump - Test mdb_dump with an extension"

EXTENSION = test_mdb_dump
DATA = test_mdb_dump--1.0.sql

REGRESS = test_mdb_dump

check: prove-check

prove-check:
	$(prove_check)

ifdef USE_PGXS
MDB_CONFIG = mdb_config
PGXS := $(shell $(MDB_CONFIG) --pgxs)
include $(PGXS)
else
subdir = src/test/modules/test_mdb_dump
top_builddir = ../../../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
