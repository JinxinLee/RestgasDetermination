#!/usr/bin/env perl 

use strict;


my $cfgfile  = $ARGV[0];
my $sqs      = $ARGV[1];
my $mode     = $ARGV[2];

if (!defined $ARGV[0]) 
{
    print "USAGE:\n";
    print "create_decfiles.pl <config filename> [sqrt(s)] [mode]\n\n";
	print "   <config filename> : file containing decay infos (default: decays.txt)\n";
    print "   [sqrt(s)]         : modes accessible up to sqrt(s) [optional]\n";
	print "   [mode]            : modes beginning with 'mode' [optional]\n\n";
     
    exit(0);
}

if (!defined($sqs)) {$sqs=6.;}

open(TEMPLATE, "<$cfgfile");
my(@lines) = <TEMPLATE>; 
close(TEMPLATE);

my $line;      # single line
my %recoil;    # hash array of recoils (00 .. 09)
my %antipart;  # hash array turing a particle to it's antiparticle
my %m0;        # hash array mapping particle names to masses in GeV
my %thres;     # hash array holding the min sqrt(s) for every mode
my %decfile;   # hash containing full decay files for each mode
my %decay;     # decay strings
my %assocdec;  # associated decay particles (e.g. D-/D*- for a D+ decay)

####
# Subroutines
####

