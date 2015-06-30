////////////////////////////////////////////////////////////
//
// PndTrkQualityAssuranceTask
//
// Class for Quality Assurance
//
// authors: Lia Lavezzi - University of Torino (2015)
//
////////////////////////////////////////////////////////////

#include "PndTrkQualityAssuranceTask.h"
#include "PndSttMapCreator.h"
#include "PndSttHit.h"
#include "PndSttTube.h"
#include "PndTrack.h"
#include "PndTrackID.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairRunAna.h"
#include "FairHit.h"
#include "FairMCPoint.h"
#include "PndSttPoint.h"
#include <iostream>

using namespace std;


// -----   Default constructor   -------------------------------------------
PndTrkQualityAssuranceTask::PndTrkQualityAssuranceTask() : FairTask("QualityAssurance", 0), fPersistence(kTRUE) {
  sprintf(fSttBranch,"STTHit");
  sprintf(fMvdPixelBranch,"MVDHitsPixel");
  sprintf(fMvdStripBranch,"MVDHitsStrip");
  sprintf(fGemBranch,"GEMHit");
  sprintf(fSciTBranch,"SciTHit");

  //
  sprintf(fTrackBranch,"CombiTrack");
  sprintf(fTrackIDBranch,"CombiTrackID");
  sprintf(fIdealTrackBranch,"IdealTrack");
  sprintf(fIdealTrackIDBranch,"IdealTrackID");
}

// -----   Destructor   ----------------------------------------------------
PndTrkQualityAssuranceTask::~PndTrkQualityAssuranceTask() {

}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndTrkQualityAssuranceTask::Init() {

  fEventCounter = 0;

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndTrkQualityAssuranceTask::Init: "
	 << "RootManager not instantiated, return!" << endl;
    return kFATAL;
  }

  // -- HITS -------------------------------------------------
  //

  // STT
  fSttPointArray = (TClonesArray*) ioman->GetObject("STTPoint");
  if (!fSttPointArray ) return kERROR;
  if(fVerbose > 9) cout << "Found STT points\n";
  fSttHitArray = (TClonesArray*) ioman->GetObject(fSttBranch);
  if (!fSttHitArray ) return kERROR;
  if(fVerbose > 9) cout << "Found STT hits\n";

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
  
  // SciT HIT
  fSciTilHitArray = (TClonesArray*) ioman->GetObject(fSciTBranch);
  if ( !fSciTilHitArray){
    std::cout << "-W- PndTrkTrackFinder::Init: " << "No SciT hitArray, return!" << std::endl;
    return kERROR;
  }
  // SCIT POINT
  fSciTilPointArray = (TClonesArray*) ioman->GetObject("SciTPoint");
  if ( !fSciTilPointArray){
    std::cout << "-W- PndTrkTrackFinder::Init: " << "No SciT pointArray, return!" << std::endl;
    return kERROR;
  }
  
  // GEM HIT
  fGemHitArray = (TClonesArray*) ioman->GetObject(fGemBranch);
  if ( !fGemHitArray){
    std::cout << "-W- PndTrkTrackFinder::Init: " << "No GEM hitArray, return!" << std::endl;
    return kERROR;
  }
  // GEM POINT
  fGemPointArray = (TClonesArray*) ioman->GetObject("GEMPoint");
  if ( !fGemPointArray){
    std::cout << "-W- PndTrkTrackFinder::Init: " << "No GEM pointArray, return!" << std::endl;
    return kERROR;
  }
  
  // TRACKS
  // MC
  fMCTrackArray = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( !fMCTrackArray) return kERROR;
  if(fVerbose > 9) cout << "Found MC Tracks \n";
  // RECO
  fTrackArray = (TClonesArray*) ioman->GetObject(fTrackBranch);
  if ( !fTrackArray) return kERROR;
  if(fVerbose > 9) cout << "Found Track Array from algorithm under investigation\n";
  fTrackIDArray = (TClonesArray*) ioman->GetObject(fTrackIDBranch);
  if ( !fTrackIDArray) return kERROR;
  if(fVerbose > 9) cout << "Found Track ID \n";

  // IDEAL
  fIdealTrackArray = (TClonesArray*) ioman->GetObject(fIdealTrackBranch);
  if ( !fIdealTrackArray) return kERROR;
  if(fVerbose > 9) cout << "Found Ideal Track Array \n";
  fIdealTrackIDArray = (TClonesArray*) ioman->GetObject(fIdealTrackIDBranch);
  if ( !fIdealTrackIDArray) return kERROR;
  if(fVerbose > 9) cout << "Found Ideal Track ID Array \n";

  fMCTrackInfo = new TClonesArray("PndTrkMCTrackInfo");
  ioman->Register("MCTrackInfo",  "QualityAssurance", fMCTrackInfo, fPersistence); // CHECK

  fRecoTrackInfo = new TClonesArray("PndTrkRecoTrackInfo");
  ioman->Register("RecoTrackInfo",  "QualityAssurance", fRecoTrackInfo, fPersistence); // CHECK


  // ----------------------------------------   maps of STT tubes
  PndSttMapCreator *mapperStt = new PndSttMapCreator(fSttParameters);
  fSttTubeArray = mapperStt->FillTubeArray();
  // ----------------------------------------------------  end map

  return kSUCCESS;

}


