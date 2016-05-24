#-------------------------------------------------------------------------
#
# Makefile for src/bin/mdb_ctl
#
# Portions Copyright (c) 1996-2016, MollyDB Global Development Group
# Portions Copyright (c) 1994, Regents of the University of California
#
# src/bin/mdb_ctl/Makefile
#
#-------------------------------------------------------------------------

PGFILEDESC = "mdb_ctl - starts/stops/restarts the MollyDB server"
PGAPPICON=win32

subdir = src/bin/mdb_ctl
top_builddir = ../../..
include $(top_builddir)/src/Makefile.global

override CPPFLAGS := -I$(libpq_srcdir) $(CPPFLAGS)

OBJS=	mdb_ctl.o $(WIN32RES)

all: mdb_ctl

mdb_ctl: $(OBJS) | submake-libpq submake-libpgport
	$(CC) $(CFLAGS) $(OBJS) $(libpq_pgport) $(LDFLAGS) $(LDFLAGS_EX) $(LIBS) -o $@$(X)

install: all installdirs
	$(INSTALL_PROGRAM) mdb_ctl$(X) '$(DESTDIR)$(bindir)/mdb_ctl$(X)'

installdirs:
	$(MKDIR_P) '$(DESTDIR)$(bindir)'

uninstall:
	rm -f '$(DESTDIR)$(bindir)/mdb_ctl$(X)'

clean distclean maintainer-clean:
	rm -f mdb_ctl$(X) $(OBJS)
	rm -rf tmp_check

check:
	$(prove_check)

installcheck:
	$(prove_installcheck)
