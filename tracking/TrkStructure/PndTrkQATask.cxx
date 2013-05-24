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
PndTrkQATask::PndTrkQATask() : FairTask("QA plots", 0), fPersistence(kTRUE), fUseMVDPix(kTRUE), fUseMVDStr(kTRUE), fUseSTT(kTRUE), fUseSTTSkew(kTRUE) {

  sprintf(fSttBranch,"STTHit");
  sprintf(fMvdPixelBranch,"MVDHitsPixel");
  sprintf(fMvdStripBranch,"MVDHitsStrip");
  sprintf(fInputTrackBranch,"Track");
  sprintf(fInputTrackIDBranch,"TrackID");
}

// -------------------------------------------------------------------------
PndTrkQATask::PndTrkQATask(int verbose) : FairTask("QA plots", verbose), fPersistence(kTRUE), fUseMVDPix(kTRUE), fUseMVDStr(kTRUE), fUseSTT(kTRUE), fUseSTTSkew(kTRUE) {

  sprintf(fSttBranch,"STTHit");
  sprintf(fMvdPixelBranch,"MVDHitsPixel");
  sprintf(fMvdStripBranch,"MVDHitsStrip");
  sprintf(fInputTrackBranch,"Track");
  sprintf(fInputTrackIDBranch,"TrackID");

}

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

  fGoodTrack = 0, fBadTrack = 0, fMCReconstructableTrack = 0, fNotReconstructed = 0, fGhostTrack = 0, fRecoTrack = 0;


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

  fIdealTrackCandArray->Delete();

 
  // ----------------------------------------------------------------
  fThisGoodTrack = 0, fThisBadTrack = 0, fThisMCReconstructableTrack = 0, fThisNotReconstructed = 0, fThisGhostTrack = 0, fThisRecoTrack = 0;

  // CHECK delete this ---
//   if(fEventCounter == 525) {
//     fEventCounter++;
//     return;
//   }
//   // -----
  fEventCounter++;

  Bool_t idealtrackfinder = IdealTrackFinding();
  fMCReconstructableTrack += fIdealTrackCandArray->GetEntriesFast();
  fThisMCReconstructableTrack += fIdealTrackCandArray->GetEntriesFast();

 MapMCToReco();

 std::map<int, std::vector< int > >::iterator it = fMC2RecoMap.begin();


//  // DELETE THIS ///////////////////////////////////////
//  cout << "@@@@@@@@@@@@@@@@@@@@ RECO MAP" << endl;
//  cout << "MAPPA " << fMC2RecoMap.size() << endl;
//  while(it != fMC2RecoMap.end()) {
//     // take a MC track 
//    int imctrack = it->first; 
//    // take reco associated
//    std::vector<int> asso = it->second;
//    // ... PRINT ............. 
//    if(asso.at(0) != -1)  cout << "MCtrack " << imctrack << " has " << asso.size() << " associated track(s):";
//    else cout << "MCtrack " << imctrack << " has 0 associated tracks";
   
//    for(int itrk = 0; itrk < asso.size(); itrk++ ) cout << " " << asso.at(itrk);
//    cout << endl;
//    // ........................
//    it++;
//  }
//  cout << "@@@@@@@@@@@@@@@@@@@@ RECO MAP" << endl;

