rootlogon()
{
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
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


  // Load Panda libraries
  if (isLibrary("libDpmEvtGen"))gSystem->Load("libDpmEvtGen");
  if(isLibrary("libGeoBase"))gSystem->Load("libGeoBase");
  if(isLibrary("libParBase"))gSystem->Load("libParBase");
  if(isLibrary("libBase"))gSystem->Load("libBase");
  if(isLibrary("libPndBase"))gSystem->Load("libPndBase");
  if(isLibrary("libGlobalTasks"))gSystem->Load("libGlobalTasks");
  if(isLibrary("libTrkBase"))gSystem->Load("libTrkBase");
  if(isLibrary("libPndData"))gSystem->Load("libPndData");
  if(isLibrary("libField"))gSystem->Load("libField");
  if(isLibrary("libPassive"))gSystem->Load("libPassive");
  if(isLibrary("libGen"))gSystem->Load("libGen");
  if(isLibrary("libPGen"))gSystem->Load("libPGen");
  if(isLibrary("libEmc"))gSystem->Load("libEmc"); 
  if(isLibrary("libgenfit"))gSystem->Load("libgenfit");
  if(isLibrary("libtrackrep"))gSystem->Load("libtrackrep");
  if(isLibrary("libgenfitAdapters"))gSystem->Load("libgenfitAdapters");
  if(isLibrary("libriemann"))gSystem->Load("libriemann");
  if(isLibrary("libtpc"))gSystem->Load("libtpc"); 
  if(isLibrary("libtpcreco"))gSystem->Load("libtpcreco");
  if(isLibrary("libStt"))gSystem->Load("libStt");
  if(isLibrary("libSttReco"))gSystem->Load("libSttReco");
  if(isLibrary("libMvd"))gSystem->Load("libMvd");
  if(isLibrary("libMvdReco"))gSystem->Load("libMvdReco");
  if(isLibrary("libMvdTrk"))gSystem->Load("libMvdTrk");
  if(isLibrary("libGem"))gSystem->Load("libGem");
  if(isLibrary("libTof"))gSystem->Load("libTof");
  if(isLibrary("libDrcProp"))gSystem->Load("libDrcProp");
  if(isLibrary("libDrc"))gSystem->Load("libDrc");
  if(isLibrary("libMdt"))gSystem->Load("libMdt");
  if(isLibrary("libDch"))gSystem->Load("libDch");
  if(isLibrary("libLheTrack"))gSystem->Load("libLheTrack");
  if(isLibrary("libGeane"))gSystem->Load("libGeane");
  if(isLibrary("libRpc"))gSystem->Load("libRpc");
  if(isLibrary("libLumi"))gSystem->Load("libLumi");
  if(isLibrary("libRho"))gSystem->Load("libRho");
  if(isLibrary("libTMVA"))gSystem->Load("libTMVA.so");
  if(isLibrary("libPid"))gSystem->Load("libPid");
  if(isLibrary("libAnalysisTools"))gSystem->Load("libAnalysisTools");
  if(isLibrary("librecotasks"))gSystem->Load("librecotasks");
  if(isLibrary("libRecoHits"))gSystem->Load("libRecoHits");
  if(isLibrary("libRecoTasks"))gSystem->Load("libRecoTasks");
  if(isLibrary("libEnDrc"))gSystem->Load("libEnDrc");
  if(isLibrary("libDsk"))gSystem->Load("libDsk");
  if(isLibrary("libGlobal"))gSystem->Load("libGlobal");
}

Bool_t isLibrary(const char* libName)
{
  if (TString(gSystem->DynamicPathName(libName, kTRUE)) != TString(""))
    return kTRUE;
  else  
    return kFALSE;
}
