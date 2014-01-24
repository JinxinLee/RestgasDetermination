#!/usr/local/bin/perl -w

my @files = <data/DPM*>;

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

foreach my $file (@files)
{
  $file=~m/data\/(.+_(\d\d)(\d\d\d)_(.+)k)\.root/;
  my $name = $1;
  my $en = $nrgs{$2};
  my $mode = $3;
  my $num = $4*1000;

  my $command = "root -l -b -q 'toy_core.C+(\"$file\",$num, $en, 1, 0, $pres, $trkeff, $pideff, $pidmis, $mix, 1, $mode)'";
  print "$command\n"; 
  `$command &> /dev/null`;
}
