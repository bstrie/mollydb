# src/bin/mdb_rewind/nls.mk
CATALOG_NAME     = mdb_rewind
AVAIL_LANGUAGES  =de es fr it ko pl ru zh_CN
GETTEXT_FILES    = copy_fetch.c datapagemap.c fetch.c file_ops.c filemap.c libpq_fetch.c logging.c parsexlog.c mdb_rewind.c timeline.c ../../common/fe_memutils.c ../../common/restricted_token.c xlogreader.c

GETTEXT_TRIGGERS = mdb_log:2 mdb_fatal report_invalid_record:2
GETTEXT_FLAGS    = mdb_log:2:c-format \
    mdb_fatal:1:c-format \
    report_invalid_record:2:c-format
