#!/usr/bin/env perl

use strict;

my $modepref = $ARGV[0];
my $nevt     = $ARGV[1];
my $jobs     = $ARGV[2];
my $sqs      = $ARGV[3];
my $option   = "";

if (!defined($nevt)) {$nevt = 25000;}
if (!defined($jobs)) {$jobs = 1;}
if (!defined($sqs))  {$sqs  = 0;}

if (!defined $ARGV[0]) 
{
    print "USAGE:\n";
    print "myqafsimscan.pl <modepref> [n_evt] [min] [max] [sqs]\n\n";
    print "   <modepref> : modes with prefix 'modepref'; all submits all scans; testxxx prints out command w/o submission\n\n"; 
	print "   [n_evt]    : number of events per job (default = 10000) \n";
	print "   [jobs]     : number of jobs (default = 1) \n";
	print "   [sqs]      : sqrt(s); if given, only this energy is submitted\n\n";
    exit(0);
}

# this subroutine works as the script 'mystscan.pl' and submits the scan job arrays to the queue
sub mystscan
{
#   print "./mystscan.pl $min $max $mode"."00"."$rec \"$decay\" $nevt \"$parm\" pbp0 $jobs\n";
	
    # parameters for quickfsimana.C
	my $min      = $_[0];  # min energy
	my $max      = $_[1];  # max energy
	my $mode     = $_[2];  # mode number
	my $decay    = $_[3];  # decay pattern
	my $nevt     = $_[4];  # number of events
	my $parms    = $_[5];  # parameters for the analysis macro
	my $res      = $_[6];  # resonance
	my $njobs    = $_[7];  # number of jobs 
	
    # parameters for compress
    my $ntpname  = $_[8];  # name of ntuple to shrink
	my $vars     = $_[9];  # vars to keep in ntuple
    my $precut   = $_[10]; # precut to apply for shrinking
	my $option   = $_[11]; # option: '' or 'check' (check which output files are missing) or 'resub' (resubmitts failed jobs)

	if (!defined($option)) {$option="";}

	my $macro  = "quickfsimana.C";
	my $macro2 = "compress.C";
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
			my $commandpref = "";  # contains the qsub part
			my $command     = "";  # the command part starting from job script
			my $dosubmit = 1;      # defaul: submit jobs; can be changed by options 'test', 'check', 'resub'
			
			$decay =~ s/ /§/g;
			$vars  =~ s/ /§/g;
			$parms =~ s/ //g;

			if ($option eq "test") {$dosubmit = 0;}

			if ($njobs<0) # just submit 1 jobs with number njobs
			{
				my $jobnum = -$njobs;
				$commandpref = "qsub -t $jobnum-$jobnum";
			}
			else # submit jobs 1-njobs
			{
				$commandpref = "qsub -t 1-$njobs";
			}
			$command  = " job_scan.sge '$wd/$macro(\"MI$mode$ecmstr\",\"$wd/decfiles/M$mode.dec\",".(-$ecm/100.).",\"$decay\",$nevt,\"$res\",\"$parms\",true,SGE_TASK_ID,$mode)'"; # the analyis
			$command .= " '$wd/$macro2(\"$ntpname\",\"MI$mode$ecmstr"."_SGE_TASK_ID_ana.root\",\"MI$mode$ecmstr"."_SGE_TASK_ID_shr_$ntpname.root\",\"$vars\",\"$precut\")'";                # shrink tuple
			
			my $outfilepatt = "data/MI$mode$ecmstr"."_%d_shr_$ntpname.root";

			# check whether output file from previous submit is existing
			if ($option eq "check" || $option eq "resub")
			{
				for (my $ii=1;$ii<=$njobs;++$ii)
				{
					my $fname = sprintf($outfilepatt,$ii);
					# if not, print a message
					unless (-e $fname ) 
					{	
						print "Missing file: ".$fname."\n";
						if ($option eq "resub")
						{
							$commandpref = "qsub -t $ii-$ii";
							print "re-submitting ".$commandpref.$command."\n";
							system($commandpref.$command);
						}
					}
				} 
			}
			else
			{
				if ($dosubmit==1) {system($commandpref.$command);}
				print $commandpref.$command."\n";
			}
		}
	}
	
}

# test mode only?
if ($modepref =~ m/^test/)  
{
	$modepref =~ s/test//g;
	$option   = "test";
}
# check which outputs are missing?
elsif ($modepref =~ m/^check/)
{
	$modepref =~ s/check//g;
	$option   = "check";
}
# resubmit missing jobs
elsif ($modepref =~ m/^resub/)
{
	$modepref =~ s/resub//g;
	$option   = "resub";
}

my $precut    = "xmct";
my $vars3body = "xdal01 xdal02 xdal12 beamm trig xm mode";

