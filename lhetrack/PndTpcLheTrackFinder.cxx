#include "PndTpcLheTrackFinder.h"
#include "PndDetectorList.h"
#include "PndLheCMCandidate.h"
#include "PndLheCMPoint.h"
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

ClassImp(PndTpcLheTrackFinder)

//________________________________________________________________
PndTpcLheTrackFinder::PndTpcLheTrackFinder() {
  //---

  fFoundTracks = new TClonesArray("PndLheCandidate");
  fCMHits = new TClonesArray("PndLheCMPoint");
  fBench = new TBenchmark();
  fVertex = NULL;
  fVerbose = kFALSE;
}

//_________________________________________________________________
PndTpcLheTrackFinder::
PndTpcLheTrackFinder( const char *name, const char *title):FairTask(name) {
  //---

  fFoundTracks = new TClonesArray("PndLheCandidate");
  fCMHits = new TClonesArray("PndLheCMPoint");
  fBench = new TBenchmark();
  fVertex = NULL;
  fVerbose = kFALSE;

}

//_________________________________________________________________
PndTpcLheTrackFinder::~PndTpcLheTrackFinder() {

  if (fCMTracks) { fCMTracks->Delete(); delete fCMTracks; }
  if (fCMHits) { fCMHits->Delete(); delete fCMHits; }

  if (fSegments) delete fSegments;

  FairRootManager *fManager =FairRootManager::Instance();
  fManager->Write();

}

//_________________________________________________________________
void PndTpcLheTrackFinder::Register() {
  //---
  FairRootManager::
    Instance()->Register("LheCandidate",
  			 "Lhe", fFoundTracks, kTRUE);

  FairRootManager::
    Instance()->Register("LheCMPoint",
  			 "Lhe",fCMHits, kTRUE);

}

//________________________________________________________________
InitStatus PndTpcLheTrackFinder::Init() {
  // ---

  FairRootManager *fManager = FairRootManager::Instance();
  fTrackCuts = PndTpcLheTrackCuts::Instance();

  if(fOption.Contains("geant")) {
    fGeantTracks = (TClonesArray *)fManager->GetObject("LheGeantTrack");
  }
  else {
    fLheHits  = (TClonesArray *)fManager->GetObject("LheHit");
  }

  Register();

  // create TObjArrays
  fCMTracks = new TObjArray(64);

  fVertex = new PndLhePoint(0.0, 0.0, 0.0);

  fSegment = new PndTpcLheSegments();
  fSegment->Init();

  fSegments = fSegment->GetSegments();

  return kSUCCESS;
}

//_________________________________________________________________
void PndTpcLheTrackFinder::Exec(Option_t * option) {
    
  if(fOption.Contains("geant")) {
    cout << "  Copy Geant tracks  \n" << endl;
    CopyClones(fGeantTracks, fFoundTracks);
    return;
  }


  if (fBench) {
    fBench->Start("finder");
  }

  Reset();
  
  Int_t n_hits = fLheHits->GetEntriesFast();    // number of hits

  PndLheHit *ghit = NULL;

  Int_t good_hits = 0;

  for (Int_t ih = 0; ih < n_hits; ih++) {
    ghit = (PndLheHit *) fLheHits->UncheckedAt(ih);
    TClonesArray &cmhits = *fCMHits;
    PndLheCMPoint *cmhit = new(cmhits[good_hits++]) PndLheCMPoint(ghit);
    cmhit->SetHitNumber(ghit->GetHitNumber());
    cmhit->Setup(fVertex);
    cmhit->SetUsage(kFALSE);
  }

  cout << " Working with " << good_hits << " hits\n";

  fSegment->FillSegments(fCMHits);

  DoTracking();

  if(fBench) {
    cout << endl;
    fBench->Show("finder");
  }

}

