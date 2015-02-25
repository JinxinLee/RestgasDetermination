#!/usr/bin/env perl

use strict;

my @modes = qw( 100 101 102 103 104 110 111 112 113 114
                115 116 117 118 119 120 121 122 123 130
                131 132 133 134 135 136 137 138 140 141
                150 151 200 201 220 221 222 223 224 240
                241 400 410 420 421 422 500 600 601 602
                620 621 622 640 641 642 660 );

my $macro = "inspectmode.C+";

foreach (@modes)
{
	my $command = "root -l -b -q '$macro($_,\"col\")'";
	print $command."\n";
	`$command`;
}

