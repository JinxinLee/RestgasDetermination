void create_half_geo(Double_t distance = 750){
  // Forward tof geometry parameters
  //-----------------------------
  //-- macro created by Alicia S. based on step file
  //-- barrel-SciTil_18122012
  //-- edited by D. Steinschaden 
  //-- last update 08.2015


 //--------------------------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  
  // Load this libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libPassive");
  
  TString outfile= "half_geo.root";
  //TString outfile= "../../../geometry/SciTil_test.root";
  TFile* fi = new TFile(outfile,"RECREATE");  
  
  FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  geoFace->setMediaFile("../../../geometry/media_pnd.geo");
  geoFace->readMedia();
  geoFace->print();

  FairGeoMedia *Media =  geoFace->getMedia();
  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();

  FairGeoMedium *MediumSci  = Media->getMedium("polyvinyltoluene"); 
  FairGeoMedium *MediumSiPm = Media->getMedium("silicon"); 
  FairGeoMedium *MediumCard = Media->getMedium("pcbmvd");

  const Double_t pi = 3.1415926535;
  Int_t nmed=geobuild->createMedium(MediumSci); // ----why is the return value saved?
   nmed=geobuild->createMedium(MediumSiPm);
  nmed=geobuild->createMedium(MediumCard);

   TGeoManager* gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
   TGeoVolume *top = new TGeoVolumeAssembly("SciT");
   
                    
  gGeoMan->SetTopVolume(top);
  

//-------------------- creating Tile and SiPm and Readout Card --------------------

  //Volumes containing SENSOR in the name will be taken into account in the MC hit production
  //check PndSciT.h and PndSciT.cxx and the listofsensitives 

  TGeoBBox* TileShape;
  TileShape = new TGeoBBox("SciTil_logicTile", 4.3475, 0.25, 1.47);// dimension is always given from the mid to the end e.g. only the half length

  TGeoVolume* TileVol;
  TileVol = new TGeoVolume("SciTil_SENSOR",TileShape, gGeoMan->GetMedium("polyvinyltoluene"));
  TileVol->SetLineColor(kBlue);
  
  TGeoBBox* SiPmShape= new TGeoBBox("SiPm_logicSiPm",0.215,0.1925,0.0725);
   TGeoVolume* SiPmVol= new TGeoVolume("SiPm",SiPmShape, gGeoMan->GetMedium("silicon"));
  SiPmVol->SetLineColor(kRed);

  TGeoBBox* CardShape= new TGeoBBox("Card_logicCard",0.005,0.25,1.4);
   TGeoVolume* CardVol= new TGeoVolume("SciTil_Card",CardShape, gGeoMan->GetMedium("pcbmvd"));
   CardVol->SetLineColor(kYellow);

  TGeoBBox* CardOutShape= new TGeoBBox("CardOut_logicCard",0.5,0.075,90.);
   TGeoVolume* CardOutVol= new TGeoVolume("SciTil_CardOut",CardOutShape, gGeoMan->GetMedium("pcbmvd"));
   CardOutVol->SetLineColor(kGreen);
  

  TGeoSphere* CenterShape;
  CenterShape = new TGeoSphere("logicCenter", 0, 3);// dimension is always given from the mid to the end e.g. only the half length

  TGeoVolume* CenterVol;
  CenterVol = new TGeoVolume("Center",CenterShape, gGeoMan->GetMedium("polyvinyltoluene"));
  CenterVol->SetLineColor(kYellow);

  // --------------creating module (Line)-------------
  
  TGeoVolumeAssembly* LineAssVol = new TGeoVolumeAssembly("SciTil_Line");// 2 tiles + 16 SiPm + 3 connection cards

  TGeoRotation* rot_SiPm = new TGeoRotation;
 LineAssVol->AddNode(TileVol,0,new TGeoCombiTrans(4.4975, 0., 0., new TGeoRotation (0)));
 LineAssVol->AddNode(TileVol,1,new TGeoCombiTrans(-4.4975, 0., 0., new TGeoRotation (0))); 

 rot_SiPm->SetAngles(90.,90.,90.);
 LineAssVol->AddNode(SiPmVol,0,new TGeoCombiTrans(8.9175, 0., 1.06875, rot_SiPm));
 LineAssVol->AddNode(SiPmVol,1,new TGeoCombiTrans(8.9175, 0., 0.35625, rot_SiPm));
 LineAssVol->AddNode(SiPmVol,2,new TGeoCombiTrans(8.9175, 0., -0.35625, rot_SiPm));
 LineAssVol->AddNode(SiPmVol,3,new TGeoCombiTrans(8.9175, 0., -1.06875, rot_SiPm)); 

 LineAssVol->AddNode(SiPmVol,4,new TGeoCombiTrans(0.0775, 0., 1.06875, rot_SiPm));
 LineAssVol->AddNode(SiPmVol,5,new TGeoCombiTrans(0.0775, 0., 0.35625, rot_SiPm));
 LineAssVol->AddNode(SiPmVol,6,new TGeoCombiTrans(0.0775, 0., -0.35625, rot_SiPm));
 LineAssVol->AddNode(SiPmVol,7,new TGeoCombiTrans(0.0775, 0., -1.06875, rot_SiPm)); 
 
 LineAssVol->AddNode(SiPmVol,8,new TGeoCombiTrans(-0.0775, 0., 1.06875, rot_SiPm));
 LineAssVol->AddNode(SiPmVol,9,new TGeoCombiTrans(-0.0775, 0., 0.35625, rot_SiPm));
 LineAssVol->AddNode(SiPmVol,10,new TGeoCombiTrans(-0.0775, 0., -0.35625, rot_SiPm));
 LineAssVol->AddNode(SiPmVol,11,new TGeoCombiTrans(-0.0775, 0., -1.06875, rot_SiPm));

 LineAssVol->AddNode(SiPmVol,12,new TGeoCombiTrans(-8.9175, 0., 1.06875, rot_SiPm));
 LineAssVol->AddNode(SiPmVol,13,new TGeoCombiTrans(-8.9175, 0., 0.35625, rot_SiPm));
 LineAssVol->AddNode(SiPmVol,14,new TGeoCombiTrans(-8.9175, 0., -0.35625, rot_SiPm));
 LineAssVol->AddNode(SiPmVol,15,new TGeoCombiTrans(-8.9175, 0., -1.06875, rot_SiPm)); 

 LineAssVol->AddNode(CardVol,0,new TGeoCombiTrans(8.995, 0., 0., new TGeoRotation (0)));
 LineAssVol->AddNode(CardVol,1,new TGeoCombiTrans(0., 0., 0., new TGeoRotation (0)));
 LineAssVol->AddNode(CardVol,2,new TGeoCombiTrans(-8.995, 0., 0., new TGeoRotation (0)));

 // eines mit farbe:
  TGeoVolume* TileVol2;
  TileVol2 = new TGeoVolume("SciTil_SENSOR_2",TileShape, gGeoMan->GetMedium("polyvinyltoluene"));
  TileVol2->SetLineColor(kRed);


  TGeoVolumeAssembly* LineAssVol2 = new TGeoVolumeAssembly("SciTil_Line");// 2 tiles + 16 SiPm + 3 connection cards

 LineAssVol2->AddNode(TileVol2,0,new TGeoCombiTrans(4.4975, 0., 0., new TGeoRotation (0)));
 LineAssVol2->AddNode(TileVol2,1,new TGeoCombiTrans(-4.4975, 0., 0., new TGeoRotation (0))); 

 rot_SiPm->SetAngles(90.,90.,90.);
 LineAssVol2->AddNode(SiPmVol,0,new TGeoCombiTrans(8.9175, 0., 1.06875, rot_SiPm));
 LineAssVol2->AddNode(SiPmVol,1,new TGeoCombiTrans(8.9175, 0., 0.35625, rot_SiPm));
 LineAssVol2->AddNode(SiPmVol,2,new TGeoCombiTrans(8.9175, 0., -0.35625, rot_SiPm));
 LineAssVol2->AddNode(SiPmVol,3,new TGeoCombiTrans(8.9175, 0., -1.06875, rot_SiPm)); 

 LineAssVol2->AddNode(SiPmVol,4,new TGeoCombiTrans(0.0775, 0., 1.06875, rot_SiPm));
 LineAssVol2->AddNode(SiPmVol,5,new TGeoCombiTrans(0.0775, 0., 0.35625, rot_SiPm));
 LineAssVol2->AddNode(SiPmVol,6,new TGeoCombiTrans(0.0775, 0., -0.35625, rot_SiPm));
 LineAssVol2->AddNode(SiPmVol,7,new TGeoCombiTrans(0.0775, 0., -1.06875, rot_SiPm)); 
 
 LineAssVol2->AddNode(SiPmVol,8,new TGeoCombiTrans(-0.0775, 0., 1.06875, rot_SiPm));
 LineAssVol2->AddNode(SiPmVol,9,new TGeoCombiTrans(-0.0775, 0., 0.35625, rot_SiPm));
 LineAssVol2->AddNode(SiPmVol,10,new TGeoCombiTrans(-0.0775, 0., -0.35625, rot_SiPm));
 LineAssVol2->AddNode(SiPmVol,11,new TGeoCombiTrans(-0.0775, 0., -1.06875, rot_SiPm));

 LineAssVol2->AddNode(SiPmVol,12,new TGeoCombiTrans(-8.9175, 0., 1.06875, rot_SiPm));
 LineAssVol2->AddNode(SiPmVol,13,new TGeoCombiTrans(-8.9175, 0., 0.35625, rot_SiPm));
 LineAssVol2->AddNode(SiPmVol,14,new TGeoCombiTrans(-8.9175, 0., -0.35625, rot_SiPm));
 LineAssVol2->AddNode(SiPmVol,15,new TGeoCombiTrans(-8.9175, 0., -1.06875, rot_SiPm)); 

 LineAssVol2->AddNode(CardVol,0,new TGeoCombiTrans(8.995, 0., 0., new TGeoRotation (0)));
 LineAssVol2->AddNode(CardVol,1,new TGeoCombiTrans(0., 0., 0., new TGeoRotation (0)));
 LineAssVol2->AddNode(CardVol,2,new TGeoCombiTrans(-8.995, 0., 0., new TGeoRotation (0)));
  
  //-----------------creating Super-Module----------------

  TGeoVolumeAssembly* SModAssVol = new TGeoVolumeAssembly("SciTil_Mod");//60 Lines
 
  Double_t  dz=0;
  for(int j=0;j<40;j++){ 
      dz = -90+1.5+j*3;
      SModAssVol->AddNode(LineAssVol, j, new TGeoCombiTrans(0., 0., dz, new TGeoRotation (0)));  
  }
  for(int j=41;j<60;j++){ 
      dz = -90+1.5+j*3;
      SModAssVol->AddNode(LineAssVol, j, new TGeoCombiTrans(0., 0., dz, new TGeoRotation (0)));  
  }
  j=40;
  dz = -90+1.5+j*3;
  SModAssVol->AddNode(LineAssVol2, j, new TGeoCombiTrans(0., 0., dz, new TGeoRotation (0)));

  SModAssVol->AddNode(CardOutVol, 0, new TGeoCombiTrans(8.995, 0.325, 0., new TGeoRotation (0)));
  SModAssVol->AddNode(CardOutVol, 1, new TGeoCombiTrans(0., 0.325, 0., new TGeoRotation (0)));
  SModAssVol->AddNode(CardOutVol, 2, new TGeoCombiTrans(-8.995, 0.325, 0., new TGeoRotation (0)));


  // -----------------creating Barrel --------------------------------------------
  TGeoVolumeAssembly* BarrelAssVol = new TGeoVolumeAssembly("SciTil_barrel");
  
  //                                       ------------Values of Dirc Detector------------
  Double_t radius        =  50.55      ;  // 50. radius in middle of the barbox (x and y)
  Double_t hthick        =  1.7/2.;       // 1.7/2. half thickness of the bars
  Double_t barnum        =  6;            // 6 number of bars per barbox
  Int_t bbnum         =  16;	  //16. total number of sides = barboxes
  Double_t bbGap         =  1.5;	  //1.5 gap btw the neighboring barboxes (at the middle height)
  Double_t pipehAngle    =  3.6;	  //3.6 [degrees] half of the angular space needed for the target pipe
   
  
  Double_t phi0 = (180.-2.*pipehAngle)/bbnum + pipehAngle;
  Double_t dphi = (180.-2.*pipehAngle)/bbnum*2.;  

//taking angles from dirc geometry 
	  Double_t dx_bbox, dy_bbox, dz_bbox, phi_curr;   
	     
	  for(Int_t m = 0; m < 8; m ++){  //nur die hälfte wird erstellt
	    phi_curr = (90. - phi0 - dphi*m)/180.*pi;   
	    if(m > bbnum/2-1){ phi_curr = (90. - phi0 - dphi*m - 2.*pipehAngle)/180.*pi; }
	    dx_bbox = radius * cos(phi_curr);
	    dy_bbox = radius * sin(phi_curr);
	    
	    TGeoRotation rot_bbox;   
	    rot_bbox.RotateZ( -phi0 - m*dphi - (TMath::Floor(2.*m/bbnum))*(2.*pipehAngle));   
	    BarrelAssVol->AddNode(SModAssVol, m, new TGeoCombiTrans(dx_bbox, dy_bbox, 0, new TGeoRotation(rot_bbox)));   
	  }
  	
  
	  top->AddNode(BarrelAssVol,0, new TGeoCombiTrans(0.,0.,32.,new TGeoRotation()));
	  top->AddNode(CenterVol, 0);
	  //top->AddNode(SModAssVol,0, new TGeoCombiTrans(0.,0.,32.,new TGeoRotation()));
	  //top->AddNode(LineAssVol,0, new TGeoCombiTrans(0.,0.,32.,new TGeoRotation()));

  gGeoMan->SetVisLevel(5);
	  gGeoMan->CloseGeometry();
	  //top->Write();
	  fi->Close();
	  // gGeoManager->Export(outfile);
	  top->Draw("ogl");
	  
  }  
  
