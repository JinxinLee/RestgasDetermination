// macro that creates the Barrel DIRC geometry
// to be able to run this macro please be sure you have the following configuration in gconfig/g4Config.C file: 
// TG4RunConfiguration* runConfiguration 
//           = new TG4RunConfiguration("geomRoot", "QGSP_BERT_EMV+optical", "stepLimiter+specialCuts+specialControls");

// input parameters are:
// fFocusingSystem = 0 - no focusing is used
// fFocusingSystem = 1 - lens 
// fFocusingSystem = 2 - forward mirror is used
// fFocusingSystem = 3 - new lens (no air gap) is used
// fFocusingSystem = 4 - two component spherical lens is used
// fprizm = kFALSE - no prism
// fprizm = kTRUE - prism

// allowed configurations: 
// fFocusingSystem = 0 && fprizm = kFALSE  output file is in geometry/dirc_l0_p0.root
// fFocusingSystem = 0 && fprizm = kTRUE   output file is in geometry/dirc_l0_p1.root
// fFocusingSystem = 1 && fprizm = kFALSE  output file is in geometry/dirc_l1_p0.root
// fFocusingSystem = 2 && fprizm = kFALSE  output file is in geometry/dirc_l2_p0.root
// fFocusingSystem = 2 && fprizm = kTRUE   output file is in geometry/dirc_l2_p1.root
// corresponding .root files with geometry please fone in the geometry directory

// B E F O R E   R U N N I N G   T H E   P R O G R A M
// C H E C K   T H E   N A M E   O F   T H E   O U T P U T   F I L E ! ! ! 
const Double_t pi =  4.*atan(1.);

void getXrange(double dphi, double radiusCornerOut, double radiusCornerIn, double MCPactiveArea, int cas1, int cas2, double ylow, double step, double &x1, double &x2);
double getX(TVector3 corner1, TVector3 corner2, double yy);
int findSectorIn(double y, double dphi, double radius, double EVdrop, double hthick);
int findSectorOut(double y, double dphi_rad, double radiusCornerOut);

