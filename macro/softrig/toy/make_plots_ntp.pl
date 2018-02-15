#!/usr/local/bin/perl -w

my @files = <*>;

my %nrgs = (
   "24" => 2.4,
   "38" => 3.77,
   "45" => 4.5,
   "55" => 5.5 
);


my $pres = 0.05;
my $trkeff = 95.;
my $pideff = 95.;
my $pidmis = 5.;
my $mix = 0;

`rm test.txt`;

foreach my $file (@files)
{
  if ($file=~m/(.+_(\d\d)(\d\d\d)_(.+)k)\.root/)
  {
	my $name = $1;
	my $en = $nrgs{$2};
	my $mode = $3;
	my $num = $4;

	my $command = "root -l -b -q 'toy_core_ntp.C+(\"$file\",$num,1)'";
	print "$command\n"; 
	`$command &> /dev/null`;
  }
}

foreach my $file (@files)
{
  if ($file=~m/(.+_(\d\d)(\d\d\d)_(.+)k)\.root/)
  {
	my $name = $1;
	my $en = $nrgs{$2};
	my $mode = $3;
	my $num = $4;

	my $command = "root -l -b -q 'toy_core_ntp.C+(\"$file\",$num,0)'";
	print "$command\n"; 
	`$command &> /dev/null`;
  }
}
