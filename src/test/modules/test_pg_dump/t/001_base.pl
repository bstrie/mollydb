use strict;
use warnings;

use Config;
use MollyDBNode;
use TestLib;
use Test::More;

my $tempdir       = TestLib::tempdir;
my $tempdir_short = TestLib::tempdir_short;

###############################################################
# This structure is based off of the src/bin/mdb_dump/t test
# suite.
###############################################################
# Definition of the mdb_dump runs to make.
#
# Each of these runs are named and those names are used below
# to define how each test should (or shouldn't) treat a result
# from a given run.
#
# test_key indicates that a given run should simply use the same
# set of like/unlike tests as another run, and which run that is.
#
# dump_cmd is the mdb_dump command to run, which is an array of
# the full command and arguments to run.  Note that this is run
# using $node->command_ok(), so the port does not need to be
# specified and is pulled from $PGPORT, which is set by the
# MollyDBNode system.
#
# restore_cmd is the mdb_restore command to run, if any.  Note
# that this should generally be used when the mdb_dump goes to
# a non-text file and that the restore can then be used to
# generate a text file to run through the tests from the
# non-text file generated by mdb_dump.
#
# TODO: Have mdb_restore actually restore to an independent
# database and then mdb_dump *that* database (or something along
# those lines) to validate that part of the process.

my %pgdump_runs = (
	binary_upgrade => {
		dump_cmd => [
			'mdb_dump',
			'-f', "$tempdir/binary_upgrade.sql",
			'--schema-only',
			'--binary-upgrade',
			'-d', 'mollydb', # alternative way to specify database
		],
	},
	clean => {
		dump_cmd => [
			'mdb_dump',
			'-f', "$tempdir/clean.sql",
			'-c',
			'-d', 'mollydb', # alternative way to specify database
		],
	},
	clean_if_exists => {
		dump_cmd => [
			'mdb_dump',
			'-f', "$tempdir/clean_if_exists.sql",
			'-c',
			'--if-exists',
			'-E', 'UTF8', # no-op, just tests that option is accepted
			'mollydb',
		],
	},
	column_inserts => {
		dump_cmd => [
			'mdb_dump',
			'-f', "$tempdir/column_inserts.sql",
			'-a',
			'--column-inserts',
			'mollydb',
		],
	},
	createdb => {
		dump_cmd => [
			'mdb_dump',
			'-f', "$tempdir/createdb.sql",
			'-C',
			'-R', # no-op, just for testing
			'mollydb',
		],
	},
	data_only => {
		dump_cmd => [
			'mdb_dump',
			'-f', "$tempdir/data_only.sql",
			'-a',
			'-v', # no-op, just make sure it works
			'mollydb',
		],
	},
	defaults => {
		dump_cmd => [
			'mdb_dump',
			'-f', "$tempdir/defaults.sql",
			'mollydb',
		],
	},
	defaults_custom_format => {
		test_key => 'defaults',
		dump_cmd => [
			'mdb_dump',
			'-Fc',
			'-Z6',
			'-f', "$tempdir/defaults_custom_format.dump",
			'mollydb',
		],
		restore_cmd => [
			'mdb_restore',
			'-f', "$tempdir/defaults_custom_format.sql",
			"$tempdir/defaults_custom_format.dump",
		],
	},
	defaults_dir_format => {
		test_key => 'defaults',
		dump_cmd => [
			'mdb_dump',
			'-Fd',
			'-f', "$tempdir/defaults_dir_format",
			'mollydb',
		],
		restore_cmd => [
			'mdb_restore',
			'-f', "$tempdir/defaults_dir_format.sql",
			"$tempdir/defaults_dir_format",
		],
	},
	defaults_parallel => {
		test_key => 'defaults',
		dump_cmd => [
			'mdb_dump',
			'-Fd',
			'-j2',
			'-f', "$tempdir/defaults_parallel",
			'mollydb',
		],
		restore_cmd => [
			'mdb_restore',
			'-f', "$tempdir/defaults_parallel.sql",
			"$tempdir/defaults_parallel",
		],
	},
	defaults_tar_format => {
		test_key => 'defaults',
		dump_cmd => [
			'mdb_dump',
			'-Ft',
			'-f', "$tempdir/defaults_tar_format.tar",
			'mollydb',
		],
		restore_cmd => [
			'mdb_restore',
			'-f', "$tempdir/defaults_tar_format.sql",
			"$tempdir/defaults_tar_format.tar",
		],
	},
	mdb_dumpall_globals => {
		dump_cmd => [
			'mdb_dumpall',
			'-f', "$tempdir/mdb_dumpall_globals.sql",
			'-g',
		],
	},
	no_privs => {
		dump_cmd => [
			'mdb_dump',
			'-f', "$tempdir/no_privs.sql",
			'-x',
			'mollydb',
		],
	},
	no_owner => {
		dump_cmd => [
			'mdb_dump',
			'-f', "$tempdir/no_owner.sql",
			'-O',
			'mollydb',
		],
	},
	schema_only => {
		dump_cmd => [
			'mdb_dump',
			'-f', "$tempdir/schema_only.sql",
			'-s',
			'mollydb',
		],
	},
	section_pre_data => {
		dump_cmd => [
			'mdb_dump',
			'-f', "$tempdir/section_pre_data.sql",
			'--section=pre-data',
			'mollydb',
		],
	},
	section_data => {
		dump_cmd => [
			'mdb_dump',
			'-f', "$tempdir/section_data.sql",
			'--section=data',
			'mollydb',
		],
	},
	section_post_data => {
		dump_cmd => [
			'mdb_dump',
			'-f', "$tempdir/section_post_data.sql",
			'--section=post-data',
			'mollydb',
		],
	},
);

