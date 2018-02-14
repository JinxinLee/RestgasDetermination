int geotester(TString filename="../../../geometry/MVD_v1.0_woPassiveTraps.root")
{

  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  TFile* file = new TFile(filename.Data());

 //A
//   TGeoManager *geoMan = (TGeoManager*) file->Get("CADtoROOT");
//   TGeoVolume* topvol = geoMan->GetMasterVolume();

 //B
  TGeoManager *geoMan = new TGeoManager();
  TGeoVolume* topvol = (TGeoVolume*)file->Get("topNode");
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
  return 0;
}

