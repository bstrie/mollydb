# src/bin/mdb_test_fsync/Makefile

PGFILEDESC = "mdb_test_fsync - test various disk sync methods"
PGAPPICON = win32

subdir = src/bin/mdb_test_fsync
top_builddir = ../../..
include $(top_builddir)/src/Makefile.global

OBJS = mdb_test_fsync.o $(WIN32RES)

all: mdb_test_fsync

mdb_test_fsync: $(OBJS) | submake-libpgport
	$(CC) $(CFLAGS) $^ $(LDFLAGS) $(LDFLAGS_EX) $(LIBS) -o $@$(X)

install: all installdirs
	$(INSTALL_PROGRAM) mdb_test_fsync$(X) '$(DESTDIR)$(bindir)/mdb_test_fsync$(X)'

installdirs:
	$(MKDIR_P) '$(DESTDIR)$(bindir)'

uninstall:
	rm -f '$(DESTDIR)$(bindir)/mdb_test_fsync$(X)'

clean distclean maintainer-clean:
	rm -f mdb_test_fsync$(X) $(OBJS)
