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
// fts
#include "PndFtsPoint.h"
#include "PndFtsHit.h"
#include "PndFtsTube.h"
#include "PndFtsMapCreator.h"
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
PndTrkQATask::PndTrkQATask() : FairTask("QA plots", 0), fPersistence(kTRUE), fUseMVDPixHits(kTRUE), fUseMVDStrHits(kTRUE), fUseSTTHits(kTRUE), fUseSTTSkewHits(kTRUE), fUseFTSHits(kTRUE), fUseFTSSkewHits(kTRUE), fNFtsPoints(0), fNSttPoints(0), fNMvdPoints(0), fNMCPoints(0), fNFtsHits(0), fNSttHits(0), fNMvdPixHits(0), fNMvdStrHits(0), fNHits(0), fNMCTracks(0), fFtsPointArray(NULL), fFtsHitArray(NULL), fSttPointArray(NULL), fSttHitArray(NULL), fMvdPointArray(NULL), fMvdPixelHitArray(NULL), fMvdStripHitArray(NULL), fMCTrackArray(NULL), fTrackArray(NULL), fTrackCandArray(NULL), fIdealTrackCandArray(NULL), fTubeArrayStt(NULL), fTubeArrayFts(NULL), fTrackIDArray(NULL), fSttParameters(NULL), fFtsParameters(NULL), fMinimumNofSttParalHits(3), fMinimumNofSttSkewHits(3), fMaximumNofSttHits(1000), fMinimumNofFtsHits(6), fAccountForMultiHits(kTRUE) {
  SetDetectorsToStudy("STT");
  sprintf(fFtsBranch,"FTSHit");
  sprintf(fSttBranch,"STTHit");
  sprintf(fMvdPixelBranch,"MVDHitsPixel");
  sprintf(fMvdStripBranch,"MVDHitsStrip");
  sprintf(fInputTrackBranch,"Track");
  sprintf(fInputTrackIDBranch,"TrackID");
  fSttTubeIDToMCPointID.clear();
}




// ---------------------------------------------------------
PndTrkQATask::PndTrkQATask(TString detectorsToStudy, Int_t verbose=0) : FairTask("QA plots", verbose), fPersistence(kTRUE), fUseMVDPixHits(kTRUE), fUseMVDStrHits(kTRUE), fUseSTTHits(kTRUE), fUseSTTSkewHits(kTRUE), fUseFTSHits(kTRUE), fUseFTSSkewHits(kTRUE), fNFtsPoints(0), fNSttPoints(0), fNMvdPoints(0), fNMCPoints(0), fNFtsHits(0), fNSttHits(0), fNMvdPixHits(0), fNMvdStrHits(0), fNHits(0), fNMCTracks(0), fFtsPointArray(NULL), fFtsHitArray(NULL), fSttPointArray(NULL), fSttHitArray(NULL), fMvdPointArray(NULL), fMvdPixelHitArray(NULL), fMvdStripHitArray(NULL), fMCTrackArray(NULL), fTrackArray(NULL), fTrackCandArray(NULL), fIdealTrackCandArray(NULL), fTubeArrayStt(NULL), fTubeArrayFts(NULL), fTrackIDArray(NULL), fSttParameters(NULL), fFtsParameters(NULL), fMinimumNofSttParalHits(3), fMinimumNofSttSkewHits(3), fMaximumNofSttHits(1000), fMinimumNofFtsHits(6), fAccountForMultiHits(kTRUE) {
  SetDetectorsToStudy(detectorsToStudy);
  sprintf(fFtsBranch,"FTSHit");
  sprintf(fSttBranch,"STTHit");
  sprintf(fMvdPixelBranch,"MVDHitsPixel");
  sprintf(fMvdStripBranch,"MVDHitsStrip");
  sprintf(fInputTrackBranch,"Track");
  sprintf(fInputTrackIDBranch,"TrackID");
  fSttTubeIDToMCPointID.clear();
}



void PndTrkQATask::SetDetectorsToStudy(TString detectorsToStudy){
  if (("STT"!=detectorsToStudy) && ("FTS"!=detectorsToStudy)){
    detectorsToStudy="STT";
    cout << "PndTrkQATask Ctor WARNING: Unknown Detector Mode! Will use " << detectorsToStudy << " instead of your input " << detectorsToStudy << endl;
  }
  fDetectorsToStudy=detectorsToStudy;


}

// -----   Destructor   ----------------------------------------------------
PndTrkQATask::~PndTrkQATask() {

}
// -------------------------------------------------------------------------





// -----   Public method Init   --------------------------------------------
InitStatus PndTrkQATask::Init() {

  if(fVerbose > 9) cout << "PndTrkQATask::Init() Begin \n";


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
  if(fDetectorsToStudy == "STT") {
    fSttPointArray = (TClonesArray*) ioman->GetObject("STTPoint");
    if (!fSttPointArray ) return kERROR;
    if(fVerbose > 9) cout << "Found STT points\n";
    fSttHitArray = (TClonesArray*) ioman->GetObject(fSttBranch);
    if (!fSttHitArray ) return kERROR;
    if(fVerbose > 9) cout << "Found STT hits\n";
  }

  // FTS
  if(fDetectorsToStudy == "FTS") {
    fFtsPointArray = (TClonesArray*) ioman->GetObject("FTSPoint");
    if (!fFtsPointArray ) return kERROR;
    if(fVerbose > 9) cout << "Found FTS points\n";
    fFtsHitArray = (TClonesArray*) ioman->GetObject(fFtsBranch);
    if (!fFtsHitArray ) return kERROR;
    if(fVerbose > 9) cout << "Found FTS hits\n";
  }

  // MVD
  fMvdPointArray = (TClonesArray*) ioman->GetObject("MVDPoint");
  if (!fMvdPointArray ) return kERROR;
  if(fVerbose > 9) cout << "Found MVD points\n";
  // MVD PIXEL
  fMvdPixelHitArray = (TClonesArray*) ioman->GetObject(fMvdPixelBranch);
  if ( !fMvdPixelHitArray) return kERROR;
  if(fVerbose > 9) cout << "Found MVD Pixel hits\n";
  // MVD STRIP
  fMvdStripHitArray = (TClonesArray*) ioman->GetObject(fMvdStripBranch);
  if ( !fMvdStripHitArray) return kERROR;
  if(fVerbose > 9) cout << "Found MVD Strip hits\n";

  // TRACKS
  fMCTrackArray = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( !fMCTrackArray) return kERROR;
  if(fVerbose > 9) cout << "Found MC Tracks \n";
  fTrackArray = (TClonesArray*) ioman->GetObject(fInputTrackBranch);
  if ( !fTrackArray) return kERROR;
  if(fVerbose > 9) cout << "Found Track Array from algorithm under investigation\n";
  fTrackIDArray = (TClonesArray*) ioman->GetObject(fInputTrackIDBranch);
  if ( !fTrackIDArray) return kERROR;
  if(fVerbose > 9) cout << "Found Track ID \n";


  fIdealTrackCandArray = new TClonesArray("PndTrackCand");
  ioman->Register("IdealTrackCand",  "QA", fIdealTrackCandArray, fPersistence); // CHECK

  // ----------------------------------------   maps of STT tubes
  if(fDetectorsToStudy == "STT") {
    PndSttMapCreator *mapperStt = new PndSttMapCreator(fSttParameters);
    fTubeArrayStt = mapperStt->FillTubeArray();
  }
  // ----------------------------------------------------  end map

  // ----------------------------------------   maps of FTS tubes
  if(fDetectorsToStudy == "FTS") {
    PndFtsMapCreator *mapperFts = new PndFtsMapCreator(fFtsParameters);
    fTubeArrayFts = mapperFts->FillTubeArray();
  }
  // ----------------------------------------------------  end map



  hEfficiency = new TH2F("hEfficiency","efficiency = assigned/nof MC points", 100, 0., 100., 50, 0., 1.1);

  hEfficiencyStt = new TH2F("hEfficiencyStt","STT efficiency = assigned/nof MC points", 100, 0., 100., 50, 0., 1.1);
  hEfficiencySttSkew = new TH2F("hEfficiencySttSkew","STT skew efficiency = assigned/nof MC points", 30, 0., 30., 50, 0., 1.1);
  hEfficiencySttParal = new TH2F("hEfficiencySttParal","STT paral efficiency = assigned/nof MC points", 100, 0., 100., 50, 0., 1.1);

  hEfficiencyFts = new TH2F("hEfficiencyFts","FTS efficiency = assigned/nof MC points", 100, 0., 100., 50, 0., 1.1);
  hEfficiencyFtsSkew = new TH2F("hEfficiencyFtsSkew","FTS skew efficiency = assigned/nof MC points", 30, 0., 30., 50, 0., 1.1);
  hEfficiencyFtsParal = new TH2F("hEfficiencyFtsParal","FTS paral efficiency = assigned/nof MC points", 100, 0., 100., 50, 0., 1.1);

  hEfficiencyMvdPixel = new TH2F("hEfficiencyMvdPixel","MVDPIX efficiency = assigned/nof MC points", 20, 0., 20., 50, 0., 1.1);
  hEfficiencyMvdStrip = new TH2F("hEfficiencyMvdStrip","MVDSTR efficiency = assigned/nof MC points", 20, 0., 20., 50, 0., 1.1);


  hInefficiency = new TH2F("hInefficiency","inefficiency = not assigned/nof MC points ", 100, 0., 100., 50, 0., 1.1);

  hInefficiencyStt = new TH2F("hInefficiencyStt","STT inefficiency = not assigned/nof MC points ", 100, 0., 100., 50, 0., 1.1);
  hInefficiencySttSkew = new TH2F("hInefficiencySttSkew","STT skew inefficiency = not assigned/nof MC points ", 30, 0., 30., 50, 0., 1.1);
  hInefficiencySttParal = new TH2F("hInefficiencySttParal","STT paral inefficiency = not assigned/nof MC points ", 100, 0., 100., 50, 0., 1.1);

  hInefficiencyFts = new TH2F("hInefficiencyFts","FTS inefficiency = not assigned/nof MC points ", 100, 0., 100., 50, 0., 1.1);
  hInefficiencyFtsSkew = new TH2F("hInefficiencyFtsSkew","FTS skew inefficiency = not assigned/nof MC points ", 30, 0., 30., 50, 0., 1.1);
  hInefficiencyFtsParal = new TH2F("hInefficiencyFtsParal","FTS paral inefficiency = not assigned/nof MC points ", 100, 0., 100., 50, 0., 1.1);

  hInefficiencyMvdPixel = new TH2F("hInefficiencyMvdPixel","MVDPIX inefficiency = not assigned/nof MC points ", 20, 0., 20., 50, 0., 1.1);
  hInefficiencyMvdStrip = new TH2F("hInefficiencyMvdStrip","MVDSTR inefficiency = not assigned/nof MC points ", 20, 0., 20., 50, 0., 1.1);

  hContamination = new TH2F("hContamination","contamination = wrongly assigned/nof track hits", 100, 0., 100., 50, 0., 1.1);
  hPurity = new TH2F("hPurity","purity = correctly assigned/nof track hits", 100, 0., 100., 50, 0., 1.1);

  fGoodTrack = 0, fBadTrack = 0, fMCReconstructableTrack = 0, fNotReconstructed = 0, fGhostTrack = 0, fRecoTrack = 0, fIgnoredTrack = 0;


  if(fVerbose > 9) cout << "PndTrkQATask::Init() End \n";
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

  hEfficiencyFts->Write();
  delete hEfficiencyFts;
  hEfficiencyFtsSkew->Write();
  delete hEfficiencyFtsSkew;
  hEfficiencyFtsParal->Write();
  delete hEfficiencyFtsParal;

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

  hInefficiencyFts->Write();
  delete hInefficiencyFts;
  hInefficiencyFtsSkew->Write();
  delete hInefficiencyFtsSkew;
  hInefficiencyFtsParal->Write();
  delete hInefficiencyFtsParal;

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
  if(fDetectorsToStudy == "STT") fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
  if(fDetectorsToStudy == "FTS") fFtsParameters = (PndGeoFtsPar*) rtdb->getContainer("PndGeoFtsPar");
}