void createdirc(Int_t fFocusingSystem = 3, Bool_t fprizm = kFALSE){ 

  { // initialization 
    gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  
    TString vmcWorkdir = getenv("VMCWORKDIR");
  
    // Load this libraries
    gSystem->Load("libGeoBase");
    gSystem->Load("libParBase");
    gSystem->Load("libBase");
    gSystem->Load("libPndData");
    gSystem->Load("libPassive");
  
    // variable to achieve the DIRC basic parameters
    PndGeoDrc* fGeo = new PndGeoDrc(); 

    // units = cm  
   
    Double_t eps          	= 0.01;                   // epsilon
    Double_t mirr_hthick  	= 0.02;  
    Double_t PDbaseLayer  	= 5.;			  // [cm] thickness of the carbon at the back of the EV
  
    Double_t radius       	=  fGeo->radius();       // 50. radius in middle of the barbox (x and y)
    Double_t hthick       	=  fGeo->barHalfThick(); // 1.7/2. half thickness of the bars
    Double_t barwidth		=  fGeo->BarWidth();       // 3.2 width of the radiator bar
    Double_t barnum       	=  fGeo->barNum();       // 6 number of bars per barbox
    Double_t bbnum        	=  fGeo->BBoxNum();	  //16. total number of sides = barboxes
    Double_t bbGapAngle        	=  fGeo->BBoxGap();	  //1.5 gap btw the neighboring barboxes (at the middle height)
    Double_t pipehAngle   	=  fGeo->PipehAngle();	  //3.6 [degrees] half of the angular space needed for the target pipe
    Double_t barWin_hthick	=  0.1/2.;		  // [cm]=15um thickness of the 'glas' window at the readout end of the barbox;
    Double_t phi0 		= (180.-2.*pipehAngle)/bbnum + pipehAngle;
    Double_t dphi 		= (180.-2.*pipehAngle)/bbnum*2.;
    Double_t dphi_rad 		= dphi/180.*pi;
  
    Double_t bbox_zdown   	=  fGeo->barBoxZDown();  // 130. bar box z downstream
    Double_t bbox_zup     	=  fGeo->barBoxZUp();    // -120. bar box z upstream
    Double_t bbox_hlen    	=  0.5*(bbox_zdown - bbox_zup);           // 125. bar box half length
    Double_t bbox_shift   	=  bbox_zup + bbox_hlen; // 5. bar box shift
    Double_t barhgap       	=  fGeo->barhGap();       // 0.01 half gap between bars  
    Double_t boxgap       	=  fGeo->boxGap(); 	  // 0.1 gap between bars and the bar box
    Double_t boxthick		=  fGeo->boxThick();     // 0.05 thickness of the bar box
    Double_t gluehthick 		=  0.0005;//[cm]	  // glue layer, connects two halfs into one long radiator bar
    Double_t len          	=  0.;                   // length of the lenses block. see further
    Double_t fSlabEnd		=  0.;			  // [cm] position at which bar in front of EV ends 
    Double_t fdz_mirr1		=  0.;
    Double_t fdz_mirr2		=  0.;
    Double_t fdz_lens3		=  0.;  
    Double_t fdz_lens2		=  0.;
    Double_t fdz_lens1		=  0.; 
  
    //parameters for MCPs:
    Double_t MCPsize 		= 5.76; //[cm] width=height of an MCP
    Double_t MCPactiveArea	= 5.3; //[cm] width=height of the active area of an MCP
    Double_t MCPgap  		= 0.1; //[cm] gap between MCPs
    Double_t PixelSize 		= 0.65; //[cm] size of a pixel
    Int_t    Npix 		= 8; // number of mcps in a row/column
    const Int_t NpixTotal 	= Npix*Npix; // total number of pixels for 1 mcp
    Double_t PixelGap 		= (MCPactiveArea - Double_t(Npix)*PixelSize)/(Double_t(Npix)-1.);  
    Double_t PDwindowThick 	= 0.1; //[cm]
    Double_t PhCathodeThick 	= 0.0001; //[cm] = 1 um
    Double_t PDsensitiveThick 	= 0.01; //[cm]
    Double_t PDgreaseLayer 	= 0.1; //[cm]
    Double_t hgap 		= 0.5*(MCPsize - MCPactiveArea + MCPgap); // gap btw MCPs
    Double_t step 		= MCPactiveArea + 2.*hgap; // step in which to locate MCPs = 6 for Mcp1, Mcp2; = 5.88 for Mcp2a. This is total size of one MCP with gaps in between
  
    // EV parameters   
    bool radEv =  true;
    Double_t sob_len      	=  fGeo->EVlen();        // 30. in current version
    Double_t sob_shift    	=  -bbox_hlen + bbox_shift - sob_len; // -150.   
    Double_t sob_angleB		=  fGeo->EVbackAngle();  //90. [degrees] angle of the EV (usually it is 90)
    Double_t EVdrop		=  fGeo->EVdrop();	  //0.7 [cm] drop of the EV - inner radius
    Double_t EVoffset		=  fGeo->EVoffset();	  //0. [cm] offset of the EV - outer radius
    Double_t EVgreaseLayer	=  0.0015;		  //[cm] grease layer thickness btw the bar window and the EV 
    //Double_t sob_angle		=  45.;//fGeo->EVangle();	  //80. [degrees] opening angle of the EV 
    Double_t sob_angle		=  atan((5.*step-2.*hthick-EVoffset-EVdrop)/sob_len)/pi*180.;// [degrees] opening angle of the EV  
    Double_t sob_Rout     	=  (radius + hthick + EVoffset + sob_len*tan(sob_angle/180.*pi));//  [cm] radius in the middle of the section
    
    Double_t bbSideGap		=  0.5*( ((barwidth+2.*barhgap)*barnum) - (2.*radius*sin((dphi-bbGapAngle)/180.*pi/2.)+2.*barhgap) );
    Double_t bbX          	=  2.*radius*sin((dphi-bbGapAngle)/180.*pi/2.)+2.*barhgap;  

    Double_t radiusCornerIn	= (radius-hthick-EVdrop)/cos(dphi/2./180.*pi);
    Double_t radiusCornerOut	= sob_Rout/cos(dphi_rad/2.);///cos(dphi_rad/2.);
    Double_t radiusMiddleSmall	= radius-hthick-EVdrop;
  
    // cout<<"radius = "<<radius<<"rad corner in = "<< radiusCornerIn<<", rad corner out = "<< radiusCornerOut<<endl;     cout<<"pixel gap  = "<< PixelGap<<endl;  
    // cout<<"sob_angle = "<<sob_angle<<endl;
    // cout<<"sob_Rout = "<<sob_Rout<<endl; 
    // cout<<"bbAngle = "<<bbAngle<<", bbX = "<<bbX<<endl;    
    // cout<<"dphi = "<<dphi<<", phi0 = "<<phi0<<endl;  
    //----------------------------------------------------------------------
   
    // prizm:  
    Double_t phlength	 =  fGeo->PrismhLength(); //4.5 [cm] half length of the prizm
    Double_t pdrop	 =  fGeo->PrismDrop();	  //0.5 [cm] drop of the prizm - inner side
    Double_t pangle	 =  fGeo->PrismAngle();	  //30. [degrees] angle of the edge
    Double_t poffset	 =  fGeo->PrismOffset();  //1.[cm] prizm offset - outer side
    Double_t pheight	 =  2.*phlength * tan(pangle/180.*pi);  // [cm] half heigth of the prism
  			      
    Double_t sob_Rprizm    =  radius + hthick + poffset + pheight + EVoffset + (sob_len-2.*phlength)*tan(60./180.*pi);  
    //----------------------------------------------------------
      
    // Rotations:
    TGeoRotation rot1;
    rot1.RotateZ(90.);
    
    //TString fGeoFile= Form("../../geometry/dirc_l%d_p%d_mirrorGap_Mcp2a_GG_PC_correctedEVsensor.root",fFocusingSystem, fprizm);
    TString fGeoFile= Form("../../geometry/dirc_l%d_p%d_R2.root",fFocusingSystem, fprizm);
    TFile* fi = new TFile(fGeoFile,"RECREATE");
    cout<<"Output file = "<<fGeoFile<<endl;
    
    Double_t par[10];   
    Double_t aa;   
    Double_t z, density, radl, absl, w; 
    Int_t nel, numed, nz;

    FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
    FairGeoInterface *geoFace = geoLoad->getGeoInterface();
    geoFace->setMediaFile("../../geometry/media_pnd.geo");
    geoFace->readMedia();

    FairGeoMedia *Media =  geoFace->getMedia();
    FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();

    FairGeoMedium *DrcAir       	= Media->getMedium("air");
    FairGeoMedium *DrcAirNoSens   = Media->getMedium("DIRCairNoSens");
    FairGeoMedium *DrcEpotek301_2 = Media->getMedium("Epotek301_2");
    FairGeoMedium *DrcOpticalGrease    = Media->getMedium("OpticalGrease");
    FairGeoMedium *DrcCarbonFiber = Media->getMedium("DIRCcarbonFiber");
    FairGeoMedium *DrcFusedSil    = Media->getMedium("FusedSil");
    FairGeoMedium *DrcMirror      = Media->getMedium("Mirror");
    FairGeoMedium *DrcMarcol82_7  = Media->getMedium("Marcol82_7");
    FairGeoMedium *DrcNLAK33A     = Media->getMedium("NLAK33A");
    FairGeoMedium *DrcPhotocathode= Media->getMedium("Photocathode");  
  
    Int_t nmed=geobuild->createMedium(DrcAir);
    nmed=geobuild->createMedium(DrcAirNoSens);
    nmed=geobuild->createMedium(DrcEpotek301_2);
    nmed=geobuild->createMedium(DrcOpticalGrease);
    nmed=geobuild->createMedium(DrcCarbonFiber);
    nmed=geobuild->createMedium(DrcFusedSil);
    nmed=geobuild->createMedium(DrcMirror);
    nmed=geobuild->createMedium(DrcMarcol82_7);
    nmed=geobuild->createMedium(DrcNLAK33A);
    nmed=geobuild->createMedium(DrcPhotocathode);

  }
     
  { // focusing systems 
    if(fFocusingSystem == 0){  // no focusing
      Double_t flen = 0.;
      fSlabEnd = -bbox_hlen + bbox_shift + flen;  
      cout<<"bar ends at = "<<fSlabEnd<<endl;
    }
  
    if(fFocusingSystem == 1){  // L E N S E S  (airgap, thin nlak33)
      // some notes to lens operations (revision 9649) is in
      // ~carsten/work/documents/software/PandaRoot/lens_definitions_2.pdf
    
      Double_t r = 75.18; // first lens radius (cm)    
      Double_t alpha = TMath::ASin(barwidth/2./r);
      Double_t a = r - r*TMath::Cos(alpha);
      Double_t b = a + 0.5; // box dimension  .6 instead of .5 due to strong curvature

      Double_t r2 = 75.18; // radius second lens (cm)
      Double_t b2 = 0.2;   // + a2;
     
      Double_t r3 = 17.95; // third lens radius (cm)
      Double_t alpha3 = TMath::ASin(barwidth/2./r3); 
      Double_t a3 = r3 - r3*TMath::Cos(alpha3);
      Double_t b3 = a3 + 0.0; // box dimension  .6 instead of .5 due to strong curvature
   
      len = b + 0.2 + b3 + 0.5;//+ a2; // dimension of the box containing both lenses
      fSlabEnd = -bbox_hlen + bbox_shift + len; // used in processHits
 
      // Lens 1
      Double_t t = -r +b/2;

      TGeoSphere* logicSphere= new TGeoSphere("S",0.,r, 0. ,180.,0.,360.); 
      TGeoBBox* lBox = new TGeoBBox("B", barwidth/2., hthick, b/2.);
      TGeoTranslation *tr1 = new TGeoTranslation("tr1", 0.,0., t);
      tr1->RegisterYourself();
      TGeoCompositeShape *cs = new TGeoCompositeShape("cs","S*(B:tr1)");
      TGeoVolume *lens1 = new TGeoVolume("DrcLENS1Sensor",cs, gGeoManager->GetMedium("FusedSil"));
      lens1->SetLineColor(kRed-8);
      lens1->SetTransparency(40); 
     
      // position lens within already shifted bar container at -(bbox_hlen-eps)+len, the lens base is -r + b
      // with -0.01 one can make a gap visible (.1mm) for orientation   
      //barContainer->(lens1, 1,new TGeoCombiTrans(0., 0., -(bbox_hlen-eps)+len -(-r+b) /*-0.01*/, new TGeoRotation (0)));
      /*    barContainer->AddNode(lens1, 1,new TGeoCombiTrans(0., 0., -(bbox_hlen)+len -(-r+b), new TGeoRotation (0)));
       */  fdz_lens1 = -(bbox_hlen)+len -(-r+b);
     
       
      //Lens 2
      Double_t t2 = -r2;// +b2/2 r2  is the reference point (concave lens) 
      TGeoSphere* logicSphere2 = new TGeoSphere("S2",0 ,r2, 0. ,180.,0.,360.);
      TGeoBBox*   lBox2        = new TGeoBBox("B2", barwidth/2., hthick, b2/2.);
      TGeoTranslation *tr2     = new TGeoTranslation("tr2", 0.,0., t2);
      tr2->RegisterYourself();
      TGeoCompositeShape *cs2 = new TGeoCompositeShape("cs2","(B2:tr2)-S2");
      TGeoVolume *lens2 = new TGeoVolume("DrcLENS2Sensor",cs2, gGeoManager->GetMedium("NLAK33A"));
      lens2->SetLineColor(kRed+2);
      lens2->SetTransparency(40); 

      // place the lens exactly on lens1
      // position lens within already shifted bar container at -(bbox_hlen-eps)+len, the lens base is -r2
      // the tip of lens1 is at b
      // with -0.02 one can make a gap visible (.1mm due to lens1) for orientation   
      //barContainer->AddNode(lens2, 1,new TGeoCombiTrans(0., 0., -(bbox_hlen-eps)+len -(-r2) -b /*-0.02*/, new TGeoRotation (0)));
      /*    barContainer->AddNode(lens2, 1,new TGeoCombiTrans(0., 0., -(bbox_hlen)+len -(-r2) -b , new TGeoRotation (0)));
       */  fdz_lens2 = -(bbox_hlen)+len -(-r2) -b;

      //Lens3 (like lens1, same treatment)
      Double_t t3 = -r3+b3/2;
      TGeoSphere* logicSphere3= new TGeoSphere("S3",0.,r3, 0. ,180.,0.,360.);
      TGeoBBox* lBox3 = new TGeoBBox("B3", barwidth/2., hthick, b3/2.);
      TGeoTranslation *tr3 = new TGeoTranslation("tr3", 0.,0., t3);
      tr3->RegisterYourself();
      TGeoCompositeShape *cs3 = new TGeoCompositeShape("cs3","S3*(B3:tr3)");
      TGeoVolume *lens3 = new TGeoVolume("DrcLENS3Sensor",cs3, gGeoManager->GetMedium("NLAK33A"));
      lens3->SetLineColor(kRed-6);
      lens3->SetTransparency(40); 
     
      // place the lens exactly on lens2 plane side
      // position lens within already shifted bar container at -(bbox_hlen-eps)+len, the lens base is -r3 + b3
      // with -0.03 one can make a gap visible (.1mm due to lens 1&2) for orientation   
      // b2/2 is the thickness of lens2 in the middle 
      //barContainer->AddNode(lens3, 1,new TGeoCombiTrans(0., 0., -(bbox_hlen-eps)+len -(-r3+b3) -b - b2/2 /*-0.03*/, new TGeoRotation (0)));
      /*    barContainer->AddNode(lens3, 1,new TGeoCombiTrans(0., 0., -(bbox_hlen)+len -(-r3+b3) -b - b2/2 , new TGeoRotation (0)));
      //cout<<" DIRC r,r2,r3 = "<<r<<" "<<r2<<" "<<r3<<endl;    
      */  fdz_lens3 = -(bbox_hlen)+len -(-r3+b3) -b - b2/2. ;  
    } 

    if(fFocusingSystem == 2){  // Mirrors at front
      // Put some mirrors at the downstream end with a focal plane at the PD.
      
      double zpos=120.;
     
      // The bar is produced with "bbox_hlen-fabs(len)/2.-mirr_hthick"
      len = -130. + zpos + 1.;
      //len = -247.0; // negative to make space at downstream end of bar.
     
      Double_t len1 = 1.5; // 1st block
     
      Double_t mirror_angle  = 0.0;// 0.0 is pointing upstream, -90.0 is pointing to the beam axis
      Double_t focal_length  = 2. * bbox_hlen + sob_len - fabs(len) + len1;
      Double_t mirror_radius = 2. * focal_length;
 
      cout<<" mirror radius: "<<mirror_radius<<endl;
     

      // no angle for the moment
      // block
      TGeoSphere* logicSphere = new TGeoSphere("S",0.,mirror_radius, 0. ,180.,0.,360.);
      TGeoBBox*   lBox        = new TGeoBBox("B", barwidth/2., hthick, fabs(len1)/2.);
      
      Double_t t = mirror_radius - len1/2.;

      TGeoTranslation *tr1 = new TGeoTranslation("tr1", 0.,0., t);
      tr1->RegisterYourself();
      TGeoCompositeShape *cs = new TGeoCompositeShape("cs","S*(B:tr1)");
 
      TGeoVolume *block1 = new TGeoVolume("DrcBlock1",cs, gGeoManager->GetMedium("FusedSil"));
      block1->SetLineColor(kRed);
      block1->SetTransparency(40);
      

      Double_t shift1 = len1-mirror_radius; // Now the start of block1 is at zero
      shift1         += bbox_hlen-fabs(len)-2.*mirr_hthick;
      
      /*     
	     barContainer->AddNode(block1, 1, new TGeoCombiTrans(0., 0., shift1, new TGeoRotation (0)));
      */   fdz_mirr1 =  shift1;   


      Double_t gap  = 0;
      Double_t len2 = fabs(len) - len1 - gap;
   
      // no angle for the moment
      // block
      TGeoSphere* logicSphere2 = new TGeoSphere("S2",0,mirror_radius,             0. ,180.,0.,360.);
      TGeoBBox*   lBox2        = new TGeoBBox("B2", barwidth/2., hthick, fabs(len2)/2.); 

      //  make radius part of block
      Double_t t2 = mirror_radius + len2/2 - 1;

      TGeoTranslation *tr2 = new TGeoTranslation("tr2", 0.,0., t2);
      tr2->RegisterYourself();
      TGeoCompositeShape *cs2 = new TGeoCompositeShape("cs2","(B2:tr2)-S2");

      TGeoVolume *block2 = new TGeoVolume("DrcBlock2",cs2, gGeoManager->GetMedium("Mirror"));
      block2->SetLineColor(kGreen);
      block2->SetTransparency(40);
      

      Double_t shift2  = -mirror_radius; // Now the start of the block is at zero
      shift2          += bbox_hlen-fabs(len)-2*mirr_hthick; // at end of bar
      shift2          += len1 + gap;
     
      fdz_mirr2 = shift2; 
      
      /*       
	       barContainer->AddNode(block2, 
	       1,
	       new TGeoCombiTrans(0., 
	       0.,
	       shift2,
	       //  bbox_hlen-mirror_radius-2*mirr_hthick-len2+len1+1,
	       //bbox_hlen-mirror_radius-len2+0.1,
	       new TGeoRotation (0)
	       )
	       );
      */			    
     

      Double_t flen = 0.;
      fSlabEnd = -bbox_hlen + bbox_shift + flen;  
      cout<<"bar ends at = "<<fSlabEnd<<endl;
      
      //fAtBarEnd = "DrcBar";    
    }
   
    if(fFocusingSystem == 3){ // cylindrical lens w/o airgap
     
      // main parameters:
      Double_t Hcyl2 = 0.1; //[cm] thickness in the middle of the second (defocusing) (FusedSil) lens           
      Double_t Rcyl = 7.35; // [cm]
      Double_t Lcyl = barwidth/2.; // Double_t Lcyl = (bbX/barnum)/2.-bargap;  //cylinder length/2.
      Double_t Acyl = TMath::ASin(hthick/Rcyl);  // angle
      Double_t Tcyl1 = 0.5*Rcyl*(1. + TMath::Cos(Acyl)); // distance between centers of the cylinder and the box
     
      len = Hcyl2 + Rcyl*(1. - TMath::Cos(Acyl));     
     
      //Lens1 
      TGeoEltu* lCylinder = new TGeoEltu("Cyl",Rcyl,Rcyl,Lcyl);
      TGeoBBox* lCylBox = new TGeoBBox("CylBox", hthick, len/2., Lcyl);
      TGeoTranslation *trCyl1 = new TGeoTranslation("trCyl", 0., Rcyl*TMath::Cos(Acyl)+len/2., 0.);// !!!
      trCyl1->RegisterYourself();
      TGeoCompositeShape *llens1 = new TGeoCompositeShape("llens1","Cyl*(CylBox:trCyl)");
      TGeoVolume *CylLens1 = new TGeoVolume("DrcLENS1Sensor",llens1, gGeoManager->GetMedium("NLAK33A"));
      CylLens1->SetLineColor(kRed-8);
      CylLens1->SetTransparency(40);
      // CylLens1->SetLineColor(kCyan+1);
      // CylLens1->SetTransparency(60);

      fdz_lens1 = -(bbox_hlen - barWin_hthick) + len/2. - (-Rcyl + Rcyl*(1.-TMath::Cos(Acyl)));
     
      //Lens2          
      TGeoTranslation *trCyl2 = new TGeoTranslation("trCyl2", 0., Rcyl + Hcyl2 - len/2., 0.);
      trCyl2->RegisterYourself();
      TGeoCompositeShape *llens2 = new TGeoCompositeShape("llens2", "(CylBox:trCyl2) - Cyl");
      TGeoVolume* CylLens2 = new TGeoVolume("DrcLENS2Sensor", llens2, gGeoManager->GetMedium("FusedSil"));
      CylLens2->SetLineColor(kRed+2);
      CylLens2->SetTransparency(40);
      // CylLens2->SetLineColor(kCyan+1);
      // CylLens2->SetTransparency(60);

      fdz_lens2 = -(bbox_hlen - barWin_hthick) + len/2. + (hthick - Rcyl*(1.-TMath::Cos(Acyl)) - Hcyl2);
      fSlabEnd = -bbox_hlen + bbox_shift;
      cout<<"bar ends at = "<<fSlabEnd<<endl;
     
    }

    if(fFocusingSystem == 4){ // combined spherical lense
     
      Double_t hlens1 = 0.3; //[cm] thickness in the middle of the first (focising) (FusedSil) lens      
      Double_t hlens2 = 0.1; //[cm] thickness in the middle of the second (defocusing) (FusedSil) lens           
      Double_t rlens = 7.35; // [cm]
      Double_t llens =  barwidth/2.; //(bbX/barnum)/2.-bargap;
      len =  hlens1+hlens2;     
     
      //lens1 
      TGeoSphere* lSphUp = new TGeoSphere("SphUp",0 ,rlens, 0. ,180.,0.,360.);
      TGeoBBox* lCylBox = new TGeoBBox("LensBox", hthick, len, llens);
      TGeoTranslation *trCyl1 = new TGeoTranslation("trCyl", 0., rlens+hlens2, 0.);
      trCyl1->RegisterYourself();
      TGeoCompositeShape *cslens1 = new TGeoCompositeShape("cslens1","SphUp*(LensBox:trCyl)");
      TGeoVolume *CylLens1 = new TGeoVolume("DrcLENS1Sensor",cslens1, gGeoManager->GetMedium("NLAK33A"));
      CylLens1->SetLineColor(kRed+2);
      CylLens1->SetTransparency(40);
      // CylLens1->SetLineColor(kCyan+1);
      // CylLens1->SetTransparency(60);
     
      fdz_lens1 = -bbox_hlen + 3*barWin_hthick - len/2. + rlens;
     
      //lens2          
      TGeoTranslation *trCyl2 = new TGeoTranslation("trCyl2", 0., rlens-hlens1, 0.);
      trCyl2->RegisterYourself();
      TGeoCompositeShape *cslens2 = new TGeoCompositeShape("cslens2", "(LensBox:trCyl2) - SphUp");
      TGeoVolume* CylLens2 = new TGeoVolume("DrcLENS2Sensor", cslens2, gGeoManager->GetMedium("FusedSil"));
      CylLens2->SetLineColor(kRed+2);
      CylLens2->SetTransparency(40);
      // CylLens2->SetLineColor(kCyan+-3);
      // CylLens2->SetTransparency(60);

      fdz_lens2 = fdz_lens1 + hlens2;
      fSlabEnd = -bbox_hlen + bbox_shift;
     
    }
  }
 
  // {

  //   TGeoVolume *top = gGeoManager->MakeBox("DIRC",gGeoManager->GetMedium("air"),100,100,100);
  //   top->AddNode(CylLens1, 1, new TGeoCombiTrans(0, 0, 0, new TGeoRotation(0)));
  //   top->AddNode(CylLens2, 2, new TGeoCombiTrans(0, 0, 0, new TGeoRotation(0))); 
    
  //   //gGeoManager->SetTopVolume(top);
  //   //gGeoManager->CloseGeometry();
  //   gGeoManager->SetNsegments(1000);
  //   //top->CheckOverlaps(0.0001, "");
  //   //gGeoManager->CheckOverlaps(0.00001,"");
  //   //top->Write();
  //   //fi->Close(); 
  //   top->Draw("ogl");
  //   return;
  // }
 
  // create top volume:
  TGeoManager* gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  gGeoManager->SetNsegments(10000);

  TGeoBBox*   lTop = new TGeoBBox(500,500,300);
  TGeoVolume* top = new TGeoVolume("DIRC", lTop, gGeoManager->GetMedium("air"));
  gGeoManager->SetTopVolume(top);
  
  // create pre-top volume:
  TGeoVolume* vLocalMother;
  TGeoPcon*   shape = new TGeoPcon("BarrelDIRCShape", 0, 360., 4);
  shape->DefineSection(0, bbox_zdown, 35., 60.);
  shape->DefineSection(1, bbox_zup, 35., 60.);
  if(radEv){
    shape->DefineSection(2, bbox_zup - sob_len +20., 35., sob_Rout+poffset+pheight+EVoffset+1.);
    shape->DefineSection(3, bbox_zup - sob_len - PDbaseLayer - EVgreaseLayer, 35., sob_Rout+poffset+pheight+EVoffset+1.);
  }else{
    shape->DefineSection(2, bbox_zup - sob_len, 35., sob_Rout+poffset+pheight+EVoffset+1.);
    shape->DefineSection(3, bbox_zup - sob_len - PDbaseLayer - EVgreaseLayer, 35., sob_Rout+poffset+pheight+EVoffset+1.);
  }
  vLocalMother = new TGeoVolume("BarrelDIRC", shape, gGeoManager->GetMedium("DIRCairNoSens"));
  top->AddNode(vLocalMother, 0,0);

  // create BarBoxes:  
  TGeoBBox* logicbbL;
  TGeoVolume *bbox;
  logicbbL = new TGeoBBox("logicbbL", 0.5*barnum*(barwidth+2.*barhgap)+bbSideGap+boxthick, hthick+boxgap+boxthick, bbox_hlen);
  bbox = new TGeoVolume("DrcBarBox", logicbbL,gGeoManager->GetMedium("DIRCcarbonFiber")); 
  bbox->SetLineColor(30); 
  
  TGeoBBox* logicbbS;
  TGeoVolume *abox;
  if(fFocusingSystem == 0 || fFocusingSystem == 1 || fFocusingSystem == 2){
    logicbbS = new TGeoBBox("logicbbS", 0.5*barnum*(barwidth+2.*barhgap)+bbSideGap, hthick+boxgap, bbox_hlen-barWin_hthick);
  }
  if(fFocusingSystem == 3 || fFocusingSystem == 4){
    logicbbS = new TGeoBBox("logicbbS", 0.5*barnum*(barwidth+2.*barhgap)+bbSideGap, hthick+boxgap, bbox_hlen-barWin_hthick);
  }
  abox = new TGeoVolume("DrcAirBox", logicbbS, gGeoManager->GetMedium("DIRCairNoSens"));
  if(fFocusingSystem == 0 || fFocusingSystem == 1 || fFocusingSystem == 2){ 
    bbox->AddNode(abox, 1, new TGeoCombiTrans(0., 0., barWin_hthick, new TGeoRotation(0)));
  }
  if(fFocusingSystem == 3 || fFocusingSystem == 4){
    bbox->AddNode(abox, 1, new TGeoCombiTrans(0., 0., barWin_hthick, new TGeoRotation(0)));
  }
  abox->SetLineColor(19); // gray
  
  //create windows at the readout end of the bar boxes:
  TGeoBBox* logicBarWin = new  TGeoBBox("logicBarWin", 0.5*barnum*(barwidth+2.*barhgap)+bbSideGap, hthick+boxgap, barWin_hthick);
  TGeoVolume* barwin = new TGeoVolume("DrcBarboxWindowSensor", logicBarWin, gGeoManager->GetMedium("FusedSil"));
  barwin->SetLineColor(kBlue-4);
  bbox->AddNode(barwin, 1, new TGeoCombiTrans(0.,0.,-bbox_hlen+barWin_hthick,new TGeoRotation(0)));
 
  //create layers of grease at the readout end of the bar boxes, between the  windows and the EV: 
  TGeoBBox* logicEVgrease = new  TGeoBBox("logicEVgrease", 0.5*barnum*(barwidth+2.*barhgap)+bbSideGap, hthick+boxgap, EVgreaseLayer/2.);
  TGeoVolume* evgrease = new TGeoVolume("DrcEVgrease", logicEVgrease, gGeoManager->GetMedium("OpticalGrease"));  
  evgrease->SetLineColor(kSpring);
  
  // put barboxes into right positions:    
  Double_t dx_bbox, dy_bbox, dz_bbox, phi_curr;    
  for(Int_t m = 0; m < bbnum; m ++){
    phi_curr = (90. - phi0 - dphi*m)/180.*pi;    
    if(m > bbnum/2-1){ phi_curr = (90. - phi0 - dphi*m - 2.*pipehAngle)/180.*pi; }
    dx_bbox = radius * cos(phi_curr);
    dy_bbox = radius * sin(phi_curr);
    dz_bbox = bbox_shift;
         
    TGeoRotation rot_bbox;    
    rot_bbox.RotateZ( -phi0 - m*dphi - (TMath::Floor(2.*m/bbnum))*(2.*pipehAngle));    
    vLocalMother->AddNode(bbox, m+1, new TGeoCombiTrans(dx_bbox, dy_bbox, dz_bbox, new TGeoRotation(rot_bbox)));
    vLocalMother->AddNode(evgrease, m+1, new TGeoCombiTrans(dx_bbox, dy_bbox, bbox_zup-EVgreaseLayer/2.,new TGeoRotation(rot_bbox)));    
  }
  
  // create logic bar:  
  TGeoBBox* logicBar;
  if(fFocusingSystem == 0 || fFocusingSystem == 1 || fFocusingSystem == 2){
    logicBar = new TGeoBBox("logicBar", barwidth/2., hthick, bbox_hlen-fabs(len)/2.-mirr_hthick-barWin_hthick); 
  }
  if(fFocusingSystem == 3 || fFocusingSystem == 4){
    logicBar = new TGeoBBox("logicBar", barwidth/2., hthick, bbox_hlen-fabs(len)/2.-mirr_hthick-barWin_hthick); 
  }
  
  TGeoVolume* bar;
  bar = new TGeoVolume("DrcBarSensor",logicBar, gGeoManager->GetMedium("FusedSil"));
  bar->SetLineColor(kCyan-9);
  bar->SetTransparency(60);
  
  // create logic mirror: 
  TGeoBBox* logicMirror  = new TGeoBBox("logicMirror", barwidth/2., hthick, mirr_hthick /2.);
  TGeoVolume *mirr  = new TGeoVolume("DrcMirr", logicMirror,  gGeoManager->GetMedium("Mirror"));
  mirr->SetLineColor(5);
  
  // create glue layer inside the bar (connects two halves):
  TGeoBBox* logicBarGlue  = new TGeoBBox("logicBarGlue", barwidth/2., hthick, gluehthick);
  TGeoVolume *barglue  = new TGeoVolume("DrcBarGlueSensor", logicBarGlue,  gGeoManager->GetMedium("Epotek301_2"));
  barglue->SetLineColor(kSpring-5);
  bar->AddNode(barglue, 1, new TGeoCombiTrans(0., 0., 0., new TGeoRotation (0)));
  
  Double_t dx, dy, dz_bar, dz_mirr;
  
  for(Int_t j=0; j<barnum; j++){ 
    dx      = - (bbX/2.) - bbSideGap + (barwidth+2.*barhgap)/2. + j * (barwidth+2.*barhgap);
    dy  = 0.;//len/2. - mirr_hthick;    
    if(fFocusingSystem == 0 || fFocusingSystem == 1 || fFocusingSystem == 2){
      dz_mirr = bbox_hlen - barWin_hthick - mirr_hthick;
      dz_bar = len/2.-mirr_hthick;
    }
    if(fFocusingSystem == 3 || fFocusingSystem == 4){
      dz_mirr = bbox_hlen - barWin_hthick - mirr_hthick;
      dz_bar = -mirr_hthick +len/2.;
    }
    if(fFocusingSystem == 1){ // lens
      abox->AddNode(lens1,  1+j, new TGeoCombiTrans(dx, dy, fdz_lens1, new TGeoRotation (0)));
      abox->AddNode(lens2,  1+j, new TGeoCombiTrans(dx, dy, fdz_lens2, new TGeoRotation (0)));
      abox->AddNode(lens3,  1+j, new TGeoCombiTrans(dx, dy, fdz_lens3, new TGeoRotation (0)));
    }
    if(fFocusingSystem == 2){ // mirror
      abox->AddNode(block1, 1+j, new TGeoCombiTrans(dx, dy, fdz_mirr1, new TGeoRotation (0)));
      abox->AddNode(block2, 1+j, new TGeoCombiTrans(dx, dy, fdz_mirr2, new TGeoRotation (0)));
    }
    if(fFocusingSystem == 3 || fFocusingSystem == 4){
      TGeoRotation rot_lens3;
      rot_lens3.RotateZ(-90.);
      rot_lens3.RotateY( 90.);               
      abox->AddNode(CylLens1, 1+j, new TGeoCombiTrans(dx, dy, fdz_lens1+len/2., new TGeoRotation(rot_lens3)));
      abox->AddNode(CylLens2, 1+j, new TGeoCombiTrans(dx, dy, fdz_lens1+len/2., new TGeoRotation(rot_lens3)));
    }        
    abox->AddNode(bar,  1+j, new TGeoCombiTrans(dx, dy, dz_bar, new TGeoRotation(0)));
    if(fFocusingSystem != 2){ // not a forward mirror
      abox->AddNode(mirr, 1+j, new TGeoCombiTrans(dx, dy, dz_mirr, new TGeoRotation(0)));
    }
    //abox->Draw("ogl"); errbreak;
  }
    
  { // Expansion volume:

    TGeoPgon* logicEV1, * logicEV2, *logicEV3, * logicEV4; 
    TGeoPgon* logicEV1s, * logicEV2s, *logicEV3s, * logicEV4s;
    TGeoPgon* logicEV1b, * logicEV2b, *logicEV3b, * logicEV4b;

    Double_t cosFactor1 = cos(pipehAngle/180.*pi)/cos(dphi/180.*pi/2.);  
    std::cout.precision(20);
    
    Double_t dR = (radius+hthick+boxgap+boxthick)/cos(dphi/2./180.*pi) - (radius-hthick);
    Double_t xEV = (dR + sob_len*tan(sob_angle/180.*pi))/ (tan(sob_angle/180.*pi) + tan(sob_angleB/180.*pi)); 
    Double_t maxrz;

    Double_t 
      minrad = radiusMiddleSmall,
      viscorrection =  0.000001, //fix visualization
      mcptot = MCPsize + MCPgap,
      alpharad = TMath::ATan(mcptot/sob_len),
      steprad, stepz, currz = 0.;    
    if(radEv){
      logicEV1 = new TGeoPgon("logicEV1",  90 + pipehAngle, 180 - 2.*pipehAngle, bbnum/2, 7);
      logicEV2 = new TGeoPgon("logicEV2", -90 + pipehAngle, 180 - 2.*pipehAngle, bbnum/2, 7);
      logicEV3 = new TGeoPgon("logicEV3",  90 - pipehAngle-0.02, 2*pipehAngle+0.04, 1, 7); //fix visualization
      logicEV4 = new TGeoPgon("logicEV4", -90 - pipehAngle-0.02, 2*pipehAngle+0.04, 1, 7);

      for(int i=0; i<6; i++){
	steprad = mcptot*TMath::Cos((i+1)*alpharad); 
	stepz = mcptot*TMath::Sin((i+1)*alpharad);	
	if(i!=0) viscorrection = 0;
	logicEV1->DefineSection(i, currz ,radiusMiddleSmall, minrad+viscorrection);
	logicEV2->DefineSection(i, currz ,radiusMiddleSmall, minrad+viscorrection);
	logicEV3->DefineSection(i, currz ,radiusMiddleSmall*cosFactor1, (minrad+viscorrection)*cosFactor1);
	logicEV4->DefineSection(i, currz ,radiusMiddleSmall*cosFactor1, (minrad+viscorrection)*cosFactor1);
	minrad += steprad;
	currz += stepz;
	if(i==4) maxrz = currz;

      }

      logicEV1->DefineSection(6, sob_len, radiusMiddleSmall,  (radius+hthick+boxgap+boxthick+EVoffset)/cos(dphi/2./180.*pi));
      logicEV2->DefineSection(6, sob_len, radiusMiddleSmall,  (radius+hthick+boxgap+boxthick+EVoffset)/cos(dphi/2./180.*pi));    
      logicEV3->DefineSection(6, sob_len, radiusMiddleSmall*cosFactor1,  (radius+hthick+boxgap+boxthick+EVoffset)/cos(dphi/2./180.*pi)*cosFactor1);   
      logicEV4->DefineSection(6, sob_len, radiusMiddleSmall*cosFactor1,  (radius+hthick+boxgap+boxthick+EVoffset)/cos(dphi/2./180.*pi)*cosFactor1);


      //small volume to constract EVcover
      minrad = radiusMiddleSmall;
      currz = 0.;    

      logicEV1s = new TGeoPgon("logicEV1s",  90 + pipehAngle, 180 - 2.*pipehAngle, bbnum/2, 7);
      logicEV2s = new TGeoPgon("logicEV2s", -90 + pipehAngle, 180 - 2.*pipehAngle, bbnum/2, 7);
      logicEV3s = new TGeoPgon("logicEV3s",  90 - pipehAngle-0.02, 2*pipehAngle+0.04, 1, 7); //fix visualization
      logicEV4s = new TGeoPgon("logicEV4s", -90 - pipehAngle-0.02, 2*pipehAngle+0.04, 1, 7);
      
      Double_t airgap = 0.05, airdz=airgap/tan(pi/2.-alpharad);
      for(int i=0; i<6; i++){
	steprad = mcptot*TMath::Cos((i+1)*alpharad); 
	stepz = mcptot*TMath::Sin((i+1)*alpharad);	
	if(i!=0) {viscorrection = 0; airgap = 0; airdz=0;}
	logicEV1s->DefineSection(i, currz-airdz ,radiusMiddleSmall-airgap, minrad+viscorrection-airgap);
	logicEV2s->DefineSection(i, currz-airdz ,radiusMiddleSmall-airgap, minrad+viscorrection-airgap);
	logicEV3s->DefineSection(i, currz-airdz ,(radiusMiddleSmall-airgap)*cosFactor1, (minrad+viscorrection-airgap)*cosFactor1);
	logicEV4s->DefineSection(i, currz-airdz ,(radiusMiddleSmall-airgap)*cosFactor1, (minrad+viscorrection-airgap)*cosFactor1);
	minrad += steprad;
	currz += stepz;
	if(i==4) maxrz = currz;

      }
      airgap = 0.05;
      logicEV1s->DefineSection(6, sob_len, radiusMiddleSmall-airgap,  (radius+hthick+boxgap+boxthick+EVoffset+airgap)/cos(dphi/2./180.*pi));
      logicEV2s->DefineSection(6, sob_len, radiusMiddleSmall-airgap,  (radius+hthick+boxgap+boxthick+EVoffset+airgap)/cos(dphi/2./180.*pi));    
      logicEV3s->DefineSection(6, sob_len, (radiusMiddleSmall-airgap)*cosFactor1,  (radius+hthick+boxgap+boxthick+EVoffset+airgap)/cos(dphi/2./180.*pi)*cosFactor1);   
      logicEV4s->DefineSection(6, sob_len, (radiusMiddleSmall-airgap)*cosFactor1,  (radius+hthick+boxgap+boxthick+EVoffset+airgap)/cos(dphi/2./180.*pi)*cosFactor1);

      //big volume to constract EVcover
      minrad = radiusMiddleSmall; 
      mcptot = MCPsize + MCPgap;
      alpharad = TMath::ATan(mcptot/sob_len);
      currz = 0.;    
      
      logicEV1b = new TGeoPgon("logicEV1b",  90 + pipehAngle, 180 - 2.*pipehAngle, bbnum/2, 8);
      logicEV2b = new TGeoPgon("logicEV2b", -90 + pipehAngle, 180 - 2.*pipehAngle, bbnum/2, 8);
      logicEV3b = new TGeoPgon("logicEV3b",  90 - pipehAngle-0.02, 2*pipehAngle+0.04, 1, 8);
      logicEV4b = new TGeoPgon("logicEV4b", -90 - pipehAngle-0.02, 2*pipehAngle+0.04, 1, 8);

      Double_t tga, 
	mcpcoverthick = 1,
	coverthick = 0.1;
      for(int i=0; i<6; i++){
	steprad = mcptot*TMath::Cos((i+1)*alpharad); 
	stepz = mcptot*TMath::Sin((i+1)*alpharad);	

	logicEV1b->DefineSection(i, currz-mcpcoverthick ,radiusMiddleSmall-coverthick, minrad+mcpcoverthick);
	logicEV2b->DefineSection(i, currz-mcpcoverthick ,radiusMiddleSmall-coverthick, minrad+mcpcoverthick);
	logicEV3b->DefineSection(i, currz-mcpcoverthick ,(radiusMiddleSmall-coverthick)*cosFactor1, (minrad+mcpcoverthick)*cosFactor1);
	logicEV4b->DefineSection(i, currz-mcpcoverthick ,(radiusMiddleSmall-coverthick)*cosFactor1, (minrad+mcpcoverthick)*cosFactor1);
	if(i==5) {
	  tga = (sob_len - currz)/(minrad - (radius+hthick+boxgap+boxthick+EVoffset)/cos(dphi/2./180.*pi));
	  double tshift = mcpcoverthick*tga - coverthick;
	  logicEV1b->DefineSection(6, currz-tshift ,radiusMiddleSmall-coverthick, minrad+mcpcoverthick);
	  logicEV2b->DefineSection(6, currz-tshift ,radiusMiddleSmall-coverthick, minrad+mcpcoverthick);
	  logicEV3b->DefineSection(6, currz-tshift ,(radiusMiddleSmall-coverthick)*cosFactor1, (minrad+mcpcoverthick)*cosFactor1);
	  logicEV4b->DefineSection(6, currz-tshift ,(radiusMiddleSmall-coverthick)*cosFactor1, (minrad+mcpcoverthick)*cosFactor1);
	} 
	minrad += steprad;
	currz += stepz;
	if(i==4) maxrz = currz;

      }

      logicEV1b->DefineSection(7, sob_len-0.01, radiusMiddleSmall-coverthick,  (radius+hthick+boxgap+boxthick+EVoffset)/cos(dphi/2./180.*pi)+coverthick/tga);
      logicEV2b->DefineSection(7, sob_len-0.01, radiusMiddleSmall-coverthick,  (radius+hthick+boxgap+boxthick+EVoffset)/cos(dphi/2./180.*pi)+coverthick/tga);    
      logicEV3b->DefineSection(7, sob_len-0.01, (radiusMiddleSmall-coverthick)*cosFactor1, ((radius+hthick+boxgap+boxthick+EVoffset)/cos(dphi/2./180.*pi)+coverthick/tga)*cosFactor1);
      logicEV4b->DefineSection(7, sob_len-0.01, (radiusMiddleSmall-coverthick)*cosFactor1, ((radius+hthick+boxgap+boxthick+EVoffset)/cos(dphi/2./180.*pi)+coverthick/tga)*cosFactor1);


    }else{
   
      if(sob_angleB == 90.){
	logicEV1 = new TGeoPgon("logicEV1", 93.6, 172.8, bbnum/2, 2);
	logicEV1->DefineSection(0, 0.,      radiusMiddleSmall,  sob_Rout);
	logicEV1->DefineSection(1, sob_len, radiusMiddleSmall,  (radius+hthick+boxgap+boxthick+EVoffset)/cos(dphi/2./180.*pi));
	logicEV2 = new TGeoPgon("logicEV2", -86.4, 172.8, bbnum/2, 2);
	logicEV2->DefineSection(0, 0.,      radiusMiddleSmall,  sob_Rout);
	logicEV2->DefineSection(1, sob_len, radiusMiddleSmall,  (radius+hthick+boxgap+boxthick+EVoffset)/cos(dphi/2./180.*pi));     
	logicEV3 = new TGeoPgon("logicEV3", 86.4, 7.2, 1, 2);
	logicEV3->DefineSection(0, 0.,      (radiusMiddleSmall)*cosFactor1,  sob_Rout*cosFactor1);
	logicEV3->DefineSection(1, sob_len, (radiusMiddleSmall)*cosFactor1,  (radius+hthick+boxgap+boxthick+EVoffset)/cos(dphi/2./180.*pi)*cosFactor1);     
	logicEV4 = new TGeoPgon("logicEV4", -93.6, 7.2, 1, 2);
	logicEV4->DefineSection(0, 0.,      (radiusMiddleSmall)*cosFactor1,  sob_Rout*cosFactor1);
	logicEV4->DefineSection(1, sob_len, (radiusMiddleSmall)*cosFactor1,  (radius+hthick+boxgap+boxthick+EVoffset)/cos(dphi/2./180.*pi)*cosFactor1);     
      
      }
      if(sob_angleB != 90.){  
	logicEV1 = new TGeoPgon("logicEV1", 93.6, 172.8, bbnum/2, 3);
	logicEV1->DefineSection(0, 0.,      radius-hthick,  radius-hthick+eps);
	logicEV1->DefineSection(1, xEV,     radius-hthick,  sob_Rout - xEV*tan(sob_angle/180.*pi));
	logicEV1->DefineSection(2, sob_len, radius-hthick,  (radius+hthick+boxgap+boxthick)/cos(dphi/2./180.*pi));
	logicEV2 = new TGeoPgon("logicEV2", -86.4, 172.8, bbnum/2, 3);
	logicEV2->DefineSection(0, 0.,      radius-hthick,  radius-hthick+eps);
	logicEV2->DefineSection(1, xEV,     radius-hthick,  sob_Rout - xEV*tan(sob_angle/180.*pi));
	logicEV2->DefineSection(2, sob_len, radius-hthick,  (radius+hthick+boxgap+boxthick)/cos(dphi/2./180.*pi));
	logicEV3 = new TGeoPgon("logicEV3", 86.4, 7.2, 1, 3);
	logicEV3->DefineSection(0, 0.,      (radius-hthick)*cosFactor1,  (radius-hthick+eps)*cosFactor1);
	logicEV3->DefineSection(1, xEV,     (radius-hthick)*cosFactor1,  (sob_Rout - xEV*tan(sob_angle/180.*pi))*cosFactor1);
	logicEV3->DefineSection(2, sob_len, (radius-hthick)*cosFactor1,  (radius+hthick+boxgap+boxthick)/cos(dphi/2./180.*pi)*cosFactor1);
	logicEV4 = new TGeoPgon("logicEV4", -93.6, 7.2, 1, 3);
	logicEV4->DefineSection(0, 0.,      (radius-hthick)*cosFactor1,  (radius-hthick+eps)*cosFactor1);
	logicEV4->DefineSection(1, xEV,     (radius-hthick)*cosFactor1,  (sob_Rout - xEV*tan(sob_angle/180.*pi))*cosFactor1);
	logicEV4->DefineSection(2, sob_len, (radius-hthick)*cosFactor1,  (radius+hthick+boxgap+boxthick)/cos(dphi/2./180.*pi)*cosFactor1);           
      }
    }
    
    TGeoCompositeShape *logicEV = new TGeoCompositeShape("logicEV","logicEV1 + logicEV3 + logicEV2 + logicEV4"); // 
    TGeoCompositeShape *logicEVs = new TGeoCompositeShape("logicEVs","logicEV1s + logicEV3s + logicEV2s + logicEV4s"); // 
    TGeoCompositeShape *logicEVb = new TGeoCompositeShape("logicEVb","logicEV1b + logicEV3b + logicEV2b + logicEV4b"); // 
    TGeoVolume* baseEV = new TGeoVolume("DrcEVSensor", logicEV, gGeoManager->GetMedium("FusedSil"));//Marcol82_7_m);
    baseEV->SetLineColor(kMagenta+2);
    baseEV->SetTransparency(50);
    vLocalMother->AddNode(baseEV, 1, new TGeoCombiTrans(0.,0.,sob_shift - EVgreaseLayer, new TGeoRotation(0)));
  }

  // PhotoDetector Basic material - Carbon. It is placed on the back side of the EV to simulate the support structure of the MCPs, the photons are to hit it in gaps between MCPs:
  TGeoVolume *pdbase;
  if(radEv){
    TGeoCompositeShape *logicEVcover = new TGeoCompositeShape("logicEVcover","logicEVb - logicEVs");
    pdbase = new TGeoVolume("DrcPDbase", logicEVcover, gGeoManager->GetMedium("DIRCcarbonFiber")); //DrcEVcover
    pdbase->SetLineColor(kTeal-8);

 // {

 //    TGeoVolume *top = gGeoManager->MakeBox("DIRC",gGeoManager->GetMedium("air"),100,100,100);
 //    top->AddNode(pdbase, 1, new TGeoCombiTrans(0, 0, 0, new TGeoRotation(0)));

 //    gGeoManager->SetNsegments(1000);
 //    top->Draw("ogl");
 //    return;
 // }

    vLocalMother->AddNode(pdbase, 1, new TGeoCombiTrans(0., 0., sob_shift-EVgreaseLayer, new TGeoRotation(0)));
  }else{
    TGeoPgon *logicPDbase1, *logicPDbase2, *logicPDbase3,  *logicPDbase4;
    Double_t rad_delta = (MCPsize-MCPactiveArea)/2./cos(45./180.*pi);
    logicPDbase1 = new TGeoPgon("logicPDbase1",  93.6, 172.8, bbnum/2, 2);
    logicPDbase2 = new TGeoPgon("logicPDbase2", -86.4, 172.8, bbnum/2, 2);
    logicPDbase3 = new TGeoPgon("logicPDbase3",  86.4,   7.2,       1, 2);
    logicPDbase4 = new TGeoPgon("logicPDbase4", -93.6,   7.2,       1, 2);
    if(sob_angleB == 90.){       
      logicPDbase1->DefineSection(0, 0.,  radiusMiddleSmall-rad_delta, sob_Rout);
      logicPDbase1->DefineSection(1, PDbaseLayer,  radiusMiddleSmall-rad_delta, sob_Rout);           
      logicPDbase2->DefineSection(0, 0.,  radiusMiddleSmall-rad_delta, sob_Rout);
      logicPDbase2->DefineSection(1, PDbaseLayer,  radiusMiddleSmall-rad_delta, sob_Rout);      
      logicPDbase3->DefineSection(0, 0., (radiusMiddleSmall)*cosFactor1-rad_delta, sob_Rout*cosFactor1);
      logicPDbase3->DefineSection(1, PDbaseLayer, (radiusMiddleSmall)*cosFactor1-rad_delta, sob_Rout*cosFactor1);
      logicPDbase4->DefineSection(0, 0., (radiusMiddleSmall)*cosFactor1-rad_delta, sob_Rout*cosFactor1);
      logicPDbase4->DefineSection(1, PDbaseLayer, (radiusMiddleSmall)*cosFactor1-rad_delta, sob_Rout*cosFactor1);
    }

    TGeoCompositeShape *logicPDbase = new TGeoCompositeShape("logicPDbase","logicPDbase1  + logicPDbase2"); //+ logicPDbase3 + logicPDbase4
    pdbase = new TGeoVolume("DrcPDbase", logicPDbase, gGeoManager->GetMedium("DIRCcarbonFiber"));
    pdbase->SetLineColor(kGreen-6);
    if(sob_angleB == 90.){
      vLocalMother->AddNode(pdbase, 1, new TGeoCombiTrans(0., 0., sob_shift-EVgreaseLayer-PDbaseLayer, new TGeoRotation(0)));
    }
  }
  pdbase->SetTransparency(50);
  


  //version 2: place MCPs in local bar coordinate systems:
  //create a map of MCPs for one sector (in bar c.s.):  
  Double_t sectorWidth = 0.;    
  Int_t nmcp = 0;
  Int_t totalnumbering = 1;
  TVector3 location;
  Double_t phi_curr1 = 0.;  
  for(Int_t m = 0; m < bbnum; m ++){       
    phi_curr1 = (90. - phi0 - dphi*m)/180.*pi;    
    if(m > bbnum/2-1){ phi_curr1 = (90. - phi0 - dphi*m - 2.*pipehAngle)/180.*pi; }
  
    stepz = mcptot*TMath::Sin(alpharad)/2.;
    steprad =  mcptot*(1-TMath::Cos(alpharad))/2.;

    // placement of MCPs in one sector
    for(Int_t nrow=0; nrow < Int_t((sob_Rout-radiusMiddleSmall)/step); nrow++){
      
      TGeoRotation rot_sector;    
      rot_sector.RotateX((nrow+1)*alpharad*180./pi);
      rot_sector.RotateZ( -phi0 - m*dphi - (TMath::Floor(2.*m/bbnum))*(2.*pipehAngle));

      sectorWidth = 2.* (radiusMiddleSmall + step*nrow) * tan(dphi_rad/2.);
      nmcp = Int_t(sectorWidth/step);
      xpos = (radiusMiddleSmall + 0.5*mcptot + step*(nrow));
      for(Int_t ny=0; ny<nmcp; ny++){


	//create pixel plates: one for each MCP:
	TGeoBBox* logicPD = new TGeoBBox("logicPD", MCPactiveArea/2., MCPactiveArea/2., PDsensitiveThick/2.); 
	TGeoVolume *pixelholder  = new TGeoVolume("DrcPDSensor", logicPD,  gGeoManager->GetMedium("FusedSil"));
	pixelholder->SetLineColor(kGreen+1);
  
	// create photo cathodes for each MCP
	TGeoBBox* logicPhCathode = new TGeoBBox("logicPhCathode", MCPactiveArea/2., MCPactiveArea/2., PhCathodeThick/2.); 
	TGeoVolume *phcathode  = new TGeoVolume("DrcPhCathodeSensor", logicPhCathode,  gGeoManager->GetMedium("Photocathode"));//("FusedSil"));
	phcathode->SetLineColor(kGray+1);
  
	// create Windows for each MCP
	TGeoBBox* logicWindow = new TGeoBBox("logicWindow", MCPsize/2., MCPsize/2., PDwindowThick/2.); 
	TGeoVolume *window  = new TGeoVolume("DrcPDwindowSensor", logicWindow,  gGeoManager->GetMedium("FusedSil"));
	window->SetLineColor(kBlue-4);
  
	// create grease layers between MCP window and the EV back side
	TGeoBBox* logicMCPgrease = new TGeoBBox("logicMCPgrease", MCPsize/2., MCPsize/2., PDgreaseLayer/2.); 
	TGeoVolume *mcpgrease  = new TGeoVolume("DrcMcpGreaseSensor", logicMCPgrease, gGeoManager->GetMedium("OpticalGrease"));
	mcpgrease->SetLineColor(kSpring);


	TGeoBBox *logicMCP = new TGeoBBox("logicMCP", MCPsize/2.+MCPgap/2., MCPsize/2.+MCPgap/2., (PDsensitiveThick+PhCathodeThick+PDwindowThick+PDgreaseLayer)/2.);
	TGeoVolume *oneMCP = new TGeoVolume("DrcMCP", logicMCP, gGeoManager->GetMedium("DIRCcarbonFiber"));
	oneMCP->SetLineColor(kBlue);

	oneMCP->AddNode(mcpgrease, totalnumbering, new TGeoCombiTrans(0, 0, (PDwindowThick+PhCathodeThick+PDsensitiveThick)/2., new TGeoRotation(0)));
	oneMCP->AddNode(window, totalnumbering, new TGeoCombiTrans(0, 0, (PhCathodeThick+PDsensitiveThick-PDgreaseLayer)/2., new TGeoRotation(0)));
	oneMCP->AddNode(phcathode, totalnumbering, new TGeoCombiTrans(0, 0, (PDsensitiveThick-PDgreaseLayer-PDwindowThick)/2., new TGeoRotation(0))); 
	oneMCP->AddNode(pixelholder, totalnumbering, new TGeoCombiTrans(0, 0, (-PDgreaseLayer-PDwindowThick-PhCathodeThick)/2., new TGeoRotation(0)));


	ypos = ((-Int_t(nmcp/2.) - 0.5*(nmcp%2))*step + (0.5+ny)*step);
	location.SetXYZ(xpos-steprad + (logicMCP->GetDZ())*(TMath::Cos(pi/2. - (nrow+1)*alpharad)) ,ypos,0.);
        location.RotateZ(phi_curr1);

        pdbase->AddNode(oneMCP, totalnumbering, new TGeoCombiTrans(location.X(), location.Y(),stepz - (logicMCP->GetDZ())*(TMath::Sin(pi/2. - (nrow+1)*alpharad))
								   , new TGeoRotation(rot_sector)));
	totalnumbering = totalnumbering + 1; 	
      }
      
      stepz += mcptot*TMath::Sin((nrow+1)*alpharad)/2. 	+ mcptot*TMath::Sin((nrow+2)*alpharad)/2.;
      steprad += mcptot*(1-TMath::Cos((nrow+1)*alpharad))/2. + mcptot*(1-TMath::Cos((nrow+2)*alpharad))/2.;

    }
  }
  //return;
  // for(Int_t nrow=0; nrow < Int_t((sob_Rout-radiusMiddleSmall)/step); nrow++){
  //   for(Int_t nadd = 0; nadd<2; nadd++){
  //     xpos = (radiusMiddleSmall*cosFactor1 + 0.5*MCPactiveArea + step*(nrow));
  //     location.SetXYZ(xpos,0.,0.);
  //     location.RotateZ(pi/2.+pi*nadd);
  //     pdbase->AddNode(mcpgrease, totalnumbering, new TGeoCombiTrans(location.X(), location.Y(), PDbaseLayer-PDgreaseLayer/2., new TGeoRotation(0)));
  //     pdbase->AddNode(window, totalnumbering, new TGeoCombiTrans(location.X(), location.Y(), PDbaseLayer-PDgreaseLayer-PDwindowThick/2., new TGeoRotation(0)));
  //     pdbase->AddNode(phcathode, totalnumbering, new TGeoCombiTrans(location.X(), location.Y(), PDbaseLayer-PDgreaseLayer-PhCathodeThick/2.-PDwindowThick, new TGeoRotation(0))); 
  //     pdbase->AddNode(pixelholder, totalnumbering, new TGeoCombiTrans(location.X(), location.Y(), PDbaseLayer-PDgreaseLayer-PhCathodeThick-PDwindowThick-PDsensitiveThick/2., new TGeoRotation(0)));    
  //     totalnumbering = totalnumbering + 1;
  //   }
  // }

    //   gGeoManager->SetTopVisible(); 		
  // pdbase->Draw("ogl"); return;

  gGeoManager->CloseGeometry();
  top->CheckOverlaps(0.0001, "");
  gGeoManager->CheckOverlaps(0.00001,""); // [cm]
  //gGeoManager->CheckGeometryFull();
  gGeoManager->SetVisLevel(4); 	
      
  top->Write();
  fi->Close(); 
  top->Draw("ogl");
  
  TObjArray *listOfOverlaps = gGeoManager->GetListOfOverlaps();
  cout<<listOfOverlaps->GetEntries()<<endl;
  listOfOverlaps->Print(); 
}

