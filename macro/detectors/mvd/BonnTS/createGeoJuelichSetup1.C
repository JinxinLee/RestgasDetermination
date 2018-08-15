	// Tracking station for the MVD of PANDA
        // Setup Nr. 1 used in Beamtime Feb/02 to Feb/06/2010
        // setup was used at Feb/02 and from Feb/04 until Feb/06
	
	// Simone Bianco 15/12/2009
	
	{
	  // Sizes of the detectors
	 
	  // Detectors defined as TGeoBBoxes, so the following kDx,kDy and kDz
	  // are half of the real sizes of the boxes
	
	  const Double_t  kDx  = 0.96;   // in cm!!
	  const Double_t  kDy  = 0.96;   // in cm!!
	  const Double_t  kDz  = 0.015;   // in cm!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
	  // Positions of the detector
	
	  // Sensors displaced along z, laying on perpendicular xy plane,
	  // there centered in (0,0)
	
	  // 2 double sided sensors
	
	  const Double_t  kPos1    = 11.0; // (cm) Box 1
	  const Double_t  kPos2    = 183.2; // (cm) Box 4
	
	  // single sided sensors
	  // this is the position of the center of the system
	  // containing the two single sides detectors
	
	  const Double_t  kPos3    = 80.7; // (cm) Box 2
	  const Double_t  kPos4    = 139.0; // (cm) Box 3
	 
	  // Gap between the two single sides sensors
	
	  const Double_t  kGap3    = 0.45; // (cm)
	  const Double_t  kGap4    = 0.45; // (cm)
	
	
	  //##################################################
	
	
	  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	 
	  // Load this libraries
	  gSystem->Load("libGeoBase");
	  gSystem->Load("libParBase");
	  gSystem->Load("libBase");
	  gSystem->Load("libPndData");
	  gSystem->Load("libPassive");
	 
	  TString outfile= "TrackingStation.root";
	  //TFile* fi = new TFile(outfile,"RECREATE"); 
	 
	  FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
	  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
	  geoFace->setMediaFile("../../../geometry/media_pnd.geo");
	  geoFace->readMedia();
	  geoFace->print();
	
	  //##################################################
	
	  FairGeoMedia *Media =  geoFace->getMedia();
	  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();
	
	  FairGeoMedium *CbmMediumAir  = Media->getMedium("air");
	  FairGeoMedium *CbmMediumSilicon = Media->getMedium("silicon");
	 
	  Int_t nmed=geobuild->createMedium(CbmMediumAir);
	  nmed=geobuild->createMedium(CbmMediumSilicon);
	
	  TGeoManager* gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
	
	  TGeoVolume *top = new TGeoVolumeAssembly("TS");
	                   
	  gGeoMan->SetTopVolume(top);
	 
	  TGeoShape *BoxDouble1 = new TGeoBBox("StripActiveTD1", kDx, kDy, kDz);
	  TGeoShape *BoxDouble2 = new TGeoBBox("StripActiveTD2", kDx, kDy, kDz);
	  TGeoShape *BoxSingle1 = new TGeoBBox("StripActiveTS1", kDx, kDy, kDz);
	  TGeoShape *BoxSingle2 = new TGeoBBox("StripActiveTS2", kDx, kDy, kDz);
	  TGeoShape *BoxSingle3 = new TGeoBBox("StripActiveTS3", kDx, kDy, kDz);
	  TGeoShape *BoxSingle4 = new TGeoBBox("StripActiveTS4", kDx, kDy, kDz);
	 
	  TGeoVolume* BoxVolDouble1 = new TGeoVolume("StripActiveTD1",BoxDouble1,gGeoMan->GetMedium("silicon"));
	  TGeoVolume* BoxVolDouble2 = new TGeoVolume("StripActiveTD2",BoxDouble2,gGeoMan->GetMedium("silicon"));
	  TGeoVolume* BoxVolSingle3a = new TGeoVolume("StripActiveTS3a",BoxSingle1,gGeoMan->GetMedium("silicon"));
	  TGeoVolume* BoxVolSingle3b = new TGeoVolume("StripActiveTS3b",BoxSingle2,gGeoMan->GetMedium("silicon"));
	  TGeoVolume* BoxVolSingle4a = new TGeoVolume("StripActiveTS4a",BoxSingle3,gGeoMan->GetMedium("silicon"));
	  TGeoVolume* BoxVolSingle4b = new TGeoVolume("StripActiveTS4b",BoxSingle4,gGeoMan->GetMedium("silicon"));
	
	
	  TGeoTranslation* trt1 = new TGeoTranslation(0.,0.,kPos1);
	  TGeoTranslation* trt2 = new TGeoTranslation(0.,0.,kPos2);
	  TGeoTranslation* trt3a = new TGeoTranslation(0.,0.,kPos3 - 0.5*kGap3 - kDz/2.);
	  TGeoTranslation* trt3b = new TGeoTranslation(0.,0.,kPos3 + 0.5*kGap3 + kDz/2.);
	  TGeoTranslation* trt4a = new TGeoTranslation(0.,0.,kPos4 - 0.5*kGap4 - kDz/2.);
	  TGeoTranslation* trt4b = new TGeoTranslation(0.,0.,kPos4 + 0.5*kGap4 + kDz/2.);
	
	  TGeoRotation* dummyrot = new TGeoRotation();
	  TGeoRotation* rot1 = new TGeoRotation("",90.,0.,0.);
	
	  TGeoCombiTrans* trc1= new TGeoCombiTrans(*trt1,*dummyrot);
	  trc1->SetName("Sensor1");
	  trc1->RegisterYourself();
	 
	  TGeoCombiTrans* trc2= new TGeoCombiTrans(*trt2,*dummyrot);
	  trc2->SetName("Sensor2");
	  trc2->RegisterYourself();
	
	  TGeoCombiTrans* trc3a= new TGeoCombiTrans(*trt3a,*dummyrot);
	  trc3a->SetName("Sensor3a");
	  trc3a->RegisterYourself();
	
	  //  TGeoCombiTrans* trc3b= new TGeoCombiTrans(*trt3b,*dummyrot);
	  TGeoCombiTrans* trc3b= new TGeoCombiTrans(*trt3b,*rot1);
	  trc3b->SetName("Sensor3b");
	  trc3b->RegisterYourself();
	
	  TGeoCombiTrans* trc4a= new TGeoCombiTrans(*trt4a,*dummyrot);
	  trc4a->SetName("Sensor4a");
	  trc4a->RegisterYourself();
	
	  //  TGeoCombiTrans* trc4b= new TGeoCombiTrans(*trt4b,*dummyrot);
	  TGeoCombiTrans* trc4b= new TGeoCombiTrans(*trt4b,*rot1);
	  trc4b->SetName("Sensor4b");
	  trc4b->RegisterYourself();
	
	  TGeoVolumeAssembly* SubunitVol = new TGeoVolumeAssembly("TTVol");
	
	  TGeoVolumeAssembly* SubunitDouble = new TGeoVolumeAssembly("TTDouble");
	
	  SubunitDouble->AddNode(BoxVolDouble1,0,trc1);
	  SubunitDouble->AddNode(BoxVolDouble2,0,trc2);
	
	  TGeoVolumeAssembly* SubunitSingle = new TGeoVolumeAssembly("TTSingle");
	
	  SubunitSingle->AddNode(BoxVolSingle3a,0,trc3a);
	  SubunitSingle->AddNode(BoxVolSingle3b,0,trc3b);
	  SubunitSingle->AddNode(BoxVolSingle4a,0,trc4a);
	  SubunitSingle->AddNode(BoxVolSingle4b,0,trc4b);
	 
	  SubunitVol->AddNode(SubunitDouble,0,new TGeoCombiTrans());
	  SubunitVol->AddNode(SubunitSingle,0,new TGeoCombiTrans());
	
	  top->AddNode(SubunitVol,0,new TGeoCombiTrans());
	
	  gGeoMan->CloseGeometry();
    gGeoMan->Export(outfile.Data());
	  //top->Write();
	  //fi->Close();
	  //gGeoManager->Export(outfile);
	  //top->Draw("ogl");
} 