// -------------------------------------------------------------------------

void PndTrkQATask::ReInitTask() {

  // init to 0
  fNFtsPoints   = 0;
  fNSttPoints   = 0;
  fNMvdPoints   = 0;
  fNMCPoints    = 0;
  fNFtsHits     = 0;
  fNSttHits     = 0;
  fNMvdPixHits  = 0;
  fNMvdStrHits  = 0;
  fNHits        = 0;
  fNMCTracks    = 0;

  // mvd
  fNMvdPoints = fMvdPointArray->GetEntriesFast();
  fNMvdPixHits = fMvdPixelHitArray->GetEntriesFast();
  fNMvdStrHits = fMvdStripHitArray->GetEntriesFast();
  if(fVerbose > 9) {
    cout << "Found " << fNMvdPoints << " MVD points\n";
    cout << "Found " << fNMvdPixHits << " MVD Pixel hits\n";
    cout << "Found " << fNMvdStrHits << " MVD Strip hits\n";
  }

  // stt
  if(fDetectorsToStudy == "STT") {
    fNSttPoints = fSttPointArray->GetEntriesFast();
    fNSttHits = fSttHitArray->GetEntriesFast();
    if(fVerbose > 9) {
      cout << "Found " << fNSttPoints << " STT points\n";
      cout << "Found " << fNSttHits << " STT hits\n";
    }
  }

  // fts
  if(fDetectorsToStudy == "FTS") {
    fNFtsPoints = fFtsPointArray->GetEntriesFast();
    fNFtsHits = fFtsHitArray->GetEntriesFast();
    if(fVerbose > 9) {
      cout << "Found " << fNFtsPoints << " FTS points\n";
      cout << "Found " << fNFtsPoints << " FTS hits\n";
    }
  }

  fNMCPoints = fNFtsPoints + fNSttPoints + fNMvdPoints;
  fNHits = fNFtsHits + fNSttHits + fNMvdPixHits + fNMvdStrHits;

  // mc tracks
  fNMCTracks = fMCTrackArray->GetEntriesFast();

  // reset counters to 0
  fThisGoodTrack = 0, fThisBadTrack = 0, fThisMCReconstructableTrack = 0, fThisNotReconstructed = 0, fThisGhostTrack = 0, fThisRecoTrack = 0, fThisIgnoredTrack = 0;

  fSttTubeIDToMCPointID.clear();
}


