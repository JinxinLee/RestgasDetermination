{
  // FwEndCap geometry parameters
  //-----------------------------
  const Double_t  kSpaceInBox    = 0.06;    // Space in Box (cm)
  const Double_t  kSpaceInSub    = 0.024;   // Space in Subunit (cm)
  const Double_t  kSpaceSubGlue  = 0.036;   // Thickness of glued alveole 
                                            // between Boxes in Subunit (cm)
  const Double_t  kAlveoleThickness = 0.018;// Thickness of alveole between Subunits (cm)

  const Int_t     kNumOfQuarters = 1;
  const Int_t     kNumOfBoxes    = 4;
  const Int_t     kNumOfCrystals = 4;

  Double_t tr = 2.51875;             // Size of the Crystal in its center (cm)
  Double_t dz,vert[20];              // Parameters for TGeoArb8 shape of crystal (cm)
  Double_t vert0[20],vertBox[20],vertSub[20],vertQuar[20];

  // Parameters for crystal shape:  front size:    24.375(mm) x 24.375(mm)
  //                                back  size:    26.0(mm) x 26.0(mm)
  dz       = 10.000;
  vert[0]  = 0.0000; 
  vert[1]  = 0.0000;
  vert[2]  = 0.0000;
  vert[3]  = 2.6000;
  vert[4]  = 2.6000;
  vert[5]  = 2.6000;
  vert[6]  = 2.6000;
  vert[7]  = 0.0000;
  vert[8]  = 0.0000;
  vert[9]  = 0.0000;
  vert[10] = 0.0000;
  vert[11] = 2.4375;
  vert[12] = 2.4375;  // Tapered right side the front face 
  vert[13] = 2.4375;  // of the crystal (upper corner)
  vert[14] = 2.4375;  // Tapered right side the front face
  vert[15] = 0.0000;  // of the crystal (lower corner)
   
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
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  
  // Load this example libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libPassive");
  
  TString outfile= "../../geometry/emc_module3.root";
  TFile* fi = new TFile(outfile,"RECREATE");  
  
  FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  geoFace->setMediaFile("../../../pandaroot/geometry/media_pnd.geo");
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
  Double_t sizeOfQuar = 96.192;  // size of one Quarter (the same in X and Y direction)
                                 // size of one Subunit (back side): 10.688*9 = 96.192 (cm)
  TGeoVolume *top = gGeoMan->MakeTube("top",gGeoMan->GetMedium("air"),0,98,22.5);//cm
                    
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
  TGeoVolume* BoxVol;
  TGeoVolume* CrystalVol;     

  TString name;

  // ========== QUARTER (55 subunits) ===================
  cout<< "-----------------------------------------------> Quarter VOLUME " <<endl;
  name = "QuarterShape";
  QuarterShape = new TGeoArb8(name,dz,vertQuar); 
  
  name = "QuarterVol";
  TString medium="air";
      
  QuarterVol = new TGeoVolume(name,QuarterShape,gGeoMan->GetMedium(medium));
  
  cout << "Quarter Medium ==== "<< gGeoMan->GetMedium(medium)->GetName()<< endl;
    
  // ========== SUBUNIT (16 crystals) ===================
  name = "SubunitShape";
  SubunitShape = new TGeoArb8(name,dz,vertSub); 
      
  //TString medium="carbon";
  TString medium="air";
  name = "SubunitVol";
  SubunitVol = new TGeoVolume(name,SubunitShape,gGeoMan->GetMedium(medium));
    
  // ========== BOX (4 crystals) ========================
  name = "BoxShape";
  BoxShape = new TGeoArb8(name,dz,vertBox); 
  
  //TString medium="carbon";
  TString medium="air";
  name="BoxVol";
  BoxVol = new TGeoVolume(name,BoxShape,gGeoMan->GetMedium(medium));
  
  
  // Translations and Rotations for Boxes are according to co-ordinate system 
  // which is in the center of the Subunit
  for(Int_t b=0; b<kNumOfBoxes; b++){
    cout << "     " << endl;
    cout << "----------------> BOX number: " << b <<endl;
    if (b==0) {      
      trBox = new TGeoTranslation(tr+kSpaceInSub+kAlveoleThickness+0.5*kSpaceInBox,tr+kSpaceInSub+kAlveoleThickness+0.5*kSpaceInBox, 0.);
      rotBox = new TGeoRotation();    
      rotBox.RotateX(0.465518);  // rotation in "up" direction 
      rotBox.RotateY(-0.465518); // rotation in "right" direction 
    }
    if (b==1) {
      trBox = new TGeoTranslation(-tr-kSpaceInSub-kAlveoleThickness-0.5*kSpaceInBox,-tr-kSpaceInSub-kAlveoleThickness-0.5*kSpaceInBox, 0.);
      rotBox = new TGeoRotation(); 
      rotBox.RotateX(-0.465518); // rotation in "down" direction
      rotBox.RotateY(0.465518);  // rotation in "left" direction
    }
    if (b==2) {
      trBox = new TGeoTranslation(tr+kSpaceInSub+kAlveoleThickness+0.5*kSpaceInBox,-tr-kSpaceInSub-kAlveoleThickness-0.5*kSpaceInBox, 0.);
      rotBox = new TGeoRotation();
      rotBox.RotateX(-0.465518); // rotation in "down" direction
      rotBox.RotateY(-0.465518); // rotation in "right" direction  
    }
    if (b==3) {
      trBox = new TGeoTranslation(-tr-kSpaceInSub-kAlveoleThickness-0.5*kSpaceInBox,tr+kSpaceInSub+kAlveoleThickness+0.5*kSpaceInBox, 0.);
      rotBox = new TGeoRotation();
      rotBox.RotateX(0.465518); // rotation in "up" direction  
      rotBox.RotateY(0.465518); // rotation in "left" direction  
    }
    TGeoCombiTrans* trrotBox= new TGeoCombiTrans(trBox,rotBox); 

    name="BoxVol";
    name+=b;
    trrotBox->SetName(name);
    trrotBox->RegisterYourself();
    
    SubunitVol->AddNode(BoxVol,b,trrotBox);
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
      trCrystal = new TGeoTranslation(0.5*kSpaceInBox,0.5*kSpaceInBox,0.);
      rotCrystal = new TGeoRotation() ; // "right-upper" Crystal
    }else if (k==1){
      trCrystal= new TGeoTranslation(-0.5*kSpaceInBox,-0.5*kSpaceInBox,0.); 
      rotCrystal = new TGeoRotation();  // "left-lower" Crystal
      rotCrystal.RotateZ(180.);      
    }else if (k==2){
      trCrystal= new TGeoTranslation(-0.5*kSpaceInBox,0.5*kSpaceInBox,0.); 
      rotCrystal = new TGeoRotation();  // "left-upper" Crystal
      rotCrystal.RotateZ(90.);       
    }else if (k==3){
      trCrystal= new TGeoTranslation(0.5*kSpaceInBox,-0.5*kSpaceInBox,0.); 
      rotCrystal = new TGeoRotation() ; // "right-lower" Crystal
      rotCrystal.RotateZ(270.);      
    }
    TGeoCombiTrans* trrotCrystal= new TGeoCombiTrans(trCrystal,rotCrystal); 
 
    name = "CrystalVol";
    name+=k;
    trrotCrystal->SetName(name);
    trrotCrystal->RegisterYourself();
    
    BoxVol->AddNode(CrystalVol,k,trrotCrystal);
  }
  
  Double_t ShiftToZeroSub= kSpaceInSub + kAlveoleThickness;
  Double_t kSubShift=10.363;        // size of 1 Subunit (cm) in its half Z-distance
  Double_t FrontFaceToOffPoint=295; // Off_point -> front_face_of_crystals distance (cm)
  Int_t jj=0;
  for (Int_t row=0; row<9; row++){
    for (Int_t col=0; col<9; col++){
      	
      Int_t flag=1;
      
      if (row<3  && col <2) flag=0;
      if (row==3 && col==0) flag=0;
      if (col>1  && row >7) flag=0;
      if (col>4  && row >6) flag=0;
      if (col>5  && row >5) flag=0;
      if (col>6  && row >4) flag=0;
      if (col>7  && row >1) flag=0;
      
      if (flag){
	
	jj++;
	cout<< "----------------------------> 16 CRYSTALS SUBUNIT  No: " << jj <<endl;
      
	rotSub = new TGeoRotation();
	Int_t n=2*row+1; // fixed angle for each Subunit
	Int_t m=2*col+1;
	      
	Double_t RotToZeroSubX=  0.931*m; // 0.931 (deg) -> rotation along X and Y
	Double_t RotToZeroSubY= -0.931*n; // axis to have "right angle" between Quarters

	rotSub.RotateX(RotToZeroSubX);
	rotSub.RotateY(RotToZeroSubY);

	Double_t tan_alpha1=tan(n*(TMath::Pi()/180.));
	Double_t addShiftX=FrontFaceToOffPoint*tan_alpha1;

	Double_t tan_alpha2=tan(m*(TMath::Pi()/180.));
	Double_t addShiftY=FrontFaceToOffPoint*tan_alpha2;

	//cout << "RotToZeroSubX: " << RotToZeroSubX << endl ;
	//cout << "RotToZeroSubY: " << RotToZeroSubY << endl ;    
	//cout << "Distance X=    " << addShiftX     << endl;
	//cout << "Distance Y=    " << addShiftY     << endl;
    
	Double_t ShiftX=(ShiftToZeroSub + addShiftX -0.5*sizeOfQuar);
	Double_t ShiftY=(ShiftToZeroSub + addShiftY -0.5*sizeOfQuar);
	//cout << "ShiftX == " <<ShiftX << ", ShiftY == " <<ShiftY << endl;

	trSub = new TGeoTranslation(ShiftX, ShiftY, 0.);
		
	TGeoCombiTrans* thAngle= new TGeoCombiTrans(trSub,rotSub); 
	  
	name="SubunitVol";
	name+=jj;
	thAngle->SetName(name);
	thAngle->RegisterYourself();                 
      
	QuarterVol->AddNode(SubunitVol,jj,thAngle);
      }
    }
  }
  
  trQuar  = new TGeoTranslation(0.5*sizeOfQuar,0.5*sizeOfQuar,0.);
  rotQuar = new TGeoRotation();
  
  TGeoCombiTrans* trrotQuar= new TGeoCombiTrans(trQuar,rotQuar);    
  name="QuarterVol";
  trrotQuar->SetName(name);
  trrotQuar->RegisterYourself();
  
  top->AddNode(QuarterVol,1,trrotQuar); //Quarter of EMC 
    
  for (Int_t q=1; q<=3; q++){
    TGeoCombiTrans reflection; 
    TGeoTranslation ttt;
    if (q==1){
      reflection.ReflectX(1);
      ttt = new TGeoTranslation(-0.5*sizeOfQuar,0.5*sizeOfQuar,0.);
    }
    if (q==2){
      reflection.ReflectY(1); 
      ttt = new TGeoTranslation(0.5*sizeOfQuar,-0.5*sizeOfQuar,0.);
    }
    if (q==3){
      reflection.ReflectY(1);
      reflection.ReflectX(1);
      ttt = new TGeoTranslation(-0.5*sizeOfQuar,-0.5*sizeOfQuar,0.);
    }

    top->AddNode(QuarterVol,q+1,new TGeoCombiTrans(ttt,reflection)); 
  }
  
  gGeoMan->CloseGeometry();
  top->Write();
  fi->Close();
  //gGeoManager->Export(outfile);//
  //top->Draw();//
}  
