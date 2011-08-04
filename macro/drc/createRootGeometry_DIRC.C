// macro that creates the Barrel DIRC geometry
// to be able to run this macro please be sure you have the following configuration in gconfig/g4Config.C file: 
// TG4RunConfiguration* runConfiguration 
//           = new TG4RunConfiguration("geomRoot", "QGSP_BERT_EMV+optical", "stepLimiter+specialCuts+specialControls");

// input parameters are:
// fFocusingSystem = 0 - no focusing is used
// fFocusingSystem = 1 - lens 
// fFocusingSystem = 2 - forward mirror is used
// fprizm = kFALSE - no prism
// fprizm = kTRUE - prism

// allowed configurations: 
// fFocusingSystem = 0 && fprizm = kFALSE  output file is in geometry/dirc_l0_p0.root
// fFocusingSystem = 0 && fprizm = kTRUE   output file is in geometry/dirc_l0_p1.root
// fFocusingSystem = 1 && fprizm = kFALSE  output file is in geometry/dirc_l1_p0.root
// fFocusingSystem = 2 && fprizm = kFALSE  output file is in geometry/dirc_l2_p0.root
// fFocusingSystem = 2 && fprizm = kTRUE   output file is in geometry/dirc_l2_p1.root
// corresponding .root files with geometry please fone in the geometry directory