void PndTrkQATask::Exec(Option_t* opt) {
  fIdealTrackCandArray->Delete();
  ReInitTask();
  // CHECK delete this ---
  //   if(fEventCounter == 525) {
  //     fEventCounter++;
  //     return;
  //   }
  //   // -----
  fEventCounter++;

  int nofdoublehits = FillTubeIDMap();
  if(fVerbose > 1)  cout << "PndTrkQATask: number of double hit tubes in reconstructable tracks: " << nofdoublehits << endl;

  Bool_t idealtrackfinder = IdealTrackFinding();
  fMCReconstructableTrack += fIdealTrackCandArray->GetEntriesFast();
  fThisMCReconstructableTrack += fIdealTrackCandArray->GetEntriesFast();

  MapMCToReco();

  // iterator over mc track candidates from ideal pattern recognition
  //	 fMC2RecoMap.second is a vector of indices of associated tracks found in tracking algorithm under investigation
  std::map<Int_t, std::vector< Int_t > >::iterator it = fMC2RecoMap.begin();


  //  // DELETE THIS ///////////////////////////////////////
  //  cout << "@@@@@@@@@@@@@@@@@@@@ RECO MAP" << endl;
  //  cout << "MAPPA " << fMC2RecoMap.size() << endl;
  //  while(it != fMC2RecoMap.end()) {
  //     // take a MC track
  //    Int_t imctrack = it->first;
  //    // take reco associated
  //    std::vector<Int_t> asso = it->second;
  //    // ... PRINT .............
  //    if(asso.at(0) != -1)  cout << "MCtrack " << imctrack << " has " << asso.size() << " associated track(s):";
  //    else cout << "MCtrack " << imctrack << " has 0 associated tracks";

  //    for(Int_t itrk = 0; itrk < asso.size(); itrk++ ) cout << " " << asso.at(itrk);
  //    cout << endl;
  //    // ........................
  //    it++;
  //  }
  //  cout << "@@@@@@@@@@@@@@@@@@@@ RECO MAP" << endl;

  //  // /////////////////////////////////////////////////////




  it = fMC2RecoMap.begin();


  while(it != fMC2RecoMap.end()) {

    // take a MC track
    Int_t imctrack = it->first;
    // take vector of all the reco associated (from tracking algorithm under investigation)
    std::vector<Int_t> asso = it->second;


    // how many associated?
    if(asso.at(0) == -1) {
      fNotReconstructed++;
      fThisNotReconstructed++;

      // take the MC info
      PndTrackCand *mctrkcand = (PndTrackCand*) fIdealTrackCandArray->At(imctrack);
      if(!mctrkcand) continue;

      Int_t nofmctrackpoints   = mctrkcand->GetNHits();
      Int_t nofmctracksttpoints = mctrkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId(fSttBranch));
      cout << "TRACK " << " "<<  nofmctrackpoints << " " << nofmctracksttpoints << endl;



      it++;
      continue;
    }

    // how many ghosts?
    if(asso.size() > 1) {
      fGhostTrack += asso.size() - 1;
      fThisGhostTrack += asso.size() - 1;
    }

    // take the MC info
    PndTrackCand *mctrkcand = (PndTrackCand*) fIdealTrackCandArray->At(imctrack);
    if(!mctrkcand) continue;
    Int_t mctrackID = mctrkcand->getMcTrackId();

    Int_t nofmctrackpoints   = mctrkcand->GetNHits();
    Int_t nofmctracksttpoints = mctrkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId(fSttBranch));
    Int_t nofmctracksttskewpoints = 0, nofmctracksttparalpoints = 0;
    Int_t nofmctrackftspoints = mctrkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId(fFtsBranch));
    Int_t nofmctrackftsskewpoints = 0, nofmctrackftsparalpoints = 0;
    Int_t nofmctrackmvdpixpoints = mctrkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId(fMvdPixelBranch));
    Int_t nofmctrackmvdstrpoints = mctrkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId(fMvdStripBranch));

    // this loop counts skewed (--> parallel) STT/FTS hits
    for(Int_t ihit = 0; ihit < nofmctrackpoints; ihit++) {
      PndTrackCandHit mccandhit = mctrkcand->GetSortedHit(ihit);
      Int_t hitID1 = mccandhit.GetHitId();
      Int_t detID1 = mccandhit.GetDetId();


      //       if(!fUseMVDPixHits && detID1 == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) continue;
      //       if(!fUseMVDStrHits && detID1 == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) continue;

      if(!fUseSTTHits && detID1 == FairRootManager::Instance()->GetBranchId(fSttBranch)) continue;
      if(!fUseFTSHits && detID1 == FairRootManager::Instance()->GetBranchId(fFtsBranch)) continue;


      // count skew STT ---------------------------
      if(detID1 == FairRootManager::Instance()->GetBranchId(fSttBranch)) {
	PndSttHit *stthit = (PndSttHit*) fSttHitArray->At(hitID1);
	Int_t tubeID = stthit->GetTubeID();
	PndSttTube *tube = (PndSttTube*) fTubeArrayStt->At(tubeID);
	if(tube->IsSkew()) nofmctracksttskewpoints++;
      }
      //       nofmctracksttparalpoints = nofmctracksttpoints - nofmctracksttskewpoints;
      // ---------------------------------------

      // count skew FTS ---------------------------
      else if(detID1 == FairRootManager::Instance()->GetBranchId(fFtsBranch)) {
	PndFtsHit *ftshit = (PndFtsHit*) fFtsHitArray->At(hitID1);
	Int_t tubeID = ftshit->GetTubeID();
	PndFtsTube *tube = (PndFtsTube*) fTubeArrayFts->At(tubeID);
	if(tube->IsSkew()) nofmctrackftsskewpoints++;
      }
      //       nofmctrackftsparalpoints = nofmctrackftspoints - nofmctrackftsskewpoints;
      // ---------------------------------------

      else continue;
    }
    nofmctracksttparalpoints = nofmctracksttpoints - nofmctracksttskewpoints;
    nofmctrackftsparalpoints = nofmctrackftspoints - nofmctrackftsskewpoints;

    // switch some detectors off ............
    if(!fUseMVDPixHits) {
      nofmctrackpoints -= nofmctrackmvdpixpoints;
      nofmctrackmvdpixpoints = 0;
    }
    if(!fUseMVDStrHits) {
      nofmctrackpoints -= nofmctrackmvdstrpoints;
      nofmctrackmvdstrpoints = 0;
    }
    if(!fUseSTTHits) {
      nofmctrackpoints -= nofmctracksttpoints;
      nofmctracksttpoints = 0;
    }
    else if(!fUseSTTSkewHits) {                                   // CHECK
      nofmctrackpoints -= nofmctracksttskewpoints;            // CHECK
      nofmctracksttskewpoints = 0;                            // CHECK
      nofmctracksttpoints -= nofmctracksttskewpoints;         // CHECK
    }
    if(!fUseFTSHits) {
      nofmctrackpoints -= nofmctrackftspoints;
      nofmctrackftspoints = 0;
    }
    else if(!fUseFTSSkewHits) {                                   // CHECK
      nofmctrackpoints -= nofmctrackftsskewpoints;            // CHECK
      nofmctrackftsskewpoints = 0;                            // CHECK
      nofmctrackftspoints -= nofmctrackftsskewpoints;         // CHECK
    }

    Int_t nAssigned = 0, nNotAssigned = 0, nWrong = 0;
    Int_t nAssignedStt = 0, nNotAssignedStt = 0, nWrongStt = 0;
    Int_t nAssignedSttSkew = 0, nNotAssignedSttSkew = 0, nWrongSttSkew = 0;
    Int_t nAssignedSttParal = 0, nNotAssignedSttParal = 0, nWrongSttParal = 0;
    Int_t nAssignedFts = 0, nNotAssignedFts = 0, nWrongFts = 0;
    Int_t nAssignedFtsSkew = 0, nNotAssignedFtsSkew = 0, nWrongFtsSkew = 0;
    Int_t nAssignedFtsParal = 0, nNotAssignedFtsParal = 0, nWrongFtsParal = 0;
    Int_t nAssignedMvdPixel = 0, nNotAssignedMvdPixel = 0, nWrongMvdPixel = 0;
    Int_t nAssignedMvdStrip = 0, nNotAssignedMvdStrip = 0, nWrongMvdStrip = 0;
    Int_t nofrecotrackpoints = 0;
    Int_t nofrecotracksttpoints = 0;
    Int_t nofrecotracksttskewpoints = 0, nofrecotracksttparalpoints = 0;
    Int_t nofrecotrackftspoints = 0;
    Int_t nofrecotrackftsskewpoints = 0, nofrecotrackftsparalpoints = 0;
    Int_t nofrecotrackmvdpixpoints = 0;
    Int_t nofrecotrackmvdstrpoints = 0;
    Int_t jrecotrack = 0;


    // begin loop over all associated tracks (found with tracking algorithm under investigation) to the mc track (found by ideal tracking)
    for(Int_t itrk = 0; itrk < asso.size(); itrk++) {

      Int_t nTmpAssigned = 0, nTmpNotAssigned = 0, nTmpWrong = 0;
      Int_t nTmpAssignedStt = 0, nTmpNotAssignedStt = 0, nTmpWrongStt = 0;
      Int_t nTmpAssignedSttSkew = 0, nTmpNotAssignedSttSkew = 0, nTmpWrongSttSkew = 0;
      Int_t nTmpAssignedSttParal = 0, nTmpNotAssignedSttParal = 0, nTmpWrongSttParal = 0;
      Int_t nTmpAssignedFts = 0, nTmpNotAssignedFts = 0, nTmpWrongFts = 0;
      Int_t nTmpAssignedFtsSkew = 0, nTmpNotAssignedFtsSkew = 0, nTmpWrongFtsSkew = 0;
      Int_t nTmpAssignedFtsParal = 0, nTmpNotAssignedFtsParal = 0, nTmpWrongFtsParal = 0;
      Int_t nTmpAssignedMvdPixel = 0, nTmpNotAssignedMvdPixel = 0, nTmpWrongMvdPixel = 0;
      Int_t nTmpAssignedMvdStrip = 0, nTmpNotAssignedMvdStrip = 0, nTmpWrongMvdStrip = 0;



      Int_t jtrk = asso.at(itrk);
      PndTrack *trk = (PndTrack*) fTrackArray->At(jtrk);
      if(!trk) continue;
      PndTrackID *trkID = (PndTrackID*) fTrackIDArray->At(jtrk);
      if(!trkID) continue;

      Int_t recotrackID = trkID->GetCorrTrackID();
      if(recotrackID != mctrackID) cout << "ERROR " << endl;

      PndTrackCand *trkcand = trk->GetTrackCandPtr();
      if(!trkcand) continue;

      Int_t noftmprecotrackpoints = trkcand->GetNHits();
      Int_t noftmprecotracksttpoints = trkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId(fSttBranch));
      Int_t noftmprecotracksttskewpoints = 0, noftmprecotracksttparalpoints = 0;
      Int_t noftmprecotrackftspoints = trkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId(fFtsBranch));
      Int_t noftmprecotrackftsskewpoints = 0, noftmprecotrackftsparalpoints = 0;
      Int_t noftmprecotrackmvdpixpoints = trkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId(fMvdPixelBranch));
      Int_t noftmprecotrackmvdstrpoints = trkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId(fMvdStripBranch));

      //     //
      //      cout << "POINTS MC " << endl;
      //      cout << nofmctrackpoints << " " << nofmctracksttpoints << " " << nofmctrackmvdpixpoints << " " << nofmctrackmvdstrpoints << " " << nofmctrackftspoints << endl;
      //       cout << "POINTS RECO " << endl;
      //       cout << noftmprecotrackpoints << " " << noftmprecotracksttpoints << " " << noftmprecotrackmvdpixpoints << " " << noftmprecotrackmvdstrpoints << " " << noftmprecotracksttpoints << endl;

      // loop over reco hits of track found by tracking algorithm under investigation ---------------------------
      for(Int_t ihit = 0; ihit < noftmprecotrackpoints; ihit++) {
	PndTrackCandHit candhit = trkcand->GetSortedHit(ihit);
	Int_t hitID = candhit.GetHitId();
	Int_t detID = candhit.GetDetId();
	FairHit *hit = NULL;
	if(detID == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) {
	  if(!fUseMVDPixHits) continue;
	  hit = (PndSdsHit*) fMvdPixelHitArray->At(hitID);
	  if(hit->GetRefIndex() == -1) {
	    nTmpWrongMvdPixel++;
	    nTmpWrong++;
	  }
	  else {
	    PndSdsMCPoint *pnt = (PndSdsMCPoint*) fMvdPointArray->At(hit->GetRefIndex());
	    if(pnt->GetTrackID() == mctrackID) {
	      nTmpAssignedMvdPixel++;
	      nTmpAssigned++;
	      //	      cout << "ADDED pix " << hitID << " " << detID << " " << hit->GetRefIndex() << endl;
	    }
	    else {
	      nTmpWrongMvdPixel++;
	      nTmpWrong++;
	    }
	  }
	}
	else if(detID == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) {
	  if(!fUseMVDStrHits) continue;
	  hit = (PndSdsHit*) fMvdStripHitArray->At(hitID);
	  if(hit->GetRefIndex() == -1) {
	    nTmpWrongMvdStrip++;
	    nTmpWrong++;
	  }
	  else {
	    PndSdsMCPoint *pnt = (PndSdsMCPoint*) fMvdPointArray->At(hit->GetRefIndex());
	    if(pnt->GetTrackID() == mctrackID) {
	      nTmpAssignedMvdStrip++;
	      nTmpAssigned++;
	      //	      cout << "ADDED str " << hitID << " " << detID << " " << hit->GetRefIndex() << endl;
	    }
	    else  {
	      nTmpWrongMvdStrip++;
	      nTmpWrong++;
	    }
	  }
	}
	else if(detID == FairRootManager::Instance()->GetBranchId(fSttBranch)) {

	  hit = (PndSttHit*) fSttHitArray->At(hitID);
	  if(fVerbose > 9) {
		  cout << "hitID = " << hitID << endl;
		  cout << "hit = " << hit << endl;
	  }
	  Int_t tubeID = ((PndSttHit*) hit)->GetTubeID();
	  if(fVerbose > 9) {
		  cout << "tubeID = " << tubeID << endl;
	  }
      PndSttTube *tube = (PndSttTube*) fTubeArrayStt->At(tubeID);
	  //	  if(IsTubeMultiHit(tubeID)) cout << "IsTubeMultiHit(tubeID) " << tubeID << " " << IsTubeMultiHit(tubeID) << endl;
 	  if(fAccountForMultiHits == kFALSE && IsTubeMultiHit(tubeID) == kTRUE) {
	    Bool_t isgoodhit = CheckIfTrackHitTube(mctrackID, tubeID);
	    if(isgoodhit == kFALSE) {
	      nTmpWrongStt++;
	      nTmpWrong++;
	      if(tube->IsSkew()) nTmpWrongSttSkew++;
	      else nTmpWrongSttParal++;
	    }
	    else {
	      nTmpAssignedStt++;
	      nTmpAssigned++;

	      //	      cout << "ADDED stt " << hitID << " " << detID << " " << hit->GetRefIndex() << " " << tubeID << " " << tube->IsSkew() << endl;
	      if(tube->IsSkew()) nTmpAssignedSttSkew++;
	      else nTmpAssignedSttParal++;

	    }
	  }
	  else {
	    if(hit->GetRefIndex() == -1) {
	      nTmpWrongStt++;
	      nTmpWrong++;
	      if(tube->IsSkew()) nTmpWrongSttSkew++;
	      else nTmpWrongSttParal++;
	    }
	    else {
	      //	      cout << "checking point " << hit->GetRefIndex() << endl;
	      PndSttPoint *pnt = (PndSttPoint*) fSttPointArray->At(hit->GetRefIndex());
	      if(tube->IsSkew()) noftmprecotracksttskewpoints++;
	      else noftmprecotracksttparalpoints++;


	      if(pnt->GetTrackID() == mctrackID) {
		nTmpAssignedStt++;
		nTmpAssigned++;
		//		cout << "ADDED stt " << hitID << " " << detID << " " << hit->GetRefIndex() << " " << tubeID << " " << tube->IsSkew() << endl;
		if(tube->IsSkew()) nTmpAssignedSttSkew++;
		else nTmpAssignedSttParal++;
	      }
	      else {
		nTmpWrongStt++;
		nTmpWrong++;
		if(tube->IsSkew()) nTmpWrongSttSkew++;
		else nTmpWrongSttParal++;
	      }
	    }
	  }
	}
	else if(detID == FairRootManager::Instance()->GetBranchId(fFtsBranch)) {

	  hit = (PndFtsHit*) fFtsHitArray->At(hitID);
	  Int_t tubeID = ((PndFtsHit*) hit)->GetTubeID();
	  PndFtsTube *tube = (PndFtsTube*) fTubeArrayFts->At(tubeID);
	  if(hit->GetRefIndex() == -1) {
	    nTmpWrongFts++;
	    nTmpWrong++;
	    if(tube->IsSkew()) nTmpWrongFtsSkew++;
	    else nTmpWrongFtsParal++;
	  }
	  else {
	    PndFtsPoint *pnt = (PndFtsPoint*) fFtsPointArray->At(hit->GetRefIndex());
	    if(tube->IsSkew()) noftmprecotrackftsskewpoints++;
	    else noftmprecotrackftsparalpoints++;

	    if(pnt->GetTrackID() == mctrackID) {
	      nTmpAssignedFts++;
	      nTmpAssigned++;
	      if(tube->IsSkew()) nTmpAssignedFtsSkew++;
	      else nTmpAssignedFtsParal++;
	    }
	    else {
	      nTmpWrongFts++;
	      nTmpWrong++;
	      if(tube->IsSkew()) nTmpWrongFtsSkew++;
	      else nTmpWrongFtsParal++;
	    }
	  }
	}

      }

      nTmpNotAssigned = nofmctrackpoints - nTmpAssigned;
      //      cout << "TMP " << nofmctrackpoints << " " << nTmpNotAssigned << " " << nTmpAssigned << endl;

      nTmpNotAssignedMvdPixel = nofmctrackmvdpixpoints - nTmpAssignedMvdPixel;
      nTmpNotAssignedMvdStrip = nofmctrackmvdstrpoints - nTmpAssignedMvdStrip;
      nTmpNotAssignedStt = nofmctracksttpoints - nTmpAssignedStt;
      nTmpNotAssignedSttSkew = nofmctracksttskewpoints - nTmpAssignedSttSkew;
      nTmpNotAssignedSttParal = nofmctracksttparalpoints - nTmpAssignedSttParal;
      nTmpNotAssignedFts = nofmctrackftspoints - nTmpAssignedFts;
      nTmpNotAssignedFtsSkew = nofmctrackftsskewpoints - nTmpAssignedFtsSkew;
      nTmpNotAssignedFtsParal = nofmctrackftsparalpoints - nTmpAssignedFtsParal;

      // Pick the best reco track (from tracking algorithm under investigation) to a given mc track cand (found by ideal tracking)
      // PICK THIS if it has an higher number of assigned hits
      //                     OR
      // if that number is equal, if it is cleaner

      if((nTmpAssigned > nAssigned) || (nTmpAssigned == nAssigned && nTmpWrong < nWrong)) {


	nAssigned    = nTmpAssigned;
	nNotAssigned = nTmpNotAssigned;
	nWrong       = nTmpWrong;
	nAssignedStt     = nTmpAssignedStt;
	nNotAssignedStt  = nTmpNotAssignedStt;
	nWrongStt        = nTmpWrongStt;
	nAssignedSttSkew    = nTmpAssignedSttSkew;
	nNotAssignedSttSkew = nTmpNotAssignedSttSkew;
	nWrongSttSkew       = nTmpWrongSttSkew;
	nAssignedSttParal    = nTmpAssignedSttParal;
	nNotAssignedSttParal = nTmpNotAssignedSttParal;
	nWrongSttParal       = nTmpWrongSttParal;
	nAssignedFts     = nTmpAssignedFts;
	nNotAssignedFts  = nTmpNotAssignedFts;
	nWrongFts        = nTmpWrongFts;
	nAssignedFtsSkew    = nTmpAssignedFtsSkew;
	nNotAssignedFtsSkew = nTmpNotAssignedFtsSkew;
	nWrongFtsSkew       = nTmpWrongFtsSkew;
	nAssignedFtsParal    = nTmpAssignedFtsParal;
	nNotAssignedFtsParal = nTmpNotAssignedFtsParal;
	nWrongFtsParal       = nTmpWrongFtsParal;
	nAssignedMvdPixel    = nTmpAssignedMvdPixel;
	nNotAssignedMvdPixel = nTmpNotAssignedMvdPixel;
	nWrongMvdPixel       = nTmpWrongMvdPixel;
	nAssignedMvdStrip    = nTmpAssignedMvdStrip;
	nNotAssignedMvdStrip = nTmpNotAssignedMvdStrip;
	nWrongMvdStrip       = nTmpWrongMvdStrip;
	nofrecotrackpoints         = noftmprecotrackpoints;
	nofrecotracksttpoints      = noftmprecotracksttpoints;
	nofrecotracksttskewpoints  = noftmprecotracksttskewpoints;
	nofrecotracksttparalpoints = noftmprecotracksttparalpoints;
	nofrecotrackftspoints      = noftmprecotrackftspoints;
	nofrecotrackftsskewpoints  = noftmprecotrackftsskewpoints;
	nofrecotrackftsparalpoints = noftmprecotrackftsparalpoints;
	nofrecotrackmvdpixpoints   = noftmprecotrackmvdpixpoints;
	nofrecotrackmvdstrpoints   = noftmprecotrackmvdstrpoints;
	jrecotrack = jtrk;
      }

    }



    if(fVerbose > 1) {
      cout << "POINTS MC (mc trk, stt, mvd pix, mvd str, fts)" << endl;
      cout << nofmctrackpoints << " " << nofmctracksttpoints << " " << nofmctrackmvdpixpoints << " " << nofmctrackmvdstrpoints << " " << nofmctrackftspoints<< endl;
      cout << "POINTS RECO (trk alg, stt, mvd pix, mvd str, fts)" << endl;
      cout << nofrecotrackpoints << " " << nofrecotracksttpoints << " " << nofrecotrackmvdpixpoints << " " << nofrecotrackmvdstrpoints << " " << nofrecotrackftspoints << endl;
    }
    // EFFICIENCY one reco for one mc
    if(nofmctrackpoints > 0) {
      if(((Double_t) nAssigned/nofmctrackpoints) > 1) {
      cout << "EFFICIENCY LOOKS STRANGE" << endl;;
      cout << "MC " << nofmctrackpoints << " " << nofmctrackmvdpixpoints << " " << nofmctrackmvdstrpoints << " " << nofmctracksttpoints << " " <<  nofmctracksttparalpoints << " " <<  nofmctracksttskewpoints << " " << nofmctrackftspoints << endl;
      cout << "RECO " << nofrecotrackpoints << " " << nofrecotrackmvdpixpoints << " " << nofrecotrackmvdstrpoints << " " << nofrecotracksttpoints << " " <<  nofrecotracksttparalpoints << " " <<  nofrecotracksttskewpoints << endl;
      cout << "assigned : "<< nAssigned << " " << nAssignedMvdPixel << " " << nAssignedMvdStrip << " " << nAssignedStt << " " << nAssignedSttParal << " " << nAssignedSttSkew << endl;
   cout << "not assigned : "<< nNotAssigned << " " << nNotAssignedMvdPixel << " " << nNotAssignedMvdStrip << " " << nNotAssignedStt << " " << nNotAssignedSttParal << " " << nNotAssignedSttSkew << endl;
     cout << "wrong : "<< nWrong << " " << nWrongMvdPixel << " " << nWrongMvdStrip << " " << nWrongStt << " " << nWrongSttParal << " " << nWrongSttSkew << endl;
    }

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

    if(nofmctrackftspoints > 0) {
      hEfficiencyFts->Fill(nofmctrackftspoints, (Double_t) nAssignedFts/nofmctrackftspoints);
      hInefficiencyFts->Fill(nofmctrackftspoints, (Double_t) nNotAssignedFts/nofmctrackftspoints);
    }
    if(nofmctrackftsskewpoints > 0) {
      hEfficiencyFtsSkew->Fill(nofmctrackftsskewpoints, (Double_t) nAssignedFtsSkew/nofmctrackftsskewpoints);
      hInefficiencyFtsSkew->Fill(nofmctrackftsskewpoints, (Double_t) nNotAssignedFtsSkew/nofmctrackftsskewpoints);
    }

    if(nofmctrackftsparalpoints > 0) {
      hEfficiencyFtsParal->Fill(nofmctrackftsparalpoints, (Double_t) nAssignedFtsParal/nofmctrackftsparalpoints);
      hInefficiencyFtsParal->Fill(nofmctrackftsparalpoints, (Double_t) nNotAssignedFtsParal/nofmctrackftsparalpoints);
    }

    if(nofmctrackmvdpixpoints > 0) {
      hEfficiencyMvdPixel->Fill(nofmctrackmvdpixpoints, (Double_t) nAssignedMvdPixel/nofmctrackmvdpixpoints);
      hInefficiencyMvdPixel->Fill(nofmctrackmvdpixpoints, (Double_t) nNotAssignedMvdPixel/nofmctrackmvdpixpoints);
    }

    if(nofmctrackmvdstrpoints > 0) {
      hEfficiencyMvdStrip->Fill(nofmctrackmvdstrpoints, (Double_t) nAssignedMvdStrip/nofmctrackmvdstrpoints);
      hInefficiencyMvdStrip->Fill(nofmctrackmvdstrpoints, (Double_t) nNotAssignedMvdStrip/nofmctrackmvdstrpoints);
    }


    // CONTAMINATION/PURITY one reco for one mc
    if(nofrecotrackpoints > 0) {
      hContamination->Fill(nofrecotrackpoints, (Double_t) nWrong/nofrecotrackpoints);
      hPurity->Fill(nofrecotrackpoints, (Double_t) nAssigned/nofrecotrackpoints);
    }

    // 	hContaminationStt->Fill(nofrecotracksttpoints, (Double_t) nWrongStt/nofrecotracksttpoints);
    // 	hContaminationSttSkew  ->Fill(nofrecotracksttskewpoints, (Double_t) nWrongSttSkew  /nofrecotracksttskewpoints);
    // 	hContaminationSttParal->Fill(nofrecotracksttparalpoints, (Double_t) nWrongSttParal/nofrecotracksttparalpoints);
    //  hContaminationFts->Fill(nofrecotrackftspoints, (Double_t) nWrongFts/nofrecotrackftspoints);
    //  hContaminationFtsSkew  ->Fill(nofrecotrackftsskewpoints, (Double_t) nWrongFtsSkew  /nofrecotrackftsskewpoints);
    //  hContaminationFtsParal->Fill(nofrecotrackftsparalpoints, (Double_t) nWrongFtsParal/nofrecotrackftsparalpoints);
    // 	hContaminationMvdPixel->Fill(nofrecotrackmvdpixpoints, (Double_t) nWrongMvdPixel/nofrecotrackmvdpixpoints);
    // 	hContaminationMvdStrip->Fill(nofrecotrackmvdstrpoints, (Double_t) nWrongMvdStrip/nofrecotrackmvdstrpoints);



    if(fVerbose > 1) {

      cout << "TRACK " << jrecotrack << " MCTRACK " << imctrack << " " << endl;
      cout << "recototal " << nofrecotrackpoints << " mctotal " << nofmctrackpoints << endl;
      cout << "assigned " <<  nAssigned <<  " not assigned " << nNotAssigned << " wrong " << nWrong << endl;
    }


    if((nAssigned + nNotAssigned) != nofmctrackpoints) cout << "ERROR 1" << endl;
    if((nAssigned + nWrong) != nofrecotrackpoints) cout << "ERROR 2" << endl;
    if((nAssignedSttSkew + nAssignedSttParal) != nAssignedStt) cout << "ERROR 3" << endl;
    if((nAssignedFtsSkew + nAssignedFtsParal) != nAssignedFts) cout << "ERROR 4" << endl;

    // CHECK for now:
    // a track is good if it has more than 80% of mc points assigned to it


    if(((Double_t) nAssigned/nofmctrackpoints) > 0.8){
      fGoodTrack++;
      fThisGoodTrack++;
    }
    else {
      fBadTrack++;
      fThisBadTrack++;
    }

    it++;
  }



  // if the nof reconstructed tracks is > assigned + ghosts it means
  // that a track not considered reconstructable has been reconstructed
  if(fThisRecoTrack > fThisGoodTrack + fThisGhostTrack) {
    if(fVerbose > 0) cout << "fThisRecoTrack " << fThisRecoTrack << " fThisMCReconstructableTrack " << fThisMCReconstructableTrack << " fThisGoodTrack " <<  fThisGoodTrack << " fThisGhostTrack " << fThisGhostTrack << endl;
    fThisIgnoredTrack += (fThisRecoTrack - (fThisGoodTrack + fThisGhostTrack));
    fIgnoredTrack += (fThisRecoTrack - (fThisGoodTrack + fThisGhostTrack));
  }



  // CHECK for now:
  // fMCReconstructableTrack are the mc tracks which satisfy the criterion in IsMcTrackAcceptable (at least fMinimumNofXXXHits)

  if(fVerbose > 0) {
    if(fThisRecoTrack == 0 || fThisMCReconstructableTrack == 0){
      cout << "#### NOW: Recontructed Tracks " << fThisRecoTrack << " MC Reconstructable Tracks " << fThisMCReconstructableTrack << endl;
    }
    else{
      cout << "#### NOW: GOOD = "<< 100. *  fThisGoodTrack/fThisMCReconstructableTrack << "%, BAD = " << 100. *  fThisBadTrack/fThisMCReconstructableTrack << "%, MISSED = " << 100. *  fThisNotReconstructed/fThisMCReconstructableTrack << "%, GHOSTS " << 100. *  fThisGhostTrack/fThisRecoTrack << "%. [IGNORED " << 100. * fThisIgnoredTrack/fThisRecoTrack << "%]" << endl;
    }
    if(fRecoTrack == 0 || fMCReconstructableTrack == 0){
      cout << "TOTAL: Recontructed Tracks " << fRecoTrack << " MC Reconstructable Tracks " << fMCReconstructableTrack << endl;
    }
    else{
      cout << "======== AFTER THIS EVENT: GOOD = "<< 100. *  fGoodTrack/fMCReconstructableTrack << "%, BAD = " << 100. *  fBadTrack/fMCReconstructableTrack << "%, MISSED = " << 100. *  fNotReconstructed/fMCReconstructableTrack << "%, GHOSTS " << 100. *  fGhostTrack/fRecoTrack << "%. [IGNORED " << 100. * fIgnoredTrack/fRecoTrack << "%]" << endl;
    }
  }
}


