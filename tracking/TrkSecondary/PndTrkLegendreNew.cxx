////////////////////////////////////////////////////////////
//
// PndTrkLegendreNew
// 
// Class for secondary track pattern recognition
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//                                   
////////////////////////////////////////////////////////////

#include "PndTrkLegendreNew.h"

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
// tracking 
#include "PndTrkClusterList.h"
#include "PndTrkGlpkFits.h"
#include "PndTrkClean.h"
#include "PndTrkNeighboringMap.h"

#include <iostream>
    
using namespace std;


// -----   Default constructor   -------------------------------------------
PndTrkLegendreNew::PndTrkLegendreNew() : FairTask("secondary track finder", 0), fDisplayOn(kFALSE), fPersistence(kTRUE), fUseMVDPix(kTRUE), fUseMVDStr(kTRUE), fUseSTT(kTRUE), fSecondary(kFALSE), fMvdPix_RealDistLimit(1000), fMvdStr_RealDistLimit(1000), fStt_RealDistLimit(1000), fMvdPix_ConfDistLimit(1000), fMvdStr_ConfDistLimit(1000), fStt_ConfDistLimit(1000), fInitDone(kFALSE) {
  sprintf(fSttBranch,"STTHit");
  sprintf(fMvdPixelBranch,"MVDHitsPixel");
  sprintf(fMvdStripBranch,"MVDHitsStrip");
  PndGeoHandling::Instance();
}

PndTrkLegendreNew::PndTrkLegendreNew(int verbose) : FairTask("secondary track finder", verbose), fDisplayOn(kFALSE), fPersistence(kTRUE), fUseMVDPix(kTRUE), fUseMVDStr(kTRUE), fUseSTT(kTRUE), fSecondary(kFALSE), fMvdPix_RealDistLimit(1000), fMvdStr_RealDistLimit(1000), fStt_RealDistLimit(1000), fMvdPix_ConfDistLimit(1000), fMvdStr_ConfDistLimit(1000), fStt_ConfDistLimit(1000), fInitDone(kFALSE) {
  sprintf(fSttBranch,"STTHit");
  sprintf(fMvdPixelBranch,"MVDHitsPixel");
  sprintf(fMvdStripBranch,"MVDHitsStrip");
  PndGeoHandling::Instance();
}

// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndTrkLegendreNew::~PndTrkLegendreNew() { 

}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndTrkLegendreNew::Init() {
  
  fEventCounter = 0;

  fMvdPix_RealDistLimit = 0.5; // CHECK limits
  fMvdStr_RealDistLimit = 0.5; // CHECK limits
  fStt_RealDistLimit =  1.5 * 0.5; // CHECK limits
  fMvdPix_ConfDistLimit = 0.003; // CHECK limits
  fMvdStr_ConfDistLimit = 0.003; // CHECK limits
  fStt_ConfDistLimit =  0.001; // CHECK limits
  if(fSecondary) {
    fMvdPix_RealDistLimit = 1.; // CHECK limits
    fMvdStr_RealDistLimit = 1.; // CHECK limits
    fMvdPix_ConfDistLimit = 0.007; // CHECK limits
    fMvdStr_ConfDistLimit = 0.007; // CHECK limits
   }
   

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndTrkLegendreNew::Init: "
	 << "RootManager not instantiated, return!" << endl;
    return kFATAL;
  }

  // -- HITS -------------------------------------------------
  //
  // STT
  fSttHitArray = (TClonesArray*) ioman->GetObject(fSttBranch);
  if ( ! fSttHitArray ) {
    cout << "-W- PndTrkLegendreNew::Init: "
	 << "No STTHit array, return!" << endl;
    return kERROR;
  }
  //
  // MVD PIXEL
  fMvdPixelHitArray = (TClonesArray*) ioman->GetObject(fMvdPixelBranch);
  if ( !fMvdPixelHitArray){
    std::cout << "-W- PndTrkLegendreNew::Init: " << "No MVD Pixel hitArray, return!" << std::endl;
    return kERROR;
  }
  //
  // MVD STRIP
  fMvdStripHitArray = (TClonesArray*) ioman->GetObject(fMvdStripBranch);
  if ( !fMvdStripHitArray){
    std::cout << "-W- PndTrkLegendreNew::Init: " << "No MVD Strip hitArray, return!" << std::endl;
    return kERROR;
  }

  fTrackArray = new TClonesArray("PndTrack");
  fTrackCandArray = new TClonesArray("PndTrackCand");
  ioman->Register("Track", "pr", fTrackArray, fPersistence); // CHECK
  ioman->Register("TrackCand",  "pr", fTrackCandArray, fPersistence); // CHECK

  
  // ----------------------------------------   maps of STT tubes
  fMapper = new PndSttMapCreator(fSttParameters);
  fTubeArray = fMapper->FillTubeArray();
  // ----------------------------------------------------  end map
 
  if(fDisplayOn) {
    display = new TCanvas("display", "display", 0, 0, 800, 800); // CHECK
    //    display->Divide(2, 2);
  }

  legendre = new PndTrkLegendreTransform();
  if(fSecondary) legendre->SetUpLegendreHisto(90, 0, 180, 1000, -1.5, 1.5); // CHECK
  else legendre->SetUpLegendreHisto();
  legendre->SetUpZoomHisto();

  conform = new PndTrkConformalTransform();

  tools = new PndTrkTools();
  fFitter = new PndTrkFitter(fVerbose);

  return kSUCCESS;

}

