#!/usr/bin/env perl

my $pref  = $ARGV[0];
my $mode  = $ARGV[1];
my $num   = $ARGV[2];
my $min   = $ARGV[3];
my $max   = $ARGV[4];

my $check = 0;

if (!defined($pref))
{
    print "\nSubmits analysis jobs over multiple files on PROMETHEUS cluster at GSI.\n\n";
    print "USAGE:\n";
    print "anasub_pro.pl <pref> [mode] [num] [min] [max]\n";
    print "  <pref>  : prefix of files with names data/pref_<run>_pid.root; if 'check_' added in front, commands are only printed\n";
    print "  [mode]  : mode number to be written in TTree; default:0\n";
    print "  [num]   : number of files to be analysed per analysis job; default:50\n";
    print "  [min]   : first run; if not given, all files found are analysed)\n";
    print "  [max]   : last run;  if not given, all files starting from [min] are analysed)\n\n";
    print "Example 1 : anasub_pro.pl check_mysim 1 10  --> only prints out the jobs to be submitted\n";
    print "Example 2 : anasub_pro.pl mysim 1 10        --> submits the jobs\n\n";
    
    exit(0);
}

if ($pref=~s/^check_//) { $check = 1;}

if (!defined($num))  {$num=50;}
if (!defined($mode)) {$mode=0;}

my $minfound=100000;
my $maxfound=0;

if (!defined($min) || !defined($max))
{
	print "Searching for files with name data/$pref\_<run>_pid.root...\n";
	my @dir = `ls data/$pref\_*_pid.root`;

	foreach my $fname (@dir)
	{
		if ($fname =~ m/$pref\_(\d+)_pid.root/)
		{
			if ($minfound>$1) {$minfound=$1;}
			if ($maxfound<$1) {$maxfound=$1;}
		}
	}
}

if (!defined($min)) {$min=$minfound;}
if (!defined($max)) {$max=$maxfound;}

my $curr = $min;

while ($curr<$max)
{
	my $up = $curr+$num-1;
	if ($up>$max) {$up=$max;}
	my $cmd = "qsub job_ana.sge $pref $curr $up $mode";
	print "$cmd\n";
	$curr+=$num;
	if (!$check) {system($cmd);}
}