void PndTrkQualityAssuranceTask::WriteHistograms(){

  TFile* file = FairRootManager::Instance()->GetOutFile();
  file->cd();
  file->mkdir("QualityAssurance");
  file->cd("QualityAssurance");

//   hEfficiency->Write();
//   delete hEfficiency;

}
// -------------------------------------------------------------------------

void PndTrkQualityAssuranceTask::SetParContainers() {
  FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}

// -------------------------------------------------------------------------


void PndTrkQualityAssuranceTask::Exec(Option_t* opt) {
  fMCTrackInfo->Delete();
  fRecoTrackInfo->Delete();

  cout << "--------------------> " << fEventCounter << endl;
  fEventCounter++;
  
//   for(int ihit = 0; ihit < fGemHitArray->GetEntriesFast(); ihit++) {
    
//     PndGemHit * hit = (PndGemHit*) fGemHitArray->At(ihit);
//     int refindex = hit->GetRefIndex();
//     if(refindex == -1) continue;  
//     PndGemMCPoint *point = (PndGemMCPoint*) fGemPointArray->At(refindex);
//     cout << refindex <<  " point gem " << point << " " << point->GetTrackID() << endl;
//   }

  // retrieve the MC information
  // fill PndTrkMCTrackInfo
  for(int itrk = 0; itrk < fIdealTrackArray->GetEntriesFast(); itrk++) {
    PndTrack *idealtrack = (PndTrack*) fIdealTrackArray->At(itrk);
    PndTrackID *idealtrackid = (PndTrackID*) fIdealTrackIDArray->At(itrk);
    
    int size = fMCTrackInfo->GetEntriesFast();
    PndTrkMCTrackInfo mctrackinfo = GetMCInfoFromIdealTrack(idealtrack, idealtrackid);
    if(mctrackinfo.GetNofMCPoints() > 0) new((*fMCTrackInfo)[size]) PndTrkMCTrackInfo(mctrackinfo);
    //     cout << "MCTRack " << mctrackinfo.GetMCTrackID() << endl;
  }
  
  // retrieve the reco information
  // fill the PndTrkRecoTrackQA
  for(int itrk = 0; itrk < fTrackArray->GetEntriesFast(); itrk++) {
    PndTrack *track = (PndTrack*) fTrackArray->At(itrk);
    PndTrackID *trackid = (PndTrackID*) fTrackIDArray->At(itrk);
    
    int size = fRecoTrackInfo->GetEntriesFast();
    PndTrkRecoTrackInfo trackinfo = GetRecoInfoFromRecoTrack(itrk, track, trackid);
    new((*fRecoTrackInfo)[size]) PndTrkRecoTrackInfo(trackinfo);
    //     cout << "RECO Track " << trackinfo.GetMCTrackID() << endl;
    
  }
  
  // fill the quality numbers
  // comparison PndTrk Reco<->MC  TrackQA
  

  // print out
  for(int itrk = 0; itrk < fRecoTrackInfo->GetEntriesFast(); itrk++) {

    PndTrkRecoTrackInfo *info = (PndTrkRecoTrackInfo *) fRecoTrackInfo->At(itrk);
    PndTrkMCTrackInfo mcinfo = info->GetMCTrackInfo();

    //    if(fVerbose > 0) 
    {
      cout << "track " << info->GetRecoTrackID() << " , with " << info->GetNofRecoHits() << " reco hits, corresponds to mc " << info->GetMCTrackID() << " , with " << mcinfo.GetNofMCPoints() << " mc points " << endl;
      cout << "efficiency " << info->GetEfficiency() << " purity " << info->GetPurity() << endl;

      cout << "mvd eff " << info->GetMvdEfficiency() << " " << info->GetMvdPixEfficiency() << " " << info->GetMvdStrEfficiency() << endl;
      cout << "stt eff " << info->GetSttEfficiency() << " " << info->GetSttParalEfficiency() << " " << info->GetSttSkewEfficiency() << endl;
      cout << "scit eff " << info->GetSciTilEfficiency() << " gem " << info->GetGemEfficiency() << endl;


      cout << "pt " << info->GetMomentumFirst().Perp() << " pl " << info->GetMomentumFirst().Z() << endl;
      cout << "mcpt " << mcinfo.GetMomentumFirst().Perp() << " mcpl " << mcinfo.GetMomentumFirst().Z() << endl;
      cout << endl;
    }

  }

  // fill flag fRecoTrackID
  for(int itrk = 0; itrk < fMCTrackInfo->GetEntriesFast(); itrk++) {
    PndTrkMCTrackInfo *mcinfo = (PndTrkMCTrackInfo*) fMCTrackInfo->At(itrk);

    // count how many times it was reconstructed
    std::vector< int > recolist;
    int tmpcandidate = -1, tmpnoftrue = -1, tmpnoffake = 1000;
    for(int jtrk = 0; jtrk < fRecoTrackInfo->GetEntriesFast(); jtrk++) {
      PndTrkRecoTrackInfo *info = (PndTrkRecoTrackInfo*) fRecoTrackInfo->At(jtrk);
      if(info->GetMCTrackID() != mcinfo->GetMCTrackID()) continue;
      recolist.push_back(jtrk);

      if(info->GetNofTrueHits() > tmpnoftrue) {
	tmpcandidate = jtrk;
	tmpnoftrue = info->GetNofTrueHits();
	tmpnoffake = info->GetNofFakeHits();
      }
      else if(info->GetNofTrueHits() == tmpnoftrue && info->GetNofFakeHits() < tmpnoffake) {
	tmpcandidate = jtrk;
	tmpnoftrue = info->GetNofTrueHits();
	tmpnoffake = info->GetNofFakeHits();
      }
    }

    if(recolist.size() > 0 && tmpcandidate == -1) cout << "ERRORRRRRRRRRR " << endl;
    cout << "recolist " << recolist.size() << endl;
    //     if(recolist.size() == 0) mcinfo->SetRecoTrackID(-1);
    if(recolist.size() >= 1) {
      PndTrkRecoTrackInfo *info = (PndTrkRecoTrackInfo*) fRecoTrackInfo->At(tmpcandidate);
      info->SetTrue();
      mcinfo->SetRecoTrackID(info->GetRecoTrackID());
      
      for(int jtrk = 0; jtrk < recolist.size(); jtrk++) {
	if(tmpcandidate == recolist[jtrk]) continue;
	info = (PndTrkRecoTrackInfo*) fRecoTrackInfo->At(recolist[jtrk]);
	info->SetClone();
	mcinfo->SetRecoTrackID(info->GetRecoTrackID());
      }
    }
  }
}


