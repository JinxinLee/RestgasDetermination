#!/usr/bin/env perl

use strict;


my $pref     = $ARGV[0];
my $qa       = $ARGV[1];

if (!defined $ARGV[0]) 
{
    print "USAGE:\n";
    print "autocut.pl [pref] [qa]\n\n";
    print "   [pref]    : file names prefix\n";
	print "   [qa]      : qa factor\n\n";
    exit(0);
}

if (!defined($qa)) {$qa=0.95};

my $macro = $ENV{"VMCWORKDIR"}."/macro/softrig/autocutx.C+";

# remove leading and trailing whitespace
sub  trim { my $s = shift; $s =~ s/^\s+|\s+$//g; return $s };

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
	
	my $sqs  = $1;
	my $mode = $2;
	my $Ns0  = $3*10000;
	my $Nb0  = $4*10000;
	
	my $command = "root -l -b -q '$macro(\"$_\",\"\",$qa)'";
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
	print  $cut."\n";
	
	printf SELFILE "# %s : eff_s = %.1f%% (rel. %.1f%%%s)  eff_b = %.3f%%\n", $sqs.$mode, $seff, $rseff, $range, $beff;
	print SELFILE $cut."\n";
	print OUTFILE @out;
}

close (SELFILE); 
close (OUTFILE); 
