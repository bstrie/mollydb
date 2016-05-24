use strict;
use warnings;

use MollyDBNode;
use TestLib;
use Test::More tests => 10;

program_help_ok('mdb_isready');
program_version_ok('mdb_isready');
program_options_handling_ok('mdb_isready');

command_fails(['mdb_isready'], 'fails with no server running');

my $node = get_new_node('main');
$node->init;
$node->start;

$node->command_ok(['mdb_isready'], 'succeeds with server running');
