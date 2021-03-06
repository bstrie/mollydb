use strict;
use warnings;

use MollyDBNode;
use TestLib;
use Test::More tests => 3;

my $tempdir       = TestLib::tempdir;
my $tempdir_short = TestLib::tempdir_short;

command_exit_is([ 'mdb_ctl', 'status', '-D', "$tempdir/nonexistent" ],
	4, 'mdb_ctl status with nonexistent directory');

my $node = get_new_node('main');
$node->init;

command_exit_is([ 'mdb_ctl', 'status', '-D', $node->data_dir ],
	3, 'mdb_ctl status with server not running');

system_or_bail 'mdb_ctl', '-l', "$tempdir/logfile", '-D',
  $node->data_dir, '-w', 'start';
command_exit_is([ 'mdb_ctl', 'status', '-D', $node->data_dir ],
	0, 'mdb_ctl status with server running');

system_or_bail 'mdb_ctl', 'stop', '-D', $node->data_dir, '-m', 'fast';
