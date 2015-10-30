#!/usr/bin/env perl

my $cmd   = $ARGV[0];
my $check = defined($ARGV[1]);

# DPM:    qsub -t 1-20 job_prod.sge DPM10GeV 1000 DPM 10.0 summy saveall
# EvtGen: qsub -t 1-20 job_prod.sge D0Kpi 1000 D0toKpi.dec 10.0 pbarpSystem saveall

if (!defined($cmd))
{
	print "\nChecks the jobs output (*_pid.root existing and reasonable in size) and resubmitts all failed.\n\n";
    print "USAGE:\n";
    print 'resub.pl "<cmd>" [check]'."\n";
    print "  <cmd>   : the complete qsub command line in quotes; the 'qsub -t' at the beginning can optionally be skipped\n";
	print "  [check] : optional parameter 'check', which just prints out what would be resubmitted\n\n";
    exit(0);
}


$cmd =~ m/(\d+)-(\d+)\s+([\.\w]+)\s+(\w+)\s+(.*)/;

defined($4) or die("qsub string seems wrongly formatted: \"$cmd\"\n");

print "Checking for files \"data/$4_<run>_pid.root\" for runs $1 - $2 (cmd opt:\"$1-$2 $3 $4 $5\")\n"; 

my $min     = $1;
my $max     = $2;
my $pref    = $4;

my @broken;

for (my $i=$min; $i<=$max; $i++)
{
	my $fname = "data/M".$pref."_".$i."_pid.root";
	if (!-e $fname) 
	{
		print $fname.": doesn't exist\n";
		push(@broken,$i);
	}
	else
	{
		my $filesize = -s $fname;
		if ($filesize<10000)
		{
			print $fname.": small filesize\n";
			push(@broken,$i);
		}
	}
} 

if ($check) {print "Would ";}
print "Re-submit : \n";

foreach my $nums (@broken)
{
	my $recmd = "qsub -t $nums\-$nums $3 $4 $5";
	print "$recmd\n";
	if (!$check) {system($recmd);}
}

print "\n";
