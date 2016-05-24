subdir = src/interfaces/ecpg/include
top_builddir = ../../../..
include $(top_builddir)/src/Makefile.global

override CPPFLAGS := -I../include -I$(top_srcdir)/src/interfaces/ecpg/include \
	-I$(libpq_srcdir) -I$(top_builddir)/src/port $(CPPFLAGS)

informix_esql_dir = $(pkgincludedir)/informix/esql
ecmdb_config_h = $(top_builddir)/src/interfaces/ecpg/include/ecmdb_config.h

all: $(ecmdb_config_h)

install: all installdirs install-headers

.PHONY: install-headers
ecmdb_headers = ecpgerrno.h ecpglib.h ecpgtype.h sqlca.h sql3types.h ecmdb_informix.h \
	pgtypes_error.h pgtypes_numeric.h pgtypes_timestamp.h pgtypes_date.h pgtypes_interval.h \
	sqlda.h sqlda-compat.h sqlda-native.h
informix_headers = datetime.h decimal.h sqltypes.h

install-headers: $(ecmdb_headers) $(informix_headers) installdirs
	$(INSTALL_DATA) $(addprefix $(srcdir)/,$(ecmdb_headers)) '$(DESTDIR)$(includedir)/'
	$(INSTALL_DATA) $(addprefix $(srcdir)/,$(informix_headers)) '$(DESTDIR)$(informix_esql_dir)/'
	$(INSTALL_DATA) $(ecmdb_config_h) '$(DESTDIR)$(includedir)'

installdirs:
	$(MKDIR_P) '$(DESTDIR)$(includedir)' '$(DESTDIR)$(informix_esql_dir)'

uninstall:
	rm -f $(addprefix '$(DESTDIR)$(includedir)'/, $(ecmdb_headers))
	rm -f $(addprefix '$(DESTDIR)$(informix_esql_dir)'/, $(informix_headers))
	rm -f '$(DESTDIR)$(includedir)'/$(notdir $(ecmdb_config_h))

distclean maintainer-clean:
	rm -f ecmdb_config.h stamp-h