/**
   Int_t PndTrkQATask::CheckIfPresent(Int_t trackid) {
   for(Int_t itrk = 0; itrk < fNofMCTrack; itrk++) {
   if(fMCTracks[itrk][0] == trackid) return itrk;
   }
   return -1;
   }
**/



Bool_t PndTrkQATask::IdealTrackFinding() {
  // Fills fIdealTrackCandArray with ideal tracking using MC truth information
  // determines which track is reconstructable for the detector(s) under investigation

  fIdealTrackCandArray->Delete();
  //  cout << "IDEAL" << endl;
  // Initialise control counters
  Int_t nNoMCTrack    = 0;
  Int_t nNoTrack      = 0;

  //   Int_t nFtsPoints   = 0;
  //   Int_t nSttPoints   = 0;
  //   Int_t nMvdPoints   = 0;
  //   Int_t nMCPoints    = 0;
  //   nFtsPoints = fFtsPointArray->GetEntriesFast();
  //   if(fDetectorsToStudy == "STT") nSttPoints = fSttPointArray->GetEntriesFast();
  //   nMvdPoints = fMvdPointArray->GetEntriesFast();
  //   nMCPoints = nFtsPoints + nSttPoints + nMvdPoints;


  //   Int_t nFtsHits     = 0;
  //   Int_t nSttHits     = 0;
  //   Int_t nMvdPixHits  = 0;
  //   Int_t nMvdStrHits  = 0;
  //   Int_t nHits        = 0;
  //   nFtsHits = fFtsHitArray->GetEntriesFast();
  //    if(fDetectorsToStudy == "STT")  nSttHits = fSttHitArray->GetEntriesFast();
  //   nMvdPixHits = fMvdPixelHitArray->GetEntriesFast();
  //   nMvdStrHits = fMvdStripHitArray->GetEntriesFast();
  //   nHits = nFtsHits + nSttHits + nMvdPixHits + nMvdStrHits;

  // Create pointers to hit and MCPoint
  PndFtsHit*       pMhitFTS = NULL;
  PndSttHit*       pMhitSTT = NULL;
  PndSdsHit*  pMhitPIX = NULL;
  PndSdsHit*  pMhitSTR = NULL;

  FairMCPoint*     pMCpt = NULL;
  PndMCTrack*      pMCtr = NULL;
  PndTrackCand* pTrckCand = NULL;



  // Declare some variables outside the loops
  Int_t ptIndex      = 0;     // MCPoint index
  Int_t mcTrackIndex = 0;     // MCTrack index
  Int_t idealrecoTrackIndex   = 0;     // ideal tracking track index

  // Create array with
  // first index = MCtrack index
  // second index = number of MCPoints
  //   Int_t fNMCTracks = fMCTrackArray->GetEntriesFast();
  Int_t hitMap[fNMCTracks][NOFDETECTORS];
  for (Int_t iMCTrack = 0; iMCTrack < fNMCTracks; iMCTrack++) {
    hitMap[iMCTrack][0] = 0; // MVD
    hitMap[iMCTrack][1] = 0; // STT parallel
    hitMap[iMCTrack][2] = 0; // STT skew
    hitMap[iMCTrack][3] = 0; // FTS
  }

  // loops over mc points, count points in detectors with hitMap
  if(fVerbose > 11) cout << "nMvdPoints = " << fNMvdPoints << endl;
  for (Int_t ipnt = 0; ipnt < fNMvdPoints; ipnt++){
    pMCpt =  (PndSdsMCPoint*) fMvdPointArray->At(ipnt);
    if (!pMCpt){
      if(fVerbose > 9) cout << "Mvd Point " << ipnt << "is skipped because of bad 0 pMCpt" << endl;
      continue;
    }
    mcTrackIndex = pMCpt->GetTrackID();
    hitMap[mcTrackIndex][0]++;
    if(fVerbose > 2)    cout << "MVD ON " << ipnt << " " <<  mcTrackIndex << " " << endl;
  }



  if(fVerbose > 11) cout << "nSttPoints = " << fNSttPoints << endl;
  for (Int_t ipnt = 0; ipnt < fNSttPoints; ipnt++){
    pMCpt =  (PndSttPoint*) fSttPointArray->At(ipnt);
    if (!pMCpt){
      if(fVerbose > 9) cout << "Stt Point " << ipnt << "is skipped because of bad 0 pMCpt" << endl;
      continue;
    }
    mcTrackIndex = pMCpt->GetTrackID();

    Int_t tubeID = ((PndSttPoint*) pMCpt)->GetTubeID();
    PndSttTube *tube = (PndSttTube*) fTubeArrayStt->At(tubeID);
    if(tube->IsParallel() == kTRUE)  hitMap[mcTrackIndex][1]++;
    else hitMap[mcTrackIndex][2]++;
    // -----------------------------------------------

    if(fVerbose > 2)     cout << "STT ON " << ipnt << " " << mcTrackIndex << " " << tubeID << endl;
  }

  if(fVerbose > 11) cout << "fNFtsPoints = " << fNFtsPoints << endl;
  for (Int_t ipnt = 0; ipnt < fNFtsPoints; ipnt++){
    pMCpt =  (PndFtsPoint*) fFtsPointArray->At(ipnt);
    if (!pMCpt){
      if(fVerbose > 9) cout << "Fts Point " << ipnt << "is skipped because of bad 0 pMCpt" << endl;
      continue;
    }

    // CHECK
    // do not count fts hits on skewed straws
    // in station 1+2 OR 5+6 at least 3 non-skewed hits need to be present
    // in addition in stations 3+4 we need at least 2
    // for the line Hough transform to work
    Int_t tubeID = ((PndFtsPoint*) pMCpt)->GetTubeID();
    PndFtsTube *tube = (PndFtsTube*) fTubeArrayFts->At(tubeID);
    if(fVerbose > 11) cout << "tube->GetWireDirection().X(),Y(),Z() = (" << tube->GetWireDirection().X() << " , " << tube->GetWireDirection().Y() << " , " << tube->GetWireDirection().Z() << ")\n";
    if (tube->IsSkew()) continue;
    // TODO Add a check for the station number in which the tube is contained
    // -----------------------------------------------

    mcTrackIndex = pMCpt->GetTrackID();
    hitMap[mcTrackIndex][3]++;
    if(fVerbose > 2) cout << "FTS ON " << mcTrackIndex << " " << endl;
  }
  // --------------------------------------------------------------------

  // Create STL map from MCTrack index to Track index
  map<Int_t, Int_t> correlationMap, trackMap;

  // Create STTTracks for reconstructable MCTracks
  Int_t nMCacc  = 0;         // STT / FTS accepted MCTracks (according to criterion in IsMcTrackAcceptable)
  Int_t nTracks = 0;         // reconstructable MCTracks

  for (Int_t iMCTrack = 0; iMCTrack < fNMCTracks; iMCTrack++){
    pMCtr = (PndMCTrack*) fMCTrackArray->At(iMCTrack);
    if ( ! pMCtr ) 	continue;


    // temporary hack, fix this in monte carlo ....
    if (TDatabasePDG::Instance()->GetParticle(pMCtr->GetPdgCode()) == NULL)	continue;

    if (!(fabs(TDatabasePDG::Instance()->GetParticle(pMCtr->GetPdgCode())->Charge() / 3.0) > 0.)) continue;



    if(fVerbose > 13) {
      cout << "STT: hitMap[iMCTrack][1]=" << hitMap[iMCTrack][1] << " hitMap[iMCTrack][2]=" << hitMap[iMCTrack][2] << endl;
      cout << "FTS: hitMap[iMCTrack][3]=" << hitMap[iMCTrack][3] << endl;
    }


    // Determine whether to accept the track or not (depending on which detector's tracking should be analysed)
    if(!IsMcTrackAcceptable(iMCTrack, hitMap)) continue;


    nMCacc++;

    pTrckCand = new((*fIdealTrackCandArray)[nTracks]) PndTrackCand();
    pTrckCand->setMcTrackId(iMCTrack);

    correlationMap[nTracks] = iMCTrack;
    trackMap[iMCTrack] = nTracks++;

  }
  // -------------------------------------------------------------------

  // Loop over hits. Get corresponding MCPoint and MCTrack index
  for (Int_t iHit = 0; iHit < fNMvdPixHits; iHit++){
    pMhitPIX = (PndSdsHit*) fMvdPixelHitArray->At(iHit);
    if (!pMhitPIX) continue;
    ptIndex = pMhitPIX->GetRefIndex();
    if (ptIndex < 0) continue;
    pMCpt =  (PndSdsMCPoint*) fMvdPointArray->At(ptIndex);
    if (!pMCpt) continue;
    mcTrackIndex = pMCpt->GetTrackID();

    if (mcTrackIndex < 0 || mcTrackIndex > fNMCTracks)
      {
	cout << "-E- MCTrack index out of range. " << mcTrackIndex << " " << fNMCTracks << endl;
	nNoMCTrack++;
	continue;
      }

    if (trackMap.find(mcTrackIndex) == trackMap.end()) continue;

    idealrecoTrackIndex = trackMap[mcTrackIndex];
    pTrckCand = (PndTrackCand*) fIdealTrackCandArray->At(idealrecoTrackIndex);
    if ( ! pTrckCand )
      {
	cout << "-E- No Track pointer. " << iHit << " " << ptIndex
	     << " " << mcTrackIndex << " " << idealrecoTrackIndex << endl;
	nNoTrack++;
	continue;
      }
    Int_t counter = pTrckCand->GetNHits();
    pTrckCand->AddHit(FairRootManager::Instance()->GetBranchId(fMvdPixelBranch), iHit, counter);
    //    cout << "added pixel " << iHit << " " << ptIndex << " " << mcTrackIndex  << endl;
  }


  for (Int_t iHit = 0; iHit < fNMvdStrHits; iHit++)
    {
      pMhitSTR = (PndSdsHit*) fMvdStripHitArray->At(iHit);
      if (!pMhitSTR) continue;
      ptIndex = pMhitSTR->GetRefIndex();
      if (ptIndex < 0) continue;
      pMCpt =  (PndSdsMCPoint*) fMvdPointArray->At(ptIndex);
      if (!pMCpt) continue;
      mcTrackIndex = pMCpt->GetTrackID();

      if (mcTrackIndex < 0 || mcTrackIndex > fNMCTracks)
	{
	  cout << "-E- MCTrack index out of range. " << mcTrackIndex << " " << fNMCTracks << endl;
	  nNoMCTrack++;
	  continue;
	}

      if (trackMap.find(mcTrackIndex) == trackMap.end()) continue;

      idealrecoTrackIndex = trackMap[mcTrackIndex];
      pTrckCand = (PndTrackCand*) fIdealTrackCandArray->At(idealrecoTrackIndex);
      if ( ! pTrckCand )
	{
	  cout << "-E- No Track pointer. " << iHit << " " << ptIndex
	       << " " << mcTrackIndex << " " << idealrecoTrackIndex << endl;
	  nNoTrack++;
	  continue;
	}
      Int_t counter = pTrckCand->GetNHits();
      pTrckCand->AddHit(FairRootManager::Instance()->GetBranchId(fMvdStripBranch), iHit,  counter);
      //    cout << "added strip " << iHit << " " << ptIndex << " " << mcTrackIndex  << endl;

    }

  for (Int_t iHit = 0; iHit < fNSttHits; iHit++)
    {
      pMhitSTT = (PndSttHit*) fSttHitArray->At(iHit);
      if (!pMhitSTT) continue;

      Int_t tubeID = pMhitSTT->GetTubeID();

      ptIndex = pMhitSTT->GetRefIndex();

      // to possibly account for multiple hitsin the same tube
      TArrayI multihits;

      //      cout << " ADD " << iHit << " "  << ptIndex << " " << tubeID << " " << IsTubeMultiHit(tubeID) << endl;

      if(fAccountForMultiHits == kTRUE && IsTubeMultiHit(tubeID) == kTRUE) multihits = GetTubeMultiHits(tubeID);
      else {
	int size =  multihits.GetSize();
	multihits.Set(size + 1);
	multihits.AddAt(ptIndex, size);
      }



      for(int ipnt = 0; ipnt < multihits.GetSize(); ipnt++) {
	ptIndex = multihits.At(ipnt);

	if (ptIndex < 0) continue;
	pMCpt =  (PndSttPoint*) fSttPointArray->At(ptIndex);
	if (!pMCpt) continue;
	mcTrackIndex = pMCpt->GetTrackID();

	if (mcTrackIndex < 0 || mcTrackIndex > fNMCTracks)
	  {
	    cout << "-E- MCTrack index out of range. " << mcTrackIndex << " " << fNMCTracks << endl;
	    nNoMCTrack++;
	    continue;
	  }

	if (trackMap.find(mcTrackIndex) == trackMap.end()) continue;

	idealrecoTrackIndex = trackMap[mcTrackIndex];
	pTrckCand = (PndTrackCand*) fIdealTrackCandArray->At(idealrecoTrackIndex);
	if ( ! pTrckCand )
	  {
	    cout << "-E- No Track pointer. " << iHit << " " << ptIndex
		 << " " << mcTrackIndex << " " << idealrecoTrackIndex << endl;
	    nNoTrack++;
	    continue;
	  }
	Int_t counter = pTrckCand->GetNHits();
	pTrckCand->AddHit(FairRootManager::Instance()->GetBranchId(fSttBranch), iHit,  counter);
	//	cout << "added stt " << iHit << " " << ptIndex << " " << mcTrackIndex  << endl;
      }
    }

  for (Int_t iHit = 0; iHit < fNFtsHits; iHit++)
    {
      pMhitFTS = (PndFtsHit*) fFtsHitArray->At(iHit);

      if (!pMhitFTS) continue;

      ptIndex = pMhitFTS->GetRefIndex();
      if (ptIndex < 0) continue;
      pMCpt =  (PndFtsPoint*) fFtsPointArray->At(ptIndex);
      if (!pMCpt) continue;
      mcTrackIndex = pMCpt->GetTrackID();

      if (mcTrackIndex < 0 || mcTrackIndex > fNMCTracks)
	{
	  cout << "-E- MCTrack index out of range. " << mcTrackIndex << " " << fNMCTracks << endl;
	  nNoMCTrack++;
	  continue;
	}

      if (trackMap.find(mcTrackIndex) == trackMap.end()) continue;

      idealrecoTrackIndex = trackMap[mcTrackIndex];
      pTrckCand = (PndTrackCand*) fIdealTrackCandArray->At(idealrecoTrackIndex);
      if ( ! pTrckCand )
	{
	  cout << "-E- No Track pointer. " << iHit << " " << ptIndex
	       << " " << mcTrackIndex << " " << idealrecoTrackIndex << endl;
	  nNoTrack++;
	  continue;
	}
      Int_t counter = pTrckCand->GetNHits();
      pTrckCand->AddHit(FairRootManager::Instance()->GetBranchId(fFtsBranch), iHit,  counter);

    }

}



