{
   // This macro creates the Backward EndCap (BwEndCap) geometry 
   // of the EMC detector for PANDA. 
   // The straight crystals with the size of 26x26x200 mm^3 are used
   //
   // Autor: Aleksandra Biegun, KVI (27.02.2009)

  //-----------------------------
  const Double_t  kSpaceInBox    = 0.06;    // Space in Box (cm)
  const Double_t  kSpaceInSub    = 0.024;   // Space in Subunit (cm)
  const Double_t  kSpaceSubGlue  = 0.036;   // Thickness of glued alveole 
                                            // between Boxes in Subunit (cm)
  const Double_t  kAlveoleThickness = 0.018;// Thickness of alveole between Subunits (cm)

  const Int_t     kNumOfQuarters = 1;       // Number of Quarter - only 1 is created, next 3 are copies at the end of the file
  const Int_t     kNumOfBoxes    = 4;       // Numbers of Boxes in one Subunit
  const Int_t     kNumOfCrystals = 4;       // Numbers of Crystals in one Box


  Double_t tr = 2.6;                   // Size of the Crystal in its center (cm)

  Double_t b1 = 2.6000-0.5*tr;       // Back-tilted part (-Z direction) of the Crystal center (cm)
  Double_t b2 = b1;//2.4375-0.5*tr;       // Front-tilted part (+Z direction) of the Crystal center (cm)

  Double_t dz,vert[20];              // Parameters for TGeoArb8 shape of crystal (cm)
  Double_t vert0[20],vertBox[20],vertSub[20],vertQuar[20];

  // Parameters for crystal shape:  front size:    26.0(mm) x 26.0(mm)
  //                                back  size:    26.0(mm) x 26.0(mm)
  
  dz       = 10.000;
  // first four vertices - in -Z direction
  vert[0]  = -0.5*tr;  
  vert[1]  = -0.5*tr;
  vert[2]  = -0.5*tr;
  vert[3]  = +b1;
  vert[4]  = +b1;
  vert[5]  = +b1;
  vert[6]  = +b1;
  vert[7]  = -0.5*tr;
  
  // second four vertices - in +Z direction
  vert[8]  = -0.5*tr;
  vert[9]  = -0.5*tr;
  vert[10] = -0.5*tr;
  vert[11] = +b2; 
  vert[12] = +b2;     // Tapered right side the front face 
  vert[13] = +b2;     // of the crystal (upper corner)
  vert[14] = +b2;     // Tapered right side the front face
  vert[15] = -0.5*tr; // of the crystal (lower corner)
  

  // Box size (4 crystals) - creation a SIZE for BOX shape
  for (Int_t i=0; i< 16; i++){
    Double_t spaces = 0.5*kSpaceInBox + kSpaceInSub;
    if (i==0 || i==1) 
      {
	vertBox[i] = (-vert[3] - spaces);   // lower-left corner back-side
      }
    else if (i==8 || i==9) 
      {
	vertBox[i] = (-vert[11] - spaces);  // lower-left corner front-side 
      }
    else if (i==7 || i==15)
      { 
	vertBox[i] = (-vert[i-1] - spaces); // lower-right points (front & back sides) 
      }
    else if (i==2 || i==10)                // upper-left points (front & back sides) 
      {
	vertBox[i] = (-vert[i+1] - spaces);
      }
    else 
      {
	vertBox[i] = (vert[i] + spaces);    // rest of those points
      }
  }
    
  // Subunit size (16 crystals, 4 boxes) - creation a SIZE for SUBUNIT shape
  for (Int_t i=0; i< 16; i++){
    Double_t spacesSub = 0.5*kSpaceSubGlue + kAlveoleThickness;
    if (i==0 || i==1) 
      {
	vertSub[i] = (vertBox[i]*2 - spacesSub); // lower-left corner back-side
      }
    else if (i==8 || i==9) 
      {
	vertSub[i] = (vertBox[i]*2 - spacesSub); // lower-left corner front-side 
      }
    else if (i==7 || i==15)
      { 
	vertSub[i] = (vertBox[i]*2 - spacesSub);
      }
    else if (i==2 || i==10)
      {
	vertSub[i] = (vertBox[i]*2 - spacesSub);
      }
    else 
      {
	vertSub[i] = (vertBox[i]*2 + spacesSub);
      }
  }
    
  // Quarter size (Max is: 9X9 Subunits)
  Double_t spacesQuar = 0.0;
  for (Int_t i=0; i< 16; i++){
    if (i==0 || i==1) 
      {
	vertQuar[i] = (vertSub[i]*9 - spacesQuar); // lower-left corner back-side
      }
    else if (i==8 || i==9) 
      {
	vertQuar[i] = (vertSub[i]*9 - spacesQuar); // lower-left corner front-side 
      }
    else if (i==7 || i==15)
      { 
	vertQuar[i] = (vertSub[i]*9 - spacesSub);
      }
    else if (i==2 || i==10)
      {
	vertQuar[i] = (vertSub[i]*9 - spacesQuar);
      }
    else 
      {
	vertQuar[i] = (vertSub[i]*9 + spacesQuar);
      }
  }

  //--------------------------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  
  // Load this example libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libPassive");
 
  TString outfile= "../../geometry/emc_module4_StraightGeo.root";
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

  //construct the overall box
  Double_t sizeOfQuar = 42.752;  // size of one Quarter (the same in X and Y direction)
                                  // size of one Subunit (back side): 10.688*4 = 42.752 (cm)

  TGeoVolume *top = new TGeoVolumeAssembly("Emc4");
                    
  gGeoMan->SetTopVolume(top);
  
  TGeoTranslation trQuar;
  TGeoTranslation trSub;
  TGeoTranslation trBox; 
  TGeoTranslation trCrystal; 

  TGeoRotation rotQuar;
  TGeoRotation rotSub;
  TGeoRotation rotBox; 
  TGeoRotation rotCrystal; 

  TGeoShape* QuarterShape;
  TGeoShape* SubunitShape;
  TGeoShape* BoxShape;
  TGeoShape* CrystalShape;
  
  TGeoVolume* QuarterVol;
  TGeoVolume* SubunitVol;
  TGeoVolume* SubunitVol1;
  TGeoVolume* SubunitVol2;
  TGeoVolume* SubunitVol3;
  TGeoVolume* SubunitVol4;
  TGeoVolume* SubunitVol5;
  TGeoVolume* SubunitVol6;
  TGeoVolume* SubunitVol7;
  TGeoVolume* SubunitVol8;
  TGeoVolume* SubunitVol9;
  TGeoVolume* BoxVol;
  TGeoVolume* BoxVol1;
  TGeoVolume* BoxVol2;
  TGeoVolume* BoxVol3;
  TGeoVolume* BoxVol4;
  TGeoVolume* CrystalVol;     

  TString name,name1,name2,name3,name4,name5,name6,name7,name8,name9;

  // ========== QUARTER (13 subunits) ===================
  cout<< "-----------------------------------------------> Quarter VOLUME " <<endl;
  name = "QuarterShape";
  QuarterShape = new TGeoArb8(name,dz,vertQuar); 
  
  name = "Quarter4Vol";
  TString medium="air";
      
  QuarterVol = new TGeoVolumeAssembly(name); 
      
  // ========== SUBUNIT (16 crystals) ===================
  name = "SubunitShape";
  SubunitShape = new TGeoArb8(name,dz,vertSub); 
      
  TString medium="air";

 // SubunitVol = new TGeoVolume(name,SubunitShape,gGeoMan->GetMedium(medium));
  name = "SubunitVol";
  name1 = "SubunitVol1";
  name2 = "SubunitVol2";
  name3 = "SubunitVol3";
  name4 = "SubunitVol4";
  name5 = "SubunitVol5";
  name6 = "SubunitVol6";
  name7 = "SubunitVol7";
  name8 = "SubunitVol8";
  name9 = "SubunitVol9";
  SubunitVol = new TGeoVolumeAssembly(name);
  SubunitVol1 = new TGeoVolumeAssembly(name1);
  SubunitVol2 = new TGeoVolumeAssembly(name2);
  SubunitVol3 = new TGeoVolumeAssembly(name3);
  SubunitVol4 = new TGeoVolumeAssembly(name4);
  SubunitVol5 = new TGeoVolumeAssembly(name5);
  SubunitVol6 = new TGeoVolumeAssembly(name6);
  SubunitVol7 = new TGeoVolumeAssembly(name7);
  SubunitVol8 = new TGeoVolumeAssembly(name8);
  SubunitVol9 = new TGeoVolumeAssembly(name9);
    
  // ========== BOX (4 crystals) ========================
  name = "BoxShape";
  BoxShape = new TGeoArb8(name,dz,vertBox); 
  
  TString medium="air";
  name="BoxVol";
  //BoxVol = new TGeoVolume(name,BoxShape,gGeoMan->GetMedium(medium));
  BoxVol = new TGeoVolumeAssembly(name);

  // To avoid overlaps for Box Volumes use TGeoVolumeAssembly for your analysis
  // Box Shapes have been changed and not corrected yet,
  // nevertheless only Crystals are active volumes
  name1="BoxVol1";
  name2="BoxVol2";
  name3="BoxVol3";
  name4="BoxVol4";
  //BoxVol1 = new TGeoVolume(name1,BoxShape,gGeoMan->GetMedium(medium));
  //BoxVol2 = new TGeoVolume(name2,BoxShape,gGeoMan->GetMedium(medium));
  //BoxVol3 = new TGeoVolume(name3,BoxShape,gGeoMan->GetMedium(medium));
  //BoxVol4 = new TGeoVolume(name4,BoxShape,gGeoMan->GetMedium(medium));
  BoxVol1 = new TGeoVolumeAssembly(name1);
  BoxVol2 = new TGeoVolumeAssembly(name2);
  BoxVol3 = new TGeoVolumeAssembly(name3);
  BoxVol4 = new TGeoVolumeAssembly(name4);

  // Translations and Rotations for Boxes are according to the local 
  // co-ordinate system, which is in the center of the Subunit.
  // For the straight geometry, both, front & back sides are 26mm x 26 mm, 
  // so, the angle = 0. (e.g. rotBox.RotateX(0.);)
  for(Int_t b=0; b<kNumOfBoxes; b++){
    cout << "     " << endl;
    cout << "----------------> BOX number: " << b <<endl;
    if (b==0) {      
      trBox = new TGeoTranslation(tr+kSpaceInSub+kAlveoleThickness+0.5*kSpaceInBox,tr+kSpaceInSub+kAlveoleThickness+0.5*kSpaceInBox, 0.);
      rotBox = new TGeoRotation();    
      rotBox.RotateX(0.);  // rotation in "up" direction 
      rotBox.RotateY(0.); // rotation in "right" direction 
    }
    if (b==1) {
      trBox = new TGeoTranslation(-tr-kSpaceInSub-kAlveoleThickness-0.5*kSpaceInBox,-tr-kSpaceInSub-kAlveoleThickness-0.5*kSpaceInBox, 0.);
      rotBox = new TGeoRotation(); 
      rotBox.RotateX(0.); // rotation in "down" direction
      rotBox.RotateY(0.);  // rotation in "left" direction
    }
    if (b==2) {
      trBox = new TGeoTranslation(tr+kSpaceInSub+kAlveoleThickness+0.5*kSpaceInBox,-tr-kSpaceInSub-kAlveoleThickness-0.5*kSpaceInBox, 0.);
      rotBox = new TGeoRotation();
      rotBox.RotateX(0.); // rotation in "down" direction
      rotBox.RotateY(0.); // rotation in "right" direction
    }
    if (b==3) {
      trBox = new TGeoTranslation(-tr-kSpaceInSub-kAlveoleThickness-0.5*kSpaceInBox,tr+kSpaceInSub+kAlveoleThickness+0.5*kSpaceInBox, 0.);
      rotBox = new TGeoRotation();
      rotBox.RotateX(0.); // rotation in "up" direction  
      rotBox.RotateY(0.); // rotation in "left" direction  
    }
    TGeoCombiTrans* trrotBox= new TGeoCombiTrans(trBox,rotBox); 

    name="BoxVol";

    name+=b;
    trrotBox->SetName(name);
    trrotBox->RegisterYourself();

    SubunitVol->AddNode(BoxVol,b,trrotBox);
    //top->AddNode(BoxVol,b,trrotBox);
    
    //differentc conditions: depends on combination of crystals in Box
    if (b==1){
      name+=b;
      trrotBox->SetName(name);
      trrotBox->RegisterYourself();
      
      SubunitVol1->AddNode(BoxVol,b,trrotBox);      
    }
    if (b==2){
      name+=b;
      trrotBox->SetName(name);
      trrotBox->RegisterYourself();
      
      SubunitVol2->AddNode(BoxVol1,b,trrotBox);
    }
    if (b==0){
      name+=b;
      trrotBox->SetName(name);
      trrotBox->RegisterYourself();

      SubunitVol3->AddNode(BoxVol3,b,trrotBox);
      SubunitVol9->AddNode(BoxVol4,b,trrotBox);
    }
    if (b==1 || b==2){
      name+=b;
      trrotBox->SetName(name);
      trrotBox->RegisterYourself();
      
      SubunitVol4->AddNode(BoxVol,b,trrotBox);
    }
    if (b==0 || b==3){
      name+=b;
      trrotBox->SetName(name);
      trrotBox->RegisterYourself();
      
      SubunitVol5->AddNode(BoxVol1,b,trrotBox);
    }
    if (b==3){
      name+=b;
      trrotBox->SetName(name);
      trrotBox->RegisterYourself();
      
      SubunitVol6->AddNode(BoxVol,b,trrotBox);
      SubunitVol8->AddNode(BoxVol2,b,trrotBox);
    }
    if (b==0 || b==2){
      name+=b;
      trrotBox->SetName(name);
      trrotBox->RegisterYourself();
      
      SubunitVol7->AddNode(BoxVol2,b,trrotBox);
    }
  }
  // ========== CRYSTAL (TGeoArb8 shape)=================
  name = "CrystalShape";
  CrystalShape = new TGeoArb8(name,dz,vert); 

  TString medium="PWO";
  name = "CrystalVol";
  CrystalVol = new TGeoVolume(name,CrystalShape,gGeoMan->GetMedium(medium));

  // Translation and Rotation for each Crystal are according to 
  // co-ordinate system in the center of the Box.
  // Crystals arrangement: right angle of each Crystal is in the middle of the box.
  CrystalVol->SetLineColor(5);
  for(Int_t k=0; k<kNumOfCrystals; k++){
    cout << "               " << endl;
    cout << "         ----->  CRYSTAL number: "<< k << endl;
     if (k==0){     
	trCrystal = new TGeoTranslation(tr/2.+0.5*kSpaceInBox,tr/2.+0.5*kSpaceInBox,0.);
	rotCrystal = new TGeoRotation() ; // "right-upper" Crystal
     }else if (k==1){
	trCrystal= new TGeoTranslation(-tr/2.-0.5*kSpaceInBox,-tr/2.-0.5*kSpaceInBox,0.); 
	rotCrystal = new TGeoRotation();  // "left-lower" Crystal
	rotCrystal.RotateZ(180.);      
     }else if (k==2){
	trCrystal= new TGeoTranslation(-tr/2.-0.5*kSpaceInBox,tr/2.+0.5*kSpaceInBox,0.); 
	rotCrystal = new TGeoRotation();  // "left-upper" Crystal
	rotCrystal.RotateZ(90.);       
     }else if (k==3){
	trCrystal= new TGeoTranslation(tr/2.+0.5*kSpaceInBox,-tr/2.-0.5*kSpaceInBox,0.); 
	rotCrystal = new TGeoRotation() ; // "right-lower" Crystal
	rotCrystal.RotateZ(270.);      
     }

    TGeoCombiTrans* trrotCrystal= new TGeoCombiTrans(trCrystal,rotCrystal); 
    
    name = "CrystalVol";
    name+=k;
    trrotCrystal->SetName(name);
    trrotCrystal->RegisterYourself();

    BoxVol->AddNode(CrystalVol,k,trrotCrystal);

    if (k==1 || k==3) {
      name+=k;
      trrotCrystal->SetName(name);
      trrotCrystal->RegisterYourself();
      
      BoxVol1->AddNode(CrystalVol,k,trrotCrystal);
    }
    if (k==1 || k==2) {
      name+=k;
      trrotCrystal->SetName(name);
      trrotCrystal->RegisterYourself();
      
      BoxVol2->AddNode(CrystalVol,k,trrotCrystal);
    }
    if (k==1) {
      name+=k;
      trrotCrystal->SetName(name);
      trrotCrystal->RegisterYourself();
      
      BoxVol3->AddNode(CrystalVol,k,trrotCrystal);
    }
    if (k==0 || k==2 || k==3) {
      name+=k;
      trrotCrystal->SetName(name);
      trrotCrystal->RegisterYourself();
      
      BoxVol4->AddNode(CrystalVol,k,trrotCrystal);
    }
  }
  
  Double_t ShiftToZeroSub= kSpaceInSub + kAlveoleThickness;
  Double_t kSubShift=10.688;        // size of 1 Subunit (cm) in its half Z-distance of crystals
  //Double_t FrontFaceToOffPoint=295; // Off_point -> front_face_of_crystals distance (cm)
  Double_t FrontFaceToOffPoint=305; // Off_point -> front_face_of_crystals distance (cm)

  Int_t jj=0;
  for (Int_t row=0; row<4; row++){
    for (Int_t col=0; col<4; col++){
  	
      Int_t flag=1;
      
      // *** 3 Subunits are fully disables 
      //if (row==0  && col==0) flag=0; // middle subunit      
      if (row==3  && col==3) flag=0; // the most external corner's subunit
      if (row==3  && col==2) flag=0; // here: row==column => 4th col, 3rd Subunit
      if (row==2  && col==3) flag=0; // here: row==column => 3th col, 4th Subunit 

      // *** Subunits with disabled crystals
      if (row==0  && col==0) flag=10; // middle subunit
      if (row==0  && col==3) flag=-1; // here: row==column => 1st col, 4th Subunit (top)
      if (row==1  && col==3) flag=-2; // here: row==column => 2st col, 4th Subunit (top)
      if (row==2  && col==2) flag=-3; // here: row==column => 3st col, 3rd Subunit
      if (row==3  && col==0) flag=-4; // here: row==column => 4st col, 1st Subunit
      if (row==3  && col==1) flag=-5; // here: row==column => 4st col, 2st Subunit
     

      if (flag!=0){
	
	jj++;
	cout<< "----------------------------> 16 CRYSTALS SUBUNIT  No: " << jj <<endl;
      
	rotSub = new TGeoRotation();
	Int_t n=2*row+1; // fixed angle for each Subunit
	Int_t m=2*col+1;
	      
	//Double_t RotToZeroSubX=  0.931*m; // 0.931 (deg) -> rotation along X and Y
	//Double_t RotToZeroSubY= -0.931*n; // axis to have "right angle" between Quarters

	//rotSub.RotateX(RotToZeroSubX);
	//rotSub.RotateY(RotToZeroSubY);
	rotSub.RotateX(0.);
	rotSub.RotateY(0.);

	Double_t tan_alpha1=tan(n*(TMath::Pi()/180.));
	Double_t addShiftX=FrontFaceToOffPoint*tan_alpha1;

	Double_t tan_alpha2=tan(m*(TMath::Pi()/180.));
	Double_t addShiftY=FrontFaceToOffPoint*tan_alpha2;
   
	Double_t ShiftX=(ShiftToZeroSub + addShiftX -0.5*sizeOfQuar);
	Double_t ShiftY=(ShiftToZeroSub + addShiftY -0.5*sizeOfQuar);

	trSub = new TGeoTranslation(ShiftX, ShiftY, 0.);
		
	TGeoCombiTrans* thAngle= new TGeoCombiTrans(trSub,rotSub); 
	  
	name="SubunitVol";
	name+=jj;
	thAngle->SetName(name);
	thAngle->RegisterYourself();                 
      
	if (flag==1)
	  {
	    QuarterVol->AddNode(SubunitVol,jj,thAngle);
	  }
	else if (flag==10) // middle subunit
	  {
	    QuarterVol->AddNode(SubunitVol9,jj,thAngle);
	  }
	else if (flag==-1)
	  {
	    QuarterVol->AddNode(SubunitVol4,jj,thAngle);
	    QuarterVol->AddNode(SubunitVol5,jj,thAngle);
	  }
	else if (flag==-2)
	  {
	    QuarterVol->AddNode(SubunitVol1,jj,thAngle);
	    QuarterVol->AddNode(SubunitVol2,jj,thAngle);
	  }
	else if (flag==-3)
	  {
	    QuarterVol->AddNode(SubunitVol4,jj,thAngle);
	    QuarterVol->AddNode(SubunitVol6,jj,thAngle);
	    QuarterVol->AddNode(SubunitVol3,jj,thAngle);
	  }
	else if (flag==-4)
	  {
	    QuarterVol->AddNode(SubunitVol1,jj,thAngle);
	    QuarterVol->AddNode(SubunitVol6,jj,thAngle);
	    QuarterVol->AddNode(SubunitVol7,jj,thAngle);
	  }
	else if (flag==-5)
	  {
	    QuarterVol->AddNode(SubunitVol1,jj,thAngle);
	    QuarterVol->AddNode(SubunitVol8,jj,thAngle);
	  }
      }
      
    }
  }
    
  trQuar  = new TGeoTranslation(0.5*sizeOfQuar,0.5*sizeOfQuar,0.);
  rotQuar = new TGeoRotation();
  
  TGeoCombiTrans* trrotQuar= new TGeoCombiTrans(trQuar,rotQuar);    
  name="QuarterVol";
  trrotQuar->SetName(name);
  trrotQuar->RegisterYourself();

  // The Backward endcap has additional ReflectX(1) for each copy,
  // thus, 'ttt' translations are differnt, compare to the Forward endcap.
  // Bwendcap is placed in -Z direction and the order of four copies (4 Quarters)
  // is the same like for the old (first) version of backward endcap.
  // The coordinates shown below are according to PandaRoot.

  trrotQuar->ReflectX(1); // first copy: -X +Y
  
  top->AddNode(QuarterVol,1,trrotQuar); //right-upper Quarter 
    
  for (Int_t q=1; q<=3; q++){
    TGeoCombiTrans reflection; 
    TGeoTranslation ttt;

    if (q==1){                // second copy: -X -Y
      reflection.ReflectY(1); // right-lower Quarter   
      reflection.ReflectX(1);
      ttt = new TGeoTranslation(-0.5*sizeOfQuar,-0.5*sizeOfQuar,0.);
    }
    if (q==2){                // third copy: +X -Y
      reflection.ReflectY(1); // left-lower Quarter
      reflection.ReflectX(1); 
      reflection.ReflectX(1);
      ttt = new TGeoTranslation(0.5*sizeOfQuar,-0.5*sizeOfQuar,0.);
    }
    if (q==3){                // fourth copy: +X +Y
      reflection.ReflectX(1); // left-upper Quarter      
      reflection.ReflectX(1); 
      ttt = new TGeoTranslation(0.5*sizeOfQuar,0.5*sizeOfQuar,0.);
    }

    top->AddNode(QuarterVol,q+1,new TGeoCombiTrans(ttt,reflection)); 
  }
  
  gGeoMan->CloseGeometry();
  top->Write();
  fi->Close();
  //gGeoManager->Export(outfile);//
  //top->Draw();//
}  