###############################################################
# Definition of the tests to run.
#
# Each test is defined using the log message that will be used.
#
# A regexp should be defined for each test which provides the
# basis for the test.  That regexp will be run against the output
# file of each of the runs which the test is to be run against
# and the success of the result will depend on if the regexp
# result matches the expected 'like' or 'unlike' case.
#
# For each test, there are two sets of runs defined, one for
# the 'like' tests and one for the 'unlike' tests.  'like'
# essentially means "the regexp for this test must match the
# output file".  'unlike' is the opposite.
#
# There are a few 'catch-all' tests which can be used to have
# a single, simple, test to over a range of other tests.  For
# example, there is a '^CREATE ' test, which is used for the
# 'data-only' test as there should never be any kind of CREATE
# statement in a 'data-only' run.  Without the catch-all, we
# would have to list the 'data-only' run in each and every
# 'CREATE xxxx' test, which would be a lot of additional tests.
#
# Note that it makes no sense for the same run to ever be listed
# in both 'like' and 'unlike' categories.
#
# There can then be a 'create_sql' and 'create_order' for a
# given test.  The 'create_sql' commands are collected up in
# 'create_order' and then run against the database prior to any
# of the mdb_dump runs happening.  This is what "seeds" the
# system with objects to be dumped out.
#
# Building of this hash takes a bit of time as all of the regexps
# included in it are compiled.  This greatly improves performance
# as the regexps are used for each run the test applies to.

my %tests = (
	'CREATE EXTENSION test_mdb_dump' => {
		create_order => 2,
		create_sql => 'CREATE EXTENSION test_mdb_dump;',
		regexp => qr/^
			\QCREATE EXTENSION IF NOT EXISTS test_mdb_dump WITH SCHEMA public;\E
			$/xm,
		like => {
			clean => 1,
			clean_if_exists => 1,
			createdb => 1,
			defaults => 1,
			no_privs => 1,
			no_owner => 1,
			schema_only => 1,
			section_pre_data => 1,
		},
		unlike => {
			binary_upgrade => 1,
			mdb_dumpall_globals => 1,
			section_post_data => 1,
		},
	},
	'CREATE ROLE dump_test' => {
		create_order => 1,
		create_sql => 'CREATE ROLE dump_test;',
		regexp => qr/^CREATE ROLE dump_test;$/m,
		like => {
			mdb_dumpall_globals => 1,
		},
		unlike => {
			binary_upgrade => 1,
			clean => 1,
			clean_if_exists => 1,
			createdb => 1,
			defaults => 1,
			no_privs => 1,
			no_owner => 1,
			schema_only => 1,
			section_pre_data => 1,
			section_post_data => 1,
		},
	},
	'CREATE TABLE regress_mdb_dump_table' => {
		regexp => qr/^
			\QCREATE TABLE regress_mdb_dump_table (\E
			\n\s+\Qcol1 integer,\E
			\n\s+\Qcol2 integer\E
			\n\);$/xm,
		like => {
			binary_upgrade => 1,
		},
		unlike => {
			clean => 1,
			clean_if_exists => 1,
			createdb => 1,
			defaults => 1,
			no_privs => 1,
			no_owner => 1,
			mdb_dumpall_globals => 1,
			schema_only => 1,
			section_pre_data => 1,
			section_post_data => 1,
		},
	},
	'COMMENT ON EXTENSION test_mdb_dump' => {
		regexp => qr/^
			\QCOMMENT ON EXTENSION test_mdb_dump \E
			\QIS 'Test mdb_dump with an extension';\E
			$/xm,
		like => {
			binary_upgrade => 1,
			clean => 1,
			clean_if_exists => 1,
			createdb => 1,
			defaults => 1,
			no_privs => 1,
			no_owner => 1,
			schema_only => 1,
			section_pre_data => 1,
		},
		unlike => {
			mdb_dumpall_globals => 1,
			section_post_data => 1,
		},
	},
	'GRANT SELECT ON TABLE regress_mdb_dump_table' => {
		regexp => qr/^
			\QSELECT binary_upgrade_set_record_init_privs(true);\E\n
			\QGRANT SELECT ON TABLE regress_mdb_dump_table TO dump_test;\E\n
			\QSELECT binary_upgrade_set_record_init_privs(false);\E
			$/xms,
		like => {
			binary_upgrade => 1,
		},
		unlike => {
			clean => 1,
			clean_if_exists => 1,
			createdb => 1,
			defaults => 1,
			no_owner => 1,
			schema_only => 1,
			section_pre_data => 1,
			no_privs => 1,
			mdb_dumpall_globals => 1,
			section_post_data => 1,
		},
	},
	'GRANT SELECT(col1) ON regress_mdb_dump_table' => {
		regexp => qr/^
			\QSELECT binary_upgrade_set_record_init_privs(true);\E\n
			\QGRANT SELECT(col1) ON TABLE regress_mdb_dump_table TO PUBLIC;\E\n
			\QSELECT binary_upgrade_set_record_init_privs(false);\E
			$/xms,
		like => {
			binary_upgrade => 1,
		},
		unlike => {
			clean => 1,
			clean_if_exists => 1,
			createdb => 1,
			defaults => 1,
			no_owner => 1,
			schema_only => 1,
			section_pre_data => 1,
			no_privs => 1,
			mdb_dumpall_globals => 1,
			section_post_data => 1,
		},
	},
	'GRANT SELECT(col2) ON regress_mdb_dump_table TO dump_test' => {
		create_order => 4,
		create_sql => 'GRANT SELECT(col2) ON regress_mdb_dump_table
						   TO dump_test;',
		regexp => qr/^
			\QGRANT SELECT(col2) ON TABLE regress_mdb_dump_table TO dump_test;\E
			$/xm,
		like => {
			binary_upgrade => 1,
			clean => 1,
			clean_if_exists => 1,
			createdb => 1,
			defaults => 1,
			no_owner => 1,
			schema_only => 1,
			section_pre_data => 1,
		},
		unlike => {
			no_privs => 1,
			mdb_dumpall_globals => 1,
			section_post_data => 1,
		},
	},
	'REVOKE SELECT(col1) ON regress_mdb_dump_table' => {
		create_order => 3,
		create_sql => 'REVOKE SELECT(col1) ON regress_mdb_dump_table
						   FROM PUBLIC;',
		regexp => qr/^
			\QREVOKE SELECT(col1) ON TABLE regress_mdb_dump_table FROM PUBLIC;\E
			$/xm,
		like => {
			binary_upgrade => 1,
			clean => 1,
			clean_if_exists => 1,
			createdb => 1,
			defaults => 1,
			no_owner => 1,
			schema_only => 1,
			section_pre_data => 1,
		},
		unlike => {
			no_privs => 1,
			mdb_dumpall_globals => 1,
			section_post_data => 1,
		},
	},
);