void PndTrkQATask::MapMCToReco()
{
  // This fills the fMC2RecoMap which contains
  // first = track index in fIdealTrackCandArray
  // second = vector of tracks found in tracking algorithm under investigation

  // INDEX in IdealTrackCandArray <---> fTrackArray entries
  //  std::map< Int_t, std::vector<Int_t> > fMC2RecoMap;
  if(fVerbose > 2) {
    cout << "mc   tracks " << fIdealTrackCandArray->GetEntriesFast() << endl;
    cout << "reco tracks " << fTrackArray->GetEntriesFast() << endl;
  }

  // maps mc track id to reco track id
  fMC2RecoMap.clear();
  fRecoTrack += fTrackArray->GetEntriesFast();
  fThisRecoTrack += fTrackArray->GetEntriesFast();

  // begin loop over ideal track candidates
  for(Int_t itrk = 0; itrk < fIdealTrackCandArray->GetEntriesFast(); itrk++) {
    PndTrackCand *mctrkcand = (PndTrackCand*) fIdealTrackCandArray->At(itrk);
    if(!mctrkcand) continue;
    Int_t mctrackID = mctrkcand->getMcTrackId();
    std::vector< Int_t > associatedrecotracks;
    //    cout << mctrkcand->getMcTrackId() << " NOF MC ASSOCIATED PNTS " << mctrkcand->GetNHits() << endl;
    // begin loop over track candidates from tracking algorithm which is under investigation
    for(Int_t jtrk = 0; jtrk < fTrackArray->GetEntriesFast(); jtrk++) {
      PndTrack *trk = (PndTrack*) fTrackArray->At(jtrk);
      if(!trk){
	if(fVerbose > 1) cout << "Reco Track " << jtrk << " not found!" << endl;
	continue;
      }

      //			Int_t recotrackIDtest = trk->GetTrackCand().getMcTrackId();
      PndTrackID *trkID = (PndTrackID*) fTrackIDArray->At(jtrk);
      if(!trkID){
	if(fVerbose > 1) cout << "Reco Track " << jtrk << " has no track ID!" << endl;
	continue;
      }

      Int_t recotrackID = trkID->GetCorrTrackID();

      //			if(recotrackID != recotrackIDtest){
      //				if(fVerbose > 9) cout << "recotrackID " << recotrackID << " != recotrackIDtest " << recotrackIDtest << endl;
      //			}

      if(recotrackID != mctrackID){
	if(fVerbose > 9) cout << "recotrackID " << recotrackID << " != mctrackID " << mctrackID << endl;
	continue;
      }
      associatedrecotracks.push_back(jtrk);
    } // end loop over track candidates from tracking algorithm which is under investigation
    if(associatedrecotracks.size() == 0) associatedrecotracks.push_back(-1);

    if(fVerbose > 2) {
      if(associatedrecotracks.at(0) != -1)  cout << "track mc " << itrk << " associated to " << associatedrecotracks.size() << " tracks" << endl;
      else  cout << "track mc " << itrk << " associated to 0 tracks" << endl;
    }

    fMC2RecoMap.insert(std::pair< Int_t, std::vector< Int_t > > (itrk, associatedrecotracks));
  } // end loop over ideal track candidates
}



