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
  gSystem->Load("libPndData");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libGen");
  gSystem->Load("libGeom.so");

  gSystem->Load("libEmc");
  gSystem->Load("libStt");
  gSystem->Load("libMuo");
  gSystem->Load("libEmc");
  gSystem->Load("libTof");
  gSystem->Load("libDrcProp");
  gSystem->Load("libDrc");
  gSystem->Load("libtpc");
  gSystem->Load("libMvd");
 
  TFile* file = new TFile("mc.root");
  TGeoManager *geoMan = (TGeoManager*) file->Get("FAIRGeom");

  
  TCanvas* c1 = new TCanvas("c1", "", 100, 100, 800, 800);
  c1->SetFillColor(10);

  geoMan->SetVisLevel(4);
  geoMan->GetMasterVolume()->Draw("");

  TView* view = c1->GetView();
  view->Top();
  view->Centered();
  //  view->SetParralel();
  

}

