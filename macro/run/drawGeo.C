drawGeo()
{
  TFile* file = new TFile("simparams.root");
  
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

