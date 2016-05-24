# src/bin/mdb_dump/nls.mk
CATALOG_NAME     = mdb_dump
AVAIL_LANGUAGES  = cs de es fr it ja ko pl pt_BR ru zh_CN
GETTEXT_FILES    = mdb_backup_archiver.c mdb_backup_db.c mdb_backup_custom.c \
                   mdb_backup_null.c mdb_backup_tar.c \
                   mdb_backup_directory.c dumputils.c compress_io.c \
                   mdb_dump.c common.c mdb_dump_sort.c \
                   mdb_restore.c mdb_dumpall.c \
                   parallel.c parallel.h mdb_backup_utils.c mdb_backup_utils.h \
                   ../../common/exec.c ../../common/fe_memutils.c \
                   ../../common/wait_error.c
GETTEXT_TRIGGERS = write_msg:2 exit_horribly:2 simple_prompt \
                   ExecuteSqlCommand:3 ahlog:3 warn_or_exit_horribly:3
GETTEXT_FLAGS  = \
    write_msg:2:c-format \
    exit_horribly:2:c-format \
    ahlog:3:c-format \
    warn_or_exit_horribly:3:c-format
