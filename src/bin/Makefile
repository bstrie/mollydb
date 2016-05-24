#-------------------------------------------------------------------------
#
# Makefile for src/bin (client programs)
#
# Portions Copyright (c) 1996-2016, MollyDB Global Development Group
# Portions Copyright (c) 1994, Regents of the University of California
#
# src/bin/Makefile
#
#-------------------------------------------------------------------------

subdir = src/bin
top_builddir = ../..
include $(top_builddir)/src/Makefile.global

SUBDIRS = \
	initdb \
	mdb_archivecleanup \
	mdb_basebackup \
	mdb_config \
	mdb_controldata \
	mdb_ctl \
	mdb_dump \
	mdb_resetxlog \
	mdb_rewind \
	mdb_test_fsync \
	mdb_test_timing \
	mdb_upgrade \
	mdb_xlogdump \
	pgbench \
	psql \
	scripts

ifeq ($(PORTNAME), win32)
SUBDIRS += pgevent
else
ALWAYS_SUBDIRS += pgevent
endif

$(recurse)
$(recurse_always)
