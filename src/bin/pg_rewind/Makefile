#-------------------------------------------------------------------------
#
# Makefile for src/bin/mdb_rewind
#
# Portions Copyright (c) 2013-2016, MollyDB Global Development Group
#
# src/bin/mdb_rewind/Makefile
#
#-------------------------------------------------------------------------

PGFILEDESC = "mdb_rewind - synchronize a data directory with another one forked from"
PGAPPICON = win32

subdir = src/bin/mdb_rewind
top_builddir = ../../..
include $(top_builddir)/src/Makefile.global

PG_CPPFLAGS = -I$(libpq_srcdir)
PG_LIBS = $(libpq_pgport)

override CPPFLAGS := -I$(libpq_srcdir) -DFRONTEND $(CPPFLAGS)

OBJS	= mdb_rewind.o parsexlog.o xlogreader.o datapagemap.o timeline.o \
	fetch.o file_ops.o copy_fetch.o libpq_fetch.o filemap.o logging.o \
	$(WIN32RES)

EXTRA_CLEAN = xlogreader.c

all: mdb_rewind

mdb_rewind: $(OBJS) | submake-libpq submake-libpgport
	$(CC) $(CFLAGS) $^ $(libpq_pgport) $(LDFLAGS) $(LDFLAGS_EX) $(LIBS) -o $@$(X)

xlogreader.c: % : $(top_srcdir)/src/backend/access/transam/%
	rm -f $@ && $(LN_S) $< .

install: all installdirs
	$(INSTALL_PROGRAM) mdb_rewind$(X) '$(DESTDIR)$(bindir)/mdb_rewind$(X)'

installdirs:
	$(MKDIR_P) '$(DESTDIR)$(bindir)'

uninstall:
	rm -f '$(DESTDIR)$(bindir)/mdb_rewind$(X)'

clean distclean maintainer-clean:
	rm -f mdb_rewind$(X) $(OBJS) xlogreader.c
	rm -rf tmp_check

check:
	$(prove_check)

installcheck:
	$(prove_installcheck)
