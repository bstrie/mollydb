# contrib/mdb_trgm/Makefile

MODULE_big = mdb_trgm
OBJS = trgm_op.o trgm_gist.o trgm_gin.o trgm_regexp.o $(WIN32RES)

EXTENSION = mdb_trgm
DATA = mdb_trgm--1.2.sql mdb_trgm--1.0--1.1.sql mdb_trgm--1.1--1.2.sql mdb_trgm--unpackaged--1.0.sql
PGFILEDESC = "mdb_trgm - trigram matching"

REGRESS = mdb_trgm mdb_word_trgm

ifdef USE_PGXS
PG_CONFIG = mdb_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/mdb_trgm
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
