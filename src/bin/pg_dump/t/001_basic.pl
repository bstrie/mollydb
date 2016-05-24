use strict;
use warnings;

use Config;
use MollyDBNode;
use TestLib;
use Test::More tests => 15;

my $tempdir       = TestLib::tempdir;
my $tempdir_short = TestLib::tempdir_short;

#########################################
# Basic checks

program_help_ok('mdb_dump');
program_version_ok('mdb_dump');
program_options_handling_ok('mdb_dump');

#########################################
# Test various invalid options and disallowed combinations
# Doesn't require a PG instance to be set up, so do this first.

command_exit_is([ 'mdb_dump', 'qqq', 'abc' ],
	1, 'mdb_dump: too many command-line arguments (first is "asd")');

command_exit_is([ 'mdb_dump', '-s', '-a' ],
	1, 'mdb_dump: options -s/--schema-only and -a/--data-only cannot be used together');

command_exit_is([ 'mdb_dump', '-c', '-a' ],
	1, 'mdb_dump: options -c/--clean and -a/--data-only cannot be used together');

command_exit_is([ 'mdb_dump', '--inserts', '-o' ],
	1, 'mdb_dump: options --inserts/--column-inserts and -o/--oids cannot be used together');

command_exit_is([ 'mdb_dump', '--if-exists' ],
	1, 'mdb_dump: option --if-exists requires option -c/--clean');

command_exit_is([ 'mdb_dump', '-j' ],
	1, 'mdb_dump: option requires an argument -- \'j\'');

command_exit_is([ 'mdb_dump', '-j3' ],
	1, 'mdb_dump: parallel backup only supported by the directory format');
