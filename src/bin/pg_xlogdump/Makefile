# src/bin/mdb_xlogdump/Makefile

PGFILEDESC = "mdb_xlogdump - decode and display WAL"
PGAPPICON=win32

subdir = src/bin/mdb_xlogdump
top_builddir = ../../..
include $(top_builddir)/src/Makefile.global

OBJS = mdb_xlogdump.o compat.o xlogreader.o rmgrdesc.o \
	$(RMGRDESCOBJS) $(WIN32RES)

override CPPFLAGS := -DFRONTEND $(CPPFLAGS)

RMGRDESCSOURCES = $(sort $(notdir $(wildcard $(top_srcdir)/src/backend/access/rmgrdesc/*desc.c)))
RMGRDESCOBJS = $(patsubst %.c,%.o,$(RMGRDESCSOURCES))


all: mdb_xlogdump

mdb_xlogdump: $(OBJS) | submake-libpgport
	$(CC) $(CFLAGS) $^ $(LDFLAGS) $(LDFLAGS_EX) $(LIBS) -o $@$(X)

xlogreader.c: % : $(top_srcdir)/src/backend/access/transam/%
	rm -f $@ && $(LN_S) $< .

$(RMGRDESCSOURCES): % : $(top_srcdir)/src/backend/access/rmgrdesc/%
	rm -f $@ && $(LN_S) $< .

install: all installdirs
	$(INSTALL_PROGRAM) mdb_xlogdump$(X) '$(DESTDIR)$(bindir)/mdb_xlogdump$(X)'

installdirs:
	$(MKDIR_P) '$(DESTDIR)$(bindir)'

uninstall:
	rm -f '$(DESTDIR)$(bindir)/mdb_xlogdump$(X)'

clean distclean maintainer-clean:
	rm -f mdb_xlogdump$(X) $(OBJS) $(RMGRDESCSOURCES) xlogreader.c
