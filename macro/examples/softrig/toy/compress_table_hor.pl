#!/usr/local/bin/perl -w

open(FILE, "<", "test.txt") or die("Can't open file");
my @lines = <FILE>;

my @nrgs = (2.4, 3.77, 4.5, 5.5);

my %nrgsidx = (
   "2.4" => 0,
   "3.8" => 1,
   "4.5" => 2,
   "5.5" => 3 
);

my @modes = ("e^+e^-","\\phi", "\\eta_c", "J/\\psi(ee)", "J/\\psi(\\mu\\mu)",  "D^0", "D^\\pm", "D_s","\\Lambda", "\\Lambda_c", "DPM");

my %modesidx = (
  "000" => 1,
  "400" => 8,
  "300" => 0,
  "200" => 3,
  "201" => 4,
  "221" => 2,
  "100" => 5,
  "120" => 6,
  "140" => 7,
  "420" => 9,
  "900" => 10
);

my @effi;
my $off=0;


foreach my $line (@lines)
{
	if ($line =~ m/^(\d{3})\s*&\s*(\d\.\d).+&\s*(.+)\s\\\\/)
	{
		my $idx = $modesidx{$1}*4+$nrgsidx{$2};
		$effi[$idx+$off] = $3;

		if ( ($1 eq "420") && ($2 eq "5.5") ) { $off=50;}
	}
}

open (TEX, '>test.tex');

printf ("%-4s   ","");
printf TEX ("%-4s  ",""); 
for (my $k=0;$k<11;++$k)
{
	printf ("%10s",$modes[$k]);
	printf TEX (" & %s",$modes[$k]);
}
print "\n";
print TEX "\\\\\\hline\n";

for (my $j=0; $j<4; $j++)
{
    printf("%-4s | ",$nrgs[$j]);
    printf TEX ("%5s ",$nrgs[$j]);
	for (my $i=0; $i<11; $i++)
	{
 		my $val = $effi[$j+$i*4];
		if (defined($val)) 
		{
			printf (" %8.2f ", $val);
			printf TEX (" & %8.2f", $val);
		}
		else 
		{
			print "     -    ";
			print  TEX " &      -  ";
		}
	}
    print "\n";
    print TEX "\\\\\n";
}
print "\n";
print TEX "\n";


printf ("%-4s   ","");
printf TEX ("%-4s  ",""); 
for (my $k=0;$k<11;++$k)
{
	printf ("%10s",$modes[$k]);
	printf TEX (" & %s",$modes[$k]);
}
print "\n";
print TEX "\\\\\\hline\n";

for (my $j=0; $j<4; $j++)
{
    printf("%-4s | ",$nrgs[$j]);
    printf TEX ("%5s ",$nrgs[$j]);
	for (my $i=0; $i<11; $i++)
	{
 		my $val = $effi[$j+$i*4+50];
		if (defined($val)) 
		{
			printf (" %8.2f ", $val);
			printf TEX (" & %8.2f", $val);
		}
		else 
		{
			print "     -    ";
			print  TEX " &      -  ";
		}
	}
    print "\n";
    print TEX "\\\\\n";
}
print "\n";
print TEX "\n";