//________________________________________________________________
void PndTpcLheTrackFinder::AddTrackForFit(PndLheCMCandidate *track_in) {
  //--- Add a new track to the list of tracks for this event.

  TClonesArray &tracks = *fFoundTracks;
  Int_t size = tracks.GetEntriesFast();

  TObjArray *rhits = (TObjArray* )track_in->GetCMHits();
  if (fVerbose) cout << "PndTpcLheTrackFinder::AddTrackForFit "<<  rhits << endl;
  Int_t NHits = 0;
  if (rhits){ 
    NHits = rhits->GetEntriesFast();

    PndLheCandidate* track = new(tracks[size]) PndLheCandidate(size);
    track->SetCharge(track_in->GetCharge());
 
    Int_t mvdHits = 0;
    Int_t tpcHits = 0; 
    Int_t gemHits = 0;
    for (Int_t lh = 0; lh < NHits; lh++) {
      PndLheHit* hit = dynamic_cast <PndLheHit*> (rhits->At(lh));
      if (NULL==hit) break; 
      track->AddHit(hit);
      if ( (hit->GetDetectorID() == kTpcPoint)     ||
	   (hit->GetDetectorID() == kTpcCluster)     )   tpcHits++;
      if ( (hit->GetDetectorID() == kSttPoint)     ||
	   (hit->GetDetectorID() == kSttHit)         )   tpcHits++; // for the moment
      if ( (hit->GetDetectorID() == kMVDPoint)     || 
	   (hit->GetDetectorID() == kMVDHitsStrip) ||
	   (hit->GetDetectorID() == kMVDHitsPixel)   )   mvdHits++;
      if ( (hit->GetDetectorID() == kGemPoint)     ||
	   (hit->GetDetectorID() == kGemHit)         )   gemHits++;
    }
    track->SortHits();
    track->SetTpcHits(tpcHits);
    track->SetMvdHits(mvdHits);
    track->SetGemHits(gemHits);
  }
}

//_________________________________________________________________
void PndTpcLheTrackFinder::Finish() {

  cout << " found  "<< fFoundTracks->GetEntriesFast() << " tracks\n";

  //FairRootManager *fManager =FairRootManager::Instance(); 
  //fManager->Fill();

}

//________________________________________________________________
void PndTpcLheTrackFinder::Reset() {
  //---

  if (fCMTracks->GetEntriesFast() != 0)  fCMTracks->Clear("C");
  if (fCMHits->GetEntriesFast() != 0)  fCMHits->Clear("C");
  if (fFoundTracks->GetEntriesFast() != 0)  fFoundTracks->Clear("C");

}

//________________________________________________________________
void PndTpcLheTrackFinder::DoTracking() {
  //--- Tracking

  for (Int_t im = HIGH; im <= HIGH; im++ ) {

    switch (im) {
      
    case HIGH:
      fTrackCuts->SetHighMomTrackCuts();
      break;

    case LOW:
      fTrackCuts->SetLowMomTrackCuts();
      break;
      
    default:
      fTrackCuts->SetHighMomTrackCuts();
    
    }

    LoopOverHits();

    TrackingInfo(" ");

  }
  
}

//________________________________________________________________
void PndTpcLheTrackFinder::CheckClones() {
  //---

  TIter next(fCMTracks);  next.Reset();
  PndLheCMCandidate* trk;
  Int_t ntracks = fCMTracks->GetEntriesFast();

  next.Reset();
  while((trk = (PndLheCMCandidate*) next())) {
    trk->SetGood(kTRUE);
  }

  for (Int_t it = 0;it < ntracks; it++) {
    trk = (PndLheCMCandidate*)fCMTracks->At(it);

    PndLhePoint hel_ex = trk->GetCircle();
    
    Double_t xc_ex = hel_ex.GetX();
    Double_t yc_ex = hel_ex.GetY();
    Double_t rad_ex = hel_ex.GetZ();

    for (Int_t jt = it + 1; jt < ntracks; jt++) {
      PndLheCMCandidate* trk_in = (PndLheCMCandidate*) fCMTracks->At(jt);
      PndLhePoint hel_in = trk_in->GetCircle();
    
      Double_t xc_in = hel_in.GetX();
      Double_t yc_in = hel_in.GetY();
      Double_t rad_in = hel_in.GetZ();
      if (TMath::Abs(xc_ex - xc_in) < 1. &&
	  TMath::Abs(yc_ex - yc_in) < 1. &&
	  TMath::Abs(rad_ex - rad_in) < 1.)
	trk_in->SetGood(kFALSE);
      //      cout << "\n Clones\n"; trk->Print(); trk_in->Print();
    }

  }
  
  next.Reset();
  while((trk = (PndLheCMCandidate*) next())) {
    if (trk->IsGood()) AddTrackForFit(trk);
  }
  
}

