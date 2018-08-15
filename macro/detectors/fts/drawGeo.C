

drawGeo()
{

  /*  
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  
  TFile* file = new TFile("params.root");
  file->Get("FairBaseParSet"); 
  
  gGeoManager->SetVisLevel(3); //defoult 3
  gGeoManager->GetMasterVolume()->Draw("ogl");
  */

  TFile* file = new TFile("simparams.root"); //params.root
  
  if (!gGeoManager) {
    file->Get("FairBaseParSet");
    TGeoManager *geoMan = gGeoManager;
    if(!geoMan) {
      file->Get("FairGeoParSet");
      geoMan = gGeoManager;
      if(!geoMan) {
	std::cout<<"Could not find valid GeoManager. Abort now!"<<std::endl;
	exit(1);
      }
    }
    geoMan->SetVisLevel(3);
    geoMan->GetMasterVolume()->Draw("ogl");
  }

}