PndTrkMCTrackInfo PndTrkQualityAssuranceTask::GetMCInfoFromIdealTrack(PndTrack *idealtrack, PndTrackID *idealtrackid) {
  
  PndTrackCand *idealtrkcand = idealtrack->GetTrackCandPtr();
    
  Int_t nofsttpoint = idealtrkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId(fSttBranch));
  Int_t nofmvdpixpoint = idealtrkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId(fMvdPixelBranch));
  Int_t nofmvdstrpoint = idealtrkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId(fMvdStripBranch));
  int nofmvdpoint = nofmvdpixpoint + nofmvdstrpoint;
  Int_t nofscitilpoint = idealtrkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId(fSciTBranch));
  Int_t nofgempoint = idealtrkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId(fGemBranch));
  
  int nofsttskewpoint = 0, nofsttparalpoint = 0;    
  // this loop counts skewed (--> parallel) STT/FTS hits
  for(Int_t ihit = 0; ihit < idealtrkcand->GetNHits(); ihit++) {
    PndTrackCandHit idealcandhit = idealtrkcand->GetSortedHit(ihit);
    Int_t hitID1 = idealcandhit.GetHitId();
    Int_t detID1 = idealcandhit.GetDetId();
      
    if(detID1 != FairRootManager::Instance()->GetBranchId(fSttBranch)) continue;
    PndSttHit *stthit = (PndSttHit*) fSttHitArray->At(hitID1);
    Int_t tubeID = stthit->GetTubeID();
    PndSttTube *tube = (PndSttTube*) fSttTubeArray->At(tubeID);
    if(tube->IsSkew()) nofsttskewpoint++;
    else nofsttparalpoint++;
  }
  // -------------------------------------------------------
  // CHECK CHECK CHECK CHECK CHECK CHECK CHECK CHECK CHECK CHECK CHECK CHECK CHECK CHECK 
  //    temporary hacking not to save the SciTil to PndTrackCand CHECK CHECK CHECK CHECK 
  // CHECK CHECK CHECK CHECK CHECK CHECK CHECK CHECK CHECK CHECK CHECK CHECK CHECK CHECK 
  nofscitilpoint = 0;

  PndTrkMCTrackInfo info(nofmvdpixpoint, nofmvdstrpoint, nofsttparalpoint, nofsttskewpoint, nofgempoint, nofscitilpoint); 

  Bool_t isreco = Reconstructability(nofmvdpixpoint, nofmvdstrpoint, nofsttparalpoint, nofsttskewpoint, nofgempoint, nofscitilpoint);
  info.SetReconstructability(isreco);

  // temporary CHECK .........
  int laycheck[4] = {0, 0, 0, 0};
  for(int ipnt = 0; ipnt < fSttPointArray->GetEntriesFast(); ipnt++) {
    PndSttPoint *point = (PndSttPoint*) fSttPointArray->At(ipnt);
    if(point->GetTrackID() != idealtrackid->GetCorrTrackID()) continue;
    Int_t tubeID = point->GetTubeID();
    PndSttTube *tube = (PndSttTube*) fSttTubeArray->At(tubeID);

    if(tube->GetLayerID() == 0) laycheck[0]++;
    else if(tube->GetLayerID() == 7) laycheck[1]++;
    else if(tube->GetLayerID() == 16) laycheck[2]++;
    else if(tube->GetLayerID() == 20) laycheck[3]++;
  }

  bool recook = false;
  if(laycheck[0] == 1 && laycheck[1] == 1 && laycheck[2] == 1 &&  laycheck[3] == 1) recook = true;
  //  info.SetReconstructability(recook);
  // .........................


  info.SetMCTrackID(idealtrackid->GetCorrTrackID());

  info.SetPositionFirst(idealtrack->GetParamFirst().GetPosition());
  info.SetMomentumFirst(idealtrack->GetParamFirst().GetMomentum());

  info.SetPositionLast(idealtrack->GetParamLast().GetPosition());
  info.SetMomentumLast(idealtrack->GetParamLast().GetMomentum());

  info.SetCharge(idealtrack->GetParamFirst().GetQ());

