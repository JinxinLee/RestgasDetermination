
  using namespace std;

  //----------------------- INIT --------------------------------------

  //get geometry definitions:
  FairGeoLoader* loader = new FairGeoLoader("TGeo", "FairGeoLoader");
  FairGeoBuilder* builder = loader->getGeoBuilder();

  FairGeoInterface* geoIFC = loader->getGeoInterface();
  TString workdir = gSystem->Getenv("VMCWORKDIR");
  cout << workdir.Data() << endl;
  geoIFC->setMediaFile(workdir+"/geometry/media_pnd.geo");
  geoIFC->readMedia();
  //geoIFC->print();
  FairGeoMedia* media = geoIFC->getMedia();
  TGeoManager* geoMan = (TGeoManager*) gROOT->FindObject("FAIRGeom");



  //------------------ LOAD MATERIALS ----------------------------------

  FairGeoMedium* rohacell = media->getMedium("rohacell");
  builder->createMedium(rohacell);



  //---------------- CONSTRUCT FIELDCAGE ------------------------------
TGeoVolumeAssembly* createFieldCage() {
  double cageRohaOut1_meas[6] = { 41.,   //rMin
			       41.5,   //rMax
			       150.,   //dZ
			       95.,
			       265. };
  double cageRohaIn1_meas[6] = { 15..,   //rMin
				 15.5,   //rMax
				 150.,   //dZ
				 95.,
				 265. };
  
  
  
  TGeoTubeSeg* cageRohaOut1 = new TGeoTubeSeg(cageRohaOut1_meas);
  TGeoTubeSeg* cageRohaIn1 = new TGeoTubeSeg(cageRohaIn1_meas);
  
  TGeoVolumeAssembly* FieldCage = new TGeoVolumeAssembly("FieldCage");

  TGeoVolume* OUT1 = new TGeoVolume("OUTROHA1", cageRohaOut1, 
				    geoMan->GetMedium("rohacell"));
  FieldCage->AddNode(OUT1,1);
  TGeoVolume* IN1 = new TGeoVolume("INROHA1", cageRohaIn1, 
				    geoMan->GetMedium("rohacell"));
  FieldCage->AddNode(IN1,1);

}

  

  geoMan->SetTopVolume(FieldCage);

  TEveManager::Create();
  TGeoNode* top = geoMan->GetTopNode();

  TEveGeoTopNode* meh = new TEveGeoTopNode(geoMan, top);
  gEve->AddGlobalElement(meh);

  gEve->Redraw3D(true);
  

}

