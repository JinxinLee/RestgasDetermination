
void create_lumi_geanetest(){
  // Lumi Si-Set Parameter
  //-----------------------------
  const Double_t  kTrapInnerDist    = 3.;   // (cm) //Dimension of Trapezoid
  const Double_t  kTrapOuterDist    = 10.;   // (cm)
  const Int_t     kNumTraps= 8;

  const Double_t  kTrapHalfAngle = TMath::Pi()/(Double_t)kNumTraps; // (rad)
  const Double_t  kTrapHigh = kTrapOuterDist - kTrapInnerDist;

  //vars for trap-builder
  const Double_t  kShortSide = kTrapInnerDist * tan(kTrapHalfAngle);
  const Double_t  kLongSide = kTrapOuterDist * tan(kTrapHalfAngle);

  Double_t kPlaneZPosition[4];
  kPlaneZPosition[0] = 0.0;   // (cm) //Dist to IP before rotating
  kPlaneZPosition[1] = 10.0;   // (cm) //
  kPlaneZPosition[2] = 20.0;   // (cm) //
  kPlaneZPosition[3] = 30.0;   // (cm) //

  const Double_t  kHalfFoilThickness  = 0.1; // Thickness of sensitive foil (cm)
  // const Double_t  kHalfFoilThickness  = 0.0035; // Thickness of sensitive foil (cm) TEST

   const Double_t  kRot = 2.326; //(deg) //Rotate to dipol
   const Double_t  kRotUmZ = 476.03; //(cm) //z-point to rotate
  // const Double_t  kRot = 0; //(deg) //Rotate to dipol TEST
  // const Double_t  kRotUmZ = 0; //(cm) //z-point to rotate TEST
  
  const Double_t  kTransZ = 1100; //(cm) //move at z-position

  const Double_t  kTransX = 25; //(cm) //move at x-position

  //--------------------------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  
  // Load this libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libPassive");
  
  TString outfile= "lmd_geane_test.root";
  TFile* fi = new TFile(outfile,"RECREATE");  
  
  FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  geoFace->setMediaFile("../../geometry/media_pnd.geo");
  geoFace->readMedia();
  geoFace->print();

  FairGeoMedia *Media =  geoFace->getMedia();
  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();

  FairGeoMedium *FairMediumSilicon = Media->getMedium("air");
  
  Int_t nmed=geobuild->createMedium(FairMediumSilicon);

  TGeoManager* gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");

  TGeoVolume *top = new TGeoVolumeAssembly("Gem");
                    
  gGeoMan->SetTopVolume(top);
  
  //  TGeoShape *Trap1Shape = new TGeoTrd1("trap1shape",kShortSide,kLongSide,kHalfFoilThickness,kTrapHigh/2.);
  TGeoShape *Trap1Shape = 
    new TGeoTrap("trap1shape",kHalfFoilThickness, 0, -90, kTrapHigh/2.,kShortSide,kLongSide,0,
		 kTrapHigh/2.,kShortSide,kLongSide,0);

 
  //rotations and translations for all 4*8 traps
  TList trc;
  TList vol;
  Double_t move = (kTrapHigh/2.+kTrapInnerDist);

  for(Int_t j=0; j<4; ++j){
    std::stringstream plane;
    plane << (j+1);
    for(Int_t i=0; i<8; ++i){
      std::stringstream num;
      num << (i+1);
      TString name = ("LumActiveTrap"+plane.str()+"_"+num.str());
      TString rotname = ("rot"+plane.str()+"_"+num.str());
      Double_t angle = i*TMath::Pi()/4.;
      TGeoVolume* tmpvol = new TGeoVolume(name,Trap1Shape,gGeoMan->GetMedium("air"));
      // TGeoBBox* actBox1 = (TGeoBBox*)(tmpvol->GetShape());
      // cout<<"tmpvol.X="<<actBox1->GetDX()<<" "<<"tmpvol.Y="<<actBox1->GetDY()<<" "<<"tmpvol.Z="<<actBox1->GetDZ()<<endl;
      Double_t dreh = -i*45.;
      if(j==1) {dreh += 22.5; angle-=TMath::Pi()/8.;}   //sort out ambiguities 
      if(j==3) {dreh += 22.5; angle-=TMath::Pi()/8.;}   //sort out ambiguities 
      //TGeoRotation* rot = new TGeoRotation(rotname,dreh,-90,0);
      TGeoRotation* rot = new TGeoRotation(rotname,dreh,0,0);
      TGeoTranslation* trt = new TGeoTranslation(sin(angle)*move, cos(angle)*move, 1100+kPlaneZPosition[j]-kRotUmZ); 
      TGeoCombiTrans* trctmp = new TGeoCombiTrans(*trt,*rot);
      trctmp->SetName(name);
      trctmp->RegisterYourself();

      trc.Add( trctmp ); 
      vol.Add( tmpvol ); 
      delete rot; delete trt;
    }
  }

  TGeoVolumeAssembly* SubunitVol = new TGeoVolumeAssembly("Lumi_Strip_Disks");
  for(Int_t i=0; i<(8*4); i++)
    SubunitVol->AddNode((TGeoVolume*)vol.At(i),0,(TGeoCombiTrans*)trc.At(i));
  TGeoRotation* rotf = new TGeoRotation("final",90,kRot,0);
  TGeoTranslation* trtf = new TGeoTranslation(0, 0, kRotUmZ); 
  TGeoCombiTrans *trcf = new TGeoCombiTrans(*trtf,*rotf);
  top->AddNode(SubunitVol,0,trcf);

  gGeoMan->CloseGeometry();
  top->Write();
  fi->Close();
//   gGeoManager->Export(outfile);
  
  top->Draw("ogl");
}  
