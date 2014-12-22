////////////////////////////////////////////////////////////
//
// PndTrkShortTrackFinder
// 
// Class for secondary track pattern recognition
//
// authors: Lia Lavezzi - University of Torino (2014)
//                                   
////////////////////////////////////////////////////////////

#include "PndTrkShortTrackFinder.h"

// stt
#include "PndSttHit.h"
#include "PndSttTube.h"
#include "PndSttMapCreator.h"
// sds
#include "PndSdsHit.h"
// track(cand)
#include "PndTrackCand.h"
#include "PndTrackCandHit.h"
#include "PndTrack.h"

#include "PndTrkConformalHit.h"
#include "PndTrkConformalHitList.h"
#include "PndTrkConformalTransform.h"
#include "PndTrkTools.h"
#include "PndTrkSkewHit.h"
#include "PndTrkFitter.h"
// fairroot
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
// ROOT
#include "TClonesArray.h"
#include "TVector3.h"
#include "TArc.h"
#include "TLine.h"
#include "TMarker.h"
#include "TSpectrum2.h" 
#include "TSpectrum.h" 
#include "TStopwatch.h" 
#include "TH3D.h"
// tracking 
#include "PndTrkClusterList.h"
#include "PndTrkTrackList.h"
#include "PndTrkClean.h"
#include "PndTrkNeighboringMap.h"
#include "PndTrkIndivisibleHit.h"

#include <iostream>
    
using namespace std;


// -----   Default constructor   -------------------------------------------
PndTrkShortTrackFinder::PndTrkShortTrackFinder() : FairTask("secondary track finder", 0), fDisplayOn2(kFALSE), fPersistence(kTRUE), fUseMVDPix(kTRUE), fUseMVDStr(kTRUE), fUseSTT(kTRUE), fUseSCIT(kTRUE), fUseGEM(kTRUE), fSecondary(kFALSE), fMvdPix_RealDistLimit(1000), fMvdStr_RealDistLimit(1000), fStt_RealDistLimit(1000), fMvdPix_ConfDistLimit(1000), fMvdStr_ConfDistLimit(1000), fStt_ConfDistLimit(1000), fInitDone(kFALSE), fUmin(-0.07), fUmax(0.07), fVmin(-0.07), fVmax(0.07), fRmin(-1.5), fRmax(1.5), fThetamin(0), fThetamax(180), fRefHit(NULL) {
  sprintf(fSttBranch,"STTHit");
  sprintf(fMvdPixelBranch,"MVDHitsPixel");
  sprintf(fMvdStripBranch,"MVDHitsStrip");
  sprintf(fSciTBranch,"SciTHit");
  sprintf(fGemBranch,"GEMHit");
  PndGeoHandling::Instance();
}

PndTrkShortTrackFinder::PndTrkShortTrackFinder(int verbose) : FairTask("secondary track finder", verbose), fDisplayOn2(kFALSE), fPersistence(kTRUE), fUseMVDPix(kTRUE), fUseMVDStr(kTRUE), fUseSTT(kTRUE), fUseSCIT(kTRUE), fUseGEM(kTRUE), fSecondary(kFALSE), fMvdPix_RealDistLimit(1000), fMvdStr_RealDistLimit(1000), fStt_RealDistLimit(1000), fMvdPix_ConfDistLimit(1000), fMvdStr_ConfDistLimit(1000), fStt_ConfDistLimit(1000), fInitDone(kFALSE), fUmin(-0.07), fUmax(0.07), fVmin(-0.07), fVmax(0.07), fRmin(-1.5), fRmax(1.5), fThetamin(0), fThetamax(180), fRefHit(NULL) {
  sprintf(fSttBranch,"STTHit");
  sprintf(fMvdPixelBranch,"MVDHitsPixel");
  sprintf(fMvdStripBranch,"MVDHitsStrip");
  sprintf(fSciTBranch,"SciTHit");
  sprintf(fGemBranch,"GEMHit");
  PndGeoHandling::Instance();
}

// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndTrkShortTrackFinder::~PndTrkShortTrackFinder() { 

  delete fSttPointArray;
  delete fSttHitArray;
  delete fMvdPixelHitArray;
  delete fMvdStripHitArray;
  delete fSciTHitArray;
  delete fGemHitArray;
  delete fTrackArray;
  delete fTrackCandArray;
  delete fTubeArray;

  delete fSttParameters;
  delete fMapper;

  delete stthitlist;
  delete mvdpixhitlist;
  delete mvdstrhitlist;
  delete scithitlist;
  delete gemhitlist;
  
  delete fConformalHitList;
  delete tools;

  delete fRefHit;

  // delete fFoundPeaks;

  delete fTimer;

  delete fFitter;
  delete fHitMap;

  delete hxy;
  delete hxz;
  delete hzphi;
  delete display;
  delete huv;
  
}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndTrkShortTrackFinder::Init() {
  
  fEventCounter = 0;

   

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndTrkShortTrackFinder::Init: "
	 << "RootManager not instantiated, return!" << endl;
    return kFATAL;
  }

  // -- HITS -------------------------------------------------
  //
  // STT
  fSttHitArray = (TClonesArray*) ioman->GetObject(fSttBranch);
  if ( ! fSttHitArray ) {
    cout << "-W- PndTrkShortTrackFinder::Init: "
	 << "No STTHit array, return!" << endl;
    return kERROR;
  }
  //
  // MVD PIXEL
  fMvdPixelHitArray = (TClonesArray*) ioman->GetObject(fMvdPixelBranch);
  if ( !fMvdPixelHitArray){
    std::cout << "-W- PndTrkShortTrackFinder::Init: " << "No MVD Pixel hitArray, return!" << std::endl;
    return kERROR;
  }
  //
  // MVD STRIP
  fMvdStripHitArray = (TClonesArray*) ioman->GetObject(fMvdStripBranch);
  if ( !fMvdStripHitArray){
    std::cout << "-W- PndTrkShortTrackFinder::Init: " << "No MVD Strip hitArray, return!" << std::endl;
    return kERROR;
  }
  //
  // SciT 
  fSciTHitArray = (TClonesArray*) ioman->GetObject(fSciTBranch);
  if ( !fSciTHitArray){
    std::cout << "-W- PndTrkShortTrackFinder::Init: " << "No SciT hitArray, return!" << std::endl;
    return kERROR;
  }
  //
  // GEM
  fGemHitArray = (TClonesArray*) ioman->GetObject(fGemBranch);
  if ( !fGemHitArray){
    std::cout << "-W- PndTrkShortTrackFinder::Init: " << "No GEM hitArray, return!" << std::endl;
    return kERROR;
  }
  //
  // long tracks
  fLongTrackArray = (TClonesArray*) ioman->GetObject("Track");
  if ( !fLongTrackArray) {
    std::cout << "-W- PndTrkShortTrackFinder::Init: " << "No long track Array, return!" << std::endl;
    return kERROR;
  }


  // output
  fTrackArray = new TClonesArray("PndTrack");
  fTrackCandArray = new TClonesArray("PndTrackCand");
  ioman->Register("ShortTrack", "pr", fTrackArray, fPersistence); // CHECK
  ioman->Register("ShortTrackCand",  "pr", fTrackCandArray, fPersistence); // CHECK

  
  // ----------------------------------------   maps of STT tubes
  fMapper = new PndSttMapCreator(fSttParameters);
  fTubeArray = fMapper->FillTubeArray();
  // ----------------------------------------------------  end map
 
  if(fDisplayOn2) {
    display = new TCanvas("display", "display", 0, 0, 800, 800); // CHECK
    display->Divide(2, 2);
  }



  tools = new PndTrkTools();
  fFitter = new PndTrkFitter(fVerbose);
  fHitMap = new PndTrkNeighboringMap(fTubeArray);
  fTimer = new TStopwatch();

  fCluster = new PndTrkCluster();
  fFinalCluster = new PndTrkCluster();
  fIndivCluster = new PndTrkCluster();
  fSkewCluster = new PndTrkCluster();
  fFinalSkewCluster = new PndTrkCluster();
  fTrackList = new PndTrkTrackList();
  fIndivisibleHitList = new PndTrkCluster(); 

  fConformalHitList = new PndTrkConformalHitList();


  return kSUCCESS;

}

// -------------------------------------------------------------------------

void PndTrkShortTrackFinder::SetParContainers() {
  FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}
 
// -------------------------------------------------------------------------
 