//________________________________________________________________
Bool_t PndTpcLheTrackFinder::TrackExtension(PndLheCMCandidate *track) {

  PndLheCMPoint *hit =  (PndLheCMPoint *)(track->GetRHits())->First();

  Bool_t back;

  if (hit->GetZ() < 0.) {
    back = kTRUE;
  }
  else {
    back = kFALSE;
  }

  GetClosestHit(track, 0, back);

  if(!fTrackCuts->IsGoodFoundTrack(track)) {
    // cleanup
    track->Clear();
    return kFALSE;
  }
  else {
    return kTRUE;
  }

}


//________________________________________________________________
void PndTpcLheTrackFinder::TrackingInfo(char *info) {
  // Information about the tracking process.
  
  cout << " found ";
  
  cout.width(5);  cout << fFoundTracks->GetEntriesFast() << " tracks ";

}

//________________________________________________________________
void PndTpcLheTrackFinder::LoopOverHits() {
  //--- loops over all hits

  Int_t entries = fCMHits->GetEntriesFast();

  for (Int_t nhit = 0; nhit < entries; nhit++) {
    PndLheCMPoint *hit = (PndLheCMPoint *)fCMHits->At(nhit);
    if (fVerbose) hit->Print();
    // start hit was used before
    if (!hit->GetUsage() ) {
      CreateTrack(hit);
    }
  }

  CheckClones();
}

//_______________________________________________________________________
void PndTpcLheTrackFinder::CreateTrack(PndLheCMPoint *seed_hit) {
  //---

  Double_t theta_cut = 3.*.0002;
  Int_t theta_low = fSegment->GetThetaSegm(seed_hit->GetTheta() - theta_cut);
  Int_t theta_high = fSegment->GetThetaSegm(seed_hit->GetTheta() + theta_cut);

  Double_t phi_cut = 3.*.004;
  Int_t phi_low = fSegment->GetPhiSegm(seed_hit->GetPhi() - phi_cut);
  Int_t phi_high = fSegment->GetPhiSegm(seed_hit->GetPhi() + phi_cut);

  for (Int_t theta_segm = theta_low; theta_segm <= theta_high;
       theta_segm++) {
     
    //     phi
    for (Int_t phi_segm = phi_low; phi_segm <= phi_high;
	 phi_segm++) {
        
      // loop over entries in one sub segment
      TObjArray *cell = (TObjArray *)fSegments->At
	//	(fSegment->GetSegm(theta_segm, phi_segm));
	(fSegment->GetSegm(theta_segm, phi_segm));

      Int_t cell_entries = cell->GetEntriesFast();
      if (cell_entries) {
          
	for (Int_t hit_num = 0; hit_num < cell_entries; hit_num++) {
	  PndLheCMPoint *hit = (PndLheCMPoint *)cell->At(hit_num);
            
	  if( TMath::Abs(seed_hit->GetTheta() - hit->GetTheta()) >
	      theta_cut) continue;
	  if( TMath::Abs(seed_hit->GetPhi() - hit->GetPhi()) >
	      phi_cut) continue;
	  if (!hit->GetUsage() &&
	      hit->GetHitNumber() != seed_hit->GetHitNumber()) {

	    Int_t ntracks = fCMTracks->GetEntriesFast();
	    fCMTracks->AddAtAndExpand(new PndLheCMCandidate(ntracks), ntracks);
	    PndLheCMCandidate *new_track =
	      (PndLheCMCandidate *)fCMTracks->At(ntracks);
	    //	    fNTracks++;

	    new_track->AddPoint(seed_hit, kFALSE);
	    new_track->AddPoint(hit, kFALSE);

	    seed_hit->SetUsage(kTRUE);
	    hit->SetUsage(kTRUE);
	    TrackExtension(new_track);

	  }      //  if (!hit->GetUsage())
	}        //  over entries in segment
      }          //  over segments
    }            //  for (phi)
  }              //  for (theta)

 }

