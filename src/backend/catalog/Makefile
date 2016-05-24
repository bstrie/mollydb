#-------------------------------------------------------------------------
#
# Makefile for backend/catalog
#
# src/backend/catalog/Makefile
#
#-------------------------------------------------------------------------

subdir = src/backend/catalog
top_builddir = ../../..
include $(top_builddir)/src/Makefile.global

OBJS = catalog.o dependency.o heap.o index.o indexing.o namespace.o aclchk.o \
       objectaccess.o objectaddress.o mdb_aggregate.o mdb_collation.o \
       mdb_constraint.o mdb_conversion.o \
       mdb_depend.o mdb_enum.o mdb_inherits.o mdb_largeobject.o mdb_namespace.o \
       mdb_operator.o mdb_proc.o mdb_range.o mdb_db_role_setting.o mdb_shdepend.o \
       mdb_type.o storage.o toasting.o

BKIFILES = mollydb.bki mollydb.description mollydb.shdescription

include $(top_srcdir)/src/backend/common.mk

all: $(BKIFILES) schemapg.h

# Note: there are some undocumented dependencies on the ordering in which
# the catalog header files are assembled into mollydb.bki.  In particular,
# indexing.h had better be last, and toasting.h just before it.

POSTGRES_BKI_SRCS = $(addprefix $(top_srcdir)/src/include/catalog/,\
	mdb_proc.h mdb_type.h mdb_attribute.h mdb_class.h \
	mdb_attrdef.h mdb_constraint.h mdb_inherits.h mdb_index.h mdb_operator.h \
	mdb_opfamily.h mdb_opclass.h mdb_am.h mdb_amop.h mdb_amproc.h \
	mdb_language.h mdb_largeobject_metadata.h mdb_largeobject.h mdb_aggregate.h \
	mdb_statistic.h mdb_rewrite.h mdb_trigger.h mdb_event_trigger.h mdb_description.h \
	mdb_cast.h mdb_enum.h mdb_namespace.h mdb_conversion.h mdb_depend.h \
	mdb_database.h mdb_db_role_setting.h mdb_tablespace.h mdb_pltemplate.h \
	mdb_authid.h mdb_auth_members.h mdb_shdepend.h mdb_shdescription.h \
	mdb_ts_config.h mdb_ts_config_map.h mdb_ts_dict.h \
	mdb_ts_parser.h mdb_ts_template.h mdb_extension.h \
	mdb_foreign_data_wrapper.h mdb_foreign_server.h mdb_user_mapping.h \
	mdb_foreign_table.h mdb_policy.h mdb_replication_origin.h \
	mdb_default_acl.h mdb_init_privs.h mdb_seclabel.h mdb_shseclabel.h \
	mdb_collation.h mdb_range.h mdb_transform.h \
	toasting.h indexing.h \
    )

# location of Catalog.pm
catalogdir = $(top_srcdir)/src/backend/catalog

# locations of headers that genbki.pl needs to read
mdb_includes = -I$(top_srcdir)/src/include/catalog -I$(top_builddir)/src/include/catalog

# see explanation in ../parser/Makefile
mollydb.description: mollydb.bki ;

mollydb.shdescription: mollydb.bki ;

schemapg.h: mollydb.bki ;

# Technically, this should depend on Makefile.global, but then
# mollydb.bki would need to be rebuilt after every configure run,
# even in distribution tarballs.  So this is cheating a bit, but it
# will achieve the goal of updating the version number when it
# changes.
mollydb.bki: genbki.pl Catalog.pm $(POSTGRES_BKI_SRCS) $(top_srcdir)/configure $(top_srcdir)/src/include/catalog/duplicate_oids
	cd $(top_srcdir)/src/include/catalog && $(PERL) ./duplicate_oids
	$(PERL) -I $(catalogdir) $< $(mdb_includes) --set-version=$(MAJORVERSION) $(POSTGRES_BKI_SRCS)

.PHONY: install-data
install-data: $(BKIFILES) installdirs
	$(INSTALL_DATA) $(call vpathsearch,mollydb.bki) '$(DESTDIR)$(datadir)/mollydb.bki'
	$(INSTALL_DATA) $(call vpathsearch,mollydb.description) '$(DESTDIR)$(datadir)/mollydb.description'
	$(INSTALL_DATA) $(call vpathsearch,mollydb.shdescription) '$(DESTDIR)$(datadir)/mollydb.shdescription'
	$(INSTALL_DATA) $(srcdir)/system_views.sql '$(DESTDIR)$(datadir)/system_views.sql'
	$(INSTALL_DATA) $(srcdir)/information_schema.sql '$(DESTDIR)$(datadir)/information_schema.sql'
	$(INSTALL_DATA) $(srcdir)/sql_features.txt '$(DESTDIR)$(datadir)/sql_features.txt'

installdirs:
	$(MKDIR_P) '$(DESTDIR)$(datadir)'

.PHONY: uninstall-data
uninstall-data:
	rm -f $(addprefix '$(DESTDIR)$(datadir)'/, $(BKIFILES) system_views.sql information_schema.sql sql_features.txt)

# mollydb.bki, mollydb.description, mollydb.shdescription, and schemapg.h
# are in the distribution tarball, so they are not cleaned here.
clean:

maintainer-clean: clean
	rm -f $(BKIFILES)
