use strict;
use warnings;

use MollyDBNode;
use TestLib;
use Test::More tests => 11;

program_help_ok('droplang');
program_version_ok('droplang');
program_options_handling_ok('droplang');

my $node = get_new_node('main');
$node->init;
$node->start;

$node->issues_sql_like(
	[ 'droplang', 'plpgsql', 'mollydb' ],
	qr/statement: DROP EXTENSION "plpgsql"/,
	'SQL DROP EXTENSION run');

$node->command_fails(
	[ 'droplang', 'nonexistent', 'mollydb' ],
	'fails with nonexistent language');
