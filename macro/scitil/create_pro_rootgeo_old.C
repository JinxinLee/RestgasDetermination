void create_pro_rootgeo_old(Double_t distance = 750){
  // Forward tof geometry parameters
  //-----------------------------
  //-- macro created by Alicia S. based on step file
  //-- barrel-SciTil_18122012


 //--------------------------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  
  // Load this libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libPassive");
  
  TString outfile= "../../geometry/BarrelScitil_Copy.root";
  TFile* fi = new TFile(outfile,"RECREATE");  
  
  FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  geoFace->setMediaFile("../../geometry/media_pnd.geo");
  geoFace->readMedia();
  geoFace->print();

  FairGeoMedia *Media =  geoFace->getMedia();
  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();

  FairGeoMedium *MediumSci  = Media->getMedium("polyvinyltoluene");
 
  const Double_t pi = 3.1415926535;
  Int_t nmed=geobuild->createMedium(MediumSci);
  /****/


  TGeoManager* gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");

  TGeoVolume *top = new TGeoVolumeAssembly("SciT");
                    
  gGeoMan->SetTopVolume(top);
  

TGeoBBox* TileShape;

TGeoVolume* TileVol;


Double_t bx[22],by[22],bz[22];
Double_t bvx[46],bvy[46],bvz[46];

TGeoVolumeAssembly* SciTVol = new TGeoVolumeAssembly("SciTil_strips");
 TGeoVolumeAssembly* QuadAssVol = new TGeoVolumeAssembly("SciTil_quad");// 4 tiles
 TGeoVolumeAssembly* SModAssVol = new TGeoVolumeAssembly("SciTil_mod");//90 quads


char name[13];
char namB[13];
char namV[13];
char name1[13];
char namB1[13];
char namV1[20];
char name2[13];
char namB2[13];
char namV2[13];

// creating Tile

 
  TileShape = new TGeoBBox("Scitil_logicTile", 1.425, 0.25, 1.425);
  
  TileVol = new TGeoVolume("SciTil",TileShape, gGeoMan->GetMedium("polyvinyltoluene"));
   TileVol->SetLineColor(kBlue);
  // creating Quad

  Double_t dx, dy, dz;

  QuadAssVol->AddNode(TileVol,0,new TGeoCombiTrans(1.525, 0.,1.525 , new TGeoRotation (0)) );
  QuadAssVol->AddNode(TileVol,1,new TGeoCombiTrans(1.525, 0.,-1.525, new TGeoRotation (0)) );
  QuadAssVol->AddNode(TileVol,2,new TGeoCombiTrans(-1.525, 0.,-1.525, new TGeoRotation (0)) );
  QuadAssVol->AddNode(TileVol,3,new TGeoCombiTrans(-1.525, 0.,1.525 , new TGeoRotation (0)) );

  //creating Module
  
  dy=0.;
  dx=0;
  dz=0.;
  Double_t Lx,Lz,g,d;
  d = 5.9;
  g = 0.1;
  Lx = 3*d +2*g;
  Lz = 30*d +29*g;

  for(int j=0;j<30;j++){
    for(int l=0;l<3;l++){
      dz = -0.5*Lz + (2*j + 1)*d*0.5 +j*g;
      dx = -0.5*Lx +(2*l + 1)*d*0.5 +l*g; 

      SModAssVol->AddNode(QuadAssVol,30*l+j,new TGeoCombiTrans(dx, 0.,dz, new TGeoRotation (0)));

  }
  }

  Double_t radius        =  50.55      ; // 50. radius in middle of the barbox (x and y)
  Double_t hthick        =  1.7/2.; // 1.7/2. half thickness of the bars
  Double_t barnum        =  6;       // 6 number of bars per barbox
  Double_t bbnum         =  16.;	  //16. total number of sides = barboxes
  Double_t bbGap         =  1.5;	  //1.5 gap btw the neighboring barboxes (at the middle height)
  Double_t pipehAngle    =  3.6;	  //3.6 [degrees] half of the angular space needed for the target pipe
   
  
  Double_t phi0 = (180.-2.*pipehAngle)/bbnum + pipehAngle;
  Double_t dphi = (180.-2.*pipehAngle)/bbnum*2.;  

//taking angles from dirc geometry 
	  Double_t dx_bbox, dy_bbox, dz_bbox, phi_curr;   
	     
	  for(Int_t m = 0; m < bbnum; m ++){       
	    phi_curr = (90. - phi0 - dphi*m)/180.*pi;   
	    if(m > bbnum/2-1){ phi_curr = (90. - phi0 - dphi*m - 2.*pipehAngle)/180.*pi; }
	    dx_bbox = radius * cos(phi_curr);
	    dy_bbox = radius * sin(phi_curr);
	    
	    TGeoRotation rot_bbox;   
	    rot_bbox.RotateZ( -phi0 - m*dphi - (TMath::Floor(2.*m/bbnum))*(2.*pipehAngle));   
	    SciTVol->AddNode(SModAssVol, m+1, new TGeoCombiTrans(dx_bbox, dy_bbox, 0, new TGeoRotation(rot_bbox)));   
	  }
	

	  top->AddNode(SciTVol,0,new TGeoCombiTrans(0.,0.,32.0,new TGeoRotation ()));
	  //top->AddNode(SModAssVol,0,new TGeoCombiTrans(0.,0.,32.0,new TGeoRotation ()));
	  gGeoMan->CloseGeometry();
	  top->Write();
	  fi->Close();
	  //gGeoManager->Export(outfile);
	  top->Draw("ogl");
  }  
  