#########################################
# Create a PG instance to test actually dumping from

my $node = get_new_node('main');
$node->init;
$node->start;

my $port = $node->port;

my $num_tests = 0;

foreach my $run (sort keys %pgdump_runs) {
	my $test_key = $run;

	# Each run of mdb_dump is a test itself
	$num_tests++;

	# If there is a restore cmd, that's another test
	if ($pgdump_runs{$run}->{restore_cmd}) {
		$num_tests++;
	}

	if ($pgdump_runs{$run}->{test_key}) {
		$test_key = $pgdump_runs{$run}->{test_key};
	}

	# Then count all the tests run against each run
	foreach my $test (sort keys %tests) {
		if ($tests{$test}->{like}->{$test_key}) {
			$num_tests++;
		}

		if ($tests{$test}->{unlike}->{$test_key}) {
			$num_tests++;
		}
	}
}
plan tests => $num_tests;

#########################################
# Set up schemas, tables, etc, to be dumped.

# Build up the create statements
my $create_sql = '';

foreach my $test (
	sort {
		if ($tests{$a}->{create_order} and $tests{$b}->{create_order}) {
			$tests{$a}->{create_order} <=> $tests{$b}->{create_order};
		} elsif ($tests{$a}->{create_order}) {
			-1;
		} elsif ($tests{$b}->{create_order}) {
			1;
		} else {
			0;
		}
	} keys %tests) {
	if ($tests{$test}->{create_sql}) {
		$create_sql .= $tests{$test}->{create_sql};
	}
}

# Send the combined set of commands to psql
$node->safe_psql('mollydb', $create_sql);

#########################################
# Run all runs

foreach my $run (sort keys %pgdump_runs) {

	my $test_key = $run;

	$node->command_ok(\@{ $pgdump_runs{$run}->{dump_cmd} }, "$run: mdb_dump runs");

	if ($pgdump_runs{$run}->{restore_cmd}) {
		$node->command_ok(\@{ $pgdump_runs{$run}->{restore_cmd} }, "$run: mdb_restore runs");
	}

	if ($pgdump_runs{$run}->{test_key}) {
		$test_key = $pgdump_runs{$run}->{test_key};
	}

	my $output_file = slurp_file("$tempdir/${run}.sql");

	#########################################
	# Run all tests where this run is included
	# as either a 'like' or 'unlike' test.

	foreach my $test (sort keys %tests) {
		if ($tests{$test}->{like}->{$test_key}) {
			like($output_file, $tests{$test}->{regexp}, "$run: dumps $test");
		}

		if ($tests{$test}->{unlike}->{$test_key}) {
			unlike($output_file, $tests{$test}->{regexp}, "$run: does not dump $test");
		}
	}
}

#########################################
# Stop the database instance, which will be removed at the end of the tests.

$node->stop('fast');