void PndTrkShortTrackFinder::Initialize() {
  
  stthitlist = new PndTrkSttHitList(fTubeArray);
  mvdpixhitlist = new PndTrkSdsHitList(MVDPIXEL);
  mvdstrhitlist = new PndTrkSdsHitList(MVDSTRIP);
  scithitlist = new PndTrkSciTHitList();
  gemhitlist = new PndTrkGemHitList();

  if(fUseSTT) { 
    stthitlist->AddTCA(FairRootManager::Instance()->GetBranchId(fSttBranch), fSttHitArray);
    stthitlist->Instanciate();
  }

  if(fUseMVDPix) {
    mvdpixhitlist->AddTCA(FairRootManager::Instance()->GetBranchId(fMvdPixelBranch), fMvdPixelHitArray);
    mvdpixhitlist->InstanciatePixel();
  }

  if(fUseMVDStr) {
    mvdstrhitlist->AddTCA(FairRootManager::Instance()->GetBranchId(fMvdStripBranch), fMvdStripHitArray);
    mvdstrhitlist->InstanciateStrip();
  }

  if(fUseSCIT) {
    scithitlist->AddTCA(FairRootManager::Instance()->GetBranchId(fSciTBranch), fSciTHitArray);
    scithitlist->Instanciate();
  }

  if(fUseGEM) {
    gemhitlist->AddTCA(FairRootManager::Instance()->GetBranchId(fGemBranch), fGemHitArray);
    gemhitlist->Instanciate();
  }

  fConformalHitList->Clear();
  fFoundPeaks.clear();

  fInitDone = kTRUE;
  //  stthitlist->PrintSectors();
}


void PndTrkShortTrackFinder::Exec(Option_t* opt)  {
  // ############## I N I T I A L I Z A T I O N S ##############
  fTrackArray->Delete();
  fTrackCandArray->Delete();
  //  if(fVerbose > 0) 

  if(fSttHitArray->GetEntriesFast() > 200) {   // CHECK
    fEventCounter++;
    return;
  }

  cout << "*********************** " << fEventCounter << " ***********************" << endl;
  fEventCounter++;
  // initialize -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~
  Initialize();
   if(fVerbose > 1) 
  {
    cout << "number of stt    hits " << fSttHitArray->GetEntriesFast() << endl;
    cout << "number of mvdpix hits " << fMvdPixelHitArray->GetEntriesFast() << endl;
    cout << "number of mvdstr hits " << fMvdStripHitArray->GetEntriesFast() << endl;
    cout << "number of scit   hits " << fSciTHitArray->GetEntriesFast() << endl;
    cout << "number of gem    hits " << fGemHitArray->GetEntriesFast() << endl;
  }
  // initialize display -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-
  if(fDisplayOn2)  {
    Refresh();
    char goOnChar;
    display->Update();
    display->Modified();
    cout << " STARTING" << endl;
    cin >> goOnChar;
    display->Update();
    display->Modified();
  }

  // initialize hit map -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-
  fHitMap->Clear();
  FillHitMap();
  // ##########################################################

  PndTrkHit *hit = NULL;
  for(int itrk = 0; itrk < fLongTrackArray->GetEntriesFast(); itrk++) {
    PndTrack *trk = (PndTrack*) fLongTrackArray->At(itrk);
    PndTrackCand *cand = trk->GetTrackCandPtr();
    if(!cand) 
      { 
	cout << "ERROR track " << itrk << " has no candidate association" << endl; 
	continue;
      }

    for (Int_t ihit = 0; ihit < cand->GetNHits(); ihit++) {
      PndTrackCandHit candhit = cand->GetSortedHit(ihit);
      Int_t hitId = candhit.GetHitId();
      Int_t detId = candhit.GetDetId();

      if(detId == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) {
	hit = mvdpixhitlist->GetHitByID(hitId);
	hit->SetUsedFlag(true);
      }
      // mvd str
      else if(detId == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) {
	hit = mvdstrhitlist->GetHitByID(hitId);
	hit->SetUsedFlag(true);
      }
      // stt
      else if(detId == FairRootManager::Instance()->GetBranchId(fSttBranch)) {
	hit = stthitlist->GetHitByID(hitId);
	hit->SetUsedFlag(true);
      }
      // scitil;
      else if(detId == FairRootManager::Instance()->GetBranchId(fSciTBranch)) {
	hit = scithitlist->GetHitByID(hitId);
	hit->SetUsedFlag(true);
      }
      // gem
      else if(detId == FairRootManager::Instance()->GetBranchId(fGemBranch)) {
	hit = gemhitlist->GetHitByID(hitId);
	hit->SetUsedFlag(true);
      }

    } 
    
  }


  if(fDisplayOn2)  {


    Refresh();
    char goOnChar;
    display->Update();
    display->Modified();
    cin >> goOnChar;
  }


 
//   Reset();
//   fFinalCluster->Clear("C");
//   fTrackList->Clear("C");
//   fConformalHitList->Clear("C"); 
}



