
// -------------------------------------------------------------------------
// -----                        PndDrc source file                     -----
// -----               Created 11/10/06 by Annalisa Cecchi             -----
// -----                                                               -----
// -------------------------------------------------------------------------

#include "PndDrc.h"

#include "PndDrcSurfPolyFlat.h"
#include "PndDrcOptReflSilver.h"
#include "PndDrcOptMatLithotecQ0.h"
#include "PndDrcOptDevSys.h"
#include "PndDrcOptVol.h"
#include "PndDrcOptDevManager.h"
#include "PndGeoDrc.h"
#include "PndDrcPDPoint.h"
#include "PndDrcBarPoint.h"
#include "PndGeoDrcPar.h"

#include "TString.h"
//#include <iostream>
using std::endl;
using std::cout;

#include "TClonesArray.h"
#include "TVirtualMC.h"
#include "TObjArray.h"
#include "TGeoMCGeometry.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoPgon.h"
#include "TGeoSphere.h"
#include "TGeoBBox.h"
#include "TGeoCompositeShape.h"
#include "TGeoMatrix.h"
#include "TGeoManager.h"
#include "TObject.h"

#include "CbmGeoInterface.h"
#include "CbmGeoLoader.h"
#include "CbmGeoNode.h"
#include "CbmRootManager.h"
#include "CbmVolume.h"
#include "CbmGeoMedia.h"
#include "CbmGeoMedium.h"
#include "CbmGeoRootBuilder.h"
#include "CbmStack.h"

// add on for debug
#include "CbmGeoG3Builder.h"
#include "CbmRun.h"
//#include "CbmRunSim.h"
#include "CbmRuntimeDb.h"


