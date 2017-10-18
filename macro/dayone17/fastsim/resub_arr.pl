#!/usr/bin/perl

my $para  = $ARGV[0];
my $suff  = $ARGV[1];
my $check = defined($ARGV[2]);

if (!defined($suff)) {$suff="";}

if ($suff eq "check") {$check = true; $suff ="";}

# Example commands
# DPM:    sbatch -a1-20 jobsim_kronos.sh DPM10GeV 1000 DPM         10.0 saveall
# EvtGen: sbatch -a1-20 jobsim_kronos.sh D0Kpi    1000 D0toKpi.dec 10.0 saveall

# print some usage information
if (!defined($para))
{
    print "\nChecks the jobs output (<prefix>_<num>_<suff>.root existing and reasonable in size) and resubmits all failed ones (KRONOS version).\n\n";
    print "USAGE:\n";
    print 'resub_arr.pl "<cmd>" [check]'."\n";
    print "  <cmd>   : The complete sbatch command line in quotes (job script name must be 'job...sh'), the -a parameter the first.\n";
    print "            The 'sbatch -a' at the beginning can optionally be skipped.\n";
    print "            If given a file name 'xxx.jobs' (w/o quotes) instead, the above is done for every command listed in the file.\n";
    print "  <suff>  : Suffix of the filenames; filepattern looked for is <prefix>_<num>_<suff>.root\n";
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

my $linecnt=0;
my $totresub=0;
my @resubs;

# little helper function to remove whitespace at begin/end of string
sub trim { my $s = shift; $s =~ s/^\s+|\s+$//g; return $s };

# read out the queued jobs for this user
my @runjobs = `squeue -u \$USER`;



# ----------------------------------------------------------
#  loop over all sbatch commands
# ----------------------------------------------------------


foreach my $cmd (@commands)
{
 	# ----------------------------------------------------------
	#  analyse sbatch command
	# ----------------------------------------------------------

   	# cut away the CR and NL
    chomp $cmd;
	
	# empty line only consisting of whitespace?
	if ( $cmd =~ m/^\s*$/) {next;}
    
	# if commented line (first char = '#'), skip
    if ( $cmd =~ m/^#/) {next;}
    
    print "\n\n** LINE $linecnt: ".$cmd."\n";
	$linecnt+=1;
	
	# analyse sbatch command
    $cmd =~ m/(\d+)-(\d+)(.+)(job.*\.sh)\s+(\w+)\s+(.*)/;

    my $min     = $1; # min array number
    my $max     = $2; # max array number
    my $parms   = $3; # further parameters of sbatch
    my $script  = $4; # the job script
    my $pref    = $5; # prefix (for file names to look for)
    my $rest    = $6; # the whole rest of the sbatch command line
    
    # what kind of output suffix: full sim, fast sim, quick ana tool?
    # this is determined by the name of the job submission script (not so robust somehow) --> FIXME for better solution
	if ($suff eq "")
	{
    	$suff    = "pid";
    	if ($script =~ /jobfsim/) {$suff = "fsim";}   # do we have fast sim output
    	if ($script =~ /jobquickfa/) {$suff = "ana";} # do we have ana output from quickana tool
    }
    
    print "Checking for files \"data/$pref"."_<run>_$suff.root\" for runs $min - $max (cmd opt: \"-a$min-$max $parms $script $pref $rest\")\n\n";
    
	
	# --------------------------------------------------------------
	#  find all numbers from queued/running jobs with name if given
	# --------------------------------------------------------------
   
    my @running=(), @queued=(), @runque=();
    
    # are the jobs named, and we can check the queue for running/queued jobs?
    my $name="";
	
	if ($parms =~ /-J(\w+)/)
    {
		$name = $1;
		# find numbers of jobs with that name already queued or running ==> won't be resubmitted 
		foreach my $ljob (@runjobs)
		{
			# outline of squeue command
			my $ss = trim($ljob);
			
			# something like 1221987_273 could be running or queued
			if ($ss =~ m/^\d+_(\d+)\s+\w+\s+(\w+)/)
			{
				# with correct name?
				if ($name eq $2) 
				{
					# is it of status ... R ... (running)?
					if ($ss =~ m/\s+R\s+/) {push @running, $1;push @runque, $1;}
					# or not
					else {push @queued, $1;push @runque, $1;}
				}
			}
			
			# something like 1221987_[56-29] or 1221987_[56-29 (missing ] bug in output?) is queued
			if ($ss =~ m/^\d+_\[(\d+)-(\d+)\]*\s+\w+\s+(\w+)/)
			{
				if ($name eq $3)
				{
					for (my $ii=$1; $ii<=$2; ++$ii) {push @queued, $ii;push @runque, $ii;}
				}
			}
		} 
	}
	
	# sort array numbers
	@running = sort {$a <=> $b} @running;
	@queued  = sort {$a <=> $b} @queued;
	@runque  = sort {$a <=> $b} @runque;
	
	# get numbers of running, queued and both
	my $nrunning = scalar @running;
	my $nqueued  = scalar @queued;
	my $runque   = scalar @runque;
	
	# print info about running jobs
	if ($nrunning>0)
	{
		printf "Running '%s' (%4d) : ", $name, $nrunning;
		foreach my $jnum (@running) {print $jnum." ";}
		print "\n";
	}

	# print info about queued (not yet running) jobs
	if ($nqueued>0)
	{
		printf "Queued '%s' (%4d)  : ", $name, $nqueued;
		foreach my $jnum (@queued) {print $jnum." ";}
		print "\n\n";
	}
	
	
	# ----------------------------------------------------------
	#  now check for output files
	# ----------------------------------------------------------
    
    my @broken=(), @nexist=(), @small=();

    # find run numbers of non-existing and too small file
    for (my $i=$min; $i<=$max; $i++)
    {
		# check whether missing number is already queued or running 
		my $inque = grep( /^$i$/, @runque );
		
		# this is the output name we are looking for
		my $fname = "data/".$pref."_".$i."_$suff.root";

		if (!-e $fname) 
		{
			# only add to broken numbers, if not queued/running
	    	if (!$inque) {push(@broken, $i); }
	    	# however store as not-existing
	    	push(@nexist, $i);
		}
		else
		{
	    	my $filesize = -s $fname;
	    	if ($filesize<10000)
	    	{
				# only add to broken numbers, if not queued/running
				if (!$inque) {push(@broken, $i);}
	    		# however store as too small file
				push(@small,  $i);
	    	}
		}
    } 
	
	$totresub += scalar @broken;

    # print out numbers of failed jobs
	my $nnexist = scalar @nexist;
	my $nsmall  = scalar @small;
	
	# the current number of job number to be resubmitted (non-existing/too small and NOT queued/running)
	my $locresub = scalar @broken;
	
	# all files are there -> fine!
	if ($nnexist+$nsmall==0) 
	{
		print "--> All ok!";
	}
	else
	{
		# print non-exist numbers
    	printf "Not existing (%4d) : ", $nnexist;
    	foreach my $run (@nexist) {print "$run ";}
		# print too small file numbers
    	printf "\nSmall file   (%4d) : ", $nsmall;
    	foreach my $run (@small) {print "$run ";}
    	
		# if all missing files have a queued/running job, do nothing
    	if ($locresub==0)
    	{
			print "\n\n--> Nothing to re-submit...";
		}
    }
	
	print "\n\n";
    
    
	# ----------------------------------------------------------
   	#  re-submit jobs
	# ----------------------------------------------------------
	
	if ($locresub>0)
	{
		# only print-out what would be done?
    	if ($check) {print "Would ";}
    	print "Re-submit : \n";
		
		my $amin = -1;
		my $last = -1;
		
    	foreach my $nums (@broken)
    	{
			if ($amin == -1) {$amin=$nums; $last=$nums;}
			if ($nums-$last>1)
			{
				# print out the submit command
				my $recmd = "sbatch -a$amin\-$last$parms$script $pref $rest";
				print "$recmd\n";

				push @resubs, $recmd;

				# if not in check mode, re-submit the jobs 
				if (!$check) {system($recmd);}
				
				$last = $nums;
				$amin = $nums;
			}
			$last = $nums;			
    	}
		# print out the submit command
		my $recmd = "sbatch -a$amin\-$last$parms$script $pref $rest";
		print "$recmd\n";

		push @resubs, $recmd;
		
		# if not in check mode, re-submit the jobs 
		if (!$check) {system($recmd);}
		
		
	}
}


# ----------------------------------------------------------
#  print a summary of all (to be) submitted jobs
# ----------------------------------------------------------

print "\n**** Re-submit summary:\n\n";

foreach my $c (@resubs) {print "$c\n";}
print "\n\n";

if ($check) {print "****  Total number of jobs to be re-submitted: $totresub  ****";}
else {print "****  Re-submitted $totresub jobs  ****";}
print "\n\n";
