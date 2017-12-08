//-------------------------------------------------------------//
  // This macro creates the Backward EndCap (BwEndCap) geometry 
  // of the EMC detector for PANDA. 
  //
  // Autor: Luigi Capozza, HIM/Uni Mainz (10.05.2017)
  //-------------------------------------------------------------//


//------------------------------------------------------ definition of constants

//----------------------------- outer/inner BWEC volume

//--- dimensions
static const Double_t mm = 1.0e-1; // 1 mm in cm
static const Double_t kBWECzLength     = 436.0*mm;
static const Double_t kBWECinnerRadius = 165.0*mm;
static const Double_t kBWECouterRadius = 438.0*mm;
static const Double_t kBWECxyWidth     = 401.6*mm;

static const Double_t kMPThickness    =  30.0*mm;
static const Double_t kCoverThickness =  4.0*mm;
static const Double_t kInnerVolLength =  kBWECzLength - kMPThickness - kCoverThickness;


//--- positions
static const Double_t kInnerVolZpos = 0.5*(kMPThickness-kCoverThickness);// inside the outer volume



//----------------------------- Alveole w/ PWO crystals + capsules + inserts

//--- dimensions
static const Double_t kCrWidth  =  24.4*mm;  // Width of crystal
static const Double_t kCrHeight =  24.4*mm;  // Height of crystal
static const Double_t kCrLength = 200.0*mm;	// Length of Crystal

static const Double_t kCapsuleLength = 42.0*mm; // Length of Al capsule (containing the APFEL preamp)

static const Double_t kAlvHoleWidth = kCrWidth + 0.5*mm; // Width of alveole hole containing one crystal
static const Double_t kAlvLength = kCrLength + kCapsuleLength; // Alveole's length
static const Double_t kAlvWallThickness = 0.3*mm; // Wall thickness of carbon fibre alveoles
static const Double_t k4x4AlvWidth = 4.*kAlvHoleWidth + 5.*kAlvWallThickness; // Outer width of a 4x4 alveole
static const Double_t kSingleAlvWidth = kAlvHoleWidth + 2.*kAlvWallThickness; // Outer width of a 1x1 alveole

static const Double_t kInsertLength  = 45.0*mm; // Length (in z direction) of a Al insert
static const Double_t kInsAlvDiff    = 0.45*mm; // distance between outer surf of insert and of alveole
static const Double_t kInsertWidth   = k4x4AlvWidth - 2.*kInsAlvDiff; // Width of a Al insert
static const Double_t kInsHoleWidth1 = 0.25*kInsertWidth - 2.*5.0*mm; // Width of Al insert' hole for a 1x1 crystals
static const Double_t kInsHoleWidth2 = 0.50*kInsertWidth - 2.*5.0*mm; // Width of Al insert' hole for a 2x2 crystals


//--- positions
static const Double_t firstCrystPosX = -1.5*(kAlvHoleWidth +  kAlvWallThickness);
static const Double_t firstCrystPosY =  1.5*(kAlvHoleWidth +  kAlvWallThickness);
static const Double_t crystDPos = kAlvHoleWidth +  kAlvWallThickness;




//----------------------------- Submodule layouts derived from the basic 4x4 array
const Int_t kNSubmoduleType = 6;

//--- 1: crystal present, 0: no crystal

static const Int_t AlvLayout0[]={1,1,1,1,
				 1,1,1,1,
				 1,1,1,1,
				 1,1,1,1};

static const Int_t AlvLayout1[]={1,1,1,1,
				 1,1,1,1,
				 1,1,1,1,
				 1,1,0,0};

static const Int_t AlvLayout2[]={1,1,1,1,
				 1,1,1,1,
				 1,1,1,1,
				 0,0,0,0};

static const Int_t AlvLayout3[]={1,1,1,1,
				 1,1,1,1,
				 1,1,1,1,
				 0,0,1,1};

static const Int_t AlvLayout4[]={1,1,0,0,
				 1,1,1,0,
				 1,1,1,1,
				 1,1,1,1};

static const Int_t AlvLayout5[]={0,0,0,0,
				 0,0,0,0,
				 0,0,0,0,
				 1,0,0,0};


//----------------------------- Submodule shapes are implented as TGeoXtru objects
//--- Here the data for the polygonal sections for alveoles and inserts

//--- submodule type 0
static const Int_t nVtx0 = 4;
static const  Double_t v0X[nVtx0] = {
  -0.5*k4x4AlvWidth, +0.5*k4x4AlvWidth, 
  +0.5*k4x4AlvWidth, -0.5*k4x4AlvWidth};
static const Double_t v0Y[nVtx0] = {
  -0.5*k4x4AlvWidth, -0.5*k4x4AlvWidth, 
  +0.5*k4x4AlvWidth, 0.5*k4x4AlvWidth};


//--- submodule type 1
static const Int_t nVtx1 = 6;
static const  Double_t v1X[nVtx1] = {
  -0.5*k4x4AlvWidth, 
  +0.5*kAlvWallThickness, 0.5*kAlvWallThickness, 
  +0.5*k4x4AlvWidth, 0.5*k4x4AlvWidth,
  -0.5*k4x4AlvWidth};
static const Double_t v1Y[nVtx1] = {
  -0.5*k4x4AlvWidth, -0.5*k4x4AlvWidth, 
  -0.5*k4x4AlvWidth + kAlvWallThickness + kAlvHoleWidth,
  -0.5*k4x4AlvWidth + kAlvWallThickness + kAlvHoleWidth,
  +0.5*k4x4AlvWidth, 0.5*k4x4AlvWidth};


