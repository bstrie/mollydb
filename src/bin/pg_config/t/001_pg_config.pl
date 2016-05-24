use strict;
use warnings;
use TestLib;
use Test::More tests => 20;

program_help_ok('mdb_config');
program_version_ok('mdb_config');
program_options_handling_ok('mdb_config');
command_like([ 'mdb_config', '--bindir' ], qr/bin/, 'mdb_config single option')
  ;    # XXX might be wrong
command_like([ 'mdb_config', '--bindir', '--libdir' ],
	qr/bin.*\n.*lib/, 'mdb_config two options');
command_like([ 'mdb_config', '--libdir', '--bindir' ],
	qr/lib.*\n.*bin/, 'mdb_config two options different order');
command_like(['mdb_config'], qr/.*\n.*\n.*/,
	'mdb_config without options prints many lines');
