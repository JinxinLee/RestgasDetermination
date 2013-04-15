  // This macro creates the Forward EndCap geometry of the EMC detector for PANDA. 
  // The tapered crystals with the size of:
  // 24.375 x 24.375 x 200 mm^3 (front side) and 
  // 26.0 x 26.0 x 200 mm^3 (back side) are introduced.
  //
  // in the NX5 geometry of the drawing office, target is 2005+34=2039 mm away from the front face of the crystals. This is equivalent to 2039+200/2=2139 mm to the mid-point of the FwEndCap.
  // This is the number which should be inserted for the FwEndCap module distance in the PndEmc.cxx
  // (subunits are oriented toward the off-point region, which is about 3088-2139 = 949 mm away from the target point.)
  
  // The distance from the target to the mounting plate is 2350 mm. So the distance from the target to the center of the crystrals is (2350) - (z_position of the subunit)
  // in which z_position of the subunit is the distance of the mid-point of the subunit to the mounting plate.
  // Thus, 2350-2039= 311 mm is the distance between the mounting plate to the front-face of the FwEndCap.
  
  // Autor: Hossein Moeini, KVI (20.04.2012)

  //-----------------------------
  // 4*24.374+0.36+2*(0.6+0.18+0.24+0.24)=100.376 mm (geometry of the Subunit made using UGS NX 5)
  //
