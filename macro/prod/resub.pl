#!/usr/bin/perl

my $para  = $ARGV[0];
my $check = defined($ARGV[1]);

# Example commands
# DPM:    sbatch -a1-20 jobsim_kronos.sh DPM10GeV 1000 DPM         10.0 saveall
# EvtGen: sbatch -a1-20 jobsim_kronos.sh D0Kpi    1000 D0toKpi.dec 10.0 saveall

# print some usage information
if (!defined($para))
{
    print "\nChecks the jobs output (*_pid.root existing and reasonable in size) and resubmits all failed ones (KRONOS version).\n\n";
    print "USAGE:\n";
    print 'resub.pl "<cmd>" [check]'."\n";
    print "  <cmd>   : The complete sbatch command line in quotes (job script name must be 'job...sh'), the -a parameter the first.\n";
    print "            The 'sbatch -a' at the beginning can optionally be skipped.\n";
    print "            If given a file name 'xxx.jobs' (w/o quotes) instead, the above is done for every command listed in the file.\n";
    print "  [check] : Optional parameter 'check', which just prints out what would be resubmitted\n\n";
    exit(0);
}

my @commands;

# we have a file containing a list of sbatch commands
if ( $para =~ m/\.jobs$/ )
{
    open (in,"<$para");
    @commands= <in>;
    close in;
}
# we only have one sbatch command directly given as parameter
else
{
    push(@commands, $para);
}

# for each entry in the commands array
foreach my $cmd (@commands)
{
    # cut away the CR and NL
    chomp $cmd;
    # if commented line (first char = '#'), skip
    if ( $cmd =~ m/^#/) {next;}
    
    print "\n".$cmd."\n";
    $cmd =~ m/(\d+)-(\d+)(.+)(job.*\.sh)\s+(\w+)\s+(.*)/;

    print "Checking for files \"data/$5_<run>_pid.root\" for runs $1 - $2 (cmd opt: \"$1-$2 $3 jobsim_kronos.sh $5 $6\")\n\n"; 
    
    my $min     = $1;
    my $max     = $2;
    my $pref    = $5;
    
    my @broken=(), @nexist=(), @small=();

    # find run numbers of non-existing and too small file
    for (my $i=$min; $i<=$max; $i++)
    {
	my $fname = "data/".$pref."_".$i."_pid.root";
	if (!-e $fname) 
	{
	    push(@broken,$i);
	    push(@nexist, $i);
	}
	else
	{
	    my $filesize = -s $fname;
	    if ($filesize<10000)
	    {
		push(@broken,$i);
		push(@small,$i);
	    }
	}
    } 

    # print out numbers of failed jobs
    print "Not existing : ";
    foreach my $run (@nexist) {print "$run ";}
    print "\nSmall file   : ";
    foreach my $run (@small) {print "$run ";}
    print "\n\n";
    
    if ($check) {print "Would ";}
    print "Re-submit : \n";
    
    foreach my $nums (@broken)
    {
	# print out the submit command
	my $recmd = "sbatch -a$nums\-$nums$3$4 $5 $6";
	print "$recmd\n";
	
	# if not in check mode, re-submit the jobs 
	if (!$check) {system($recmd);}
    }
}
print "\n";
