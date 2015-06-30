{
  // Gem disk geometry parameters
  //-----------------------------
  const Double_t  kDisk1InnerRadius    = 5.0;   // (cm) //from EMC TDR
  const Double_t  kDisk2InnerRadius    = 10.0;   // (cm) //?
  const Double_t  kDisk3InnerRadius    = 10.0;   // (cm) //?

  const Double_t  kDisk1OuterRadius    = 90.0;   // (cm) //from EMC TDR
  const Double_t  kDisk2OuterRadius    = 105.0;   // (cm) //?
  const Double_t  kDisk3OuterRadius    = 150.0;   // (cm) //from EMC TDR

  const Double_t  kDisk1ZPosition = 110.0;   // (cm) //from EMC TDR
  const Double_t  kDisk2ZPosition = 140.0;   // (cm) //from EMC TDR
  const Double_t  kDisk3ZPosition = 190.0;   // (cm) //from EMC TDR

  const Double_t  kHalfFoilThickness  = 0.1; // Thickness of sensitive foil (cm)


  //--------------------------------------------------------------------
  TString outfile= "../../geometry/gem.root";
  TFile* fi = new TFile(outfile,"RECREATE");  
  
  FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  geoFace->setMediaFile("../../geometry/media_pnd.geo");
  geoFace->readMedia();
  geoFace->print();

  FairGeoMedia *Media =  geoFace->getMedia();
  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();

  FairGeoMedium *CbmMediumAir  = Media->getMedium("air");
  FairGeoMedium *CbmMediumPWO = Media->getMedium("PWO");
  FairGeoMedium *CbmMediumCarbon  = Media->getMedium("carbon");
  FairGeoMedium *CbmMediumAluminium = Media->getMedium("aluminium");
  
  Int_t nmed=geobuild->createMedium(CbmMediumAir);
  nmed=geobuild->createMedium(CbmMediumPWO);
  nmed=geobuild->createMedium(CbmMediumCarbon);
  nmed=geobuild->createMedium(CbmMediumAluminium);

  TGeoManager* gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");

  TGeoVolume *top = new TGeoVolumeAssembly("Gem");
                    
  gGeoMan->SetTopVolume(top);
  

TGeoShape *Disk1Shape = new TGeoTube("disk1shape",kDisk1InnerRadius,kDisk1OuterRadius,kHalfFoilThickness);
TGeoShape *Disk2Shape = new TGeoTube("disk2shape",kDisk2InnerRadius,kDisk2OuterRadius,kHalfFoilThickness);
TGeoShape *Disk3Shape = new TGeoTube("disk3shape",kDisk3InnerRadius,kDisk3OuterRadius,kHalfFoilThickness);

TGeoVolume* Disk1Vol = new TGeoVolume("Gem_Disk1_Sensor",Disk1Shape,gGeoMan->GetMedium("air"));
TGeoVolume* Disk2Vol = new TGeoVolume("Gem_Disk2_Sensor",Disk2Shape,gGeoMan->GetMedium("air"));
TGeoVolume* Disk3Vol = new TGeoVolume("Gem_Disk3_Sensor",Disk3Shape,gGeoMan->GetMedium("air"));

TGeoTranslation* trt1 = new TGeoTranslation(0.,0.,kDisk1ZPosition);
TGeoTranslation* trt2 = new TGeoTranslation(0.,0.,kDisk2ZPosition);
TGeoTranslation* trt3 = new TGeoTranslation(0.,0.,kDisk3ZPosition);

TGeoRotation* dummyrot = new TGeoRotation();

TGeoCombiTrans* trc1= new TGeoCombiTrans(*trt1,*dummyrot); 
trc1->SetName("Gem_Disk1_Sensor");
trc1->RegisterYourself();

TGeoCombiTrans* trc2= new TGeoCombiTrans(*trt2,*dummyrot); 
trc2->SetName("Gem_Disk2_Sensor");
trc2->RegisterYourself();

TGeoCombiTrans* trc3= new TGeoCombiTrans(*trt3,*dummyrot); 
trc3->SetName("Gem_Disk3_Sensor");
trc3->RegisterYourself();


TGeoVolumeAssembly* SubunitVol = new TGeoVolumeAssembly("Gem_Disks");
SubunitVol->AddNode(Disk1Vol,0,trc1);
SubunitVol->AddNode(Disk2Vol,0,trc2);
SubunitVol->AddNode(Disk3Vol,0,trc3);


top->AddNode(SubunitVol,0,new TGeoCombiTrans());

  gGeoMan->CloseGeometry();
  top->Write();
  fi->Close();
//   gGeoManager->Export(outfile);
  top->Draw("ogl");
}  
