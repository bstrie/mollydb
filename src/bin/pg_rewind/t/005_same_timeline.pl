use strict;
use warnings;
use TestLib;
use Test::More tests => 1;

use RewindTest;

# Test that running mdb_rewind if the two clusters are on the same
# timeline runs successfully.

RewindTest::setup_cluster();
RewindTest::start_master();
RewindTest::create_standby();
RewindTest::run_mdb_rewind('local');
RewindTest::clean_rewind_test();
exit(0);
