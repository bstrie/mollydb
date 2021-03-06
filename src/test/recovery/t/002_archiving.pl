# test for archiving with hot standby
use strict;
use warnings;
use MollyDBNode;
use TestLib;
use Test::More tests => 1;
use File::Copy;

# Initialize master node, doing archives
my $node_master = get_new_node('master');
$node_master->init(
	has_archiving    => 1,
	allows_streaming => 1);
my $backup_name = 'my_backup';

# Start it
$node_master->start;

# Take backup for slave
$node_master->backup($backup_name);

# Initialize standby node from backup, fetching WAL from archives
my $node_standby = get_new_node('standby');
$node_standby->init_from_backup($node_master, $backup_name,
	has_restoring => 1);
$node_standby->append_conf(
	'mollydb.conf', qq(
wal_retrieve_retry_interval = '100ms'
));
$node_standby->start;

# Create some content on master
$node_master->safe_psql('mollydb',
	"CREATE TABLE tab_int AS SELECT generate_series(1,1000) AS a");
my $current_lsn =
  $node_master->safe_psql('mollydb', "SELECT mdb_current_xlog_location();");

# Force archiving of WAL file to make it present on master
$node_master->safe_psql('mollydb', "SELECT mdb_switch_xlog()");

# Add some more content, it should not be present on standby
$node_master->safe_psql('mollydb',
	"INSERT INTO tab_int VALUES (generate_series(1001,2000))");

# Wait until necessary replay has been done on standby
my $caughtup_query =
  "SELECT '$current_lsn'::mdb_lsn <= mdb_last_xlog_replay_location()";
$node_standby->poll_query_until('mollydb', $caughtup_query)
  or die "Timed out while waiting for standby to catch up";

my $result = $node_standby->safe_psql('mollydb', "SELECT count(*) FROM tab_int");
is($result, qq(1000), 'check content from archives');