Bool_t PndTrkQATask::IsMcTrackAcceptable(Int_t iMCTrack, Int_t hitMap[][NOFDETECTORS])
{
  // The selection criterion depends on which detector's tracking performance is investigated
  if ("STT"==fDetectorsToStudy){
    bool hasmorethanminparall = !( hitMap[iMCTrack][1] < fMinimumNofSttParalHits);
    bool hasmorethanminskew = !( hitMap[iMCTrack][2] < fMinimumNofSttSkewHits);
    bool haslessthanmax = (hitMap[iMCTrack][1] + hitMap[iMCTrack][2]) < fMaximumNofSttHits;
    bool fullrequest = hasmorethanminparall && hasmorethanminskew && haslessthanmax;

    if(fVerbose > 13) cout << "STT for " << iMCTrack << " track: hitMap[iMCTrack][1]= " << hitMap[iMCTrack][1] << " hitMap[iMCTrack][2]= " <<  hitMap[iMCTrack][2] << " IsMcTrackAcceptable returns " << fullrequest << endl;
    return fullrequest; // CHECK
  }
  else if ("FTS"==fDetectorsToStudy){
    if(fVerbose > 13) cout << "FTS: hitMap[iMCTrack][3]=" << hitMap[iMCTrack][3] << " IsMcTrackAcceptable returns " << !(hitMap[iMCTrack][3] < fMinimumNofFtsHits) << endl;
    return !(hitMap[iMCTrack][3] < fMinimumNofFtsHits);
  }
}


