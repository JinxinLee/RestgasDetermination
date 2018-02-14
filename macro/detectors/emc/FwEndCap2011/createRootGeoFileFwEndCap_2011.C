  // This macro creates the Forward EndCap geometry of the EMC detector for PANDA. 
  //it contains the crystals as well as the alveoles. Alveoles are added in two lines; if you don't need them in the geometry, just comment these two lines as follows:
  //(1) SubunitVol->AddNode(AlveoleVol_subunit,0,0);
  //(2) BoxVol->AddNode(AlveoleVol_box,0,0);
  
  // The tapered crystals with the size of
  // 24.375 x 24.375 x 200 mm^3 (front side) and 
  // 26.0 x 26.0 x 200 mm^3 (back side) are introduced.
  //
  // in the Henk's geometry, target is 2037 mm away from the front of the crystals. The thicknes of the top assembly in this code is around 230 mm (measured in the geometry).
  // Thus, the distance from the target point to the center of thetop assembly will be around 2037+230/2=2152 mm.
  // This is the number which should be inserted for the FwEndCap module distance in the PndEmc.cxx
  // (subunits are more or less oriented toward the off-point, which is 963 mm away from the target point.)
  //
  // Author: Hossein Moeini, KVI (2011)

  //-----------------------------
  // 4*24.374+0.36+2*(0.6+0.18+0.24+0.24)=100.376 mm (Henk geometry of a Subunit made using UGS NX 5)
  //
#include <string>