//--- submodule type 2
static const Int_t nVtx2 = 4;
static const Double_t v2X[nVtx2] = {
  -0.5*k4x4AlvWidth,  0.5*k4x4AlvWidth, 
  +0.5*k4x4AlvWidth, -0.5*k4x4AlvWidth};
static const Double_t v2Y[nVtx2] = {
  -0.5*k4x4AlvWidth + kAlvHoleWidth + kAlvWallThickness, 
  -0.5*k4x4AlvWidth + kAlvHoleWidth + kAlvWallThickness, 
  +0.5*k4x4AlvWidth,  0.5*k4x4AlvWidth};

//--- submodule type 3
static const Int_t nVtx3 = 6;
static const  Double_t v3X[nVtx3] = {
  -0.5*k4x4AlvWidth, 
  -0.5*kAlvWallThickness, -0.5*kAlvWallThickness, 
  +0.5*k4x4AlvWidth, 0.5*k4x4AlvWidth,
  -0.5*k4x4AlvWidth};
static const Double_t v3Y[nVtx3] = {
  -0.5*k4x4AlvWidth + kAlvWallThickness + kAlvHoleWidth,
  -0.5*k4x4AlvWidth + kAlvWallThickness + kAlvHoleWidth,
  -0.5*k4x4AlvWidth, -0.5*k4x4AlvWidth, 
  +0.5*k4x4AlvWidth,  0.5*k4x4AlvWidth};

//--- submodule type 4
static const Int_t nVtx4 = 8;
static const  Double_t v4X[nVtx4] = {
  -0.5*k4x4AlvWidth, 
  +0.5*k4x4AlvWidth, 0.5*k4x4AlvWidth,
  +0.5*k4x4AlvWidth - kAlvWallThickness - kAlvHoleWidth,
  +0.5*k4x4AlvWidth - kAlvWallThickness - kAlvHoleWidth,
  +0.5*kAlvWallThickness, 0.5*kAlvWallThickness, 
  -0.5*k4x4AlvWidth};
static const Double_t v4Y[nVtx4] = {
  -0.5*k4x4AlvWidth, -0.5*k4x4AlvWidth, 
  +0.5001*kAlvWallThickness, 0.5001*kAlvWallThickness, // extra micron to avoid representation error
  +0.5*k4x4AlvWidth - kAlvWallThickness - kAlvHoleWidth,
  +0.5*k4x4AlvWidth - kAlvWallThickness - kAlvHoleWidth,
  +0.5*k4x4AlvWidth,  0.5*k4x4AlvWidth};

//--- submodule type 5
static const Int_t nVtx5 = 4;
static const Double_t v5X[nVtx5] = {
  -0.5*k4x4AlvWidth,  
  -0.5*k4x4AlvWidth + kSingleAlvWidth, 
  -0.5*k4x4AlvWidth + kSingleAlvWidth, 
  -0.5*k4x4AlvWidth};
static const Double_t v5Y[nVtx5] = {
  -0.5*k4x4AlvWidth, -0.5*k4x4AlvWidth,  
  -0.5*k4x4AlvWidth + kSingleAlvWidth, 
  -0.5*k4x4AlvWidth + kSingleAlvWidth};



//----------------------------- Positions of submodules in the calorimeter
static const Int_t kNsubmodQuarter = 10;
static const Double_t kOuterSideR = 372.0*mm;
static const Double_t kDistBtwAlvs = 0.55*mm;
static const Double_t kSubmodPosUnit = k4x4AlvWidth + kDistBtwAlvs;
static const Double_t kModuleZpos = 51.0*mm;
static const Double_t kCornerShift = 16.4*mm;

//---  submodules on the upper side
static const Double_t kSubmodPos0[3] = {0.0*kSubmodPosUnit, kOuterSideR - 0.5*k4x4AlvWidth                , kModuleZpos};
static const Double_t kSubmodPos1[3] = {-1.*kSubmodPosUnit, kOuterSideR - 0.5*k4x4AlvWidth                , kModuleZpos};
static const Double_t kSubmodPos2[3] = {+1.*kSubmodPosUnit, kOuterSideR - 0.5*k4x4AlvWidth                , kModuleZpos};
static const Double_t kSubmodPos3[3] = {-1.*kSubmodPosUnit, kOuterSideR - 1.5*k4x4AlvWidth - kDistBtwAlvs , kModuleZpos};
static const Double_t kSubmodPos4[3] = {0.0*kSubmodPosUnit, kOuterSideR - 1.5*k4x4AlvWidth - kDistBtwAlvs , kModuleZpos};
static const Double_t kSubmodPos5[3] = {+1.*kSubmodPosUnit, kOuterSideR - 1.5*k4x4AlvWidth - kDistBtwAlvs , kModuleZpos};

//---  submodules on the upper-right corner
static const Double_t kSubmodPos6[3] = {+2.*kSubmodPosUnit, kOuterSideR - 1.5*k4x4AlvWidth - kDistBtwAlvs - kCornerShift, kModuleZpos};
static const Double_t kSubmodPos7[3] = {+3.*kSubmodPosUnit, kOuterSideR - 1.5*k4x4AlvWidth - kDistBtwAlvs - kCornerShift, kModuleZpos};
static const Double_t kSubmodPos8[3] = {+2.*kSubmodPosUnit, kOuterSideR - 0.5*k4x4AlvWidth                - kCornerShift, kModuleZpos};
static const Double_t kSubmodPos9[3] = {+3.*kSubmodPosUnit, kOuterSideR - 0.5*k4x4AlvWidth                - kCornerShift, kModuleZpos};

