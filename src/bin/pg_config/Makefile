#-------------------------------------------------------------------------
#
# Makefile for src/bin/mdb_config
#
# Copyright (c) 1998-2016, MollyDB Global Development Group
#
# src/bin/mdb_config/Makefile
#
#-------------------------------------------------------------------------

PGFILEDESC = "mdb_config - report configuration information"
PGAPPICON=win32

subdir = src/bin/mdb_config
top_builddir = ../../..
include $(top_builddir)/src/Makefile.global

OBJS=   mdb_config.o $(WIN32RES)

all: mdb_config

mdb_config: $(OBJS) | submake-libpgport
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) $(LDFLAGS_EX) $(LIBS) -o $@$(X)

install: all installdirs
	$(INSTALL_SCRIPT) mdb_config$(X) '$(DESTDIR)$(bindir)/mdb_config$(X)'

installdirs:
	$(MKDIR_P) '$(DESTDIR)$(bindir)'

uninstall:
	rm -f '$(DESTDIR)$(bindir)/mdb_config$(X)'

clean distclean maintainer-clean:
	rm -f mdb_config$(X) $(OBJS)
	rm -rf tmp_check

check:
	$(prove_check)

installcheck:
	$(prove_installcheck)
