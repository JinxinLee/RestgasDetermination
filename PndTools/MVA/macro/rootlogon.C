rootlogon()
{
  
  // Load basic libraries
  // If it does not work,  please check the path of the libs and put it by hands
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  /*
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
  */
  // Header paths
  gSystem->AddIncludePath("-I$VMCWORKDIR/fairtools/");
  gSystem->AddIncludePath("-I$VMCWORKDIR/emc/EmcData/");
  gSystem->AddIncludePath("-I$VMCWORKDIR/emc/EmcCorr/");
  gSystem->AddIncludePath("-I$VMCWORKDIR/emc/EmcDigi/");
  gSystem->AddIncludePath("-I$VMCWORKDIR/emc/EmcMC/");
  gSystem->AddIncludePath("-I$VMCWORKDIR/emc/EmcReco/");
  gSystem->AddIncludePath("-I$VMCWORKDIR/emc/EmcTools/");
  gSystem->AddIncludePath("-I$VMCWORKDIR/pndbase/PndStdUtils/");
  gSystem->AddIncludePath("-I$VMCWORKDIR/pnddata/");
  gSystem->AddIncludePath("-I$VMCWORKDIR/pnddata/TrackData/");
  gSystem->AddIncludePath("-I$VMCWORKDIR/base/");
  gSystem->AddIncludePath("-I$VMCWORKDIR/trackbase/");
  gSystem->AddIncludePath("-I$VMCWORKDIR/parbase/");
  
  //gSystem->Load("libMemStat");
}