void PndTrkQATask::FinishTask()
{
  WriteHistograms();
}

// stuff to take into account that a tube can be hit twice in --------------
// the same event and so only one hit is registered
Int_t PndTrkQATask::FillTubeIDMap() {

  for(int ipnt = 0; ipnt < fSttPointArray->GetEntriesFast(); ipnt++) {
    PndSttPoint *point = (PndSttPoint*) fSttPointArray->At(ipnt);
    fSttTubeIDToMCPointID.insert( std::pair<int , int > (point->GetTubeID(), ipnt));
  }
  return CleanTubeIDMap();
}

// delete uniquely assigned tubes from the map
Int_t PndTrkQATask::CleanTubeIDMap() {
  int counter = 0;

  std::multimap< int, int >::iterator it = fSttTubeIDToMCPointID.begin();
  int key = (*it).first;

  cout << "size " << fSttTubeIDToMCPointID.size() << endl;
  while(it != fSttTubeIDToMCPointID.end()) {
  key = (*it).first;
  // std::cout << "There are " << fSttTubeIDToMCPointID.count(key) << " elements with key " << key << endl;
  while(it != fSttTubeIDToMCPointID.equal_range(key).second) it++;
  if(fSttTubeIDToMCPointID.count(key) == 1) fSttTubeIDToMCPointID.erase(key);
  else counter++;
  }

  // returns the number of multiply hit tubes in this event
  return counter;
}

