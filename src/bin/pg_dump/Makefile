#-------------------------------------------------------------------------
#
# Makefile for src/bin/mdb_dump
#
# Portions Copyright (c) 1996-2016, MollyDB Global Development Group
# Portions Copyright (c) 1994, Regents of the University of California
#
# src/bin/mdb_dump/Makefile
#
#-------------------------------------------------------------------------

PGFILEDESC = "mdb_dump/mdb_restore/mdb_dumpall - backup and restore MollyDB databases"
PGAPPICON=win32

subdir = src/bin/mdb_dump
top_builddir = ../../..
include $(top_builddir)/src/Makefile.global

override CPPFLAGS := -I$(libpq_srcdir) $(CPPFLAGS)
LDFLAGS += -L$(top_builddir)/src/fe_utils -lpgfeutils -lpq

OBJS=	mdb_backup_archiver.o mdb_backup_db.o mdb_backup_custom.o \
	mdb_backup_null.o mdb_backup_tar.o mdb_backup_directory.o \
	mdb_backup_utils.o parallel.o compress_io.o dumputils.o $(WIN32RES)

all: mdb_dump mdb_restore mdb_dumpall

mdb_dump: mdb_dump.o common.o mdb_dump_sort.o $(OBJS) | submake-libpq submake-libpgport submake-libpgfeutils
	$(CC) $(CFLAGS) mdb_dump.o common.o mdb_dump_sort.o $(OBJS) $(libpq_pgport) $(LDFLAGS) $(LDFLAGS_EX) $(LIBS) -o $@$(X)

mdb_restore: mdb_restore.o $(OBJS) | submake-libpq submake-libpgport submake-libpgfeutils
	$(CC) $(CFLAGS) mdb_restore.o $(OBJS) $(libpq_pgport) $(LDFLAGS) $(LDFLAGS_EX) $(LIBS) -o $@$(X)

mdb_dumpall: mdb_dumpall.o dumputils.o | submake-libpq submake-libpgport submake-libpgfeutils
	$(CC) $(CFLAGS) mdb_dumpall.o dumputils.o $(WIN32RES) $(libpq_pgport) $(LDFLAGS) $(LDFLAGS_EX) $(LIBS) -o $@$(X)

install: all installdirs
	$(INSTALL_PROGRAM) mdb_dump$(X) '$(DESTDIR)$(bindir)'/mdb_dump$(X)
	$(INSTALL_PROGRAM) mdb_restore$(X) '$(DESTDIR)$(bindir)'/mdb_restore$(X)
	$(INSTALL_PROGRAM) mdb_dumpall$(X) '$(DESTDIR)$(bindir)'/mdb_dumpall$(X)

installdirs:
	$(MKDIR_P) '$(DESTDIR)$(bindir)'

check:
	$(prove_check)

installcheck:
	$(prove_installcheck)

uninstall:
	rm -f $(addprefix '$(DESTDIR)$(bindir)'/, mdb_dump$(X) mdb_restore$(X) mdb_dumpall$(X))

clean distclean maintainer-clean:
	rm -f mdb_dump$(X) mdb_restore$(X) mdb_dumpall$(X) $(OBJS) mdb_dump.o common.o mdb_dump_sort.o mdb_restore.o mdb_dumpall.o
	rm -rf tmp_check