int createRootGeoFileFwEndCap_2011()
{
  TH1F *HISTOsubunitTheta = new TH1F("HISTOsubunitTheta","Subunit #theta",1250,4.5,17.);//0.01 degree resolution of the histogram
  TH1F *HISTOoffpointToCrystalMidpointAlongBeam_calculatedForEachSubunit = new TH1F("HISTOoffpointToCrystalMidpointAlongBeam_calculatedForEachSubunit","Along-the-beam prjection of the distance between offpoint and the crystal midpoint",640,316.5,322.9);//cm
  TH1F *HISTOoffpointToFwEndCap_calculatedForEachSubunit = new TH1F("HISTOoffpointToFwEndCap_calculatedForEachSubunit","Distance between offpoint and the midpoint of the FwEndCap",640,316.5,322.9);//0.1 mm resolution of the histogram
  TH2F *HISTOsubunitTheta_offpointToFwEndCap = new TH2F("HISTOsubunitTheta_offpointToFwEndCap","Distance between offpoint and the midpoint of the FwEndCap vs subunit #theta ",1250,4.5,17.,640,316.5,322.9);

  Double_t  kSpaceInBox    = 0.06;    // Space in Box (cm)
  Double_t  kSpaceInSub    = 0.024;   // Space in Subunit (cm)
  Double_t  kSpaceSubGlue  = 0.036;   // Thickness of glued alveole between Boxes in Subunit (cm)
  Double_t  kAlveoleThickness = 0.018;// Thickness of alveole between Subunits (cm)

  Double_t aa = 2.4375;   // size of the crystal's front side in cm
  Double_t bb = 2.6;      // size of the crystal's back side in cm
  Double_t tr = 2.51875;  // size of the crystal's profile at its center in cm ----> (aa+bb)/2

  Double_t b1 = bb-tr/2.;
  Double_t b2 = aa-tr/2.;

  Double_t vert[16],vertBox[16],vertSub[16],vertSub_Subtracted[16],vertHalfSub[16],vertHalfSub_Subtracted[16]; // x-y coordinates of the 8 vertices for TGeoArb8 shapes of Crystal, Box, Subunit, and half Subunit
  Double_t dz = 10.0; //half length of crystals in cm

  // first four vertices (back-face)
  vert[0]  = -tr/2.;
  vert[1]  = -tr/2.;
  vert[2]  = -tr/2.;
  vert[3]  = +b1;
  vert[4]  = +b1;
  vert[5]  = +b1;
  vert[6]  = +b1;
  vert[7]  = -tr/2.;   
  // second four vertices (front-face)
  vert[8]  = -tr/2.;
  vert[9]  = -tr/2.;
  vert[10] = -tr/2.;
  vert[11] = +b2; 
  vert[12] = +b2;
  vert[13] = +b2;
  vert[14] = +b2;
  vert[15] = -tr/2.;
   
  // creating a SIZE for the BOX (4 crystals) shape
  for (Int_t i=0; i< 16; i++){ //setting x and y components of the box vertices (with respect to the center of the box) starting from the lower left vertex of the back-face
    Double_t spaces = kSpaceInBox/2.;
    if (i==0 || i==1 || i==2 || i==7)
	vertBox[i] = (-bb - spaces);
    else if (i==8 || i==9 || i==10 || i==15)
	vertBox[i] = (-aa - spaces);
    else if (i==3 || i==4 || i==5 || i==6)
	vertBox[i] = (bb + spaces);
    else if (i==11 || i==12 || i==13 || i==14)
	vertBox[i] = (aa + spaces);
  }

  // creating a SIZE for the SUBUNIT (16 crystals, 4 boxes) shape
  Double_t spacesSub = kSpaceSubGlue/2. + kAlveoleThickness + 2*kSpaceInSub;
  for (Int_t i=0; i< 16; i++){
    if (i==0 || i==1 || i==2 || i==7 || i==8 || i==9 || i==10 || i==15) {
      vertSub[i] = (vertBox[i]*2 - spacesSub);
      vertSub_Subtracted[i] = vertSub[i] + kAlveoleThickness;
    }
    else {
      vertSub[i] = (vertBox[i]*2 + spacesSub);
      vertSub_Subtracted[i] = vertSub[i] - kAlveoleThickness;
    }
  }

  // creating a SIZE for the half SUBUNIT (8 crystals, 2 boxes) shape
  for (Int_t i=0; i< 16; i++){
    Double_t spacesHalfSub = kSpaceInBox/2. + kAlveoleThickness + kSpaceInSub;
    if (i==0 || i==2 || i==8 || i==10) {
      vertHalfSub[i] = (2*vertBox[i] - spacesSub);
      vertHalfSub_Subtracted[i] = vertHalfSub[i] + kAlveoleThickness; 
    }
    else if (i==4 || i==6 || i==12 || i==14) {
      vertHalfSub[i] = (2*vertBox[i] + spacesSub);
      vertHalfSub_Subtracted[i] = vertHalfSub[i] - kAlveoleThickness;
    }
    else if (i==1 || i==7) {
      vertHalfSub[i] = (-bb - spacesHalfSub);
      vertHalfSub_Subtracted[i] = vertHalfSub[i] + kAlveoleThickness;
    }
    else if (i==3 || i==5) {
      vertHalfSub[i] = (bb + spacesHalfSub);
      vertHalfSub_Subtracted[i] = vertHalfSub[i] - kAlveoleThickness;
    }
    else if (i==9 || i==15) {
      vertHalfSub[i] = (-aa - spacesHalfSub);
      vertHalfSub_Subtracted[i] = vertHalfSub[i] + kAlveoleThickness;
    }
    else {
      vertHalfSub[i] = (aa + spacesHalfSub);
      vertHalfSub_Subtracted[i] = vertHalfSub[i] - kAlveoleThickness;
    }
  }

  //--------------------------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  // Get env variable value (VMC working directory)
  std::string vmcWorkdir = getenv("VMCWORKDIR");

  // Open the output file, to write geometry
  TFile* fi = new TFile( (vmcWorkdir + "/geometry/emc_module3_2011_new.root").c_str(),
			 "RECREATE");
   
  FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  geoFace->setMediaFile( (vmcWorkdir + "/geometry/media_pnd.geo").c_str());
  geoFace->readMedia();
  geoFace->print();

  FairGeoMedia *Media =  geoFace->getMedia();
  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();

  FairGeoMedium *CbmMediumAir = Media->getMedium("air");
  FairGeoMedium *CbmMediumPWO = Media->getMedium("PWO");
  FairGeoMedium *CbmMediumCarbon = Media->getMedium("carbon");
  //FairGeoMedium *CbmMediumAluminium = Media->getMedium("aluminium");  
  Int_t nmed=geobuild->createMedium(CbmMediumAir);
  nmed=geobuild->createMedium(CbmMediumPWO);
  nmed=geobuild->createMedium(CbmMediumCarbon);
  //nmed=geobuild->createMedium(CbmMediumAluminium);

  //TGeoVolume *top = gGeoMan->MakeTube("top",gGeoMan->GetMedium("air"),0,98,22.5);
  TGeoVolume *top = new TGeoVolumeAssembly("Emc3");
  TGeoManager* gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  gGeoMan->SetTopVolume(top);

  TGeoTranslation trSub;
  TGeoTranslation trHalfSub;
  TGeoTranslation trBox; 
  TGeoTranslation trCrystal;
  TGeoTranslation trMiddleColumnSubunit;

  TGeoRotation rotSub;
  TGeoRotation rotHalfSub;
  TGeoRotation rotBox;
  TGeoRotation rotCrystal;
  TGeoRotation rotMiddleColumnSubunit;

  TString name, medium;

    // ========== SUBUNIT (16 crystals) ============
    name = "SubunitShape";
    TGeoShape* SubunitShape = new TGeoArb8(name,dz,vertSub); 

    medium="air";
    name = "SubunitVolFwEndCap";
    //SubunitVol[q] = new TGeoVolume(name,SubunitShape,gGeoMan->GetMedium(medium));
    TGeoVolume* SubunitVol = new TGeoVolumeAssembly(name);

    // ========== half SUBUNIT (8 crystals) ============
    name = "HalfSubunitShape";
    TGeoShape* HalfSubunitShape = new TGeoArb8(name,dz,vertHalfSub);

    name = "HalfSubunitVolFwEndCap";
    //HalfSubunitVol[q] = new TGeoVolume(name,HalfSubunitShape,gGeoMan->GetMedium(medium));
    TGeoVolume* HalfSubunitVol = new TGeoVolumeAssembly(name);

    // ========== BOX (4 crystals) =============
    name = "BoxShape";
    TGeoShape* BoxShape = new TGeoArb8(name,dz,vertBox);

    name="BoxVol";
    TGeoVolume* BoxVol = new TGeoVolume(name,BoxShape,gGeoMan->GetMedium(medium));
    //BoxVol[q] = new TGeoVolumeAssembly(name);
    
    // ========== Alveole package =============   
    TGeoShape* Subunit_Subtracted = new TGeoArb8("Subunit_Subtracted",dz,vertSub_Subtracted); 
    TGeoShape* MiddleWall_subunit = new TGeoTrd1("MiddleWall_subunit",fabs(vertSub_Subtracted[0]),fabs(vertSub_Subtracted[8]),kSpaceSubGlue/2.,dz);
    TGeoRotation* rotSubunitWall = new TGeoRotation("rotSubunitWall");   
    rotSubunitWall->RotateZ(90.);
    rotSubunitWall->RegisterYourself();
    TGeoCompositeShape* AlveoleShape_subunit = new TGeoCompositeShape("AlveoleShape_subunit","(SubunitShape - Subunit_Subtracted) + (MiddleWall_subunit) + (MiddleWall_subunit:rotSubunitWall)");
    TGeoVolume* AlveoleVol_subunit = new TGeoVolume("AlveoleVol_subunit",AlveoleShape_subunit,gGeoMan->GetMedium("carbon"));
    AlveoleVol_subunit->SetLineColor(6);

    TGeoShape* HalfSubunit_Subtracted = new TGeoArb8("HalfSubunit_Subtracted",dz,vertHalfSub_Subtracted); 
    TGeoShape* MiddleWall_halfsubunit = new TGeoTrd1("MiddleWall_halfsubunit",fabs(vertHalfSub_Subtracted[1]),fabs(vertHalfSub_Subtracted[9]),kSpaceSubGlue/2.,dz);
    TGeoCompositeShape* AlveoleShape_halfsubunit = new TGeoCompositeShape("AlveoleShape_halfsubunit","(HalfSubunitShape - HalfSubunit_Subtracted) + (MiddleWall_halfsubunit:rotSubunitWall)");
    TGeoVolume* AlveoleVol_halfsubunit = new TGeoVolume("AlveoleVol_halfsubunit",AlveoleShape_halfsubunit,gGeoMan->GetMedium("carbon"));
    AlveoleVol_halfsubunit->SetLineColor(6);
    
    TGeoShape* MiddleWall_box = new TGeoTrd1("MiddleWall_box",fabs(vertBox[0])+kSpaceInSub,fabs(vertBox[8])+kSpaceInSub,kAlveoleThickness/2.,dz);
    TGeoCompositeShape* AlveoleShape_box = new TGeoCompositeShape("AlveoleShape_box","(MiddleWall_box) + (MiddleWall_box:rotSubunitWall)");
    TGeoVolume* AlveoleVol_box = new TGeoVolume("AlveoleVol_box",AlveoleShape_box,gGeoMan->GetMedium("carbon"));
    AlveoleVol_box->SetLineColor(6);

// making the subunits; translations and rotations for Boxes are with respect to the coordinate system in the center of the Subunit:
    Double_t boxRotAngle = 0.465518;// = atan[24.375/3000] (deg.)----> atan[24.375/320]==0.436424
    Double_t boxShiftInSubunit = (tr+kSpaceInSub+kSpaceSubGlue/2.+kSpaceInBox/2.)*cos(boxRotAngle*TMath::Pi()/180);
      cout << "     " << endl;
	trBox = new TGeoTranslation(boxShiftInSubunit,boxShiftInSubunit, 0.);
	rotBox = new TGeoRotation();
	rotBox.RotateX(boxRotAngle);  // rotation in "up" direction
	rotBox.RotateY(-boxRotAngle); // rotation in "right" direction 
        TGeoCombiTrans* trrotBox= new TGeoCombiTrans(trBox,rotBox);
        name="BoxVol";
        name+=1;
        trrotBox->SetName(name);
        trrotBox->RegisterYourself();
        SubunitVol->AddNode(BoxVol,1,trrotBox);

	trBox = new TGeoTranslation(-boxShiftInSubunit,-boxShiftInSubunit, 0.);
	rotBox = new TGeoRotation();
	rotBox.RotateX(-boxRotAngle); // rotation in "down" direction
	rotBox.RotateY(boxRotAngle);  // rotation in "left" direction
        TGeoCombiTrans* trrotBox= new TGeoCombiTrans(trBox,rotBox);
        name="BoxVol";
        name+=2;
        trrotBox->SetName(name);
        trrotBox->RegisterYourself();
        SubunitVol->AddNode(BoxVol,2,trrotBox);

	trBox = new TGeoTranslation(boxShiftInSubunit,-boxShiftInSubunit, 0.);
	rotBox = new TGeoRotation();
	rotBox.RotateX(-boxRotAngle); // rotation in "down" direction
	rotBox.RotateY(-boxRotAngle); // rotation in "right" direction
        TGeoCombiTrans* trrotBox= new TGeoCombiTrans(trBox,rotBox);
        name="BoxVol";
        name+=3;
        trrotBox->SetName(name);
        trrotBox->RegisterYourself();
        SubunitVol->AddNode(BoxVol,3,trrotBox);

	trBox = new TGeoTranslation(-boxShiftInSubunit,boxShiftInSubunit, 0.);
	rotBox = new TGeoRotation();
	rotBox.RotateX(boxRotAngle); // rotation in "up" direction
	rotBox.RotateY(boxRotAngle); // rotation in "left" direction
        TGeoCombiTrans* trrotBox= new TGeoCombiTrans(trBox,rotBox);
        name="BoxVol";
        name+=4;
        trrotBox->SetName(name);
        trrotBox->RegisterYourself();
        SubunitVol->AddNode(BoxVol,4,trrotBox);
	
        SubunitVol->AddNode(AlveoleVol_subunit,0,0);


// making the half Subunit:
      trBox = new TGeoTranslation(boxShiftInSubunit, 0., 0.);
      rotBox = new TGeoRotation();
      rotBox.RotateY(-boxRotAngle); // rotation in "right" direction
      TGeoCombiTrans* trrotBox= new TGeoCombiTrans(trBox,rotBox);
      name="BoxVol";
      name+=1;
      trrotBox->SetName(name);
      trrotBox->RegisterYourself();
      HalfSubunitVol->AddNode(BoxVol,1,trrotBox);

      trBox = new TGeoTranslation(-boxShiftInSubunit, 0., 0.);
      rotBox = new TGeoRotation();
      rotBox.RotateY(boxRotAngle);  // rotation in "left" direction
      TGeoCombiTrans* trrotBox= new TGeoCombiTrans(trBox,rotBox);
      name="BoxVol";
      name+=2;
      trrotBox->SetName(name);
      trrotBox->RegisterYourself();
      HalfSubunitVol->AddNode(BoxVol,2,trrotBox);
      
      //HalfSubunitVol->AddNode(AlveoleVol_halfsubunit,0,0);

    // ========== CRYSTAL (TGeoArb8 shape)=================
    name = "CrystalShape";
    TGeoShape* CrystalShape = new TGeoArb8(name,dz,vert);
    medium="PWO";
    name = "CrystalVol";
    TGeoVolume* CrystalVol = new TGeoVolume(name,CrystalShape,gGeoMan->GetMedium(medium));
    CrystalVol->SetLineColor(4);

// making the Box out of 4 crystals:
      cout << "     " << endl;
	trCrystal = new TGeoTranslation(tr/2.+kSpaceInBox/2.,tr/2.+kSpaceInBox/2.,0.);
	rotCrystal = new TGeoRotation() ; // "right-upper" Crystal
        TGeoCombiTrans* trrotCrystal= new TGeoCombiTrans(trCrystal,rotCrystal);
        name = "CrystalVol";
        name+=1;
        trrotCrystal->SetName(name);
        trrotCrystal->RegisterYourself();
        BoxVol->AddNode(CrystalVol,1,trrotCrystal);

	trCrystal= new TGeoTranslation(-tr/2.-kSpaceInBox/2.,-tr/2.-kSpaceInBox/2.,0.); 
	rotCrystal = new TGeoRotation();  // "left-lower" Crystal
	rotCrystal.RotateZ(180.);
        TGeoCombiTrans* trrotCrystal= new TGeoCombiTrans(trCrystal,rotCrystal);
        name = "CrystalVol";
        name+=2;
        trrotCrystal->SetName(name);
        trrotCrystal->RegisterYourself();
        BoxVol->AddNode(CrystalVol,2,trrotCrystal);

	trCrystal= new TGeoTranslation(-tr/2.-kSpaceInBox/2.,tr/2.+kSpaceInBox/2.,0.); 
	rotCrystal = new TGeoRotation();  // "left-upper" Crystal
	rotCrystal.RotateZ(90.);
        TGeoCombiTrans* trrotCrystal= new TGeoCombiTrans(trCrystal,rotCrystal);
        name = "CrystalVol";
        name+=3;
        trrotCrystal->SetName(name);
        trrotCrystal->RegisterYourself();
        BoxVol->AddNode(CrystalVol,3,trrotCrystal);

	trCrystal= new TGeoTranslation(tr/2.+kSpaceInBox/2.,-tr/2.-kSpaceInBox/2.,0.); 
	rotCrystal = new TGeoRotation() ; // "right-lower" Crystal
	rotCrystal.RotateZ(270.);
        TGeoCombiTrans* trrotCrystal= new TGeoCombiTrans(trCrystal,rotCrystal);
        name = "CrystalVol";
        name+=4;
        trrotCrystal->SetName(name);
        trrotCrystal->RegisterYourself();
        BoxVol->AddNode(CrystalVol,4,trrotCrystal);
	
	BoxVol->AddNode(AlveoleVol_box,0,0);

    //Double_t sizeOfQuar = 96.192; // size of one Quarter (back side) in X and Y directions in cm
                                  // size of one Subunit (back side):  10.688*9 = 96.192 (cm) & 4*2.6+0.288=10.688, in which 0.288=0.036+2*(0.06+0.018+0.024+0.024)
                                  // size of one Subunit (front side): 10.038*9 = 90.342 (cm) & 4*2.4375+0.288=10.038
    //Double_t kSubShift=10.363;  // size of 1 Subunit in its half Z-distance = (10.688+10.038)/2 cm
    //Double_t FrontFaceToOffPoint=295;   // Off_point to front face of crystals (cm)

    Double_t SubRots_x[10] = {0, 1.397, 3.259, 5.121, 6.983, 8.845, 10.707, 12.569, 14.431, 16.293}; // in degrees
    Double_t SubRots_y[9] = {0, 1.862, 3.724, 5.586, 7.448, 9.310, 11.172, 13.034, 14.896}; // in degrees
    
    Double_t SubShifts_x[9][10],SubShifts_y[9][10],SubShifts_z[9][10];
    // below: ri=(xi,yi,zi) are the positions of the center of the full subunits located at i-th column. The dimention of ri indicates the # of full subunits in the i-th column.
    Double_t x0[7] = {0, 0, 0, 0, 0, 0, 0};// cm
    Double_t y0[7] = {28.8265117778573, 39.3493990842636, 49.9182059980705, 60.5427138182193, 71.2329503536249, 81.9992433988977, 92.8522777922782};// cm
    Double_t z0[7] = {20.8922266939356, 20.7643907501524, 20.647680715811, 20.542219840098, 20.4481194927622, 20.3654790465039, 20.2943857720353};// cm
    Double_t x1[7] = {10.4561465784774, 10.457980770291, 10.4587432590363, 10.4584210182162, 10.4569949523523, 10.4544395865992, 10.4507226619133};// cm
    Double_t y1[7] = {28.8415506036169, 39.3698737009689, 49.9440943683685, 60.5739911406306, 71.269589122811, 82.0412134694661, 92.8995464471194};// cm
    Double_t z1[7] = {20.7353467228561, 20.608097645279, 20.4921269616535, 20.3875570856334, 20.2944985110308, 20.2130496945219, 20.1432969499363};// cm
    Double_t x2[6] = {20.9321821237215, 20.9358595992309, 20.9373981752432, 20.9367717976619, 20.9339422773645, 20.9288586697734};// cm
    Double_t y2[6] = {28.8575314144401, 39.3916128487797, 49.9715678752264, 60.6071778753597, 71.3084709234057, 82.0857752642087};// cm
    Double_t z2[6] = {20.5895321756643, 20.4629244166279, 20.3477101063494, 20.2440107183219, 20.1519360485205, 20.0715840968625};// cm
    Double_t x3[7] = {31.435, 31.4374657965934, 31.443004685364, 31.445346474863, 31.4444520826492, 31.4402642313971, 31.4327065190925};// cm
    Double_t y3[7] = {18.37, 28.8744010011301, 39.4145444804275, 50.0005359941237, 60.642165433689, 71.3494695656328, 82.1327855036172};// cm
    Double_t z3[7] = {20.56, 20.4549342199356, 20.3290191826595, 20.2145744178588, 20.111720383633, 20.0205663879042, 19.9412104670771};// cm
    Double_t x4[8] = {41.9539813282223, 41.9698288739971, 41.9814693214709, 41.9888966825685, 41.9920821980654, 41.9909737596839, 41.9854950144576, 41.9755441265108};// cm
    Double_t y4[8] = {7.89830880167419, 18.3818329576968, 28.8921116541933, 39.4386040325354, 50.0309175849214, 60.6788563819023, 71.3924716114887, 82.1821150419249};// cm
    Double_t z4[8] = {20.6160360262256, 20.4682450405479, 20.3316930634579, 20.2065198113991, 20.0928548902714, 19.9908176835001, 19.9005172334079, 19.8220521159127};// cm
    Double_t x5[7] = {52.5394297215891, 52.5592393983295, 52.5738238277316, 52.5831754879774, 52.587258416773, 52.5860074900363, 52.5793273040347};// cm
    Double_t y5[7] = {7.90341507861587, 18.3936742404941, 28.9106213388012, 39.4637346600652, 50.0626411828193, 60.7171647853164, 71.437376767925};// cm
    Double_t z5[7] = {20.5027092994528, 20.3557018043881, 20.2199378244604, 20.0955538082943, 19.9826771540363, 19.8814261134414, 19.7919096791188};// cm
    Double_t x6[6] = {63.1830462065919, 63.2068179821323, 63.2243685753296, 63.2356889146366, 63.2407358379524, 63.239431228515};// cm
    Double_t y6[6] = {7.90874065384292, 18.4060125259458, 28.9298938377538, 39.4898874292926, 50.0956452405859, 60.7570164991038};// cm
    Double_t z6[6] = {20.4007777494568, 20.2546771228153, 20.1197864051635, 19.9962381996988, 19.8841573722027, 19.7836609746115};// cm
    Double_t x7[5] = {73.8949255128963, 73.9226591547656, 73.9432021341159, 73.9565438187476, 73.9626338624611};// cm
    Double_t y7[5] = {7.91427699482984, 18.4188282598441, 28.949898861872, 39.5170214685491, 50.1298783211089};// cm
    Double_t z7[5] = {20.3103490711865, 20.1652779875055, 20.0313453686846, 19.9086794119197, 19.7974021224738};// cm
    Double_t x8[4] = {84.6854762517226, 84.7171712230529, 84.7407366383712, 84.7561603031881};// cm
    Double_t y8[4] = {7.92001718048636, 18.4321056920017, 28.9706121273752, 39.5451040760829};// cm
    Double_t z8[4] = {20.2315188788966, 20.0875994791296, 19.9547098194095, 19.8329731459031};// cm
    
    for (Int_t j=3; j<=9; j++) { //range of j indicates the # of full subunits in the i-th column (in this for loop, columns 0 and 1)
      SubShifts_x[0][j] = x0[j-3];
      SubShifts_y[0][j] = y0[j-3];
      SubShifts_z[0][j] = z0[j-3];
      SubShifts_x[1][j] = x1[j-3];
      SubShifts_y[1][j] = y1[j-3];
      SubShifts_z[1][j] = z1[j-3];      
    }
    for (Int_t j=3; j<=8; j++) { //(in this for loop, column 2)
      SubShifts_x[2][j] = x2[j-3];
      SubShifts_y[2][j] = y2[j-3];
      SubShifts_z[2][j] = z2[j-3];  
    }
    for (Int_t j=2; j<=8; j++) { //(in this for loop, column 3)
      SubShifts_x[3][j] = x3[j-2];
      SubShifts_y[3][j] = y3[j-2];
      SubShifts_z[3][j] = z3[j-2];      
    }    
    for (Int_t j=1; j<=8; j++) { //(in this for loop, column 4)
      SubShifts_x[4][j] = x4[j-1];
      SubShifts_y[4][j] = y4[j-1];
      SubShifts_z[4][j] = z4[j-1];  
    }
     for (Int_t j=1; j<=7; j++) { //(in this for loop, column 5)
      SubShifts_x[5][j] = x5[j-1];
      SubShifts_y[5][j] = y5[j-1];
      SubShifts_z[5][j] = z5[j-1];  
    }
    for (Int_t j=1; j<=6; j++) { //(in this for loop, column 6)
      SubShifts_x[6][j] = x6[j-1];
      SubShifts_y[6][j] = y6[j-1];
      SubShifts_z[6][j] = z6[j-1];  
    }
    for (Int_t j=1; j<=5; j++) { //(in this for loop, column 7)
      SubShifts_x[7][j] = x7[j-1];
      SubShifts_y[7][j] = y7[j-1];
      SubShifts_z[7][j] = z7[j-1];  
    }
    for (Int_t j=1; j<=4; j++) { //(in this for loop, column 8)
      SubShifts_x[8][j] = x8[j-1];
      SubShifts_y[8][j] = y8[j-1];
      SubShifts_z[8][j] = z8[j-1];  
    }
     

  for(Int_t q=0; q<4; q++){ //making the Subunits of the 4 quarters (50 Subunits per quarter)

    Int_t jj=0; // counter running on the number of Subunits
    for (Int_t row=1; row<=9; row++)
      {
       for (Int_t col=1; col<=8; col++)
       {
	Int_t flag=1; // indicating the presence of a Subunit
	
	if (row<3 && col<3)        flag=0; // not having a Subunit
	else if (col==3 && row<2)  flag=0;
	else if (col>7  && row>4)  flag=0;
	else if (col>6  && row>5)  flag=0;
	else if (col>5  && row>6)  flag=0;
	else if (col>4  && row>7)  flag=0;
	else if (col>1  && row>8)  flag=0;
	
	if (flag){

	  jj++;
	  cout<< "QuarterNo " << q+1<< " ....SubunitNo " << jj <<endl;

          Double_t trSub_x, trSub_y, trSub_z, rotSub_x, rotSub_y;
	  if (q==0){             // first quarter (top right)
            trSub_x = SubShifts_x[col][row];
            trSub_y = SubShifts_y[col][row];
	    trSub_z = SubShifts_z[col][row];
	    rotSub_x = SubRots_x[row];
	    rotSub_y = -SubRots_y[col];
	  }
	  else if (q==1){        // second quarter (bottom right)
	    trSub_x = SubShifts_x[col][row];
            trSub_y = -SubShifts_y[col][row];
	    trSub_z = SubShifts_z[col][row];
            rotSub_x = -SubRots_x[row];
	    rotSub_y = -SubRots_y[col];
	  }
	  else if (q==2){        // third quarter (bottom left)
	    trSub_x = -SubShifts_x[col][row];
            trSub_y = -SubShifts_y[col][row];
	    trSub_z = SubShifts_z[col][row];
            rotSub_x = -SubRots_x[row];
	    rotSub_y = SubRots_y[col];
	  }
	  else if (q==3){        // fourth quarter (top left)
	    trSub_x = -SubShifts_x[col][row];
            trSub_y = SubShifts_y[col][row];
	    trSub_z = SubShifts_z[col][row];
            rotSub_x = SubRots_x[row];
	    rotSub_y = SubRots_y[col];
	  }

          rotSub = new TGeoRotation();
	  rotSub.RotateX(rotSub_x);
	  rotSub.RotateY(rotSub_y);
	  trSub = new TGeoTranslation(trSub_x, trSub_y, trSub_z-20.);//the -20 subtraction is done in order to make the z-position of the crystals around 0 in the top assembly volume,
								     //this way shifting the top volume in the PANDA geometry (done in PndEmc.cxx) would be straightforward,
								     //assuming that the center of the top volume coincides more or less with the center of mass of the crystals.
	  TGeoCombiTrans *trrotSub= new TGeoCombiTrans(trSub,rotSub);
	  name="SubunitVolFwEndCap";
	  name+= 100*(q+1)+jj;
	  trrotSub->SetName(name);
	  trrotSub->RegisterYourself();
	
	  top->AddNode(SubunitVol,100*(q+1)+jj,trrotSub);// copy numbering of Subunits from 101 to 150, 201 to 250, 301 to 350, and 401 to 450
	  
		//deriving the orientation of the full subunits and draw the angle and off-point of them
		Double_t theta1, phi1, theta2, phi2, theta3, phi3;
	  	rotSub.GetAngles(theta1, phi1, theta2, phi2, theta3, phi3);
		if (q==0) {
		  HISTOsubunitTheta->Fill(theta3);
		  HISTOoffpointToCrystalMidpointAlongBeam_calculatedForEachSubunit->Fill(sqrt((SubShifts_x[col][row]*SubShifts_x[col][row])+(SubShifts_y[col][row]*SubShifts_y[col][row]))/tan(theta3*TMath::DegToRad()));
		  HISTOoffpointToFwEndCap_calculatedForEachSubunit->Fill(sqrt((SubShifts_x[col][row]*SubShifts_x[col][row])+(SubShifts_y[col][row]*SubShifts_y[col][row]))/tan(theta3*TMath::DegToRad()) + (trSub_z-20.));
		  HISTOsubunitTheta_offpointToFwEndCap->Fill(theta3, sqrt((SubShifts_x[col][row]*SubShifts_x[col][row])+(SubShifts_y[col][row]*SubShifts_y[col][row]))/tan(theta3*TMath::DegToRad()) + (trSub_z-20.));
		}
	}
      }
    }
  }

//making the middle-column Subunits
    for (Int_t index = 1; index <= 2; index++)
     {Int_t coeff = 1;
      if (index == 2)
        coeff = -1;
      for (Int_t middleSubunits = 3; middleSubunits<=9; middleSubunits++)
       {
        trMiddleColumnSubunit = new TGeoTranslation(SubShifts_x[0][middleSubunits], coeff*SubShifts_y[0][middleSubunits], SubShifts_z[0][middleSubunits]-20.);
        rotMiddleColumnSubunit = new TGeoRotation();
        rotMiddleColumnSubunit.RotateX(coeff*SubRots_x[middleSubunits]);
        TGeoCombiTrans* trrotMiddleColumnSubunit = new TGeoCombiTrans(trMiddleColumnSubunit,rotMiddleColumnSubunit);
	
	  name="SubunitVolFwEndCap";
	  name+= 7*(index-1)+(60 + middleSubunits-2);
	  trrotSub->SetName(name);
	  trrotSub->RegisterYourself();
	
        top->AddNode(SubunitVol,7*(index-1)+(60 + middleSubunits-2),trrotMiddleColumnSubunit);//copy numbering from 61 to 74
	
		//deriving the orientation of the full subunits and draw the angle and off-point of them
		Double_t theta1, phi1, theta2, phi2, theta3, phi3;
	  	rotMiddleColumnSubunit.GetAngles(theta1, phi1, theta2, phi2, theta3, phi3);
		if (index == 1) {
		  HISTOsubunitTheta->Fill(theta3);
		  HISTOoffpointToCrystalMidpointAlongBeam_calculatedForEachSubunit->Fill(sqrt((SubShifts_x[0][middleSubunits]*SubShifts_x[0][middleSubunits])+(SubShifts_y[0][middleSubunits]*SubShifts_y[0][middleSubunits]))/tan(theta3*TMath::DegToRad()));
		  HISTOoffpointToFwEndCap_calculatedForEachSubunit->Fill(sqrt((SubShifts_x[0][middleSubunits]*SubShifts_x[0][middleSubunits])+(SubShifts_y[0][middleSubunits]*SubShifts_y[0][middleSubunits]))/tan(theta3*TMath::DegToRad()) +(SubShifts_z[0][middleSubunits]-20.));
		  HISTOsubunitTheta_offpointToFwEndCap->Fill(theta3, sqrt((SubShifts_x[0][middleSubunits]*SubShifts_x[0][middleSubunits])+(SubShifts_y[0][middleSubunits]*SubShifts_y[0][middleSubunits]))/tan(theta3*TMath::DegToRad()) +(SubShifts_z[0][middleSubunits]-20.));
		}
	
       }
     }

//making the middle-region half Subunits
    Double_t middleHalfSubRots_x[9] = {3.724, 3.724, 3.724, 3.724, 0, 0, 0, 0, 0}; // in degrees
    Double_t middleHalfSubRots_y[9] = {0, 1.862, 3.724, 5.586, 7.448, 9.310, 11.172, 13.034, 14.896}; // in degrees
    Double_t HalfSubShifts_x[58] = {-84.6563520970098, -73.8693906775208, -63.1611211867801, -52.521132074127, -41.9393256755252, //corresponding to copy numbers 1-5 (cm)
				    41.9393256755252, 52.521132074127, 63.1611211867801, 73.8693906775208, 84.6563520970098,      //corresponding to copy numbers 6-10
				    99999, //corresponding to copy number 11 which we don't have in the geometry and that's why it's set to a big number (it's not ganna be used anyway)
				    -20.9174269458209, -10.4487681185361, 0, 10.4487681185361, 20.9174269458209, //corresponding to copy numbers 12-16
				    99999, //corresponding to copy number 17 which we don't have in the geometry and that's why it's set to a big number (it's not ganna be used anyway)
				    99999, //corresponding to copy number 18 which we don't have in the geometry and that's why it's set to a big number (it's not ganna be used anyway)
				    -20.9174269458209, -10.4487681185361, 0, 10.4487681185361, 20.9174269458209, //corresponding to copy numbers 19-23
				    99999, //corresponding to copy number 24 which we don't have in the geometry and that's why it's set to a big number (it's not ganna be used anyway)
				    -92.6747786463306, -92.7160730719282, -92.7485342040102, -81.9829059977037, -71.2396466047239, //corresponding to copy numbers 25-29
				    -63.1300146020203, -52.4697812081315, -31.355563839864, -20.8773507855441, 20.8773507855441, 31.355563839864, //corresponding to copy numbers 30-35
				    52.4697812081315, 63.1300146020203, 71.2396466047239, 81.9829059977037, 92.7485342040102, //corresponding to copy numbers 36-40 
				    92.7160730719282, 92.6747786463306, 92.7160730719282, 92.7485342040102, 81.9829059977037, //corresponding to copy numbers 41-45 
				    71.2396466047239, 63.1300146020203, 52.4697812081315, 31.355563839864, 20.8773507855441, -20.8773507855441, //corresponding to copy numbers 46-51 
				    -31.355563839864, -52.4697812081315, -63.1300146020203, -71.2396466047239, -81.9829059977037, -92.7485342040102, -92.7160730719282};
				    
    Double_t HalfSubShifts_y[58] = {0, 0, 0, 0, 0, //corresponding to copy numbers 1-5
				    0, 0, 0, 0, 0, //corresponding to copy numbers 6-10
				    99999, //corresponding to copy number 11 which we don't have in the geometry and that's why it's set to a big number (it's not ganna be used anyway)
				    20.8885295181039, 20.876851374154, 20.8658909883895, 20.876851374154, 20.8885295181039, //corresponding to copy numbers 12-16
				    99999, //corresponding to copy number 17 which we don't have in the geometry and that's why it's set to a big number (it's not ganna be used anyway)
				    99999, //corresponding to copy number 18 which we don't have in the geometry and that's why it's set to a big number (it's not ganna be used anyway)
				    -20.8885295181039, -20.876851374154, -20.8658909883895, -20.876851374154, -20.8885295181039, //corresponding to copy numbers 19-23
				    99999, //corresponding to copy number 24 which we don't have in the geometry and that's why it's set to a big number (it's not ganna be used anyway)
				    0, 10.4475848056181, 20.9494917954074, 50.0632009767915, 60.6888391507159, //corresponding to copy numbers 25-29
				    68.7741496062525, 79.4684133522537, 90.1738532457449, 90.1213466966609, 90.1213466966609, 90.1738532457449, //corresponding to copy numbers 30-35
				    79.4684133522537, 68.7741496062525, 60.6888391507159, 50.0632009767915, 20.9494917954074, //corresponding to copy numbers 36-40  
				    10.4475848056181, 0, -10.4475848056181, -20.9494917954074, -50.0632009767915, //corresponding to copy numbers 41-45 
				    -60.6888391507159, -68.7741496062525, -79.4684133522537, -90.1738532457449, -90.1213466966609, -90.1213466966609, //corresponding to copy numbers 46-51 
				    -90.1738532457449, -79.4684133522537, -68.7741496062525, -60.6888391507159, -50.0632009767915, -20.9494917954074, -10.4475848056181};
				    
    Double_t HalfSubShifts_z[58] = {20.3466200388019, 20.4263813988843, 20.517625616103, 20.6202563339138, 20.7341651712972, //corresponding to copy numbers 1-5
				    20.7341651712972, 20.6202563339138, 20.517625616103, 20.4263813988843, 20.3466200388019, //corresponding to copy numbers 6-10
				    99999, //corresponding to copy number 11 which we don't have in the geometry and that's why it's set to a big number (it's not ganna be used anyway)
				    20.8544382200522, 21.0009144090997, 21.1582196313325, 21.0009144090997, 20.8544382200522, //corresponding to copy numbers 12-16
				    99999, //corresponding to copy number 17 which we don't have in the geometry and that's why it's set to a big number (it's not ganna be used anyway)
				    99999, //corresponding to copy number 18 which we don't have in the geometry and that's why it's set to a big number (it's not ganna be used anyway)
				    20.8544382200522, 21.0009144090997, 21.1582196313325, 21.0009144090997, 20.8544382200522, //corresponding to copy numbers 19-23
				    99999, //corresponding to copy number 24 which we don't have in the geometry and that's why it's set to a big number (it's not ganna be used anyway)
				    20.9979827713258, 20.8465516574732, 20.7052688946296, 20.3581441163706, 20.2547272198587, //corresponding to copy numbers 25-29 
				    20.2322699798398, 20.3302699346598, 20.570496344343, 20.701744222906, 20.701744222906, 20.570496344343, //corresponding to copy numbers 30-35
				    20.3302699346598, 20.2322699798398, 20.2547272198587, 20.3581441163706, 20.7052688946296, //corresponding to copy numbers 36-40  
				    20.8465516574732, 20.9979827713258, 20.8465516574732, 20.7052688946296, 20.3581441163706, //corresponding to copy numbers 41-45 
				    20.2547272198587, 20.2322699798398, 20.3302699346598, 20.570496344343, 20.701744222906, 20.701744222906, //corresponding to copy numbers 46-51 
				    20.570496344343, 20.3302699346598, 20.2322699798398, 20.2547272198587, 20.3581441163706, 20.7052688946296, 20.8465516574732};
    
    
    for (Int_t col=4; col<=8; col++){ //in total 10 horizontal middle-region (equator) half Subunits
      rotHalfSub = new TGeoRotation();
      rotHalfSub.RotateX(middleHalfSubRots_x[col]);
      rotHalfSub.RotateY(-middleHalfSubRots_y[col]);
      trHalfSub = new TGeoTranslation(HalfSubShifts_x[col+1], HalfSubShifts_y[col+1], HalfSubShifts_z[col+1] - 20.);//the -20 subtraction is done in order to make the z-position
																//of the crystals around 0 in the top assembly volume,
      TGeoCombiTrans *trrotHalfSub= new TGeoCombiTrans(trHalfSub,rotHalfSub);
      name="HalfSubunitVolFwEndCap";
      name+= (6+col-4);
      trrotSub->SetName(name);
      trrotSub->RegisterYourself();
      top->AddNode(HalfSubunitVol,6+col-4,trrotHalfSub);// copy numbering from 6 to 10

      rotHalfSub = new TGeoRotation();
      rotHalfSub.RotateX(middleHalfSubRots_x[12-col]);
      rotHalfSub.RotateY(middleHalfSubRots_y[12-col]);
      trHalfSub = new TGeoTranslation(HalfSubShifts_x[col-4], HalfSubShifts_y[col-4], HalfSubShifts_z[col-4] - 20.);
      TGeoCombiTrans *trrotHalfSub= new TGeoCombiTrans(trHalfSub,rotHalfSub);
      name="HalfSubunitVolFwEndCap";
      name+= (9-col);
      trrotSub->SetName(name);
      trrotSub->RegisterYourself();
      top->AddNode(HalfSubunitVol,9-col,trrotHalfSub);// copy numbering from 5 to 1
    }

    for (Int_t col=1; col<=5; col++){ //in total 10 horizontal middle-region half Subunits
      rotHalfSub = new TGeoRotation();
      rotHalfSub.RotateX(middleHalfSubRots_x[0]);
      if (col<=2) { //copy numbering from 12 to 13
        rotHalfSub.RotateY(middleHalfSubRots_y[3-col]);
        trHalfSub = new TGeoTranslation(HalfSubShifts_x[10+col], HalfSubShifts_y[10+col], HalfSubShifts_z[10+col] - 20.);
      }
      else { //copy numbering from 14 to 16
      rotHalfSub.RotateY(-middleHalfSubRots_y[col-3]);
      trHalfSub = new TGeoTranslation(HalfSubShifts_x[10+col], HalfSubShifts_y[10+col], HalfSubShifts_z[10+col] - 20.);
      }
      TGeoCombiTrans *trrotHalfSub= new TGeoCombiTrans(trHalfSub,rotHalfSub);
      name="HalfSubunitVolFwEndCap";
      name+= (col+11);
      trrotSub->SetName(name);
      trrotSub->RegisterYourself();
      top->AddNode(HalfSubunitVol,col+11,trrotHalfSub);// copy numbering from 12 to 16

      rotHalfSub = new TGeoRotation();
      rotHalfSub.RotateX(-middleHalfSubRots_x[0]);
      if (col<=2) { // copy numbering from 19 to 20
      rotHalfSub.RotateY(middleHalfSubRots_y[3-col]);
      trHalfSub = new TGeoTranslation(HalfSubShifts_x[17+col], HalfSubShifts_y[17+col], HalfSubShifts_z[17+col] - 20.);
      }
      else { // copy numbering from 21 to 23
      rotHalfSub.RotateY(-middleHalfSubRots_y[col-3]);
      trHalfSub = new TGeoTranslation(HalfSubShifts_x[17+col], HalfSubShifts_y[17+col], HalfSubShifts_z[17+col] - 20.);
      }
      TGeoCombiTrans *trrotHalfSub= new TGeoCombiTrans(trHalfSub,rotHalfSub);
      name="HalfSubunitVolFwEndCap";
      name+= (18+col);
      trrotSub->SetName(name);
      trrotSub->RegisterYourself();
      top->AddNode(HalfSubunitVol,18+col,trrotHalfSub);// copy numbering from 19 to 23
    }

//making the peripheral half Subunits
    Double_t peripheralHalfSubRots_x[9] = {0, 1.862, 3.724, 8.845, 10.707, 12.103, 13.965, 15.827, 15.827}; // in degrees
    Double_t peripheralHalfSubRots_y[9] = {16.293, 16.293, 16.293, 14.431, 12.569, 11.172, 9.310, 5.586, 3.724}; // in degrees
    for (Int_t index=0; index<=8; index++){ //in total 18 upper peripheral half Subunits
      rotHalfSub = new TGeoRotation();
      if (index<=4)
        rotHalfSub.RotateZ(90);
      rotHalfSub.RotateX(peripheralHalfSubRots_x[index]);
      rotHalfSub.RotateY(peripheralHalfSubRots_y[index]);
      trHalfSub = new TGeoTranslation(HalfSubShifts_x[24+index], HalfSubShifts_y[24+index], HalfSubShifts_z[24+index] - 20.);
      TGeoCombiTrans *trrotHalfSub= new TGeoCombiTrans(trHalfSub,rotHalfSub);
      name="HalfSubunitVolFwEndCap";
      name+= (25+index);
      trrotSub->SetName(name);
      trrotSub->RegisterYourself();
      top->AddNode(HalfSubunitVol,25+index,trrotHalfSub);// copy numbering from 25 to 33 (top left part)

      rotHalfSub = new TGeoRotation();
      if (index<=4)
        rotHalfSub.RotateZ(90);
      rotHalfSub.RotateX(peripheralHalfSubRots_x[index]);
      rotHalfSub.RotateY(-peripheralHalfSubRots_y[index]);
      trHalfSub = new TGeoTranslation(HalfSubShifts_x[41-index], HalfSubShifts_y[41-index], HalfSubShifts_z[41-index] - 20.);
      TGeoCombiTrans *trrotHalfSub= new TGeoCombiTrans(trHalfSub,rotHalfSub);
      name="HalfSubunitVolFwEndCap";
      name+= (42-index);
      trrotSub->SetName(name);
      trrotSub->RegisterYourself();
      top->AddNode(HalfSubunitVol,42-index,trrotHalfSub);// copy numbering from 42 to 34 (top right part)
    }

    for (Int_t index=1; index<=8; index++){ //in total 16 lower peripheral half Subunits
      rotHalfSub = new TGeoRotation();
      if (index<=4)
        rotHalfSub.RotateZ(90);
      rotHalfSub.RotateX(-peripheralHalfSubRots_x[index]);
      rotHalfSub.RotateY(-peripheralHalfSubRots_y[index]);
      trHalfSub = new TGeoTranslation(HalfSubShifts_x[41+index], HalfSubShifts_y[41+index], HalfSubShifts_z[41+index] - 20.);
      TGeoCombiTrans *trrotHalfSub= new TGeoCombiTrans(trHalfSub,rotHalfSub);
      name="HalfSubunitVolFwEndCap";
      name+= (42+index);
      trrotSub->SetName(name);
      trrotSub->RegisterYourself();
      top->AddNode(HalfSubunitVol,42+index,trrotHalfSub);// copy numbering from 43 to 50 (bottom right part)

      rotHalfSub = new TGeoRotation();
      if (index<=4)
        rotHalfSub.RotateZ(90);
      rotHalfSub.RotateX(-peripheralHalfSubRots_x[index]);
      rotHalfSub.RotateY(peripheralHalfSubRots_y[index]);
      trHalfSub = new TGeoTranslation(HalfSubShifts_x[58-index], HalfSubShifts_y[58-index], HalfSubShifts_z[58-index] - 20.);
      TGeoCombiTrans *trrotHalfSub= new TGeoCombiTrans(trHalfSub,rotHalfSub);
      name="HalfSubunitVolFwEndCap";
      name+= (59-index);
      trrotSub->SetName(name);
      trrotSub->RegisterYourself();
      top->AddNode(HalfSubunitVol,59-index,trrotHalfSub);// copy numbering from 58 to 51 (bottom left part)
    }


//overlap checking:
  TGeoNode* myNode = SubunitVol->GetNode(0);//allowed arguments for SubunitVol: 0-4, 4 being for Alveole
  gGeoManager->CheckOverlaps(0.001);// myNode->CheckOverlaps(0.001)
  //cout <<"Overlaps: "<<gGeoManager->PrintOverlaps()<<endl ;

//finally:  
  gGeoMan->CloseGeometry();
  top->Write();
  fi->Close();
  top->Draw();
  
  TCanvas* c0 = new TCanvas("c0","", 10, 20, 1200, 900);
  gStyle->SetOptStat(0);
  c0->SetFillColor(10);
  c0->SetFrameFillColor(10);
  c0->Divide(2,2);
  c0->cd(1);
  HISTOsubunitTheta->Draw();  
  c0->cd(3);
  HISTOsubunitTheta_offpointToFwEndCap->Draw();
  c0->cd(2);
  HISTOoffpointToCrystalMidpointAlongBeam_calculatedForEachSubunit->Draw();
  c0->cd(4);
  HISTOoffpointToFwEndCap_calculatedForEachSubunit->Draw();

  return 0;
}