#include <string>  
void createRootGeoFileFwEndCap_Sep2012()
{
  TH1F *HISTOsubunitTheta = new TH1F("HISTOsubunitTheta","Subunit #theta",15,4.5,17.5+0.01);//0.5 degree resolution of the histogram
  ///TH1F *HISTOoffpointToCrystalMidpointAlongBeam_calculatedForEachSubunit = new TH1F("HISTOoffpointToCrystalMidpointAlongBeam_calculatedForEachSubunit","Along-the-beam prjection of the distance between offpoint and the crystal midpoint",640,307.7,312.7);//cm
  TH1F *HISTOoffpointToFwEndCap_calculatedForEachSubunit = new TH1F("HISTOoffpointToFwEndCap_calculatedForEachSubunit","Distance between offpoint and the front plane of the FwEndCap",16,305.5,312.4);
  TH2F *HISTOsubunitTheta_offpointToFwEndCap = new TH2F("HISTOsubunitTheta_offpointToFwEndCap","the above vs subunit #theta",15,4.5,17.5+0.01,16,305.5,312.4);
  TH2F *HISTOoffpoint_xy = new TH2F("HISTOoffpoint_xy","off-point x-y profile of 214 subunits;x [cm];y [cm]",60,-0.95,0.95,60,-0.75,0.75);

  /////////////////histograms for the paper:
  TH1F *HISTOoffpointToFronPlane_z = new TH1F("HISTOoffpointToFrontPlane_z",";z [cm];entries",16,305.8,312.2);// 3mm bin size
  TH1F *HISTOoffpoint_x = new TH1F("HISTOoffpoint_x",";x/y [cm];entries",21,-1.05,1.05);// 1mm bin size
  TH1F *HISTOoffpoint_y = new TH1F("HISTOoffpoint_y","",21,-1.05,1.05);// 1mm bin size
  /////////////////
  
  Double_t  kSpaceInBox    = 0.06;    // Space in Box (cm)
  Double_t  kSpaceInSub    = 0.024;   // Space in Subunit (cm)
  Double_t  kSpaceSubGlue  = 0.036;   // Thickness of glued alveole between Boxes in Subunit (cm)
  Double_t  kAlveoleThickness = 0.018;// Thickness of alveole between crystals of a box (cm)
  kAlveoleThickness = 0.; //new value after discussing with the drawing office (11/4/2012)

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
  TFile* fi = new TFile( (vmcWorkdir + "/geometry/emc_module3_2012_new.root").c_str(),"RECREATE");
   
  FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  geoFace->setMediaFile((vmcWorkdir + "/geometry/media_pnd.geo").c_str());
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
    Double_t boxRotAngle = 0.465518;// = atan[24.375/3000] (deg.)
    Double_t boxShiftInSubunit = (tr+kSpaceInSub+kSpaceSubGlue/2.+kSpaceInBox/2.)*cos(boxRotAngle*TMath::Pi()/180);
      cout << "     " << endl;
	trBox = new TGeoTranslation(boxShiftInSubunit,boxShiftInSubunit, 0.);
	rotBox = new TGeoRotation();
	rotBox.RotateX(boxRotAngle);  // rotation in "up" direction
	rotBox.RotateY(-boxRotAngle); // rotation in "right" direction 
        TGeoCombiTrans* trrotBox= new TGeoCombiTrans(trBox,rotBox);
        name="BoxVol";
        name+="1";
        trrotBox->SetName(name);
        trrotBox->RegisterYourself();
        SubunitVol->AddNode(BoxVol,1,trrotBox);

	trBox = new TGeoTranslation(-boxShiftInSubunit,-boxShiftInSubunit, 0.);
	rotBox = new TGeoRotation();
	rotBox.RotateX(-boxRotAngle); // rotation in "down" direction
	rotBox.RotateY(boxRotAngle);  // rotation in "left" direction
        TGeoCombiTrans* trrotBox= new TGeoCombiTrans(trBox,rotBox);
        name="BoxVol";
        name+="2";
        trrotBox->SetName(name);
        trrotBox->RegisterYourself();
        SubunitVol->AddNode(BoxVol,2,trrotBox);

	trBox = new TGeoTranslation(boxShiftInSubunit,-boxShiftInSubunit, 0.);
	rotBox = new TGeoRotation();
	rotBox.RotateX(-boxRotAngle); // rotation in "down" direction
	rotBox.RotateY(-boxRotAngle); // rotation in "right" direction
        TGeoCombiTrans* trrotBox= new TGeoCombiTrans(trBox,rotBox);
        name="BoxVol";
        name+="3";
        trrotBox->SetName(name);
        trrotBox->RegisterYourself();
        SubunitVol->AddNode(BoxVol,3,trrotBox);

	trBox = new TGeoTranslation(-boxShiftInSubunit,boxShiftInSubunit, 0.);
	rotBox = new TGeoRotation();
	rotBox.RotateX(boxRotAngle); // rotation in "up" direction
	rotBox.RotateY(boxRotAngle); // rotation in "left" direction
        TGeoCombiTrans* trrotBox= new TGeoCombiTrans(trBox,rotBox);
        name="BoxVol";
        name+="4";
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
      name+="1";
      trrotBox->SetName(name);
      trrotBox->RegisterYourself();
      HalfSubunitVol->AddNode(BoxVol,1,trrotBox);

      trBox = new TGeoTranslation(-boxShiftInSubunit, 0., 0.);
      rotBox = new TGeoRotation();
      rotBox.RotateY(boxRotAngle);  // rotation in "left" direction
      TGeoCombiTrans* trrotBox= new TGeoCombiTrans(trBox,rotBox);
      name="BoxVol";
      name+="2";
      trrotBox->SetName(name);
      trrotBox->RegisterYourself();
      HalfSubunitVol->AddNode(BoxVol,2,trrotBox);
      
      HalfSubunitVol->AddNode(AlveoleVol_halfsubunit,0,0);

    // ========== CRYSTAL (TGeoArb8 shape) =================
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
	rotCrystal.RotateZ(0.);
        TGeoCombiTrans* trrotCrystal= new TGeoCombiTrans(trCrystal,rotCrystal);
        name = "CrystalVol";
        name+="1";
        trrotCrystal->SetName(name);
        trrotCrystal->RegisterYourself();
        BoxVol->AddNode(CrystalVol,1,trrotCrystal);

	trCrystal= new TGeoTranslation(-tr/2.-kSpaceInBox/2.,-tr/2.-kSpaceInBox/2.,0.); 
	rotCrystal = new TGeoRotation();  // "left-lower" Crystal
	rotCrystal.RotateZ(180.);
        TGeoCombiTrans* trrotCrystal= new TGeoCombiTrans(trCrystal,rotCrystal);
        name = "CrystalVol";
        name+="2";
        trrotCrystal->SetName(name);
        trrotCrystal->RegisterYourself();
        BoxVol->AddNode(CrystalVol,2,trrotCrystal);

	trCrystal= new TGeoTranslation(-tr/2.-kSpaceInBox/2.,tr/2.+kSpaceInBox/2.,0.); 
	rotCrystal = new TGeoRotation();  // "left-upper" Crystal
	rotCrystal.RotateZ(90.);
        TGeoCombiTrans* trrotCrystal= new TGeoCombiTrans(trCrystal,rotCrystal);
        name = "CrystalVol";
        name+="3";
        trrotCrystal->SetName(name);
        trrotCrystal->RegisterYourself();
        BoxVol->AddNode(CrystalVol,3,trrotCrystal);

	trCrystal= new TGeoTranslation(tr/2.+kSpaceInBox/2.,-tr/2.-kSpaceInBox/2.,0.); 
	rotCrystal = new TGeoRotation() ; // "right-lower" Crystal
	rotCrystal.RotateZ(270.);
        TGeoCombiTrans* trrotCrystal= new TGeoCombiTrans(trCrystal,rotCrystal);
        name = "CrystalVol";
        name+="4";
        trrotCrystal->SetName(name);
        trrotCrystal->RegisterYourself();
        BoxVol->AddNode(CrystalVol,4,trrotCrystal);
	
	///BoxVol->AddNode(AlveoleVol_box,0,0); //THERE IS NO ALVEOLE BETWEEN THE CRYSTALS IN A BOX


    Double_t SubRots_x[10] = {0, 1.3965, 3.2585, 5.1205, 6.9825, 8.8445, 10.7065, 12.5685, 14.4305, 16.2925}; // in degrees
    Double_t SubRots_y[9] = {0, 1.862, 3.724, 5.586, 7.448, 9.310, 11.172, 13.034, 14.896}; // in degrees
    
    Double_t SubShifts_x[9][10],SubShifts_y[9][10],SubShifts_z[9][10];
    // below: ri=(xi,yi,zi) are the positions of the center of the full subunits located at i-th column. The size of ri indicates the # of full subunits in the i-th column.
    Double_t x0[7] = {0, 0, 0, 0, 0, 0, 0};// cm
    Double_t y0[7] = {28.8264835787071, 39.3493908944976, 49.9182473789085, 60.5426935698649, 71.2329059529138, 81.9992763955985, 92.8522898715935};// cm
    Double_t z0[7] = {20.8922266939356, 20.7643907501524, 20.647680715811, 20.542219840098, 20.4481194927622, 20.3654790465039, 20.2943857720353};// cm
    Double_t x1[7] = {10.456106625304, 10.4579408171176, 10.4587033058629, 10.4583810650428, 10.4569549991789, 10.4543996334258, 10.4506827087399};// cm
    Double_t y1[7] = {28.8415224044667, 39.3698655112029, 49.9441357492065, 60.5739708922762, 71.2695447220999, 82.0412464661669, 92.8995585264347};// cm
    Double_t z1[7] = {20.7353467228561, 20.608097645279, 20.4921269616535, 20.3875570856334, 20.2944985110308, 20.2130496945219, 20.1432969499363};// cm
    Double_t x2[6] = {20.9322268533826, 20.935904328892, 20.9374429049043, 20.936816527323, 20.9339870070256, 20.9289033994345};// cm
    Double_t y2[6] = {28.8575032152899, 39.3916046590137, 49.9716092560644, 60.6071576270053, 71.3084265226946, 82.0858082609095};// cm
    Double_t z2[6] = {20.5895321756643, 20.4629244166279, 20.3477101063494, 20.2440107183219, 20.1519360485205, 20.0715840968625};// cm
    Double_t x3[7] = {31.4287676468273, 31.4374820747636, 31.4430209635343, 31.4453627530333, 31.4444683608195, 31.4402805095674, 31.4327227972628};// cm
    Double_t y3[7] = {18.3704933873073, 28.8743728019799, 39.4145362906615, 50.0005773749617, 60.6421451853346, 71.3494251649217, 82.1328185003181};// cm    
    Double_t z3[7] = {20.5921880444311, 20.4549342199356, 20.3290191826595, 20.2145744178588, 20.111720383633, 20.0205663879042, 19.9412104670771};// cm
    Double_t x4[8] = {41.9539926281137, 41.9698401738885, 41.9814806213623, 41.9889079824599, 41.9920934979568, 41.9909850595753, 41.985506314349, 41.9755554264022};// cm
    Double_t y4[8] = {7.8983553964663, 18.3818143582945, 28.8920834550431, 39.4385958427694, 50.0309589657594, 60.6788361335479, 71.3924272107776, 82.1821480386256};// cm
    Double_t z4[8] = {20.6160360262256, 20.4682450405479, 20.3316930634579, 20.2065198113991, 20.0928548902714, 19.9908176835001, 19.9005172334079, 19.8220521159127};// cm
    Double_t x5[7] = {52.5394056475252, 52.5592153242656, 52.5737997536677, 52.5831514139135, 52.5872343427091, 52.5859834159724, 52.5793032299708};// cm
    Double_t y5[7] = {7.90346167340798, 18.3936556410918, 28.910593139651, 39.4637264702993, 50.0626825636573, 60.717144536962, 71.4373323672139};// cm
    Double_t z5[7] = {20.5027092994528, 20.3557018043881, 20.2199378244604, 20.0955538082943, 19.9826771540363, 19.8814261134414, 19.7919096791188};// cm
    Double_t x6[6] = {63.1830446557151, 63.2068164312554, 63.2243670244527, 63.2356873637597, 63.2407342870755, 63.2394296776381};// cm
    Double_t y6[6] = {7.90878724863503, 18.4059939265435, 28.9298656386036, 39.4898792395266, 50.0956866214239, 60.7569962507494 };// cm    
    Double_t z6[6] = {20.4007777494568, 20.2546771228153, 20.1197864051635, 19.9962381996988, 19.8841573722027, 19.7836609746115};// cm
    Double_t x7[5] = {73.8949640364187, 73.922697678288, 73.9432406576383, 73.95658234227, 73.9626723859835};// cm    
    Double_t y7[5] = {7.91432358962194, 18.4188096604418, 28.9498706627218, 39.5170132787831, 50.1299197019469};// cm    
    Double_t z7[5] = {20.3103490711865, 20.1652779875055, 20.0313453686846, 19.9086794119197, 19.7974021224738};// cm
    Double_t x8[4] = {84.6854281597051, 84.7171231310355, 84.7406885463538, 84.7561122111708};// cm    
    Double_t y8[4] = {7.92006377527847, 18.4320870925993, 28.970583928225, 39.5450958863169};// cm    
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
	  trSub = new TGeoTranslation(trSub_x, trSub_y, trSub_z-21.);//the -21cm subtraction is done in order to bring the z-position of the mid-point of the hypothetical mid-subunit to around 0.
								     //This way shifting the FwEndCap volume in the PANDA geometry (done in PndEmc.cxx) would be straightforward (2139 mm to target).
								     //
	  TGeoCombiTrans *trrotSub= new TGeoCombiTrans(trSub,rotSub);
	  name="SubunitVolFwEndCap";
	  name+= 100*(q+1)+jj;
	  trrotSub->SetName(name);
	  trrotSub->RegisterYourself();
	
	  top->AddNode(SubunitVol,100*(q+1)+jj,trrotSub);// copy numbering of Subunits from 101 to 150, 201 to 250, 301 to 350, and 401 to 450
	  
		//deriving the orientation of the full subunits and draw the angle and off-point of them
		Double_t theta1, phi1, theta2, phi2, theta3, phi3;
	  	rotSub.GetAngles(theta1, phi1, theta2, phi2, theta3, phi3);
		///if (q==0) {
		  HISTOsubunitTheta->Fill(theta3);
		  ///HISTOoffpointToCrystalMidpointAlongBeam_calculatedForEachSubunit->Fill(sqrt(trSub_x*trSub_x+trSub_y*trSub_y)/tan(theta3*TMath::DegToRad()));
		  ///HISTOoffpointToFwEndCap_calculatedForEachSubunit->Fill(sqrt(trSub_x*trSub_x+trSub_y*trSub_y)/tan(theta3*TMath::DegToRad()) + (21-trSub_z) -10.);//-10cm added here to make the distance of the off-point to the front face of the FwEndCap
		  ///HISTOsubunitTheta_offpointToFwEndCap->Fill(theta3, sqrt(trSub_x*trSub_x+trSub_y*trSub_y)/tan(theta3*TMath::DegToRad()) + (21-trSub_z) -10.);
		  
		  Double_t s0 = ( -trSub_x*cos(theta1*TMath::DegToRad())-trSub_y*cos(theta2*TMath::DegToRad()) )/( cos(theta1*TMath::DegToRad())*cos(theta1*TMath::DegToRad())+cos(theta2*TMath::DegToRad())*cos(theta2*TMath::DegToRad()) );
		  Double_t t0 = (21-trSub_z)+s0*cos(theta3*TMath::DegToRad());
		  Double_t Q0 = (trSub_x+s0*cos(theta1*TMath::DegToRad()))*(trSub_x+s0*cos(theta1*TMath::DegToRad())) + (trSub_y+s0*cos(theta2*TMath::DegToRad()))*(trSub_y+s0*cos(theta2*TMath::DegToRad()))
								      + (21-trSub_z-t0+s0*cos(theta3*TMath::DegToRad()))*(21-trSub_z-t0+s0*cos(theta3*TMath::DegToRad()));
		  ///cout<<"distance between the z-axis and the symmetry axis of this subunit (cm) = "<< sqrt(Q0) << endl;//lowest cm
		  //figuring out the quasi-ellipsoidal region (off-point region) within which we would expect the closest points of the subunits' symmetry axes to the z-axis lie:
		  Double_t R1 = trSub_x+s0*cos(theta1*TMath::DegToRad());
		  Double_t R2 = trSub_y+s0*cos(theta2*TMath::DegToRad());
		  Double_t R3 = (21-trSub_z)+s0*cos(theta3*TMath::DegToRad());
		  //hence, R=(R1,R2,R3) represents such off-point region
		  Double_t R3_abs = fabs(R3); //z-distance between the closest points to the z-axis of the symmetry axis inside the off-point region to the mid-point of the FwEndCap
		  ///cout<<"z-distance between the closest points to the z-axis of the symmetry axis inside the off-point region to the front plane (cm) = "<< R3_abs - 10. << 
			endl;//10 cm is the distance between the mid-point of the FwEndCap and the front plane
		  HISTOoffpointToFwEndCap_calculatedForEachSubunit->Fill(R3_abs - 10.);
		  HISTOsubunitTheta_offpointToFwEndCap->Fill(theta3,R3_abs - 10.);
		  
		  HISTOoffpoint_xy->Fill(R1,R2);
		  ///cout<<"R1,R2= "<<R1<<" ,  "<<R2<<endl;
		  
		  HISTOoffpointToFronPlane_z->Fill(R3_abs - 10.);
		  HISTOoffpoint_x->Fill(R1);
		  HISTOoffpoint_y->Fill(R2);
		///}
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
        trMiddleColumnSubunit = new TGeoTranslation(SubShifts_x[0][middleSubunits], coeff*SubShifts_y[0][middleSubunits], SubShifts_z[0][middleSubunits]-21.);
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
		//theta3=0;
	  	rotMiddleColumnSubunit.GetAngles(theta1, phi1, theta2, phi2, theta3, phi3);
		///if (index == 1) {
		  HISTOsubunitTheta->Fill(theta3);
		  ///HISTOoffpointToCrystalMidpointAlongBeam_calculatedForEachSubunit->Fill(sqrt((SubShifts_x[0][middleSubunits]*SubShifts_x[0][middleSubunits])+(SubShifts_y[0][middleSubunits]*SubShifts_y[0][middleSubunits]))/tan(theta3*TMath::DegToRad()));
		  ///HISTOoffpointToFwEndCap_calculatedForEachSubunit->Fill(sqrt((SubShifts_x[0][middleSubunits]*SubShifts_x[0][middleSubunits])+(SubShifts_y[0][middleSubunits]*SubShifts_y[0][middleSubunits]))/tan(theta3*TMath::DegToRad()) +(21-SubShifts_z[0][middleSubunits]) -10.);
		  ///HISTOsubunitTheta_offpointToFwEndCap->Fill(theta3, sqrt((SubShifts_x[0][middleSubunits]*SubShifts_x[0][middleSubunits])+(SubShifts_y[0][middleSubunits]*SubShifts_y[0][middleSubunits]))/tan(theta3*TMath::DegToRad()) +(21-SubShifts_z[0][middleSubunits]) -10.);
		  
		  Double_t s0 = ( -SubShifts_x[0][middleSubunits]*cos(theta1*TMath::DegToRad())-SubShifts_y[0][middleSubunits]*cos(theta2*TMath::DegToRad()) )/( cos(theta1*TMath::DegToRad())*cos(theta1*TMath::DegToRad())+cos(theta2*TMath::DegToRad())*cos(theta2*TMath::DegToRad()) );
		  Double_t t0 = (21-SubShifts_z[0][middleSubunits])+s0*cos(theta3*TMath::DegToRad());
		  Double_t Q0 = (SubShifts_x[0][middleSubunits]+s0*cos(theta1*TMath::DegToRad()))*(SubShifts_x[0][middleSubunits]+s0*cos(theta1*TMath::DegToRad())) + (SubShifts_y[0][middleSubunits]+s0*cos(theta2*TMath::DegToRad()))*(SubShifts_y[0][middleSubunits]+s0*cos(theta2*TMath::DegToRad()))
								      + (21-SubShifts_z[0][middleSubunits]-t0+s0*cos(theta3*TMath::DegToRad()))*(21-SubShifts_z[0][middleSubunits]-t0+s0*cos(theta3*TMath::DegToRad()));
		  ///cout<<"distance between the z-axis and the symmetry axis of this subunit (cm) = "<< sqrt(Q0) << endl;//lowest cm
		  //figuring out the quasi-ellipsoidal region (off-point region) within which we would expect the closest points of the subunits' symmetry axes to the z-axis lie:
		  Double_t R1 = SubShifts_x[0][middleSubunits]+s0*cos(theta1*TMath::DegToRad());
		  Double_t R2 = SubShifts_y[0][middleSubunits]+s0*cos(theta2*TMath::DegToRad());
		  Double_t R3 = (21-SubShifts_z[0][middleSubunits])+s0*cos(theta3*TMath::DegToRad());
		  //hence, R=(R1,R2,R3) represents such off-point region
		  Double_t R3_abs = fabs(R3); //z-distance between the closest points to the z-axis of the symmetry axis inside the off-point region to the mid-point of the FwEndCap
		  ///cout<<"z-distance between the closest points to the z-axis of the symmetry axis inside the off-point region to the front plane (cm) = "<< R3_abs - 10. << 
			///endl;//10 cm is the distance between the mid-point of the FwEndCap and the front plane
		  HISTOoffpointToFwEndCap_calculatedForEachSubunit->Fill(R3_abs - 10.);
		  HISTOsubunitTheta_offpointToFwEndCap->Fill(theta3,R3_abs - 10.);
		  
		  R1=R2=0;//this assignment is done only for the sake of drawing of HISTOoffpoint_xy, since practically R1 and R2 are obtained to be 0
		  HISTOoffpoint_xy->Fill(R1,R2);
		  ///cout<<"R1,R2= "<<R1<<" ,  "<<R2<<endl;
		  
		  HISTOoffpointToFronPlane_z->Fill(R3_abs - 10.);
		  HISTOoffpoint_x->Fill(R1);
		  HISTOoffpoint_y->Fill(R2);
		///}
	
       }
     }