// -----   Default constructor   -------------------------------------------
PndDrc::PndDrc() {
  fDrcPDCollection = new TClonesArray("PndDrcPDPoint");
  fDrcBarCollection = new TClonesArray("PndDrcBarPoint");
  fPosIndex   = 0;
  volDetector = 0;
  fRunCherenkov = kTRUE;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndDrc::PndDrc(const char* name, Bool_t active)
  : CbmDetector(name, active) {
    fDrcPDCollection = new TClonesArray("PndDrcPDPoint");
    fDrcBarCollection = new TClonesArray("PndDrcBarPoint");
    fPosIndex   = 0;
    volDetector = 0;
    fEventID    = -1;
    fSenId1 =0;
    fSenId2 =0;
    fSenIdBar = 0;
    fRunCherenkov = kTRUE;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndDrc::~PndDrc() {
  if (fDrcPDCollection) {
    fDrcPDCollection->Delete();
    delete fDrcPDCollection;
  }

  if (fDrcBarCollection) {
    fDrcBarCollection->Delete();
    delete fDrcBarCollection;
  }
  
}
// -------------------------------------------------------------------------



// -----   Public method Intialize   ---------------------------------------
void PndDrc::Initialize() {
  
  CbmDetector::Initialize();
  CbmRun       *sim  = CbmRun::Instance();
  CbmRuntimeDb *rtdb = sim->GetRuntimeDb();
  PndGeoDrcPar *par  = (PndGeoDrcPar*)(rtdb->getContainer("PndGeoDrcPar"));


  // create a clone of each bar, which will handle the photon propagation.
  //
  //         p4----------p7
  //        /|          /|                 y
  //       / |         / |                 |  z
  //      /  p5-------/--p6                | /
  //     /  /        /  /                  |/
  //    /  /        /  /             x ----0
  //   /  /        /  /
  //  /  /        /  /
  // p0---------p3  /
  // | /         | /
  // |/          |/
  // p1---------p2 





 
  int verbosity = fVerboseLevel; // 0=quiet ... 5=talk too much


  XYZPoint p[8];   // 8 space points of radiator bar in sequence of above

  PndDrcOptDevManager* manager = new PndDrcOptDevManager();
  manager->SetVerbosity(verbosity);


  TObjArray    *sensNodes = par->GetGeoSensitiveNodes();

 //  CbmGeoNode *fm1= (CbmGeoNode *) sensNodes->FindObject("bar"); // barrel (single slab)
//   CbmGeoNode *fm2= (CbmGeoNode *) sensNodes->FindObject("pd"); // photodetector
  
//   fSenIdBar=fm1->getMCid();
//   fSenId2=fm2->getMCid();

 

  for( int inode=0; inode<sensNodes->GetEntries(); inode++) 
    {// for inode
      CbmGeoNode *node = dynamic_cast<CbmGeoNode*> (sensNodes->At(inode));	
      if ( !node ) continue;
      
      TString name = node->getName();
      TString shapeName = node->getShapePointer()->GetName();
      TString name_clone = "none";

      cout<<" node,name "<<inode<<" "<<name.Data()<<endl;//###

      if (shapeName.CompareTo("BOX"))
	{//if box
	  name_clone  = name;
	  name_clone += "_clone";
	    

	  CbmGeoTransform trans = node->getTransform();

	  for (int ii=0; ii<node->getNumPoints(); ii++)
	    {
	      CbmGeoVector vec = *(node->getPoint(ii));
	      CbmGeoVector vec1 = trans.transFrom(vec);
	      p[ii].SetXYZ(vec1.X(),vec1.Y(),vec1.Z());
	      //cout<<" points: "<<vec1.X()<<" "<<vec1.Y()<<" "<<vec1.Z()<<endl;
	    }

	  // assemble bar

	  // Declare 6 flat surfaces with 4 edge points.
	  PndDrcSurfPolyFlat a1,a2,a3,a4,a5,a6;

	  a1.SetVerbosity(verbosity);
	  a1.AddPoint(p[0]);
	  a1.AddPoint(p[1]);
	  a1.AddPoint(p[2]);
	  a1.AddPoint(p[3]);
	  a1.SetPixel();                // screen
	  a1.SetName("a_upstream");

	  a2.SetVerbosity(verbosity);
	  a2.AddPoint(p[1]);
	  a2.AddPoint(p[5]);
	  a2.AddPoint(p[6]);
	  a2.AddPoint(p[2]);
	  a2.SetName("a_side1");

	  a3.SetVerbosity(verbosity);
	  a3.AddPoint(p[0]);
	  a3.AddPoint(p[4]);
	  a3.AddPoint(p[5]);
	  a3.AddPoint(p[1]);
	  a3.SetName("a_side2");

	  a4.SetVerbosity(verbosity);
	  a4.AddPoint(p[3]);
	  a4.AddPoint(p[2]);
	  a4.AddPoint(p[6]);
	  a4.AddPoint(p[7]);
	  a4.SetName("a_side3");

	  a5.SetVerbosity(verbosity);
	  a5.AddPoint(p[0]);
	  a5.AddPoint(p[3]);
	  a5.AddPoint(p[7]);
	  a5.AddPoint(p[4]);
	  a5.SetName("a_side4");
	  
	  PndDrcOptReflSilver refl;

	  a6.SetVerbosity(verbosity);
	  a6.SetReflectivity(refl);   // mirror
	  a6.AddPoint(p[7]);
	  a6.AddPoint(p[6]);
	  a6.AddPoint(p[5]);
	  a6.AddPoint(p[4]);
	  a6.SetName("a_downstream");

	  // create a volume consiting of surfaces
	  // create a material the bar will consist of
	  PndDrcOptVol bar;
	  PndDrcOptMatLithotecQ0 quartz;
	  bar.SetVerbosity(verbosity);
	  bar.SetOptMaterial(quartz);
	  bar.AddSurface(a1);
	  bar.AddSurface(a2);
	  bar.AddSurface(a3);
	  bar.AddSurface(a4);
	  bar.AddSurface(a5);
	  bar.AddSurface(a6);
	  bar.SetName("bar");
	  //bar.setCopyNumber(inode);

	  // create a screen (simplified photon detector) where photons end and 
	  // get the status Drc::Measured (no further propagation). For this you need 
	  // one single plane.
	  

	  // Build a optical system consisting out of several 
	  // volumes, mirrors and screens. This layer has the advantage, that 
	  // a device consisting out of many equal subsystems
	  // like a bar box, easily can be reproduced. See one of the 
  // test examples in ./drcprop.

	  PndDrcOptDevSys opt_system;
	  opt_system.SetVerbosity(verbosity);
	  opt_system.AddDevice(bar);
	  opt_system.SetNameCopyNumber(name_clone.Data(),inode);
	  manager->AddDeviceSystem(opt_system);

	}
    } 
  
  if (fRunCherenkov==kFALSE) cout << " -I- PndDrc: Switching OFF Cherenkov Propagation" << endl;
  cout << " -I- PndDrc: Intialization successfull" << endl;
  
  // print out for debugging all names and pointers stored in manager
  // manager->Print();
}

// -------------------------------------------------------------------------
void PndDrc::BeginEvent(){
  
  if (fVerboseLevel >0) cout<<" \n\n>>>>>>>>>>>>>>>>>>>>new event in the Barrel DIRC" <<endl;
  fEventID++;
}



// -----   Public method ProcessHits  --------------------------------------
Bool_t PndDrc::ProcessHits(CbmVolume* vol) {
  
  if (fVerboseLevel >0) cout << "PndDrc::ProcessHits " << vol->GetName() << endl;
 
  //Register points in the barrel (PndDrcBarPoints)
  Int_t  fEventID = gMC->CurrentEvent();
  Int_t  fPdgCode = gMC->TrackPid(); 
  Double_t fCharge = gMC->TrackCharge();

  if (fRunCherenkov==kFALSE && fPdgCode == 50000050) 
    {
      gMC->StopTrack();
      if (fVerboseLevel >0) cout<< "Photon killed" << endl;
    }
  
  TLorentzVector fPos, fMom;
  gMC->TrackPosition(fPos);
  TString nam = gMC->CurrentVolName();
  
  if (nam.BeginsWith("bar") && gMC->IsTrackEntering()==1 &&  fPdgCode != 50000050 && fCharge !=0.) 
  {
    Int_t  fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
    Double_t fTime    = gMC->TrackTime() * 1.0e09;
    Double_t fLength = gMC->TrackLength();
    Int_t  copyNo = vol->getCopyNo();
           
    Int_t s, b; //side and bar
    TString path = gMC->CurrentVolPath();     
    if (fVerboseLevel >0) cout<< "Volume: " << gMC->CurrentVolPath() << endl;
    
    sscanf(path, "/cave_1/baseVol_1/side_%d/box_1/barContainer_%d/bar_1", &s, &b);
    
    Int_t fNBar = s*10 +b;
    gMC->TrackMomentum(fMom); // GeV/c

 //    Double_t r = 49.7;
//     Double_t phi = acos(fPos.X()/r);
//     Double_t tht;
//     if (fPos.Z() !=0) tht = atan(fPos.Y()/fPos.Z());
    //  cout << "hit phi: "<< phi <<";   theta = " << tht << endl;

    Double_t Px= fMom.Px();
    Double_t Py= fMom.Py();
    Double_t Pz= fMom.Pz();
    Double_t fP = sqrt(Px*Px + Py*Py +Pz*Pz);
    Double_t fMass = gMC->TrackMass();
    Double_t fEnergy = TMath::Sqrt(fP*fP + fMass*fMass); 
    
    Double_t fAngIn;
 if ( fabs(Pz/fP) > 1. || fP == 0.){
      fAngIn = -1.;
    }
    else{
      fAngIn = acos(Pz/fP);
    }
    
    Double_t fThetaC;
    if (fabs(1./(1.47*(fP/fEnergy))) > 1. || fP == 0. || fEnergy == 0.){
      fThetaC = -1.;
    }
    else{
      fThetaC = acos(1/(1.47*(fP/fEnergy)));
    } 
    
    AddBarHit(fTrackID,
	      fCopyNo,
	      TVector3(fPos.X(),   fPos.Y(),   fPos.Z()),
	      TVector3(fMom.Px(),  fMom.Py(),  fMom.Pz()),
	      fTime,
	      fLength,
	      fPdgCode,
	      fAngIn,
	      fThetaC,
	      fNBar,
	      fEventID,
	      fMass);
    
  }
  
  if (nam.BeginsWith("bar") && gMC->IsTrackExiting()==1 &&  fPdgCode == 50000050 && fPos.Z() > -148.0 ) 
    
    {
      gMC->StopTrack();
      if (fVerboseLevel >0) cout<< "Photon killed" << endl;
      
    }
  
  
  if (nam.BeginsWith("pd") && gMC->IsTrackEntering()==1 && fPdgCode == 50000050) 
    {
      Int_t  fCopyNo = vol->getCopyNo();
      Int_t  fTrackID = gMC->GetStack()->GetCurrentTrackNumber(); //track ID     
      
      gMC->TrackPosition(fPos);
      gMC->TrackMomentum(fMom); // GeV/c
      AddHit(fTrackID,
	     fCopyNo,
	     TVector3(fPos.X(),   fPos.Y(),   fPos.Z()),
	     TVector3(fMom.Px(),  fMom.Py(),  fMom.Pz()),
	     fTime,
	     fLength,
	     fPdgCode,
	     fEventID);
    } 
  
  
  
  ResetParameters();
  
  return kTRUE; 
  
  
  
}

// ---------------------------------------------------------------------------



// -----   Public method EndOfEvent   -----------------------------------------
void PndDrc::EndOfEvent() {
  if (fVerboseLevel)  Print();
  Reset();
}
// ----------------------------------------------------------------------------



// -----   Public method Register   -------------------------------------------
void PndDrc::Register() {
  CbmRootManager::Instance()->Register("DrcBarPoint","Drc", fDrcBarCollection, kTRUE);
  CbmRootManager::Instance()->Register("DrcPDPoint","Drc", fDrcPDCollection, kTRUE);

}
// ----------------------------------------------------------------------------



// -----   Public method GetCollection   --------------------------------------
TClonesArray* PndDrc::GetCollection(Int_t iColl) const {
   if (iColl == 0) return fDrcPDCollection;
   if (iColl == 1) return fDrcBarCollection;
  return NULL;
}
// ----------------------------------------------------------------------------




// -----   Public method Print   ----------------------------------------------
void PndDrc::Print() const {
    Int_t nPDHits = fDrcPDCollection->GetEntriesFast();
    Int_t nbarHits = fDrcBarCollection->GetEntriesFast();
    cout << "-I- PndDrc: " << nPDHits << " points registered in the photodetector for this event." << endl;
    cout << "-I- PndDrc: " << nbarHits << " points registered in the bar for this event." 	<< endl;

 if (fVerboseLevel>1){
   for (Int_t i=0; i<nPDHits; i++) (*fDrcPDCollection)[i]->Print();
   for (Int_t i=0; i<nbarHits; i++) (*fDrcBarCollection)[i]->Print();
 }
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void PndDrc::Reset() {
   fDrcPDCollection->Clear();
   fDrcBarCollection->Clear();
   fPosIndex = 0;
}
// ----------------------------------------------------------------------------


// guarda in CbmRootManager::CopyClones
// -----   Public method CopyClones   -----------------------------------------
void PndDrc::CopyClones(TClonesArray* clPD1, TClonesArray* clPD2,TClonesArray* clBar1, TClonesArray* clBar2, Int_t offset ) {
  Int_t nPDEntries = clPD1->GetEntriesFast();
  cout << "-I- PndDrc: " << nPDEntries << " entries to add." << endl;
  TClonesArray& clrefPD = *clPD2;

  Int_t nBarEntries = clBar1->GetEntriesFast();
  cout << "-I- PndDrc: " << nBarEntries << " entries to add." << endl;
  TClonesArray& clrefBar = *clBar2;

  PndDrcPDPoint* oldpointPD = NULL;
  PndDrcBarPoint* oldpointBar = NULL;

  for (Int_t i=0; i<nPDEntries; i++) {
    oldpointPD = (PndDrcPDPoint*) clPD1->At(i);
    Int_t indexPD = oldpointPD->GetTrackID() + offset;
    oldpointPD->SetTrackID(indexPD);
    new (clrefPD[fPosIndex]) PndDrcPDPoint(*oldpointPD);
    fPosIndex++;
  }

  for (Int_t i=0; i<nBarEntries; i++) {
    oldpointBar = (PndDrcBarPoint*) clBar1->At(i);
    Int_t indexBar = oldpointBar->GetTrackID() + offset;
    oldpointBar->SetTrackID(indexBar);
    new (clrefBar[fPosIndex]) PndDrcBarPoint(*oldpointBar);
    fPosIndex++;
  }

  cout << " -I- PndDrc: " << clPD2->GetEntriesFast() << " merged entries." << endl;
  cout << " -I- PndDrc: " << clBar2->GetEntriesFast() << " merged entries." << endl;
}
// ----------------------------------------------------------------------------



// -----   Public method ConstructGeometry  -----------------------------------
void PndDrc::ConstructGeometry()
{
  cout<< "                                               " << endl;
  cout<< " =======  DRC::  ConstructGeometry()  ======== " << endl;
  cout<< " ============================================= " << endl;
  
  CbmGeoLoader*    drcgeoLoad = CbmGeoLoader::Instance();
  CbmGeoInterface* drcgeoFace = drcgeoLoad->getGeoInterface();

  CbmGeoMedia *Media =  drcgeoFace->getMedia();
  CbmGeoBuilder *geobuild = drcgeoLoad->getGeoBuilder();

  // Call materials
  CbmGeoMedium *fusedSil  = Media->getMedium("FusedSil");
  Int_t nFusedSil = geobuild->createMedium(fusedSil);
  CbmGeoMedium *nlak33a  = Media->getMedium("NLAK33A");
  Int_t nNlak33a = geobuild->createMedium(nlak33a);
  CbmGeoMedium *air  = Media->getMedium("DIRCair");
  Int_t nAir = geobuild->createMedium(air);
  CbmGeoMedium *airNoSens  = Media->getMedium("DIRCairNoSens");
  Int_t nAirNoSens = geobuild->createMedium(airNoSens);
  CbmGeoMedium *mirror  = Media->getMedium("Mirror");
  Int_t nMirror = geobuild->createMedium(mirror);

  TGeoVolume *cave = gGeoManager->GetTopVolume();
 
  TGeoRotation rot;
  rot.RotateZ(11.25);

  
  // Create base volume 
  TGeoPgon* basePol = new TGeoPgon("basePol",11.25, 360., 16, 2);
  basePol->DefineSection(0, -130., 45., 53.8);
  basePol->DefineSection(1, 130., 45., 53.8);
  TGeoVolume *baseVol = new TGeoVolume("baseVol",basePol,gGeoManager->GetMedium("DIRCairNoSens"));
  cave->AddNode(baseVol, 1, new TGeoCombiTrans(0, 0, -20., new TGeoRotation(0)));


  //Create the sides
  Double_t lside = 18.7286709135483108; //side length (cm)
  TGeoBBox* logicSide = new TGeoBBox("logicSide", lside/2, 0.85, 130.);
  TGeoVolume *side = new TGeoVolume("side",logicSide, gGeoManager->GetMedium("DIRCairNoSens"));
  baseVol->AddNode(side, 1, new TGeoCombiTrans(0., 48.85, 0., new TGeoRotation(0))); 
  Int_t n = 1;
  Double_t rad = 48.85;
  Double_t dx, dy, dz;


  for (Double_t theta=22.5; theta<360.; theta+=22.5){
    n++;
    Double_t tht=theta* TMath::Pi()/180.;
    dx=rad*sin(tht);
    dy=rad*cos(tht);
    dz=0.;
    TGeoRotation rot1;
    rot1.RotateZ(-22.5*(n-1));
    baseVol->AddNode(side, n,new TGeoCombiTrans(dx, dy, dz, new TGeoRotation (rot1)) );


  }

  // Box contains 6 bars 
  TGeoBBox* logicBox = new TGeoBBox("logicBox", lside/2, 0.85, 129.9);
  TGeoBBox* logicMirror = new TGeoBBox("logicMirror", lside/2, 0.85, 0.1);
  TGeoVolume *box = new TGeoVolume("box",logicBox, gGeoManager->GetMedium("DIRCairNoSens"));
  TGeoVolume *mirr = new TGeoVolume("mirr",logicMirror, gGeoManager->GetMedium("Mirror"));
  side->AddNode(box, 1,new TGeoCombiTrans(0., 0., -0.1, new TGeoRotation (0)) );
  side->AddNode(mirr, 1,new TGeoCombiTrans(0., 0., 129.9, new TGeoRotation (0)) );

  //  Bars container (slab + lens)
  Double_t r = 15.29; // first lens radius (cm)
  Double_t alpha = TMath::ASin(0.85/r); // 8.5 mm - half height of the bar
  Double_t a = r - r*TMath::Cos(alpha);
  Double_t b = a + .5; // box dimension

  Double_t r2 = 3.6; // radius second lens (cm)
  Double_t alpha2 = TMath::ASin(0.85/r2); // 8.5 mm - half height of the bar
  Double_t a2 = r2 - r2*TMath::Cos(alpha2);
  Double_t b2 = .5 + a2;
 
  Double_t l = 0.5+ 0.2+ 0.5+ a2; // dimension of the box containing both lenses

  TGeoBBox* logicBarContainer = new TGeoBBox("logicBarContainer",  (lside/6)/2, 0.85, 129.9);
  TGeoVolume *barContainer = new TGeoVolume("barContainer",logicBarContainer, gGeoManager->GetMedium("DIRCairNoSens"));
  box->AddNode(barContainer, 1,new TGeoCombiTrans(-lside/2+(lside/12), 0., 0., new TGeoRotation (0)) );

  for (Int_t j = 1; j <6 ; j++)
     {
       
       dx= - lside/2 + (lside/6)/2  + j * (lside/6); 
       dy=0.;
       dz=0.;
       box->AddNode(barContainer, 1+j,new TGeoCombiTrans(dx, dy, dz, new TGeoRotation (0)) );
       
     }

  // Fused Silica bars
  TGeoBBox* logicBar = new TGeoBBox("logicBar",  ((lside/6)/2)-0.05, 0.85, 129.9-l/2);
  TGeoVolume *bar = new TGeoVolume("bar",logicBar, gGeoManager->GetMedium("FusedSil"));
  barContainer->AddNode(bar, 1,new TGeoCombiTrans(0., 0., l/2, new TGeoRotation (0)) );
  AddSensitiveVolume(bar);

 
  // SOB
  TGeoPgon* baseSOB = new TGeoPgon("baseSOB",11.25, 360., 16, 3);
  baseSOB->DefineSection(0, 0., 48., 88.);
  baseSOB->DefineSection(1, 10., 48., 88.);
  baseSOB->DefineSection(2, 30., 48., 49.7);
  TGeoVolume *sob = new TGeoVolume("sob",baseSOB, gGeoManager->GetMedium("DIRCairNoSens"));
  cave->AddNode(sob, 1,new TGeoCombiTrans(0., 0., -180., new TGeoRotation (0)));

  // Photodetector
  TGeoPgon* logicPD = new TGeoPgon("logicPD",11.25, 360., 16, 2);
  logicPD->DefineSection(0, 0., 48., 88.);
  logicPD->DefineSection(1, 0.1, 48., 88.);
  TGeoVolume *pd = new TGeoVolume("pd", logicPD, gGeoManager->GetMedium("DIRCair"));
  sob->AddNode(pd, 1,new TGeoCombiTrans(0., 0., 0., new TGeoRotation (0)));
  AddSensitiveVolume(pd); 
  
  // Lenses
 
  // Lens 1
  Double_t t = -r +b/2;
  TGeoSphere* logicSphere= new TGeoSphere("S",0.,r, 0. ,180.,0.,360.);
  TGeoBBox* lBox = new TGeoBBox("B", (lside/6)/2-0.05, 0.85, b/2.);
  TGeoTranslation *tr1 = new TGeoTranslation("tr1", 0.,0., t);
  tr1->RegisterYourself();
  TGeoCompositeShape *cs = new TGeoCompositeShape("cs","S*B:tr1");
  TGeoVolume *lens1 = new TGeoVolume("LENS1",cs, gGeoManager->GetMedium("FusedSil"));
 //  TGeoRotation rot2;
//   rot2.RotateX(180.);
  barContainer->AddNode(lens1, 1,new TGeoCombiTrans(0., 0., -129.9 +r +a2 + 0.5 + 0.2 - a , new TGeoRotation (0)));
  

   //Lens 2
  Double_t t2 = -r2 +b2/2;

  TGeoSphere* logicSphere2= new TGeoSphere("S2", r2-a2 ,r2, 0. ,180.,0.,360.);
  TGeoBBox* lBox2 = new TGeoBBox("B2", (lside/6)/2-0.05, 0.85, b2/2.);
  TGeoTranslation *tr2 = new TGeoTranslation("tr2", 0.,0., t2);
  tr2->RegisterYourself();
  TGeoCompositeShape *cs2 = new TGeoCompositeShape("cs2","S2*B2:tr2");
  TGeoVolume *lens2 = new TGeoVolume("LENS2",cs2, gGeoManager->GetMedium("NLAK33A"));
 
  barContainer->AddNode(lens2, 1,new TGeoCombiTrans(0., 0., -129.9+r2, new TGeoRotation (0)));
  

  gGeoManager->CloseGeometry();

}

// -----   Public method CheckIfSensitive   --------------------------------------
// bool PndDrc::CheckIfSensitive(std::string name)
// {
//  for (Int_t i = 0; i < fListOfSensitives.size(); i++){
//   if (name.find(fListOfSensitives[i]) != std::string::npos)
//    return true;
//  }
// return false;
// }



// -----   Private method AddHit   --------------------------------------------
PndDrcPDPoint* PndDrc::AddHit(Int_t trackID, Int_t copyNo, TVector3 pos, TVector3 mom, Double_t time, Double_t length, Int_t pdgCode, Int_t eventID) {
 
  TClonesArray& clrefPD = *fDrcPDCollection;
  Int_t size = clrefPD.GetEntriesFast();
  if (fVerboseLevel>1) 
    cout << "-I- PndDrc: Adding Point at (" << pos.X() << ", " << pos.Y() 
	 << ", " << pos.Z() << ") cm, detector " << copyNo << ", track "
	 << trackID <<" event "<<eventID << endl;
  return new(clrefPD[size]) PndDrcPDPoint(trackID, 
					  copyNo, 
					  pos, 
					  mom, 
					  time, 
					  length, 
					  pdgCode,
					  eventID);

}


PndDrcBarPoint* PndDrc::AddBarHit(Int_t trackID, Int_t copyNo, TVector3 pos, TVector3 mom, Double_t time, Double_t length, Int_t pdgCode, Double_t angIn, Double_t thetaC, Int_t nBar, Int_t eventID, Double_t mass) {
 
  TClonesArray& clrefBar = *fDrcBarCollection;
  Int_t size = clrefBar.GetEntriesFast();
  if (fVerboseLevel>1) 
    cout << "-I- PndBarDrc: Adding Point at (" << pos.X() << ", " << pos.Y() 
	 << ", " << pos.Z() << ") cm, detector " << copyNo << ", track "
	 << trackID <<" event "<<eventID << endl;
  return new(clrefBar[size]) PndDrcBarPoint(trackID, 
					    copyNo, 
					    pos, 
					    mom, 
					    time, 
					    length, 
					    pdgCode,
					    angIn,
					    thetaC,
					    nBar,
					    eventID, 
					    mass);
  
}


// ----


ClassImp(PndDrc)
