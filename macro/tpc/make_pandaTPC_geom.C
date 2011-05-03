{
  using namespace std;

  //get geometry definitions:
  FairGeoLoader* loader = new FairGeoLoader("TGeo", "FairGeoLoader");
  FairGeoBuilder* build = loader->getGeoBuilder();

  FairGeoInterface* geoIFC = loader->getGeoInterface();
  TString workdir = gSystem->Getenv("VMCWORKDIR");
  cout << workdir.Data() << endl;
  geoIFC->setMediaFile(workdir+"/geometry/media_pnd.geo");
  geoIFC->readMedia();
  geoIFC->print();
  FairGeoMedia* media = geoIFC->getMedia();
  
  double paramsHem1[6] = {15.5,   //rMin
			  41.5,   //rMax
			  150.,   //dZ
			  95.,
			  265. };
  
  TGeoTubeSeg* hemi1 = new TGeoTubeSeg(paramsHem1);
  
  FairGeoMedium* rohacell = media->getMedium("rohacell");
  build->createMedium(rohacell);

  
  TGeoManager* geoMan = (TGeoManager*) gROOT->FindObject("FAIRGeom");

  TList* mediaList = (TList*)geoMan->GetListOfMedia();
  for(unsigned int i=0; i<mediaList->GetEntries(); i++) {
    TGeoMedium* imed = (TGeoMedium*) mediaList[i];
    cout<<"Medium "<<i<<": "<<imed->GetName()<<endl;
  }

  TGeoVolume* HEM1 = new TGeoVolume("HEM1", hemi1, rohacell);
  geoMan->SetTopVolume(HEM1);

  TEveManager::Create();
  TGeoNode* top = geoMan->GetTopNode();

  TEveGeoTopNode* meh = new TEveGeoTopNode(geoMan, top);
  gEve->AddGlobalElement(meh);

  gEve->Redraw3D(true);
  

}

