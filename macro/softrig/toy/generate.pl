#!/usr/bin/perl -w

use strict;

if ( @ARGV!=2 ) 
{
    print "\nUSAGE:\n";
    print "generate.pl <mode> <nevts>\n\n";
    print "  mode  : mode code for soft trigger, e.g. 38201: J/psi pi+ pi-, J/psi -> e+ e- at E_cm = 3.77 GeV\n";
    print "          mode='all' generates all Evt modes (caution! takes long)\n";
    print "          mode='core' generates 10 Evt core modes (caution! takes also long)\n";
    print "          mode='dpm' generates all DPM data\n";
    print "  nevts : number of events to be generated\n\n";
    exit(0);
}

my $enmode  = $ARGV[0];
my $nevts   = $ARGV[1];

my %moms = (
   "24" => 1.913547,
   "38" => 6.56903,
   "43" => 8.773474,
   "45" => 9.808065,
   "50" => 12.348596,
   "55" => 15.152765 
);

my @dpm    = qw(24900 38900 45900 55900);

my @core24 = qw(24000 24300 24400);
my @core38 = qw(38000 38100 38120 38200 38201 38221 38300 38400);
my @core45 = qw(45000 45100 45120 45140 45200 45201 45221 45300 45400);
my @core55 = qw(55000 55100 55120 55140 55200 55201 55221 55300 55400 55420);

my @modes24 = qw(24000 
                 24300 24320 24340 
                 24400 
                 24900);

my @modes38 = qw(38000 
                 38100 38101 38102 
                 38120 38121 38122 38123 
                 38200 38201 
                 38220 38221 38222 38223 
                 38240 38241 38242 
                 38300 38320 38340 
                 38400
                 38900);

my @modes45 = qw(45000 
                 45100 45101 45102 45103 45104 45105 45106 45107 
                 45120 45121 45122 45123 45124 45125 45126 45127 45128 45129 45130 45131
                 45140 45141 45142 45143 45144 45145
                 45200 45201 
                 45220 45221 45222 45223 
                 45240 45241 45242 
                 45300 45320 45340 
                 45400 45420
                 45900);

my @modes55 = qw(55000 
                 55100 55101 55102 55103 55104 55105 55106 55107 
                 55120 55121 55122 55123 55124 55125 55126 55127 55128 55129 55130 55131
                 55140 55141 55142 55143 55144 55155
                 55200 55201 
                 55220 55221 55222 55223 
                 55240 55241 55242 
                 55300 55320 55340 
                 55400 55420
                 55900);


my @modelist;
if ($enmode eq "all")
{
	push (@modelist,@modes24);
	push (@modelist,@modes38);
	push (@modelist,@modes45);
	push (@modelist,@modes55);
}
elsif ($enmode eq "core")
{
	push (@modelist,@core24);
	push (@modelist,@core38);
	push (@modelist,@core45);
	push (@modelist,@core55);
}
elsif ($enmode eq "dpm")
{
	push (@modelist,@dpm);
}
else
{
	push (@modelist,$enmode);
}

print "Generating modes: ".join(", ", @modelist)."\n";

my $res  = "pbarpSystem0";

foreach (@modelist)
{
	my $en   = substr($_,0,2);
	my $pmom = $moms{$en};
	my $mode = substr($_,2,3);
	my $decfile = `ls ../decfiles/M$mode*dec 2> /dev/null`;
	chomp($decfile);

	if ($nevts<=0) {print "invalid number of events: $nevts\n";exit(0);}
	if ($decfile eq "" && $mode ne "900") {print "no decay file found for mode $mode\n";exit(0);}
	if ($pmom<1) { print "invalid pmom\n"; exit(0);}

	my $nevtsk   = $nevts/1000;

	if ($mode!="900")
	{
		my $outfile = "EVT_".$_."_".$nevtsk."k.root";
        print "Generating EvtGen mode $_(p=$pmom), $nevts evts -> data/$outfile\n";
        if (-e "evtOutput.root") {`rm evtOutput.root`;}
		`./simpleEvtGenRO $res $decfile $nevts $pmom > /dev/null`;
        if (-e "evtOutput.root") {
          `mv evtOutput.root data/$outfile > /dev/null`;
		}
		else {print "Failed to generate mode $_\n";}
	}
	else
	{
		my $outfile = "DPM_".$_."_".$nevtsk."k.root";
        print "Generating DPMGen mode $_(p=$pmom), $nevts evts -> data/$outfile\n";
		`echo 0 $pmom 0 $nevts | ./DPMGen 2> /dev/null`;
        `mv Background-micro.root data/$outfile`;
	}
}