//making the middle-region half Subunits
    Double_t middleHalfSubRots_x[9] = {3.724, 3.724, 3.724, 3.724, 0, 0, 0, 0, 0}; // in degrees
    Double_t middleHalfSubRots_y[9] = {0, 1.862, 3.724, 5.586, 7.448, 9.310, 11.172, 13.034, 14.896}; // in degrees
    Double_t HalfSubShifts_x[58] = {-84.6563520970098, -73.8693906775208, -63.1611211867801, -52.521132074127, -41.9393256755252, //corresponding to copy numbers 5-1 (cm)
				    41.9393256755252, 52.521132074127, 63.1611211867801, 73.8693906775208, 84.6563520970098,      //corresponding to copy numbers 6-10
				    99999, //corresponding to copy number 11 which we don't have in the geometry and that's why it's set to a big number (it's not ganna be used anyway)
				    -20.9174269458209, -10.4487681185361, 0, 10.4487681185361, 20.9174269458209, //corresponding to copy numbers 12-16
				    99999, //corresponding to copy number 17 which we don't have in the geometry and that's why it's set to a big number (it's not ganna be used anyway)
				    99999, //corresponding to copy number 18 which we don't have in the geometry and that's why it's set to a big number (it's not ganna be used anyway)
				    -20.9174269458209, -10.4487681185361, 0, 10.4487681185361, 20.9174269458209, //corresponding to copy numbers 19-23
				    99999, //corresponding to copy number 24 which we don't have in the geometry and that's why it's set to a big number (it's not ganna be used anyway)
				    -92.4952892135162, -92.5400931844914, -92.5746759356478, -81.7979947950095, -71.0590410029143, //corresponding to copy numbers 25-29
				    -63.1300146020203, -52.4697812081315, -31.355563839864, -20.8773507855441, 20.8773507855441, 31.355563839864, //corresponding to copy numbers 30-35
				    52.4697812081315, 63.1300146020203, 71.0590410029143, 81.7979947950095, 92.5746759356478, //corresponding to copy numbers 36-40 
				    92.5400931844914, 92.4952892135162, 92.5400931844914, 92.5746759356478, 81.7979947950095, //corresponding to copy numbers 41-45 
				    71.0590410029143, 63.1300146020203, 52.4697812081315, 31.355563839864, 20.8773507855441, -20.8773507855441, //corresponding to copy numbers 46-51 
				    -31.355563839864, -52.4697812081315, -63.1300146020203, -71.0590410029143, -81.7979947950095, -92.5746759356478, -92.5400931844914};// 52-58
				    
    Double_t HalfSubShifts_y[58] = {0, 0, 0, 0, 0, //corresponding to copy numbers 5-1
				    0, 0, 0, 0, 0, //corresponding to copy numbers 6-10
				    99999, //corresponding to copy number 11 which we don't have in the geometry and that's why it's set to a big number (it's not ganna be used anyway)
				    20.8885295181039, 20.876851374154, 20.8658909883895, 20.876851374154, 20.8885295181039, //corresponding to copy numbers 12-16
				    99999, //corresponding to copy number 17 which we don't have in the geometry and that's why it's set to a big number (it's not ganna be used anyway)
				    99999, //corresponding to copy number 18 which we don't have in the geometry and that's why it's set to a big number (it's not ganna be used anyway)
				    -20.8885295181039, -20.876851374154, -20.8658909883895, -20.876851374154, -20.8885295181039, //corresponding to copy numbers 19-23
				    99999, //corresponding to copy number 24 which we don't have in the geometry and that's why it's set to a big number (it's not ganna be used anyway)
				    0, 10.5087330266401, 21.022545290017, 50.0694277965205, 60.6958855430583, //corresponding to copy numbers 25-29
				    68.7741496062525, 79.4684133522537, 90.1738532457449, 90.1213466966609, 90.1213466966609, 90.1738532457449, //corresponding to copy numbers 30-35
				    79.4684133522537, 68.7741496062525, 60.6958855430583, 50.0694277965205, 21.022545290017, //corresponding to copy numbers 36-40  
				    10.5087330266401, 0, -10.5087330266401, -21.022545290017, -50.0694277965205, //corresponding to copy numbers 41-45 
				    -60.6958855430583, -68.7741496062525, -79.4684133522537, -90.1738532457449, -90.1213466966609, -90.1213466966609, //corresponding to copy numbers 46-51 
				    -90.1738532457449, -79.4684133522537, -68.7741496062525, -60.6958855430583, -50.0694277965205, -21.022545290017, -10.5087330266401};// 52-58
				    
    Double_t HalfSubShifts_z[58] = {20.3466200388019, 20.4263813988843, 20.517625616103, 20.6202563339138, 20.7341651712972, //corresponding to copy numbers 5-1
				    20.7341651712972, 20.6202563339138, 20.517625616103, 20.4263813988843, 20.3466200388019, //corresponding to copy numbers 6-10
				    99999, //corresponding to copy number 11 which we don't have in the geometry and that's why it's set to a big number (it's not ganna be used anyway)
				    20.8544382200522, 21.0009144090997, 21.1582196313325, 21.0009144090997, 20.8544382200522, //corresponding to copy numbers 12-16
				    99999, //corresponding to copy number 17 which we don't have in the geometry and that's why it's set to a big number (it's not ganna be used anyway)
				    99999, //corresponding to copy number 18 which we don't have in the geometry and that's why it's set to a big number (it's not ganna be used anyway)
				    20.8544382200522, 21.0009144090997, 21.1582196313325, 21.0009144090997, 20.8544382200522, //corresponding to copy numbers 19-23
				    99999, //corresponding to copy number 24 which we don't have in the geometry and that's why it's set to a big number (it's not ganna be used anyway)
				    21.0015573246815, 20.8413680499497, 20.6925829714711, 20.3600600966017, 20.2538984166576, //corresponding to copy numbers 25-29 
				    20.2322699798398, 20.3302699346598, 20.570496344343, 20.701744222906, 20.701744222906, 20.570496344343, //corresponding to copy numbers 30-35
				    20.3302699346598, 20.2322699798398, 20.2538984166576, 20.3600600966017, 20.6925829714711, //corresponding to copy numbers 36-40  
				    20.8413680499497, 21.0015573246815, 20.8413680499497, 20.6925829714711, 20.3600600966017, //corresponding to copy numbers 41-45 
				    20.2538984166576, 20.2322699798398, 20.3302699346598, 20.570496344343, 20.701744222906, 20.701744222906, //corresponding to copy numbers 46-51 
				    20.570496344343, 20.3302699346598, 20.2322699798398, 20.2538984166576, 20.3600600966017, 20.6925829714711, 20.8413680499497};// 52-58
    
    
    for (Int_t col=4; col<=8; col++){ //in total 10 horizontal middle-region (equator) half Subunits
      rotHalfSub = new TGeoRotation();
      rotHalfSub.RotateX(middleHalfSubRots_x[col]);
      rotHalfSub.RotateY(-middleHalfSubRots_y[col]);
      trHalfSub = new TGeoTranslation(HalfSubShifts_x[col+1], HalfSubShifts_y[col+1], HalfSubShifts_z[col+1] - 21.);
      TGeoCombiTrans *trrotHalfSub= new TGeoCombiTrans(trHalfSub,rotHalfSub);
      name="HalfSubunitVolFwEndCap";
      name+= (6+col-4);
      trrotSub->SetName(name);
      trrotSub->RegisterYourself();
      top->AddNode(HalfSubunitVol,6+col-4,trrotHalfSub);// copy numbering from 6 to 10

      rotHalfSub = new TGeoRotation();
      rotHalfSub.RotateX(middleHalfSubRots_x[12-col]);
      rotHalfSub.RotateY(middleHalfSubRots_y[12-col]);
      trHalfSub = new TGeoTranslation(HalfSubShifts_x[col-4], HalfSubShifts_y[col-4], HalfSubShifts_z[col-4] - 21.);
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
        trHalfSub = new TGeoTranslation(HalfSubShifts_x[10+col], HalfSubShifts_y[10+col], HalfSubShifts_z[10+col] - 21.);
      }
      else { //copy numbering from 14 to 16
      rotHalfSub.RotateY(-middleHalfSubRots_y[col-3]);
      trHalfSub = new TGeoTranslation(HalfSubShifts_x[10+col], HalfSubShifts_y[10+col], HalfSubShifts_z[10+col] - 21.);
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
      trHalfSub = new TGeoTranslation(HalfSubShifts_x[17+col], HalfSubShifts_y[17+col], HalfSubShifts_z[17+col] - 21.);
      }
      else { // copy numbering from 21 to 23
      rotHalfSub.RotateY(-middleHalfSubRots_y[col-3]);
      trHalfSub = new TGeoTranslation(HalfSubShifts_x[17+col], HalfSubShifts_y[17+col], HalfSubShifts_z[17+col] - 21.);
      }
      TGeoCombiTrans *trrotHalfSub= new TGeoCombiTrans(trHalfSub,rotHalfSub);
      name="HalfSubunitVolFwEndCap";
      name+= (18+col);
      trrotSub->SetName(name);
      trrotSub->RegisterYourself();
      top->AddNode(HalfSubunitVol,18+col,trrotHalfSub);// copy numbering from 19 to 23
    }