//  // /////////////////////////////////////////////////////




 it = fMC2RecoMap.begin();


 while(it != fMC2RecoMap.end()) {

   // take a MC track 
   int imctrack = it->first; 
   // take reco associated
   std::vector<int> asso = it->second;
  
  
   // how many associated?
   if(asso.at(0) == -1) {
     fNotReconstructed++;
     fThisNotReconstructed++;
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
    
   int nofmctrackpoints   = mctrkcand->GetNHits();
   int nofmctracksttpoints = mctrkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId(fSttBranch));
   int nofmctracksttskewpoints = 0, nofmctracksttparalpoints = 0;
   int nofmctrackmvdpixpoints = mctrkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId(fMvdPixelBranch));
   int nofmctrackmvdstrpoints = mctrkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId(fMvdStripBranch));

   for(Int_t ihit = 0; ihit < nofmctrackpoints; ihit++) {
     PndTrackCandHit mccandhit = mctrkcand->GetSortedHit(ihit);
     Int_t hitID1 = mccandhit.GetHitId();
     Int_t detID1 = mccandhit.GetDetId();


     if(!fUseMVDPix && detID1 == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) continue;
     if(!fUseMVDStr && detID1 == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) continue;
     if(!fUseSTT && detID1 == FairRootManager::Instance()->GetBranchId(fSttBranch)) continue;


     // count skew ---------------------------
     if(detID1 == FairRootManager::Instance()->GetBranchId(fSttBranch)) {
       PndSttHit *stthit = (PndSttHit*) fSttHitArray->At(hitID1);
       int tubeID = stthit->GetTubeID();
       PndSttTube *tube = (PndSttTube*) fTubeArray->At(tubeID);
       if(tube->IsSkew()) nofmctracksttskewpoints++;
     }
     nofmctracksttparalpoints = nofmctracksttpoints - nofmctracksttskewpoints;
     // ---------------------------------------
	
   }

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

   Int_t nAssigned = 0, nNotAssigned = 0, nWrong = 0;
   Int_t nAssignedStt = 0, nNotAssignedStt = 0, nWrongStt = 0;
   Int_t nAssignedSttSkew = 0, nNotAssignedSttSkew = 0, nWrongSttSkew = 0;
   Int_t nAssignedSttParal = 0, nNotAssignedSttParal = 0, nWrongSttParal = 0;
   Int_t nAssignedMvdPixel = 0, nNotAssignedMvdPixel = 0, nWrongMvdPixel = 0;
   Int_t nAssignedMvdStrip = 0, nNotAssignedMvdStrip = 0, nWrongMvdStrip = 0;
   int nofrecotrackpoints = 0;
   int nofrecotracksttpoints = 0;
   int nofrecotracksttskewpoints = 0, nofrecotracksttparalpoints = 0;
   int nofrecotrackmvdpixpoints = 0;
   int nofrecotrackmvdstrpoints = 0;
   int jrecotrack = 0;

     
   for(int itrk = 0; itrk < asso.size(); itrk++) {
     
     Int_t nTmpAssigned = 0, nTmpNotAssigned = 0, nTmpWrong = 0;
     Int_t nTmpAssignedStt = 0, nTmpNotAssignedStt = 0, nTmpWrongStt = 0;
     Int_t nTmpAssignedSttSkew = 0, nTmpNotAssignedSttSkew = 0, nTmpWrongSttSkew = 0;
     Int_t nTmpAssignedSttParal = 0, nTmpNotAssignedSttParal = 0, nTmpWrongSttParal = 0;
     Int_t nTmpAssignedMvdPixel = 0, nTmpNotAssignedMvdPixel = 0, nTmpWrongMvdPixel = 0;
     Int_t nTmpAssignedMvdStrip = 0, nTmpNotAssignedMvdStrip = 0, nTmpWrongMvdStrip = 0;

    

     int jtrk = asso.at(itrk);
     PndTrack *trk = (PndTrack*) fTrackArray->At(jtrk);
     if(!trk) continue;
     PndTrackID *trkID = (PndTrackID*) fTrackIDArray->At(jtrk);
     if(!trkID) continue;
	
     Int_t recotrackID = trkID->GetCorrTrackID();
     if(recotrackID != mctrackID) cout << "ERROR " << endl;

     PndTrackCand *trkcand = trk->GetTrackCandPtr();
     if(!trkcand) continue;

     int noftmprecotrackpoints = trkcand->GetNHits();
     int noftmprecotracksttpoints = trkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId(fSttBranch));
     int noftmprecotracksttskewpoints = 0, noftmprecotracksttparalpoints = 0;
     int noftmprecotrackmvdpixpoints = trkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId(fMvdPixelBranch));
     int noftmprecotrackmvdstrpoints = trkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId(fMvdStripBranch));
     
 //     // 
