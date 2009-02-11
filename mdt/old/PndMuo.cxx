// -------------------------------------------------------------------------
#include "PndMuo.h"

#include "PndMuoPoint.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "PndGeoMuo.h"
#include "FairGeoRootBuilder.h"
#include "PndStack.h"
#include "FairRootManager.h"
#include "FairVolume.h"
#include "FairGeoG3Builder.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"

#include "TClonesArray.h"
#include "TGeoMCGeometry.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TObjArray.h"

#include <iostream>

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
PndMuo::PndMuo() {
  fMuoCollection        = new TClonesArray("PndMuoPoint");
  fPosIndex   = 0;
  nFlag = 0;
  nSec = -1;
  
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
PndMuo::PndMuo(const char* name, Bool_t active)
  : FairDetector(name, active) {
    fMuoCollection        = new TClonesArray("PndMuoPoint");
    fPosIndex   = 0;
    nFlag = 0;
    nSec = -1;
    fEventID=-1;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndMuo::~PndMuo() {
  if (fMuoCollection) {
    fMuoCollection->Delete();
    delete fMuoCollection;
  }
  
}
// -------------------------------------------------------------------------



// -----   Public method Intialize   ---------------------------------------
void PndMuo::Initialize() {
  
  // controlla-- forse non tutto e' necessario.
  FairDetector::Initialize();
  FairRun* sim = FairRun::Instance();
  FairRuntimeDb* rtdb=sim->GetRuntimeDb();
  par=(PndGeoMuoPar*)(rtdb->getContainer("PndGeoMuoPar"));
  
  // TObjArray *fSensNodes = par->GetGeoSensitiveNodes();

}
// -------------------------------------------------------------------------
void PndMuo::BeginEvent(){
  //cout<<">>>>>>>>>>>>>>>>>>>>inizio evento "<<endl;
  //fevent++;
}



// -----   Public method ProcessHits  --------------------------------------
Bool_t PndMuo::ProcessHits(FairVolume* vol) {
  
  fTrackID  = gMC->GetStack()->GetCurrentTrackNumber(); // trk ID
  fEventID = gMC->CurrentEvent();
  //  cout<<"nuova traccia"<<fTrackID<<endl;

  nFlag = 0;
  // if entering
  if ( gMC->IsTrackEntering() ) {
    nFlag = -1;
  }
  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) {
    nFlag = 1;
  } 
  
  gMC->TrackPosition(fPos); // cm
  gMC->TrackMomentum(fMom); // GeV
  
  TString nam = gMC->CurrentVolName();
  TVector3 pos(fPos.X(),   fPos.Y(),   fPos.Z());
  
  Int_t nPlane = -1, nBox = -1, nTube = -1;
  
  nSec = nam[4]-49;
  gMC->CurrentVolOffID(3, nPlane); nPlane--;
  gMC->CurrentVolOffID(2, nBox); nBox--;
  gMC->CurrentVolOffID(0, nTube); nTube--;
  
  // Set additional parameters at exit of active volume. Create PndMuoPoint.
  if (nFlag==1) 
    AddHit(fTrackID, fVolumeID, fEventID,
	   TVector3(fPos.X(),   fPos.Y(),   fPos.Z()),
	   TVector3(fMom.Px(),  fMom.Py(),  fMom.Pz()),
	   fTime, fLength, fELoss, nFlag, nSec, (Short_t)nPlane, (Short_t)nBox, (Short_t)nTube);
  ResetParameters();
  
  return kTRUE;
  
}
// ----------------------------------------------------------------------------



// -----   Public method EndOfEvent   -----------------------------------------
void PndMuo::EndOfEvent() {
  if (fVerboseLevel)  Print();
  Reset();
}
// ----------------------------------------------------------------------------



// -----   Public method Register   -------------------------------------------
void PndMuo::Register() {
  FairRootManager::Instance()->Register("MuoPoint","Muo", fMuoCollection, kTRUE);
}
// ----------------------------------------------------------------------------



// -----   Public method GetCollection   --------------------------------------
TClonesArray* PndMuo::GetCollection(Int_t iColl) const {
   if (iColl == 0) return fMuoCollection;

  return NULL;
}
// ----------------------------------------------------------------------------




// -----   Public method Print   ----------------------------------------------
void PndMuo::Print() const {
    Int_t nHits = fMuoCollection->GetEntriesFast();
    cout << "-I- PndMuo: " << nHits << " points registered in this event."
 	<< endl;

    if (fVerboseLevel>1)
      for (Int_t i=0; i<nHits; i++) (*fMuoCollection)[i]->Print();
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void PndMuo::Reset() {
   fMuoCollection->Clear();
 
  fPosIndex = 0;
}
// ----------------------------------------------------------------------------


// guarda in FairRootManager::CopyClones
// -----   Public method CopyClones   -----------------------------------------
void PndMuo::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset ) {
  Int_t nEntries = cl1->GetEntriesFast();
  cout << "-I- PndMuo: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  PndMuoPoint* oldpoint = NULL;
  for (Int_t i=0; i<nEntries; i++) {
    oldpoint = (PndMuoPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) PndMuoPoint(*oldpoint);
    fPosIndex++;
  }
  cout << " -I- PndMuo: " << cl2->GetEntriesFast() << " merged entries."
       << endl;
}
// ----------------------------------------------------------------------------



// -----   Public method ConstructGeometry   ----------------------------------
void PndMuo::ConstructGeometry() {

  FairGeoLoader*    muogeoLoad = FairGeoLoader::Instance();
  FairGeoInterface* muogeoFace = muogeoLoad->getGeoInterface();
  PndGeoMuo*      muoGeo = new PndGeoMuo();
  muoGeo->setGeomFile(GetGeometryFileName());
  muogeoFace->addGeoModule(muoGeo);

  Bool_t rc = muogeoFace->readSet(muoGeo);
  if (rc) muoGeo->create(muogeoLoad->getGeoBuilder());
  TList* volList = muoGeo->getListOfVolumes();

  // store geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  PndGeoMuoPar* par=(PndGeoMuoPar*)(rtdb->getContainer("PndGeoMuoPar"));
  TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
  TObjArray *fPassNodes = par->GetGeoPassiveNodes();

  TListIter iter(volList);
  FairGeoNode* node   = NULL;
  FairGeoVolume *aVol=NULL;

  while( (node = (FairGeoNode*)iter.Next()) ) {
    aVol = dynamic_cast<FairGeoVolume*> ( node );
    if ( node->isSensitive()  ) {
      fSensNodes->AddLast( aVol );
    }
    else{
      fPassNodes->AddLast( aVol );
    }
  }
  
  ProcessNodes ( volList );
  par->setChanged();
  par->setInputVersion(fRun->GetRunId(),1);

}
// ----------------------------------------------------------------------------
/*
Short_t PndMuo::GetPlane(TVector3 pos) {
  MuoTransform(pos);
  return (Short_t)((pos.Y()*10.-par->barrel_y)/par->dplane);
}
Short_t PndMuo::GetBox(TVector3 pos)   {
  MuoTransform(pos);
  return (Short_t)((pos.X()*10.+(par->nbox*par->dbox)*0.5)/par->dbox);
}
Short_t PndMuo::GetTube(TVector3 pos)  {
  MuoTransform(pos);
  return (Short_t)((pos.X()*10.+(par->nbox*par->dbox)*0.5-par->dbox*((Short_t)((pos.X()*10.+(par->nbox*par->dbox)*0.5)/par->dbox)))/par->dtube);
}
*/
Short_t PndMuo::GetStrip(TVector3 pos) {
  MuoTransform(pos);
  return (Short_t)((pos.Z()*10.-par->barrel_z)/par->dstrip);
}
void PndMuo::MuoTransform(TVector3 &pos) {
  Double_t sec_x = pos.X();
  Double_t sec_y = pos.Y();
  pos.SetX(cos(TMath::Pi()/4.*nSec)*sec_x +sin(TMath::Pi()/4.*nSec)*sec_y);
  pos.SetY(-sin(TMath::Pi()/4.*nSec)*sec_x +cos(TMath::Pi()/4.*nSec)*sec_y);
}

// -----   Private method AddHit   --------------------------------------------
PndMuoPoint* PndMuo::AddHit(Int_t trackID, Int_t detID, Int_t evtID, TVector3 pos, TVector3 mom, Double_t time, Double_t length, Double_t eLoss, Short_t flag, Short_t sec, Short_t plane, Short_t box, Short_t tube) {
  TClonesArray& clref = *fMuoCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel>1) 
    cout << "-I- PndMuo: Adding Point at IN (" << pos.X() << ", " << pos.Y() 
	 << ", " << pos.Z() << ") cm, detector " << detID << ", evt " << evtID << ",flag "<< flag << ", track "
	 << trackID <<", energy loss " << eLoss*1e06 << " keV, sec " << sec << " plane " << plane << " strip " <<  GetStrip(pos) << " box " << box << " tube " << tube << endl;
  
  return new(clref[size]) PndMuoPoint(trackID, detID, evtID, pos, mom, time, length, eLoss, flag,
				      sec, plane, GetStrip(pos), box, tube);
}

inline void PndMuo::ResetParameters() {
  fTrackID = -999;
  fVolumeID = -999;
  fEventID = -999;
  fPos.SetXYZT(0., 0., 0., 0.);
  fMom.SetXYZT(0., 0., 0., 0.) ;
  fTime = -999;
  fLength = -999;
  fELoss = -999;
  nFlag = 0;
  nSec = -1;
}

// ----




ClassImp(PndMuo)