//   cout << "MC TRACK INFO: " << info.GetMCTrackID() << " with " << " " << info.GetNofMCPoints() << " pnts and reconstructability " << isreco << endl;

  return info;

}
  
Bool_t  PndTrkQualityAssuranceTask::Reconstructability(int nofmvdpixpoint, int nofmvdstrpoint, int nofsttparalpoint, int nofsttskewpoint, int nofgempoint, int nofscitilpoint) {


  // CHECK
  return kTRUE;
}

PndTrkRecoTrackInfo PndTrkQualityAssuranceTask::GetRecoInfoFromRecoTrack(int recotrackid, PndTrack *track, PndTrackID *trackid) {
  
  PndTrackCand *trkcand = track->GetTrackCandPtr();
    
  Int_t nofsttpoint = trkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId(fSttBranch));
  Int_t nofmvdpixpoint = trkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId(fMvdPixelBranch));
  Int_t nofmvdstrpoint = trkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId(fMvdStripBranch));
  int nofmvdpoint = nofmvdpixpoint + nofmvdstrpoint;
  Int_t nofscitilpoint = trkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId(fSciTBranch));
  Int_t nofgempoint = trkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId(fGemBranch));

  int nofsttskewpoint = 0, nofsttparalpoint = 0;    
  // this loop counts skewed (--> parallel) STT/FTS hits
  for(Int_t ihit = 0; ihit < trkcand->GetNHits(); ihit++) {
    PndTrackCandHit candhit = trkcand->GetSortedHit(ihit);
    Int_t hitID1 = candhit.GetHitId();
    Int_t detID1 = candhit.GetDetId();
      
    if(detID1 != FairRootManager::Instance()->GetBranchId(fSttBranch)) continue;
    PndSttHit *stthit = (PndSttHit*) fSttHitArray->At(hitID1);
    Int_t tubeID = stthit->GetTubeID();
    PndSttTube *tube = (PndSttTube*) fSttTubeArray->At(tubeID);
    if(tube->IsSkew()) nofsttskewpoint++;
    else nofsttparalpoint++;
  }

  // -------------------------------------------------------
  int mctrackid = trackid->GetCorrTrackID();
  int noftruehits = trackid->GetMultTrackID();

  int nofassomctracks = trackid->GetNCorrTrackId();
  int noffakehits = 0;
  for(int itrk = 1; itrk <  nofassomctracks; itrk++) {
    int noffake = trackid->GetMultTrackID(itrk);
    noffakehits += noffake;
  }

  //:...................:
  int noftruemvdpixhits = 0, noftruemvdstrhits = 0, noftruesttparalhits = 0, noftruesttskewhits = 0, noftruegemhits = 0, noftruescitilhits = 0;
  int noffakemvdpixhits = 0, noffakemvdstrhits = 0, noffakesttparalhits = 0, noffakesttskewhits = 0, noffakegemhits = 0, noffakescitilhits = 0;

  FairHit *hit = NULL;
  FairMCPoint *point = NULL;
  for(Int_t ihit = 0; ihit < trkcand->GetNHits(); ihit++) {
    PndTrackCandHit candhit = trkcand->GetSortedHit(ihit);
    Int_t hitID1 = candhit.GetHitId();
    Int_t detID1 = candhit.GetDetId();
  
    if(detID1 == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) {
      hit = (FairHit*) fMvdPixelHitArray->At(hitID1);
      int refindex = hit->GetRefIndex();
      if(refindex == -1) noffakemvdpixhits++;
      else {
	point = (FairMCPoint*) fMvdPointArray->At(refindex);
	int reftrackid = point->GetTrackID();
	if(reftrackid != mctrackid) noffakemvdpixhits++;
	else noftruemvdpixhits++;
      }
    }
    else if(detID1 == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) {
      hit = (FairHit*) fMvdStripHitArray->At(hitID1);
      int refindex = hit->GetRefIndex();
      if(refindex == -1) noffakemvdstrhits++;
      else {
	point = (FairMCPoint*) fMvdPointArray->At(refindex);
	int reftrackid = point->GetTrackID();
	if(reftrackid != mctrackid) noffakemvdstrhits++;
	else noftruemvdstrhits++;
      }
    }
    if(detID1 == FairRootManager::Instance()->GetBranchId(fSttBranch)) {
      hit = (FairHit*) fSttHitArray->At(hitID1);

      Int_t tubeID = ((PndSttHit*) hit)->GetTubeID();
      PndSttTube *tube = (PndSttTube*) fSttTubeArray->At(tubeID);
      int refindex = hit->GetRefIndex();
      if(refindex == -1) {
	if(tube->IsSkew()) noffakesttskewhits++;
	else noffakesttparalhits++;
      }
      else {
	point = (FairMCPoint*) fSttPointArray->At(refindex);
	int reftrackid = point->GetTrackID();
	if(reftrackid != mctrackid) {
	  if(tube->IsSkew()) noffakesttskewhits++;
	  else noffakesttparalhits++;
	}
	else {
	  if(tube->IsSkew()) noftruesttskewhits++;
	  else noftruesttparalhits++;
	}
      }
    }
    else if(detID1 == FairRootManager::Instance()->GetBranchId(fGemBranch)) {
      hit = (FairHit*) fGemHitArray->At(hitID1);
      int refindex = hit->GetRefIndex();
      if(refindex == -1) noffakegemhits++;
      else {
	point = (FairMCPoint*) fGemPointArray->At(refindex);
	{
	  int reftrackid = point->GetTrackID();
	  if(reftrackid != mctrackid) noffakegemhits++;
	  else noftruegemhits++;
	}
      }
    }
    else if(detID1 == FairRootManager::Instance()->GetBranchId(fSciTBranch)) {
      hit = (FairHit*) fSciTilHitArray->At(hitID1);
      int refindex = hit->GetRefIndex();
      if(refindex == -1) noffakescitilhits++;
      else {
	point = (FairMCPoint*) fSciTilPointArray->At(refindex);
	int reftrackid = point->GetTrackID();
	if(reftrackid != mctrackid) noffakescitilhits++;
	else noftruescitilhits++;
      }
    }
    // ----------------------------------------------------
  }

  PndTrkRecoTrackInfo info(recotrackid);
  info.SetNofMvdPixTrueHits(noftruemvdpixhits);
  info.SetNofMvdStrTrueHits(noftruemvdstrhits);
  info.SetNofSttParalTrueHits(noftruesttparalhits);
  info.SetNofSttSkewTrueHits(noftruesttskewhits);
  info.SetNofGemTrueHits(noftruegemhits);
  info.SetNofSciTilTrueHits(noftruescitilhits);
  info.SetNofMvdPixFakeHits(noffakemvdpixhits);
  info.SetNofMvdStrFakeHits(noffakemvdstrhits);
  info.SetNofSttParalFakeHits(noffakesttparalhits);
  info.SetNofSttSkewFakeHits(noffakesttskewhits);
  info.SetNofGemFakeHits(noffakegemhits);
  info.SetNofSciTilFakeHits(noffakescitilhits);
  
  info.SetMCTrackID(mctrackid);
  
  info.SetPositionFirst(track->GetParamFirst().GetPosition());
  info.SetMomentumFirst(track->GetParamFirst().GetMomentum());
  
  info.SetPositionLast(track->GetParamLast().GetPosition());
  info.SetMomentumLast(track->GetParamLast().GetMomentum());
  
  info.SetCharge(track->GetParamFirst().GetQ());
  
  
  for(int jtrk = 0; jtrk < fMCTrackInfo->GetEntriesFast(); jtrk++) {
    PndTrkMCTrackInfo *mcinfo = (PndTrkMCTrackInfo*) fMCTrackInfo->At(jtrk);
    int mcinfotrackid = mcinfo->GetMCTrackID();
    //        cout << "mcinfotrackid " << mcinfo->GetMCTrackID() << " " << info.GetMCTrackID() << endl;
    if(mcinfotrackid == mctrackid) {
      
      info.SetNofMvdPixMissingHits(mcinfo->GetNofMvdPixPoints() - noftruemvdpixhits);
      info.SetNofMvdStrMissingHits(mcinfo->GetNofMvdStrPoints() - noftruemvdstrhits);
      info.SetNofSttParalMissingHits(mcinfo->GetNofSttParalPoints() - noftruesttparalhits);
      info.SetNofSttSkewMissingHits(mcinfo->GetNofSttSkewPoints() - noftruesttskewhits);
      info.SetNofGemMissingHits(mcinfo->GetNofGemPoints() - noftruegemhits);
      info.SetNofSciTilMissingHits(mcinfo->GetNofSciTilPoints() - noftruescitilhits);
      

//       cout << "good mcinfotrackid " << mcinfo->GetMCTrackID() << " " << info.GetMCTrackID() << endl;
      info.SetMCTrackInfo(mcinfo);
      break;
    }
  }

  info.SetFlag(track->GetFlag());
  return info;
}
  
 

ClassImp(PndTrkQualityAssuranceTask)