int findSectorIn(double y, double dphi_rad, double radius, double EVdrop, double hthick){  
  TVector3 EVcornerIn;
  EVcornerIn.SetXYZ((radius-EVdrop-hthick)/cos(dphi_rad/2.),0.,0.);
  Double_t Y0[4];
  for(Int_t i=0; i<4; i++){
    EVcornerIn.RotateZ(dphi_rad);
    Y0[i] = EVcornerIn.Y();
  }
  if(y <= Y0[0]){return 1;}
  if(y > Y0[0] && y <= Y0[1]){return 2;}
  if(y > Y0[1] && y <= Y0[2]){return 3;}
  if(y > Y0[2] && y <= Y0[3]){return 4;}
  if(y > Y0[3]){return 5;}
   
}

int findSectorOut(double y, double dphi_rad, double radiusCornerOut){  
  TVector3 EVcorner;
  EVcorner.SetXYZ(radiusCornerOut,0.,0.);  
  Double_t Y0[4];
  for(Int_t i=0; i<4; i++){
    EVcorner.RotateZ(dphi_rad);
    Y0[i] = EVcorner.Y();
    cout<<"Y ["<<i<<"] = "<<Y0[i]<<endl;
  }
  if(y <= Y0[0]){return 1;}
  if(y > Y0[0] && y <= Y0[1]){return 2;}
  if(y > Y0[1] && y <= Y0[2]){return 3;}
  if(y > Y0[2] && y <= Y0[3]){return 4;}
  if(y > Y0[3]){return 5;}
   
}

