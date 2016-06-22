#!/usr/bin/perl

my $para  = $ARGV[0];
my $mode  = $ARGV[1];
my $num   = $ARGV[2];
my $min   = $ARGV[3];
my $max   = $ARGV[4];

my $check = 0;

# print some usage information
if (!defined($para))
{
    print "\nSubmits analysis jobs over multiple files on the KRONOS cluster at GSI.\n\n";
    print "USAGE:\n";
    print "anasub.pl <pref> [mode] [num] [min] [max]\n";
    print "  <pref>  : Prefix of files with names data/pref_<run>_pid.root; if 'check_' added in front, commands are only printed\n";
    print "            If filename 'xxx.jobs' is given, all commands in file are treated.\n";
    print "  [mode]  : Mode number to be written in TTree; default:0\n";
    print "  [num]   : Number of files to be analysed per analysis job; default:50\n";
    print "  [min]   : First run; if not given, all files found are analysed)\n";
    print "  [max]   : Last run;  if not given, all files starting from [min] are analysed)\n\n";
    print "Example 1 : anasub.pl check_mysim 1 10  --> only prints out the jobs to be submitted\n";
    print "Example 2 : anasub.pl mysim 1 10        --> submits the jobs\n";
    print "Example 3 : anasub.pl mystudy.jobs      --> submits the jobs according to all commands in mystudy.jobs\n\n";
    
    exit(0);
}

# are we in check-only mode
if ($para=~s/^check_//) { $check = 1;}

# set some defaults
if (!defined($num))  {$num=50;}
if (!defined($mode)) {$mode=0;}

my @prefs;
my @commands;

# we have a file containing a list of sbatch commands
if ( $para =~ m/\.jobs$/ )
{
    open (in,"<$para");
    @commands= <in>;
    close in;

    foreach my $cmd (@commands)
    {
	chomp $cmd;
	if ( $cmd =~ m/^#/) {next;}
    
	print "\n".$cmd."\n";
	$cmd =~ m/(\d+)-(\d+)(.+)(job.*\.sh)\s+([\w,\/]+)\s+(.*)/;
	push(@prefs, $5);
    }
}
# we only have one prefix directly given as parameter
else
{
    push(@prefs, $para);
}

my $cnt = 1;

# for each entry in the commands array
foreach my $pref (@prefs)
{

    my $minfound=100000;
    my $maxfound=0;

    # if no min and max is given, find minimum and maximum number
    if (!defined($min) || !defined($max))
    {
	print "Searching for files with name data/$pref\_<run>_pid.root or data/$pref\_<run>_fsim.root...\n";

	my $suff = "pid";
	my @dir = `ls data/$pref\_*_pid.root`;
	if (@dir == 0) {@dir = `ls data/$pref\_*_fsim.root`; $suff="fsim";}
	
	foreach my $fname (@dir)
	{
	    if ($fname =~ m/$pref\_(\d+)_$suff.root/)
	    {
		if ($minfound>$1) {$minfound=$1;}
		if ($maxfound<$1) {$maxfound=$1;}
	    }
	}
    }
    
    if (!defined($min)) {$min=$minfound;}
    if (!defined($max)) {$max=$maxfound;}

    print "$min - $max\n";

    # if mode=-1 is given, the mode number is increase for each line in the .jobs file
    my $curr = $min;
    my $currmode = $mode;
    if ($mode==-1)
    {
	$currmode = $cnt;
    }

    # submit a bunch of jobs necessary to cover all run numbers in chunks of $num inputs 
    while ($curr<$max)
    {
	my $up = $curr+$num-1;
	if ($up>$max) {$up=$max;}
	my $cmd = "sbatch jobana_kronos.sh $pref $curr $up $currmode";
	print "$cmd\n";
	$curr+=$num;
	if (!$check) {system($cmd);}
    }
    
    $cnt++;
    undef($min);
    undef($max);
}
