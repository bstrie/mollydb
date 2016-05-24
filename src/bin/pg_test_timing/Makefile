# src/bin/mdb_test_timing/Makefile

PGFILEDESC = "mdb_test_timing - test timing overhead"
PGAPPICON = win32

subdir = src/bin/mdb_test_timing
top_builddir = ../../..
include $(top_builddir)/src/Makefile.global

OBJS = mdb_test_timing.o $(WIN32RES)

all: mdb_test_timing

mdb_test_timing: $(OBJS) | submake-libpgport
	$(CC) $(CFLAGS) $^ $(LDFLAGS) $(LDFLAGS_EX) $(LIBS) -o $@$(X)

install: all installdirs
	$(INSTALL_PROGRAM) mdb_test_timing$(X) '$(DESTDIR)$(bindir)/mdb_test_timing$(X)'

installdirs:
	$(MKDIR_P) '$(DESTDIR)$(bindir)'

uninstall:
	rm -f '$(DESTDIR)$(bindir)/mdb_test_timing$(X)'

clean distclean maintainer-clean:
	rm -f mdb_test_timing$(X) $(OBJS)
