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
//   gSystem->Load("libMvd");
//  gSystem->Load("libDrc");

  TFile* file = new TFile("teststation.root");

 //A
//   TGeoManager *geoMan = (TGeoManager*) file->Get("CADtoROOT");
//   TGeoVolume* topvol = geoMan->GetMasterVolume();

 //B
  TGeoManager *geoMan = new TGeoManager();
  TGeoVolume* topvol = (TGeoVolume*)file->Get("top");
  if(!topvol) cout<<"Error: topvolume is not there"<<endl;
  geoMan->SetTopVolume(topvol);

 //C
//   TGeoManager *geoMan = (TGeoManager*) file->Get("FAIRGeom");

  // Number of folded geo levels; 8 for mvd.
  // Try to change the number and to see how the geometry changes.


  geoMan->SetVisLevel(100);
  geoMan->DefaultColors();
  topvol->Draw("ogl");

std::cout<<"------------------------------"<<std::endl;
std::cout<<"------------------------------"<<std::endl;
std::cout<<"------------------------------"<<std::endl;

  geoMan->GetListOfVolumes()->Print();

//   geoMan->CheckOverlaps();
//   geoMan->PrintOverlaps();
}

