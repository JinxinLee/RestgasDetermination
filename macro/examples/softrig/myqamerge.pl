#!/usr/bin/env perl

use strict;
use File::Basename;

my $opt      = $ARGV[0];
my $mpref    = $ARGV[1];

if ( !defined($opt) || ($opt ne "do" && $opt ne "test") ) 
{
    print "USAGE:\n";
    print "myqamerge.pl <do/test> [m_pref]\n\n";
    print "   <do/test>   : 'do' does merge; 'test' just prints merge commands\n";
    print "   [m_pref]    : prefix for mode (e.g. 221 only merges mode 221)\n\n"; 
    exit(0);
}


# --------------
# main routine
# --------------

my @dir= `ls data/MI*_shr_*.root`;
my %mode;
my %ntp;

# replace 'all' by '' (for interpolation in $command)

# get directory entries ending with *root
foreach (@dir)
{
	chomp;
	$_ = basename($_);
	if (m/^MI(\d{3}).+(ntp\d)\.root$/)
	{
		if (!defined($mode{$1})) {$mode{$1}=1;} else {$mode{$1} += 1;} # count modes
		$ntp{$1} = $2;
	}
}

# loop over energies
foreach my $m (sort keys %mode)
{
	# only for preferred one if specified
	if (defined($mpref) && ($m !~ m/^$mpref/)) {next;}
	
	print "Mode $m: Found $mode{$m} files ($ntp{$m}) to be merged. -> ";
	my $command = "root -l -b -q 'merge.C(\"$ntp{$m}\",\"data/MI$m"."_$ntp{$m}.root\",\"data/MI$m*_shr_$ntp{$m}.root\")'";
	print "$command\n";
	if ($opt ne "test") {system($command);}
}