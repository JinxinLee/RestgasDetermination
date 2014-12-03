#!/usr/bin/env perl

use strict;


my $pref     = $ARGV[0];
my $test     = $ARGV[1];

if (!defined $ARGV[0]) 
{
    print "USAGE:\n";
    print "compress.pl [pref]\n\n";
    print "   [pref]    : prefix of file names\n\n";
    print "   [test]    : put 'test' as 2nd parameter to just print commands\n\n";
   exit(0);
}

if ( !defined($test) || ($test ne "test") ) {$test="";}

my $macro = "../../compress.C+";

# remove leading and trailing whitespace
sub  trim { my $s = shift; $s =~ s/^\s+|\s+$//g; return $s };

# --------------
# main routine
# --------------

my @files = `ls $pref*`;

my %mdifcuts = (
  110 => 3.9, 111 => 3.9, 112 => 3.9, 113 => 3.9, 114 => 3.9,
  115 => 4.3, 116 => 4.3, 117 => 4.3, 118 => 4.3, 119 => 4.3,
  130 => 1.2, 131 => 1.2, 132 => 1.2, 133 => 1.2, 134 => 1.2,
  135 => 3.8, 136 => 3.8, 137 => 3.8, 138 => 3.8,
  150 => 4.3, 151 => 4.3, 
);

foreach my $infile (@files)
{
	chomp $infile;
	$infile =~ m/M(\d+)_n(\d+)_(\d+)S_(\d+)B.+root/;
	my $mode = $2;
	my $ntp = "n$2";
	
	my $outfile = $infile;
	
	$outfile =~ s/\.root/\_shr\.root/; 
		
	my $precut = "tag";
	
	if (defined $mdifcuts{$mode}) 
	{
		my $mean = 0.1422, my $win = 4.*$mdifcuts{$mode}/1000.;
		if ($mode>=150) {$mean = 0.1439;}
		elsif ($mode>=135) {$mean = 0.1408;}
		elsif ($mode>=130) {$mean = 0.1455;}

		$precut .="&&abs(xmdif-$mean)<$win";
	}	
	
	my $command = "root -l -b -q '$macro(\"$ntp\",\"$infile\",\"$outfile\",\"\",\"$precut\")'";
	print $command."\n";
	if ($test ne "test") {`$command`;}
}
