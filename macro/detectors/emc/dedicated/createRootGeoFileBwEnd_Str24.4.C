{
   // This macro creates the Backward EndCap (BwEndCap) geometry 
   // of the EMC detector for PANDA. 
   // The straight crystals with the size of 24.4x24.4x200 mm^3 are used
   //
   // Autor: Dmitry Khaneft, Uni Minaz/GSI (10.05.2009)

  //-----------------------------
  const Double_t kWidth = 2.44;		// Width of crystal (cm)
  const Double_t kHeight = 2.44;	// Height of crystal (cm)
  const Double_t kLength = 10.0;	// Length of Crystal (cm)
  const Double_t kSizeOfCrys = 2.44;
  
  const Double_t kAlveoleThickness = 0.02;	// Thickness of alveole between Subunits (cm)
  const Double_t kGluedAlveoleThickness = 0.04;	// Thickness of glued alveole between boxes4 inside Subunits (cm)
  const Double_t kCrystalCrystal = 0.06;	// Distance Crystal-Crystal (cm)
  const Double_t kAlveoleAlveole = 0.08;	// Distance Alveole-Alveole (cm)
  const Double_t kCrystalAlveole = 0.024;	// Distance Crystal-Alveole (cm)
  
  const Int_t kNumOfQuarters = 4;
  const Int_t kNumOfSubunit = 4;
  const Int_t kNumOfBoxes4InSubunit = 4;
  const Int_t kNumOfBoxes4 = 12;
  const Int_t kNumOfBoxes2 = 10;
  const Int_t kNumOfBoxes1 = 3;
  const Int_t kNumOfCrystalsInBox4 = 4;
  const Int_t kNumOfCrystalsInBox2 = 2;
  
  Double_t sizeOfBox1 = kWidth + 2*kAlveoleThickness + 2*kCrystalAlveole;
  Double_t sizeOfBox2 = 2*kWidth + 2*kAlveoleThickness + 2*kCrystalAlveole + kCrystalCrystal; // Longer side
  Double_t sizeOfBox4 = sizeOfBox2;
  Double_t sizeOfSubunit = 2*sizeOfBox4;

  const Int_t     kNumOfQuarters = 1;       // Number of Quarter - only 1 is created, next 3 are copies at the end of the file
  const Int_t     kNumOfBoxes    = 4;       // Numbers of Boxes in one Subunit
  const Int_t     kNumOfCrystals = 4;       // Numbers of Crystals in one Box

  //--------------------------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  
  // Load this example libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libPassive");
 
  TString outfile= "./emc_module4_StraightGeo24.4.root";
  TFile* fi = new TFile(outfile,"RECREATE");  
  
  FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  geoFace->setMediaFile("/home/dkhaneft/Programs/PANDAroot/trunk/geometry/media_pnd.geo");
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
  Double_t sizeOfQuar = 37.644;  // size of one Quarter (the same in X and Y direction)
                                 // size of one Subunit (back side): 10.688*4 = 42.752 (cm) ???

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
  TGeoVolume* BoxVol5;
  TGeoVolume* BoxVol6;
  TGeoVolume* CrystalVol;     

  TString name,name1,name2,name3,name4,name5,name6,name7,name8,name9;

  // ========== QUARTER (14 subunits) ===================
  cout<< "-----------------------------------------------> Quarter VOLUME " <<endl;
  name = "QuarterShape";
  QuarterShape = new TGeoBBox(name,sizeOfQuar*0.5,sizeOfQuar*0.5,10.5,0);
  
  name = "QuarterNewVol";
  TString medium="air";
      
  QuarterVol = new TGeoVolumeAssembly(name);
      
  // ========== SUBUNIT (16 crystals) ===================
  name = "SubunitShape";
  SubunitShape = new TGeoBBox(name,2*kWidth + 2*kAlveoleThickness + kCrystalCrystal + 2*kCrystalAlveole,
  				   2*kHeight + 2*kAlveoleThickness + kCrystalCrystal + 2*kCrystalAlveole,
				   kLength,
				   0);
      
  TString medium="air";

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
  BoxShape = new TGeoBBox(name,kWidth + kAlveoleThickness + 0.5*kCrystalCrystal + kCrystalAlveole,
  			       kHeight + kAlveoleThickness + 0.5*kCrystalCrystal + kCrystalAlveole,
			       kLength,
			       0);
  
  TString medium="air";
  name="BoxVol";

  BoxVol = new TGeoVolumeAssembly(name);

  name1="BoxVol1";
  name2="BoxVol2";
  name3="BoxVol3";
  name4="BoxVol4";
  name5="BoxVol5";
  name6="BoxVol6";
  BoxVol1 = new TGeoVolumeAssembly(name1);
  BoxVol2 = new TGeoVolumeAssembly(name2);
  BoxVol3 = new TGeoVolumeAssembly(name3);
  BoxVol4 = new TGeoVolumeAssembly(name4);
  BoxVol5 = new TGeoVolumeAssembly(name5);
  BoxVol6 = new TGeoVolumeAssembly(name6);

  // Translations and Rotations for Boxes are according to the local 
  // co-ordinate system, which is in the center of the Subunit.
  // For the straight geometry, both, front & back sides are 24.4mm x 24.4mm, 
  // so, the angle = 0. (e.g. rotBox.RotateX(0.);)
  for(Int_t b=0; b<kNumOfBoxes; b++){
    cout << "     " << endl;
    cout << "----------------> BOX number: " << b <<endl;
    if (b==0) {      
      trBox = new TGeoTranslation(kSizeOfCrys + kAlveoleThickness + 0.5*kCrystalCrystal + kCrystalAlveole,
				  kSizeOfCrys + kAlveoleThickness + 0.5*kCrystalCrystal + kCrystalAlveole,
				  0.);
      rotBox = new TGeoRotation();
      rotBox.RotateX(0.);  // rotation in "up" direction 
      rotBox.RotateY(0.); // rotation in "right" direction 
    }
    if (b==1) {
      trBox = new TGeoTranslation(-kSizeOfCrys - kAlveoleThickness - 0.5*kCrystalCrystal - kCrystalAlveole,
				  -kSizeOfCrys - kAlveoleThickness - 0.5*kCrystalCrystal - kCrystalAlveole,
				  0.);
      rotBox = new TGeoRotation(); 
      rotBox.RotateX(0.); // rotation in "down" direction
      rotBox.RotateY(0.);  // rotation in "left" direction
    }
    if (b==2) {
      trBox = new TGeoTranslation(kSizeOfCrys + kAlveoleThickness + 0.5*kCrystalCrystal + kCrystalAlveole,
				  -kSizeOfCrys - kAlveoleThickness - 0.5*kCrystalCrystal - kCrystalAlveole,
				  0.);
      rotBox = new TGeoRotation();
      rotBox.RotateX(0.); // rotation in "down" direction
      rotBox.RotateY(0.); // rotation in "right" direction
    }
    if (b==3) {
      trBox = new TGeoTranslation(-kSizeOfCrys - kAlveoleThickness - 0.5*kCrystalCrystal - kCrystalAlveole,
				  kSizeOfCrys + kAlveoleThickness + 0.5*kCrystalCrystal + kCrystalAlveole,
				  0.);
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
    if (b==0){
      name+=b;
      trrotBox->SetName(name);
      trrotBox->RegisterYourself();

      SubunitVol1->AddNode(BoxVol1,b,trrotBox);
      SubunitVol4->AddNode(BoxVol2,b,trrotBox);
      SubunitVol6->AddNode(BoxVol6,b,trrotBox);
      SubunitVol7->AddNode(BoxVol4,b,trrotBox);
      SubunitVol8->AddNode(BoxVol,b,trrotBox);
      SubunitVol9->AddNode(BoxVol5,b,trrotBox);
    }
    if (b==1){
      name+=b;
      trrotBox->SetName(name);
      trrotBox->RegisterYourself();

      SubunitVol1->AddNode(BoxVol,b,trrotBox);
      SubunitVol2->AddNode(BoxVol,b,trrotBox);
      SubunitVol3->AddNode(BoxVol2,b,trrotBox);
      SubunitVol4->AddNode(BoxVol,b,trrotBox);
      SubunitVol5->AddNode(BoxVol,b,trrotBox);
      SubunitVol6->AddNode(BoxVol,b,trrotBox);
    }
    if (b==2){
      name+=b;
      trrotBox->SetName(name);
      trrotBox->RegisterYourself();

      SubunitVol1->AddNode(BoxVol,b,trrotBox);
      SubunitVol2->AddNode(BoxVol3,b,trrotBox);
      SubunitVol4->AddNode(BoxVol,b,trrotBox);
      SubunitVol5->AddNode(BoxVol2,b,trrotBox);
      SubunitVol6->AddNode(BoxVol6,b,trrotBox);
      SubunitVol8->AddNode(BoxVol,b,trrotBox);
    }
    if (b==3){
      name+=b;
      trrotBox->SetName(name);
      trrotBox->RegisterYourself();

      SubunitVol1->AddNode(BoxVol1,b,trrotBox);
      SubunitVol2->AddNode(BoxVol2,b,trrotBox);
      SubunitVol4->AddNode(BoxVol,b,trrotBox);
      SubunitVol5->AddNode(BoxVol3,b,trrotBox);
      SubunitVol6->AddNode(BoxVol,b,trrotBox);
      SubunitVol8->AddNode(BoxVol,b,trrotBox);
    }
  }
  // ========== CRYSTAL (TGeoBBox shape)=================
  name = "CrystalShape";
  CrystalShape = new TGeoBBox(name,0.5*kWidth,0.5*kHeight,kLength,0);

  TString medium="PWO";
  name = "CrystalVol";
  CrystalVol = new TGeoVolume(name,CrystalShape,gGeoMan->GetMedium(medium));

  // Translation and Rotation for each Crystal are according to 
  // co-ordinate system in the center of the Box.
  // Crystals arrangement: right angle of each Crystal is in the middle of the box.
  CrystalVol->SetLineColor(4);
  for(Int_t k=0; k<kNumOfCrystals; k++){
    cout << "               " << endl;
    cout << "         ----->  CRYSTAL number: "<< k << endl;
     if (k==0){     
	trCrystal = new TGeoTranslation(kSizeOfCrys/2. + 0.5*kCrystalCrystal,kSizeOfCrys/2. + 0.5*kCrystalCrystal,0.);
	rotCrystal = new TGeoRotation(); // "right-upper" Crystal
     }else if (k==1){
	trCrystal= new TGeoTranslation(-kSizeOfCrys/2. - 0.5*kCrystalCrystal,-kSizeOfCrys/2. - 0.5*kCrystalCrystal,0.); 
	rotCrystal = new TGeoRotation();  // "left-lower" Crystal
	rotCrystal.RotateZ(180.);      
     }else if (k==2){
	trCrystal= new TGeoTranslation(-kSizeOfCrys/2. - 0.5*kCrystalCrystal,kSizeOfCrys/2. + 0.5*kCrystalCrystal,0.); 
	rotCrystal = new TGeoRotation();  // "left-upper" Crystal
	rotCrystal.RotateZ(90.);       
     }else if (k==3){
	trCrystal= new TGeoTranslation(kSizeOfCrys/2. + 0.5*kCrystalCrystal,-kSizeOfCrys/2. - 0.5*kCrystalCrystal,0.); 
	rotCrystal = new TGeoRotation(); // "right-lower" Crystal
	rotCrystal.RotateZ(270.);      
     }

    TGeoCombiTrans* trrotCrystal= new TGeoCombiTrans(trCrystal,rotCrystal); 
    
    name = "CrystalVol";
    name+=k;
    trrotCrystal->SetName(name);
    trrotCrystal->RegisterYourself();

    BoxVol->AddNode(CrystalVol,k,trrotCrystal);

    if (k==0) {
      name+=k;
      trrotCrystal->SetName(name);
      trrotCrystal->RegisterYourself();
      
      BoxVol4->AddNode(CrystalVol,k,trrotCrystal);
      BoxVol5->AddNode(CrystalVol,k,trrotCrystal);
    }
    if (k==1) {
      name+=k;
      trrotCrystal->SetName(name);
      trrotCrystal->RegisterYourself();
      
      BoxVol1->AddNode(CrystalVol,k,trrotCrystal);
      BoxVol2->AddNode(CrystalVol,k,trrotCrystal);
      BoxVol3->AddNode(CrystalVol,k,trrotCrystal);
      BoxVol6->AddNode(CrystalVol,k,trrotCrystal);
    }
    if (k==2) {
      name+=k;
      trrotCrystal->SetName(name);
      trrotCrystal->RegisterYourself();
      
      BoxVol3->AddNode(CrystalVol,k,trrotCrystal);
      BoxVol4->AddNode(CrystalVol,k,trrotCrystal);
      BoxVol6->AddNode(CrystalVol,k,trrotCrystal);
    }
    if (k==3) {
      name+=k;
      trrotCrystal->SetName(name);
      trrotCrystal->RegisterYourself();
      
      BoxVol1->AddNode(CrystalVol,k,trrotCrystal);
      BoxVol3->AddNode(CrystalVol,k,trrotCrystal);
      BoxVol5->AddNode(CrystalVol,k,trrotCrystal);
    }
  }
  
  Int_t jj=0;
  for (Int_t row=0; row<4; row++){
    for (Int_t col=0; col<4; col++){

	Int_t flag=1;

	// *** 2 Subunits are fully disables
	if (row==3  && col==3) flag=0; // outer
	if (row==0  && col==0) flag=0; // inner

	// *** Subunits with disabled crystals
	if (row==0  && col==1) flag=-1; // middle subunit
	if (row==0  && col==3) flag=-2; // here: row==column => 1st col, 4th Subunit (top)
	if (row==1  && col==0) flag=-3; // here: row==column => 1st col, 4th Subunit (top)
	if (row==1  && col==1) flag=-4; // here: row==column => 2st col, 4th Subunit (top)
	if (row==1  && col==3) flag=-5; // here: row==column => 3st col, 3rd Subunit
	if (row==2  && col==2) flag=-6; // here: row==column => 4st col, 1st Subunit
	if (row==2  && col==3) flag=-7; // here: row==column => 4st col, 2st Subunit
	if (row==3  && col==0) flag=-8; // here: row==column => 3st col, 3rd Subunit
	if (row==3  && col==1) flag=-9; // here: row==column => 4st col, 1st Subunit
	if (row==3  && col==2) flag=-10; // here: row==column => 4st col, 2st Subunit

	Double_t ShiftX = row*(sizeOfSubunit + kAlveoleAlveole) + 0.5*sizeOfSubunit + 0.5*kAlveoleAlveole;// - 2*sizeOfSubunit - 2*kAlveoleAlveole;
	Double_t ShiftY = col*(sizeOfSubunit + kAlveoleAlveole) + 0.5*sizeOfSubunit + 0.5*kAlveoleAlveole;// - 2*sizeOfSubunit - 2*kAlveoleAlveole;

	trSub = new TGeoTranslation(ShiftX, ShiftY, 0.);
		
	TGeoCombiTrans* thAngle= new TGeoCombiTrans(trSub,rotSub); 

	if (flag==1){jj++;
	    QuarterVol->AddNode(SubunitVol,jj,thAngle);
        }
	else if (flag==-1){jj++;
	    QuarterVol->AddNode(SubunitVol7,jj,thAngle);
        }
	else if (flag==-2){jj++;
	    QuarterVol->AddNode(SubunitVol1,jj,thAngle);
        }
	else if (flag==-3){jj++;
	    QuarterVol->AddNode(SubunitVol9,jj,thAngle);
        }
	else if (flag==-4){jj++;
	    QuarterVol->AddNode(SubunitVol8,jj,thAngle);
        }
	else if (flag==-5){jj++;
	    QuarterVol->AddNode(SubunitVol2,jj,thAngle);
        }
	else if (flag==-6){jj++;
	    QuarterVol->AddNode(SubunitVol4,jj,thAngle);
        }
	else if (flag==-7){jj++;
	    QuarterVol->AddNode(SubunitVol3,jj,thAngle);
        }
	else if (flag==-8){jj++;
	    QuarterVol->AddNode(SubunitVol6,jj,thAngle);
        }
	else if (flag==-9){jj++;
	    QuarterVol->AddNode(SubunitVol5,jj,thAngle);
        }
	else if (flag==-10){jj++;
	    QuarterVol->AddNode(SubunitVol3,jj,thAngle);
        }
    }
  }
    
  trQuar  = new TGeoTranslation(0.,0.,0.);//0.5*sizeOfQuar,0.5*sizeOfQuar,0.);
  rotQuar = new TGeoRotation();
  
  TGeoCombiTrans* trrotQuar= new TGeoCombiTrans(trQuar,rotQuar);    
  name="QuarterVol";
  trrotQuar->SetName(name);
  trrotQuar->RegisterYourself();

  trrotQuar->ReflectX(1); // first copy: -X +Y
  
  top->AddNode(QuarterVol,1,trrotQuar); //right-upper Quarter 
    
  for (Int_t q=1; q<=3; q++){
    TGeoCombiTrans reflection; 
    TGeoTranslation ttt;

    if (q==1){                // second copy: -X -Y
      reflection.ReflectY(1); // right-lower Quarter   
      reflection.ReflectX(1);
      ttt = new TGeoTranslation(0.,0.,0.);//-0.5*sizeOfQuar,-0.5*sizeOfQuar,0.);
    }
    if (q==2){                // third copy: +X -Y
      reflection.ReflectY(1); // left-lower Quarter
      reflection.ReflectX(1); 
      reflection.ReflectX(1);
      ttt = new TGeoTranslation(0.,0.,0.);//0.5*sizeOfQuar,-0.5*sizeOfQuar,0.);
    }
    if (q==3){                // fourth copy: +X +Y
      reflection.ReflectX(1); // left-upper Quarter      
      reflection.ReflectX(1); 
      ttt = new TGeoTranslation(0.,0.,0.);//0.5*sizeOfQuar,0.5*sizeOfQuar,0.);
    }

    top->AddNode(QuarterVol,q+1,new TGeoCombiTrans(ttt,reflection)); 
  }
  
  gGeoMan->CloseGeometry();
  top->Write();
  fi->Close();
    gGeoManager->Export(outfile);
    top->Draw();
    gGeoManager->SetVisLevel(10);
}