//      cout << "POINTS MC " << endl;
//      cout << nofmctrackpoints << " " << nofmctracksttpoints << " " << nofmctrackmvdpixpoints << " " << nofmctrackmvdstrpoints << endl;
//       cout << "POINTS RECO " << endl;
//       cout << noftmprecotrackpoints << " " << noftmprecotracksttpoints << " " << noftmprecotrackmvdpixpoints << " " << noftmprecotrackmvdstrpoints << endl;

     // loop over reco hits ---------------------------
     for(Int_t ihit = 0; ihit < noftmprecotrackpoints; ihit++) {
       PndTrackCandHit candhit = trkcand->GetSortedHit(ihit);
       Int_t hitID = candhit.GetHitId();
       Int_t detID = candhit.GetDetId();
       FairHit *hit = NULL;
       if(detID == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) {
	 if(!fUseMVDPix) continue;
	 hit = (PndSdsHit*) fMvdPixelHitArray->At(hitID);
	 if(hit->GetRefIndex() == -1) {
	   nTmpWrongMvdPixel++;
	   nTmpWrong++;
	 }
	 else {	 PndSdsMCPoint *pnt = (PndSdsMCPoint*) fMvdPointArray->At(hit->GetRefIndex());
	   if(pnt->GetTrackID() == mctrackID) {
	     nTmpAssignedMvdPixel++;
	     nTmpAssigned++;
	   }
	   else {
	     nTmpWrongMvdPixel++;
	     nTmpWrong++;
	   }
	 }
       }
       else if(detID == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) {
	 if(!fUseMVDStr) continue;
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
	   }   
	   else  {
	     nTmpWrongMvdStrip++;
	     nTmpWrong++;
	   }
	 }
       }
       else if(detID == FairRootManager::Instance()->GetBranchId(fSttBranch)) {

	 hit = (PndSttHit*) fSttHitArray->At(hitID);
	 if(hit->GetRefIndex() == -1) {
	   nTmpWrongStt++;
	   nTmpWrong++;
	 }
	 else {
	   PndSttPoint *pnt = (PndSttPoint*) fSttPointArray->At(hit->GetRefIndex());
	   int tubeID = ((PndSttHit*) hit)->GetTubeID();
	   PndSttTube *tube = (PndSttTube*) fTubeArray->At(tubeID);
	   if(tube->IsSkew()) noftmprecotracksttskewpoints++;
	   else noftmprecotracksttparalpoints++;
	   
	   if(pnt->GetTrackID() == mctrackID) {
	     nTmpAssignedStt++;
	     nTmpAssigned++;
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
     nTmpNotAssigned = nofmctrackpoints - nTmpAssigned;
     nTmpNotAssignedMvdPixel = nofmctrackmvdpixpoints - nTmpAssignedMvdPixel;
     nTmpNotAssignedMvdStrip = nofmctrackmvdstrpoints - nTmpAssignedMvdStrip; 
     nTmpNotAssignedStt = nofmctracksttpoints - nTmpAssignedStt;
     nTmpNotAssignedSttSkew = nofmctracksttskewpoints - nTmpAssignedSttSkew;
     nTmpNotAssignedSttParal = nofmctracksttparalpoints - nTmpAssignedSttParal;

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
       nofrecotrackmvdpixpoints   = noftmprecotrackmvdpixpoints;
       nofrecotrackmvdstrpoints   = noftmprecotrackmvdstrpoints;
       jrecotrack = jtrk;
     }

   }
  
   if(fVerbose > 1) {
     cout << "POINTS MC " << endl;
     cout << nofmctrackpoints << " " << nofmctracksttpoints << " " << nofmctrackmvdpixpoints << " " << nofmctrackmvdstrpoints << endl;
     cout << "POINTS RECO " << endl;
     cout << nofrecotrackpoints << " " << nofrecotracksttpoints << " " << nofrecotrackmvdpixpoints << " " << nofrecotrackmvdstrpoints << endl;
   }
// EFFICIENCY one reco for one mc
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


   // CONTAMINATION/PURITY one reco for one mc  
   if(nofrecotrackpoints > 0) {
     hContamination->Fill(nofrecotrackpoints, (Double_t) nWrong/nofrecotrackpoints);
     hPurity->Fill(nofrecotrackpoints, (Double_t) nAssigned/nofrecotrackpoints);
   }
   
   // 	hContaminationStt->Fill(nofrecotracksttpoints, (Double_t) nWrongStt/nofrecotracksttpoints);
   // 	hContaminationSttSkew  ->Fill(nofrecotracksttskewpoints, (Double_t) nWrongSttSkew  /nofrecotracksttskewpoints);
   // 	hContaminationSttParal->Fill(nofrecotracksttparalpoints, (Double_t) nWrongSttParal/nofrecotracksttparalpoints);
   // 	hContaminationMvdPixel->Fill(nofrecotrackmvdpixpoints, (Double_t) nWrongMvdPixel/nofrecotrackmvdpixpoints);
   // 	hContaminationMvdStrip->Fill(nofrecotrackmvdstrpoints, (Double_t) nWrongMvdStrip/nofrecotrackmvdstrpoints);



   if(fVerbose > 1) {
     
     cout << "TRACK " << jrecotrack << " MCTRACK " << imctrack << " " << endl;
     cout << "total " << nofrecotrackpoints << " mctotal " << nofmctrackpoints << endl;
     cout << "assigned " <<  nAssigned <<  " not assigned " << nNotAssigned << " wrong " << nWrong << endl;
   }
   
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
 
   it++;
 }
 
 // CHECK for now:
 // fMCReconstructableTrack are the mc tracks with at least 3 parallel stt point

 if(fVerbose > 0) {
   if(fThisRecoTrack == 0 || fThisMCReconstructableTrack == 0) cout << "#### NOW: Recontructed Tracks " << fThisRecoTrack << " MC Reconstructable Tracks " << fThisMCReconstructableTrack << endl;
   else cout << "#### NOW: GOOD = "<< 100. *  fThisGoodTrack/fThisMCReconstructableTrack << "%, BAD = " << 100. *  fThisBadTrack/fThisMCReconstructableTrack << "%, MISSED = " << 100. *  fThisNotReconstructed/fThisMCReconstructableTrack << "%, GHOSTS " << 100. *  fThisGhostTrack/fThisRecoTrack << "%" << endl;
   if(fRecoTrack == 0 || fMCReconstructableTrack == 0) cout << "TOTAL: Recontructed Tracks " << fRecoTrack << " MC Reconstructable Tracks " << fMCReconstructableTrack << endl;
   else cout << "======== AFTER THIS EVENT: GOOD = "<< 100. *  fGoodTrack/fMCReconstructableTrack << "%, BAD = " << 100. *  fBadTrack/fMCReconstructableTrack << "%, MISSED = " << 100. *  fNotReconstructed/fMCReconstructableTrack << "%, GHOSTS " << 100. *  fGhostTrack/fRecoTrack << "%"  << endl;
 } 
}

