use strict;
use warnings;
use MollyDBNode;
use TestLib;
use Test::More tests => 13;

program_help_ok('mdb_controldata');
program_version_ok('mdb_controldata');
program_options_handling_ok('mdb_controldata');
command_fails(['mdb_controldata'], 'mdb_controldata without arguments fails');
command_fails([ 'mdb_controldata', 'nonexistent' ],
	'mdb_controldata with nonexistent directory fails');

my $node = get_new_node('main');
$node->init;

command_like([ 'mdb_controldata', $node->data_dir ],
	qr/checkpoint/, 'mdb_controldata produces output');
