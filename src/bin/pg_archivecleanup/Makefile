# src/bin/mdb_archivecleanup/Makefile

PGFILEDESC = "mdb_archivecleanup - cleans archive when used with streaming replication"
PGAPPICON = win32

subdir = src/bin/mdb_archivecleanup
top_builddir = ../../..
include $(top_builddir)/src/Makefile.global

OBJS	= mdb_archivecleanup.o $(WIN32RES)

all: mdb_archivecleanup

mdb_archivecleanup: $(OBJS) | submake-libpgport
	$(CC) $(CFLAGS) $^ $(LDFLAGS) $(LDFLAGS_EX) $(LIBS) -o $@$(X)

install: all installdirs
	$(INSTALL_PROGRAM) mdb_archivecleanup$(X) '$(DESTDIR)$(bindir)/mdb_archivecleanup$(X)'

installdirs:
	$(MKDIR_P) '$(DESTDIR)$(bindir)'

uninstall:
	rm -f '$(DESTDIR)$(bindir)/mdb_archivecleanup$(X)'

clean distclean maintainer-clean:
	rm -f mdb_archivecleanup$(X) $(OBJS)
