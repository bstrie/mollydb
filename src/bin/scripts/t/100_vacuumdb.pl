use strict;
use warnings;

use PostgresNode;
use TestLib;
use Test::More tests => 18;

program_help_ok('vacuumdb');
program_version_ok('vacuumdb');
program_options_handling_ok('vacuumdb');

my $node = get_new_node('main');
$node->init;
$node->start;

$node->issues_sql_like(
	[ 'vacuumdb', 'mollydb' ],
	qr/statement: VACUUM;/,
	'SQL VACUUM run');
$node->issues_sql_like(
	[ 'vacuumdb', '-f', 'mollydb' ],
	qr/statement: VACUUM \(FULL\);/,
	'vacuumdb -f');
$node->issues_sql_like(
	[ 'vacuumdb', '-F', 'mollydb' ],
	qr/statement: VACUUM \(FREEZE\);/,
	'vacuumdb -F');
$node->issues_sql_like(
	[ 'vacuumdb', '-z', 'mollydb' ],
	qr/statement: VACUUM \(ANALYZE\);/,
	'vacuumdb -z');
$node->issues_sql_like(
	[ 'vacuumdb', '-Z', 'mollydb' ],
	qr/statement: ANALYZE;/,
	'vacuumdb -Z');
