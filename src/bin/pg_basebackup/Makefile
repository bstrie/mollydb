#-------------------------------------------------------------------------
#
# Makefile for src/bin/mdb_basebackup
#
# Portions Copyright (c) 1996-2016, MollyDB Global Development Group
# Portions Copyright (c) 1994, Regents of the University of California
#
# src/bin/mdb_basebackup/Makefile
#
#-------------------------------------------------------------------------

PGFILEDESC = "mdb_basebackup/mdb_receivexlog/mdb_recvlogical - streaming WAL and backup receivers"
PGAPPICON=win32

subdir = src/bin/mdb_basebackup
top_builddir = ../../..
include $(top_builddir)/src/Makefile.global

override CPPFLAGS := -I$(libpq_srcdir) $(CPPFLAGS)

OBJS=receivelog.o streamutil.o $(WIN32RES)

all: mdb_basebackup mdb_receivexlog mdb_recvlogical

mdb_basebackup: mdb_basebackup.o $(OBJS) | submake-libpq submake-libpgport
	$(CC) $(CFLAGS) mdb_basebackup.o $(OBJS) $(libpq_pgport) $(LDFLAGS) $(LDFLAGS_EX) $(LIBS) -o $@$(X)

mdb_receivexlog: mdb_receivexlog.o $(OBJS) | submake-libpq submake-libpgport
	$(CC) $(CFLAGS) mdb_receivexlog.o $(OBJS) $(libpq_pgport) $(LDFLAGS) $(LDFLAGS_EX) $(LIBS) -o $@$(X)

mdb_recvlogical: mdb_recvlogical.o $(OBJS) | submake-libpq submake-libpgport
	$(CC) $(CFLAGS) mdb_recvlogical.o $(OBJS) $(libpq_pgport) $(LDFLAGS) $(LDFLAGS_EX) $(LIBS) -o $@$(X)

install: all installdirs
	$(INSTALL_PROGRAM) mdb_basebackup$(X) '$(DESTDIR)$(bindir)/mdb_basebackup$(X)'
	$(INSTALL_PROGRAM) mdb_receivexlog$(X) '$(DESTDIR)$(bindir)/mdb_receivexlog$(X)'
	$(INSTALL_PROGRAM) mdb_recvlogical$(X) '$(DESTDIR)$(bindir)/mdb_recvlogical$(X)'

installdirs:
	$(MKDIR_P) '$(DESTDIR)$(bindir)'

uninstall:
	rm -f '$(DESTDIR)$(bindir)/mdb_basebackup$(X)'
	rm -f '$(DESTDIR)$(bindir)/mdb_receivexlog$(X)'
	rm -f '$(DESTDIR)$(bindir)/mdb_recvlogical$(X)'

clean distclean maintainer-clean:
	rm -f mdb_basebackup$(X) mdb_receivexlog$(X) mdb_recvlogical$(X) \
		mdb_basebackup.o mdb_receivexlog.o mdb_recvlogical.o \
		$(OBJS)
	rm -rf tmp_check

check:
	$(prove_check)

installcheck:
	$(prove_installcheck)
