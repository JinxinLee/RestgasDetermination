#include "PndTpcLheTrackFinder.h"

#include "PndTpcLheCMTrack.h"
#include "PndTpcLheCMPoint.h"
#include "PndTpcLheHit.h"
#include "lhe.h"

#include "CbmMCApplication.h"
#include "CbmTask.h"
#include "CbmRunAna.h"
#include "CbmGeoNode.h"
#include "CbmGeoVector.h"
#include "CbmGeoMedium.h"
#include "CbmRootManager.h"

#include "TObjectTable.h"
#include "TClonesArray.h"

ClassImp(PndTpcLheTrackFinder)

//________________________________________________________________
PndTpcLheTrackFinder::PndTpcLheTrackFinder() {
  //---

  fFoundTracks = new TClonesArray("PndTpcLheTrack");
  fCMHits = new TClonesArray("PndTpcLheCMPoint");
  fBench = new TBenchmark();
  fVertex = NULL;
  fVerbose = kFALSE;
}

//_________________________________________________________________
PndTpcLheTrackFinder::
PndTpcLheTrackFinder( const char *name, const char *title):CbmTask(name) {
  //---

  fFoundTracks = new TClonesArray("PndTpcLheTrack");
  fCMHits = new TClonesArray("PndTpcLheCMPoint");
  fBench = new TBenchmark();
  fVertex = NULL;
  fVerbose = kFALSE;

}

//_________________________________________________________________
PndTpcLheTrackFinder::~PndTpcLheTrackFinder() {

  if (fCMTracks) { fCMTracks->Delete(); delete fCMTracks; }
  if (fCMHits) { fCMHits->Delete(); delete fCMHits; }

  if (fSegments) delete fSegments;

  CbmRootManager *fManager =CbmRootManager::Instance();
  fManager->Write();

}

//_________________________________________________________________
void PndTpcLheTrackFinder::Register() {
  //---
  CbmRootManager::
    Instance()->Register("PndTpcLheTrack",
  			 "Lhe", fFoundTracks, kTRUE);

  CbmRootManager::
    Instance()->Register("PndTpcLheCMPoint",
  			 "Lhe",fCMHits, kTRUE);

}

