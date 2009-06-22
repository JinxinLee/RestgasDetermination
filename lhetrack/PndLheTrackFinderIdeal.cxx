#include "PndLheTrackFinderIdeal.h"

#include "PndDetectorList.h"
#include "PndTpcLheCMTrack.h"
#include "PndTpcLheCMPoint.h"
#include "PndLheHit.h"
#include "lhe.h"

#include "FairMCApplication.h"
#include "FairTask.h"
#include "FairRunAna.h"
#include "FairGeoNode.h"
#include "FairGeoVector.h"
#include "FairGeoMedium.h"
#include "FairRootManager.h"

#include "TObjectTable.h"
#include "TClonesArray.h"

ClassImp(PndLheTrackFinderIdeal)

//________________________________________________________________
PndLheTrackFinderIdeal::PndLheTrackFinderIdeal() {
  //---

  fFoundTracks = new TClonesArray("PndTpcLheTrack");
  fCMHits = new TClonesArray("PndTpcLheCMPoint");
  fBench = new TBenchmark();
  fVertex = NULL;
  fVerbose = kFALSE;
}

//_________________________________________________________________
PndLheTrackFinderIdeal::
PndLheTrackFinderIdeal( const char *name, const char *title):FairTask(name) {
  //---

  fFoundTracks = new TClonesArray("PndTpcLheTrack");
  fCMHits = new TClonesArray("PndTpcLheCMPoint");
  fBench = new TBenchmark();
  fVertex = NULL;
  fVerbose = kFALSE;

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
    Instance()->Register("PndTpcLheTrack",
  			 "Lhe", fFoundTracks, kTRUE);

  FairRootManager::
    Instance()->Register("PndTpcLheCMPoint",
  			 "Lhe",fCMHits, kTRUE);

}

//________________________________________________________________
InitStatus PndLheTrackFinderIdeal::Init() {
  // ---

  FairRootManager *fManager = FairRootManager::Instance();
  
  fLheHits  = (TClonesArray *)fManager->GetObject("LheHit");
  
  Register();

  // create TObjArrays
  fCMTracks = new TObjArray(64);

  fVertex = new PndTpcLhePoint(0.0, 0.0, 0.0);
  
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
  std::map<Int_t, PndTpcLheTrack*> candlist;
  for (Int_t ih = 0; ih < n_hits; ih++) {
    ghit = (PndLheHit *) fLheHits->At(ih);
    
    TClonesArray &cmhits = *fCMHits;
    PndTpcLheCMPoint *cmhit = new(cmhits[good_hits++]) PndTpcLheCMPoint(ghit);
    cmhit->SetHitNumber(ghit->GetHitNumber());
    cmhit->Setup(fVertex);
    cmhit->SetUsage(kFALSE);

    Int_t trackID = ghit->GetTrackID();
    if (trackID==-1) continue;

    PndTpcLheTrack* cand=candlist[trackID];
    if(cand==NULL){
      cand=new PndTpcLheTrack();
    }
    cand->AddHit(ghit);
    Int_t tpcHits = cand->GetTpcHits();
    Int_t mvdHits = cand->GetMvdHits();
    Int_t gemHits = cand->GetGemHits();
    if ( (ghit->GetDetectorID() == kTpcPoint)     ||
	 (ghit->GetDetectorID() == kTpcCluster)     )   tpcHits++;
    if ( (ghit->GetDetectorID() == kSttPoint)     ||
	 (ghit->GetDetectorID() == kSttHit)         )   tpcHits++; // for the moment
    if ( (ghit->GetDetectorID() == kMVDPoint)     || 
	 (ghit->GetDetectorID() == kMVDHitsStrip) ||
	 (ghit->GetDetectorID() == kMVDHitsPixel)   )   mvdHits++;
    if ( (ghit->GetDetectorID() == kGemPoint)     ||
	 (ghit->GetDetectorID() == kGemHit)         )   gemHits++;
    
    cand->SetTpcHits(tpcHits);
    cand->SetMvdHits(mvdHits); 
    cand->SetGemHits(gemHits); 
    candlist[trackID] = cand;
  }
  
  std::map<Int_t, PndTpcLheTrack*>::iterator  candit;
  for(candit=candlist.begin(); candit!=candlist.end(); ++candit) {
    PndTpcLheTrack* cand=candit->second; 
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
PndTpcLheTrack* PndLheTrackFinderIdeal::AddTrack(PndTpcLheTrack* track) {
  // Creates a new hit in the TClonesArray.
  
  TClonesArray& trkRef = *fFoundTracks;
  Int_t size = trkRef.GetEntriesFast();
  return new(trkRef[size]) PndTpcLheTrack(*track);
}
