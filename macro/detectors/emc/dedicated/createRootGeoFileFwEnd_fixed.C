{
  // This macro creates the Forward EndCap (BwEndCap) geometry 
  // of the EMC detector for PANDA. 
  // The tapered crystals with the size of:
  // 24.4 x 24.4 x 200 mm^3 (front side) and 
  // 26.0 x 26.0 x 200 mm^3 (back side) are used
  //
  // Autor: Aleksandra Biegun, KVI (2.07.2009)

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

  Double_t b1 = 2.6000-0.5*tr;       // Back-tilted part (-Z direction) of the Crystal center (cm)
  Double_t b2 = 2.4375-0.5*tr;       // Front-tilted part (+Z direction) of the Crystal center (cm)
   
  Double_t dz,vert[20];              // Parameters for TGeoArb8 shape of crystal (cm)
  Double_t vert0[20],vertBox[20],vertSub[20],vertQuar[20];

  // Parameters for crystal shape:  front size:    24.375(mm) x 24.375(mm)
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
   
  TString outfile= "../../geometry/emc_module3new.root";
     
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
  Double_t sizeOfQuar = 96.192;  // size of one Quarter (the same in X and Y direction)
                                 // size of one Subunit (back side): 10.688*9 = 96.192 (cm)
  //TGeoVolume *top = gGeoMan->MakeTube("top",gGeoMan->GetMedium("air"),0,98,22.5);//cm

  TGeoVolume *top = new TGeoVolumeAssembly("Emc3");
  
  gGeoMan->SetTopVolume(top);
  
  TGeoTranslation trQuar;
  TGeoRotation rotQuar;
  
  TGeoTranslation trSub;
  TGeoTranslation trBox; 
  TGeoTranslation trCrystal; 

  TGeoRotation rotSub;
  TGeoRotation rotBox; 
  TGeoRotation rotCrystal; 
    
  TGeoShape* QuarterShape;
  TGeoShape* SubunitShape;
  TGeoShape* BoxShape;
  TGeoShape* CrystalShape;
  
  TGeoVolume* QuarterVol[4];
  TGeoVolume* SubunitVol[4];
  TGeoVolume* BoxVol[4];
  TGeoVolume* CrystalVol;     

  TString name;

  for(Int_t q=0; q<4; q++){   
    
    // ========== SUBUNIT (16 crystals) ========================
    name = "SubunitShape";
    SubunitShape = new TGeoArb8(name,dz,vertSub); 
    
    TString medium="air";
    name = "SubunitVol";
    name+=(q+1);
    //SubunitVol[q] = new TGeoVolume(name,SubunitShape,gGeoMan->GetMedium(medium));
    SubunitVol[q] = new TGeoVolumeAssembly(name);


    // ========== BOX (4 crystals) ========================
    Int_t bx[4]=0;  
    name = "BoxShape";
    BoxShape = new TGeoArb8(name,dz,vertBox); 
    
    //TString medium="carbon";
    TString medium="air";
    name="BoxVol";
    name+=(q+1);
    //BoxVol[q] = new TGeoVolume(name,BoxShape,gGeoMan->GetMedium(medium));
    BoxVol[q] = new TGeoVolumeAssembly(name);
    
    if (q==0){
      bx[0]=0;
      bx[1]=1;
      bx[2]=2;
      bx[3]=3;
    }
    if (q==1){
      bx[0]=2;
      bx[1]=3;
      bx[2]=0;
      bx[3]=1;
    }
    if (q==2){
      bx[0]=1;
      bx[1]=0;
      bx[2]=3;
      bx[3]=2;
    }
    if (q==3){
      bx[0]=3;
      bx[1]=2;
      bx[2]=1;
      bx[3]=0;
    }
    cout <<" "<<endl;
    cout <<"bx[0]= "<<bx[0]
	 <<", bx[1]= "<<bx[1]
	 <<", bx[2]= "<<bx[2]
	 <<", bx[3]= "<<bx[3]
	 <<", kNumOfBoxes = "<<kNumOfBoxes
	 <<endl;

    // Translations and Rotations for Boxes are according to co-ordinate system 
    // which is in the center of the Subunit
    for(Int_t b=0; b<kNumOfBoxes; b++){
      cout << "     " << endl;
      cout << "----------------> BOX number: " << b <<endl;
      if (b==bx[0]) {  
	trBox = new TGeoTranslation(tr+kSpaceInSub+kAlveoleThickness+0.5*kSpaceInBox,tr+kSpaceInSub+kAlveoleThickness+0.5*kSpaceInBox, 0.);
	rotBox = new TGeoRotation();    
	rotBox.RotateX(0.465518);  // rotation in "up" direction 
	rotBox.RotateY(-0.465518); // rotation in "right" direction 
      }
      if (b==bx[1]) {
	trBox = new TGeoTranslation(-tr-kSpaceInSub-kAlveoleThickness-0.5*kSpaceInBox,-tr-kSpaceInSub-kAlveoleThickness-0.5*kSpaceInBox, 0.);
	rotBox = new TGeoRotation(); 
	rotBox.RotateX(-0.465518); // rotation in "down" direction
	rotBox.RotateY(0.465518);  // rotation in "left" direction
      }
      if (b==bx[2]) {
	trBox = new TGeoTranslation(tr+kSpaceInSub+kAlveoleThickness+0.5*kSpaceInBox,-tr-kSpaceInSub-kAlveoleThickness-0.5*kSpaceInBox, 0.);
	rotBox = new TGeoRotation();
	rotBox.RotateX(-0.465518); // rotation in "down" direction
	rotBox.RotateY(-0.465518); // rotation in "right" direction  
      }
      if (b==bx[3]) {
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
      
      SubunitVol[q]->AddNode(BoxVol[q],b,trrotBox);    
    }

    // ========== CRYSTAL (TGeoArb8 shape)=================
    Int_t cr[4]=0;  
    name = "CrystalShape";
    CrystalShape = new TGeoArb8(name,dz,vert); 
    
    TString medium="PWO";
    name = "CrystalVol";
    name+=(q+1);
    CrystalVol = new TGeoVolume(name,CrystalShape,gGeoMan->GetMedium(medium));
    
    CrystalVol->SetLineColor(5);
    
    if (q==0){
      cr[0]=0;
      cr[1]=1;
      cr[2]=2;
      cr[3]=3;
    }
    if (q==1){
      cr[0]=3;
      cr[1]=2;
      cr[2]=1;
      cr[3]=0;
    }
    if (q==2){
      cr[0]=1;
      cr[1]=0;
      cr[2]=3;
      cr[3]=2;
    }
    if (q==3){
      cr[0]=2;
      cr[1]=3;
      cr[2]=0;
      cr[3]=1;
    }
    /*cout <<" "<<endl;
    cout <<"->cr[0]= "<<cr[0]
	 <<", cr[1]= "<<cr[1]
	 <<", cr[2]= "<<cr[2]
	 <<", cr[3]= "<<cr[3]
	 <<", kNumOfCrystals = "<<kNumOfCrystals
	 <<endl;
    */
    for(Int_t k=0; k<kNumOfCrystals; k++){    
      if (k==cr[0]){     
	trCrystal = new TGeoTranslation(tr/2.+0.5*kSpaceInBox,tr/2.+0.5*kSpaceInBox,0.);
	rotCrystal = new TGeoRotation() ; // "right-upper" Crystal
      }else if (k==cr[1]){
	trCrystal= new TGeoTranslation(-tr/2.-0.5*kSpaceInBox,-tr/2.-0.5*kSpaceInBox,0.); 
	rotCrystal = new TGeoRotation();  // "left-lower" Crystal
	rotCrystal.RotateZ(180.);      
      }else if (k==cr[2]){
	trCrystal= new TGeoTranslation(-tr/2.-0.5*kSpaceInBox,tr/2.+0.5*kSpaceInBox,0.); 
	rotCrystal = new TGeoRotation();  // "left-upper" Crystal
	rotCrystal.RotateZ(90.);       
      }else if (k==cr[3]){
	trCrystal= new TGeoTranslation(tr/2.+0.5*kSpaceInBox,-tr/2.-0.5*kSpaceInBox,0.); 
	rotCrystal = new TGeoRotation() ; // "right-lower" Crystal
	rotCrystal.RotateZ(270.);      
      }
      
      TGeoCombiTrans* trrotCrystal= new TGeoCombiTrans(trCrystal,rotCrystal); 
      name = "CrystalVol";
      name+=k;
      trrotCrystal->SetName(name);
      trrotCrystal->RegisterYourself();
      
      BoxVol[q]->AddNode(CrystalVol,k,trrotCrystal);
    }

    Double_t ShiftToZeroSub= kSpaceInSub + kAlveoleThickness;
    Double_t kSubShift=10.363;        // size of 1 Subunit (cm) in its half Z-distance
    Double_t FrontFaceToOffPoint=295; // Off_point -> front_face_of_crystals distance (cm)
 
    Int_t cp_row[4]=0;
    Int_t cp_col[4]=0;  

    // ========== QUARTER (55 subunits) ===================
    cout << "  "
	 << "  " 
	 <<endl;
    cout<< "------------------------> Quarter VOLUME ** " <<q+1<<" ** is created."<<endl;
    name = "QuarterVol";
    name+=(q+1);
    QuarterVol[q] = new TGeoVolumeAssembly(name); 
    
    TString medium="air";
    cout << "Quarter Medium ==== "<< gGeoMan->GetMedium(medium)->GetName()<< endl;
    
    Int_t jj=0;
    for (Int_t row=0; row<9; row++){  // <-- tj. col!!
      for (Int_t col=0; col<9; col++){ // <-- tj.row !!
      	
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
	  
	  if (q==0){       // first quarter 
	    cp_row[q]=n;   // X
	    cp_col[q]=m;   // Y
	  }
	  if (q==1){       // second quarter 
	    cp_row[q]=n;   // X
	    cp_col[q]=-m;  //-Y
	  }
	  if (q==2){       // third quarter 
	    cp_row[q]=-n;  //-X
	    cp_col[q]=-m;  //-Y
	  }
	  if (q==3){       // fourth quarter 
	    cp_row[q]=-n;  //-X
	    cp_col[q]=m;   // Y
	  }

	  //cout << " X: "<<cp_row[q]
	  //<< " Y: "<<cp_col[q]
	  //<< endl;

	  Double_t RotToZeroSubX=  0.931*cp_col[q]; // 0.931 (deg) -> rotation along X and Y
	  Double_t RotToZeroSubY= -0.931*cp_row[q]; // axis to have "right angle" between Quarters

	  rotSub.RotateX(RotToZeroSubX);
	  rotSub.RotateY(RotToZeroSubY);
	  
	  Double_t tan_alpha1=tan(cp_row[q]*(TMath::Pi()/180.));
	  Double_t addShiftX=FrontFaceToOffPoint*tan_alpha1;

	  Double_t tan_alpha2=tan(cp_col[q]*(TMath::Pi()/180.));
	  Double_t addShiftY=FrontFaceToOffPoint*tan_alpha2;

	  Double_t ShiftX=(ShiftToZeroSub + addShiftX -0.5*sizeOfQuar);
	  Double_t ShiftY=(ShiftToZeroSub + addShiftY -0.5*sizeOfQuar);

	  trSub = new TGeoTranslation(ShiftX, ShiftY, 0.);
		
	  TGeoCombiTrans* thAngle= new TGeoCombiTrans(trSub,rotSub); 
	  
	  name="SubunitVol";
	  name+=jj;
	  thAngle->SetName(name);
	  thAngle->RegisterYourself();                 
	  
	  QuarterVol[q]->AddNode(SubunitVol[q],jj,thAngle); // right-upper Quarter
	}
      }
    }
    
    trQuar  = new TGeoTranslation(0.5*sizeOfQuar,0.5*sizeOfQuar,0.);
    rotQuar = new TGeoRotation();
  
    TGeoCombiTrans* trrotQuar= new TGeoCombiTrans(trQuar,rotQuar);    
    name = "QuarterVol";
    name+=(q+1);
    trrotQuar->SetName(name);
    trrotQuar->RegisterYourself();
  
    top->AddNode(QuarterVol[q],q+1,trrotQuar); //Quarter of EMC 
  }
  /*
  cout <<"             "<< endl;
  cout <<"*************************************** "<< endl;
  TString nameBoxVol0 = BoxVol[0]->GetName();
  TString nameBoxVol1 = BoxVol[1]->GetName();
  TString nameBoxVol2 = BoxVol[2]->GetName();
  TString nameBoxVol3 = BoxVol[3]->GetName();
  cout <<"Checking overlaps for --------->>>> "<< nameBoxVol0 <<endl ;
  cout <<"-> BOX Volume has:  " <<BoxVol[0]->GetNdaughters() <<" Crystals"<<endl;
  gGeoManager->GetVolume(nameBoxVol0)->CheckOverlaps(0.001);
  cout <<"Number of Overlaps for "<< nameBoxVol0 <<" is:  "<<gGeoManager->PrintOverlaps()<<endl ;
  
  cout <<"Checking overlaps for --------->>>> "<< nameBoxVol1 <<endl ;
  cout <<"-> BOX Volume has:  " <<BoxVol[1]->GetNdaughters() <<" Crystals"<<endl;
  gGeoManager->GetVolume(nameBoxVol1)->CheckOverlaps(0.001);
  cout <<"Number of Overlaps for "<< nameBoxVol1 <<" is:  "<<gGeoManager->PrintOverlaps()<<endl ;
  
  cout <<"Checking overlaps for --------->>>> "<< nameBoxVol2 <<endl ;
  cout <<"-> BOX Volume has:  " <<BoxVol[2]->GetNdaughters() <<" Crystals"<<endl;
  gGeoManager->GetVolume(nameBoxVol2)->CheckOverlaps(0.001);
  cout <<"Number of Overlaps for "<< nameBoxVol2 <<" is:  "<<gGeoManager->PrintOverlaps()<<endl ;
  
  cout <<"Checking overlaps for --------->>>> "<< nameBoxVol3 <<endl ;
  cout <<"-> BOX Volume has:  " <<BoxVol[3]->GetNdaughters() <<" Crystals"<<endl;
  gGeoManager->GetVolume(nameBoxVol3)->CheckOverlaps(0.001);
  cout <<"Number of Overlaps for "<< nameBoxVol3 <<" is:  "<<gGeoManager->PrintOverlaps()<<endl ;

  
  cout <<"             "<< endl;
  cout <<"*************************************** "<< endl;
  TString nameSubVol0 = SubunitVol[0]->GetName();
  TString nameSubVol1 = SubunitVol[1]->GetName();
  TString nameSubVol2 = SubunitVol[2]->GetName();
  TString nameSubVol3 = SubunitVol[3]->GetName();
  cout <<"Checking overlaps for --------->>>> "<< nameSubVol0 <<endl ;
  cout <<"-> SUBUNIT Volume has:  " <<SubunitVol[0]->GetNdaughters()<<" Boxes"<<endl;
  gGeoManager->GetVolume(nameSubVol0)->CheckOverlaps(0.001);
  cout <<"Number of Overlaps for "<< nameSubVol0 <<" is:  "<<gGeoManager->PrintOverlaps()<<endl ;

  cout <<"Checking overlaps for --------->>>> "<< nameSubVol1 <<endl ;
  cout <<"-> SUBUNIT Volume has:  " <<SubunitVol[1]->GetNdaughters()<<" Boxes"<<endl;
  gGeoManager->GetVolume(nameSubVol1)->CheckOverlaps(0.001);
  cout <<"Number of Overlaps for "<< nameSubVol1 <<" is:  "<<gGeoManager->PrintOverlaps()<<endl ;
  
  cout <<"Checking overlaps for --------->>>> "<< nameSubVol2 <<endl ;
  cout <<"-> SUBUNIT Volume has:  " <<SubunitVol[2]->GetNdaughters()<<" Boxes"<<endl;
  gGeoManager->GetVolume(nameSubVol2)->CheckOverlaps(0.001);
  cout <<"Number of Overlaps for "<< nameSubVol2 <<" is:  "<<gGeoManager->PrintOverlaps()<<endl ;

  cout <<"Checking overlaps for --------->>>> "<< nameSubVol3 <<endl ;
  cout <<"-> SUBUNIT Volume has:  " <<SubunitVol[3]->GetNdaughters()<<" Boxes"<<endl;
  gGeoManager->GetVolume(nameSubVol3)->CheckOverlaps(0.001);
  cout <<"Number of Overlaps for "<< nameSubVol3 <<" is:  "<<gGeoManager->PrintOverlaps()<<endl ;

  cout <<"             "<< endl;
  cout <<"*************************************** "<< endl;
  TString nameQuarVol0 = QuarterVol[0]->GetName();
  TString nameQuarVol1 = QuarterVol[1]->GetName();
  TString nameQuarVol2 = QuarterVol[2]->GetName();
  TString nameQuarVol3 = QuarterVol[3]->GetName();
  cout <<"Checking overlaps for --------->>>> "<< nameQuarVol0 <<endl ;
  cout <<"-> QUARTER Volume has:  " <<QuarterVol[0]->GetNdaughters()<< " Subunits"<<endl;
  gGeoManager->GetVolume(nameQuarVol0)->CheckOverlaps(0.001);
  cout <<"Number of Overlaps for "<< nameQuarVol0 <<" is:  "<<gGeoManager->PrintOverlaps()<<endl ;

  cout <<"Checking overlaps for --------->>>> "<< nameQuarVol1 <<endl ;
  cout <<"-> QUARTER Volume has:  " <<QuarterVol[1]->GetNdaughters()<< " Subunits"<<endl;
  gGeoManager->GetVolume(nameQuarVol1)->CheckOverlaps(0.001);
  cout <<"Number of Overlaps for "<< nameQuarVol1 <<" is:  "<<gGeoManager->PrintOverlaps()<<endl ;

  cout <<"Checking overlaps for --------->>>> "<< nameQuarVol2 <<endl ;
  cout <<"-> QUARTER Volume has:  " <<QuarterVol[2]->GetNdaughters()<< " Subunits"<<endl;
  gGeoManager->GetVolume(nameQuarVol2)->CheckOverlaps(0.001);
  cout <<"Number of Overlaps for "<< nameQuarVol2 <<" is:  "<<gGeoManager->PrintOverlaps()<<endl ;

  cout <<"Checking overlaps for --------->>>> "<< nameQuarVol3 <<endl ;
  cout <<"-> QUARTER Volume has:  " <<QuarterVol[3]->GetNdaughters()<< " Subunits"<<endl;
  gGeoManager->GetVolume(nameQuarVol3)->CheckOverlaps(0.001);
  cout <<"Number of Overlaps for "<< nameQuarVol3 <<" is:  "<<gGeoManager->PrintOverlaps()<<endl ;
  
  cout <<"             "<< endl;
  cout <<"*************************************** "<< endl;
  TString nameTopVol = top->GetName();
  cout <<"Checking overlaps for --------->>>> "<< nameTopVol <<endl ;
  cout <<"-> TOP Volume has:  " <<top->GetNdaughters() <<" Quarters"<<endl;
  gGeoManager->GetVolume(nameTopVol)->CheckOverlaps(0.001);
  cout <<"Number of Overlaps for TOP Volume is: " <<gGeoManager->PrintOverlaps()<<endl;
  
  cout<<"   "<<endl;
  */



  gGeoMan->CloseGeometry();
  top->Write();
  fi->Close();
  //gGeoManager->Export(outfile);//
  top->Draw();//
}  