# all?
if ($modepref eq "all") {$modepref="";}

# standard triggerlines file
my $tlfile = $ENV{"VMCWORKDIR"}."/softrig/triggerlines_fsim.cfg";

# read trigger lines to extract decay modes
open(FILE, "<", $tlfile) or die "Failed to open file: $!\n";
chomp(my @triglines = <FILE>);
close(FILE);

# recoil masses needed to determine thresholds
#                     none   gam    pi0    eta   2pi0   2pi    2K     2K0    2eta   2pipi0
my @recoilmass = qw ( 0.000  0.000  0.135  0.547 0.270  0.280  0.988  1.000  1.100  0.420  );
my @scanpoint  = qw ( 2.4  3.0  3.5  3.8  4.5  5.0  5.5); 

#
# three body decays
#

foreach my $line (@triglines)
{
	# skip emtpy and comment lines
	next if ($line =~ m/^#/ || $line=~m/\[ \]/g || $line eq "" );
	
	#chop to pieces
	my @fields = split(/:/, $line);

	$fields[2] =~ s/cc//g;          # remove 'cc' from decay pattern
	for (my $i=0;$i<9;++$i) {$fields[$i] =~ s/^\s+|\s+$//g;} # remove leading/trailing whitespace

	my @decprods = split /\s+/, $fields[2];
	next if (scalar(@decprods) != 5); # only select 3-body modes

	#print "".$fields[0]." ".$fields[2]." ".$fields[9]."\n";
	my $mode   = $fields[0];
	my $decay  = $fields[2];
	my $thresh = $fields[9];
	my $parm   = "mwin=0.3:pid=Loose";
	my $intp   = 0;

	if ($decay =~ m/pi0/)  {$decay = "pi0->gamma gamma;".$decay; $parm .= ":mwin(pi0)=0.04:emin=0.1"; ++$intp;}
	if ($decay =~ m/K_S0/) {$decay = "K_S0->pi+ pi-;".$decay;    $parm .= ":mwin(K_S0)=0.06";++$intp;}
	#print "$mode, ntp$intp : \"$decay\", \"$parm\"\n";
	
	# take care about different recoils
	for (my $rec=0; $rec<10; ++$rec)
	{
		next if ( (($mode>=200 && $mode<299) || $mode==500) && $rec==0 ); # skip 'no recoil' for charmonia and phi (because sqrt has to be on resonance)
		next if ( $mode>=600 && $rec>0);                                  # skip all recoils != '' for modes 6xx (exclusive modes!)

		my $fullcode = $mode."00".$rec;           # full mode code
		my $minsqs = $thresh+$recoilmass[$rec];   # sqs thresholds
		my $minscanidx = 0;                       # the index for the scan points array

		next if ($minsqs>5.5); # skip if required energy is beyond PANDA limit

		while ($scanpoint[$minscanidx]<$minsqs) {++$minscanidx;}

		next if ($sqs>0 && $scanpoint[$minscanidx]>$sqs); # skip if user energy is below threshold 

		my $min = $scanpoint[$minscanidx];
		my $max = 5.5;

		if ($sqs>0) {$min=$sqs; $max = $sqs;} # if user wants one energy, just submit that one

		if ($fullcode =~ m/^$modepref/) 
		{
			my $command = "./mystscan.pl $min $max $mode"."00"."$rec \"$decay\" $nevt \"$parm\" pbp0 $jobs";
# 			if ($test==0)
# 			{
			#	system($command);
			mystscan($min, $max, $mode."00".$rec, $decay, $nevt, $parm, "pbp0", $jobs, "ntp".$intp, $vars3body, $precut, $option);
# 			}
# 			else {
# 				print "$command\n";
# 			}
		}
		
		if ($mode>=100 && $mode<=199)  # care about D D* variations (modes xxx1xx)
		{
			$fullcode = $mode."10".$rec;
			while ($scanpoint[$minscanidx]<($minsqs+0.145)) {++$minscanidx;}
			next if ($sqs>0 && $scanpoint[$minscanidx]>$sqs); # skip if user energy is below threshold 
			if ($sqs==0) {$min = $scanpoint[$minscanidx];}
			if ($fullcode =~ m/^$modepref/) 
			{
# 				my $command = "./mystscan.pl $min $max $mode"."10"."$rec \"$decay\" $nevt \"$parm\" pbp0 $jobs";
# 				if ($test==0)
# 				{
				#	system($command);
				mystscan($min, $max, $mode."10".$rec, $decay, $nevt, $parm, "pbp0", $jobs, "ntp".$intp, $vars3body, $precut, $option);
# 				}
# 				else {
# 					print "$command\n";
# 				}
			}
		}
	}
}
