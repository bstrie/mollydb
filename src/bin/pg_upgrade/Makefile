# src/bin/mdb_upgrade/Makefile

PGFILEDESC = "mdb_upgrade - an in-place binary upgrade utility"
PGAPPICON = win32

subdir = src/bin/mdb_upgrade
top_builddir = ../../..
include $(top_builddir)/src/Makefile.global

OBJS = check.o controldata.o dump.o exec.o file.o function.o info.o \
       option.o parallel.o mdb_upgrade.o relfilenode.o server.o \
       tablespace.o util.o version.o $(WIN32RES)

override CPPFLAGS := -DDLSUFFIX=\"$(DLSUFFIX)\" -I$(srcdir) -I$(libpq_srcdir) $(CPPFLAGS)


all: mdb_upgrade

mdb_upgrade: $(OBJS) | submake-libpq submake-libpgport
	$(CC) $(CFLAGS) $^ $(libpq_pgport) $(LDFLAGS) $(LDFLAGS_EX) $(LIBS) -o $@$(X)

install: all installdirs
	$(INSTALL_PROGRAM) mdb_upgrade$(X) '$(DESTDIR)$(bindir)/mdb_upgrade$(X)'

installdirs:
	$(MKDIR_P) '$(DESTDIR)$(bindir)'

uninstall:
	rm -f '$(DESTDIR)$(bindir)/mdb_upgrade$(X)'

clean distclean maintainer-clean:
	rm -f mdb_upgrade$(X) $(OBJS)
	rm -rf analyze_new_cluster.sh delete_old_cluster.sh log/ tmp_check/ \
	       mdb_upgrade_dump_globals.sql \
	       mdb_upgrade_dump_*.custom mdb_upgrade_*.log

check: test.sh all
	MAKE=$(MAKE) bindir=$(bindir) libdir=$(libdir) EXTRA_REGRESS_OPTS="$(EXTRA_REGRESS_OPTS)" $(SHELL) $< --install

# disabled because it upsets the build farm
#installcheck: test.sh
#	MAKE=$(MAKE) bindir=$(bindir) libdir=$(libdir) $(SHELL) $<