// -------------------------------------------------------------------------

void PndTrkLegendreNew::SetParContainers() {
  FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}
 
// -------------------------------------------------------------------------
 

void PndTrkLegendreNew::Initialize() {
  
  stthitlist = new PndTrkSttHitList(fTubeArray);
  mvdpixhitlist = new PndTrkSdsHitList(MVDPIXEL);
  mvdstrhitlist = new PndTrkSdsHitList(MVDSTRIP);

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

  //   conformalhitlist = new PndTrkConformalHitList();
  fFoundPeaks.clear();

  fInitDone = kTRUE;
  //  stthitlist->PrintSectors();
}


void PndTrkLegendreNew::Exec(Option_t* opt) {
  fTrackArray->Delete();
  fTrackCandArray->Delete();
  if(fVerbose > 0) cout << "*********************** " << fEventCounter << " ***********************" << endl;
 

  Initialize();
  if(fVerbose > 1) {
    cout << "number of stt    hits " << fSttHitArray->GetEntriesFast() << endl;
    cout << "number of mvdpix hits " << fMvdPixelHitArray->GetEntriesFast() << endl;
    cout << "number of mvdstr hits " << fMvdStripHitArray->GetEntriesFast() << endl;
  }
 
  PndTrkClusterList clusterlist;
  // PndTrkCluster *cluster = NULL;


  if(fDisplayOn)  {
    Refresh();
    char goOnChar;
    display->Update();
    display->Modified();
    cout << " STARTING" << endl;
    cin >> goOnChar;
    display->Update();
    display->Modified();
  }

  TObjArray limits;
  TObjArray sector[6];
  PndTrkNeighboringMap hitmap(fTubeArray);
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

  hitmap.SetOwnerValue(kTRUE); // CHECK
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
   
    cout << "HIT: " << hit->GetHitID() << " has " << neighborings->GetEntriesFast() << " hits" << endl;
    hitmap.AddNeighboringsToHit(hit, neighborings);
  }

  //  neighborings = NULL;
  //   delete neighborings;

  if(fDisplayOn) {
    if(1 == 1)  DrawLists(&hitmap);
    if(1 == 2)  DrawNeighborings(&hitmap);
  }

  // get seeds *********************************************8
  TObjArray seeds = hitmap.GetSeeds();
  neighborings = NULL;
  
  // ----------------- loop over seeds
  for(int iseed = 0; iseed < seeds.GetEntriesFast(); iseed++) {
    PndTrkCluster *cluster = new PndTrkCluster();
    PndTrkHit *seedhit = (PndTrkHit*) seeds.At(iseed);
    
    // is it already used
    if(seedhit->IsUsed() == kTRUE) continue;
    
    int seedtubeID = seedhit->GetTubeID();
    PndSttTube *seedtube = (PndSttTube*) fTubeArray->At(seedtubeID);
    int seedlayerID = seedtube->GetLayerID();
    
    // add hit to cluster
    cluster->AddHit(seedhit);

    //    if(fDisplayOn) {
    //       char goOnChar;
    //       cin >> goOnChar;
    //       cout << "SEED " << seedtubeID << endl; 
    //       cluster->LightUp();
    //       display->Update();
    //       display->Modified();
    //     }

    // add cluster to clusterlist
    clusterlist.AddCluster(cluster);

    int nlastadded = 1, addedcounter = 0;
    // cout << "nlastadded to " << seedhit->GetHitID() << "(" << seedtubeID << ")" << " " << nlastadded << endl;
    //    if(nlastadded == 0) continue;
    while(nlastadded > 0) {
      // loop on the last nlastadded hits to this cluster
      // example: add to a 5 hits cluster: 0 1 2 3 4
      // the hits no. 5, 6, 7
      // --> nlastadded = 3 & nof hits in cluster = 5 + 3 = 8
      // 7 6 5 = 8 - 3
      // here loop from hit 8 - 1 = 7 to hit 8 - 3 = 5

      addedcounter = 0;

      // cout << "@@@@@@@@@@@@@@@@@ loop on the last " << nlastadded << " hits of cluster" << endl;
      //   for(int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
      // 	PndTrkHit *hit = cluster->GetHit(ihit);
      // 	cout << " " << hit->GetHitID() ;
      //       }
      //       cout << endl;


      int nclusterhits = cluster->GetNofHits();
      for(int iadded = nclusterhits - 1; iadded >= (nclusterhits - nlastadded); iadded--) {
	PndTrkHit *addedhit = cluster->GetHit(iadded);
	neighborings = hitmap.GetNeighboringsToHit(addedhit);
	if(neighborings->GetEntriesFast() == 0) continue;
	//	cout << "hit " << addedhit->GetHitID() << "(" << addedhit->GetTubeID() << ")" << " has " << neighborings->GetEntriesFast()  << " neighborigns: " << endl;

	// loop over the neighborings and add them all
	for(int ineigh = 0; ineigh < neighborings->GetEntriesFast(); ineigh++)
	  {
	    PndTrkHit *neighhit = (PndTrkHit*) neighborings->At(ineigh);
	    //  cout << " " << neighhit->GetHitID() << "(" << neighhit->GetTubeID() << ")";
	    if(cluster->DoesContain(neighhit) == kTRUE) {
	      //  cout << "UN-ADDED, in cluster already" << endl;
	      continue;
	    }
	    cluster->AddHit(neighhit);
	    addedcounter++;
	    //  cout << " - ADDED; ";

	    //     if(fDisplayOn) {
	    // 	      char goOnChar;
	    // 	      cin >> goOnChar;
	    // 	      cluster->LightUp();
	    // 	      display->Update();
	    // 	      display->Modified();
	    // 	      //  cin >> goOnChar;
	    // 	    }
	    
	  }


      }
      //       cout << endl;
      nlastadded = addedcounter;
    }
    //     cout << "NEXT seed " << endl;
  }

  cout << "CLUSTERLIST " << clusterlist.GetNofClusters() << endl;



  // ---------- PRINT ---------------------
  for(int iclus = 0; iclus < clusterlist.GetNofClusters(); iclus++) {
    PndTrkCluster *cluster = clusterlist.GetCluster(iclus);
    // if(cluster->GetNofHits() < 3) continue;
    cout << "CLUSTER " << iclus << ":";
    if(fDisplayOn)  {
      char goOnChar;
      cin >> goOnChar;
      Refresh();
      cluster->LightUp();
      display->Update();
      display->Modified();
    }
      
    for(int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
      PndTrkHit *hit = cluster->GetHit(ihit);
      cout << " " << hit->GetHitID();
    }

    cout << endl;
  }
  // ---------------------------------------

  if(fDisplayOn) {
    char goOnChar;
    display->Update();
    display->Modified();
    cout << "Finish? ";
    cin >> goOnChar;
  }
  
  
  Reset();

}

