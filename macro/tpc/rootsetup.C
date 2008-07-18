// Macro for displaying the tracks for the STT1 simulation
// only the STT1 detector is ON
// input file testrun.root contains the MC information
//13/09/2006 Pablo Genova


{

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libtpc");
      gSystem->Load("libTst");
  gSystem->Load("libGen");
  gSystem->Load("libgenfit");
 
  //gSystem->AddIncludePath("tpc");

  //TFile* file = new TFile("test.raw.root");
  //TGeoManager *geoMan = (TGeoManager*) file->Get("CBMGeom");
  
  //cbmsim->StartViewer();
}