// Simplification on y of kSubmodPos9: the 1x1 alveole should be glued
// to its neighbour in vertical direction, therefore an extra term of
// kDistBtwAlvs (0.55 mm) should be used. This term is left out here.
// This way the arrangement will be just rotated for the other corners
// without modification of the "loose" crystal position.




//------------------------------------------------------ VIPs and cooling shells

//--- dimensions
static const Double_t kCrystVIPDist          =  4.0*mm;
static const Double_t kFrontVIPThickness     = 10.0*mm;
static const Double_t kFrontCoolingThickness = 15.0*mm;

static const Double_t kSideVIPThickness    = 10.0*mm;
static const Double_t kSideVIPRout         = kBWECouterRadius-kCoverThickness;
static const Double_t kSideVIPRin          = kSideVIPRout - kSideVIPThickness;
static const Double_t kSideVIPxyOuterWidth = kBWECxyWidth - kCoverThickness;
static const Double_t kSideVIPxyInnerWidth = kSideVIPxyOuterWidth - kSideVIPThickness;
static const Double_t kSideVIPLength       = kInnerVolLength;

static const Double_t kInnerVIPRin  = kBWECinnerRadius+kCoverThickness;
static const Double_t kInnerVIPRout = kInnerVIPRin + kSideVIPThickness;

static const Double_t kSideCoolingThickness = 15.0*mm;

//--- positions
static const Double_t kFrontVIPZpos = 0.5*(kBWECzLength - kMPThickness - kCoverThickness - kFrontVIPThickness);
static const Double_t kFrontCoolingZpos = 0.5*(kBWECzLength - kMPThickness - kCoverThickness - 2.0*kFrontVIPThickness - kFrontCoolingThickness);
static const Double_t kSideVIPZpos = 0.0;





//------------------------------------------------------ end definition of constants


