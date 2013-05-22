////////////////////////////////////////////////////////////
//
// PndTrkQATask
// 
// Class for QA histograms
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//                                   
////////////////////////////////////////////////////////////

#include "PndTrkQATask.h"

#include "PndTrkLegendreTransform.h"
// stt
#include "PndSttPoint.h"
#include "PndSttHit.h"
#include "PndSttTube.h"
#include "PndSttMapCreator.h"
// mvd
#include "PndSdsMCPoint.h"
// sds
#include "PndSdsHit.h"
// track(cand)
#include "PndTrackCand.h"
#include "PndTrackCandHit.h"
#include "PndTrack.h"
#include "PndTrackID.h"

// fairroot
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
// ROOT
#include "TClonesArray.h"
#include "TVector3.h"


// tracking 
#include "PndTrkClusterList.h"
#include "PndTrkGlpkFits.h"

#include <iostream>
    
using namespace std;



// -----   Default constructor   -------------------------------------------
PndTrkQATask::PndTrkQATask() : FairTask("QA plots") {
  fVerbose = 0; 
  fPersistence = kTRUE;
  sprintf(fSttBranch,"STTHit");
  sprintf(fMvdPixelBranch,"MVDHitsPixel");
  sprintf(fMvdStripBranch,"MVDHitsStrip");
  sprintf(fInputTrackBranch,"Track");
  sprintf(fInputTrackIDBranch,"TrackID");

  fUseMVDPix = kTRUE;
  fUseMVDStr = kTRUE;
  fUseSTT = kTRUE;
  fUseSTTSkew = kTRUE;
}

// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndTrkQATask::~PndTrkQATask() { 

}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndTrkQATask::Init() {
  
  fEventCounter = 0;

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndTrkQATask::Init: "
	 << "RootManager not instantiated, return!" << endl;
    return kFATAL;
  }

  // -- HITS -------------------------------------------------
  //
  // STT
  fSttPointArray = (TClonesArray*) ioman->GetObject("STTPoint");
  if (!fSttPointArray ) return kERROR;
  fSttHitArray = (TClonesArray*) ioman->GetObject(fSttBranch);
  if (!fSttHitArray ) return kERROR;

  // MVD 
  fMvdPointArray = (TClonesArray*) ioman->GetObject("MVDPoint");
  if (!fMvdPointArray ) return kERROR;
  // MVD PIXEL
  fMvdPixelHitArray = (TClonesArray*) ioman->GetObject(fMvdPixelBranch);
  if ( !fMvdPixelHitArray) return kERROR;
  // MVD STRIP
  fMvdStripHitArray = (TClonesArray*) ioman->GetObject(fMvdStripBranch);
  if ( !fMvdStripHitArray) return kERROR;

  // TRACKS
  fMCTrackArray = (TClonesArray*) ioman->GetObject("MCTrack");
  fTrackArray = (TClonesArray*) ioman->GetObject(fInputTrackBranch);
  fTrackIDArray = (TClonesArray*) ioman->GetObject(fInputTrackIDBranch);


  fIdealTrackCandArray = new TClonesArray("PndTrackCand");
  ioman->Register("IdealTrackCand",  "QA", fIdealTrackCandArray, fPersistence); // CHECK

  // ----------------------------------------   maps of STT tubes
  PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
  fTubeArray = mapper->FillTubeArray();
  // ----------------------------------------------------  end map
  
  hEfficiency = new TH2F("hEfficiency","efficiency = assigned/nof MC points", 100, 0., 100., 50, 0., 1.1);
  hEfficiencyStt = new TH2F("hEfficiencyStt","STT efficiency = assigned/nof MC points", 100, 0., 100., 50, 0., 1.1);
  hEfficiencySttSkew = new TH2F("hEfficiencySttSkew","STT skew efficiency = assigned/nof MC points", 30, 0., 30., 50, 0., 1.1);
  hEfficiencySttParal = new TH2F("hEfficiencySttParal","STT paral efficiency = assigned/nof MC points", 100, 0., 100., 50, 0., 1.1);
  hEfficiencyMvdPixel = new TH2F("hEfficiencyMvdPixel","MVDPIX efficiency = assigned/nof MC points", 20, 0., 20., 50, 0., 1.1);
  hEfficiencyMvdStrip = new TH2F("hEfficiencyMvdStrip","MVDSTR efficiency = assigned/nof MC points", 20, 0., 20., 50, 0., 1.1);

  hInefficiency = new TH2F("hInefficiency","inefficiency = not assigned/nof MC points ", 100, 0., 100., 50, 0., 1.1);
  hInefficiencyStt = new TH2F("hInefficiencyStt","STT inefficiency = not assigned/nof MC points ", 100, 0., 100., 50, 0., 1.1);
  hInefficiencySttSkew = new TH2F("hInefficiencySttSkew","STT skew inefficiency = not assigned/nof MC points ", 30, 0., 30., 50, 0., 1.1);
  hInefficiencySttParal = new TH2F("hInefficiencySttParal","STT paral inefficiency = not assigned/nof MC points ", 100, 0., 100., 50, 0., 1.1);
  hInefficiencyMvdPixel = new TH2F("hInefficiencyMvdPixel","MVDPIX inefficiency = not assigned/nof MC points ", 20, 0., 20., 50, 0., 1.1);
  hInefficiencyMvdStrip = new TH2F("hInefficiencyMvdStrip","MVDSTR inefficiency = not assigned/nof MC points ", 20, 0., 20., 50, 0., 1.1);

  hContamination = new TH2F("hContamination","contamination = wrongly assigned/nof track hits", 100, 0., 100., 50, 0., 1.1);
  hPurity = new TH2F("hPurity","purity = correctly assigned/nof track hits", 100, 0., 100., 50, 0., 1.1);

  fGoodTrack = 0, fBadTrack = 0, fMCReconstructableTrack = 0, fNotReconstructed = 0;


  return kSUCCESS;

}
void PndTrkQATask::WriteHistograms(){
  
  TFile* file = FairRootManager::Instance()->GetOutFile();
  file->cd();
  file->mkdir("QATask");
  file->cd("QATask");

  hEfficiency->Write();
  delete hEfficiency;
  hEfficiencyStt->Write();
  delete hEfficiencyStt;
  hEfficiencySttSkew->Write();
  delete hEfficiencySttSkew;
  hEfficiencySttParal->Write();
  delete hEfficiencySttParal;
  hEfficiencyMvdPixel->Write();
  delete hEfficiencyMvdPixel;
  hEfficiencyMvdStrip->Write();
  delete hEfficiencyMvdStrip;

  hInefficiency->Write();
  delete hInefficiency;
  hInefficiencyStt->Write();
  delete hInefficiencyStt;
  hInefficiencySttSkew->Write();
  delete hInefficiencySttSkew;
  hInefficiencySttParal->Write();
  delete hInefficiencySttParal;
  hInefficiencyMvdPixel->Write();
  delete hInefficiencyMvdPixel;
  hInefficiencyMvdStrip->Write();
  delete hInefficiencyMvdStrip;

  hContamination->Write();
  delete hContamination;
  hPurity->Write();
  delete hPurity; 
}
// -------------------------------------------------------------------------

void PndTrkQATask::SetParContainers() {
  FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}
 
// -------------------------------------------------------------------------
 
