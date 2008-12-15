/////////////////////////////////////////////////////////////
//
//  PndEmc
//
//  Filler of PndEmcPoint
//
//  Created 14/08/06  by S.Spataro
//
///////////////////////////////////////////////////////////////

#include "PndEmc.h"

#include "PndEmcPoint.h"
#include "PndEmcReader.h"
#include "PndEmcStructure.h"

#include "CbmGeoInterface.h"
#include "CbmGeoLoader.h"
#include "CbmGeoRootBuilder.h"
#include "CbmRootManager.h"
#include "CbmVolume.h"
#include "CbmGeoMedia.h"
#include "CbmGeoG3Builder.h"
#include "CbmRuntimeDb.h"
#include "CbmRun.h"

#include "TObjArray.h"
#include "TClonesArray.h"
#include "TGeoMCGeometry.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoArb8.h"
#include "TGeoVoxelFinder.h"
#include "TGeoMatrix.h"

#include "CbmGeoMedium.h"
#include "CbmStack.h"
#include "TString.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndEmc::PndEmc() {
  fEmcCollection        = new TClonesArray("PndEmcPoint");
  fPosIndex   = 0;
  fEventID=-1;
  bIsFastFsc = kFALSE;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndEmc::PndEmc(const char* name, Bool_t active, Bool_t fast)
  : CbmDetector(name, active) {
    fEmcCollection        = new TClonesArray("PndEmcPoint");
    fPosIndex   = 0;
    fEventID=-1; 
    bIsFastFsc = fast;
    fwendcap = kFALSE;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndEmc::~PndEmc() {
  if (fEmcCollection) {
    fEmcCollection->Delete();
    delete fEmcCollection;
  }
  
}
// -------------------------------------------------------------------------



// -----   Public method Intialize   ---------------------------------------
void PndEmc::Initialize() {
  // Init function
  
  CbmDetector::Initialize();
  CbmRun* sim = CbmRun::Instance();
  CbmRuntimeDb* rtdb=sim->GetRuntimeDb();
  
}
// -------------------------------------------------------------------------
void PndEmc::BeginEvent(){
  // Begin of the event
  
}



// -----   Public method ProcessHits  --------------------------------------
Bool_t PndEmc::ProcessHits(CbmVolume* vol) {  
  
  // Increment number of emc points for TParticle
  if (gMC->IsTrackEntering()) 
    {
    Int_t points = gMC->GetStack()->GetCurrentTrack()->GetMother(1);
    Int_t nEmcPoints = (points & (15<<16)) >> 16;
    nEmcPoints ++;
    if (nEmcPoints <= 15)
      {
        points = ( points & ( ~ (15<<16) ) ) | (nEmcPoints << 16);
        gMC->GetStack()->GetCurrentTrack()->SetMother(1,points);
      }
    }

  if (gMC->Edep()<=0) return kTRUE; // skip all the points which have no energy loss (i.e. Entering)
  
  TString nam = gMC->CurrentVolName();

  // ---------------------------------------------------------------------------------
  // Getting parameters for the ROOT file with geometry for Forward Enc-Cap.
  // Each of the subvolume name for FwEndCap geometry in the ROOT file contains "Vol". 
  Int_t copyNoCrys=-1,copyNoBox=-1,copyNoSub=-1,copyNoQuar=-1;
  Int_t idCrys=-1,idBox=-1,idSub=-1,idQuar=-1;
  Int_t copyNo = -1, id = -1;
  Int_t nMod = -1, nRow = -1, nCrys = -1; 
   
  if (nam.Contains("Vol")){
    
    TString namCrys    = gMC->CurrentVolOffName(0); // Crystal name 
    TString namBox     = gMC->CurrentVolOffName(1); // Box name  
    TString namSub     = gMC->CurrentVolOffName(2); // Subunit name 
    TString namQuar    = gMC->CurrentVolOffName(3); // Quarter name 
   
    // Return the current volume off upward in the geometrical tree
    // ID and copy number
    idCrys = gMC->CurrentVolOffID(0,copyNoCrys);
    idBox  = gMC->CurrentVolOffID(1,copyNoBox);
    idSub  = gMC->CurrentVolOffID(2,copyNoSub)-1; // 
    idQuar = gMC->CurrentVolOffID(3,copyNoQuar);
   
     
    Int_t daughtQuar = gMC->NofVolDaughters(namQuar);
    Int_t daughtSub  = gMC->NofVolDaughters(namSub);
    Int_t daughtBox  = gMC->NofVolDaughters(namBox);
    Int_t daughtCrys = gMC->NofVolDaughters(namCrys);
     
    Int_t col=0, k1=0;
    Int_t subrow=4;   // 4 crystals in each subvolume
    Int_t next=0;     // starts (from the middle) next column, represents rows
    copyNoSub-=1;     // When geometry is created, copyNoSub starts from 1-55 
                      //and the loop below starts from 0-54
       
    if((copyNoSub >=  0) && (copyNoSub <=  6)){
      next  = copyNoSub + 2;
      col   = 0;
    }else if((copyNoSub >=  7) && (copyNoSub <= 13)){
      next  = (copyNoSub-7) +2;
      col   = 1;
    }else if((copyNoSub >= 14) && (copyNoSub <= 19)){
      next  = (copyNoSub-14) +2;
      col   = 2;
    }else if((copyNoSub >= 20) && (copyNoSub <= 26)){
      next  = (copyNoSub-20) +1;
      col   = 3;
    }else if((copyNoSub >= 27) && (copyNoSub <= 34)){
      next  = (copyNoSub-27);
      col   = 4;
    }else if((copyNoSub >= 35) && (copyNoSub <= 41)){
      next  = (copyNoSub-35);
      col   = 5;
    }else if((copyNoSub >= 42) && (copyNoSub <= 47)){
      next  = (copyNoSub-42);
      col   = 6;
    }else if((copyNoSub >= 48) && (copyNoSub <= 52)){
      next  = (copyNoSub-48);
      col   = 7;
    }else if((copyNoSub >= 53) && (copyNoSub <= 54)){
      next  = (copyNoSub-53);
      col   = 8;
    }
     
     Int_t flag=1;
	       
     if (next<2  && col <3) flag=0; // 6 copyNoSub in the beam-pipe area
     if (next==0 && col==3) flag=0; // 7th copyNoSub in the beam-pipe area
     if (col>7  && next >1) flag=0; // empty copyNoSub in the residual area
     if (col>6  && next >4) flag=0; //  -||- 
     if (col>5  && next >5) flag=0; //  -||- 
     if (col>4  && next >6) flag=0; //  -||- 
     if (col>1  && next >7) flag=0; //  -||- 

     if(flag)
       if ( (copyNoBox == 0)  || (copyNoBox == 2) ){
	  if(copyNoCrys == 0 || copyNoCrys == 1){
	     nCrys = ( (next*4) + (copyNoCrys+1) );
	     nRow = (copyNoBox + 1 + subrow*col); 	
	  }else if (copyNoCrys == 2 || copyNoCrys == 3){
	     k1 = copyNoCrys-2;			      
	     nCrys = ( (next*4) + (k1+1) );
	     nRow = (copyNoBox + 2 + subrow*col);		 
	  }
       }else if ( (copyNoBox == 1) || (copyNoBox == 3) ){
	  if(copyNoCrys == 0 || copyNoCrys == 1){
	     nCrys = ( (next*4) + (copyNoCrys+3) );
	     nRow = copyNoBox + subrow*col; 
	  }else if (copyNoCrys == 2 || copyNoCrys == 3){
	     k1 = copyNoCrys-2;
	     nCrys = ( (next*4) + (k1+3) );
	     nRow = (copyNoBox + 1 + subrow*col);
	  } //end of crystal		     
       } //end of box
     
     nMod=3;
     copyNo = copyNoQuar;
     
     //Text_t buffer[40];
     //sprintf(buffer,"emc0%dr%dc%dcp%d",nMod, nRow, nCrys, copyNoQuar);
     //copyNo = copyNoQuar;
  }
  // ---------------------------------------------------------------------------------
  
  fTrackID  = gMC->GetStack()->GetCurrentTrackNumber(); // trk ID
  fEventID = gMC->CurrentEvent();
  fELoss = gMC->Edep();
  fLength =  gMC->TrackLength();
  fTime =  gMC->TrackTime();
  gMC->TrackPosition(fPos); // cm
  gMC->TrackMomentum(fMom); // GeV
  
  if (nam.BeginsWith("emc")) 
    {
      sscanf(nam,"emc%dr%dc%d", &nMod, &nRow, &nCrys);
      
      // Crys 1-5000; copyNo 1-20; nRow 1-100, nMod 1-6
      if ((nMod==1) || (nMod==2))
	id = gMC->CurrentVolOffID(2,copyNo);
      if ((nMod==3) || (nMod==4)|| (nMod==6))
	id = gMC->CurrentVolOffID(1,copyNo); 
      // 1 -because the pad stays inside flayer4 (Emc4), so only "1" as inheritance. 
      // In barrel part one pad stays into Emc1 which stays inside Emc12 (and after Emc12 is
      // copied and rotated -> the inheritance level is "2"
    }
  else if (bIsFastFsc)
    { 
      nMod = 5;
      nRow   = abs((Int_t)(fPos.X()/11.))+1;
      nCrys  = abs((Int_t)(fPos.Y()/11.))+1;
      if ((fPos.X()<0.) && (fPos.Y()>0.))  copyNo = 1;  
      if ((fPos.X()<0.) && (fPos.Y()<0.))  copyNo = 2;
      if ((fPos.X()>0.) && (fPos.Y()<0.))  copyNo = 3;
      if ((fPos.X()>0.) && (fPos.Y()>0.))  copyNo = 4; 
    }
  else if (!bIsFastFsc)
    { 
      nam = gMC->CurrentVolOffName(2);
      sscanf(nam,"emc%dr%dc%d", &nMod, &nRow, &nCrys);
      if (nMod==5)
	id = gMC->CurrentVolOffID(3,copyNo);
    }
  
  fVolumeID = nMod*100000000 + nRow*1000000 + copyNo*10000 + nCrys; 

  TVector3 pos(fPos.X(),   fPos.Y(),   fPos.Z());
  
  AddHit(fTrackID, fVolumeID, fEventID,
	 TVector3(fPos.X(),   fPos.Y(),   fPos.Z()),
	 TVector3(fMom.Px(),  fMom.Py(),  fMom.Pz()),
	 fTime, fLength, fELoss, nMod, nRow, nCrys, copyNo);

   /*cout << "PndEmc: fTrackID= " << fTrackID 
     << " fVolumeID= " << fVolumeID 
    // << " fELoss= " << fELoss 
    // << " fTime= " << fTime 
     << " nMod= "<<nMod
     << " nRow= "<<nRow
     << " nCrys="<<nCrys
     << " copyNo="<<copyNo     
     << endl;
*/
  ResetParameters();
  
  return kTRUE;
}
// ----------------------------------------------------------------------------

// -----   Public method EndOfEvent   -----------------------------------------
void PndEmc::EndOfEvent() {
  if (fVerboseLevel)  Print();
  Reset();
}
// ----------------------------------------------------------------------------

// -----   Public method Register   -------------------------------------------
void PndEmc::Register() {
  CbmRootManager::Instance()->Register("EmcPoint","Emc", fEmcCollection, kTRUE);
}
// ----------------------------------------------------------------------------

// -----   Public method GetCollection   --------------------------------------
TClonesArray* PndEmc::GetCollection(Int_t iColl) const {
   if (iColl == 0) return fEmcCollection;

  return NULL;
}
// ----------------------------------------------------------------------------

// -----   Public method Print   ----------------------------------------------
void PndEmc::Print() const {
    Int_t nHits = fEmcCollection->GetEntriesFast();
    //cout << "-I- PndEmc: " << nHits << " points registered in this event."
    //<< endl;

    if (fVerboseLevel>1)
      for (Int_t i=0; i<nHits; i++) (*fEmcCollection)[i]->Print();
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void PndEmc::Reset() {
   fEmcCollection->Clear();
 
  fPosIndex = 0;
}
// ----------------------------------------------------------------------------


// guarda in CbmRootManager::CopyClones
// -----   Public method CopyClones   -----------------------------------------
void PndEmc::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset ) {
  Int_t nEntries = cl1->GetEntriesFast();
  //cout << "-I- PndEmc: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  PndEmcPoint* oldpoint = NULL;
  for (Int_t i=0; i<nEntries; i++) {
    oldpoint = (PndEmcPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) PndEmcPoint(*oldpoint);
    fPosIndex++;
  }
  //  cout << " -I- PndEmc: " << cl2->GetEntriesFast() << " merged entries."
  //   << endl;
}

void PndEmc::SetGeometryFileNameDouble(TString fname, TString fname2, TString geoVer)
{
  fwendcap=kTRUE;

  SetGeometryFileName(fname, geoVer);
  
  //fgeoVer=geoVer; 
  TString work = getenv("VMCWORKDIR");
	
  fgeoName2=work+"/geometry/";
  fgeoName2+=fname2;
}


// ----------------------------------------------------------------------------
 // -----   Public method ConstructGeometry   ----------------------------------
void PndEmc::ConstructGeometry() {
  TString fileName=GetGeometryFileName();

  if (!fwendcap){
    
    if (fileName.EndsWith(".dat")) {
      std::cout<< "                                               " <<std::endl;
      std::cout<< " ====== EMC::  ConstructASCIIGeometry() ====== " <<std::endl;
      std::cout<< " ============================================= " <<std::endl;
      ConstructASCIIGeometry();
    } else if(fileName.EndsWith(".root")) {
      std::cout<< "                                              " <<std::endl;
      std::cout<< " ====== EMC::  ConstructROOTGeometry() ====== " <<std::endl;
      std::cout<< " ============================================ " <<std::endl;
      ConstructRootGeometry();
    } else {
      std::cout<< "Geometry format not supported " <<std::endl;
    }
  }else {      
    if (fileName.EndsWith(".dat")) {
      std::cout<< "                                               " <<std::endl;
      std::cout<< " ====== EMC::  ConstructASCIIGeometry() ====== " <<std::endl;
      std::cout<< " ============================================= " <<std::endl;
      ConstructASCIIGeometry();
    }else {
      std::cout<< "You do not provide an ASCII file " <<std::endl;
    } 
    if (fgeoName2.EndsWith(".root")) {
      std::cout<< "                                               " <<std::endl;
      std::cout<< " ====== EMC::  ConstructRootGeometry() ====== " <<std::endl;
      std::cout<< " ============================================= " <<std::endl;
      ConstructRootGeometry();
    }else {
      std::cout<< "You do not provide a ROOT file " <<std::endl;
    }
  }
}

void PndEmc::ConstructRootGeometry() {
  
  TFile *f;
  if (!fwendcap){
    std::cout<< "File name = " << GetGeometryFileName().Data() << std::endl;
    f=new TFile(GetGeometryFileName().Data());
  }else{
    std::cout<< "File name = " << fgeoName2 << std::endl;
    f=new TFile(fgeoName2);
  }

  
  TGeoVolume *FwEmc=(TGeoVolume *)f->Get("Emc3");
  TGeoVolume *Cave = gGeoManager->GetTopVolume();
  TGeoNode *n=FwEmc->GetNode(0); 
  
  gGeoManager->AddVolume(FwEmc);
  TGeoVoxelFinder *voxels = FwEmc->GetVoxels();
  if (voxels) voxels->SetNeedRebuild();
  TGeoMatrix *M = n->GetMatrix();
  M->SetDefaultName();
  gGeoManager->GetListOfMatrices()->Remove(M);
  TGeoHMatrix *global = gGeoManager->GetHMatrix();             
  gGeoManager->GetListOfMatrices()->Remove(global); //Remove the Identity matrix 
  

  TGeoRotation rotFwEmc;
  rotFwEmc.RotateY(180.);
  Cave->AddNode(FwEmc,0, new TGeoCombiTrans(0., 0., 221.,new TGeoRotation(rotFwEmc)));

  ExpandNode(FwEmc,Cave); 
}

void PndEmc::ExpandNode(TGeoVolume *fVol, TGeoVolume *Cave){
  
  CbmGeoLoader*geoLoad = CbmGeoLoader::Instance();
  CbmGeoInterface *geoFace = geoLoad->getGeoInterface();
  CbmGeoMedia *Media = geoFace->getMedia();
  CbmGeoBuilder *geobuild=geoLoad->getGeoBuilder();
   
  TObjArray *nodeList=fVol->GetNodes();  
  Int_t nodes = nodeList->GetEntries();

  for (Int_t nod=0; nod < nodes; nod++){
    
    TGeoNode *fNode =(TGeoNode *)nodeList->At(nod);  
    TGeoVolume *v= fNode->GetVolume();
    
    if(fNode->GetNdaughters()>0) 	
      ExpandNode(v,Cave);

    TGeoMedium* med1=v->GetMedium();
    //if(fVerboseLevel>2) std::cout<< "DEBUG NodeName = " << fNode->GetName() << std::endl;
    if (med1) {
      //if(fVerboseLevel>2) std::cout<< "DEBUG medium  = " << med1->GetName() << std::endl;
      TGeoMaterial*mat1=v->GetMaterial(); 
      TGeoMaterial *newMat = gGeoManager->GetMaterial(mat1->GetName());
      if (newMat==0) {
	//std::cout<< "Material " << mat1->GetName() << " is not defined " << std::endl;
	CbmGeoMedium *CbmMedium=Media->getMedium(mat1->GetName());
	if (!CbmMedium) {
	  //std::cout << "Material is not defined in ASCII file nor in Root file" << std::endl;
	  CbmMedium=new CbmGeoMedium(mat1->GetName());
	  Media->addMedium(CbmMedium);
	}
	//std::cout << "Create Medium " << mat1->GetName() << std::endl;
	Int_t nmed=geobuild->createMedium(CbmMedium);
	v->SetMedium(gGeoManager->GetMedium(nmed));
	gGeoManager->SetAllIndex();
      } else {
	//if(fVerboseLevel>2)  
	  //std::cout<< "DEBUG material was defined  MaterialName= " << mat1->GetName() << std::endl;
	TGeoMedium *med2= gGeoManager->GetMedium(mat1->GetName());
	v->SetMedium(med2);
      }
    }

    TString name = v->GetName();
    
    if (!gGeoManager->FindVolumeFast(v->GetName())) {
      v->RegisterYourself();      
      if(fVerboseLevel>2) std::cout<< "Volume ->  " <<v->GetName()<< " --> Registered to gGeoManager" << endl;
      if (name.Contains("Crystal")){
	AddSensitiveVolume(v); 
	if(fVerboseLevel>2) std::cout<< "Volume " <<v->GetName()<< " is added to sensitives "<<endl;
      }  
    }
  }
}

void PndEmc::ConstructASCIIGeometry() {
  // Definition of materials
  
  CbmGeoLoader*geoLoad = CbmGeoLoader::Instance();
  CbmGeoInterface *geoFace = geoLoad->getGeoInterface();
  CbmGeoMedia *Media =  geoFace->getMedia();
  CbmGeoBuilder *geobuild=geoLoad->getGeoBuilder();
 
  CbmGeoMedium *CbmMediumPb  = Media->getMedium("lead");
  CbmGeoMedium *CbmMediumPWO = Media->getMedium("PWO");
  CbmGeoMedium *CbmMediumFsc = Media->getMedium("FscScint");
  
  Int_t nmedPb=geobuild->createMedium(CbmMediumPb);
  Int_t nmedPWO=geobuild->createMedium(CbmMediumPWO);
  Int_t nmedFsc=geobuild->createMedium(CbmMediumFsc);
  
  TGeoVolume *flayer1 = new TGeoVolumeAssembly("EmcLayer1");
  TGeoVolume *flayer2 = new TGeoVolumeAssembly("EmcLayer2");
  TGeoVolume *flayer3 = new TGeoVolumeAssembly("Emc3");
  TGeoVolume *flayer4 = new TGeoVolumeAssembly("Emc4");
  TGeoVolume *flayer5 = new TGeoVolumeAssembly("Fsc");
  TGeoVolume *flayer6 = new TGeoVolumeAssembly("EmcTest");
  
  Bool_t bIsModuleOn[6] = {kFALSE, kFALSE, kFALSE, kFALSE, kFALSE,kFALSE};
  Bool_t isFirst = kTRUE;
  
  PndEmcReader read(GetGeometryFileName() );
  for(Int_t module=read.GetMinModules(); module<=read.GetMaxModules(); module++) {
    cout << "Emc module = " << module; 
    if (module==5 && bIsFastFsc) {
      cout << "\t FAST"  << endl << "******** " << endl;
      continue;
    }
    cout << endl << "******** " << endl; 
    
    for(Int_t row=read.GetMinRows(module); row<=read.GetMaxRows(module); row++){
      for(Int_t crystal=read.GetMinCrystals(module,row); crystal<=read.GetMaxCrystals(module,row); crystal++) { 
	
	Text_t buffer[30];
	sprintf(buffer,"emc0%dr%dc%d",module, row, crystal);
	DataG4 data = read.GetData(module,row,crystal);
	
	if (data.module==-1) continue; //if the pad is not present, do not create geometry
	
	if ((module<5) || (module==6))
	  {	// Construction of target spectrometer geometry
	    TGeoTrap *trap = new TGeoTrap(data.pDz/10., data.pTheta, data.pPhi,
					  data.pDy1/10., data.pDx1/10., data.pDx2/10., data.pAlp1,
					  data.pDy2/10., data.pDx3/10., data.pDx4/10., data.pAlp2);
	    TGeoVolume *volume;
	    volume = new TGeoVolume(buffer, trap, gGeoManager->GetMedium("PWO"));
	    
	    volume->SetLineColor(5);
	    TGeoRotation rot;
	    rot.RotateZ(data.tau);
	    rot.RotateY(data.theta);
	    rot.RotateZ(data.phi);  
	    
	    if(module ==1) flayer1->AddNode(volume,0, new TGeoCombiTrans(data.posX/10., data.posY/10., data.posZ/10.+3.7, new TGeoRotation (rot))); // shift of 37 mm with respect to interaction point
	    if(module ==2) flayer2->AddNode(volume,0, new TGeoCombiTrans(data.posX/10., data.posY/10., data.posZ/10.+3.7, new TGeoRotation (rot))); // shift of 37 mm with respect to interaction point
	    if(module ==3) flayer3->AddNode(volume,0, new TGeoCombiTrans(data.posX/10., data.posY/10., data.posZ/10., new TGeoRotation (rot)));
	    if(module ==4) flayer4->AddNode(volume,0, new TGeoCombiTrans(data.posX/10., data.posY/10., data.posZ/10., new TGeoRotation (rot))); 
	    if(module ==6) flayer6->AddNode(volume,0, new TGeoCombiTrans(data.posX/10., data.posY/10., data.posZ/10., new TGeoRotation (rot)));
	    bIsModuleOn[module-1] = kTRUE;
	    AddSensitiveVolume(volume);
	  }
	
	if (module==5 && !bIsFastFsc) 
	  {  // Construction of forward spectrometer geometry
	    // For the forward calorimeter the box geometry was written giving different meaning to the DataG4 members:
	    // Dx1 Dy1 : half-widths in XY of the absorber
	    // Dx2 Dy2 : half-widths of XY of the scintillator
	    // pAlp1 pAlp2 : half-withs on Z of absorber/scintillator
	    // pDz : full-width of space in between absorber and scintillator
	    // pDx3 : number of layers of Pb/Scint
	    TGeoVolume *volAbs, *volSci, *volCrystal;
	    TGeoRotation rot;
	    //	    TGeoVolume *volume = new TGeoVolumeAssembly(buffer);
	    Double_t BoxZ=0.5*data.pDx3*(2.*data.pDz+2.*data.pAlp1+2.*data.pAlp2)/10.; 
	    TGeoBBox *volume =  new TGeoBBox(data.pDx1/10., data.pDy1/10., BoxZ );
	    volCrystal = new TGeoVolume(buffer, volume, gGeoManager->GetMedium("air"));
	    volCrystal->SetLineColor(3);
          //  cout << "\t Check   " << buffer << "******** " << endl;

	    TGeoBBox *absorber     = new TGeoBBox(data.pDx1/10., data.pDy1/10., data.pAlp1/10.);
	    TGeoBBox *scintillator = new TGeoBBox(data.pDx2/10., data.pDy2/10., data.pAlp2/10.); 
	    volAbs = new TGeoVolume("FscAbsorber", absorber, gGeoManager->GetMedium("lead"));
	    volSci = new TGeoVolume("FscScintillator", scintillator, gGeoManager->GetMedium("FscScint"));
	    volSci->SetLineColor(6);
	    
	    TGeoVolume *ffsclay = new TGeoVolumeAssembly("FscLayer");
	    ffsclay->AddNode(volAbs, 0, new TGeoCombiTrans(0., 0., data.pAlp1/10., new TGeoRotation (rot)));
	    ffsclay->AddNode(volSci, 0, new TGeoCombiTrans(0., 0., 2*data.pAlp1/10+data.pDz/10.+data.pAlp2/10., new TGeoRotation (rot)));
	    AddSensitiveVolume(volSci); 
	    
	    for (Int_t ll=0; ll<data.pDx3; ll++) {
	      volCrystal->AddNode(ffsclay,ll, new TGeoCombiTrans(0., 0., ll*(2.*data.pDz+2.*data.pAlp1+2.*data.pAlp2)/10.-BoxZ, new TGeoRotation (rot)));
	      //cout << " check " <<  volCrystal->GetName() << " has " <<  ffsclay->GetName() << "  At  " << ll*(2.*data.pDz+2.*data.pAlp1+2.*data.pAlp2)/10. << endl;
	    }
	    flayer5->AddNode(volCrystal, 0, new TGeoCombiTrans(data.posX/10., data.posY/10., data.posZ/10.+ BoxZ, new TGeoRotation (rot)));
	    bIsModuleOn[module-1] = kTRUE;
	  }
      }
    }
  }
  
  // Fast construction of the fsc geometry
  if (bIsFastFsc)
    { 
      TGeoVolume *volAbs1, *volSci1, *volAbs2, *volSci2, *volAbs3, *volSci3;
      TGeoRotation rot;
      TGeoVolume *volume = new TGeoVolumeAssembly("FscBox");
      Int_t padX = 28, padY = 14, holeX = 4, holeY = 2, padX1 = 11, padX2 = 13;
      Float_t pDx1 = 11., pDx2 = 11., pDy1 = 11., pDy2 = 11., pAlp1 = 0.0275, pAlp2 = 0.15, pDz = 0.00375, posZ = 760.; 	    
      
      TGeoBBox *absorber1     = new TGeoBBox(pDx1*padX/2. , pDy1*(padY-holeY)/4., pAlp1/2.); 
      TGeoBBox *scintillator1 = new TGeoBBox(pDx2*padX/2. , pDy2*(padY-holeY)/4., pAlp2/2.);
      TGeoBBox *absorber2     = new TGeoBBox(pDx1*padX1/2., pDy1*holeY/4.       , pAlp1/2.); 
      TGeoBBox *scintillator2 = new TGeoBBox(pDx2*padX1/2., pDy2*holeY/4.       , pAlp2/2.);
      TGeoBBox *absorber3     = new TGeoBBox(pDx1*padX2/2., pDy1*holeY/4.       , pAlp1/2.); 
      TGeoBBox *scintillator3 = new TGeoBBox(pDx2*padX2/2., pDy2*holeY/4.       , pAlp2/2.);
      
      volAbs1 = new TGeoVolume("FscAbsorber1",     absorber1,     gGeoManager->GetMedium("lead"));
      volSci1 = new TGeoVolume("FscScintillator1", scintillator1, gGeoManager->GetMedium("FscScint"));
      volAbs2 = new TGeoVolume("FscAbsorber2",     absorber2,     gGeoManager->GetMedium("lead"));
      volSci2 = new TGeoVolume("FscScintillator2", scintillator2, gGeoManager->GetMedium("FscScint"));
      volAbs3 = new TGeoVolume("FscAbsorber3",     absorber3,     gGeoManager->GetMedium("lead"));
      volSci3 = new TGeoVolume("FscScintillator3", scintillator3, gGeoManager->GetMedium("FscScint"));
      
      TGeoVolume *volSci1X  = gGeoManager->Division("FscScintillator1X" ,"FscScintillator1" , 1, padX          , -pDx2*padX/2.        , pDx2);
      TGeoVolume *volSci1XY = gGeoManager->Division("FscScintillator1XY","FscScintillator1X", 2, (padY-holeY)/2, -pDy2*(padY-holeY)/4., pDy2);
      TGeoVolume *volSci2X  = gGeoManager->Division("FscScintillator2X" ,"FscScintillator2" , 1, padX1         , -pDx2*padX1/2.       , pDx2);
      TGeoVolume *volSci2XY = gGeoManager->Division("FscScintillator2XY","FscScintillator2X", 2, holeY/2       , -pDy2*holeY/4.       , pDy2);
      TGeoVolume *volSci3X  = gGeoManager->Division("FscScintillator3X" ,"FscScintillator3" , 1, padX2         , -pDx2*padX2/2.       , pDx2);
      TGeoVolume *volSci3XY = gGeoManager->Division("FscScintillator3XY","FscScintillator3X", 2, holeY/2       , -pDy2*holeY/4.       , pDy2);
      
      TGeoVolume *ffsclay = new TGeoVolumeAssembly("FscLayer");
      ffsclay->AddNode(volAbs1, 0, new TGeoCombiTrans(0.                    , pDy1*(padY+holeY)/4., pAlp1/2.          , new TGeoRotation (rot)));
      ffsclay->AddNode(volSci1, 0, new TGeoCombiTrans(0.                    , pDy2*(padY+holeY)/4., pAlp1+pDz+pAlp2/2., new TGeoRotation (rot)));
      ffsclay->AddNode(volAbs2, 0, new TGeoCombiTrans(pDx1*(-padX+padX1)/2., pDy1*holeY/4.        , pAlp1/2.          , new TGeoRotation (rot)));
      ffsclay->AddNode(volSci2, 0, new TGeoCombiTrans(pDx2*(-padX+padX1)/2., pDy2*holeY/4         , pAlp1+pDz+pAlp2/2., new TGeoRotation (rot)));
      ffsclay->AddNode(volAbs3, 0, new TGeoCombiTrans(pDx1*(padX-padX2)/2. , pDy1*holeY/4.        , pAlp1/2.          , new TGeoRotation (rot)));
      ffsclay->AddNode(volSci3, 0, new TGeoCombiTrans(pDx2*(padX-padX2)/2. , pDy2*holeY/4.        , pAlp1+pDz+pAlp2/2., new TGeoRotation (rot)));

      
      AddSensitiveVolume(volSci1XY); 
      AddSensitiveVolume(volSci2XY);  
      AddSensitiveVolume(volSci3XY);
      
      for (Int_t ll=1; ll<=300; ll++) {
	volume->AddNode(ffsclay,ll, new TGeoCombiTrans(0., 0., ll*(2.*pDz+pAlp1+pAlp2), new TGeoRotation (rot)));
      }
      
      flayer5->AddNode(volume, 0, new TGeoCombiTrans(0., 0., posZ, new TGeoRotation (rot)));
      bIsModuleOn[4] = kTRUE;
    }
  
  
  TGeoVolume *flayer12 = new TGeoVolumeAssembly("Emc12");
  if (bIsModuleOn[0]) flayer12->AddNode(flayer1,0, new TGeoCombiTrans(0., 0., 0., new TGeoRotation(0)));
  if (bIsModuleOn[1]) flayer12->AddNode(flayer2,0, new TGeoCombiTrans(0., 0., 0., new TGeoRotation(0)));
  TString vname = "cave";
  vname = vname.Strip();
  TGeoVolume* vcave = gGeoManager->FindVolumeFast(vname.Data());
  if (bIsModuleOn[0] || bIsModuleOn[1]) vcave->AddNode(flayer12, 1);
  if (bIsModuleOn[2]) vcave->AddNode(flayer3, 1);
  if (bIsModuleOn[3]) vcave->AddNode(flayer4, 1); 
  if (bIsModuleOn[4]) vcave->AddNode(flayer5, 1); 
  if (bIsModuleOn[5]) vcave->AddNode(flayer6, 1);
  
  // 15 copies for barrel part of EMC (1st copy exists in emc_module12345.dat)
  if (bIsModuleOn[0] || bIsModuleOn[1])
    for (Int_t n=1;n<=15;n++){ 
      TGeoRotation rot1;
      rot1.RotateZ(22.5*n);
      vcave->AddNode(flayer12, n+1,new TGeoCombiTrans(0., 0., 0., new TGeoRotation (rot1)) );
    }
   
  // 3 copies for forward endcap of EMC  (1st copy exists in emc_module12345.dat)
  if (bIsModuleOn[2])
    for (Int_t n=1;n<=3;n++){
      TGeoCombiTrans reflection;
      if (n==1) reflection.ReflectY(1);
      if (n==2) {reflection.ReflectY(1); reflection.ReflectX(1);}
      if (n==3) reflection.ReflectX(1);
      vcave->AddNode(flayer3, n+1, new TGeoCombiTrans(reflection));
    }
  
  // 3 copies for backward endcap of EMC  (1st copy exists in emc_module12345.dat)
  if (bIsModuleOn[3])
    for (Int_t n=1;n<=3;n++){
      TGeoCombiTrans reflection;
      if (n==1) reflection.ReflectY(1);
      if (n==2) {reflection.ReflectY(1); reflection.ReflectX(1);}
      if (n==3) reflection.ReflectX(1);
      vcave->AddNode(flayer4, n+1, new TGeoCombiTrans(reflection));
    }
  
  // 3 copies for forward calorimeter (1st copy exists in emc_module12345.dat)
  if (bIsModuleOn[4])
    {
      if (bIsFastFsc) 
	{
	  TGeoCombiTrans reflection;
	  reflection.ReflectY(1); 
	  vcave->AddNode(flayer5, 2, new TGeoCombiTrans(reflection));
	}
      else
	for (Int_t n=1;n<=3;n++){
	  TGeoCombiTrans reflection;
	  if (n==1) reflection.ReflectY(1);
	  if (n==2) {reflection.ReflectY(1); reflection.ReflectX(1);}
	  if (n==3) reflection.ReflectX(1);
	  vcave->AddNode(flayer5, n+1, new TGeoCombiTrans(reflection));
	}
    }
}

// -----   Private method AddHit   --------------------------------------------
PndEmcPoint* PndEmc::AddHit(Int_t trackID, Int_t detID, Int_t evtID, TVector3 pos, TVector3 mom, Double_t time, Double_t length, Double_t eLoss, Short_t mod, Short_t row, Short_t crys, Short_t copy) {
  TClonesArray& clref = *fEmcCollection;
  Int_t size = clref.GetEntriesFast();
   if (fVerboseLevel>1) 
     cout << "-I- PndEmc: Adding Point at IN (" << pos.X() << ", " << pos.Y() 
      << ", " << pos.Z() << ") cm, detector " << detID << ", evt " << evtID << ", track "
      << trackID <<", energy loss " << eLoss*1e06 << " keV, module " << mod << " row " << row << " crystal " <<  crys << " copy " << copy << endl;
  
  return new(clref[size]) PndEmcPoint(trackID, detID, evtID, pos, mom, time, length, eLoss,
				      mod, row, crys, copy);
}

inline void PndEmc::ResetParameters() {
  fTrackID = -999;
  fVolumeID = -999;
  fEventID = -999;
  fPos.SetXYZT(0., 0., 0., 0.);
  fMom.SetXYZT(0., 0., 0., 0.) ;
  fTime = -999;
  fLength = -999;
  fELoss = -999;
}

// ----


ClassImp(PndEmc)
