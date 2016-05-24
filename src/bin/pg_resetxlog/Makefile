#-------------------------------------------------------------------------
#
# Makefile for src/bin/mdb_resetxlog
#
# Copyright (c) 1998-2016, MollyDB Global Development Group
#
# src/bin/mdb_resetxlog/Makefile
#
#-------------------------------------------------------------------------

PGFILEDESC = "mdb_resetxlog - reset MollyDB WAL log"
PGAPPICON=win32

subdir = src/bin/mdb_resetxlog
top_builddir = ../../..
include $(top_builddir)/src/Makefile.global

OBJS= mdb_resetxlog.o $(WIN32RES)

all: mdb_resetxlog

mdb_resetxlog: $(OBJS) | submake-libpgport
	$(CC) $(CFLAGS) $^ $(LDFLAGS) $(LDFLAGS_EX) $(LIBS) -o $@$(X)

install: all installdirs
	$(INSTALL_PROGRAM) mdb_resetxlog$(X) '$(DESTDIR)$(bindir)/mdb_resetxlog$(X)'

installdirs:
	$(MKDIR_P) '$(DESTDIR)$(bindir)'

uninstall:
	rm -f '$(DESTDIR)$(bindir)/mdb_resetxlog$(X)'

clean distclean maintainer-clean:
	rm -f mdb_resetxlog$(X) $(OBJS)
