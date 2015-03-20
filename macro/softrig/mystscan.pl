#!/usr/bin/env perl

use strict;

my $min      = $ARGV[0];
my $max      = $ARGV[1];
my $mode     = $ARGV[2];
my $decay    = $ARGV[3];
my $nevt     = $ARGV[4];
my $parms    = $ARGV[5];
my $res      = $ARGV[6];
my $njobs    = 0;

if (!defined($nevt))  {$nevt  = 1000;}
if (!defined($parms)) {$parms = "mwin=0.6";}
if (!defined($res))   {$res   = "pbp0";}
if (!defined($mode))  {$mode  = 0;}
if (defined($ARGV[7])) {$njobs = $ARGV[7];}

if (!defined($ARGV[3])) 
{
    print "USAGE:\n";
    print "mystscan.pl <min> <pref> <decfile> <decay> [nevt] [parms] [res]\n\n";
    print "   <min>     : min E_cm\n";
    print "   <max>     : max E_cm\n";
    print "   <mode>    : mode; used as prefix and for decfile\n";
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

my $wd = $ENV{'PWD'};

my @scanpoints = (2.4, 3.0, 3.5, 3.8, 4.5, 5.0, 5.5); 

foreach my $ecm (@scanpoints) 
{
	$ecm *= 100;
	if ($ecm >= $min && $ecm<=$max)
	{
		my $ecmstr = int($ecm+0.0001);
		my $command = "";
		if ($njobs==0)
		{
			$command = "root -l -b -q '$macro(\"MI$mode$ecmstr\",\"stdecfiles/M$mode.dec\",".(-$ecm/100.).",\"$decay\",$nevt,\"$res\",\"$parms\", true, $cnt, $mode )'";
		}
		else
		{
			$decay =~ s/ /§/g;
			$parms =~ s/ //g;
			if ($njobs<0) # just submit 1 jobs with number njobs
			{
				my $jobnum = -$njobs;
				$command = "qsub -t $jobnum-$jobnum job_scan.sge '$wd/$macro(\"MI$mode$ecmstr\",\"$wd/stdecfiles/M$mode.dec\",".(-$ecm/100.).",\"$decay\",$nevt,\"$res\",\"$parms\",true,SGE_TASK_ID,$mode)'";
			}
			else # submit jobs 1-njobs
			{
				$command = "qsub -t 1-$njobs job_scan.sge '$wd/$macro(\"MI$mode$ecmstr\",\"$wd/stdecfiles/M$mode.dec\",".(-$ecm/100.).",\"$decay\",$nevt,\"$res\",\"$parms\",true,SGE_TASK_ID,$mode)'";
			}
		}
		print $command."\n";
		system($command);
		$cnt++;
	}
}