//------------------------------------------------------ geometry construction macro
int createRootGeoFileBwEnd_2017(){

  
  //----------------------------- load/create materials/media
  FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  geoFace->setMediaFile("media_pnd.geo");
  geoFace->readMedia();
  geoFace->print();

  FairGeoMedia *Media =  geoFace->getMedia();
  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();

  FairGeoMedium *N2      = Media->getMedium("Nitrogen");
  FairGeoMedium *PWO     = Media->getMedium("PWO");
  FairGeoMedium *Prepreg = Media->getMedium("Prepreg");
  FairGeoMedium *Alu     = Media->getMedium("aluminium");
  FairGeoMedium *VIPmat  = Media->getMedium("FumedSilica");
  FairGeoMedium *CSmat   = Media->getMedium("CoolingShellMix");

  Int_t nmed=geobuild->createMedium(N2);
  nmed=geobuild->createMedium(PWO);
  nmed=geobuild->createMedium(Prepreg);
  nmed=geobuild->createMedium(Alu);
  nmed=geobuild->createMedium(VIPmat);
  nmed=geobuild->createMedium(CSmat);

  
  //----------------------------- define mother volume
  TGeoManager* gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");

  TGeoVolume *top = new TGeoVolumeAssembly("Emc4");
                    
  gGeoMan->SetTopVolume(top);


  //----------------------------- outer/inner BWEC volume
  TGeoTube *outerTube = new TGeoTube("outerTube",kBWECinnerRadius,kBWECouterRadius,0.5*kBWECzLength);
  TGeoBBox *outerBox = new TGeoBBox("outerBox",kBWECxyWidth,kBWECxyWidth,0.5*kBWECzLength+0.1);
  TGeoCompositeShape *outerVol = new TGeoCompositeShape("outerVol","outerTube*outerBox");

  TGeoVolume* BWECouterVol = new TGeoVolume("BWECouterVol",outerVol,gGeoMan->GetMedium("aluminium"));
  BWECouterVol->SetLineColor(kGray);
  BWECouterVol->SetLineWidth(5);
  BWECouterVol->SetLineStyle(kDashed);
  cout<<"transp: "<<BWECouterVol->GetTransparency()<<endl;

  top->AddNode(BWECouterVol,0);


  TGeoTube *innerTube = new TGeoTube("innerTube",
				     kBWECinnerRadius+kCoverThickness,
				     kBWECouterRadius-kCoverThickness,
				     0.5*kInnerVolLength);
				     
  TGeoBBox *innerBox = new TGeoBBox("innerBox",
				    kBWECxyWidth-kCoverThickness,
				    kBWECxyWidth-kCoverThickness,
				    0.5*kInnerVolLength+0.1);
  
  TGeoCompositeShape *innerVol = new TGeoCompositeShape("innerVol","innerTube*innerBox");
  TGeoTranslation *t1 = new TGeoTranslation(0.0,0.0,kInnerVolZpos);
  
  TGeoVolume* BWECinnerVol = new TGeoVolume("BWECinnerVol",innerVol,gGeoMan->GetMedium("Nitrogen"));
  BWECinnerVol->SetLineColor(kAzure+10);

  BWECouterVol->AddNode(BWECinnerVol,0,t1);




  //----------------------------- submodules with alveoles, crystals, capsules...
  //--- version with composite shapes for the alveoles (holes are subtracted volumes)
  int crystalNo = 0;
  int holeNo = 0;
  int capsuleNo = 0;


  //--- submodule type 0: 4x4 array
  TGeoBBox *alveoleBox = new TGeoBBox("alveoleBox",0.5*k4x4AlvWidth,0.5*k4x4AlvWidth,0.5*kAlvLength);
  const Double_t *vX[kNSubmoduleType] = {v0X,v1X,v2X,v3X,v4X,v5X};
  const Double_t *vY[kNSubmoduleType] = {v0Y,v1Y,v2Y,v3Y,v4Y,v5Y};
  
  //--- submodule type 1
  TGeoXtru *alvXtru1 = new TGeoXtru(2); alvXtru1->SetName("alvXtru1");
  alvXtru1->DefinePolygon(nVtx1,v1X,v1Y);
  alvXtru1->DefineSection(0,-0.5*kAlvLength,0.0,0.0,1.0);
  alvXtru1->DefineSection(1,+0.5*kAlvLength,0.0,0.0,1.0);
  
  //--- submodule type 2
  TGeoXtru *alvXtru2 = new TGeoXtru(2); alvXtru2->SetName("alvXtru2");
  alvXtru2->DefinePolygon(nVtx2,v2X,v2Y);
  alvXtru2->DefineSection(0,-0.5*kAlvLength,0.0,0.0,1.0);
  alvXtru2->DefineSection(1,+0.5*kAlvLength,0.0,0.0,1.0);

  //--- submodule type 3
  TGeoXtru *alvXtru3 = new TGeoXtru(2); alvXtru3->SetName("alvXtru3");
  alvXtru3->DefinePolygon(nVtx3,v3X,v3Y);
  alvXtru3->DefineSection(0,-0.5*kAlvLength,0.0,0.0,1.0);
  alvXtru3->DefineSection(1,+0.5*kAlvLength,0.0,0.0,1.0);

  //--- submodule type 4
  TGeoXtru *alvXtru4 = new TGeoXtru(2); alvXtru4->SetName("alvXtru4");
  alvXtru4->DefinePolygon(nVtx4,v4X,v4Y);
  alvXtru4->DefineSection(0,-0.5*kAlvLength,0.0,0.0,1.0);
  alvXtru4->DefineSection(1,+0.5*kAlvLength,0.0,0.0,1.0);

  //--- "loose" crystals in the corners or submodule type 5
  TGeoXtru *alvXtru5 = new TGeoXtru(2); alvXtru5->SetName("alvXtru5");
  alvXtru5->DefinePolygon(nVtx5,v5X,v5Y);
  alvXtru5->DefineSection(0,-0.5*kAlvLength,0.0,0.0,1.0);
  alvXtru5->DefineSection(1,+0.5*kAlvLength,0.0,0.0,1.0);

  
  //--- shape of alveole hole to be subtracted from the bulk alveole shapes
  TGeoBBox *alvHoleBox = new TGeoBBox("alvHoleBox",0.5*kAlvHoleWidth,0.5*kAlvHoleWidth,0.5*kAlvLength+0.1*mm);

  //--- gathering shapes and layout maps (AlvLayout#) into arrays
  const Int_t *AlvLayout[kNSubmoduleType] = {AlvLayout0,AlvLayout1,AlvLayout2,AlvLayout3,AlvLayout4,AlvLayout5};
  TGeoShape *alveoleBulkShape[]={alveoleBox,alvXtru1,alvXtru2,alvXtru3,alvXtru4,alvXtru5};
  TGeoCompositeShape *alveoleShape[kNSubmoduleType];

  TString tname,holeVolFormula[kNSubmoduleType];
  for(Int_t i=0; i<kNSubmoduleType; ++i) holeVolFormula[i] = "";
  TGeoTranslation* holeInAlvXYtrans;

  //--- definition of subtraction formulas for the alveole types
  for(Int_t i=0; i<16;++i){
    tname = "t"; tname += i;
    holeInAlvXYtrans = new TGeoTranslation(tname,firstCrystPosX + (i%4)*crystDPos, firstCrystPosY - (i/4)*crystDPos, 0.0);
    holeInAlvXYtrans->RegisterYourself();
    for(Int_t k=0; k<kNSubmoduleType; ++k){
      if(AlvLayout[k][i]){
	if(holeVolFormula[k] != "") holeVolFormula[k] += " + ";
	holeVolFormula[k] +=  "alvHoleBox:"+tname;
      }
    }
  }

  //--- subtraction of alveole holes
  for(Int_t i=0; i<kNSubmoduleType; ++i){
    TString shapeName = "alveoleShape"; shapeName += i;
    TString shapeFormula = TString(alveoleBulkShape[i]->GetName()) + " - (" +  holeVolFormula[i] + ")";
    alveoleShape[i] = new TGeoCompositeShape(shapeName,shapeFormula);
    cout<<"Subtraction formula for alveole type "<<i<<": "<<shapeFormula<<endl;
  }

  
  //--- the alveole volumes
  TGeoVolume* BWECalveole[kNSubmoduleType];
  for(Int_t i=0; i<kNSubmoduleType; ++i){
    TString volName = "BWECalveole"; volName += i;
    BWECalveole[i] = new TGeoVolume(volName, alveoleShape[i], gGeoMan->GetMedium("Prepreg"));
    BWECalveole[i]->SetLineColor(kGray+3);
  }


  //--- the crystal and Al capsule volumes and attributes
  TGeoBBox *crystalBox = new TGeoBBox("crystalBox",0.5*kCrWidth,0.5*kCrHeight,0.5*kCrLength);
  TGeoBBox *capsuleBox = new TGeoBBox("capsuleBox",0.5*kCrWidth,0.5*kCrHeight,0.5*kCapsuleLength);

  TGeoVolume* PWOcrystal    = new TGeoVolume("PWOcrystal",crystalBox,gGeoMan->GetMedium("PWO"));
  TGeoVolume* crystCapsule  = new TGeoVolume("crystCapsule",capsuleBox,gGeoMan->GetMedium("aluminium"));

  TGeoTranslation* crystInAlvZtrans   = new TGeoTranslation(0.0,0.0,0.5*kCapsuleLength);
  TGeoTranslation* capsuleInAlvZtrans = new TGeoTranslation(0.0,0.0,-0.5*kCrLength);

  PWOcrystal     -> SetLineColor(kWhite);
  crystCapsule   -> SetLineColor(kGray);


  //--- Al insert vol
  TGeoBBox *insertBox  = new TGeoBBox("insertBox", 0.5*kInsertWidth,0.5*kInsertWidth,0.5*kInsertLength); // 4x4 alveole bulk
  TGeoBBox *insHoleBox1 = new TGeoBBox("insHoleBox1",0.5*kInsHoleWidth1,0.5*kInsHoleWidth1,0.5*kInsertLength+0.1); // 1x1 hole
  TGeoBBox *insHoleBox2 = new TGeoBBox("insHoleBox2",0.5*kInsHoleWidth2,0.5*kInsHoleWidth1,0.5*kInsertLength+0.1); // 2x1 hole
  TGeoBBox *insHoleBox4 = new TGeoBBox("insHoleBox4",0.5*kInsHoleWidth2,0.5*kInsHoleWidth2,0.5*kInsertLength+0.1); // 2x2 hole

  Double_t insVX[kNSubmoduleType][16],insVY[kNSubmoduleType][16];
  Int_t nVtx[kNSubmoduleType] = {nVtx0,nVtx1,nVtx2,nVtx3,nVtx4,nVtx5};
  TGeoXtru* insertXtru[kNSubmoduleType];
  TGeoCompositeShape* insertShape[kNSubmoduleType];

  // Translations for holes and composition formulas
  const Int_t nInsHoleTrans = 8;
  TGeoTranslation* insHoleTrans[nInsHoleTrans];
  insHoleTrans[0] = new TGeoTranslation("insHtr1",-0.25*kInsertWidth,+0.25*kInsertWidth,0.0);   // 1: 2x2 up-left
  insHoleTrans[1] = new TGeoTranslation("insHtr2",+0.25*kInsertWidth,+0.25*kInsertWidth,0.0);   // 2: 2x2 up-right
  insHoleTrans[2] = new TGeoTranslation("insHtr3",-0.25*kInsertWidth,-0.25*kInsertWidth,0.0);   // 3: 2x2 down-left
  insHoleTrans[3] = new TGeoTranslation("insHtr4",+0.25*kInsertWidth,-0.25*kInsertWidth,0.0);   // 4: 2x2 down-right
  insHoleTrans[4] = new TGeoTranslation("insHtr5",-0.25*kInsertWidth,-0.125*kInsertWidth,0.0);  // 5: 2x1 down-left
  insHoleTrans[5] = new TGeoTranslation("insHtr6",+0.25*kInsertWidth,-0.125*kInsertWidth,0.0);  // 6: 2x1 down-right
  insHoleTrans[6] = new TGeoTranslation("insHtr7",+0.125*kInsertWidth,+0.125*kInsertWidth,0.0); // 7: 1x1 up-right (middle)
  insHoleTrans[7] = new TGeoTranslation("insHtr8",-0.375*kInsertWidth,-0.375*kInsertWidth,0.0); // 8: 1x1 down-left (corner)
  for(Int_t i=0;i<nInsHoleTrans;++i) insHoleTrans[i]->RegisterYourself();
  TString insSubFormula[kNSubmoduleType];
  insSubFormula[0] = "insertBox   - insHoleBox4:insHtr1 - insHoleBox4:insHtr2 - insHoleBox4:insHtr3 - insHoleBox4:insHtr4";
  insSubFormula[1] = "insertXtru1 - insHoleBox4:insHtr1 - insHoleBox4:insHtr2 - insHoleBox4:insHtr3 - insHoleBox2:insHtr6";
  insSubFormula[2] = "insertXtru2 - insHoleBox4:insHtr1 - insHoleBox4:insHtr2 - insHoleBox2:insHtr5 - insHoleBox2:insHtr6";
  insSubFormula[3] = "insertXtru3 - insHoleBox4:insHtr1 - insHoleBox4:insHtr2 - insHoleBox2:insHtr5 - insHoleBox4:insHtr4";
  insSubFormula[4] = "insertXtru4 - insHoleBox4:insHtr1 - insHoleBox1:insHtr7 - insHoleBox4:insHtr3 - insHoleBox4:insHtr4";
  insSubFormula[5] = "insertXtru5 - insHoleBox1:insHtr8";


  insertShape[0] = new TGeoCompositeShape("insertShape0",insSubFormula[0]);
  TGeoVolume* insert[kNSubmoduleType];
  insert[0] = new TGeoVolume("insert0",insertShape[0],gGeoMan->GetMedium("aluminium"));
  insert[0]->SetLineColor(kYellow+1);

  for(Int_t subi=1;subi<kNSubmoduleType;++subi){
    for(Int_t i=0; i<nVtx[subi]; ++i){
      insVX[subi][i] = vX[subi][i] + (vX[subi][i]<0 ? 1.0:-1.0)*kInsAlvDiff;
      insVY[subi][i] = vY[subi][i] + (vY[subi][i]<0 ? 1.0:-1.0)*kInsAlvDiff;
    }
    TString shapeName = "insertXtru"; shapeName += subi;
    insertXtru[subi] = new TGeoXtru(2); insertXtru[subi]->SetName(shapeName);
    insertXtru[subi]->DefinePolygon(nVtx[subi],insVX[subi],insVY[subi]);
    insertXtru[subi]->DefineSection(0,-0.5*kInsertLength,0.0,0.0,1.0);
    insertXtru[subi]->DefineSection(1,+0.5*kInsertLength,0.0,0.0,1.0);

    shapeName = "insertShape"; shapeName += subi;
    insertShape[subi] = new TGeoCompositeShape(shapeName,insSubFormula[subi]);
    TString volName = "insert"; volName += subi;
    insert[subi] = new TGeoVolume(volName,insertShape[subi],gGeoMan->GetMedium("aluminium"));
    insert[subi]->SetLineColor(kYellow+1);
  }
  TGeoTranslation* insertZtrans = new TGeoTranslation(0.0,0.0,-0.5*(kAlvLength+kInsertLength));



  //--- volume assemblies describing the BWEC submodules
  TGeoVolumeAssembly* BWECsubmodule[kNSubmoduleType];
  for(Int_t i=0; i<kNSubmoduleType; ++i){
    TString volName = "BWECsubmodule"; volName += i;
    BWECsubmodule[i] = new TGeoVolumeAssembly(volName);

    //--- adding the carbon fibre alveole
    BWECsubmodule[i]->AddNode(BWECalveole[i],i);

    //--- adding the crystals and Al capsules
    for(Int_t icr=0; icr<16;++icr){
      TGeoTranslation *crystTrans = new TGeoTranslation(firstCrystPosX + (icr%4)*crystDPos, firstCrystPosY - (icr/4)*crystDPos, 0.5*kCapsuleLength);
      TGeoTranslation *capslTrans = new TGeoTranslation(firstCrystPosX + (icr%4)*crystDPos, firstCrystPosY - (icr/4)*crystDPos,-0.5*kCrLength);
      if( AlvLayout[i][icr] ){
	BWECsubmodule[i]->AddNode(PWOcrystal  ,icr, crystTrans);
	BWECsubmodule[i]->AddNode(crystCapsule,icr, capslTrans);
      }
    }
    
    //--- adding Al insert
    BWECsubmodule[i]->AddNode(insert[i], 0, insertZtrans);

  }



  //----------------------------- Quarters of the BWEC: side (6 submodules) + corner (4 submodules)
  TGeoVolumeAssembly *BWECquarter = new TGeoVolumeAssembly("BWECquarter");

  // side
  BWECquarter->AddNode(BWECsubmodule[0],0,new TGeoTranslation(kSubmodPos0[0],kSubmodPos0[1],kSubmodPos0[2]));
  BWECquarter->AddNode(BWECsubmodule[0],1,new TGeoTranslation(kSubmodPos1[0],kSubmodPos1[1],kSubmodPos1[2]));
  BWECquarter->AddNode(BWECsubmodule[0],2,new TGeoTranslation(kSubmodPos2[0],kSubmodPos2[1],kSubmodPos2[2]));
  BWECquarter->AddNode(BWECsubmodule[1],3,new TGeoTranslation(kSubmodPos3[0],kSubmodPos3[1],kSubmodPos3[2]));
  BWECquarter->AddNode(BWECsubmodule[2],4,new TGeoTranslation(kSubmodPos4[0],kSubmodPos4[1],kSubmodPos4[2]));
  BWECquarter->AddNode(BWECsubmodule[3],5,new TGeoTranslation(kSubmodPos5[0],kSubmodPos5[1],kSubmodPos5[2]));

  // corner
  BWECquarter->AddNode(BWECsubmodule[0],6,new TGeoTranslation(kSubmodPos6[0],kSubmodPos6[1],kSubmodPos6[2]));
  BWECquarter->AddNode(BWECsubmodule[4],7,new TGeoTranslation(kSubmodPos7[0],kSubmodPos7[1],kSubmodPos7[2]));
  BWECquarter->AddNode(BWECsubmodule[4],8,new TGeoTranslation(kSubmodPos8[0],kSubmodPos8[1],kSubmodPos8[2]));
  BWECquarter->AddNode(BWECsubmodule[5],9,new TGeoTranslation(kSubmodPos9[0],kSubmodPos9[1],kSubmodPos9[2]));

  TGeoRotation *quarterRot[4];
  for(Int_t qi=0; qi<4; ++qi){
    quarterRot[qi] = new TGeoRotation;
    TString rotName("quarterRot"); rotName += qi;
    quarterRot[qi]->SetName(rotName);
    quarterRot[qi]->RotateZ(90.*qi);
    quarterRot[qi]->RegisterYourself();
    BWECinnerVol->AddNode(BWECquarter,qi,quarterRot[qi]);
  }



  //----------------------------- Dead material volumes: VIPs and cooling shells
  //--- front VIP
  TGeoTube *frontVIPTube = new TGeoTube("frontVIPTube",kInnerVIPRout,kSideVIPRin,0.5*kFrontVIPThickness);
				     
  TGeoBBox *frontVIPrBox = new TGeoBBox("frontVIPBox",kSideVIPxyInnerWidth,kSideVIPxyInnerWidth,
					0.5*kFrontVIPThickness+0.1);
  TGeoCompositeShape* frontVIPshape = new TGeoCompositeShape("frontVIPshape","frontVIPTube*frontVIPBox");
  TGeoVolume* frontVIP = new TGeoVolume("frontVIP",frontVIPshape,gGeoMan->GetMedium("FumedSilica"));
  frontVIP->SetLineColor(kGray+1);
  frontVIP->SetLineStyle(kSolid);
  BWECinnerVol->AddNode(frontVIP,0,new TGeoTranslation(0.0,0.0,kFrontVIPZpos));


  //--- front Cooling
  TGeoTube *frontCoolingTube = new TGeoTube("frontCoolingTube",kInnerVIPRout,kSideVIPRin,0.5*kFrontCoolingThickness);
				     
  TGeoBBox *frontCoolingBox = new TGeoBBox("frontCoolingBox",kSideVIPxyInnerWidth,kSideVIPxyInnerWidth,
					    0.5*kFrontCoolingThickness+0.1);
  TGeoCompositeShape* frontCoolingshape = new TGeoCompositeShape("frontCoolingshape","frontCoolingTube*frontCoolingBox");
  TGeoVolume* frontCooling = new TGeoVolume("frontCooling",frontCoolingshape,gGeoMan->GetMedium("CoolingShellMix"));
  frontCooling->SetLineColor(kWhite);
  frontCooling->SetLineStyle(kSolid);
  BWECinnerVol->AddNode(frontCooling,0,new TGeoTranslation(0.0,0.0,kFrontCoolingZpos));


  //--- Side VIP
  TGeoTube* sideVIPTubeOut = new TGeoTube("sideVIPTubeOut",1.0,kSideVIPRout,0.5*kSideVIPLength);
  TGeoTube* sideVIPTubeIn  = new TGeoTube("sideVIPTubeIn", 0.0,kSideVIPRin, 0.5*kSideVIPLength+0.1);
  TGeoBBox *sideVIPBoxOut  = new TGeoBBox("sideVIPBoxOut",kSideVIPxyOuterWidth,kSideVIPxyOuterWidth,0.5*kSideVIPLength+0.11);
  TGeoBBox *sideVIPBoxIn   = new TGeoBBox("sideVIPBoxIn", kSideVIPxyInnerWidth,kSideVIPxyInnerWidth,0.5*kSideVIPLength+0.11);
  TGeoCompositeShape* sideVIPshape = 
    new TGeoCompositeShape("sideVIPshape","(sideVIPTubeOut*sideVIPBoxOut)-(sideVIPTubeIn*sideVIPBoxIn)" ); 
  TGeoVolume* sideVIP = new TGeoVolume("sideVIP",sideVIPshape,gGeoMan->GetMedium("FumedSilica"));
  sideVIP->SetLineColor(kGray+1);
  sideVIP->SetLineStyle(kSolid);
  BWECinnerVol->AddNode(sideVIP,0,new TGeoTranslation(0.0,0.0,kSideVIPZpos));

  //--- Inner VIP
  TGeoTube* innerVIPTube = new TGeoTube("sideVIPTubeOut",kInnerVIPRin,kInnerVIPRout,0.5*kSideVIPLength);
  TGeoVolume* innerVIP = new TGeoVolume("innerVIP",innerVIPTube,gGeoMan->GetMedium("FumedSilica"));
  innerVIP->SetLineColor(kGray+1);
  innerVIP->SetLineStyle(kSolid);
  BWECinnerVol->AddNode(innerVIP,0,new TGeoTranslation(0.0,0.0,kSideVIPZpos));



  //--- Side and inner Cooling
  TGeoTube *sideCoolingTube = new TGeoTube("sideCoolingTube",kInnerVIPRout,kSideVIPRin,0.5*(kAlvLength+kInsertLength));
  TGeoBBox *sideCoolingBox = new TGeoBBox("sideCoolingBox",kSideVIPxyInnerWidth,kSideVIPxyInnerWidth,0.5*(kAlvLength+kInsertLength)+0.1);

  //--- Complex shape for the hole. Building it up from the data for the alveole Xtru objects.
  Double_t shVX[kNSubmoduleType][16],shVY[kNSubmoduleType][16];
  TGeoXtru *shellHXtru[kNSubmoduleType];

  for(Int_t subi=0;subi<kNSubmoduleType;++subi){
    for(Int_t i=0; i<nVtx[subi]; ++i){
      shVX[subi][i] = vX[subi][i] + ((vX[subi][i]<0) ? -1.0:1.0)*(kDistBtwAlvs+(subi==0?0.5*mm:0.1*mm));
      shVY[subi][i] = vY[subi][i] + ((vY[subi][i]<0) ? -1.0:1.0)*(kDistBtwAlvs+(subi==0?0.5*mm:0.1*mm));
    }
    if(subi == 5) { // special case for the "loose" crystal
      shVX[subi][1] = vX[subi][1] + kDistBtwAlvs + 0.1*mm;
      shVX[subi][2] = vX[subi][2] + kDistBtwAlvs + 0.1*mm;  shVY[subi][2] = vY[subi][2] + kDistBtwAlvs + 0.1*mm;
      shVY[subi][3] = vY[subi][3] + kDistBtwAlvs + 0.1*mm;
    }
    TString shapeName = "shXtru"; shapeName += subi;
    shellHXtru[subi] = new TGeoXtru(2); shellHXtru[subi]->SetName(shapeName);
    shellHXtru[subi]->DefinePolygon(nVtx[subi],shVX[subi],shVY[subi]);
    shellHXtru[subi]->DefineSection(0,-0.5*(kAlvLength+kInsertLength+2.),0.0,0.0,1.0);
    shellHXtru[subi]->DefineSection(1,+0.5*(kAlvLength+kInsertLength+2.),0.0,0.0,1.0);
  }

  TGeoTranslation *shTransl[kNsubmodQuarter];
  const Double_t *shTrV[kNsubmodQuarter] = {kSubmodPos0,kSubmodPos1,kSubmodPos2,kSubmodPos3,kSubmodPos4,
					    kSubmodPos5,kSubmodPos6,kSubmodPos7,kSubmodPos8,kSubmodPos9};
  TString shXtruName[kNsubmodQuarter]={"shXtru0","shXtru0","shXtru0","shXtru1","shXtru2",
				       "shXtru3","shXtru0","shXtru4","shXtru4","shXtru5"};

  TString unionFormula="";
  for(Int_t i=0; i<kNsubmodQuarter; ++i){
    TString trName = "shTr"; trName += i;
    shTransl[i] = new TGeoTranslation(trName,shTrV[i][0],shTrV[i][1],shTrV[i][2]-kInsertLength);
    shTransl[i]->RegisterYourself();
    unionFormula += (unionFormula=="")?"":" + ";
    unionFormula += shXtruName[i]+":"+trName;
  }
  cout<<unionFormula<<endl;
  TGeoCompositeShape* sideCoolingHoleShape = new TGeoCompositeShape("sideCoolHole",unionFormula);

  TString sideCoolingFormula = "sideCoolingTube*sideCoolingBox";
  for(Int_t qi=0; qi<4; ++qi) sideCoolingFormula += TString(" - sideCoolHole:")+quarterRot[qi]->GetName();
  cout<<sideCoolingFormula<<endl;

  TGeoCompositeShape* sideCoolingShape = new TGeoCompositeShape("sideCoolingShape",sideCoolingFormula);
  TGeoVolume* sideCooling = new TGeoVolume("sideCooling",sideCoolingShape,gGeoMan->GetMedium("aluminium"));
  sideCooling->SetLineColor(kWhite);
  sideCooling->SetLineStyle(kSolid);
  BWECinnerVol->AddNodeOverlap(sideCooling,0,new TGeoTranslation(0.0,0.0,kModuleZpos-0.5*kInsertLength));




// checks:
  cout<<"check: "<<kInnerVolLength - kFrontVIPThickness - kFrontCoolingThickness - kCrystVIPDist - kCrLength - 173.*mm<<endl;
  cout<<"check: "<<0.5*kInnerVolLength - (kModuleZpos + 0.5*kAlvLength + kFrontVIPThickness + kFrontCoolingThickness)<<endl;;


  gGeoMan->CloseGeometry();


  TString outfile = "/home/luigi/him/sw/pandaroot6/fairsoft_may16p1/pandaroot/macro/emc/dedicated/emc_module4_2017-tmp.root";
  gGeoManager->Export(outfile);



  //----------------------------- visibility
  PWOcrystal       -> SetVisibility(kTRUE);
  crystCapsule     -> SetVisibility(kTRUE);
  BWECalveole[0]   -> SetVisibility(kTRUE);
  BWECalveole[1]   -> SetVisibility(kTRUE);
  BWECalveole[2]   -> SetVisibility(kTRUE);
  BWECalveole[3]   -> SetVisibility(kTRUE);
  BWECalveole[4]   -> SetVisibility(kTRUE);
  BWECalveole[5]   -> SetVisibility(kTRUE);
  insert[0]        -> SetVisibility(kTRUE); 
  insert[1]        -> SetVisibility(kTRUE); 
  insert[2]        -> SetVisibility(kTRUE); 
  insert[3]        -> SetVisibility(kTRUE); 
  insert[4]        -> SetVisibility(kTRUE); 
  insert[5]        -> SetVisibility(kTRUE); 
  frontVIP         -> SetVisibility(kTRUE);
  sideVIP          -> SetVisibility(kTRUE);
  innerVIP         -> SetVisibility(kTRUE);
  frontCooling     -> SetVisibility(kTRUE);
  sideCooling      -> SetVisibility(kTRUE);
  BWECinnerVol     -> SetVisibility(kTRUE);
  BWECouterVol     -> SetVisibility(kTRUE);


  top->Raytrace();
  gStyle->SetCanvasPreferGL(kTRUE);
  top->Draw("ogl");
  gGeoManager->SetTopVisible();
  gGeoManager->SetVisOption(0);
  gGeoManager->SetVisLevel(11);

//  new TBrowser;
  return 0;

}