//making the peripheral half Subunits
    Double_t peripheralHalfSubRots_x[9] = {0, 1.978375, 3.95675, 8.8445, 10.7065, 12.103, 13.965, 15.827, 15.827}; // in degrees
    Double_t peripheralHalfSubRots_y[9] = {16.758, 16.758, 16.758, 14.896, 13.034, 11.172, 9.31, 5.586, 3.724}; // in degrees
    for (Int_t index=0; index<=8; index++){ //in total 18 upper peripheral half Subunits
      rotHalfSub = new TGeoRotation();
      if (index<=4)
        rotHalfSub.RotateZ(90);
      rotHalfSub.RotateX(peripheralHalfSubRots_x[index]);
      rotHalfSub.RotateY(peripheralHalfSubRots_y[index]);
      trHalfSub = new TGeoTranslation(HalfSubShifts_x[24+index], HalfSubShifts_y[24+index], HalfSubShifts_z[24+index] - 21.);
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
      trHalfSub = new TGeoTranslation(HalfSubShifts_x[41-index], HalfSubShifts_y[41-index], HalfSubShifts_z[41-index] - 21.);
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
      trHalfSub = new TGeoTranslation(HalfSubShifts_x[41+index], HalfSubShifts_y[41+index], HalfSubShifts_z[41+index] - 21.);
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
      trHalfSub = new TGeoTranslation(HalfSubShifts_x[58-index], HalfSubShifts_y[58-index], HalfSubShifts_z[58-index] - 21.);
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
  TPad *pad1=new TPad("pad1","pad1",.001,.001,1,1);
  Set_margins_canvas(c0,pad1,2,2);
  c0->cd();
  
  
  pad1->Draw();
  pad1->cd(1)->SetTickx();
  pad1->cd(1)->SetTicky();
  TH2D *graphContainer1 = new TH2D("graphContainer1","graphContainer1",15,4.5,17.5+0.01,100,0.01,31.4);//0.5 degree x-bin size
  graphContainer1->Draw();
  graphContainer1->SetTitle(0);
  graphContainer1->GetYaxis()->SetLabelSize(.07);
  graphContainer1->GetYaxis()->SetNdivisions(205); 
  graphContainer1->GetYaxis()->SetLabelOffset(0.02);
  graphContainer1->GetYaxis()->SetTitle("entries");
  graphContainer1->GetYaxis()->SetTitleOffset(0.9);
  graphContainer1->GetYaxis()->CenterTitle();
  graphContainer1->GetYaxis()->SetTitleSize(0.07);
  HISTOsubunitTheta->Draw("samebartext");
  HISTOsubunitTheta->SetMarkerSize(4.);//text size = 0.02 * pad-height * marker size & text color = marker color
  HISTOsubunitTheta->SetFillColor(921);
  
  pad1->cd(3)->SetTickx();
  pad1->cd(3)->SetTicky();
  TH2D *graphContainer3 = new TH2D("graphContainer3","graphContainer3",15,4.5,17.5+0.01,16,305.5,312.4);
  graphContainer3->Draw();
  graphContainer3->SetTitle(0);
  graphContainer3->GetYaxis()->SetLabelSize(.057);
  graphContainer3->GetYaxis()->SetNdivisions(204);
  graphContainer3->GetXaxis()->SetLabelSize(0.07);
  graphContainer3->GetXaxis()->SetTitle("#theta [deg]");
  graphContainer3->GetXaxis()->SetTitleOffset(1.15);
  graphContainer3->GetXaxis()->CenterTitle();
  graphContainer3->GetXaxis()->SetTitleSize(0.066);
  graphContainer3->GetYaxis()->SetTitle("[cm]");
  graphContainer3->GetYaxis()->SetTitleOffset(1.);
  graphContainer3->GetYaxis()->CenterTitle();
  graphContainer3->GetYaxis()->SetTitleSize(0.06);
  HISTOsubunitTheta_offpointToFwEndCap->Draw("sametext");
  HISTOsubunitTheta_offpointToFwEndCap->SetMarkerStyle(20);
  HISTOsubunitTheta_offpointToFwEndCap->SetMarkerSize(3.);//text size = 0.02 * pad-height * marker size & text color = marker color
    
  pad1->cd(4)->SetTickx();
  pad1->cd(4)->SetTicky();
  TH2D *graphContainer4 = new TH2D("graphContainer4","graphContainer4",100,0.01,25.5,16,305.5,312.4);
  graphContainer4->Draw();
  graphContainer4->SetTitle(0);
  graphContainer4->GetYaxis()->SetLabelSize(.07);
  graphContainer4->GetYaxis()->SetNdivisions(205);
  graphContainer4->GetXaxis()->SetNdivisions(205);
  graphContainer4->GetXaxis()->SetLabelSize(0.07);
  graphContainer4->GetXaxis()->SetTitle("entries");
  graphContainer4->GetXaxis()->SetTitleOffset(1.15);
  graphContainer4->GetXaxis()->CenterTitle();
  graphContainer4->GetXaxis()->SetTitleSize(0.07);
  HISTOoffpointToFwEndCap_calculatedForEachSubunit->Draw("samehbar");
  HISTOoffpointToFwEndCap_calculatedForEachSubunit->SetFillColor(921);
  
 /* pad1->cd();
  tex_Y4 = new TLatex(0.99,0.3,"entries");
  tex_Y4->SetTextAngle(90);
  tex_Y4->SetTextSize(0.035);
  tex_Y4->Draw();
  tex_Y4_label1 = new TLatex(0.96,0.26,"1");
  tex_Y4_label1->SetTextAngle(0);
  tex_Y4_label1->SetTextSize(0.03);
  tex_Y4_label1->Draw();
  tex_Y4_label2 = new TLatex(0.96,0.47,"2");
  tex_Y4_label2->SetTextAngle(0);
  tex_Y4_label2->SetTextSize(0.03);
  tex_Y4_label2->Draw();  
  */
 
  pad1->cd(1);
  tex_c1 = new TLatex(5,26,"a)");
  tex_c1->SetTextAngle(0);
  tex_c1->SetTextSize(0.1);
  tex_c1->Draw();
  
  pad1->cd(3);
  tex_c3 = new TLatex(5,306.5,"b)");
  tex_c3->SetTextAngle(0);
  tex_c3->SetTextSize(0.1);
  tex_c3->Draw();
  
  pad1->cd(4);
  tex_c4 = new TLatex(20,306.5,"c)");
  tex_c4->SetTextAngle(0);
  tex_c4->SetTextSize(0.1);
  tex_c4->Draw();
  
  
  gStyle->SetPalette(2);
  TCanvas* c00 = new TCanvas("c00","", 10, 20, 800, 800); 
  HISTOoffpoint_xy->Draw("textcolz");
 
  
/////////////////drawings for the paper:
TCanvas* offpoint_xyz = new TCanvas("offpoint_xyz","", 10, 20, 800, 800); 
offpoint_xyz->Divide(1,2);
offpoint_xyz->cd(1)->SetBottomMargin(0.15);
HISTOoffpointToFronPlane_z->Draw();
HISTOoffpointToFronPlane_z->SetLineColor(1);
HISTOoffpointToFronPlane_z->GetXaxis()->CenterTitle();
HISTOoffpointToFronPlane_z->GetYaxis()->CenterTitle();
HISTOoffpointToFronPlane_z->GetXaxis()->SetTitleSize(0.07);
HISTOoffpointToFronPlane_z->GetXaxis()->SetTitleOffset(1.05);
HISTOoffpointToFronPlane_z->GetXaxis()->SetLabelSize(0.07);
HISTOoffpointToFronPlane_z->GetYaxis()->SetTitleSize(0.07);
HISTOoffpointToFronPlane_z->GetYaxis()->SetTitleOffset(0.7);
HISTOoffpointToFronPlane_z->GetYaxis()->SetLabelSize(0.07);
tex_offpoint_a = new TLatex(311.6,22,"a)");
tex_offpoint_a->SetTextAngle(0);
tex_offpoint_a->SetTextSize(0.1);
tex_offpoint_a->Draw();
cout<< "mean value of the off-point value = "<< HISTOoffpointToFronPlane_z->GetMean() << endl;

offpoint_xyz->cd(2)->SetBottomMargin(0.15);
HISTOoffpoint_x->Draw();
HISTOoffpoint_x->SetLineColor(1);
HISTOoffpoint_x->GetXaxis()->CenterTitle();
HISTOoffpoint_x->GetYaxis()->CenterTitle();
HISTOoffpoint_x->GetXaxis()->SetTitleSize(0.07);
HISTOoffpoint_x->GetXaxis()->SetTitleOffset(1.05);
HISTOoffpoint_x->GetXaxis()->SetLabelSize(0.07);
HISTOoffpoint_x->GetYaxis()->SetTitleSize(0.07);
HISTOoffpoint_x->GetYaxis()->SetTitleOffset(0.7);
HISTOoffpoint_x->GetYaxis()->SetLabelSize(0.07);
tex_offpoint_b = new TLatex(0.85,40,"b)");
tex_offpoint_b->SetTextAngle(0);
tex_offpoint_b->SetTextSize(0.1);
tex_offpoint_b->Draw();

HISTOoffpoint_y->Draw("same");
HISTOoffpoint_y->SetLineColor(1);
HISTOoffpoint_y->SetLineStyle(2);
HISTOoffpoint_y->SetLineWidth(2);
///////////////////



}