//________________________________________________________________
void PndTpcLheTrackFinder::
GetClosestHit(PndLheCMCandidate *cmtrack, Int_t cur_station, Bool_t back) {
  //--- Returns the nearest hit in the next station
 
  PndLheCMPoint *hit = NULL;
 
  TArrayI *philist  = new TArrayI();
  TArrayI *thetalist  = new TArrayI();

  if (cmtrack->GetNumberOfHits() < 3 ) {
    GetPhiRange(philist, cmtrack);
    GetThetaRange(thetalist, cmtrack);
  }

  Int_t* jphi = philist->GetArray();
  Int_t* jtheta = thetalist->GetArray();
  
  for (Int_t it =  0; it < thetalist->GetSize(); it++) {
    Int_t theta_segm = jtheta[it];
    
    for(Int_t ip = 0; ip < philist->GetSize(); ip++) {
      Int_t phi_segm = jphi[ip];
      
      // loop over entries in one sub segment
      TObjArray *cell = (TObjArray *)fSegments->At
	(fSegment->GetSegm(theta_segm, phi_segm));
     
      Int_t entries = cell->GetEntriesFast();
      
      if (entries) {
	for (Int_t sub_hit_num = 0; sub_hit_num < entries; sub_hit_num++) {
	  hit = (PndLheCMPoint *)cell->At(sub_hit_num);
            
	  if (!hit->GetUsage()) {     // hit was not used before
	    if(fTrackCuts->VerifyTrack(cmtrack, hit, back)) {
	      cmtrack->AddPoint(hit, kFALSE);
	      hit->SetUsage(kTRUE);
	      GetClosestHit(cmtrack, 0, back);
	    }
	    else {
	    }
	  }      //  if (!hit->GetUsage())
	}        //  over entries
      }   //       //  over segments
      else {
      }
    }            //  for (phi)
  }              //  for (theta)
  
  delete philist;
  delete thetalist;
}

//______________________________________________________________________
void PndTpcLheTrackFinder::GetThetaRange(TArrayI* list,PndLheCMCandidate *cmtrack) {
  //---  

  TObjArray *trackpoint = cmtrack->GetCMHits();
  PndLheCMPoint *hit = (PndLheCMPoint *)trackpoint->First();
  Double_t thet = hit->GetTheta();

  Int_t theta_high, theta_low;

  if (fSegment->GetThetaSegm(thet) < fSegment->GetNumThetaSegments()/2) {
    theta_high = fSegment->GetThetaSegm(thet);
    theta_low = 0;
  } else {
    theta_high = fSegment->GetNumThetaSegments() - 1;
    theta_low = fSegment->GetThetaSegm(thet);
  }

  Int_t nTheta = theta_high - theta_low;

  list->Set(nTheta);

  for (Int_t s = 0; s < nTheta; s++) {
    list->AddAt(theta_low + s, s);
  }

}

