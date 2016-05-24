use strict;
use warnings;

use MollyDBNode;
use TestLib;
use Test::More tests => 14;

program_help_ok('createlang');
program_version_ok('createlang');
program_options_handling_ok('createlang');

my $node = get_new_node('main');
$node->init;
$node->start;

$node->command_fails([ 'createlang', 'plmdb' ],
	'fails if language already exists');

$node->safe_psql('mollydb', 'DROP EXTENSION plmdb');
$node->issues_sql_like(
	[ 'createlang', 'plmdb' ],
	qr/statement: CREATE EXTENSION "plmdb"/,
	'SQL CREATE EXTENSION run');

$node->command_like([ 'createlang', '--list' ], qr/plmdb/, 'list output');
