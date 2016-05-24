use strict;
use warnings;

use MollyDBNode;
use TestLib;
use Test::More tests => 20;

program_help_ok('reindexdb');
program_version_ok('reindexdb');
program_options_handling_ok('reindexdb');

my $node = get_new_node('main');
$node->init;
$node->start;

$ENV{PGOPTIONS} = '--client-min-messages=WARNING';

$node->issues_sql_like(
	[ 'reindexdb', 'mollydb' ],
	qr/statement: REINDEX DATABASE mollydb;/,
	'SQL REINDEX run');

$node->safe_psql('mollydb',
	'CREATE TABLE test1 (a int); CREATE INDEX test1x ON test1 (a);');
$node->issues_sql_like(
	[ 'reindexdb', '-t', 'test1', 'mollydb' ],
	qr/statement: REINDEX TABLE test1;/,
	'reindex specific table');
$node->issues_sql_like(
	[ 'reindexdb', '-i', 'test1x', 'mollydb' ],
	qr/statement: REINDEX INDEX test1x;/,
	'reindex specific index');
$node->issues_sql_like(
	[ 'reindexdb', '-S', 'pg_catalog', 'mollydb' ],
	qr/statement: REINDEX SCHEMA pg_catalog;/,
	'reindex specific schema');
$node->issues_sql_like(
	[ 'reindexdb', '-s', 'mollydb' ],
	qr/statement: REINDEX SYSTEM mollydb;/,
	'reindex system tables');
$node->issues_sql_like(
	[ 'reindexdb', '-v', '-t', 'test1', 'mollydb' ],
	qr/statement: REINDEX \(VERBOSE\) TABLE test1;/,
	'reindex with verbose output');
