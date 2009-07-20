// -------------------------------------------------------------------------
// -----                   PndDchFindTracks source file                -----
// -----                  Created 27/01/08  by A. Bubak               -----
// -----                 according to the CbmStsFindTracks             -----
// -------------------------------------------------------------------------


// Pnd includes
#include "PndDchConvertTrackCandTask.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"

// ROOT includes
#include "TClonesArray.h"

#include<iostream>
#include"assert.h"

#include"PndMCTrack.h"
#include"PndTrackCand.h"
#include"PndDetectorList.h"
#include"PndDchTrack.h"
#include"PndDchTrackMatch.h"
#include"PndDchCylinderHit.h"

// -----   Default constructor   -------------------------------------------
PndDchConvertTrackCandTask::PndDchConvertTrackCandTask() {
  fCylHitArr = NULL;
  fMCTrackArr = NULL;
  fDchTrackArr = NULL;
  fDchTrackMatchArr = NULL;
  fPndTrackCandArr = NULL;
}
// -------------------------------------------------------------------------



// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndDchConvertTrackCandTask::~PndDchConvertTrackCandTask() {
  fCylHitArr->Delete();
  fMCTrackArr->Delete();
  fDchTrackArr->Delete();
  fDchTrackMatchArr->Delete();
  fPndTrackCandArr->Delete();
  if(fCylHitArr != NULL) delete fCylHitArr;
  if(fMCTrackArr != NULL) delete fMCTrackArr;
  if(fDchTrackArr != NULL) delete fDchTrackArr;
  if(fDchTrackMatchArr != NULL) delete fDchTrackMatchArr;
  if(fPndTrackCandArr != NULL) delete fPndTrackCandArr;
}
// -------------------------------------------------------------------------



// -----   Public method Init (abstract in base class)  --------------------
InitStatus PndDchConvertTrackCandTask::Init() {

	// Get and check FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if( !ioman ) {
    std::cout << "-E- PndDchConvertTrackCandTask::Init: "
	      << "RootManager not instantised!" << std::endl;
    return kFATAL;
  }

  fCylHitArr  = (TClonesArray*) ioman->GetObject("PndDchCylinderHit");
  if ( !fCylHitArr ) {
    std::cout << "-W- PndGemIdealPatternRecoTask::Init: No PndDchCylinder array!" << std::endl;
    return kERROR;
  }

  fMCTrackArr  = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( !fMCTrackArr ) {
    std::cout << "-W- PndGemIdealPatternRecoTask::Init: No MCTrack array!" << std::endl;
    return kERROR;
  }

  fDchTrackArr  = (TClonesArray*) ioman->GetObject("PndDchTrack");
  if ( !fDchTrackArr ) {
    std::cout << "-W- PndDchConvertTrackCandTask::Init: No PndDchTrack array!" << std::endl;
    return kERROR;
  }
  fDchTrackMatchArr  = (TClonesArray*) ioman->GetObject("PndDchTrackMatch");
  if ( !fDchTrackMatchArr ) {
    std::cout << "-W- PndDchConvertTrackCandTask::Init: No PndDchMatchTrack array!" << std::endl;
    return kERROR;
  }

  // Create and register DchTrack array
  fPndTrackCandArr = new TClonesArray("PndTrackCand",100);
  ioman->Register("DchPndTrackCand", "Dch", fPndTrackCandArr  , kTRUE);
  
  return kSUCCESS;  
  
}
// -------------------------------------------------------------------------



// -----  SetParContainers -------------------------------------------------
void PndDchConvertTrackCandTask::SetParContainers() {
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb = ana->GetRuntimeDb();
  rtdb->getContainer("FairBaseParSet");
  rtdb->getContainer("PndGeoPassivePar");
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndDchConvertTrackCandTask::Exec(Option_t* opt) {
  fPndTrackCandArr->Delete();
  int nTrack = fDchTrackArr->GetEntriesFast();
  int nTrackMatch = fDchTrackMatchArr->GetEntriesFast();
  for(unsigned int itrm=0;itrm<nTrackMatch;++itrm){
    PndDchTrackMatch *aMatch = (PndDchTrackMatch*)fDchTrackMatchArr->At(itrm);
    int trackIndex = aMatch->GetRecTrackID();
    assert(trackIndex<nTrack);
    PndTrackCand cand;
    PndDchTrack *theTrack = (PndDchTrack*)fDchTrackArr->At(trackIndex);
    //theTrack->Print();
    if(aMatch->GetMCTrackID()>=0){
      for(unsigned int ihit=0;ihit<theTrack->GetNofDchCylinderHits();++ihit){    
	PndDchCylinderHit *hit = (PndDchCylinderHit*)fCylHitArr->At(theTrack->GetDchCylinderHitIndex(ihit));
	cand.AddHit(kDCH,theTrack->GetDchCylinderHitIndex(ihit),hit->GetWireZcoordGlobal());      
      }
      cand.setMcTrackId(aMatch->GetMCTrackID());
      PndMCTrack *myMCTrack = (PndMCTrack*)fMCTrackArr->At(aMatch->GetMCTrackID());
      int pdg = myMCTrack->GetPdgCode();
      double charge;
      if(pdg<100000000){
	charge =  TDatabasePDG::Instance()->GetParticle(pdg)->Charge()/3.;
      }
      else{
	charge = 0.;
      }
      cand.setTrackSeed(
			myMCTrack->GetStartVertex(),
			myMCTrack->GetMomentum(),
			charge/myMCTrack->GetMomentum().Mag()
			);
      int nentries = fPndTrackCandArr->GetEntriesFast(); 
      
      if(cand.GetNHits()>0) new ((*fPndTrackCandArr)[nentries]) PndTrackCand(cand);
    }
  }
}
// -------------------------------------------------------------------------





ClassImp(PndDchConvertTrackCandTask)
