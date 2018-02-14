#!/usr/bin/env perl

use strict;


my $pref     = $ARGV[0];
my $qa       = $ARGV[1];
my $tgt      = $ARGV[2];
my $mineff   = $ARGV[3];
my $minreff  = $ARGV[4];

if (!defined $ARGV[0]) 
{
    print "USAGE:\n";
    print "autocut.pl [pref] [qa]\n\n";
    print "   [pref]     : file names prefix\n";
	print "   [qa]       : qa factor per iteration (e.g. 0.95 = 95% signal efficiency, -0.9 = 90% bg suppression\n";
	print "   [maxbg]    : background target (e.g. 0.0001 -> iterate until 0.1% bg level reached); default = 0.0001\n";
	print "   [min eff]  : stop process when absolute signal efficiency drops below this value; default = 0.1\n";
	print "   [min reff] : stop process when relative (to mass window(D* mass diff cut) efficiency drops below this value: default = 0.9\n\n";
    exit(0);
}

if (!defined($qa))  {$qa=0.95};
if (!defined($tgt)) {$tgt = 0.0001;}
if (!defined($mineff)) {$mineff = 0.1;}
if (!defined($minreff)) {$minreff = 0.9;}

my $macro = $ENV{"VMCWORKDIR"}."/macro/softrig/autocutx.C+";

# remove leading and trailing whitespace
sub  trim { my $s = shift; $s =~ s/^\s+|\s+$//g; return $s };

# sigma values for D* mass difference cut
my %mdifcuts = (
  110 => 3.9, 111 => 3.9, 112 => 3.9, 113 => 3.9, 114 => 3.9,
  115 => 4.3, 116 => 4.3, 117 => 4.3, 118 => 4.3, 119 => 4.3,
  130 => 1.2, 131 => 1.2, 132 => 1.2, 133 => 1.2, 134 => 1.2,
  135 => 3.8, 136 => 3.8, 137 => 3.8, 138 => 3.8,
  150 => 4.3, 151 => 4.3, 
);

# --------------
# main routine
# --------------


my @files = `ls $pref*`;

$pref =~ s/\*//g;

open (SELFILE, ">selection_$pref.cfg");
open (OUTFILE, ">auto_output_$pref.log");
			
foreach (@files)
{
	chomp;
	m/M(\d+)_n(\d+)_(\d+)S_(\d+)B.+root/;
	
	print "sqs: $1  mode: $2  S: ".($3*10000)."  B: ".($4*10000)."...\n"; 
	
	my $sqs    = $1;
	my $mode   = $2;
	my $Ns0    = $3*10000;
	my $Nb0    = $4*10000;
	my $precut = "";
	
	# use D* mass diff cut as precut
	if (defined $mdifcuts{$mode}) 
	{
		my $mean = 0.1422, my $win = 4.*$mdifcuts{$mode}/1000.;
		if ($mode>=150) {$mean = 0.1439;}
		elsif ($mode>=135) {$mean = 0.1408;}
		elsif ($mode>=130) {$mean = 0.1455;}

		$precut .="xmdif>".($mean-$win)."&&xmdif<".($mean+$win);
	}	
	
	my $command = "root -l -b -q '$macro(\"$_\",\"$precut\",$qa,$tgt,$mineff,$minreff)'";
	print $command."\n";
	my @out = `$command`;
	
	my $cut, my $seff, my $rseff, my $beff, my $rec;
	
	foreach (@out)
	{
		if ( m/^CUT : ($sqs$mode : .+)\n$/ ) {$cut = $1;}
		if ( m/^SIG EFF :\s+(\d+\.\d+)%\s+BG EFF :\s+(\d+\.\d+)%/) {$seff=$1; $beff=$2;}
		if ( m/^SIG REL :\s+(\d+\.\d+)%/) {$rseff=$1; }
		if ( m/^Eff\s+:(.+)/) {$rec = $1;}
#		if ( m/^SIG EVT: (\d+).+BG: (\d+) ev/) {$Ns = $1; $Nb = $2;}
	}
	
	my @receff = split /%/, $rec;
	
	my $min=100., my $max = 0., my $range="", my $minidx=0, my $maxidx=0;
	
	my $idx = 0;
	foreach my $tmprec (@receff)
	{
		trim;
		if ($tmprec>0 && $tmprec<$min) {$min=$tmprec; $minidx=$idx;}
		if ($tmprec>0 && $tmprec>$max) {$max=$tmprec; $maxidx=$idx;}
		$idx++;
	}
	
	if ($min<$max) {$range = sprintf ", range %.1f%% [%d] ... %.1f%% [%d]", $min, $minidx, $max, $maxidx;}
	
	printf "# %s : eff_s = %.1f%% (rel. %.1f%%%s)  eff_b = %.3f%%\n", $sqs.$mode, $seff, $rseff, $range, $beff;
	#if ($beff>0.01) {print "# ";}
	print  $cut."\n";
	
	printf SELFILE "# %s : eff_s = %.1f%% (rel. %.1f%%%s)  eff_b = %.3f%%\n", $sqs.$mode, $seff, $rseff, $range, $beff;
	#if ($beff>0.01) {print SELFILE "# ";}
	print SELFILE $cut."\n";
	print OUTFILE @out;
}

close (SELFILE); 
close (OUTFILE); 
