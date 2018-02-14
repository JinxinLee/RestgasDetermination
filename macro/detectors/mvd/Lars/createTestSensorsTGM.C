{


  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  // Load this libraries
//  gSystem->Load("libGeoBase");
//  gSystem->Load("libParBase");
//  gSystem->Load("libBase");
//  gSystem->Load("libPndData");
//  gSystem->Load("libPassive");

  TString outfile= "teststation.root";
//  TFile* fi = new TFile(outfile,"RECREATE");

  TString mediafile = gSystem->Getenv("VMCWORKDIR");
  mediafile+="/geometry/media_pnd.geo";

  FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  geoFace->setMediaFile(mediafile);
  geoFace->readMedia();
  geoFace->print();

  FairGeoMedia *Media =  geoFace->getMedia();
  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();

  FairGeoMedium *CbmMediumAir  = Media->getMedium("air");
  FairGeoMedium *CbmMediumSilicon = Media->getMedium("silicon");
  FairGeoMedium *CbmMediumCarbon  = Media->getMedium("carbon");
  FairGeoMedium *CbmMediumAluminium = Media->getMedium("aluminium");

  Int_t nmed=geobuild->createMedium(CbmMediumAir);
  nmed=geobuild->createMedium(CbmMediumSilicon);
  nmed=geobuild->createMedium(CbmMediumCarbon);
  nmed=geobuild->createMedium(CbmMediumAluminium);

  TGeoManager* GeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");

  TGeoVolume *top = new TGeoVolumeAssembly("SiliconTestStation");

  GeoMan->SetTopVolume(top);


  // materials & media
  TGeoMaterial *matVacuum = new TGeoMaterial("vacuum",0,0,0);
  TGeoMaterial *matSilicon = new TGeoMaterial("silicon", 28.0855, 14, 2.33);
  TGeoMedium *medVacuum  = new TGeoMedium("vacuum", 1, matVacuum);
  TGeoMedium *medSilicon = new TGeoMedium("silicon", 2, matSilicon);

  TGeoVolumeAssembly* assamb = new TGeoVolumeAssembly("DummysensorAss");

  TGeoShape* shape = new TGeoBBox(0.96, 0.96, 0.015);
  TGeoRotation* rot = new TGeoRotation("rot",0.,0.,0.);//2.Stelle in 10er
  TGeoCombiTrans *trafo = new TGeoCombiTrans(0., 0., 4., rot);
  TGeoVolume* sensor = new TGeoVolume("Module15",shape,GeoMan->GetMedium("silicon"));

  TGeoShape* shape2 = new TGeoBBox(0.96, 0.96, 0.015);
  TGeoRotation* rot2 = new TGeoRotation("rot",90.,0.,0.);//2.Stelle in 10er
  TGeoCombiTrans *trafo2 = new TGeoCombiTrans(0., 0., 4.5, rot2);
  TGeoVolume* sensor2 = new TGeoVolume("Module16",shape2,GeoMan->GetMedium("silicon"));

  assamb->AddNode(sensor,0,trafo); 
  assamb->AddNode(sensor2,0,trafo2); 
  top->AddNode(assamb,0,new TGeoHMatrix());
cout<<""<<endl;
cout<<"----------------------------------------------------------"<<endl;
cout<<""<<endl;
  GeoMan->Print();
cout<<""<<endl;
cout<<"----------------------------------------------------------"<<endl;
cout<<""<<endl;


  GeoMan->CloseGeometry();  
  GeoMan->Export(outfile.Data());

// top->Export(outfile.Data(),"top","recreate");
//  top->Draw("ogl");


}
