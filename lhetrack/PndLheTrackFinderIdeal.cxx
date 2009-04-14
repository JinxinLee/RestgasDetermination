#include "PndLheTrackFinderIdeal.h"

#include "PndTpcLheCMTrack.h"
#include "PndTpcLheCMPoint.h"
#include "PndTpcLheHit.h"
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
  PndTpcLheHit *ghit = NULL;
  
  Int_t good_hits = 0;
  std::map<Int_t, PndTpcLheTrack*> candlist;
  for (Int_t ih = 0; ih < n_hits; ih++) {
    ghit = (PndTpcLheHit *) fLheHits->At(ih);
    
    TClonesArray &cmhits = *fCMHits;
    PndTpcLheCMPoint *cmhit = new(cmhits[good_hits++]) PndTpcLheCMPoint(ghit);
    cmhit->SetHitNumber(ghit->GetHitNumber());
    cmhit->Setup(fVertex);
    cmhit->SetUsage(kFALSE);

    Int_t trackID = ghit->GetTrackID();
    
    PndTpcLheTrack* cand=candlist[trackID];
    if(cand==NULL){
      cand=new PndTpcLheTrack();
    }
    cand->AddHit(ghit);
    Int_t tpcHits = cand->GetTpcHits();
    Int_t mvdHits = cand->GetMvdHits();
    
    if ( (ghit->GetDetectorId() == kTpcPoint)     ||
	 (ghit->GetDetectorId() == kTpcCluster)     )   tpcHits++;
    if ( (ghit->GetDetectorId() == kSttPoint)     ||
	 (ghit->GetDetectorId() == kSttHit)         )   tpcHits++; // for the moment
    if ( (ghit->GetDetectorId() == kMVDPoint)     || 
	 (ghit->GetDetectorId() == kMVDHitsStrip) ||
	 (ghit->GetDetectorId() == kMVDHitsPixel)   )   mvdHits++;
    
    cand->SetTpcHits(tpcHits);
    cand->SetMvdHits(mvdHits);
  }
 
  std::map<Int_t, PndTpcLheTrack*>::iterator  candit=candlist.begin();
  while(candit!=candlist.end()){
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
