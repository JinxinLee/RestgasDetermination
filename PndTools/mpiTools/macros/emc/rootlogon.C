{
  gSystem->AddIncludePath("-I$VMCWORKDIR/emc");
  gSystem->AddIncludePath("-I$VMCWORKDIR/emc/EmcDigi");
  gSystem->AddIncludePath("-I$VMCWORKDIR/emc/EmcMC");
  gSystem->AddIncludePath("-I$VMCWORKDIR/emc/EmcTools");
  gSystem->AddIncludePath("-I$VMCWORKDIR/emc/EmcReco");
  gSystem->AddIncludePath("-I$VMCWORKDIR/emc/EmcData");
  gSystem->AddIncludePath("-I$VMCWORKDIR/pndbase");
  gSystem->AddIncludePath("-I$VMCWORKDIR/base");
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libEmc"); 
  gSystem->Load("libGeom.so");
  gSystem->Load("libMCStack.so");
  gSystem->Load("libGui.so");

  delete gRandom;
  gRandom=new TRandom3(0);
}