void Set_margins_canvas(TCanvas *c,TPad *p,int n1,int n2){
  c->SetFillColor(10);
  c->SetFrameFillColor(10);
  p->SetFillColor(10);
  p->SetFrameFillColor(10);
  p->Divide(n1,n2,0);
  double LM=0.,RM=0.,TM=0.0,BM=0.0;
  for (int i=1;i<=n1*n2;i++){
    if (fmod(i,n1)==1 || n1==1) LM=.12;
    else LM=0.;
    if (fmod(i,n1)==0) RM=.01;
    else RM=0.;
    if (i>(n1*n2-n1)) BM=.16;
    else BM=0.;


    Set_margins_pad(p,i,LM,RM,TM,BM);
  }
}
void Set_margins_pad(TPad *p,int padn,double LM,
		     double RM,double TM,double BM){
  p->cd(padn)->SetTopMargin(TM);
  p->cd(padn)->SetBottomMargin(BM);
  p->cd(padn)->SetLeftMargin(LM);
  p->cd(padn)->SetRightMargin(RM);
  p->cd(padn)->SetFillColor(10);
  p->cd(padn)->SetFrameFillColor(10);
}


//how to draw the geometry of the generated root file:
// >> root -l
// >> gSystem->Load("libGeom")
// >> TGeoManager::Import("emc_module3_hossein_new.root")
// >> gGeoManager->GetMasterVolume()->Draw()
