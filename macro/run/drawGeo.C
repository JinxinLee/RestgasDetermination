{

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libPndData");
  gSystem->Load("libStt"); 
  gSystem->Load("libMdt");
  gSystem->Load("libEmc");
  gSystem->Load("libMvd");
  gSystem->Load("libDrcProp");
  gSystem->Load("libDrc");

  gSystem->Load("libGeom.so");
  TFile* file = new TFile("testsimu10.root");

 TGeoManager *geoMan = (TGeoManager*) file->Get("FAIRGeom");

//  TCanvas* c1 = new TCanvas("c1", "", 100, 100, 1100, 1100);
//  c1->SetFillColor(10);

  geoMan->SetVisLevel(4);
  geoMan->GetMasterVolume()->Draw("ogl");


}

