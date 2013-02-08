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

void createRootGeometry_DIRC_sepEV_MCPs(Bool_t sepEV = kTRUE, Int_t fFocusingSystem = 1, Bool_t fprizm = kFALSE){ 
  
  const Double_t pi = 3.1415926535;
  #include <time.h>

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
  Double_t MCPside	 = 5.3; //[cm] side length of the active area
  Double_t PDgap1	 = 0.8; // [cm]
  Double_t PDgap2	 = 1.6; //[cm]
  Double_t EVwidth 	 = 17.;// [cm]
  Double_t barWidth	 = 3.5; //[cm]
  
  Double_t GlueLayer	 =  0.;// [cm]fGeo->GlueLayer();	  // 0.0038 cm layer of the glue in the middle of the bar
  Double_t GreaseLayer   =  0.;//0.1;// [cm]fGeo->GreaseLayer();  // 1 mm layer of grease between the EV and the PD.
  Double_t radius        =  55.;//48.64;//fGeo->radius();       // 50. radius in middle of the barbox (x and y)
  Double_t hthick        =  1.725/2.;//fGeo->barHalfThick(); // 1.7/2. half thickness of the bars
  Double_t barnum        =  1.;//fGeo->barNum();       // 6 number of bars per barbox
  Double_t bbnum         =  fGeo->BBoxNum();	  //16. total number of sides = barboxes
  Double_t bbGap         =  fGeo->BBoxGap();	  //1.5 gap btw the neighboring barboxes (at the middle height)
  Double_t pipehAngle    =  fGeo->PipehAngle();	  //3.6 [degrees] half of the angular space needed for the target pipe
  
  Double_t bbox_zdown    =  2.5;//fGeo->barBoxZDown();  // 130. bar box z downstream
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
    
  Double_t sob_angleB	 =  fGeo->EVbackAngle();  //90. [degrees] angle of the EV (usually it is 90)
  Double_t sob_angle	 =  30.;//fGeo->EVangle();	  //60. [degrees] opening angle of the EV
  Double_t sob_len       =  fGeo->EVlen();        // 30. in current version
  Double_t sob_shift     =  -bbox_hlen + bbox_shift - sob_len; // -150. 
  Double_t sob_Rout      =  radius + hthick + sob_len*tan(sob_angle/180.*pi)/cos(pi/bbnum/2.);
   
  Double_t bbAngle       =  ( 180. - 2.*pipehAngle - bbGap/radius/pi*180.*(bbnum/2.-1.) )/(bbnum/2.);  // ~20 degrees
  Double_t bbX           =  radius*bbAngle/180.*pi;  // 17.45 cm
  cout<<"bbAngle = "<<bbAngle<<", bbX = "<<bbX<<endl;
  
  
  Double_t phi0 = (180.-2.*pipehAngle)/bbnum + pipehAngle;
  Double_t dphi = (180.-2.*pipehAngle)/bbnum*2.;  
  //----------------------------------------------------------------------
  
  // parameters for prizm and separated EV!!!
  Double_t EVdrop	 =  1.3;//fGeo->EVdrop();	  //0.5 [cm] drop of the EV - inner radius
  Double_t EVoffset	 =  0.;//fGeo->EVoffset();	  //1. [cm] offset of the EV - outer radius
    
  // prizm:  
  Double_t phlength	 =  fGeo->PrismhLength(); //4.5 [cm] half length of the prizm
  Double_t pdrop	 =  fGeo->PrismDrop();	  //0.5 [cm] drop of the prizm - inner side
  Double_t pangle	 =  fGeo->PrismAngle();	  //30. [degrees] angle of the edge
  Double_t poffset	 =  fGeo->PrismOffset();  //1.[cm] prizm offset - outer side
  Double_t pheight	 =  2.*phlength * tan(pangle/180.*pi);  // [cm] half heigth of the prism
  			      
  Double_t sob_Rprizm   =  radius + hthick + poffset + pheight + EVoffset + (sob_len-2.*phlength)*tan(60./180.*pi);
  
  Double_t GlueHeight = GlueLayer/cos(3.1415/2. - sob_angleB/180.*3.1415);
  Double_t PDHeight = PDthick/cos(3.1415/2. - sob_angleB/180.*3.1415);
  cout<<"prizm length = "<<phlength*2.<<endl;
  cout<<"prizm height = "<<2.*phlength * tan(pangle/180.*pi)<<endl;
      
  //----------------------------------------------------------
      
  // Rotations:
  TGeoRotation rot1;
  rot1.RotateZ(90.);
  
  TString fGeoFile1= Form("prototype_MCPs",fFocusingSystem, fprizm); 
  TString fGeoFile= "../../geometry/";
  fGeoFile+=fGeoFile1;
  fGeoFile+=".root";
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
   numed   = 3;  // medium number
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
   numed   = 4;  // medium number
   par[0]  = 1.000000; // isvol
   par[1]  = 1.000000; // ifield
   par[2]  = 20.000000; // fieldm
   par[3]  = -1.000000; // tmaxfd
   par[4]  = -1.000000; // stemax
   par[5]  = -1.000000; // deemax
   par[6]  = 0.001000; // epsil
   par[7]  = -1.000000; // stmin
   TGeoMedium *FusedSil_m = new TGeoMedium("FusedSil", numed,FusedSil, par);
   
// Mixture: Epotek
   nel     = 2;
   density = 2.200000;
   TGeoMixture* Epotek = new TGeoMixture("Epotek", nel,density);
      aa = 28.090000;   z = 14.000000;   w = 0.467475;  // SI
   Epotek->DefineElement(0,aa,z,w);
      aa = 15.999400;   z = 8.000000;   w = 0.532525;  // O
   Epotek->DefineElement(1,aa,z,w);
   //Epotek->SetIndex(0);
// Medium: Epotek
   numed   = 5;  // medium number
   par[0]  = 1.000000; // isvol
   par[1]  = 1.000000; // ifield
   par[2]  = 20.000000; // fieldm
   par[3]  = -1.000000; // tmaxfd
   par[4]  = -1.000000; // stemax
   par[5]  = -1.000000; // deemax
   par[6]  = 0.001000; // epsil
   par[7]  = -1.000000; // stmin
   TGeoMedium *Epotek_m = new TGeoMedium("Epotek", numed, Epotek, par);  
   
// Mixture: OpticalGrease
   nel     = 2;
   density = 2.200000;
   TGeoMixture* OpticalGrease = new TGeoMixture("OpticalGrease", nel,density);
      aa = 28.090000;   z = 14.000000;   w = 0.467475;  // SI
   OpticalGrease->DefineElement(0,aa,z,w);
      aa = 15.999400;   z = 8.000000;   w = 0.532525;  // O
   OpticalGrease->DefineElement(1,aa,z,w);
   //OpticalGrease->SetIndex(0);
// Medium: OpticalGrease
   numed   = 6;  // medium number
   par[0]  = 1.000000; // isvol
   par[1]  = 1.000000; // ifield
   par[2]  = 20.000000; // fieldm
   par[3]  = -1.000000; // tmaxfd
   par[4]  = -1.000000; // stemax
   par[5]  = -1.000000; // deemax
   par[6]  = 0.001000; // epsil
   par[7]  = -1.000000; // stmin
   TGeoMedium *OpticalGrease_m = new TGeoMedium("OpticalGrease", numed, OpticalGrease, par);   

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
   numed   = 7;  // medium number
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
   numed   = 8;  // medium number
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
   numed   = 9;  // medium number
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
   numed   = 10;  // medium number
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
  // 0 no focusing:
  if(fFocusingSystem == 0){
    //Double_t flen = 0.;
    Double_t len = GreaseLayer;
    fSlabEnd = -bbox_hlen + bbox_shift + len;  
    cout<<"bar ends at = "<<fSlabEnd<<endl;
    cout<<"no focusing, len = grease = "<<len<<endl;
    
    TGeoBBox* lay = new TGeoBBox("lay", barWidth/2./*(bbX/barnum)/2-bargap*/, hthick, GreaseLayer/2.);
    TGeoVolume* grBar = new TGeoVolume("greaseBar", lay, gGeoManager->GetMedium("OpticalGrease"));
    grBar->SetLineColor(kBlue);
    grBar->SetTransparency(60);
    
    //fAtBarEnd = "DrcBar";
    Double_t fdz_grease = -(bbox_hlen)+len/2.;
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
    
    cout<<"lens focusing, DIRC len= "<<len<<endl;
  
    fSlabEnd = -bbox_hlen + bbox_shift + len; // used in processHits
    cout<<"bar ends at = "<<fSlabEnd<<endl;
    
    // Lenses
 
    // Lens 1
    Double_t t = -r +b/2;

    TGeoSphere* logicSphere= new TGeoSphere("S",0.,r, 0. ,180.,0.,360.);
    TGeoBBox* lBox = new TGeoBBox("B", barWidth/2./*(bbX/barnum)/2-bargap*/, hthick, b/2.);
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
    TGeoBBox*   lBox2        = new TGeoBBox("B2", barWidth/2./*(bbX/barnum)/2.-bargap*/, hthick, b2/2.); // lside -> bbX
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
    TGeoBBox* lBox3 = new TGeoBBox("B3", barWidth/2./*(bbX/barnum)/2-bargap*/, hthick, b3/2.); // lside -> bbX
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
     TGeoBBox*   lBox        = new TGeoBBox("B", barWidth/2./*(bbX/barnum)/2-bargap*/, hthick, fabs(len1)/2.); // lside -> bbX
      
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
     TGeoBBox*   lBox2        = new TGeoBBox("B2", barWidth/2./*(bbX/barnum)/2-bargap*/, hthick, fabs(len2)/2.); // lside -> bbX

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
  TGeoBBox*   lTop = new TGeoBBox(400,400,600);
  cave = new TGeoVolume("DIRC", lTop, air_m);
  gGeoManager->SetTopVolume(cave);
  
  // create pre-top volume:
  TGeoVolume* vLocalMother;
  TGeoPcon*   shape = new TGeoPcon("BarrelDIRCShape", 0, 360., 4);
  shape->DefineSection(0, bbox_zdown, 35., 85.);
  shape->DefineSection(1, bbox_zup, 35., 85.);
  shape->DefineSection(2, bbox_zup - sob_len, 35., sob_Rout+poffset+pheight+EVoffset+PDHeight+GlueHeight);
  shape->DefineSection(3, bbox_zup - sob_len - PDthick - GlueLayer-eps, 35., sob_Rout+poffset+pheight+EVoffset+PDHeight+GlueHeight);
  vLocalMother = new TGeoVolume("BarrelDIRC", shape, air_m); //DIRCairNoSens_m); //################
  cave->AddNode(vLocalMother, 0,0);
 

  cout<<"bbox length = "<<2.*(bbox_hlen-0.5*(boxgap+boxthick))<<", prizm length = "<<2.*(phlength+0.5*(boxgap+boxthick))<<endl;
  cout<<"prizm shift = "<<-bbox_hlen+0.5*(boxgap+boxthick)-phlength<<endl;
  
  // create BarBoxes:  
  TGeoBBox* logicbbL;
  TGeoVolume *bbox;
  if(fprizm == kFALSE){
    logicbbL = new TGeoBBox("logicbbL", barWidth/2.+boxgap+boxthick, (2.*hthick+boxgap+boxthick+EVdrop+EVoffset)/2., bbox_hlen);
    TGeoTrap*logicSepEVBox = new TGeoTrap("logicSepEVBox", sob_len/2., 
    sob_angle/2.,
    270., 
    (2.*hthick+boxgap+boxthick+EVdrop+EVoffset +sob_len*tan(sob_angle*pi/180.))/2., 
    EVwidth/2.+boxthick+eps,
    EVwidth/2.+boxthick+eps,
    0.,				//7    
    (2.*hthick+boxgap+boxthick+EVdrop+EVoffset)/2., 
    EVwidth/2.+boxthick+eps,
    EVwidth/2.+boxthick+eps,
    0.);
    TGeoTranslation* trseb = new TGeoTranslation("trseb", 0., sob_len/2.*tan(sob_angle*pi/180./2.), -bbox_hlen-sob_len/2.);	
    trseb->RegisterYourself();
    TGeoCompositeShape* bboxL = new TGeoCompositeShape("bboxL", "logicbbL + logicSepEVBox:trseb");
    	
    bbox = new TGeoVolume("DrcBarBox", bboxL,DIRCcarbonFiber_m); 
  } 
  bbox->SetLineColor(4);//(30);
  
  
  TGeoBBox* logicbbS;
  TGeoVolume *abox;
  if(fprizm == kFALSE){
    logicbbS = new TGeoBBox("logicbbS", barWidth/2.+boxgap, (2.*hthick+boxgap+EVdrop+EVoffset)/2., bbox_hlen);
    TGeoTrap* logicSepEVContainer = new TGeoTrap("logicSepEVContainer", sob_len/2., 
      sob_angle/2.,
      270.,
      (2.*hthick+boxgap+EVdrop+EVoffset +sob_len*tan(sob_angle*pi/180.))/2., 
      EVwidth/2.+eps, 
      EVwidth/2.+eps,  
      0,				//7    
      (2.*hthick+boxgap+EVdrop+EVoffset)/2., 
      EVwidth/2.+eps,
      EVwidth/2.+eps, 
      0.);			//11)    
    TGeoCompositeShape* bboxS = new TGeoCompositeShape("bboxS","logicbbS + logicSepEVContainer:trseb");
    //TGeoCompositeShape* bboxS = new TGeoCompositeShape("bboxS","logicSepEVContainer:trseb");
    abox = new TGeoVolume("DrcAirBox", bboxS, DIRCairNoSens_m);      
    bbox->AddNode(abox, 0, new TGeoCombiTrans(0., 0., 0., new TGeoRotation(0)));
  }  
  abox->SetLineColor(2);//(19);
  
  // separated EVs put into airBoxes:
  TGeoTrap* logicEVs = new TGeoTrap("logicEVs", sob_len/2.,
      sob_angle/2.,
      270.,
      (2.*hthick+EVdrop+EVoffset +sob_len*tan(sob_angle*pi/180.))/2.,
      EVwidth/2.,
      EVwidth/2.,
      0,
      (2.*hthick+EVdrop+EVoffset)/2., 
      EVwidth/2.,
      EVwidth/2., 
      0.);
   TGeoVolume *smallEVs = new TGeoVolume("DrcEVSensor", logicEVs, FusedSil_m);
   smallEVs->SetLineColor(kMagenta+2);
   smallEVs->SetTransparency(40);
     
     
  //}
  // put barboxes and sep. EV into right positions:    
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
  cout<<"bar width = "<<barWidth/2./*2.*(((bbX/barnum)/2.)-bargap)*/ << ", bar with gaps = "<<(bbX/barnum)<<endl;
  cout<<"barboxL width = "<<bbX/2.+boxgap+boxthick<<", barboxS width = "<<bbX/2.+boxgap<<endl;
  cout<<"x="<<(((bbX/barnum)/2.)-bargap)*2<<", y="<<hthick*2<<", z="<<(bbox_hlen-fabs(len)/2.-mirr_hthick)*2<<endl<<endl;
  
  // create logic bar:  
  TGeoBBox* logicBar = new TGeoBBox("logicBar",  barWidth/2./*((bbX/barnum)/2.)-bargap*/, hthick, bbox_hlen-fabs(len)/2.-mirr_hthick);
  TGeoVolume* bar;
  if(fprizm == kFALSE){  
    bar = new TGeoVolume("DrcBarSensor",logicBar, FusedSil_m );
    //bar = new TGeoVolume("DrcBarSensor",logicBar );
  } 
  bar->SetLineColor(kCyan-9);
  bar->SetTransparency(60);
  
  // create logic mirror:
  TGeoBBox* logicMirror  = new TGeoBBox("logicMirror", barWidth/2./*bbX/barnum/2.-bargap*/, hthick, mirr_hthick);
  TGeoVolume *mirr  = new TGeoVolume("DrcMirr", logicMirror,  Mirror_m);
  mirr->SetLineColor(5);
  
  Double_t dx, dy, dz_bar, dz_mirr;
  
  for(Int_t j=0; j<barnum; j++){
    dx      = - (bbX/2.) + (bbX/barnum)/2. + j * (bbX/barnum); 
    dy  = 0.;//len/2. - mirr_hthick;
    if(fprizm == kFALSE){
      dz_bar = fabs(len)/2.-mirr_hthick;
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
      if(fFocusingSystem == 0){
        //abox->AddNode(grBar, 1, new TGeoCombiTrans(dx, dy, fdz_grease, new TGeoRotation(0)));
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
    abox->AddNode(smallEVs, 1+j, new TGeoCombiTrans(dx,dy + sob_len/2.*tan(sob_angle*pi/180./2.),-bbox_hlen-sob_len/2.,new TGeoRotation(0)));       
    abox->AddNode(bar,  1+j, new TGeoCombiTrans(dx, dy, dz_bar, new TGeoRotation(0)));
    if(fFocusingSystem != 2){ // not a forward mirror
      abox->AddNode(mirr, 1+j, new TGeoCombiTrans(dx, dy, dz_mirr, new TGeoRotation(0)));
    }
  }
  
  // PhotoDetector:
  Double_t dR; 
  Double_t xEV;
  Double_t cosFactor1; 

  TGeoPgon* logicEV1, * logicEV2, *logicEV3, * logicEV4;
  cosFactor1 = cos(pipehAngle/180.*pi)/cos(dphi/180.*pi/2.);
    
  TGeoPgon *logicPD1 = new TGeoPgon("logicPD1",  93.6, 172.8, bbnum/2, 2);
  TGeoPgon *logicPD2 = new TGeoPgon("logicPD2", -86.4, 172.8, bbnum/2, 2);
  TGeoPgon *logicPD3 = new TGeoPgon("logicPD3",  86.4,   7.2,       1, 2);
  TGeoPgon *logicPD4 = new TGeoPgon("logicPD4", -93.6,   7.2,       1, 2);
   
  if(fprizm == kFALSE){
    if(sob_angleB == 90.){  
      cout<<"sob_angleB == 90"<<endl;     
      logicPD1->DefineSection(0, 0.,  radius-hthick-EVdrop, sob_Rout+EVoffset);
      logicPD1->DefineSection(1, PDthick,  radius-hthick-EVdrop, sob_Rout+EVoffset);           
      logicPD2->DefineSection(0, 0.,  radius-hthick-EVdrop, sob_Rout+EVoffset);
      logicPD2->DefineSection(1, PDthick,  radius-hthick-EVdrop, sob_Rout+EVoffset);           
      logicPD3->DefineSection(0, 0., (radius-hthick-EVdrop)*cosFactor1, (sob_Rout+EVoffset)*cosFactor1);
      logicPD3->DefineSection(1, PDthick, (radius-hthick-EVdrop)*cosFactor1, (sob_Rout+EVoffset)*cosFactor1);
      logicPD4->DefineSection(0, 0., (radius-hthick-EVdrop)*cosFactor1, (sob_Rout+EVoffset)*cosFactor1);
      logicPD4->DefineSection(1, PDthick, (radius-hthick-EVdrop)*cosFactor1, (sob_Rout+EVoffset)*cosFactor1);
          
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
  pd->SetLineColor(kGreen);//-6);
   
   // Small PD - MCPs
  TGeoBBox* logicMcp = new TGeoBBox("logicMcp", 5.3/2., 5.3/2., PDthick/2.);
  TGeoBBox* logicMcpGlue = new TGeoBBox("logicMcpGlue", 5.3/2., 5.3/2., GlueLayer/2.);
  TGeoTranslation* trmcp[9];
  TGeoTranslation* trmcpg[9];
  for(Int_t itr=0; itr<3; itr++){
    Double_t dy_row = MCPside/2. - (hthick+EVdrop) + itr*(MCPside + PDgap1);
    for(Int_t jtr=0; jtr<3; jtr++){
      Double_t dx_row = (EVwidth/2./*((bbX/barnum)/2.)-bargap*/) - MCPside/2. - jtr*(MCPside + PDgap2);
      TString trname = Form("trmcp%d", itr*3+jtr);
      TString trgname = Form("trmcpg%d", itr*3+jtr);
      trmcp[itr*3 + jtr] = new TGeoTranslation(trname, dx_row, dy_row, dz_bbox-bbox_hlen-sob_len-GlueLayer-PDthick/2.);
      trmcp[itr*3 + jtr]->RegisterYourself();
      trmcpg[itr*3 + jtr] = new TGeoTranslation(trgname, dx_row, dy_row, dz_bbox-bbox_hlen-sob_len-GlueLayer/2.);
      trmcpg[itr*3 + jtr]->RegisterYourself();
      cout<<"translation "<<trmcp[itr*3 + jtr]->GetName()<<endl;
    }
  }
  TGeoCompositeShape *logicMcpMap = new TGeoCompositeShape("logicMcpMap","logicMcp:trmcp0 + logicMcp:trmcp1 + logicMcp:trmcp2 + logicMcp:trmcp3 + logicMcp:trmcp4 + logicMcp:trmcp5 + logicMcp:trmcp6 + logicMcp:trmcp7 + logicMcp:trmcp8") ;
  TGeoVolume* Mcp = new TGeoVolume("DrcPDSensor", logicMcpMap, FusedSil_m);
  Mcp->SetLineColor(kGreen+3);
  
  TGeoCompositeShape *logicMcpGlueMap = new TGeoCompositeShape("logicMcpGlueMap", "logicMcpGlue:trmcpg0 + logicMcpGlue:trmcpg1 + logicMcpGlue:trmcpg2 + logicMcpGlue:trmcpg3 + logicMcpGlue:trmcpg4 + logicMcpGlue:trmcpg5 + logicMcpGlue:trmcpg6 + logicMcpGlue:trmcpg7 + logicMcpGlue:trmcpg8");
  TGeoVolume* McpGlue = new TGeoVolume("DrcMcpGlue", logicMcpGlueMap, OpticalGrease_m);
  McpGlue->SetLineColor(4);//(kCyan);//-9);
  McpGlue->SetTransparency(40);
 
  if(sob_angleB == 90.){       
   for(Int_t m = 0; m < bbnum; m ++){       
    phi_curr = (90. - phi0 - dphi*m)/180.*pi;    
    if(m > bbnum/2-1){ phi_curr = (90. - phi0 - dphi*m - 2.*pipehAngle)/180.*pi; }
    dx_bbox = radius * cos(phi_curr);
    dy_bbox = radius * sin(phi_curr);
    
    TGeoRotation rot_bbox;    
    rot_bbox.RotateZ( -phi0 - m*dphi - (TMath::Floor(2.*m/bbnum))*(2.*pipehAngle));    
    TGeoRotation rot_EV;    
    rot_EV.RotateZ(-90.-phi0 - m*dphi - (TMath::Floor(2.*m/bbnum))*(2.*pipehAngle)); 
    
    vLocalMother->AddNode(Mcp, m, new TGeoCombiTrans(dx_bbox, dy_bbox, 0., new TGeoRotation(rot_bbox))); 
    
    // put glue under each Mcp:
    //vLocalMother->AddNode(McpGlue, m, new TGeoCombiTrans(dx_bbox, dy_bbox, 0., new TGeoRotation(rot_bbox)));
       
    //vLocalMother->AddNode(bbox, m+1, new TGeoCombiTrans(dx_bbox, dy_bbox, dz_bbox, new TGeoRotation(rot_bbox)));    
  } 
    
    
  }
  if(sob_angleB != 90.){
    //vLocalMother->AddNode(pd, 1, new TGeoCombiTrans(0., 0., sob_shift, new TGeoRotation(0)));
    //vLocalMother->AddNode(gluePD, 1, new TGeoCombiTrans(0., 0., sob_shift, new TGeoRotation(0))); 
  }
  //Origin
  //vLocalMother->AddNode(new TGeoVolume("Origin",new TGeoTube(10,10,0)),1);

  gGeoManager->CloseGeometry();

  cave->CheckOverlaps(0.1, "");
  gGeoManager->CheckOverlaps(0.001); // [cm]
  //gGeoManager->CheckGeometryFull();
  
  TObjArray *listOfOverlaps = gGeoManager->GetListOfOverlaps();
  cout<<listOfOverlaps->GetEntries()<<endl;
  listOfOverlaps->Print();
  
  cave->Write();
  fi->Close();
 //  McpGlue->Draw("ogl");
  cave->Draw("ogl");
 
}
