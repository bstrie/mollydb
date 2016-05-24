#-------------------------------------------------------------------------
#
# Makefile for src/bin/mdb_controldata
#
# Copyright (c) 1998-2016, MollyDB Global Development Group
#
# src/bin/mdb_controldata/Makefile
#
#-------------------------------------------------------------------------

PGFILEDESC = "mdb_controldata - reads the data from mdb_control"
PGAPPICON=win32

subdir = src/bin/mdb_controldata
top_builddir = ../../..
include $(top_builddir)/src/Makefile.global

OBJS= mdb_controldata.o $(WIN32RES)

all: mdb_controldata

mdb_controldata: $(OBJS) | submake-libpgport
	$(CC) $(CFLAGS) $^ $(LDFLAGS) $(LDFLAGS_EX) $(LIBS) -o $@$(X)

install: all installdirs
	$(INSTALL_PROGRAM) mdb_controldata$(X) '$(DESTDIR)$(bindir)/mdb_controldata$(X)'

installdirs:
	$(MKDIR_P) '$(DESTDIR)$(bindir)'

uninstall:
	rm -f '$(DESTDIR)$(bindir)/mdb_controldata$(X)'

clean distclean maintainer-clean:
	rm -f mdb_controldata$(X) $(OBJS)
	rm -rf tmp_check

check:
	$(prove_check)

installcheck:
	$(prove_installcheck)
