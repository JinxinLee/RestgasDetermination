#!/usr/bin/env perl

use strict;


my $opt      = $ARGV[0];
my $epref    = $ARGV[1];
my $mpref    = $ARGV[2];

if ( !defined($opt) || ($opt ne "all" && $opt ne "mct" && $opt ne "test") ) 
{
    print "USAGE:\n";
    print "merge.pl <all/mct/test> [E_pref] [m_pref]\n\n";
    print "   <all/mct/test> : 'all' signal or only those with 'mct' match; 'test' just prints merge commands\n";
	print "   [E_pref]       : prefix for energy (e.g. 550 = 5.5GeV)\n";
    print "   [m_pref]       : prefix for mode (e.g. 221 only merges mode 221)\n\n"; 
	print "Mode-wise merge of signal with dpm root files from softtrigger task output. Expects root macros in ../\n\n";
    exit(0);
}


# --------------
# main routine
# --------------

my @dir= `ls *root`;
my %nrg;
my %mode;
my %code;
my $mct;
my $test;

# replace 'all' by '' (for interpolation in $command)
if ($opt eq "mct") {$mct = "mct";}
if ($opt eq "test") {$test = "test";}

# get directory entries ending with *root
foreach (@dir)
{
	chomp;
	if (m/^M((\d{3})(\d{3})).+root$/)
	{
		if (!defined($code{$1})) {$code{$1}=1;} else {$code{$1} += 1;} # count combinations energy+mode
		if (!defined($mode{$3})) {$mode{$3}=1;} else {$mode{$3} += 1;} # count modes
		if (!defined($nrg{$2})) {$nrg{$2}=1;} else {$nrg{$2} += 1;}    # count energies
	}
}

# loop over energies
foreach my $e (sort keys %nrg)
{
	# only for preferred one if specified
	if (defined($epref) && $e ne $epref) {next;}
	
	print "nrg = $e (".$nrg{$e}." files)\n";
	
	# loop over modes
	foreach my $m (sort keys %mode)
	{
		# ignore DPM files for mode merge (will be added anyways)
		if ($m eq "900") {next;}
		
		# only preferred mode if specified
		if (defined($mpref) && $m !~ m/^$mpref/) {next;}
		
		# number of signal files
		my $nsig = 0;
		if (defined($code{$e.$m})) {$nsig =  $code{$e.$m};}
		
		# number of DPM files
		my $nbkg = 0;
		if (defined($code{$e."900"})) {$nbkg =  $code{$e."900"};}
		
		# if at least one signal file exists for this energy do merge
		if ($nsig>0) 
		{
			my $command = sprintf "root -l -b -q '../merge%s.C(\"n%s\",\"M%s_n%s_%sS_%sB_mct.root\",\"M%s%s*.root\",\"M%s900*.root\")'\n",
					$mct, $m, $e, $m, $nsig, $nbkg, $e, $m, $e;
					
			print  " mode $m ($nsig sig, $nbkg bg) : $command";
			if ($test ne "test") {`$command`;}
		}
	}
	print "\n";
}	