void PndTrkQATask::Exec(Option_t* opt) {

  fThisGoodTrack = 0, fThisBadTrack = 0, fThisMCReconstructableTrack = 0, fThisNotReconstructed = 0;

  // CHECK delete this ---
//   if(fEventCounter == 525) {
//     fEventCounter++;
//     return;
//   }
//   // -----
  fEventCounter++;

  fIdealTrackCandArray->Delete();
  Bool_t idealtrackfinder = IdealTrackFinding();
  fMCReconstructableTrack += fIdealTrackCandArray->GetEntriesFast();
  fThisMCReconstructableTrack += fIdealTrackCandArray->GetEntriesFast();

  for(int itrk = 0; itrk < fIdealTrackCandArray->GetEntriesFast(); itrk++) {
      PndTrackCand *mctrkcand = (PndTrackCand*) fIdealTrackCandArray->At(itrk);
      if(!mctrkcand) continue;
      Int_t mctrackID = mctrkcand->getMcTrackId();
      bool reconstructed = kFALSE;
      for(int jtrk = 0; jtrk < fTrackArray->GetEntriesFast(); jtrk++) {
	PndTrack *trk = (PndTrack*) fTrackArray->At(jtrk);
	if(!trk) continue;
	PndTrackID *trkID = (PndTrackID*) fTrackIDArray->At(jtrk);
	if(!trkID) continue;
	
	Int_t recotrackID = trkID->GetCorrTrackID();

	if(recotrackID != mctrackID) continue;

	PndTrackCand *trkcand = trk->GetTrackCandPtr();
    	if(!trkcand) continue;

	int nofmctrackpoints   = mctrkcand->GetNHits();
	int nofmctracksttpoints = mctrkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId(fSttBranch));
	int nofmctracksttskewpoints = 0, nofmctracksttparalpoints = 0;
	int nofmctrackmvdpixpoints = mctrkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId(fMvdPixelBranch));
	int nofmctrackmvdstrpoints = mctrkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId(fMvdStripBranch));
	int nofrecotrackpoints = trkcand->GetNHits();
	int nofrecotracksttpoints = trkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId(fSttBranch));
	int nofrecotracksttskewpoints = 0, nofrecotracksttparalpoints = 0;
	int nofrecotrackmvdpixpoints = trkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId(fMvdPixelBranch));
	int nofrecotrackmvdstrpoints = trkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId(fMvdStripBranch));

	cout << "POINTS MC " << endl;
	cout << nofmctrackpoints << " " << nofmctracksttpoints << " " << nofmctrackmvdpixpoints << " " << nofmctrackmvdstrpoints << endl;
	cout << "POINTS RECO " << endl;
	cout << nofrecotrackpoints << " " << nofrecotracksttpoints << " " << nofrecotrackmvdpixpoints << " " << nofrecotrackmvdstrpoints << endl;

	// count reco skew ---------------------------
	for(Int_t ihit = 0; ihit < nofrecotrackpoints; ihit++) {
	  PndTrackCandHit candhit = trkcand->GetSortedHit(ihit);
	  Int_t hitID = candhit.GetHitId();
	  Int_t detID = candhit.GetDetId();
	  if(detID == FairRootManager::Instance()->GetBranchId(fSttBranch)) {
	    PndSttHit *stthit = (PndSttHit*) fSttHitArray->At(hitID);
	    int tubeID = stthit->GetTubeID();
	    PndSttTube *tube = (PndSttTube*) fTubeArray->At(tubeID);
	    if(tube->GetWireDirection().Z() != 1.) nofrecotracksttskewpoints++;
	    else nofrecotracksttparalpoints++;
	  }
	}
	// ---------------------------------------------

	Int_t nAssigned = 0, nNotAssigned = 0, nWrong = 0;
	Int_t nAssignedStt = 0, nNotAssignedStt = 0, nWrongStt = 0;
	Int_t nAssignedSttSkew = 0, nNotAssignedSttSkew = 0, nWrongSttSkew = 0;
	Int_t nAssignedSttParal = 0, nNotAssignedSttParal = 0, nWrongSttParal = 0;
	Int_t nAssignedMvdPixel = 0, nNotAssignedMvdPixel = 0, nWrongMvdPixel = 0;
	Int_t nAssignedMvdStrip = 0, nNotAssignedMvdStrip = 0, nWrongMvdStrip = 0;

	
	for(Int_t ihit = 0; ihit < nofmctrackpoints; ihit++) {
	  PndTrackCandHit mccandhit = mctrkcand->GetSortedHit(ihit);
	  Int_t hitID1 = mccandhit.GetHitId();
	  Int_t detID1 = mccandhit.GetDetId();
	  bool correct = kFALSE, skewed = kFALSE;

	  if(!fUseMVDPix && detID1 == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) continue;
	  if(!fUseMVDStr && detID1 == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) continue;
	  if(!fUseSTT && detID1 == FairRootManager::Instance()->GetBranchId(fSttBranch)) continue;


	  // count skew ---------------------------
	  if(detID1 == FairRootManager::Instance()->GetBranchId(fSttBranch)) {
	    PndSttHit *stthit = (PndSttHit*) fSttHitArray->At(hitID1);
	    int tubeID = stthit->GetTubeID();
	    PndSttTube *tube = (PndSttTube*) fTubeArray->At(tubeID);
	    if(tube->GetWireDirection().Z() != 1.) {
	      nofmctracksttskewpoints++;
	      skewed = kTRUE;
	    }
	  }
	  nofmctracksttparalpoints = nofmctracksttpoints - nofmctracksttskewpoints;
	  // ---------------------------------------

	  for(Int_t jhit = 0; jhit < nofrecotrackpoints; jhit++) {
	    PndTrackCandHit candhit = trkcand->GetSortedHit(jhit);
	    Int_t hitID2 = candhit.GetHitId();
	    Int_t detID2 = candhit.GetDetId();

	    if(hitID1 != hitID2) continue;
	    if(detID1 != detID2) continue;
	    nAssigned++;
	    if(detID1 == FairRootManager::Instance()->GetBranchId(fSttBranch)){
	      nAssignedStt++;
	      if(skewed) nAssignedSttSkew++;
	      else nAssignedSttParal++;
	    }
	    else if(detID1 == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) nAssignedMvdPixel++;
	    else if(detID1 == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) nAssignedMvdStrip++;
	    correct = kTRUE;
	    cout << "point " <<  hitID1 << " " << detID1 << " assigned" << endl;
	    
	  }
	  if(!correct) {
	    cout << "point " <<  hitID1 << " " << detID1 << " not assigned" << endl;
	    nNotAssigned++;
	    if(detID1 == FairRootManager::Instance()->GetBranchId(fSttBranch)) {
	      nNotAssignedStt++;
	      if(skewed) nNotAssignedSttSkew++;
	      else nNotAssignedSttParal++;
	      // ------------------------
	    }
	    else if(detID1 == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) nNotAssignedMvdPixel++;
	    else if(detID1 == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) nNotAssignedMvdStrip++;
	  }
	}
	//	nofmctrackpoints -= nofskew; // CHECK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 
	//	nofmctracksttpoints -= nofskew; // CHECK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// switch some detectors off ............
	if(!fUseMVDPix) {
	  nofmctrackpoints -= nofmctrackmvdpixpoints;
	  nofmctrackmvdpixpoints = 0;
	}
	if(!fUseMVDStr) {
	  nofmctrackpoints -= nofmctrackmvdstrpoints;
	  nofmctrackmvdstrpoints = 0;
	}
	if(!fUseSTT) {	
	  nofmctrackpoints -= nofmctracksttpoints;
	  nofmctracksttpoints = 0;
	}
	else if(!fUseSTTSkew) {                                   // CHECK 
	  nofmctrackpoints -= nofmctracksttskewpoints;            // CHECK 
	  nofmctracksttskewpoints = 0;                            // CHECK 
	  nofmctracksttpoints -= nofmctracksttskewpoints;         // CHECK 
	}
	// ......................................

	if(nofmctrackpoints > 0) {
	  hEfficiency->Fill(nofmctrackpoints, (Double_t) nAssigned/nofmctrackpoints);
	  hInefficiency->Fill(nofmctrackpoints, (Double_t) nNotAssigned/nofmctrackpoints);
	}

	if(nofmctracksttpoints > 0) {
	  hEfficiencyStt->Fill(nofmctracksttpoints, (Double_t) nAssignedStt/nofmctracksttpoints);
	  hInefficiencyStt->Fill(nofmctracksttpoints, (Double_t) nNotAssignedStt/nofmctracksttpoints);
	}
	if(nofmctracksttskewpoints > 0) {
	  hEfficiencySttSkew->Fill(nofmctracksttskewpoints, (Double_t) nAssignedSttSkew/nofmctracksttskewpoints);
	  hInefficiencySttSkew->Fill(nofmctracksttskewpoints, (Double_t) nNotAssignedSttSkew/nofmctracksttskewpoints);
	}

	if(nofmctracksttparalpoints > 0) {
	  hEfficiencySttParal->Fill(nofmctracksttparalpoints, (Double_t) nAssignedSttParal/nofmctracksttparalpoints);
	hInefficiencySttParal->Fill(nofmctracksttparalpoints, (Double_t) nNotAssignedSttParal/nofmctracksttparalpoints);
	}

	if(nofmctrackmvdpixpoints > 0) {
	  hEfficiencyMvdPixel->Fill(nofmctrackmvdpixpoints, (Double_t) nAssignedMvdPixel/nofmctrackmvdpixpoints);
	  hInefficiencyMvdPixel->Fill(nofmctrackmvdpixpoints, (Double_t) nNotAssignedMvdPixel/nofmctrackmvdpixpoints);	
	}

	if(nofmctrackmvdstrpoints > 0) {
	  hEfficiencyMvdStrip->Fill(nofmctrackmvdstrpoints, (Double_t) nAssignedMvdStrip/nofmctrackmvdstrpoints);
	  hInefficiencyMvdStrip->Fill(nofmctrackmvdstrpoints, (Double_t) nNotAssignedMvdStrip/nofmctrackmvdstrpoints);
	}

	nWrong = nofrecotrackpoints - nAssigned;
	nWrongStt      = nofrecotracksttpoints - nAssignedStt;
	nWrongSttSkew  = nofrecotracksttskewpoints - nAssignedSttSkew;
	nWrongSttParal = nofrecotracksttparalpoints - nAssignedSttParal;
	nWrongMvdPixel = nofrecotrackmvdpixpoints - nAssignedMvdPixel;
	nWrongMvdStrip = nofrecotrackmvdstrpoints - nAssignedMvdStrip;

	if(nofrecotrackpoints > 0) {
	  hContamination->Fill(nofrecotrackpoints, (Double_t) nWrong/nofrecotrackpoints);
	  hPurity->Fill(nofrecotrackpoints, (Double_t) nAssigned/nofrecotrackpoints);
	}

