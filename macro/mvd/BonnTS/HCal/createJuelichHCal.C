	// Geometry for a prototype of calorimeter
	
	// Simone Bianco 21/12/2009
	
	{
	  // Detectors defined as TGeoBBoxes, so the following dx,dy and dz
	  // are half of the real sizes of the boxes

	  // Sizes of the scintillators
	  const Double_t  dxSci  = 1.4;   // in cm!!
	  const Double_t  dySci  = 1.4;   // in cm!!
	  const Double_t  dzSci  = 0.225;   // in cm!!
	
	  // Sizes of the absorbers
	  const Double_t  dxAbs  = 1.4;   // in cm!!
	  const Double_t  dyAbs  = 1.4;   // in cm!!
	  const Double_t  dzAbs  = 0.25;   // in cm!!

	  // number of stages in the sandwich of Scintillators and Absorbers
	  const Int_t nStages = 15;
	
	
	  //##################################################
	
	
	  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	 
	  // Load this libraries
	  gSystem->Load("libGeoBase");
	  gSystem->Load("libParBase");
	  gSystem->Load("libBase");
	  gSystem->Load("libPndData");
	  gSystem->Load("libPassive");
	 
	  std::string outfile= "HCal.root";
	  TFile* fi = new TFile(outfile.c_str(),"RECREATE"); 
	 
	  FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
	  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
	  geoFace->setMediaFile("../../../../geometry/media_pnd.geo");
	  geoFace->readMedia();
	  geoFace->print();
	
	  //##################################################
	
	  FairGeoMedia *Media =  geoFace->getMedia();
	  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();
	
	  FairGeoMedium *CbmMediumScint  = Media->getMedium("polyvinyltoluene");
	  FairGeoMedium *CbmMediumIron = Media->getMedium("iron");
	 
	  Int_t nmed=geobuild->createMedium(CbmMediumScint);
	  nmed=geobuild->createMedium(CbmMediumIron);
	
	  TGeoManager* gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
	
	  TGeoVolume *top = new TGeoVolumeAssembly("HCAL");
	                   
	  gGeoMan->SetTopVolume(top);


	TGeoVolumeAssembly* SubunitVol = new TGeoVolumeAssembly("TTVol");
	TGeoRotation* dummyrot = new TGeoRotation();
	TGeoRotation* rot1 = new TGeoRotation("",90.,0.,0.);

	std::string name1,name2;

	for (int i=0; i < nStages; i++)
	{

	  name1 = Form("TestHCalScintillator%d",i+1);
	  name2 = Form("TestHCalAbsorber%d",i+1);
	  TGeoShape* Box1 = new TGeoBBox(name1.c_str(), dxSci, dySci, dzSci);
	  TGeoShape* Box2 = new TGeoBBox(name2.c_str(), dxAbs, dyAbs, dzAbs);
	  TGeoVolume* BoxVolume1 = new TGeoVolume(name1.c_str(),Box1,gGeoMan->GetMedium("polyvinyltoluene"));
	  TGeoVolume* BoxVolume2 = new TGeoVolume(name2.c_str(),Box2,gGeoMan->GetMedium("iron"));
	  TGeoTranslation* translation1 = new TGeoTranslation(0.,0.,(double)i * 2. *(dzSci+dzAbs));
	  TGeoTranslation* translation2 = new TGeoTranslation(0.,0.,(double)i * 2. * (dzSci+dzAbs) + dzSci+dzAbs);

	  TGeoCombiTrans* geocombi1= new TGeoCombiTrans(*translation1,*dummyrot);
	  TGeoCombiTrans* geocombi2= new TGeoCombiTrans(*translation2,*dummyrot);
	  geocombi1->SetName("scintillator"+i);
	  geocombi2->SetName("absorber"+i);
	  geocombi1->RegisterYourself();
	  geocombi2->RegisterYourself();
	  SubunitVol->AddNode(BoxVolume1,0,geocombi1);
	  SubunitVol->AddNode(BoxVolume2,0,geocombi2);

	}

	  top->AddNode(SubunitVol,0,new TGeoCombiTrans());
	
	  gGeoMan->CloseGeometry();
	  //gGeoMan->Export(outfile.Data());
	  top->Write();
	  fi->Close();
	  //gGeoManager->Export(outfile);
	  //top->Draw("ogl");

	  std::string directory = gSystem->Getenv("VMCWORKDIR");
	  std::string move = Form("mv %s %s/geometry/.", outfile.c_str(), directory.c_str());
	  cout << "moving geometry to the correct folder" << endl;
	  system(move.c_str());
} 