Int_t PndTrkQATask::CheckIfPresent(Int_t trackid) {
  for(int itrk = 0; itrk < fNofMCTrack; itrk++) {
    if(fMCTracks[itrk][0] == trackid) return itrk;
  }
  return -1;
}

Bool_t PndTrkQATask::IdealTrackFinding() {
  fIdealTrackCandArray->Delete();
  //  cout << "IDEAL" << endl;
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
     if(fVerbose > 2)  cout << "MVD ON " << mcTrackIndex << " " << endl;
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
       if(fVerbose > 2) cout << "STT ON " << mcTrackIndex << " " << endl;
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


void PndTrkQATask::MapMCToReco()
{
// INDEX in IdealTrackCandArray <---> fTrackArray entries
//  std::map< int, std::vector<int> > fMC2RecoMap; 
  if(fVerbose > 2) cout << "mc   tracks " << fIdealTrackCandArray->GetEntriesFast() << endl;
  if(fVerbose > 2) cout << "reco tracks " << fTrackArray->GetEntriesFast() << endl;
  fMC2RecoMap.clear();
  fRecoTrack += fTrackArray->GetEntriesFast();
  fThisRecoTrack += fTrackArray->GetEntriesFast();

  for(int itrk = 0; itrk < fIdealTrackCandArray->GetEntriesFast(); itrk++) {
    PndTrackCand *mctrkcand = (PndTrackCand*) fIdealTrackCandArray->At(itrk);
    if(!mctrkcand) continue;
      Int_t mctrackID = mctrkcand->getMcTrackId();
      std::vector< int > associatedrecotracks;
      for(int jtrk = 0; jtrk < fTrackArray->GetEntriesFast(); jtrk++) {
	PndTrack *trk = (PndTrack*) fTrackArray->At(jtrk);
	if(!trk) continue;
	PndTrackID *trkID = (PndTrackID*) fTrackIDArray->At(jtrk);
	if(!trkID) continue;
	
	Int_t recotrackID = trkID->GetCorrTrackID();

	if(recotrackID != mctrackID) continue;
	associatedrecotracks.push_back(jtrk);
      }
      if(associatedrecotracks.size() == 0) associatedrecotracks.push_back(-1);
    
      if(fVerbose > 2) {
	if(associatedrecotracks.at(0) != -1)  cout << "track mc " << itrk << " associated to " << associatedrecotracks.size() << " tracks" << endl;
	else  cout << "track mc " << itrk << " associated to 0 tracks" << endl;
      }

      fMC2RecoMap.insert(std::pair< int, std::vector< int > > (itrk, associatedrecotracks));
  }
}




ClassImp(PndTrkQATask)