// 	hContaminationStt->Fill(nofrecotracksttpoints, (Double_t) nWrongStt/nofrecotracksttpoints);
// 	hContaminationSttSkew  ->Fill(nofrecotracksttskewpoints, (Double_t) nWrongSttSkew  /nofrecotracksttskewpoints);
// 	hContaminationSttParal->Fill(nofrecotracksttparalpoints, (Double_t) nWrongSttParal/nofrecotracksttparalpoints);
// 	hContaminationMvdPixel->Fill(nofrecotrackmvdpixpoints, (Double_t) nWrongMvdPixel/nofrecotrackmvdpixpoints);
// 	hContaminationMvdStrip->Fill(nofrecotrackmvdstrpoints, (Double_t) nWrongMvdStrip/nofrecotrackmvdstrpoints);



	cout << "TRACK " << jtrk << " MCTRACK " << itrk << " " << endl;
	cout << "total " << nofrecotrackpoints << " mctotal " << nofmctrackpoints << endl;
	cout << "assigned " <<  nAssigned <<  " not assigned " << nNotAssigned << " wrong " << nWrong << endl;
	if((nAssigned + nNotAssigned) != nofmctrackpoints) cout << "ERROR 1" << endl;
	if((nAssigned + nWrong) != nofrecotrackpoints) cout << "ERROR 2" << endl;
	if((nAssignedSttSkew + nAssignedSttParal) != nAssignedStt) cout << "ERROR 3" << endl;

	// CHECK for now:
	// a track if good if it has more than 80% of mc points assigned to it
	if(((Double_t) nAssigned/nofmctrackpoints) > 0.8) {
	  fGoodTrack++;
	  fThisGoodTrack++;
	}
	else {
	  fBadTrack++;
	  fThisBadTrack++;
	}
	reconstructed = kTRUE;
	break;
      }
      if(!reconstructed) {
	fNotReconstructed++;
	fThisNotReconstructed++;
      }

  }
  // CHECK for now:
  // fMCReconstructableTrack are the mc tracks with at least 3 parallel stt point
  cout << "#### NOW: GOOD = "<< (Double_t) fThisGoodTrack/fThisMCReconstructableTrack << "%, BAD = " << (Double_t) fThisBadTrack/fThisMCReconstructableTrack << "%, MISSED = " << (Double_t) fThisNotReconstructed/fThisMCReconstructableTrack << "%" << endl;
  cout << "======== AFTER THIS EVENT: GOOD = "<< (Double_t) fGoodTrack/fMCReconstructableTrack << "%, BAD = " << (Double_t) fBadTrack/fMCReconstructableTrack << "%, MISSED = " << (Double_t) fNotReconstructed/fMCReconstructableTrack << "%" << endl;
  
}

