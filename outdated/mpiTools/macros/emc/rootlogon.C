rootlogon()
{
  gSystem->AddIncludePath("-I$VMCWORKDIR/emc");
  gSystem->AddIncludePath("-I$VMCWORKDIR/emc/EmcDigi");
  gSystem->AddIncludePath("-I$VMCWORKDIR/emc/EmcMC");
  gSystem->AddIncludePath("-I$VMCWORKDIR/emc/EmcTools");
  gSystem->AddIncludePath("-I$VMCWORKDIR/emc/EmcReco");
  gSystem->AddIncludePath("-I$VMCWORKDIR/emc/EmcData");
  gSystem->AddIncludePath("-I$VMCWORKDIR/pnddata");
  gSystem->AddIncludePath("-I$VMCWORKDIR/base");
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  gSystem->Load("libDpmEvtGen.so");
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libTrkBase");
  gSystem->Load("libPndData");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libGen");
  gSystem->Load("libPGen");
  gSystem->Load("libEmc"); 

  delete gRandom;
  gRandom=new TRandom3(0);
}

Bool_t isLibrary(const char* libName)
{
  if (TString(gSystem->DynamicPathName(libName, kTRUE)) != TString(""))
    return kTRUE;
  else  
    return kFALSE;
} 
