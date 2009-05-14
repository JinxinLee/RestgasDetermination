
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
#include "PndDetectorList.h"

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

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairVolume.h"
#include "FairGeoMedia.h"
#include "FairGeoMedium.h"
#include "FairGeoRootBuilder.h"
#include "PndStack.h"
#include "PndDetectorList.h"

// add on for debug
#include "FairGeoG3Builder.h"
#include "FairRun.h"
//#include "FairRunSim.h"
#include "FairRuntimeDb.h"


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
  : FairDetector(name, active) {
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
  
  FairDetector::Initialize();
  FairRun       *sim  = FairRun::Instance();
  FairRuntimeDb *rtdb = sim->GetRuntimeDb();
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

 //  FairGeoNode *fm1= (FairGeoNode *) sensNodes->FindObject("bar"); // barrel (single slab)
//   FairGeoNode *fm2= (FairGeoNode *) sensNodes->FindObject("pd"); // photodetector
  
//   fSenIdBar=fm1->getMCid();
//   fSenId2=fm2->getMCid();

 

  for( int inode=0; inode<sensNodes->GetEntries(); inode++) 
    {// for inode
      FairGeoNode *node = dynamic_cast<FairGeoNode*> (sensNodes->At(inode));	
      if ( !node ) continue;
      
      TString name = node->getName();
      TString shapeName = node->getShapePointer()->GetName();
      TString name_clone = "none";

      cout<<" node,name "<<inode<<" "<<name.Data()<<endl;//###

      if (shapeName.CompareTo("BOX"))
	{//if box
	  name_clone  = name;
	  name_clone += "_clone";
	    

	  FairGeoTransform trans = node->getTransform();

	  for (int ii=0; ii<node->getNumPoints(); ii++)
	    {
	      FairGeoVector vec = *(node->getPoint(ii));
	      FairGeoVector vec1 = trans.transFrom(vec);
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
Bool_t PndDrc::ProcessHits(FairVolume* vol) {
  
  TString nam =vol->GetName();
  if (fVerboseLevel >0)
    {
      //if (! nam.BeginsWith("DrcBar")) 
	cout << "PndDrc::ProcessHits " << nam << endl;
    }
  
   //Register points in the barrel (PndDrcBarPoints)
  Int_t  fEventID = gMC->CurrentEvent();
  Int_t  fPdgCode = gMC->TrackPid(); 
 
  TLorentzVector fPos, fMom;
  gMC->TrackPosition(fPos);
  //TString nam = gMC->CurrentVolName();
  //TString nam =vol->GetName();
  if (fPdgCode == 50000050){
    if (fRunCherenkov==kFALSE ) {
      gMC->StopTrack();
      if (fVerboseLevel >0) cout<< "Photon killed" << endl;
    }
    if (gMC->IsTrackExiting()==1){
      if (fPos.Z() > -fSlabEnd ){
        if (nam.BeginsWith("DrcBar") ) {
           gMC->StopTrack();
           if (fVerboseLevel >0) cout<< "Photon killed" << " at z= "<<fPos.Z()<<endl;
        }
     }
   }
   if (gMC->IsTrackEntering()==1){
      if (nam.BeginsWith("DrcPd")){
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
       PndStack* stack = (PndStack*) gMC->GetStack();
       stack->AddPoint(kDRC);
     }
  }else if(gMC->TrackCharge()!=0.&&gMC->IsTrackEntering()==1 ){
        if (nam.BeginsWith("DrcBar")) {
		Double_t fCharge = gMC->TrackCharge();
		Int_t  fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
		Double_t fTime  = gMC->TrackTime() * 1.0e09;
		Double_t fLength = gMC->TrackLength();
		Int_t  copyNo = vol->getCopyNo();
		Int_t s, b; //side and bar
		TString path = gMC->CurrentVolPath();     
		if (fVerboseLevel >0) cout<< "Volume: " << gMC->CurrentVolPath() << endl;
		//  cout << path << endl;    
		sscanf(path, "/cave_1/DrcBase_1/DrcSide_%d/DrcBox_1/DrcBarContainer_%d/DrcBar_1", &s, &b);
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
		if ( fabs(Pz/fP) > 1. || fP == 0.){ fAngIn = -1.;
		}else{ fAngIn = acos(Pz/fP);}
		Double_t fThetaC;
		if (fabs(1./(1.47*(fP/fEnergy))) > 1. || fP == 0. || fEnergy == 0.){
			fThetaC = -1.;
		}else{
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
		        PndStack* stack = (PndStack*) gMC->GetStack();
       			stack->AddPoint(kDRC);
	}
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
  FairRootManager::Instance()->Register("DrcBarPoint","Drc", fDrcBarCollection, kTRUE);
  FairRootManager::Instance()->Register("DrcPDPoint","Drc", fDrcPDCollection, kTRUE);

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


// guarda in FairRootManager::CopyClones
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
  
  const Double_t pi = 3.1415926535;

  FairGeoLoader*    drcgeoLoad = FairGeoLoader::Instance();
  FairGeoInterface* drcgeoFace = drcgeoLoad->getGeoInterface();

  FairGeoMedia *Media =  drcgeoFace->getMedia();
  FairGeoBuilder *geobuild = drcgeoLoad->getGeoBuilder();

  // Call materials
  FairGeoMedium *fusedSil  = Media->getMedium("FusedSil");
  Int_t nFusedSil = geobuild->createMedium(fusedSil);
  FairGeoMedium *nlak33a  = Media->getMedium("NLAK33A");
  Int_t nNlak33a = geobuild->createMedium(nlak33a);
  FairGeoMedium *air  = Media->getMedium("DIRCair");
  Int_t nAir = geobuild->createMedium(air);
  FairGeoMedium *airNoSens  = Media->getMedium("DIRCairNoSens");
  Int_t nAirNoSens = geobuild->createMedium(airNoSens);
  FairGeoMedium *mirror  = Media->getMedium("Mirror");
  Int_t nMirror = geobuild->createMedium(mirror);
  FairGeoMedium *marcol82  = Media->getMedium("Marcol82");
  Int_t nMarcol82 = geobuild->createMedium(marcol82);

  TGeoVolume *cave = gGeoManager->GetTopVolume();
 
  TGeoRotation rot;
  rot.RotateZ(11.25);

  Double_t eps           = 0.01;                                  // epsilon

  Double_t radius        =  51.2;                                   // radius in the middle of the barbox (x and y)
  Double_t hthick        =  1.7 / 2;                              // half thickness of the bars
  //Double_t bbox_zdown    =  130.0;                                // bar box z downstream
  //Double_t bbox_zup      = -149.0; //###                                // bar box z upstream
  Double_t bbox_zdown    =  130.0; //###                                // bar box z downstream
  Double_t bbox_zup      = -120.0; //###                               // bar box z upstream
  Double_t bbox_hlen     = 0.5*(bbox_zdown - bbox_zup);           // bar box half length
  Double_t bbox_shift    = bbox_zup + bbox_hlen;                  // bar box shift


  
  // Create base volume 
  TGeoPgon* basePol = new TGeoPgon("basePol",0, 360., 16, 2);
  basePol->DefineSection(0, -bbox_hlen, 45., 53.8);
  basePol->DefineSection(1, +bbox_hlen, 45., 53.8);
  TGeoVolume *baseVol = new TGeoVolume("DrcBase",basePol,gGeoManager->GetMedium("DIRCairNoSens"));
  cave->AddNode(baseVol, 1, new TGeoCombiTrans(0, 0, bbox_shift, new TGeoRotation(0)));

  //cout<<" bbox_shift = "<<bbox_shift<<endl;
  
  //Create the sides           lside is the width in the middle of the barbox

  Double_t rad_out = (radius-hthick)/cos(2*pi/16/2); // radius at corner - thickness ###
  Double_t lside   = 2*rad_out*sin(2*pi/16/2);
  cout<<" DIRC min. radius = "<<radius-hthick<<endl;
  cout<<" DIRC max. radius = "<<rad_out+2*hthick<<endl;
  TGeoBBox* logicSide = new TGeoBBox("logicSide", lside/2, hthick, bbox_hlen);
  TGeoVolume *side = new TGeoVolume("DrcSide",logicSide, gGeoManager->GetMedium("DIRCairNoSens"));
  // 2 special sides where slabs will be missing:
  TGeoBBox* logicSide1 = new TGeoBBox("logicSide1", lside/2, hthick, bbox_hlen);
  TGeoVolume *side1 = new TGeoVolume("DrcSide1",logicSide1, gGeoManager->GetMedium("DIRCairNoSens"));
  TGeoBBox* logicSide2 = new TGeoBBox("logicSide2", lside/2, hthick, bbox_hlen);
  TGeoVolume *side2 = new TGeoVolume("DrcSide2",logicSide2, gGeoManager->GetMedium("DIRCairNoSens"));



  Int_t n = 0;
  Double_t dx, dy, dz;


  for (Double_t theta=-11.25; theta<360.-22.5; theta+=22.5)
    {
      n++;
      Double_t tht=theta* pi/180.;
      dx=radius*sin(tht);
      dy=radius*cos(tht);
      dz=0.;
      TGeoRotation rot1;
      rot1.RotateZ(-22.5*(n-1)+11.25);
      if      (n==1 || n==9)
	{
	  baseVol->AddNode(side1, n,new TGeoCombiTrans(dx, dy, dz, new TGeoRotation (rot1)) );
	}
      else if (n==2 || n==10)
	{
	  baseVol->AddNode(side2, n,new TGeoCombiTrans(dx, dy, dz, new TGeoRotation (rot1)) );
	}
      else
	{
	  baseVol->AddNode(side, n,new TGeoCombiTrans(dx, dy, dz, new TGeoRotation (rot1)) );
	}
      
    }

  // Box contains 6 bars 
  TGeoBBox* logicBox = new TGeoBBox("logicBox", lside/2, hthick, bbox_hlen-eps);
  TGeoBBox* logicBox1 = new TGeoBBox("logicBox1",  (lside*5./6.)/2, hthick, bbox_hlen-eps);
  TGeoBBox* logicBox2 = new TGeoBBox("logicBox2",  (lside*5./6.)/2, hthick, bbox_hlen-eps);

  TGeoBBox* logicMirror = new TGeoBBox("logicMirror", lside/2, hthick, eps);
  TGeoBBox* logicMirror1 = new TGeoBBox("logicMirror1", (lside*5./6.)/2, hthick, eps);
  TGeoBBox* logicMirror2 = new TGeoBBox("logicMirror2", (lside*5./6.)/2, hthick, eps);

  TGeoVolume *box = new TGeoVolume("DrcBox",logicBox, gGeoManager->GetMedium("DIRCairNoSens"));
  TGeoVolume *box1  = new TGeoVolume("DrcBox1",logicBox1, gGeoManager->GetMedium("DIRCairNoSens"));
  TGeoVolume *box2  = new TGeoVolume("DrcBox2",logicBox2, gGeoManager->GetMedium("DIRCairNoSens"));

  TGeoVolume *mirr = new TGeoVolume("DrcMirr",logicMirror, gGeoManager->GetMedium("Mirror"));
  TGeoVolume *mirr1 = new TGeoVolume("DrcMirr1",logicMirror1, gGeoManager->GetMedium("Mirror"));
  TGeoVolume *mirr2 = new TGeoVolume("DrcMirr2",logicMirror2, gGeoManager->GetMedium("Mirror"));


  side->AddNode(box, 1,new TGeoCombiTrans(0., 0., -eps, new TGeoRotation (0)) );
  side->AddNode(mirr, 1,new TGeoCombiTrans(0., 0., bbox_hlen-eps, new TGeoRotation (0)) );

  side1->AddNode(box1,  1,new TGeoCombiTrans(-lside*1./12, 0., -eps,          new TGeoRotation (0) ));
  side1->AddNode(mirr1, 1,new TGeoCombiTrans(-lside*1./12, 0., bbox_hlen-eps, new TGeoRotation (0) ));

  side2->AddNode(box2,  1,new TGeoCombiTrans(lside*1./12, 0., -eps, new TGeoRotation (0)));
  side2->AddNode(mirr2, 1,new TGeoCombiTrans(lside*1./12, 0., bbox_hlen-eps, new TGeoRotation (0)) );





  //  Bars container (slab + lens)


  // slabs
  TGeoBBox* logicBarContainer = new TGeoBBox("logicBarContainer",  (lside/6)/2, hthick, bbox_hlen-eps);
  TGeoBBox* logicBarContainer1 = new TGeoBBox("logicBarContainer1",(lside/6)/2, hthick, bbox_hlen-eps);
  TGeoBBox* logicBarContainer2 = new TGeoBBox("logicBarContainer2",(lside/6)/2, hthick, bbox_hlen-eps);


  TGeoVolume *barContainer = new TGeoVolume("DrcBarContainer",logicBarContainer, gGeoManager->GetMedium("DIRCairNoSens"));
  TGeoVolume *barContainer1= new TGeoVolume("DrcBarContainer",logicBarContainer, gGeoManager->GetMedium("DIRCairNoSens"));
  TGeoVolume *barContainer2= new TGeoVolume("DrcBarContainer",logicBarContainer, gGeoManager->GetMedium("DIRCairNoSens"));
  //box->AddNode(barContainer, 1,new TGeoCombiTrans(-lside/2+(lside/12), 0., 0., new TGeoRotation (0)) );

  for (Int_t j = 0; j <6 ; j++)
     { 
       dx= - lside/2 + (lside/6)/2  + j * (lside/6); 
       dy=0.;
       dz=0.;
       box->AddNode(barContainer, 1+j,new TGeoCombiTrans(dx, dy, dz, new TGeoRotation (0)) ); 
     }
  for (Int_t j = 0; j <5 ; j++)
     {   
       dx= - lside/2 + (lside/6)  + j * (lside/6); 
       dy=0.;
       dz=0.;
       box1->AddNode(barContainer, j+1,new TGeoCombiTrans(dx, dy, dz, new TGeoRotation (0)) ); 
     }
  for (Int_t j = 1; j <6 ; j++)
     {   
       dx= - lside/2 /*+ (lside/6)*/  + j * (lside/6); 
       dy=0.;
       dz=0.;
       box2->AddNode(barContainer, j+1,new TGeoCombiTrans(dx, dy, dz, new TGeoRotation (0)) ); 
     }



  Double_t r = 12.23; // first lens radius (cm)
  Double_t alpha = TMath::ASin(hthick/r); 
  Double_t a = r - r*TMath::Cos(alpha);
  Double_t b = a + .5; // box dimension

  //Double_t r2 = 2.48; // radius second lens (cm)
  Double_t r2 = 3.8; // radius second lens (cm)

  //std::cin>>r2;
  

  Double_t alpha2 = TMath::ASin(hthick/r2); 
  Double_t a2 = r2 - r2*TMath::Cos(alpha2);
  Double_t b2 = .5 + a2;
 
  Double_t l = 0.5+ 0.2+ 0.5+ a2; // dimension of the box containing both lenses

  // Fused Silica bars
  TGeoBBox* logicBar = new TGeoBBox("logicBar",  ((lside/6)/2)-0.05, hthick, bbox_hlen-l/2-eps);
  TGeoVolume *bar = new TGeoVolume("DrcBar",logicBar, gGeoManager->GetMedium("FusedSil"));
  barContainer->AddNode(bar, 1,new TGeoCombiTrans(0., 0., l/2, new TGeoRotation (0)) );
  AddSensitiveVolume(bar);


  // bar ends at...        *** this number +1mm has to enter ProcessHits ***
  //cout<<" bar ends at "<< -bbox_hlen + bbox_shift + l + eps<<endl;
  

  fSlabEnd = -bbox_hlen + bbox_shift + l + eps; // used in processHits  (why l and not l/2???)
  



  // SOB

  Double_t sob_len     = 30.0;
  Double_t sob_shift   = -bbox_hlen + bbox_shift - sob_len;

  Double_t sob_radius2 = radius+hthick + sob_len*tan(60./180.*pi);
  
  cout<<"sob_shift   = "<<sob_shift<<endl;
  cout<<"sob_radius2 = "<<sob_radius2<<endl;
  


  TGeoPgon* baseSOB = new TGeoPgon("baseSOB",0.0, 360., 16, 3);
  baseSOB->DefineSection(0,      0., radius-hthick,  sob_radius2);
  baseSOB->DefineSection(1,     10., radius-hthick,  sob_radius2);
  baseSOB->DefineSection(2, sob_len, radius-hthick,  radius+hthick);
  TGeoVolume *sob = new TGeoVolume("DrcSob",baseSOB, gGeoManager->GetMedium("DIRCairNoSens"));
  cave->AddNode(sob, 1,new TGeoCombiTrans(0., 0., sob_shift, new TGeoRotation (0)));

  // for visualization
  TGeoPgon* logicSOB = new TGeoPgon("baseSOB",0.0, 360., 16, 3);
  logicSOB->DefineSection(0,     0.1, radius-hthick+eps, sob_radius2-eps);
  logicSOB->DefineSection(1,    10.0, radius-hthick+eps, sob_radius2-eps);
  logicSOB->DefineSection(2, sob_len, radius-hthick+eps, radius+hthick-eps);
  TGeoVolume *lsob = new TGeoVolume("DrcLSob", logicSOB, gGeoManager->GetMedium("Marcol82"));
  sob->AddNode(lsob, 1,new TGeoCombiTrans(0., 0., 0., new TGeoRotation (0)));

  // Photodetector
  TGeoPgon* logicPD = new TGeoPgon("logicPD",0.0, 360., 16, 2);
  logicPD->DefineSection(0, 0.0, radius-hthick, sob_radius2-eps);
  logicPD->DefineSection(1, 0.1, radius-hthick, sob_radius2-eps);
  TGeoVolume *pd = new TGeoVolume("DrcPd", logicPD, gGeoManager->GetMedium("DIRCair"));
  sob->AddNode(pd, 1,new TGeoCombiTrans(0., 0., 0., new TGeoRotation (0)));
  AddSensitiveVolume(pd); 
  


  // Lenses
 
  // Lens 1
  Double_t t = -r +b/2;
  TGeoSphere* logicSphere= new TGeoSphere("S",0.,r, 0. ,180.,0.,360.);
  TGeoBBox* lBox = new TGeoBBox("B", (lside/6)/2-0.05, hthick, b/2.);
  TGeoTranslation *tr1 = new TGeoTranslation("tr1", 0.,0., t);
  tr1->RegisterYourself();
  TGeoCompositeShape *cs = new TGeoCompositeShape("cs","S*B:tr1");
  TGeoVolume *lens1 = new TGeoVolume("DrcLENS1",cs, gGeoManager->GetMedium("FusedSil"));
  barContainer->AddNode(lens1, 1,new TGeoCombiTrans(0., 0., -(bbox_hlen-eps) +r +a2 + 0.5 - a ,new TGeoRotation (0)));


   //Lens 2
  Double_t t2 = -r2 +b2/2;
  
  //TGeoSphere* logicSphere2 = new TGeoSphere("S2", r ,r2, 0. ,180.,0.,360.);
  TGeoSphere* logicSphere2 = new TGeoSphere("S2", r2-a2 ,r2, 0. ,180.,0.,360.);
  TGeoBBox*   lBox2        = new TGeoBBox("B2", (lside/6)/2-0.05, hthick, b2/2.);
  TGeoTranslation *tr2     = new TGeoTranslation("tr2", 0.,0., t2);
  tr2->RegisterYourself();
  TGeoCompositeShape *cs2 = new TGeoCompositeShape("cs2","S2*B2:tr2");
  TGeoVolume *lens2 = new TGeoVolume("DrcLENS2",cs2, gGeoManager->GetMedium("NLAK33A"));
  barContainer->AddNode(lens2, 1,new TGeoCombiTrans(0., 0., -(bbox_hlen-eps)+r2 + 0.2 , new TGeoRotation (0)));
  

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