bool getXrange(double dphi, double radiusCornerOut, double radiusCornerIn, double MCPactiveArea, int cas1, int cas2, double ylow, double step, double &x1, double &x2){
    
  Double_t dphi_rad = dphi/180.*pi;
  
  TVector3 EVcorner;
  EVcorner.SetXYZ(radiusCornerOut,0.,0.);    
  TVector3 EVcorner1 = EVcorner;
  TVector3 EVcorner2 = EVcorner;
  TVector3 EVcornerIn;
  EVcornerIn.SetXYZ(radiusCornerIn,0.,0.);
  TVector3 EVcornerIn1 = EVcornerIn;
  TVector3 EVcornerIn2 = EVcornerIn;
  //cout<<"-I- sob_Rout = "<<sob_Rout<<", dphi = "<<dphi<<", |EVcorner| = "<<EVcorner.Mag()<<", |EVcornerIn| = "<<EVcornerIn.Mag()<<endl;   
    
  Double_t xstart, xfinish;
  if(cas1 == 1){ // 0 - 21.6 degrees        
    EVcornerIn2.RotateZ(dphi_rad);
    xstart = getX(EVcornerIn1, EVcornerIn2, ylow) + MCPactiveArea/2.;        
  }
  if(cas1 == 2){    // 21.6 - 43.2 degrees    
    EVcornerIn1.RotateZ(1.*dphi_rad);
    EVcornerIn2.RotateZ(2.*dphi_rad);
    xstart = getX(EVcornerIn1, EVcornerIn2, ylow) + MCPactiveArea/2.;    
  }
  if(cas1 == 3){ // 43.2 - 63.8 degrees    
    EVcornerIn1.RotateZ(2.*dphi_rad);
    EVcornerIn2.RotateZ(3.*dphi_rad);
    xstart = getX(EVcornerIn1, EVcornerIn2, ylow) + MCPactiveArea/2.;    
  }
  if(cas1 == 4){ // 63.8 - 85.6 degrees    
    EVcornerIn1.RotateZ(3.*dphi_rad);
    EVcornerIn2.RotateZ(4.*dphi_rad);
    xstart = getX(EVcornerIn1, EVcornerIn2, ylow) + MCPactiveArea/2.;    
  }
  if(cas1 == 5){ // 85.6 - 90.    
    EVcornerIn1.RotateZ(4.*dphi_rad);
    EVcornerIn2.RotateZ(pi/2.);
    xstart = getX(EVcornerIn1, EVcornerIn2, ylow) + MCPactiveArea/2.;    
  } 
  
  if(cas2 == 1){ // 0 - 21.6 degrees   
    EVcorner2.RotateZ(dphi_rad);   
    xfinish = getX(EVcorner1, EVcorner2, ylow+step) - MCPactiveArea/2.;        
  }
  if(cas2 == 2){    // 21.6 - 43.2 degrees
    EVcorner1.RotateZ(1.*dphi_rad);
    EVcorner2.RotateZ(2.*dphi_rad);    
    xfinish = getX(EVcorner1, EVcorner2, ylow+step) - MCPactiveArea/2.;
  }
  if(cas2 == 3){ // 43.2 - 63.8 degrees
    EVcorner1.RotateZ(2.*dphi_rad);
    EVcorner2.RotateZ(3.*dphi_rad);    
    xfinish = getX(EVcorner1, EVcorner2, ylow+step) - MCPactiveArea/2.;
  }
  if(cas2 == 4){ // 63.8 - 85.6 degrees
    EVcorner1.RotateZ(3.*dphi_rad);
    EVcorner2.RotateZ(4.*dphi_rad);    
    xfinish = getX(EVcorner1, EVcorner2, ylow+step) - MCPactiveArea/2.;
  }
  if(cas2 == 5){ // 85.6 - 90.
    EVcorner1.RotateZ(4.*dphi_rad);
    EVcorner2.RotateZ(pi/2.);    
    xfinish = getX(EVcorner1, EVcorner2, ylow+step) - MCPactiveArea/2.;
  } 
  if(xstart <= 0.){xstart = step/2.;}
  if(xstart < step/2.){xstart = step/2.;}
  if(xfinish <= 0.){xfinish = step/2.;}
    x1 = xstart;
    x2 = xfinish;
  
  cout<<"-I- xstart = "<<xstart<<", xfinish = "<<xfinish<<endl;
  if(x1 != x2)return true;
  else return false;
}

double getX(TVector3 corner1, TVector3 corner2, double yy){
  Double_t b = ( corner1.X()*corner2.Y() - corner1.Y()*corner2.X() ) / ( corner1.X() - corner2.X() );
  Double_t k = ( corner1.Y() - corner2.Y() ) / ( corner1.X() - corner2.X() );
  if(( yy - b )/k >= 0.)  return ( yy - b )/k;
  if(( yy - b )/k < 0.) return 0.;
}