void PndTrkShortTrackFinder::Reset()
{
  
  if(fDisplayOn2) {
    char goOnChar;
    display->Update();
    display->Modified();
    cout << "Finish? ";
    cin >> goOnChar;
    cout << "FINISH" << endl;
  }
  
  if(fInitDone) {
    if(stthitlist) delete stthitlist;
    if(mvdpixhitlist)  delete mvdpixhitlist; 
    if(mvdstrhitlist)  delete mvdstrhitlist;
    if(scithitlist)  delete scithitlist;
    if(gemhitlist)  delete gemhitlist;
    if(fTimer) {
      fTimer->Stop();
      fTime += fTimer->RealTime();
      
      if(fVerbose > 0) cerr << fEventCounter << " Real time " << fTime << " s" << endl;
    }
  }

  fInitDone = kFALSE;
}


void PndTrkShortTrackFinder::Refresh(){
  // CHECK
  char goOnChar;
  //  cout << "Refresh?" << endl;
  //  cin >> goOnChar;
  //  cout << "REFRESHING" << endl;
  DrawGeometry();
  if(fVerbose)  cout << "Refresh stt" << endl;
  DrawHits(stthitlist);
  if(fVerbose)  cout << "Refresh pixel" << endl;
  DrawHits(mvdpixhitlist);
  if(fVerbose)  cout << "Refresh strip" << endl;
  DrawHits(mvdstrhitlist);
  if(fVerbose)  cout << "Refresh scit" << endl;
  DrawHits(scithitlist);
  if(fVerbose)  cout << "Refresh gem" << endl;
  DrawHits(gemhitlist);
  if(fVerbose)  cout << "Refresh stop" << endl;

}

void PndTrkShortTrackFinder::DrawGeometry() {
  if(hxy == NULL)  hxy = new TH2F("hxy", "xy plane", 110, -55, 55, 110, -55, 55);
  else hxy->Reset();
  display->cd(1);
  hxy->SetStats(kFALSE);
  hxy->Draw();

  // draw all the tubes
  for(int itube = 1; itube < fTubeArray->GetEntriesFast(); itube++) {
    PndSttTube *tube = (PndSttTube*) fTubeArray->At(itube);
    if(tube->IsParallel()) {
      TArc * arc = new TArc(tube->GetPosition().X(), tube->GetPosition().Y(), 0.5);
      arc->SetFillStyle(0);
      arc->SetLineColor(kCyan - 10);
//       if(tube->GetLayerID() == 0 || tube->GetLayerID() == 7 || tube->GetLayerID() == 16 || tube->GetLayerID() == 20) arc->SetFillColor(kRed);
//       else         arc->SetFillStyle(0);

      arc->Draw("SAME");
    }
    else {
      TMarker *mrk = new TMarker(tube->GetPosition().X(), tube->GetPosition().Y(), 6);
   
      mrk->SetMarkerColor(kCyan - 10);
      mrk->Draw("SAME");
    }
  }
  // ............................
 
  display->Update();
  display->Modified();  
 
}

void PndTrkShortTrackFinder::DrawHits(PndTrkHitList *hitlist) {
  display->cd(1);
  hitlist->Draw();
  display->Update();
  display->Modified();
}

