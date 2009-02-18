Libs()
{
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libField");
  gSystem->Load("libPassive");

  gSystem->Load("libGen");
//   gSystem->Load("$VMCWORKDIR/pgenerators/DpmEvtGen/lib/libDpmEvtGen.so");
  gSystem->Load("libDpmEvtGen");
  gSystem->Load("libPGen");

//   gSystem->Load("libStt");
//   gSystem->Load("libEmc"); 
//   gSystem->Load("libTof");
//   gSystem->Load("libDrcProp");
//   gSystem->Load("libDrc");
//   gSystem->Load("libMdt");
//   gSystem->Load("libDch");

  gSystem->Load("libgenfit");
  gSystem->Load("libTrkBase");
  gSystem->Load("libtrackrep");
  gSystem->Load("libtpc");
  gSystem->Load("libtpcreco");
  gSystem->Load("librecotasks");
  gSystem->Load("libLHETrack");
//   gSystem->Load("libGeane");

  gSystem->Load("libMvd");
  gSystem->Load("libMvdReco");


}
