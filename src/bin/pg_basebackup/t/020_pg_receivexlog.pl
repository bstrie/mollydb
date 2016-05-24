use strict;
use warnings;
use TestLib;
use Test::More tests => 8;

program_help_ok('mdb_receivexlog');
program_version_ok('mdb_receivexlog');
program_options_handling_ok('mdb_receivexlog');