Bool_t PndTrkQATask::CheckIfTrackHitTube(int thistrack, int tubeID)
{

  TArrayI refindices = GetTubeMultiHits(tubeID);
  for(int iref = 0; iref < refindices.GetSize(); iref++) {

    PndSttPoint *point = (PndSttPoint*) fSttPointArray->At(iref);
    if(thistrack == point->GetTrackID()) return kTRUE;
  }
  return kFALSE;

}

Bool_t PndTrkQATask::CheckRefIndexAgainstMultiHit(int tubeID, int refindex) {

  if(IsTubeMultiHit(tubeID) == kFALSE)  return kTRUE;
  TArrayI refindices = GetTubeMultiHits(tubeID);
  for(int iref = 0; iref < refindices.GetSize(); iref++) {
    if(refindex == refindices.At(iref)) return kTRUE;
  }
  return kFALSE;

}

Bool_t PndTrkQATask::IsTubeMultiHit(int tubeID) {
  return (fSttTubeIDToMCPointID.count(tubeID) > 0);
}

TArrayI PndTrkQATask::GetTubeMultiHits(int tubeID) {
  TArrayI array;


  std::pair< std::multimap< int , int >::iterator, std::multimap< int, int >::iterator > ret;
  ret = fSttTubeIDToMCPointID.equal_range(tubeID);
  for (std::multimap< int, int >::iterator it = ret.first; it != ret.second; ++it)
    {


      int size = array.GetSize();
      array.Set(size + 1);
      array.AddAt((*it).second, size);
    }
  return array;
}

// -------------------------------------------------------------


ClassImp(PndTrkQATask)