//________________________________________________________________
InitStatus PndTpcLheTrackFinder::Init() {
  // ---

  CbmRootManager *fManager = CbmRootManager::Instance();
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

  fVertex = new PndTpcLhePoint(0.0, 0.0, 0.0);

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

  PndTpcLheHit *ghit = NULL;

  Int_t good_hits = 0;

  for (Int_t ih = 0; ih < n_hits; ih++) {
    ghit = (PndTpcLheHit *) fLheHits->UncheckedAt(ih);
    TClonesArray &cmhits = *fCMHits;
    PndTpcLheCMPoint *cmhit = new(cmhits[good_hits++]) PndTpcLheCMPoint(ghit);
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
void PndTpcLheTrackFinder::AddTrackForFit(PndTpcLheCMTrack *track_in) {
  //--- Add a new track to the list of tracks for this event.

  TClonesArray &tracks = *fFoundTracks;
  Int_t size = tracks.GetEntriesFast();

  TObjArray *rhits = (TObjArray* )track_in->GetCMHits();
  cout << "PndTpcLheTrackFinder::AddTrackForFit "<<  rhits << endl;
  Int_t NHits = 0;
  if (rhits){ 
    NHits = rhits->GetEntriesFast();

    PndTpcLheTrack* track = new(tracks[size]) PndTpcLheTrack(size);
    track->SetCharge(track_in->GetCharge());
 
    Int_t mvdHits = 0;
    Int_t tpcHits = 0;
    for (Int_t lh = 0; lh < NHits; lh++) {
      PndTpcLheHit* hit = dynamic_cast <PndTpcLheHit*> (rhits->At(lh));
      track->AddHit(hit);
      if ( (hit->GetDetectorId() == kTpcPoint)     ||
	   (hit->GetDetectorId() == kTpcCluster)     )   tpcHits++;
      if ( (hit->GetDetectorId() == kMVDPoint)     || 
	   (hit->GetDetectorId() == kMVDHitsStrip) ||
	   (hit->GetDetectorId() == kMVDHitsPixel)   )   mvdHits++;
    }
    track->SetTpcHits(tpcHits);
    track->SetMvdHits(mvdHits);
  }
}

//_________________________________________________________________
void PndTpcLheTrackFinder::Finish() {

  cout << " found  "<< fFoundTracks->GetEntries() << " tracks\n";

  CbmRootManager *fManager =CbmRootManager::Instance(); 
  fManager->Fill();

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
  PndTpcLheCMTrack* trk;
  Int_t ntracks = fCMTracks->GetEntriesFast();

  next.Reset();
  while((trk = (PndTpcLheCMTrack*) next())) {
    trk->SetGood(kTRUE);
  }

  for (Int_t it = 0;it < ntracks; it++) {
    trk = (PndTpcLheCMTrack*)fCMTracks->At(it);

    PndTpcLhePoint hel_ex = trk->GetCircle();
    
    Double_t xc_ex = hel_ex.GetX();
    Double_t yc_ex = hel_ex.GetY();
    Double_t rad_ex = hel_ex.GetZ();

    for (Int_t jt = it + 1; jt < ntracks; jt++) {
      PndTpcLheCMTrack* trk_in = (PndTpcLheCMTrack*) fCMTracks->At(jt);
      PndTpcLhePoint hel_in = trk_in->GetCircle();
    
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
  while((trk = (PndTpcLheCMTrack*) next())) {
    if (trk->IsGood()) AddTrackForFit(trk);
  }
  
}

//________________________________________________________________
Bool_t PndTpcLheTrackFinder::TrackExtension(PndTpcLheCMTrack *track) {

  PndTpcLheCMPoint *hit =  (PndTpcLheCMPoint *)(track->GetRHits())->First();

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
    PndTpcLheCMPoint *hit = (PndTpcLheCMPoint *)fCMHits->At(nhit);
    if (fVerbose) hit->Print();
    // start hit was used before
    if (!hit->GetUsage() ) {
      CreateTrack(hit);
    }
  }

  CheckClones();
}

//_______________________________________________________________________
void PndTpcLheTrackFinder::CreateTrack(PndTpcLheCMPoint *seed_hit) {
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
	  PndTpcLheCMPoint *hit = (PndTpcLheCMPoint *)cell->At(hit_num);
            
	  if( TMath::Abs(seed_hit->GetTheta() - hit->GetTheta()) >
	      theta_cut) continue;
	  if( TMath::Abs(seed_hit->GetPhi() - hit->GetPhi()) >
	      phi_cut) continue;
	  if (!hit->GetUsage() &&
	      hit->GetHitNumber() != seed_hit->GetHitNumber()) {

	    Int_t ntracks = fCMTracks->GetEntries();
	    fCMTracks->AddAtAndExpand(new PndTpcLheCMTrack(ntracks), ntracks);
	    PndTpcLheCMTrack *new_track =
	      (PndTpcLheCMTrack *)fCMTracks->At(ntracks);
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
GetClosestHit(PndTpcLheCMTrack *cmtrack, Int_t cur_station, Bool_t back) {
  //--- Returns the nearest hit in the next station
 
  PndTpcLheCMPoint *hit = NULL;
 
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
	  hit = (PndTpcLheCMPoint *)cell->At(sub_hit_num);
            
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
void PndTpcLheTrackFinder::GetThetaRange(TArrayI* list,PndTpcLheCMTrack *cmtrack) {
  //---  

  TObjArray *trackpoint = cmtrack->GetCMHits();
  PndTpcLheCMPoint *hit = (PndTpcLheCMPoint *)trackpoint->First();
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
void PndTpcLheTrackFinder::GetPhiRange(TArrayI* cl1, PndTpcLheCMTrack *cmtrack ) {
  //---  
  
  TObjArray *trackpoint = cmtrack->GetCMHits();
  PndTpcLheCMPoint *hit = (PndTpcLheCMPoint *)trackpoint->First();
  PndTpcLheCMPoint *hit1 = (PndTpcLheCMPoint *)trackpoint->Last();

  Double_t phi = hit->GetPhi();

  Double_t phi_c = fTrackCuts->GetPhiPrediction(cmtrack);

  Double_t phi_m = phi_c - TMath::Pi()/2.;
  Double_t phi_p = phi_c + TMath::Pi()/2.;

  Double_t OutRadTpc = .42; // m
  Double_t Rad = cmtrack->GetRadius()/100.; // m
  Double_t MagField = 2; //    Tesla
  Double_t pt = .2998 * Rad * MagField;  // PR(pt);

  PndTpcLhePoint vert = cmtrack->GetVertex();
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
  PndTpcLheTrack* old = NULL;

  for (Int_t i=0; i < nEntries; i++) {
    old = (PndTpcLheTrack*) cl1->At(i);
    new (clref[i]) PndTpcLheTrack(*old);
  }
  
  cout << " - " << cl2->GetEntriesFast() << " Geant tracks are copied.\n";

} //&:)=

/* ============================================================================
===============================================================================
===============================================================================
===============================================================================
===============================================================================
*/
