#!/bin/bash
#
# fillhisto.sh: fills a root histogram with gaussian distribution
#               using FillRandom.
#
# argument 1: run identification number
# argument 2: number of events
# argument 3: number of bins
# argument 4: output file name
#
# An example line in the job description file could look like:
#
# JOB 10 $HOME/scripts/fillhisto.sh NULL $HOME/output 3 1000 100 output.root
#
# Setup the environmental variables to run ROOT.
# For this run the config.sh in the build directory of PandaRoot.
# Please set the path according to your setup
#
PANDAROOTBUILD=$VMCWORKDIR/../build
#
source $PANDAROOTBUILD/config.sh > logfile
#
# Create simple root macro
#
cat << EOF > macro.C
{
 delete gRandom;
 gRandom=new TRandom3($1);
 TFile *f=new TFile("$4","recreate");
 TH1D *h=new TH1D("h","",$3,-5.,5.);
 h->FillRandom("gaus",$2);
 h->Write();
 f->Close();
}
EOF
#
# Run the scripts
#
root -l -b -q "macro.C" >> logfile 2>&1
#
# Validate the output and return the appropiate value
#
for ofile in "logfile" "macro.C" "$4" ; do  
  [ -f $ofile ]  || error="$error $ofile doesn't exist,";
done

for message in "Segmentation violation" "Segmentation fault" "Abort" "Bus error" "Floating point exception" "root: command not found" "cp: cannot stat" "Error opening Input file" ; do
 grep -i "$message" logfile &&  error="$error and $message"
done
#
# Success
#
if [ -z "$error"  ] ; then 
  exit 0
fi
#
# Failure
#
cat logfile
echo "<E> Validation failed with $error"
exit -1