void PndTrkLegendreNew::Reset()
{
  
  if(fDisplayOn) {
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
    if(fTimer) {
      fTimer->Stop();
      fTime += fTimer->RealTime();
      
      if(fVerbose > 0) cerr << fEventCounter << " Real time " << fTime << " s" << endl;
    }
  }

  fInitDone = kFALSE;
}

// ============================================================================================
// DISPLAY  ***********************************************************************************
// ============================================================================================

void PndTrkLegendreNew::Refresh(){
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
  if(fVerbose)  cout << "Refresh stop" << endl;

}

void PndTrkLegendreNew::DrawHits(PndTrkHitList *hitlist) {
  display->cd(1);
  hitlist->Draw();
  display->Update();
  display->Modified();
}

void PndTrkLegendreNew::DrawGeometry() {
  if(hxy == NULL)  hxy = new TH2F("hxy", "xy plane", 100, -43, 43, 100, -43, 43);
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

// draw lists ---------
void PndTrkLegendreNew::DrawLists(PndTrkNeighboringMap *hitmap) {
  char goOnChar;
  cout << "new hit?" << endl;
  cin >> goOnChar;
  Refresh(); 
  
  
  for(int i = 0; i < hitmap->GetStandalone().GetEntriesFast(); i++) {
    PndTrkHit *hitA = (PndTrkHit*) hitmap->GetStandalone().At(i);
    hitA->DrawTube(kGreen);
  }
  for(int i = 0; i < hitmap->GetSeeds().GetEntriesFast(); i++) {
    PndTrkHit *hitA = (PndTrkHit*) hitmap->GetSeeds().At(i);
    hitA->DrawTube(kRed);
  }
  for(int i = 0; i < hitmap->GetCandseeds().GetEntriesFast(); i++) {
    PndTrkHit *hitA = (PndTrkHit*) hitmap->GetCandseeds().At(i);
    hitA->DrawTube(kBlue);
     }
  
  //     for(int i = 0; i < limits.GetEntriesFast(); i++) {
  //        PndTrkHit *hitA = (PndTrkHit*) limits.At(i);
  //        hitA->DrawTube(kYellow);
     //      }
  
  display->Update();
  display->Modified();
  cin >> goOnChar;
}


// draw neighborings ---------
void PndTrkLegendreNew::DrawNeighborings(PndTrkNeighboringMap *hitmap) {
  char goOnChar;
 
  for(int ihit = 0; ihit < stthitlist->GetNofHits(); ihit++) {
    cout << "new neigh hit?" << endl;
    cin >> goOnChar;
    PndTrkHit *hit = stthitlist->GetHit(ihit);
    DrawNeighboringsToHit(hitmap, hit);
    cin >> goOnChar;
  }

}

void PndTrkLegendreNew::DrawNeighboringsToHit(PndTrkNeighboringMap *hitmap, PndTrkHit *hit) {

 
  Refresh(); 
  hit->DrawTube(kYellow);
   PndSttTube *tube = (PndSttTube*) fTubeArray->At(hit->GetTubeID());
 TObjArray *neighs = hitmap->GetNeighboringsToHit(hit);


 cout << "HIT " << hit->GetHitID() << "(" << hit->GetTubeID() << "/" << tube->GetLayerID() << ")" << " has " << neighs->GetEntriesFast() << " neighborings: ";
  for(int i = 0; i < neighs->GetEntriesFast(); i++) {
    PndTrkHit *hitA = (PndTrkHit*) neighs->At(i);
    hitA->DrawTube(kCyan);
     PndSttTube *tubeA = (PndSttTube*) fTubeArray->At(hitA->GetTubeID());
     cout << " " << hitA->GetHitID() << "(" << hitA->GetTubeID() << "/" << tubeA->GetLayerID() << ")";
  }
  cout << endl;
  display->Update();
  display->Modified();
  
}

ClassImp(PndTrkLegendreNew)

