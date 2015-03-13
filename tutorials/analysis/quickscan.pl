#!/usr/bin/env perl

use strict;

my $min      = $ARGV[0];
my $max      = $ARGV[1];
my $step     = $ARGV[2];
my $pref     = $ARGV[3];
my $decfile  = $ARGV[4];
my $decay    = $ARGV[5];
my $nevt     = $ARGV[6];
my $parms    = $ARGV[7];
my $res      = $ARGV[8];
my $njobs    = 0;

if (!defined($nevt))  {$nevt  = 1000;}
if (!defined($parms)) {$parms = "mwin=0.6";}
if (!defined($res))   {$res   = "pbp0";}
if (defined($ARGV[9])) {$njobs = $ARGV[9];}

if (!defined($ARGV[5])) 
{
    print "USAGE:\n";
    print "quickscan.pl <min> <max> <step> <pref> <decfile> <decay> [nevt] [parms] [res]\n\n";
    print "   <min>     : min E_cm\n";
    print "   <max>     : max E_cm\n";
    print "   <step>    : dE\n";
    print "   <pref>    : prefix of file names\n";
    print "   <decfile> : decay file\n";
    print "   <decay>   : decay pattern\n";
    print "   [nevt]    : events per scan point; default: 1000\n";
    print "   [parms]   : task parameters; default: 'mwin=0.6'\n";
    print "   [res]     : resonance; default: 'pbp0'\n";
    print "   [njobs]   : if > 0, njobs jobs will be submitted per energy; default: 0\n\n";
   exit(0);
}

my $macro = "quickfsimana.C";
my $cnt = 0;
$min  *= 100;
$max  *= 100;
$step *= 100;

my $wd = $ENV{'PWD'};

for (my $ecm = $min; $ecm<=$max; $ecm+=$step)
{
	my $ecmstr = int($ecm+0.0001);
	my $command = "";
	if ($njobs==0)
	{
		$command = "root -l -b -q '$macro(\"$pref$ecmstr\",\"$decfile\",".(-$ecm/100.).",\"$decay\",$nevt,\"$res\",\"$parms\", true, $cnt )'";
	}
	else
	{
		$decay =~ s/ /§/g;
		$parms =~ s/ //g;
		$command = "qsub -t 1-$njobs job_scan.sge '$wd/$macro(\"$pref$ecmstr\",\"$wd/$decfile\",".(-$ecm/100.).",\"$decay\",$nevt,\"$res\",\"$parms\",true,$cnt)'";
	}
	print $command."\n";
	system($command);
	$cnt++;
}
