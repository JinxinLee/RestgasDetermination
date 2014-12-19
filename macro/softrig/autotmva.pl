#!/usr/bin/env perl

use strict;


my $pref     = $ARGV[0];
my $tgt      = $ARGV[1];
my $varfile  = $ARGV[2];

if (!defined $ARGV[0]) 
{
    print "USAGE:\n";
    print "autotmva.pl [pref] [qa]\n\n";
    print "   [pref]     : file names prefix\n";
	print "   [maxbg]    : background target (e.g. 0.0001 -> determine cut on TMVA output with 0.01% bgk level); default = 0.0001\n";
	print "   [var file] : file with variable lists; can be selection.cfg - variables will be determined automatically\n";
	print "                will only be applied for cases with n_var > 1\n\n";
    exit(0);
}

if (!defined($tgt)) {$tgt = 0.0001;}

my $trnmacro = $ENV{"VMCWORKDIR"}."/macro/softrig/TMVATraining.C+";
my $tstmacro = $ENV{"VMCWORKDIR"}."/macro/softrig/TMVAApply.C+";

# remove leading and trailing whitespace
sub  trim { my $s = shift; $s =~ s/^\s+|\s+$//g; return $s };

# count number of variables in string
sub  cntvars { my $s = shift; my @to = split / /, $s; return 0+@to };

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

my %varhints;
my %cuts;
my %comment;

if (defined($varfile))
{
	open(TEMPLATE, "<$varfile");
	my(@lines) = <TEMPLATE>; 
	close(TEMPLATE);

	foreach my $line (@lines)
	{
		chomp $line;
		if ($line =~ m/^# (\d{6})\s*:\s*(.+)/) {$comment{$1} = $line;}
		if ($line !~ m/^(\d{6})\s*:\s*(.+)/) {next;}
		
		my $mode = $1;	
		my $cut = $2;
		$cuts{$mode} = $line;
		
		$cut =~ s/&&/ /g;
		$cut =~ s/=//g;
		$cut =~ s/[<|>]-?\d+.?\d*//g;

		my @toks = split / /, $cut;
		my $vars = " ";
		foreach my $tok (@toks)
		{
			if ($tok =~ m/tag/) {next;}
			if ($vars =~ m/ $tok /) {next;}
			$vars .= $tok." ";
		}
		
		$vars = trim $vars;
		#print $mode.".".$vars."\n";
		$varhints{$mode} = $vars;
	}
}

my @files = `ls $pref*`;

$pref =~ s/[\*\[\]\|]//g;

open (SELFILE, ">selection_TMVA_$pref.cfg");
			
foreach my $file (@files)
{
	chomp $file;

	if ($file !~ m/M(\d+)_n(\d+)_(\d+)S_(\d+)B.+root/) {next;}
	
	#print "sqs: $1  mode: $2  S: ".($3*10000)."  B: ".($4*10000)."...\n"; 
	
	my $sqs    = $1;
	my $mode   = $2;
	my $Ns0    = $3*10000;
	my $Nb0    = $4*10000;
	my $precut = "";
	my $code = $sqs.$mode;
	
	# use D* mass diff cut as precut
	if (defined $mdifcuts{$mode}) 
	{
		my $mean = 0.1422, my $win = 4.*$mdifcuts{$mode}/1000.;
		if ($mode>=150) {$mean = 0.1439;}
		elsif ($mode>=135) {$mean = 0.1408;}
		elsif ($mode>=130) {$mean = 0.1455;}

		$precut .="xmdif>".($mean-$win)."&&xmdif<".($mean+$win);
	}	
		
	my $vars = "";
	if (defined($varhints{$code})) { $vars = $varhints{$code};}

	if (cntvars($vars)>2) 
	{
		# perform the training
		my $trncom = "root -l -b -q '$trnmacro(\"$file\",\"$vars\",\"$precut\")'";
		print $trncom."\n";
		system($trncom);
		print "\n";
		
		# determine the cut for target background level
		my $tstcom = "root -l -b -q '$tstmacro(\"$file\",\"$vars\",\"$precut\",\"\",$tgt)'\n";
		print $tstcom."\n";
		my @out = `$tstcom`;

		my $seff, my $beff, my $cfgline;

		foreach (@out)
		{
			if (m/^S : eff = (\d+\.\d+)/) {$seff = $1*100.;}
			if (m/^B : eff = (\d+\.\d+)/) {$beff = $1*100.;}
			if (m/^cfgline -> (.+)/) {$cfgline = $1;}
		}
	
		printf "# %s : eff_s = %.1f%%  eff_b = %.3f%%\n", $code, $seff, $beff;	
		printf $cfgline."\n\n";	
		printf SELFILE "# %s : eff_s = %.1f%%  eff_b = %.3f%%\n", $code, $seff, $beff;	
		printf SELFILE $cfgline."\n\n";	
	}
	else
	{
		if (defined($comment{$code}))  
		{
			print $comment{$code}."\n";
			print SELFILE $comment{$code}."\n";
		}
		if (defined($cuts{$code}))  
		{
			print $cuts{$code}."\n";
			print SELFILE $cuts{$code}."\n";
		}
	}
}
close (SELFILE); 