# remove leading and trailing whitespace
sub  trim { my $s = shift; $s =~ s/^\s+|\s+$//g; return $s };

# determine a string with antiparticle names
sub antistr
{
	my $decstr = $_[0];
	my $decstr2 = $_[0];
	my @decp1 = split / /, $_[0];
	for (my $i=0; $i<@decp1; ++$i)
	{
		my $pa = $decp1[$i];
		my $ap = $antipart{$pa};
		if (defined($ap)) { $decp1[$i] = $ap;}
		my $tok = $decp1[$i];
		$decstr =~ s/\Q$tok\E//;
	}
	my $decpat1 = join(" ",@decp1);
	if ($decstr =~ m/^\s*$/) { return $decstr2;}
	return $decpat1;
}

# determine mass of all particles in string
sub mass
{
	my @parts = split /\s+/, $_[0];
	my $m = 0.;

    foreach my $p (@parts){ if (defined($m0{$p})) { $m = $m + $m0{$p};} }
	return $m;
}

####
# Start main script
####

foreach $line (@lines) # loop thru lines
{
	chomp($line);
	if ($line eq "" || $line =~ m/^#/) {next;} # ignore empty lines and those staring with #

	my @toks = split / /, $line;               # break into tokens separated by single space
    my $n = @toks;
	if ($n==0) {next;}                         # no tokens? next!

    # particle definition: Particle <particle name> <anti particle name> mass 
	if ($toks[0] eq "Particle")                
	{
		if ($n==3) 
		{
			$antipart{$toks[1]}=$toks[1];
			$m0{$toks[1]} = $toks[2];
		}
		else 
        {
			$antipart{$toks[1]} = $toks[2];
			$antipart{$toks[2]} = $toks[1];
			$m0{$toks[1]} = $toks[3];
			$m0{$toks[2]} = $toks[3];
		}
	}

	# recoil definition
	if ($toks[0] eq "Recoil")                 
	{
		if ($n==2) {$recoil{trim($toks[1])}="";}
		else
        {
			my @sub = @toks; shift @sub; shift @sub;
			$recoil{trim($toks[1])} = join(" ", @sub);
        }
	}

	# associated decay particles
	if ($toks[0] eq "GenerateWith")                 
	{
		my @sub = @toks; shift @sub; shift @sub;
		$assocdec{$toks[1]} = join(" ",@sub);
#		print "'".$toks[1]."' -> '".$assocdec{$toks[1]}."'\n";
	}
}   

# foreach my $key (keys %assocdec)
# {
# 	print "$key : ";
# 	my @sub = split / /, $assocdec{$key};
# 	print join(", ", @sub)."\n";
# }
# 
# exit;
# print "Found Particles:\n";
# foreach my $key ( sort keys %antipart )
# {
# 	print $key." <-> ".$antipart{$key}."  m = ".$m0{$key}."\n";
# }
# exit();
# print "\n\nFound Recoils:\n";
# foreach my $key ( sort keys %recoil )
# {
# 	print $key." <-> ".$recoil{$key}."\n";
# }
# exit();
# 


foreach $line (@lines) # loop again thru lines
{
	chomp($line);
	if ($line eq "" || $line =~ m/^#/) {next;}

	my @toks = split / /, $line;
    my $n = @toks;
	if ($n==0) {next;}
	
	#
	# defintion of a decay
	#
	if ($toks[0] eq "Decay")
	{
		#
		# loop over all recoils
		#
		foreach my $key (sort keys %recoil) 
		{
			my $rec = $recoil{$key};

			$line =~ /^Decay\s+(\d+)\s+([A-Za-z0-9*_\-\/+]+).+/;

			my @sub = ("");

			if (defined($assocdec{$2})) 
			{
				@sub = split / /, $assocdec{$2}; 
			}
	
			for my $i (0 .. $#sub) 
			{
				my $ass = $sub[$i];

				my $dec;
				my $code = $1.$i.$key; # complete code consisting of mode and recoil
				
				if (defined($mode) && $code !~ m/^\Q$mode\E/)  {next;}

				$dec = "# M$code";
				if ($recoil{$key} ne "") {$dec .= " with recoil $rec";}
				$dec .= "\n";
				
				#
				# the D* modes with subdecay
				#
				if ($line =~ /^Decay\s+(\d+)\s+([A-Za-z0-9*_\-\/+]+)\s+(.+)\s+\[(.+)\]\s+(.+)/)
				{
					my $ap2 = $antipart{$2};
					my $ap3 = $antipart{$3};
					$thres{"$code"} = mass("$2 $ass $rec");
					$decay{"$code"} = "$2 $ass $rec";
					$dec .= "# ppb -> ".$decay{$code}."\n# sqrt(s) > ".$thres{$code}."\n\nnoPhotos\n\nAlias KS K_S0\n\n";

					$dec .= "Alias MyP1  $2\n";
					$dec .= "Alias MyAP1 $ap2\n";
					$dec .= "Alias MyP2  $3\n";
					$dec .= "Alias MyAP2 $ap3\n\n";

					$dec .= "Decay pbarpSystem0\n";
		#			foreach my $key (sort keys %recoil) {
						$dec .= " 1.0 MyP1  $ass $rec PHSP;\n";
						$dec .= " 1.0 MyAP1 ".$antipart{$ass}." $rec PHSP;\n";
		#			}
					$dec .= "Enddecay\n\n";
					$dec .= "Decay MyP1\n 1.0 MyP2 $5;\nEnddecay\n\n";
					$dec .= "Decay MyP2\n 1.0 $4;\nEnddecay\n\n";
					$dec .= "Decay MyAP1\n 1.0 MyAP2 ".antistr($5).";\nEnddecay\n\n";
					$dec .= "Decay MyAP2\n 1.0 ".antistr($4).";\nEnddecay\n\n";
				}
				#
				# the other modes
				#
				elsif ($line =~ /^Decay\s+(\d+)\s+(\S+)\s+(.+)$/)
				{
					#
					# all modes with a signal resonance
					#
					if ($2 ne "NR")
					{
						my $ap = $antipart{$2};
						#
						# if anti-particle exists (for the baryon and D Dbar channels)
						#
						if (defined($ap) && $ap ne $2)
						{
							$thres{"$code"} = mass("$2 $ass $rec");
							$decay{"$code"} = "$2 $ass $rec";
							$dec .= "# ppb -> ".$decay{$code}."\n# sqrt(s) > ".$thres{$code}."\n\nnoPhotos\n\nAlias KS K_S0\n\n";

							$dec .=  "Alias MyP  $2\n";
							$dec .=  "Alias MyAP $ap\n\n";

							$dec .=  "Decay pbarpSystem0\n";
							#foreach my $key (sort keys %recoil) {
								$dec .=  " 1.0 MyP  $ass $rec PHSP;\n";
								$dec .=  " 1.0 MyAP ".$antipart{$ass}." $rec PHSP;\n";
							#}
							$dec .=  "Enddecay\n\n";
							$dec .=  "Decay MyP\n 1.0 $3;\nEnddecay\n\n";
							$dec .=  "Decay MyAP\n 1.0 ".antistr($3).";\nEnddecay\n\n";
						}
						#
						# else charmonium with ppb -> P X
						#
						else {
							$thres{"$code"} = mass("$2 $ass $rec");
							$decay{"$code"} = "$2 $ass $rec";
							$dec .= "# ppb -> ".$decay{$code}."\n# sqrt(s) > ".$thres{$code}."\n\nnoPhotos\n\nAlias KS K_S0\n\n";

							$dec .=  "Alias MyP $2\n\n";
							$dec .=  "Decay pbarpSystem0\n";
							#foreach my $key (sort keys %recoil) {
								$dec .=  " 1.0 MyP $ass $rec PHSP;\n";
							#}
							$dec .=  "Enddecay\n\n";
							$dec .=  "Decay MyP\n 1.0 $3;\n";
							my $anti = antistr($3);
							if ($anti ne $3) { $dec .=  " 1.0 ".antistr($3).";\n";}
							$dec .=  "Enddecay\n\n";
						}
					}
					#
					# non resonant modes (ppb -> e+ e- X, etc)
					#
					else
					{
						$thres{"$code"} = mass("$3 $rec");
						$decay{"$code"} = "$3 $rec";
						$dec .= "# ppb -> ".$decay{$code}."\n# sqrt(s) > ".$thres{$code}."\n\nnoPhotos\n\nAlias KS K_S0\n\n";
						
						$dec .=  "Decay pbarpSystem0\n";
						#foreach my $key (sort keys %recoil) {
							$dec .=  " 1.0 $3 $rec PHSP;\n";

						#}
						$dec .=  "Enddecay\n\n";
					}
				}
				$dec .=  "Decay KS\n 1.0 pi+ pi- PHSP;\nEnddecay\n\n";
				$dec .=  "End\n\n";
				$decfile{$code} = $dec;

			} #assoc 
		} # recoils
	}
}   
if (defined($mode)) {print "mode = $mode\n";}
my $mp0 = $m0{"p+"};
foreach my $code (sort keys %decfile)
{
	if (!defined($mode) || $mode eq "" || $code=~m/^\Q$mode\E/) 
	{
		my $minp = 0.;
		my $s    = $thres{$code}*$thres{$code};
		if (sqrt($s)>2.*$mp0) {$minp = sqrt(($s-2.*$mp0*$mp0)*($s-2.*$mp0*$mp0)/(4.*$mp0*$mp0)-$mp0*$mp0);}
		if ($sqs>$thres{$code})
		{
			printf "%6d  (sqs > %6.4f GeV; p > %7.4f GeV/c)  %s\n", $code, sqrt($s), $minp, $decay{$code};
		}
		print $decfile{$code};
		print "------------------\n";
 		open (MYFILE, ">decfiles/M$code.dec");
 		print MYFILE $decfile{$code};
 		close (MYFILE); 
	} 
}

my $i = 0;
print "\@modes = qw( ";
foreach my $name (sort { $thres{$a} <=> $thres{$b} or $a cmp $b } keys %thres) 
{
	printf "%03d%s ",$thres{$name}*100+1,$name;
	if (++$i%10 == 0) { print "\n             ";}
	#printf "%03d%s %5.3f %s\n", $thres{$name}*100+1, $name, $thres{$name}, $decay{$name};
}
print ");\n";
