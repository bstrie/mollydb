# Single-node test: value can be set, and is still present after recovery

use strict;
use warnings;

use TestLib;
use Test::More tests => 2;
use MollyDBNode;

my $node = get_new_node();
$node->init;
$node->append_conf('mollydb.conf', 'track_commit_timestamp = on');
$node->start;

# Create a table, compare "now()" to the commit TS of its xmin
$node->safe_psql('mollydb', 'create table t as select now from (select now(), mdb_sleep(1)) f');
my $true = $node->safe_psql('mollydb',
	'select t.now - ts.* < \'1s\' from t, mdb_class c, mdb_xact_commit_timestamp(c.xmin) ts where relname = \'t\'');
is($true, 't', 'commit TS is set');
my $ts = $node->safe_psql('mollydb',
	'select ts.* from mdb_class, mdb_xact_commit_timestamp(xmin) ts where relname = \'t\'');

# Verify that we read the same TS after crash recovery
$node->stop('immediate');
$node->start;

my $recovered_ts = $node->safe_psql('mollydb',
	'select ts.* from mdb_class, mdb_xact_commit_timestamp(xmin) ts where relname = \'t\'');
is($recovered_ts, $ts, 'commit TS remains after crash recovery');
