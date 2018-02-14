#!/usr/bin/env perl

use strict;

my $min      = $ARGV[0];
my $max      = $ARGV[1];
my $step     = $ARGV[2];
my $pref     = $ARGV[3];
my $decfile  = $ARGV[4];
my $nevt     = $ARGV[5];
my $res      = $ARGV[6];
my $mode     = $ARGV[7];
my $njobs    = 0;

if (!defined($nevt))   {$nevt  = 1000;}
if (!defined($res))    {$res   = "pbp0";}
if (!defined($mode))   {$mode  = 900;}
if (defined($ARGV[8])) {$njobs = $ARGV[8];}

if (!defined($ARGV[4])) 
{
    print "USAGE:\n";
    print "quickSTscan.pl <min> <max> <step> <pref> <decfile> <decay> [nevt] [parms] [res]\n\n";
    print "   <min>     : min E_cm\n";
    print "   <max>     : max E_cm\n";
    print "   <step>    : dE\n";
    print "   <pref>    : prefix of file names\n";
    print "   <decfile> : decay file or 'DPM' or 'FTF'\n";
    print "   [nevt]    : events per scan point; default: 1000\n";
    print "   [res]     : resonance; default: 'pbp'\n";
    print "   [mode]    : data mode; default: [3 digits Ecm]900\n";
    print "   [njobs]   : if > 0, njobs jobs will be submitted per energy; default: 0\n\n";
   exit(0);
}

my $macro = "prod_fsim.C";

my $cnt = 0;
$min  *= 100;
$max  *= 100;
$step *= 100;

for (my $ecm = $min; $ecm<=$max; $ecm+=$step)
{
	my $ecmstr = int($ecm+0.0001);
	my $command = "";
	if ($njobs==0) 
	{
		$command = "root -l -b -q '$macro(\"$pref$ecmstr\", \"$decfile\", ".(-$ecm/100.).", $nevt, \"$res\", $ecmstr$mode, $cnt, 1)'";
	}
	else
	{
		$command = "qsub -t 1-$njobs job_sof_fsim.sge $ecmstr$mode $nevt $decfile ".(-$ecm/100.)." $res $pref 1"; 
	}
	print $command."\n";
	#system($command);
	$cnt++;
}