Int_t PndTrkQATask::CheckIfPresent(Int_t trackid) {
  for(int itrk = 0; itrk < fNofMCTrack; itrk++) {
    if(fMCTracks[itrk][0] == trackid) return itrk;
  }
  return -1;
}

Bool_t PndTrkQATask::IdealTrackFinding() {
  fIdealTrackCandArray->Delete();
  cout << "IDEAL" << endl;
  // Initialise control counters
  Int_t nNoMCTrack    = 0;
  Int_t nNoTrack      = 0;

  Int_t nSttPoints   = 0;
  Int_t nMvdPoints   = 0;
  Int_t nMCPoints    = 0;
  nSttPoints = fSttPointArray->GetEntriesFast();
  nMvdPoints = fMvdPointArray->GetEntriesFast();
  nMCPoints = nSttPoints + nMvdPoints;


  Int_t nSttHits     = 0;
  Int_t nMvdPixHits  = 0;
  Int_t nMvdStrHits  = 0;
  Int_t     nHits = 0;
  nSttHits = fSttHitArray->GetEntriesFast();
  nMvdPixHits = fMvdPixelHitArray->GetEntriesFast();
  nMvdStrHits = fMvdStripHitArray->GetEntriesFast();
  nHits = nSttHits + nMvdPixHits + nMvdStrHits;

  // Create pointers to hit and SttPoint
  PndSttHit*       pMhitSTT = NULL;
  PndSdsHit*  pMhitPIX = NULL;
  PndSdsHit*  pMhitSTR = NULL;

  FairMCPoint*     pMCpt = NULL;
  PndMCTrack*      pMCtr = NULL;
  PndTrackCand* pTrckCand = NULL; 

  // Declare some variables outside the loops
  Int_t ptIndex      = 0;     // MCPoint index
  Int_t mcTrackIndex = 0;     // MCTrack index
  Int_t trackIndex   = 0;     // STTTrack index

  // Create STL map from MCtrack index to number of MCPoints 
   Int_t nMCTracks = fMCTrackArray->GetEntriesFast();
 Int_t hitMap[nMCTracks][2];
 for (Int_t iMCTrack = 0; iMCTrack < nMCTracks; iMCTrack++) {
   hitMap[iMCTrack][0] = 0;
   hitMap[iMCTrack][1] = 0;
 }

 // loop over mc points
 for (Int_t ipnt = 0; ipnt < nMvdPoints; ipnt++) 
   {
     pMCpt =  (PndSdsMCPoint*) fMvdPointArray->At(ipnt);
     if (!pMCpt) continue; 
     mcTrackIndex = pMCpt->GetTrackID(); 
     hitMap[mcTrackIndex][0]++;
     cout << "MVD ON " << mcTrackIndex << " " << endl;
   }
  
 for (Int_t ipnt = 0; ipnt < nSttPoints; ipnt++) 
   {
     pMCpt =  (PndSttPoint*) fSttPointArray->At(ipnt);
     if (!pMCpt) continue; 
    
     // CHECK do not count stt skew since we need at least 3 hits 
     // in xy plane to do the fit in xy -------------------------
     int tubeID = ((PndSttPoint*) pMCpt)->GetTubeID();
     PndSttTube *tube = (PndSttTube*) fTubeArray->At(tubeID);
     if(tube->GetWireDirection().Z() != 1.) continue;
     // -----------------------------------------------

      mcTrackIndex = pMCpt->GetTrackID();  
      hitMap[mcTrackIndex][1]++;
      cout << "STT ON " << mcTrackIndex << " " << endl;
    }
  // --------------------------------------------------------------------



  // Create STL map from MCTrack index to Track index
  map<Int_t, Int_t> 
    correlationMap,
    trackMap;
  
  // Create STTTracks for reconstructable MCTracks
  Int_t nMCacc  = 0;         // accepted MCTracks (more than 3 points)
  Int_t nTracks = 0;         // reconstructable MCTracks
 

  for (Int_t iMCTrack = 0; iMCTrack < nMCTracks; iMCTrack++) 
    {
      pMCtr = (PndMCTrack*) fMCTrackArray->At(iMCTrack);
      if ( ! pMCtr ) 
	continue;

      // CUT CHECK 
      // CHECK cut at least 3 stt parallel MC points
      // if ((hitMap[iMCTrack][0] + hitMap[iMCTrack][1]) < 3) continue;
      if((hitMap[iMCTrack][1]) < 3) continue;

      // temporary hack, fix this in monte carlo ....
      if (TDatabasePDG::Instance()->GetParticle(pMCtr->GetPdgCode()) == NULL)	continue;

      if (!(fabs(TDatabasePDG::Instance()->GetParticle(pMCtr->GetPdgCode())->Charge() / 3.0) > 0.)) continue;

      nMCacc++;

      pTrckCand = new((*fIdealTrackCandArray)[nTracks]) PndTrackCand(); 
      pTrckCand->setMcTrackId(iMCTrack);
      cout << "iMCTrack " << iMCTrack << endl;
      correlationMap[nTracks] = iMCTrack;
      trackMap[iMCTrack] = nTracks++;


    }
  // -------------------------------------------------------------------

  // Loop over hits. Get corresponding MCPoint and MCTrack index
  for (Int_t iHit = 0; iHit < nMvdPixHits; iHit++) 
    {
      pMhitPIX = (PndSdsHit*) fMvdPixelHitArray->At(iHit);
      if (!pMhitPIX) continue; 
      ptIndex = pMhitPIX->GetRefIndex();
      if (ptIndex < 0) continue;         
      pMCpt =  (PndSdsMCPoint*) fMvdPointArray->At(ptIndex);
      if (!pMCpt) continue; 
      mcTrackIndex = pMCpt->GetTrackID();
   
      if (mcTrackIndex < 0 || mcTrackIndex > nMCTracks) 
	{
	  cout << "-E- MCTrack index out of range. " << mcTrackIndex << " " << nMCTracks << endl;
	  nNoMCTrack++;
	  continue;
	}

      if (trackMap.find(mcTrackIndex) == trackMap.end()) continue;
      
      trackIndex = trackMap[mcTrackIndex];
      pTrckCand = (PndTrackCand*) fIdealTrackCandArray->At(trackIndex);  
      if ( ! pTrckCand ) 
	{
	  cout << "-E- No Track pointer. " << iHit << " " << ptIndex 
	       << " " << mcTrackIndex << " " << trackIndex << endl;
	  nNoTrack++;
	  continue;
	}
      int counter = pTrckCand->GetNHits();
      pTrckCand->AddHit(FairRootManager::Instance()->GetBranchId(fMvdPixelBranch), iHit, counter);
  
  }


  for (Int_t iHit = 0; iHit < nMvdStrHits; iHit++) 
    {
      pMhitSTR = (PndSdsHit*) fMvdStripHitArray->At(iHit);
      if (!pMhitSTR) continue; 
      ptIndex = pMhitSTR->GetRefIndex();
      if (ptIndex < 0) continue;         
      pMCpt =  (PndSdsMCPoint*) fMvdPointArray->At(ptIndex);
      if (!pMCpt) continue; 
      mcTrackIndex = pMCpt->GetTrackID();
   
      if (mcTrackIndex < 0 || mcTrackIndex > nMCTracks) 
	{
	  cout << "-E- MCTrack index out of range. " << mcTrackIndex << " " << nMCTracks << endl;
	  nNoMCTrack++;
	  continue;
	}

      if (trackMap.find(mcTrackIndex) == trackMap.end()) continue;
      
      trackIndex = trackMap[mcTrackIndex];
      pTrckCand = (PndTrackCand*) fIdealTrackCandArray->At(trackIndex);  
      if ( ! pTrckCand ) 
	{
	  cout << "-E- No Track pointer. " << iHit << " " << ptIndex 
	       << " " << mcTrackIndex << " " << trackIndex << endl;
	  nNoTrack++;
	  continue;
	}
      int counter = pTrckCand->GetNHits();
      pTrckCand->AddHit(FairRootManager::Instance()->GetBranchId(fMvdStripBranch), iHit,  counter);

    }
 
  for (Int_t iHit = 0; iHit < nSttHits; iHit++) 
    {
      pMhitSTT = (PndSttHit*) fSttHitArray->At(iHit);

      if (!pMhitSTT) continue; 

      ptIndex = pMhitSTT->GetRefIndex();
      if (ptIndex < 0) continue;         
      pMCpt =  (PndSttPoint*) fSttPointArray->At(ptIndex);
       if (!pMCpt) continue; 
      mcTrackIndex = pMCpt->GetTrackID();
   
      if (mcTrackIndex < 0 || mcTrackIndex > nMCTracks) 
	{
	  cout << "-E- MCTrack index out of range. " << mcTrackIndex << " " << nMCTracks << endl;
	  nNoMCTrack++;
	  continue;
	}

      if (trackMap.find(mcTrackIndex) == trackMap.end()) continue;
      
      trackIndex = trackMap[mcTrackIndex];
      pTrckCand = (PndTrackCand*) fIdealTrackCandArray->At(trackIndex);  
      if ( ! pTrckCand ) 
	{
	  cout << "-E- No Track pointer. " << iHit << " " << ptIndex 
	       << " " << mcTrackIndex << " " << trackIndex << endl;
	  nNoTrack++;
	  continue;
	}
      int counter = pTrckCand->GetNHits();
      pTrckCand->AddHit(FairRootManager::Instance()->GetBranchId(fSttBranch), iHit,  counter);

    }

}
ClassImp(PndTrkQATask)

