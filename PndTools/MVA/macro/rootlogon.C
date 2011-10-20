rootlogon()
{
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();

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

  gSystem->Load("libMemStat");  
}
/*
Bool_t isLibrary(const char* libName)
{
  if (TString(gSystem->DynamicPathName(libName, kTRUE)) != TString(""))
    return kTRUE;
  else  
    return kFALSE;
}
*/
