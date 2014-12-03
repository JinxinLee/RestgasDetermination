#!/usr/bin/env perl

use strict;

my $file     = $ARGV[0];

if (!defined $ARGV[0]) 
{
    print "USAGE:\n";
    print "printvars.pl [file]\n\n";
    print "   [file]    : file containing the selection cuts\n\n";
    exit(0);
}

# remove leading and trailing whitespace
sub  trim { my $s = shift; $s =~ s/^\s+|\s+$//g; return $s };

open(TEMPLATE, "<$file");
my(@lines) = <TEMPLATE>; 
close(TEMPLATE);

my %vars;

foreach my $line (@lines)
{
	if ($line =~ m/^[\d]{6}/) 
	{
		chomp $line;
		my @toks = split /:/, $line;
		$toks[1] =~ s/\s+//;
		#print $toks[1]."\n";
		my @cuts = split /&&/, $toks[1];
		
		foreach (@cuts)
		{
			#print $_." ";
			m/(^[a-zA-Z0-9_]+)[<>]/;
			my $var = $_;
			if (defined $1) {$var = $1;} 
			if (defined $vars{$var}) {$vars{$var}+=1;}
			else {$vars{$var} = 1;}
		}
	}
}

my $cnt = 0;

foreach ( sort keys %vars)
{
	printf "(%3d) %-15s ", $cnt++, $_;
	if ($cnt%5 == 0) {print "\n";}
}
print "\n";
