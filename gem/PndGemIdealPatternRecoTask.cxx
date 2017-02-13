// -------------------------------------------------------------------------
// -----                   PndGemIdealPatternRecoTask source file      -----
// -----                  Based on PndDchFindTracks 27/01/08  by A. Bubak               -----
// -----                 according to the CbmStsFindTracks             -----
// -------------------------------------------------------------------------


// Pnd includes
#include "PndGemIdealPatternRecoTask.h"

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
#include"PndGemHit.h"
#include"PndGemMCPoint.h"

// -----   Default constructor   -------------------------------------------
PndGemIdealPatternRecoTask::PndGemIdealPatternRecoTask() {
  fMCTrackArr = NULL;
  fGemHitArr = NULL;
  fGemPointArr = NULL;
  fPndTrackCandArr = NULL;
}
// -------------------------------------------------------------------------



// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndGemIdealPatternRecoTask::~PndGemIdealPatternRecoTask() {
  fMCTrackArr->Delete();
  fGemHitArr->Delete();
  fGemPointArr->Delete();
  fPndTrackCandArr->Delete();
  if(fMCTrackArr != NULL) delete fMCTrackArr;
  if(fGemHitArr != NULL) delete fGemHitArr;
  if(fGemPointArr != NULL) delete fGemPointArr;
  if(fPndTrackCandArr != NULL) delete fPndTrackCandArr;
}
// -------------------------------------------------------------------------



// -----   Public method Init (abstract in base class)  --------------------
InitStatus PndGemIdealPatternRecoTask::Init() {

	// Get and check FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if( !ioman ) {
    std::cout << "-E- PndGemIdealPatternRecoTask::Init: "
	      << "RootManager not instantised!" << std::endl;
    return kFATAL;
  }

  fMCTrackArr  = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( !fMCTrackArr ) {
    std::cout << "-W- PndGemIdealPatternRecoTask::Init: No MCTrack array!" << std::endl;
    return kERROR;
  }

  fGemPointArr  = (TClonesArray*) ioman->GetObject("GEMPoint");
  if ( !fGemPointArr ) {
    std::cout << "-W- PndGemIdealPatternRecoTask::Init: No PndGemPoint array!" << std::endl;
    return kERROR;
  }
  fGemHitArr  = (TClonesArray*) ioman->GetObject("GEMHit");
  if ( !fGemHitArr ) {
    std::cout << "-W- PndGemIdealPatternRecoTask::Init: No PndGemHit array!" << std::endl;
    return kERROR;
  }

  // Create and register GemTrack array
  fPndTrackCandArr = new TClonesArray("PndTrackCand",100);
  ioman->Register("GemPndTrackCand", "Gem", fPndTrackCandArr  , kTRUE);
  
  std::cout << "PndGemIdealPatternRecoTask::Init() finished" << std::endl;
  return kSUCCESS;  
  
}
// -------------------------------------------------------------------------



// -----  SetParContainers -------------------------------------------------
void PndGemIdealPatternRecoTask::SetParContainers() {
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb = ana->GetRuntimeDb();
  rtdb->getContainer("FairBaseParSet");
  rtdb->getContainer("PndGeoPassivePar");
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndGemIdealPatternRecoTask::Exec(Option_t* opt) {
  fPndTrackCandArr->Delete();
 
  std::map<int,PndTrackCand*> cands;

  int nhits = fGemHitArr->GetEntriesFast();
  int npoints = fGemPointArr->GetEntriesFast();
  for(int ihit=0;ihit<nhits;++ihit){
    PndGemHit* gemHit = (PndGemHit*)fGemHitArr->At(ihit);
    FairHit * hit = gemHit;
    int index = hit->GetRefIndex();
    assert(index<npoints);
    PndGemMCPoint *gemPoint = (PndGemMCPoint*)fGemPointArr->At(index);
    FairMCPoint *point = gemPoint;
    int trackId = point->GetTrackID();
    if(cands[trackId]==NULL) {
      cands[trackId] = new PndTrackCand;
      //      cands[trackId]->setMcTrackId(trackId);
      PndMCTrack *myMCTrack = (PndMCTrack*)fMCTrackArr->At(trackId);
      //int pdg = myMCTrack->GetPdgCode(); //[R.K.02/2017] Unused variable?
      //double charge; //[R.K. 01/2017] unused variable?
      //if(pdg<100000000){ //[R.K. 01/2017] unused variable?
	//charge =  TDatabasePDG::Instance()->GetParticle(pdg)->Charge()/3.; //[R.K. 01/2017] unused variable?
      //} //[R.K. 01/2017] unused variable?
      //else{ //[R.K. 01/2017] unused variable?
	//charge = 0.; //[R.K. 01/2017] unused variable?
      //} //[R.K. 01/2017] unused variable?

    }
    cands[trackId]->AddHit(FairRootManager::Instance()->GetBranchId("GEMHit"),ihit,gemHit->GetPosition().Mag());
  }

  for(std::map<int,PndTrackCand*>::iterator iter=cands.begin();
      iter!=cands.end();++iter){
    int nentries = fPndTrackCandArr->GetEntriesFast();
    if(iter->second->GetNHits()>0) new ((*fPndTrackCandArr)[nentries]) PndTrackCand(*(iter->second));
    delete iter->second;
  }
}
// -------------------------------------------------------------------------





ClassImp(PndGemIdealPatternRecoTask)
