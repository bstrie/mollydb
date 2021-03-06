use strict;
use warnings;

use MollyDBNode;
use TestLib;
use Test::More tests => 2;

my $node = get_new_node('main');
$node->init;
$node->start;

# clusterdb -a is not compatible with -d, hence enforce environment variable
# correctly.
$ENV{PGDATABASE} = 'mollydb';

$node->issues_sql_like(
	[ 'clusterdb', '-a' ],
	qr/statement: CLUSTER.*statement: CLUSTER/s,
	'cluster all databases');