void createRootGeometry_DIRC(Int_t fFocusingSystem = 0, Bool_t fprizm = kFALSE){ 
  
  const Double_t pi = 3.1415926535;

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
  
   
  Double_t eps           = 0.01;                                  // epsilon
  Double_t mirr_hthick   = 0.01;
  Double_t PDthick       = 0.1;
  
  Double_t radius        =  fGeo->radius();       // 50. radius in middle of the barbox (x and y)
  Double_t hthick        =  fGeo->barHalfThick(); // 1.7/2. half thickness of the bars
  Double_t barnum        =  fGeo->barNum();       // 6 number of bars per barbox
  Double_t bbnum         =  fGeo->BBoxNum();	  //16. total number of sides = barboxes
  Double_t bbGap         =  fGeo->BBoxGap();	  //1.5 gap btw the neighboring barboxes (at the middle height)
  Double_t pipehAngle    =  fGeo->PipehAngle();	  //3.6 [degrees] half of the angular space needed for the target pipe
  
  Double_t bbox_zdown    =  fGeo->barBoxZDown();  // 130. bar box z downstream
  Double_t bbox_zup      =  fGeo->barBoxZUp();    // -120. bar box z upstream
  Double_t bbox_hlen     =  0.5*(bbox_zdown - bbox_zup);           // 125. bar box half length
  Double_t bbox_shift    =  bbox_zup + bbox_hlen; // 5. bar box shift
  Double_t bargap        =  fGeo->barGap();       // 0.01 half gap between bars  
  Double_t boxgap        =  fGeo->boxGap(); 	  // 0.1 gap between bars and the bar box
  Double_t boxthick	 =  fGeo->boxThick();     // 0.05 thickness of the bar box
  Double_t len           =  0.;                   // length of the lenses block. see further
  Double_t fSlabEnd	 =  0.;			  // [cm] position at which bar in front of EV ends 
  Double_t fdz_mirr1	 =  0.;
  Double_t fdz_mirr2	 =  0.;
  Double_t fdz_lens3	 =  0.;  
  Double_t fdz_lens2	 =  0.;
  Double_t fdz_lens1	 =  0.;    
    
  Double_t sob_len       =  fGeo->EVlen();        // 30. in current version
  Double_t sob_shift     =  -bbox_hlen + bbox_shift - sob_len; // -150. 
  Double_t sob_Rout      =  radius + hthick + sob_len*tan(60./180.*pi)/cos(pi/bbnum/2.);
  Double_t sob_angleB	 =  fGeo->EVbackAngle();  //90. [degrees] angle of the EV (usually it is 90)
  Double_t sob_angle	 =  fGeo->EVangle();	  //60. [degrees] opening angle of the EV
   
  Double_t bbAngle       =  ( 180. - 2.*pipehAngle - bbGap/radius/pi*180.*(bbnum/2.-1.) )/(bbnum/2.);  // ~20 degrees
  Double_t bbX           =  radius*bbAngle/180.*pi;  // 17.45 cm
  cout<<"bbAngle = "<<bbAngle<<", bbX = "<<bbX<<endl;
  
  
  Double_t phi0 = (180.-2.*pipehAngle)/bbnum + pipehAngle;
  Double_t dphi = (180.-2.*pipehAngle)/bbnum*2.;  
  //----------------------------------------------------------------------
  
  // parameters for the prizm!!!
  Double_t EVdrop	 =  fGeo->EVdrop();	  //0.5 [cm] drop of the EV - inner radius
  Double_t EVoffset	 =  fGeo->EVoffset();	  //1. [cm] offset of the EV - outer radius
    
  // prizm:  
  Double_t phlength	 =  fGeo->PrismhLength(); //4.5 [cm] half length of the prizm
  Double_t pdrop	 =  fGeo->PrismDrop();	  //0.5 [cm] drop of the prizm - inner side
  Double_t pangle	 =  fGeo->PrismAngle();	  //30. [degrees] angle of the edge
  Double_t poffset	 =  fGeo->PrismOffset();  //1.[cm] prizm offset - outer side
  Double_t pheight	 =  2.*phlength * tan(pangle/180.*pi);  // [cm] half heigth of the prism
  			      
  Double_t sob_Rprizm   =  radius + hthick + poffset + pheight + EVoffset + (sob_len-2.*phlength)*tan(60./180.*pi);
  
  cout<<"prizm length = "<<phlength*2.<<endl;
  cout<<"prizm height = "<<2.*phlength * tan(pangle/180.*pi)<<endl;
      
  //----------------------------------------------------------
      
  // Rotations:
  TGeoRotation rot1;
  rot1.RotateZ(90.);
    
  TString fGeoFile= Form("../../geometry/dirc_l%d_p%d.root",fFocusingSystem, fprizm);
  TFile* fi = new TFile(fGeoFile,"RECREATE");
  cout<<"Output file = "<<fGeoFile<<endl;
    
   Double_t par[10];   
   Double_t aa;   
   Double_t z, density, radl, absl, w; 
   Int_t nel, numed, nz;
  
   new TGeoManager("Drc", "Drc");

   // MATERIALS, MIXTURES AND TRACKING MEDIA
// Mixture: air
   nel     = 3;
   density = 0.001205;
   TGeoMixture *air = new TGeoMixture("air", nel,density);
      aa = 14.010000;   z = 7.000000;   w = 0.780000;  // N
   air->DefineElement(0,aa,z,w);
      aa = 16.000000;   z = 8.000000;   w = 0.210000;  // O
   air->DefineElement(1,aa,z,w);
      aa = 39.950000;   z = 18.000000;   w = 0.010000;  // AR
   air->DefineElement(2,aa,z,w);
   //air->SetIndex(0);
// Medium: air
   numed   = 1;  // medium number
   par[0]  = 0.000000; // isvol
   par[1]  = 1.000000; // ifield
   par[2]  = 30.000000; // fieldm
   par[3]  = -1.000000; // tmaxfd
   par[4]  = -1.000000; // stemax
   par[5]  = -1.000000; // deemax
   par[6]  = 0.001000; // epsil
   par[7]  = -1.000000; // stmin
   TGeoMedium *air_m = new TGeoMedium("air", numed,air, par);

// Mixture: DIRCairNoSens
   nel     = 3;
   density = 0.001205;
   TGeoMixture *DIRCairNoSens = new TGeoMixture("DIRCairNoSens", nel,density);
      aa = 14.010000;   z = 7.000000;   w = 0.780000;  // N
   DIRCairNoSens->DefineElement(0,aa,z,w);
      aa = 16.000000;   z = 8.000000;   w = 0.210000;  // O
   DIRCairNoSens->DefineElement(1,aa,z,w);
      aa = 39.950000;   z = 18.000000;   w = 0.010000;  // AR
   DIRCairNoSens->DefineElement(2,aa,z,w);
   //DIRCairNoSens->SetIndex(1);
// Medium: DIRCairNoSens
   numed   = 2;  // medium number
   par[0]  = 0.000000; // isvol
   par[1]  = 1.000000; // ifield
   par[2]  = 30.000000; // fieldm
   par[3]  = -1.000000; // tmaxfd
   par[4]  = -1.000000; // stemax
   par[5]  = -1.000000; // deemax
   par[6]  = 0.001000; // epsil
   par[7]  = -1.000000; // stmin
   TGeoMedium  *DIRCairNoSens_m = new TGeoMedium("DIRCairNoSens", numed,DIRCairNoSens, par);

// Material: DIRCcarbonFiber
   aa       = 12.011000;
   z       = 6.000000;
   density = 2.500000;
   radl    = 16.999068;
   absl    = 32.080525;
  TGeoMaterial *DIRCcarbonFiber = new TGeoMaterial("DIRCcarbonFiber", aa,z,density,radl,absl);
   //DIRCcarbonFiber->SetIndex(6);
// Medium: DIRCcarbonFiber
   numed   = 7;  // medium number
   par[0]  = 0.000000; // isvol
   par[1]  = 0.000000; // ifield
   par[2]  = 20.000000; // fieldm
   par[3]  = -1.000000; // tmaxfd
   par[4]  = -1.000000; // stemax
   par[5]  = -1.000000; // deemax
   par[6]  = 0.001000; // epsil
   par[7]  = -1.000000; // stmin
   TGeoMedium *DIRCcarbonFiber_m = new TGeoMedium("DIRCcarbonFiber", numed,DIRCcarbonFiber, par);

// Mixture: FusedSil
   nel     = 2;
   density = 2.200000;
   TGeoMixture* FusedSil = new TGeoMixture("FusedSil", nel,density);
      aa = 28.090000;   z = 14.000000;   w = 0.467475;  // SI
   FusedSil->DefineElement(0,aa,z,w);
      aa = 15.999400;   z = 8.000000;   w = 0.532525;  // O
   FusedSil->DefineElement(1,aa,z,w);
   //FusedSil->SetIndex(0);
// Medium: FusedSil
   numed   = 1;  // medium number
   par[0]  = 1.000000; // isvol
   par[1]  = 1.000000; // ifield
   par[2]  = 20.000000; // fieldm
   par[3]  = -1.000000; // tmaxfd
   par[4]  = -1.000000; // stemax
   par[5]  = -1.000000; // deemax
   par[6]  = 0.001000; // epsil
   par[7]  = -1.000000; // stmin
   TGeoMedium *FusedSil_m = new TGeoMedium("FusedSil", numed,FusedSil, par);

// Mixture: Mirror
   nel     = 2;
   density = 2.200000;
   TGeoMixture* Mirror = new TGeoMixture("Mirror", nel,density);
      aa = 28.090000;   z = 14.000000;   w = 0.467475;  // SI
   Mirror->DefineElement(0,aa,z,w);
      aa = 15.999400;   z = 8.000000;   w = 0.532525;  // O
   Mirror->DefineElement(1,aa,z,w);
   //Mirror->SetIndex(4);
// Medium: Mirror
   numed   = 5;  // medium number
   par[0]  = 0.000000; // isvol
   par[1]  = 0.000000; // ifield
   par[2]  = 20.000000; // fieldm
   par[3]  = -1.000000; // tmaxfd
   par[4]  = -1.000000; // stemax
   par[5]  = -1.000000; // deemax
   par[6]  = 0.001000; // epsil
   par[7]  = -1.000000; // stmin
   TGeoMedium *Mirror_m = new TGeoMedium("Mirror", numed,Mirror, par);

// Mixture: Marcol82
   nel     = 2;
   density = 0.850000;
   TGeoMixture *Marcol82 = new TGeoMixture("Marcol82", nel,density);
      aa = 1.007940;   z = 1.000000;   w = 0.148605;  // H
   Marcol82->DefineElement(0,aa,z,w);
      aa = 12.010700;   z = 6.000000;   w = 0.851395;  // C
   Marcol82->DefineElement(1,aa,z,w);
   //Marcol82->SetIndex(5);
// Medium: Marcol82
   numed   = 6;  // medium number
   par[0]  = 0.000000; // isvol
   par[1]  = 1.000000; // ifield
   par[2]  = 30.000000; // fieldm
   par[3]  = -1.000000; // tmaxfd
   par[4]  = -1.000000; // stemax
   par[5]  = -1.000000; // deemax
   par[6]  = 0.001000; // epsil
   par[7]  = -1.000000; // stmin
   TGeoMedium *Marcol82_m = new TGeoMedium("Marcol82", numed,Marcol82, par);

// Mixture: Marcol82-7
   nel     = 2;
   density = 0.850000;
   TGeoMixture *Marcol82_7 = new TGeoMixture("Marcol82-7", nel,density);
      aa = 1.007940;   z = 1.000000;   w = 0.148605;  // H
   Marcol82_7->DefineElement(0,aa,z,w);
      aa = 12.010700;   z = 6.000000;   w = 0.851395;  // C
   Marcol82_7->DefineElement(1,aa,z,w);
   //Marcol82_7->SetIndex(5);
// Medium: Marcol82_7
   numed   = 6;  // medium number
   par[0]  = 0.000000; // isvol
   par[1]  = 1.000000; // ifield
   par[2]  = 30.000000; // fieldm
   par[3]  = -1.000000; // tmaxfd
   par[4]  = -1.000000; // stemax
   par[5]  = -1.000000; // deemax
   par[6]  = 0.001000; // epsil
   par[7]  = -1.000000; // stmin
   TGeoMedium *Marcol82_7_m = new TGeoMedium("Marcol82-7", numed,Marcol82_7, par);


// Mixture: NLAK33A
   nel = 2;
   density = 4.220000;
   TGeoMixture *NLAK33A = new TGeoMixture("NLAK33A", nel, density);
      aa = 28.090000;   z = 14.000000;   w = 0.467475;  // SI
   NLAK33A->DefineElement(0,aa,z,w);
      aa = 15.999400;   z = 8.000000;   w = 0.532525;  // O
   NLAK33A->DefineElement(1,aa,z,w);
   //NLAK33A->SetIndex(2);
// Medium: NLAK33A
   numed   = 3;  // medium number
   par[0]  = 1.000000; // isvol
   par[1]  = 1.000000; // ifield
   par[2]  = 20.000000; // fieldm
   par[3]  = -1.000000; // tmaxfd
   par[4]  = -1.000000; // stemax
   par[5]  = -1.000000; // deemax
   par[6]  = 0.001000; // epsil
   par[7]  = -1.000000; // stmin
   TGeoMedium *NLAK33A_m = new TGeoMedium("NLAK33A", numed,NLAK33A, par);
  ///-------------------------------------------------------------------------------------------------------------

  

 // focusing systems:
  // 0 no ficusing:
  if(fFocusingSystem == 0){
    Double_t flen = 0.;
    fSlabEnd = -bbox_hlen + bbox_shift + flen;  
    cout<<"bar ends at = "<<fSlabEnd<<endl;
    
    //fAtBarEnd = "DrcBar";
  }
// 1 lens
  if (fFocusingSystem == 1){  // L E N S E S  (no airgaps, thin nlak33)
    // some notes to lens operations (revision 9649) is in
    // ~carsten/work/documents/software/PandaRoot/lens_definitions_2.pdf
    
    Double_t r = 75.18; // first lens radius (cm)    
    Double_t alpha = TMath::ASin(bbX/12/r);  // lside -> bbX
    Double_t a = r - r*TMath::Cos(alpha);
    Double_t b = a + 0.5; // box dimension  .6 instead of .5 due to strong curvature

    cout<<" DIRC a,b = "<<a<<" "<<b<<endl;
  

    Double_t r2 = 75.18; // radius second lens (cm)
    Double_t b2 = 0.2;   // + a2;
     
    Double_t r3 = 17.95; // third lens radius (cm)
    Double_t alpha3 = TMath::ASin(bbX/12/r3);  // lside -> bbX
    Double_t a3 = r3 - r3*TMath::Cos(alpha3);
    Double_t b3 = a3 + 0.0; // box dimension  .6 instead of .5 due to strong curvature
  
    // lens1 (b) + lens2 (0.6) + lens3 (b3) + gap (0.5) 
   
    len = b + 0.2 + b3 + 0.5;//+ a2; // dimension of the box containing both lenses
    
    cout<<"DIRC len= "<<len<<endl;
  
    fSlabEnd = -bbox_hlen + bbox_shift + len; // used in processHits
    cout<<"bar ends at = "<<fSlabEnd<<endl;
    
    // Lenses
 
    // Lens 1
    Double_t t = -r +b/2;

    TGeoSphere* logicSphere= new TGeoSphere("S",0.,r, 0. ,180.,0.,360.);
    TGeoBBox* lBox = new TGeoBBox("B", (bbX/barnum)/2-bargap, hthick, b/2.);
    TGeoTranslation *tr1 = new TGeoTranslation("tr1", 0.,0., t);
    tr1->RegisterYourself();
    TGeoCompositeShape *cs = new TGeoCompositeShape("cs","S*(B:tr1)");
    TGeoVolume *lens1 = new TGeoVolume("DrcLENS1",cs, gGeoManager->GetMedium("FusedSil"));
    lens1->SetLineColor(kRed-8);
    lens1->SetTransparency(40); 
     
    // position lens within already shifted bar container at -(bbox_hlen-eps)+len, the lens base is -r + b
    // with -0.01 one can make a gap visible (.1mm) for orientation   
    //barContainer->AddNode(lens1, 1,new TGeoCombiTrans(0., 0., -(bbox_hlen-eps)+len -(-r+b) /*-0.01*/, new TGeoRotation (0)));
/*    barContainer->AddNode(lens1, 1,new TGeoCombiTrans(0., 0., -(bbox_hlen)+len -(-r+b), new TGeoRotation (0)));
*/  fdz_lens1 = -(bbox_hlen)+len -(-r+b);
     
       
    //Lens 2
    Double_t t2 = -r2;// +b2/2 r2  is the reference point (concave lens) 
    TGeoSphere* logicSphere2 = new TGeoSphere("S2",0 ,r2, 0. ,180.,0.,360.);
    TGeoBBox*   lBox2        = new TGeoBBox("B2", (bbX/barnum)/2.-bargap, hthick, b2/2.); // lside -> bbX
    TGeoTranslation *tr2     = new TGeoTranslation("tr2", 0.,0., t2);
    tr2->RegisterYourself();
    TGeoCompositeShape *cs2 = new TGeoCompositeShape("cs2","(B2:tr2)-S2");
    TGeoVolume *lens2 = new TGeoVolume("DrcLENS2",cs2, gGeoManager->GetMedium("NLAK33A"));
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
    TGeoBBox* lBox3 = new TGeoBBox("B3", (bbX/barnum)/2-bargap, hthick, b3/2.); // lside -> bbX
    TGeoTranslation *tr3 = new TGeoTranslation("tr3", 0.,0., t3);
    tr3->RegisterYourself();
    TGeoCompositeShape *cs3 = new TGeoCompositeShape("cs3","S3*(B3:tr3)");
    TGeoVolume *lens3 = new TGeoVolume("DrcLENS3",cs3, gGeoManager->GetMedium("NLAK33A"));
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
    
    //fAtBarEnd = "DrcLENS3";
  }   // E N D      O F      N E W      L E N S E S  
  
  // 2 - mirror:
  if (fFocusingSystem == 2){  // Mirrors at front
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
     TGeoBBox*   lBox        = new TGeoBBox("B", (bbX/barnum)/2-bargap, hthick, fabs(len1)/2.); // lside -> bbX
      
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
     TGeoBBox*   lBox2        = new TGeoBBox("B2", (bbX/barnum)/2-bargap, hthick, fabs(len2)/2.); // lside -> bbX

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
   }   // E N D      O F      MIRRORS
 //--------------------------------------------------------------------------------------------- 
 

  // create top volume:
  TGeoVolume* cave;
  TGeoBBox*   lTop = new TGeoBBox(200,200,300);
  cave = new TGeoVolume("DIRC", lTop, air_m);
  gGeoManager->SetTopVolume(cave);
  
  // create pre-top volume:
  TGeoVolume* vLocalMother;
  TGeoPcon*   shape = new TGeoPcon("BarrelDIRCShape", 0, 360., 4);
  shape->DefineSection(0, bbox_zdown, 45., 55.);
  shape->DefineSection(1, bbox_zup, 45., 55.);
  shape->DefineSection(2, bbox_zup - sob_len, 45., sob_Rout+poffset+pheight+EVoffset+PDthick);
  shape->DefineSection(3, bbox_zup - sob_len - 0.1, 45., sob_Rout+poffset+pheight+EVoffset+PDthick);
  vLocalMother = new TGeoVolume("BarrelDIRC", shape, air_m); //DIRCairNoSens_m); //################
  cave->AddNode(vLocalMother, 0,0);
 

  cout<<"bbox length = "<<2.*(bbox_hlen-0.5*(boxgap+boxthick))<<", prizm length = "<<2.*(phlength+0.5*(boxgap+boxthick))<<endl;
  cout<<"prizm shift = "<<-bbox_hlen+0.5*(boxgap+boxthick)-phlength<<endl;
  
  // create BarBoxes:  
  TGeoBBox* logicbbL;
  TGeoVolume *bbox;
  if(fprizm == kFALSE){
    logicbbL = new TGeoBBox("logicbbL", bbX/2.+boxthick, hthick+boxgap+boxthick, bbox_hlen);
    bbox = new TGeoVolume("DrcBarBox", logicbbL,DIRCcarbonFiber_m); 
  }
  if(fprizm == kTRUE){
    logicbbL = new TGeoBBox("logicbbL", bbX/2.+boxthick, hthick+boxgap+boxthick, bbox_hlen-0.5*(boxgap+boxthick));
    TGeoTrap* logicPrizmBox = new TGeoTrap("logicPrizmBox", phlength+0.5*(boxgap+boxthick), pangle/2., 270.,
    hthick+boxgap+boxthick+0.5*(poffset+pdrop+pheight), bbX/2., bbX/2., 0., hthick+boxgap+boxthick+0.5*(poffset+pdrop), bbX/2., bbX/2., 0.);
  
    TGeoTranslation* trprb = new TGeoTranslation("trprb", 0., 0.5*(poffset-pdrop)+pheight/4., -bbox_hlen-phlength);
    trprb->RegisterYourself();
    TGeoCompositeShape *cspb = new TGeoCompositeShape("cspb","logicbbL + logicPrizmBox:trprb");
    bbox = new TGeoVolume("DrcBarBox", cspb,DIRCcarbonFiber_m);
  }
  bbox->SetLineColor(30);
  
  
  TGeoBBox* logicbbS;
  TGeoVolume *abox;
  if(fprizm == kFALSE){
    logicbbS = new TGeoBBox("logicbbS", bbX/2., hthick+boxgap, bbox_hlen);
    abox = new TGeoVolume("DrcAirBox", logicbbS, DIRCairNoSens_m);
    bbox->AddNode(abox, 0, new TGeoCombiTrans(0., 0., 0., new TGeoRotation(0)));
  }
  if(fprizm == kTRUE){
    logicbbS = new TGeoBBox("logicbbS", bbX/2., hthick+boxgap, bbox_hlen-0.5*boxgap);
    TGeoTrap* logicPrizmContainer = new TGeoTrap("logicPrizmContainer", phlength+0.5*boxgap, pangle/2., 270.,
    hthick+boxgap+0.5*(poffset+pdrop+pheight), bbX/2., bbX/2., 0., hthick+boxgap+0.5*(poffset+pdrop), bbX/2., bbX/2., 0.);

    TGeoTranslation* trprc = new TGeoTranslation("trprc", 0., 0.5*(poffset-pdrop)+pheight/4., -bbox_hlen-phlength);
    trprc->RegisterYourself();
    TGeoCompositeShape *cspc = new TGeoCompositeShape("cspc","logicbbS + logicPrizmContainer:trprc");  
    abox = new TGeoVolume("DrcAirBox", cspc, DIRCairNoSens_m);
    bbox->AddNode(abox, 0, new TGeoCombiTrans(0., 0., -0.5*boxthick, new TGeoRotation(0)));
  }
  abox->SetLineColor(19);
  
  // put barboxes into right positions:    
  Double_t dx_bbox, dy_bbox, dz_bbox, phi_curr;    
      
  for(Int_t m = 0; m < bbnum; m ++){       
    phi_curr = (90. - phi0 - dphi*m)/180.*pi;    
    if(m > bbnum/2-1){ phi_curr = (90. - phi0 - dphi*m - 2.*pipehAngle)/180.*pi; }
    dx_bbox = radius * cos(phi_curr);
    dy_bbox = radius * sin(phi_curr);
    if(fprizm == 0){
      dz_bbox = bbox_shift;
    } 
    if(fprizm == 1){
      dz_bbox = bbox_shift + 0.5*(boxgap+boxthick);
    }  
    TGeoRotation rot_bbox;    
    rot_bbox.RotateZ( -phi0 - m*dphi - (TMath::Floor(2.*m/bbnum))*(2.*pipehAngle));    
    vLocalMother->AddNode(bbox, m+1, new TGeoCombiTrans(dx_bbox, dy_bbox, dz_bbox, new TGeoRotation(rot_bbox)));    
  } 
  
  cout<<"bar width = "<<2.*(((bbX/barnum)/2.)-bargap) << ", bar with gaps = "<<(bbX/barnum)<<endl;
  cout<<"barboxL width = "<<bbX/2.+boxgap+boxthick<<", barboxS width = "<<bbX/2.+boxgap<<endl;
  
  // create logic bar:  
  TGeoBBox* logicBar = new TGeoBBox("logicBar",  ((bbX/barnum)/2.)-bargap, hthick, bbox_hlen-fabs(len)/2.-mirr_hthick);
  TGeoVolume* bar;
  if(fprizm == kFALSE){  
    bar = new TGeoVolume("DrcBarSensor",logicBar, FusedSil_m );
  }
  if(fprizm == kTRUE){    
    TGeoTrap* logicPrizm = new TGeoTrap("logicPrizm", phlength, pangle/2., 270., hthick+0.5*(poffset+pdrop+pheight),
    (bbX/barnum/2.)-bargap, (bbX/barnum/2.)-bargap, 0., hthick+0.5*(poffset+pdrop), (bbX/barnum/2.)-bargap, (bbX/barnum/2.)-bargap, 0.);
    
    TGeoVolume* prizmtrap = new TGeoVolume("prizmtrap", logicPrizm,FusedSil_m);
    prizmtrap->SetLineColor(kCyan-9);
    prizmtrap->SetTransparency(60);

    TGeoTranslation* trpr = new TGeoTranslation("trpr", 0., 0.5*(poffset-pdrop)+pheight/4., -(bbox_hlen-fabs(len)/2.-mirr_hthick)-phlength);
    trpr->RegisterYourself();
    TGeoCompositeShape *csbp = new TGeoCompositeShape("csbp","logicBar + logicPrizm:trpr");
    bar = new TGeoVolume("DrcBarSensor",csbp, FusedSil_m );
  }
  bar->SetLineColor(kCyan-9);
  bar->SetTransparency(60);
  
  // create logic mirror:
  TGeoBBox* logicMirror  = new TGeoBBox("logicMirror", bbX/barnum/2.-bargap, hthick, mirr_hthick);
  TGeoVolume *mirr  = new TGeoVolume("DrcMirr", logicMirror,  Mirror_m);
  mirr->SetLineColor(5);
  
  Double_t dx, dy, dz_bar, dz_mirr;
  
  for(Int_t j=0; j<barnum; j++){
    dx      = - (bbX/2.) + (bbX/barnum)/2. + j * (bbX/barnum); 
    dy  = 0.;//len/2. - mirr_hthick;
    if(fprizm == kFALSE){
      dz_bar = len/2.-mirr_hthick;
      dz_mirr = bbox_hlen - mirr_hthick;
      if(fFocusingSystem == 1){ // lens
        abox->AddNode(lens1,  1+j, new TGeoCombiTrans(dx, dy, fdz_lens1, new TGeoRotation (0)));
	abox->AddNode(lens2,  1+j, new TGeoCombiTrans(dx, dy, fdz_lens2, new TGeoRotation (0)));
        abox->AddNode(lens3,  1+j, new TGeoCombiTrans(dx, dy, fdz_lens3, new TGeoRotation (0)));
      }
      if(fFocusingSystem == 2){ // mirror
        abox->AddNode(block1, 1+j, new TGeoCombiTrans(dx, dy, fdz_mirr1, new TGeoRotation (0)));
        abox->AddNode(block2, 1+j, new TGeoCombiTrans(dx, dy, fdz_mirr2, new TGeoRotation (0)));
      }
    }
    if(fprizm == kTRUE){
      if(fFocusingSystem == 0){
        dz_bar  = -mirr_hthick - 0.5*boxgap;
        dz_mirr = bbox_hlen -0.5*boxgap - mirr_hthick;
      }
      if(fFocusingSystem == 2){
        dz_bar  = -mirr_hthick - 0.5*boxgap + len/2.;
	abox->AddNode(block1, 1+j, new TGeoCombiTrans(dx, dy, fdz_mirr1, new TGeoRotation (0)));
        abox->AddNode(block2, 1+j, new TGeoCombiTrans(dx, dy, fdz_mirr2, new TGeoRotation (0)));
      }
    }        
    abox->AddNode(bar,  1+j, new TGeoCombiTrans(dx, dy, dz_bar, new TGeoRotation(0)));
    if(fFocusingSystem != 2){ // not a forward mirror
      abox->AddNode(mirr, 1+j, new TGeoCombiTrans(dx, dy, dz_mirr, new TGeoRotation(0)));
    }
  }
    
  // Expansion volume:
  Double_t dR; 
  Double_t xEV;
  Double_t cosFactor1; 

  TGeoPgon* logicEV1, * logicEV2, *logicEV3, * logicEV4;
  cosFactor1 = cos(pipehAngle/180.*pi)/cos(dphi/180.*pi/2.);  
  if(fprizm == kFALSE){
    dR = (radius+hthick+boxgap+boxthick)/cos(dphi/2./180.*pi) - (radius-hthick);
    xEV = (dR + sob_len*tan(sob_angle/180.*pi))/ (tan(sob_angle/180.*pi) + tan(sob_angleB/180.*pi));    
    if(sob_angleB == 90.){
      logicEV1 = new TGeoPgon("logicEV1", 93.6, 172.8, bbnum/2, 2);
      logicEV1->DefineSection(0, 0.,      radius-hthick,  sob_Rout);
      logicEV1->DefineSection(1, sob_len, radius-hthick,  (radius+hthick+boxgap+boxthick)/cos(dphi/2./180.*pi));
      logicEV2 = new TGeoPgon("logicEV2", -86.4, 172.8, bbnum/2, 2);
      logicEV2->DefineSection(0, 0.,      radius-hthick,  sob_Rout);
      logicEV2->DefineSection(1, sob_len, radius-hthick,  (radius+hthick+boxgap+boxthick)/cos(dphi/2./180.*pi));     
      logicEV3 = new TGeoPgon("logicEV3", 86.4, 7.2, 1, 2);
      logicEV3->DefineSection(0, 0.,      (radius-hthick)*cosFactor1,  sob_Rout*cosFactor1);
      logicEV3->DefineSection(1, sob_len, (radius-hthick)*cosFactor1,  (radius+hthick+boxgap+boxthick)/cos(dphi/2./180.*pi)*cosFactor1);     
      logicEV4 = new TGeoPgon("logicEV4", -93.6, 7.2, 1, 2);
      logicEV4->DefineSection(0, 0.,      (radius-hthick)*cosFactor1,  sob_Rout*cosFactor1);
      logicEV4->DefineSection(1, sob_len, (radius-hthick)*cosFactor1,  (radius+hthick+boxgap+boxthick)/cos(dphi/2./180.*pi)*cosFactor1);     
      
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
  if(fprizm == kTRUE){
    dR = (radius+hthick+poffset+pheight+EVoffset)/cos(dphi/2./180.*pi) - (radius-hthick-pdrop-EVdrop);
    xEV = (dR + (sob_len-2.*phlength)*tan(sob_angle/180.*pi))/ (tan(sob_angle/180.*pi) + tan(sob_angleB/180.*pi));
    if(sob_angleB == 90.){
      logicEV1 = new TGeoPgon("logicEV1", 93.6, 172.8, bbnum/2, 2);
      logicEV1->DefineSection(0, 0.,      radius-hthick-pdrop-EVdrop,  sob_Rprizm);
      logicEV1->DefineSection(1, sob_len-2.*phlength, radius-hthick-pdrop-EVdrop, (radius+hthick+poffset+pheight+EVoffset)/cos(dphi/2./180.*pi));
      logicEV2 = new TGeoPgon("logicEV2", -86.4, 172.8, bbnum/2, 2);
      logicEV2->DefineSection(0, 0.,      radius-hthick-pdrop-EVdrop,  sob_Rprizm);
      logicEV2->DefineSection(1, sob_len-2.*phlength, radius-hthick-pdrop-EVdrop, (radius+hthick+poffset+pheight+EVoffset)/cos(dphi/2./180.*pi));
      logicEV3 = new TGeoPgon("logicEV3", 86.4, 7.2, 1, 2);
      logicEV3->DefineSection(0, 0.,      (radius-hthick-pdrop-EVdrop)*cosFactor1,  sob_Rprizm*cosFactor1);
      logicEV3->DefineSection(1, sob_len-2.*phlength, (radius-hthick-pdrop-EVdrop)*cosFactor1, (radius+hthick+poffset+pheight+EVoffset)/cos(dphi/2./180.*pi)*cosFactor1);
      logicEV4 = new TGeoPgon("logicEV4", -93.6, 7.2, 1, 2);
      logicEV4->DefineSection(0, 0.,      (radius-hthick-pdrop-EVdrop)*cosFactor1,  sob_Rprizm*cosFactor1);
      logicEV4->DefineSection(1, sob_len-2.*phlength, (radius-hthick-pdrop-EVdrop)*cosFactor1, (radius+hthick+poffset+pheight+EVoffset)/cos(dphi/2./180.*pi)*cosFactor1);
    } 
    if(sob_angleB != 90.){
      logicEV1 = new TGeoPgon("logicEV1", 93.6, 172.8, bbnum/2, 3);
      logicEV1->DefineSection(0, 0.,      radius-hthick-pdrop-EVdrop,  radius-hthick-pdrop-EVdrop+eps);
      logicEV1->DefineSection(1, xEV,     radius-hthick-pdrop-EVdrop,  sob_Rprizm - xEV*tan(sob_angle/180.*pi));
      logicEV1->DefineSection(2, sob_len-2.*phlength, radius-hthick-pdrop-EVdrop,  (radius+hthick+poffset+pheight+EVoffset)/cos(dphi/2./180.*pi));      
      logicEV2 = new TGeoPgon("logicEV2", -86.4, 172.8, bbnum/2, 3);
      logicEV2->DefineSection(0, 0.,      radius-hthick-pdrop-EVdrop,  radius-hthick-pdrop-EVdrop+eps);
      logicEV2->DefineSection(1, xEV,     radius-hthick-pdrop-EVdrop,  sob_Rprizm - xEV*tan(sob_angle/180.*pi));
      logicEV2->DefineSection(2, sob_len-2.*phlength, radius-hthick-pdrop-EVdrop,  (radius+hthick+poffset+pheight+EVoffset)/cos(dphi/2./180.*pi));
      logicEV3 = new TGeoPgon("logicEV3", 86.4, 7.2, 1, 3);
      logicEV3->DefineSection(0, 0.,      (radius-hthick-pdrop-EVdrop)*cosFactor1,  (radius-hthick-pdrop-EVdrop+eps)*cosFactor1);
      logicEV3->DefineSection(1, xEV,     (radius-hthick-pdrop-EVdrop)*cosFactor1,  (sob_Rprizm - xEV*tan(sob_angle/180.*pi))*cosFactor1);
      logicEV3->DefineSection(2, sob_len-2.*phlength, (radius-hthick-pdrop-EVdrop)*cosFactor1,  (radius+hthick+poffset+pheight+EVoffset)/cos(dphi/2./180.*pi)*cosFactor1);
      logicEV4 = new TGeoPgon("logicEV4", -93.6, 7.2, 1, 3);
      logicEV4->DefineSection(0, 0.,      (radius-hthick-pdrop-EVdrop)*cosFactor1,  (radius-hthick-pdrop-EVdrop+eps)*cosFactor1);
      logicEV4->DefineSection(1, xEV,     (radius-hthick-pdrop-EVdrop)*cosFactor1,  (sob_Rprizm - xEV*tan(sob_angle/180.*pi))*cosFactor1);
      logicEV4->DefineSection(2, sob_len-2.*phlength, (radius-hthick-pdrop-EVdrop)*cosFactor1,  (radius+hthick+poffset+pheight+EVoffset)/cos(dphi/2./180.*pi)*cosFactor1);      
    }
  }
  
  TGeoCompositeShape *logicEV = new TGeoCompositeShape("logicEV","logicEV1 + logicEV2 + logicEV3 + logicEV4");
  TGeoVolume* baseEV = new TGeoVolume("DrcEV", logicEV, Marcol82_7_m);
  baseEV->SetLineColor(kMagenta+2);
  baseEV->SetTransparency(50);
  vLocalMother->AddNode(baseEV, 1, new TGeoCombiTrans(0.,0.,sob_shift, new TGeoRotation(0)));

  // PhotoDetector:
  TGeoPgon *logicPD1 = new TGeoPgon("logicPD1",  93.6, 172.8, bbnum/2, 2);
  TGeoPgon *logicPD2 = new TGeoPgon("logicPD2", -86.4, 172.8, bbnum/2, 2);
  TGeoPgon *logicPD3 = new TGeoPgon("logicPD3",  86.4,   7.2,       1, 2);
  TGeoPgon *logicPD4 = new TGeoPgon("logicPD4", -93.6,   7.2,       1, 2);
  if(fprizm == kFALSE){
    if(sob_angleB == 90.){       
      logicPD1->DefineSection(0, 0.,  radius-hthick, sob_Rout);
      logicPD1->DefineSection(1, PDthick,  radius-hthick, sob_Rout);           
      logicPD2->DefineSection(0, 0.,  radius-hthick, sob_Rout);
      logicPD2->DefineSection(1, PDthick,  radius-hthick, sob_Rout);      
      logicPD3->DefineSection(0, 0., (radius-hthick)*cosFactor1, sob_Rout*cosFactor1);
      logicPD3->DefineSection(1, PDthick, (radius-hthick)*cosFactor1, sob_Rout*cosFactor1);
      logicPD4->DefineSection(0, 0., (radius-hthick)*cosFactor1, sob_Rout*cosFactor1);
      logicPD4->DefineSection(1, PDthick, (radius-hthick)*cosFactor1, sob_Rout*cosFactor1);
    }
    if(sob_angleB != 90.){     
      logicPD1->DefineSection(0, 0., radius-hthick+eps, radius-hthick+eps+0.1);
      logicPD1->DefineSection(1, xEV, sob_Rout - xEV*tan(sob_angle/180.*pi), sob_Rout- xEV*tan(sob_angle/180.*pi)+0.1);     
      logicPD2->DefineSection(0, 0., radius-hthick+eps, radius-hthick+eps+0.1);
      logicPD2->DefineSection(1, xEV, sob_Rout - xEV*tan(sob_angle/180.*pi), sob_Rout- xEV*tan(sob_angle/180.*pi)+0.1);
      logicPD3->DefineSection(0, 0., (radius-hthick+eps)*cosFactor1, (radius-hthick+eps+0.1)*cosFactor1);
      logicPD3->DefineSection(1, xEV, (sob_Rout - xEV*tan(sob_angle/180.*pi))*cosFactor1, (sob_Rout- xEV*tan(sob_angle/180.*pi)+0.1)*cosFactor1);
      logicPD4->DefineSection(0, 0., (radius-hthick+eps)*cosFactor1, (radius-hthick+eps+0.1)*cosFactor1);
      logicPD4->DefineSection(1, xEV, (sob_Rout - xEV*tan(sob_angle/180.*pi))*cosFactor1, (sob_Rout- xEV*tan(sob_angle/180.*pi)+0.1)*cosFactor1);
    }
  
  }
  if(fprizm == kTRUE){
    if(sob_angleB == 90.){
      logicPD1->DefineSection(0, 0.,  radius-hthick-pdrop-EVdrop+eps, sob_Rprizm);
      logicPD1->DefineSection(1, PDthick,  radius-hthick-pdrop-EVdrop+eps, sob_Rprizm);     
      logicPD2->DefineSection(0, 0.,  radius-hthick-pdrop-EVdrop+eps, sob_Rprizm);
      logicPD2->DefineSection(1, PDthick,  radius-hthick-pdrop-EVdrop+eps, sob_Rprizm);      
      logicPD3->DefineSection(0, 0., (radius-hthick-pdrop-EVdrop+eps)*cosFactor1, sob_Rprizm*cosFactor1);
      logicPD3->DefineSection(1, PDthick, (radius-hthick-pdrop-EVdrop+eps)*cosFactor1, sob_Rprizm*cosFactor1);     
      logicPD4->DefineSection(0, 0., (radius-hthick-pdrop-EVdrop+eps)*cosFactor1, sob_Rprizm*cosFactor1);
      logicPD4->DefineSection(1, PDthick, (radius-hthick-pdrop-EVdrop+eps)*cosFactor1, sob_Rprizm*cosFactor1);
    }
    if(sob_angleB != 90.){    
      logicPD1->DefineSection(0, 0., radius-hthick-pdrop-EVdrop+eps, radius-hthick-pdrop-EVdrop+eps+0.1);
      logicPD1->DefineSection(1, xEV, sob_Rprizm - xEV*tan(sob_angle/180.*pi), sob_Rprizm- xEV*tan(sob_angle/180.*pi)+0.1);     
      logicPD2->DefineSection(0, 0., radius-hthick-pdrop-EVdrop+eps, radius-hthick-pdrop-EVdrop+eps+0.1);
      logicPD2->DefineSection(1, xEV, sob_Rprizm - xEV*tan(sob_angle/180.*pi), sob_Rprizm- xEV*tan(sob_angle/180.*pi)+0.1);
      logicPD3->DefineSection(0, 0., (radius-hthick-pdrop-EVdrop+eps)*cosFactor1, (radius-hthick-pdrop-EVdrop+eps+0.1)*cosFactor1);
      logicPD3->DefineSection(1, xEV, (sob_Rprizm - xEV*tan(sob_angle/180.*pi))*cosFactor1, (sob_Rprizm- xEV*tan(sob_angle/180.*pi)+0.1)*cosFactor1);
      logicPD4->DefineSection(0, 0., (radius-hthick-pdrop-EVdrop+eps)*cosFactor1, (radius-hthick-pdrop-EVdrop+eps+0.1)*cosFactor1);
      logicPD4->DefineSection(1, xEV, (sob_Rprizm - xEV*tan(sob_angle/180.*pi))*cosFactor1, (sob_Rprizm- xEV*tan(sob_angle/180.*pi)+0.1)*cosFactor1);
    }
  }  
  TGeoCompositeShape *logicPD = new TGeoCompositeShape("logicPD","logicPD1 + logicPD2 + logicPD3 + logicPD4");
  TGeoVolume *pd = new TGeoVolume("DrcPDSensor", logicPD, FusedSil_m);
  pd->SetLineColor(kGreen-6);
  if(sob_angleB == 90.){
    vLocalMother->AddNode(pd, 1, new TGeoCombiTrans(0., 0., sob_shift-PDthick, new TGeoRotation(0)));
  }
  if(sob_angleB != 90.){
    vLocalMother->AddNode(pd, 1, new TGeoCombiTrans(0., 0., sob_shift, new TGeoRotation(0)));
  }

  gGeoManager->CloseGeometry();

  cave->CheckOverlaps(0.1, "");
  gGeoManager->CheckOverlaps(0.001); // [cm]
  //gGeoManager->CheckGeometryFull();

  
  cave->Write();
  fi->Close(); 
  cave->Draw("ogl");
}
