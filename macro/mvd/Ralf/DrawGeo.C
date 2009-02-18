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
//  gSystem->Load("libStt1"); 
////  gSystem->Load("libStt2");
//  gSystem->Load("libMuo");
//  gSystem->Load("libEmc");
  gSystem->Load("libMvd");
//  gSystem->Load("libDrc");

  gSystem->Load("libGeom.so");
  TFile* file = new TFile("../data/testMCgeo.root");

  TGeoManager *geoMan = (TGeoManager*) file->Get("FAIRGeom");

  TCanvas* c1 = new TCanvas("c1", "", 100, 100, 1100, 1100);
  c1->SetFillColor(10);

  // Number of folded geo levels; 8 for mvd.
  // Try to change the number and to see how the geometry changes.
  geoMan->SetVisLevel(100);
  geoMan->DefaultColors();
  geoMan->GetMasterVolume()->Draw("ogl");
//   geoMan->GetMasterVolume()->Draw("same");  c1->x3d();

std::cout<<"------------------------------"<<std::endl;
std::cout<<"------------------------------"<<std::endl;
std::cout<<"------------------------------"<<std::endl;

  geoMan->GetListOfVolumes()->Print();
}