void PndTrkShortTrackFinder::FillHitMap() {
  TObjArray limits;
  TObjArray sector[6];
  TObjArray *neighborings = NULL;


  //  PndTrkHit *hit, hit2;
  // Loop over all hits and look for:
  // 1 - tubes limiting sectors                               std::vector< int > limithit
  // 2 - tubes with no neighborings (will not use them)       std::vector< int > standalone
  // 3 - tubes with only 1 neighboring (will serve as seed)   std::vector< int > seeds
  // 4 - tubes with only 2 neighborings, one of which is on   std::vector< int > candseeds
  //     the same layer and has neighboring (will be candidate 
  //     to serve as seed, if needed)
  // 5 - Fill the sector std::vector according to sectors     std::vector< int > sector*

  fHitMap->SetOwnerValue(kTRUE); // CHECK
  for(int ihit = 0; ihit < stthitlist->GetNofHits(); ihit++) {

    PndTrkHit  *hit = stthitlist->GetHit(ihit);
    int tubeID = hit->GetTubeID();
    PndSttTube *tube = (PndSttTube*) fTubeArray->At(tubeID);
    if(tube->IsSectorLimit() == kTRUE) limits.Add(hit);

    neighborings = new TObjArray();
   
    for(int jhit = 0; jhit < stthitlist->GetNofHits(); jhit++) {
      if(ihit == jhit) continue;
     
      PndTrkHit *hit2 = stthitlist->GetHit(jhit);
      int tubeID2 = hit2->GetTubeID();
     
      if(tube->IsNeighboring(tubeID2) == kTRUE) neighborings->Add(hit2);
     
    }
   
    //    cout << "HIT: " << hit->GetHitID() << " has " << neighborings->GetEntriesFast() << " hits" << endl;
//     if(fDisplayOn2) { 
//       Refresh();
//     }
  

    fHitMap->AddNeighboringsToHit(hit, neighborings);

//     if(fDisplayOn2) {
//       char goOnChar;
//       display->Update();
//       display->Modified();
//       cout << " go on?" << endl;
//       cin >> goOnChar;
//     }
  }
  //  neighborings = NULL;
  //   delete neighborings;

  if(fDisplayOn2) {
    if(1 == 1)  DrawLists();
    if(1 == 2)  DrawNeighborings();
    // ================================================
    Refresh();
    char goOnChar;
    for(int ihit = 0; ihit < stthitlist->GetNofHits(); ihit++) {
      PndTrkHit *stthit = stthitlist->GetHit(ihit);
      TObjArray indiv = fHitMap->GetIndivisiblesToHit(stthit);
      for(int jhit = 0; jhit < indiv.GetEntriesFast(); jhit++) {
	PndTrkHit *stthit2 = (PndTrkHit*) indiv.At(jhit);
// 	stthit2->Draw(kOrange);
//  	stthit->Draw(kOrange);
      }
    }

    display->Update();
    display->Modified();
    cout << " FILLHITMAP STARTING" << endl;
    cin >> goOnChar;
    display->Update();
    display->Modified();
    // ================================================
  }
  //  cout << "PRINT INDIVISIBILE MAP" << endl;
  //  fHitMap->PrintIndivisibleMap();

}


// draw lists ---------
void PndTrkShortTrackFinder::DrawLists() {
  char goOnChar;
//   cout << "DrawLists" << endl;
//   cin >> goOnChar;
//   Refresh(); 
  
  
  for(int i = 0; i < fHitMap->GetStandalone().GetEntriesFast(); i++) {
    PndTrkHit *hitA = (PndTrkHit*) fHitMap->GetStandalone().At(i);
    //    hitA->DrawTube(kGreen);
  }
  for(int i = 0; i < fHitMap->GetSeeds().GetEntriesFast(); i++) {
    PndTrkHit *hitA = (PndTrkHit*) fHitMap->GetSeeds().At(i);
    //    hitA->DrawTube(kRed);
  }
  for(int i = 0; i < fHitMap->GetCandseeds().GetEntriesFast(); i++) {
    PndTrkHit *hitA = (PndTrkHit*) fHitMap->GetCandseeds().At(i);
    //    hitA->DrawTube(kBlue);
  }
  for(int i = 0; i < fHitMap->GetIndivisibles().GetEntriesFast(); i++) {
    PndTrkHit *hitA = (PndTrkHit*) fHitMap->GetIndivisibles().At(i);
    //    hitA->DrawTube(kOrange);
    TObjArray neighs = fHitMap->GetNeighboringsToHit(hitA);
    for(int j = 0; j < neighs.GetEntriesFast(); j++) {
      PndTrkHit *hitB = (PndTrkHit*) neighs.At(j);
      TObjArray neighs2 = fHitMap->GetNeighboringsToHit(hitA);
      if(neighs2.GetEntriesFast() > 2) {
	int counter = 0;
	PndSttTube *tubeB = (PndSttTube* ) fTubeArray->At(hitB->GetTubeID());
	for(int k = 0; k < neighs2.GetEntriesFast(); k++) {
	  PndTrkHit *hitC = (PndTrkHit*) neighs2.At(k);
	  PndSttTube *tubeC = (PndSttTube* ) fTubeArray->At(hitC->GetTubeID());
	  cout << "tubes " << tubeB << " "<< tubeC << endl;
	  if(tubeB->GetLayerID() == tubeC->GetLayerID()) continue;
	  counter++;
	}
	if(counter > 2) continue;
      }
      //  hitB->DrawTube(kYellow);
    }
    // display->Update();
    // display->Modified();
    // cin >> goOnChar;

  }
  
  //     for(int i = 0; i < limits.GetEntriesFast(); i++) {
  //        PndTrkHit *hitA = (PndTrkHit*) limits.At(i);
  //        hitA->DrawTube(kYellow);
  //      }
  
  display->Update();
  display->Modified();
  cin >> goOnChar;
}



ClassImp(PndTrkShortTrackFinder)
