#!/usr/bin/env perl

use strict;


my $pref     = $ARGV[0];
my $qa       = $ARGV[1];

if (!defined $ARGV[0]) 
{
    print "USAGE:\n";
    print "autocut.pl [pref] [qa]\n\n";
    print "   [pref]    : prefix of file names\n";
	print "   [qa]      : qa factor\n\n";
    exit(0);
}

if (!defined($qa)) {$qa=0.95};

my $macro = $ENV{"VMCWORKDIR"}."/macro/softrig/autocutx.C+";

# --------------
# main routine
# --------------

my @files = `ls $pref*`;

open (SELFILE, ">selection.cfg");
open (OUTFILE, ">auto_output.log");
			
foreach (@files)
{
	chomp;
	m/M(\d+)_n(\d+)_(\d+)S_(\d+)B.+root/;
	
	print "sqs: $1  mode: $2  S: ".($3*10000)."  B: ".($4*10000)."\n"; 
	
	my $sqs  = $1;
	my $mode = $2;
	my $Ns0  = $3*10000;
	my $Nb0  = $4*10000;
	
	my $command = "root -l -b -q '$macro(\"$_\",\"\",$qa)'";
	print $command."\n";
	my @out = `$command`;
	
	my $cut;
	my $Ns;
	my $Nb;
	
	foreach (@out)
	{
		if (m/^($sqs$mode : .+)\n$/) {$cut = $1;}
		if (m/^SIG EVT: (\d+).+BG: (\d+) ev/) {$Ns = $1; $Nb = $2;}
	}
	print "CUT: $cut  eff_s: $Ns/$Ns0 = ".($Ns/$Ns0)."  eff_b: $Nb/$Nb0 = ".($Nb/$Nb0)."\n";
	print OUTFILE @out;
	print SELFILE $cut."\n";
}

close (SELFILE); 
close (OUTFILE); 