//______________________________________________________________________
void PndTpcLheTrackFinder::GetPhiRange(TArrayI* cl1, PndLheCMCandidate *cmtrack ) {
  //---  
  
  TObjArray *trackpoint = cmtrack->GetCMHits();
  PndLheCMPoint *hit = (PndLheCMPoint *)trackpoint->First();
  PndLheCMPoint *hit1 = (PndLheCMPoint *)trackpoint->Last();

  Double_t phi = hit->GetPhi();

  Double_t phi_c = fTrackCuts->GetPhiPrediction(cmtrack);

  Double_t phi_m = phi_c - TMath::Pi()/2.;
  Double_t phi_p = phi_c + TMath::Pi()/2.;

  Double_t OutRadTpc = .42; // m
  Double_t Rad = cmtrack->GetRadius()/100.; // m
  Double_t MagField = 2; //    Tesla
  Double_t pt = .2998 * Rad * MagField;  // PR(pt);

  PndLhePoint vert = cmtrack->GetVertex();
  Double_t xtr_c = vert.GetX()/100.; // m
  Double_t ytr_c = vert.GetY()/100.; // m

  Double_t xcross_1 = 0.;
  Double_t ycross_1 = 0.;
  Double_t xcross_2 = 0.;
  Double_t ycross_2 = 0.;
  Int_t crossing = fTrackCuts->Circle_Circle_Intersection(0., 0., OutRadTpc,
	xtr_c, ytr_c, Rad, &xcross_1, &ycross_1, &xcross_2, &ycross_2);
  
  Int_t range, phi_low, phi_high;

  Double_t phi_cr;

  // distance from 1st point to line (vertex - 2nd point)    
  Double_t dist = hit->GetX()*hit1->GetY() - hit->GetY()*hit1->GetX();

  if (dist >  0.) {
    // negative track -- counterclockwize
    cmtrack->SetCharge(-1);

    Double_t phi_r = phi_c - TMath::Pi()/2.;
    if (phi_r < 0.) phi_r = TMath::TwoPi() + phi_r;
    
      phi_low = fSegment->GetPhiSegm(phi_r);
     
      if(crossing) {
	TVector2 v(xcross_2, ycross_2);
	phi_cr = (v.Phi() >= 0.) ? phi_cr = v.Phi() :
	  phi_cr = v.Phi() + 2.*TMath::Pi();
	phi_high = fSegment->GetPhiSegm(phi_cr);
      }
      else {
	phi_high = fSegment->GetPhiSegm(phi_c);
      }
      
      range = phi_high - phi_low + 1;

      if (phi_high < phi_low) {
	range = fSegment->GetNumPhiSegments() - phi_low + phi_high + 1;
      }
      cl1->Set(range);
      
      for (Int_t s = 0; s < range; s++) {
	//cout << "range " << range << " cont " << phi_low + s << " phi_low " << phi_low << " s " << s << " fSeg " << fSegment->GetNumPhiSegments() << endl; 
	Int_t segm = phi_low + s;
	if(segm >=  fSegment->GetNumPhiSegments()) 
	  segm = segm - fSegment->GetNumPhiSegments();
	cl1->AddAt(segm, s);
      }
      
    } else {
      
    cmtrack->SetCharge(1);
    TVector2 v(xcross_1, ycross_1);
    phi_cr = (v.Phi() >= 0.) ? phi_cr = v.Phi() :
      phi_cr = v.Phi() + 2.*TMath::Pi();
    
    phi_low = fSegment->GetPhiSegm(phi_cr);
    phi_high = fSegment->GetPhiSegm(phi_c + TMath::Pi()/2.);
    
    if(crossing) {
      phi_low = fSegment->GetPhiSegm(phi_cr);
    }
    else {
      phi_low = fSegment->GetPhiSegm(phi_c);
    }

    range = phi_high - phi_low + 1;
    
    if (phi_low > phi_high) {
      range = fSegment->GetNumPhiSegments() - phi_low + phi_high + 1;
    }
    
    cl1->Set(range);
    
    for (Int_t s = 0; s < range; s++) {
      Int_t segm = phi_high - s;
      if (segm < 0) segm = fSegment->GetNumPhiSegments() + segm;
      cl1->AddAt(segm, s);
    }
  }
    
}


//______________________________________________________________________
void PndTpcLheTrackFinder::CopyClones(TClonesArray* cl1,
				   TClonesArray* cl2) {
  //---

  Int_t nEntries = cl1->GetEntriesFast();

  TClonesArray& clref = *cl2;
  PndLheCandidate* old = NULL;

  for (Int_t i=0; i < nEntries; i++) {
    old = (PndLheCandidate*) cl1->At(i);
    new (clref[i]) PndLheCandidate(*old);
  }
  
  cout << " - " << cl2->GetEntriesFast() << " Geant tracks are copied.\n";

} //&:)=

/* ============================================================================
===============================================================================
===============================================================================
===============================================================================
===============================================================================
*/
