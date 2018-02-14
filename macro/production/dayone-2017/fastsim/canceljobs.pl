#!/usr/bin/perl

my $opt  = $ARGV[0];
my $par  = $ARGV[1];
my $check = defined($ARGV[2]);

if (!defined($par)) {$par="";}


# print some usage information
if (!defined($opt) || $par eq "")
{
    print "\nChecks the jobs output (<prefix>_<num>_<suff>.root existing and reasonable in size) and resubmits all failed ones (KRONOS version).\n\n";
    print "USAGE:\n";
    print 'canceljobs.pl <opt> <par> [check]'."\n";
    print "  <opt>   : Option -tlt (lower than), -tgt (greater than)\n";
    print "  <par>   : value of option (time in hrs)\n";
    print "  [check] : Optional parameter 'check', which just prints out what would be cancelled\n\n";
    print "  Example\n > canceljobs.pl -tgt 02:00 \n\n";
    exit(0);
}

my $user =  $ENV{'USER'};
my @jobs = `squeue -u $user`;

print "$par\n";

# for each entry in the commands array
foreach my $line (@jobs)
{
    # cut away the CR and NL
    chomp $line;
	
	my ($dummy, $jobid, $part, $name, $usern, $status ,$time, $nnodes, $node) = split /\s+/, $line;
	
	#print "id=$jobid q=$part n=$name u=$usern s=$status t=$time N=$nnodes c=$node\n";
	
	my $hrs=0;
	if ($time =~ /(\d+):(\d+):(\d+)$/)
	{
		$hrs = $1 + $2/60.;
	}
	elsif ($time =~ /(\d+):(\d+)$/)
	{
		$hrs = $1/60. + $2/3600.;
	}	
	
	my $cmd;
		
	if ($opt eq "-tgt" && ($par<$hrs))
	{
		$cmd = "scancel $jobid";
	}
	
	if ($opt eq "-tlt" && ($par>$hrs))
	{
		$cmd = "scancel $jobid";
	}
	
	if ($cmd ne "")
	{
		print "id=$jobid q=$part n=$name u=$usern s=$status t=$time N=$nnodes c=$node\n";
		print "$cmd\n";
	}
	if (!$check) {system($cmd);}
	
}	
