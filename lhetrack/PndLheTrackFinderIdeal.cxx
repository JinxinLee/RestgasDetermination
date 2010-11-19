#include "PndLheTrackFinderIdeal.h"

#include "PndDetectorList.h"
#include "PndMCTrack.h"
#include "PndLheCMPoint.h"
#include "PndLheHit.h"

#include "FairMCApplication.h"
#include "FairTask.h"
#include "FairRunAna.h"
#include "FairGeoNode.h"
#include "FairGeoVector.h"
#include "FairGeoMedium.h"
#include "FairRootManager.h"

#include "TObjectTable.h"
#include "TClonesArray.h"
#include "TParticlePDG.h"

ClassImp(PndLheTrackFinderIdeal)

//________________________________________________________________
PndLheTrackFinderIdeal::PndLheTrackFinderIdeal() {
  //---

  fFoundTracks = new TClonesArray("PndLheCandidate");
  fCMHits = new TClonesArray("PndLheCMPoint");
  fBench = new TBenchmark();
  fVertex = NULL;
  fVerbose = kFALSE;
  fCanPersistence = kTRUE;
  fCMPersistence = kFALSE;
}

//_________________________________________________________________
PndLheTrackFinderIdeal::
PndLheTrackFinderIdeal( const char *name, const char *title):FairTask(name) {
  //---

  fFoundTracks = new TClonesArray("PndLheCandidate");
  fCMHits = new TClonesArray("PndLheCMPoint");
  fBench = new TBenchmark();
  fVertex = NULL;
  fVerbose = kFALSE;
  fCanPersistence = kTRUE;
  fCMPersistence = kFALSE;
}

//_________________________________________________________________
PndLheTrackFinderIdeal::~PndLheTrackFinderIdeal() {

  if (fCMTracks) { fCMTracks->Delete(); delete fCMTracks; }
  if (fCMHits) { fCMHits->Delete(); delete fCMHits; }

  FairRootManager *fManager =FairRootManager::Instance();
  fManager->Write();

}

//_________________________________________________________________
void PndLheTrackFinderIdeal::Register() {
  //---
  FairRootManager::
    Instance()->Register("LheCandidate",
  			 "Lhe", fFoundTracks, fCanPersistence);

  FairRootManager::
    Instance()->Register("LheCMPoint",
  			 "Lhe",fCMHits, fCMPersistence);

}

//________________________________________________________________
InitStatus PndLheTrackFinderIdeal::Init() {
  // ---

  FairRootManager *fManager = FairRootManager::Instance();
  
  fLheHits  = (TClonesArray *)fManager->GetObject("LheHit");
  
  // Get MV array
  fMCTrackArray = (TClonesArray*)fManager->GetObject("MCTrack");
  if ( ! fMCTrackArray ) {
    cout << "-W-  PndLheTrackFinderIdeal::Init: "
	 << "No MCTrack array! Needed for MC Truth" << endl;
    return kERROR;
  }

  Register();

  // create TObjArrays
  fCMTracks = new TObjArray(64);

  fVertex = new PndLhePoint(0.0, 0.0, 0.0);
  
  pdg = new TDatabasePDG();
  return kSUCCESS;
}

//_________________________________________________________________
void PndLheTrackFinderIdeal::Exec(Option_t * option) {
  
  if (fBench) {
    fBench->Start("finder");
  }

  Reset();
  
  Int_t n_hits = fLheHits->GetEntriesFast();    // number of hits
  PndLheHit *ghit = NULL;
  
  Int_t good_hits = 0;
  std::map<Int_t, PndLheCandidate*> candlist;
  for (Int_t ih = 0; ih < n_hits; ih++) {
    ghit = (PndLheHit *) fLheHits->At(ih);
    
    TClonesArray &cmhits = *fCMHits;
    PndLheCMPoint *cmhit = new(cmhits[good_hits++]) PndLheCMPoint(ghit);
    cmhit->SetHitNumber(ghit->GetHitNumber());
    cmhit->Setup(fVertex);
    cmhit->SetUsage(kFALSE);

    Int_t trackID = ghit->GetTrackID();
    if (trackID<0) continue;

    PndLheCandidate* cand=candlist[trackID];
    if(cand==NULL){
      cand=new PndLheCandidate();
    }
    cand->AddHit(ghit);
    Int_t tpcHits = cand->GetTpcHits();
    Int_t mvdHits = cand->GetMvdHits();
    Int_t gemHits = cand->GetGemHits();
    if ( (ghit->GetDetectorID() == kTpcPoint)     ||
	 (ghit->GetDetectorID() == kTpcCluster)     )   tpcHits++;
    if ( (ghit->GetDetectorID() == kSttPoint)     ||
	 (ghit->GetDetectorID() == kSttHit)       ||
         (ghit->GetDetectorID() == kSttHelixHit)  )   tpcHits++; // for the moment
    if ( (ghit->GetDetectorID() == kMVDPoint)     || 
	 (ghit->GetDetectorID() == kMVDHitsStrip) ||
	 (ghit->GetDetectorID() == kMVDHitsPixel)   )   mvdHits++;
    if ( (ghit->GetDetectorID() == kGemPoint)     ||
	 (ghit->GetDetectorID() == kGemHit)         )   gemHits++;
    
    cand->SetTpcHits(tpcHits);
    cand->SetMvdHits(mvdHits); 
    cand->SetGemHits(gemHits);
    
    PndMCTrack *mc = (PndMCTrack*)fMCTrackArray->At(trackID);
    Int_t charge;
    if (mc->GetPdgCode()<100000000) charge = (Int_t)TMath::Sign(1.0, ((TParticlePDG*)pdg->GetParticle(mc->GetPdgCode()))->Charge());
    else charge = 1;
    cand->SetCharge(charge);
    candlist[trackID] = cand;
  }
  
  std::map<Int_t, PndLheCandidate*>::iterator  candit;
  for(candit=candlist.begin(); candit!=candlist.end(); ++candit) {
    PndLheCandidate* cand=candit->second; 
    if ( (cand->GetTpcHits()+ cand->GetMvdHits()) >=3 )
      AddTrack(cand);
  }
  
  if(fBench) {
    cout << endl;
    fBench->Show("finder");
  }
  
}

//_________________________________________________________________
void PndLheTrackFinderIdeal::Finish() {

  cout << " Found  "<< fFoundTracks->GetEntriesFast() << " tracks\n";
  
}

//________________________________________________________________
void PndLheTrackFinderIdeal::Reset() {
  //---

  if (fCMTracks->GetEntriesFast() != 0)  fCMTracks->Clear("C");
  if (fCMHits->GetEntriesFast() != 0)  fCMHits->Clear("C");
  if (fFoundTracks->GetEntriesFast() != 0)  fFoundTracks->Clear("C");

}

//_________________________________________________________________
PndLheCandidate* PndLheTrackFinderIdeal::AddTrack(PndLheCandidate* track) {
  // Creates a new hit in the TClonesArray.
  
  TClonesArray& trkRef = *fFoundTracks;
  Int_t size = trkRef.GetEntriesFast();
  return new(trkRef[size]) PndLheCandidate(*track);
}
