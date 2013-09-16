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
    display->Divide(2, 2);
  }

  legendre = new PndTrkLegendreTransform();
  if(fSecondary) legendre->SetUpLegendreHisto(90, 0, 180, 1000, -1.5, 1.5); // CHECK
  else legendre->SetUpLegendreHisto();
  legendre->SetUpZoomHisto();

  conform = new PndTrkConformalTransform();

  tools = new PndTrkTools();
  fFitter = new PndTrkFitter(fVerbose);
  fHitMap = new PndTrkNeighboringMap(fTubeArray);

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
  
  // ############## I N I T I A L I Z A T I O N S ##############
  fTrackArray->Delete();
  fTrackCandArray->Delete();
  if(fVerbose > 0) cout << "*********************** " << fEventCounter << " ***********************" << endl;
 
  // initialize -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~
  Initialize();
  if(fVerbose > 1) {
    cout << "number of stt    hits " << fSttHitArray->GetEntriesFast() << endl;
    cout << "number of mvdpix hits " << fMvdPixelHitArray->GetEntriesFast() << endl;
    cout << "number of mvdstr hits " << fMvdStripHitArray->GetEntriesFast() << endl;
  }
 
  // initialize display -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-
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

  // initialize hit map -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-
  fHitMap->Clear();
  FillHitMap();
  // ##########################################################
  
  
  // fDisplayOn = kFALSE;
  // clusterization -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-----
  PndTrkClusterList clusterlist = CreateFullClusterization();
  int nofclusters = clusterlist.GetNofClusters(); 
  cout << "CLUSTERLIST " << nofclusters << endl;

  // loop on clusterlist -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~
  for(int iclus = 0; iclus < nofclusters; iclus++) {
    PndTrkCluster *cluster = clusterlist.GetCluster(iclus);
    // if(cluster->GetNofHits() < 3) continue;

    // print and display cluster -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~--
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

    // count tracks in skew sector -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~
    Int_t maxnoftracks =  CountTracksInSkewSector(cluster);

    //  if(maxnoftracks > 1) continue; // CHECK

    PndTrkClusterList partialcluslist;
    cout << "\033[1;36m ITERATIONS -----------------------> " << maxnoftracks << "\033[0m" << endl;
    for(int iter = 0; iter < maxnoftracks; iter++) {
      cout << "\033[1;36m ############### ITER "  << iter << "\033[0m" << endl;
      // fitting procedure -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~
      Int_t nhits = ClusterToConformal(cluster);
      PndTrkTrack *track = LegendreFit(cluster);
      if(track == NULL) continue;
      PndTrkCluster *thiscluster = CreateClusterAroundTrack(track);
      partialcluslist.AddCluster(thiscluster);
      cout << "ADD CLUSTER TO PARTCLUSLIST " << thiscluster->GetNofHits() << endl;
      PndTrkCluster *remainingcluster = new PndTrkCluster();
      for(int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
	PndTrkHit *hit = cluster->GetHit(ihit);
	if(thiscluster->DoesContain(hit)) continue;
	remainingcluster->AddHit(hit);
      }
      cluster = remainingcluster;
      cout << "partialclusterlist " << partialcluslist.GetNofClusters() << endl;
      cout << "remainning hits " << remainingcluster->GetNofHits() << endl;

      // if there are still a lot of hits not assigned, maybe 
      // there is another track...
      if(remainingcluster->GetNofHits() > 15 && iter == maxnoftracks - 1) {
	cout << "\033[1;31m LETS TRY ANOTHER ONE -------- BONUS ------------ \033[0m" << endl;
	
	maxnoftracks++;
      }
    }

    cout << "partialclusterlist " << partialcluslist.GetNofClusters() << endl;
    for(int jclus = 0; jclus < partialcluslist.GetNofClusters(); jclus++) {
      PndTrkCluster *partcluster = partialcluslist.GetCluster(jclus);
      cout << "ADD CLUSTER TO NEW CLUSLIST " << cluster->GetNofHits() << " " << partcluster->GetNofHits() << endl;
	 
      if(jclus == 0) clusterlist.ReplaceCluster(iclus, partcluster);
      else clusterlist.AddCluster(partcluster);
    }
    cout << "clusterlist now is " << clusterlist.GetNofClusters() << endl;
    cout << "@@@@@@ " <<     clusterlist.GetCluster(iclus)->GetNofHits() << endl;

  }
  // ---------------------------------------
  

  // reloop over the clusterlist -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~
  nofclusters = clusterlist.GetNofClusters(); 
  cout << "\033[1;35m (AFTER LEGENDRE AND RECLUSTERING) NEW CLUSTERLIST " << nofclusters << " \033[0m" << endl;
  // fDisplayOn = kTRUE;
  
  std::vector< PndTrkTrack* > tracklist;
  // loop on clusterlist -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~
  for(int iclus = 0; iclus < nofclusters; iclus++) {
    PndTrkCluster *cluster = clusterlist.GetCluster(iclus);
    // if(cluster->GetNofHits() < 3) continue;

    // print and display cluster -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~--
    cout << "CLUSTER " << iclus << ":";
    if(fDisplayOn)  {
      char goOnChar;
      cin >> goOnChar;
      Refresh();
      cluster->LightUp();
      display->Update();
      display->Modified();
    }
      
    // fit with analytical chi2 -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~--
    Int_t nhits = ClusterToConformal(cluster);
    PndTrkTrack *track = LegendreFit(cluster);
    if(track == NULL) continue;
    PndTrkCluster *thiscluster = CreateClusterAroundTrack(track);
    double fitm, fitp;
    // AnalyticalFit(thiscluster, track->GetCenter().X(), track->GetCenter().Y(), track->GetRadius(), fitm, fitp);
    FromRealToConformalTrack(track->GetCenter().X(), track->GetCenter().Y(), track->GetRadius(), fitm, fitp);
    double fitm2, fitp2;
    AnalyticalFit2(thiscluster, fitm, fitp, fitm2, fitp2);

    double xc, yc, R;
    FromConformalToRealTrack(fitm, fitp, xc, yc, R);

    track->SetCluster(thiscluster);
    track->SetCenter(xc, yc);
    track->SetRadius(R);

    tracklist.push_back(track);
  }
  //  fDisplayOn = kTRUE;

  if(fDisplayOn) {
    
    Refresh();
    for(int itrk = 0; itrk < tracklist.size(); itrk++) {
      PndTrkTrack *track = tracklist.at(itrk);
      track->Draw(kRed);
      display->Update();
      display->Modified();
    }
  }


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

void PndTrkLegendreNew::DrawLegendreHisto() {
  display->cd(3);
  legendre->Draw();
  display->Update();
  display->Modified();
}

void PndTrkLegendreNew::RefreshConf() { // CHECK delete
  // CHECK
  char goOnChar;
  //  cout << "RefreshConf?" << endl;
  //  cin >> goOnChar;
  //  cout << "REFRESHING CONF" << endl;
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

void PndTrkLegendreNew::DrawGeometryConf(double x1, double y1, double x2, double y2) {
  // CHECK
  char goOnChar;
  //  cout << "DRAWING GEOMETRY CONF" << endl;
  // cin >> goOnChar;
  
  // CHECK previous calculations, now not used;
  if(huv == NULL)  huv = new TH2F("huv", "uv plane", 100, x1, y1, 100, x2, y2);
  else {
    huv->Reset(); 
    huv->GetXaxis()->SetLimits(x1, y1);
    huv->GetYaxis()->SetLimits(x2, y2);
   }
  display->cd(2);
  huv->Draw();
  display->Update();
  display->Modified();  
 
}

// draw lists ---------
void PndTrkLegendreNew::DrawLists() {
  char goOnChar;
  cout << "new hit?" << endl;
  cin >> goOnChar;
  Refresh(); 
  
  
  for(int i = 0; i < fHitMap->GetStandalone().GetEntriesFast(); i++) {
    PndTrkHit *hitA = (PndTrkHit*) fHitMap->GetStandalone().At(i);
    // hitA->DrawTube(kGreen);
  }
  for(int i = 0; i < fHitMap->GetSeeds().GetEntriesFast(); i++) {
    PndTrkHit *hitA = (PndTrkHit*) fHitMap->GetSeeds().At(i);
    //  hitA->DrawTube(kRed);
  }
  for(int i = 0; i < fHitMap->GetCandseeds().GetEntriesFast(); i++) {
    PndTrkHit *hitA = (PndTrkHit*) fHitMap->GetCandseeds().At(i);
    //  hitA->DrawTube(kBlue);
  }
  for(int i = 0; i < fHitMap->GetIndivisibles().GetEntriesFast(); i++) {
    PndTrkHit *hitA = (PndTrkHit*) fHitMap->GetIndivisibles().At(i);
    hitA->DrawTube(kOrange);
    TObjArray *neighs = fHitMap->GetNeighboringsToHit(hitA);
    for(int j = 0; j < neighs->GetEntriesFast(); j++) {
      PndTrkHit *hitB = (PndTrkHit*) neighs->At(j);
      TObjArray *neighs2 = fHitMap->GetNeighboringsToHit(hitA);
      if(neighs2->GetEntriesFast() > 2) {
	int counter = 0;
	PndSttTube *tubeB = (PndSttTube* ) fTubeArray->At(hitB->GetTubeID());
	for(int k = 0; k < neighs2->GetEntriesFast(); k++) {
	  PndTrkHit *hitC = (PndTrkHit*) neighs2->At(k);
	  PndSttTube *tubeC = (PndSttTube* ) fTubeArray->At(hitC->GetTubeID());
	  cout << "tubes " << tubeB << " "<< tubeC << endl;
	  if(tubeB->GetLayerID() == tubeC->GetLayerID()) continue;
	  counter++;
	}
	if(counter > 2) continue;
      }
      hitB->DrawTube(kYellow);
    }
 //    display->Update();
//     display->Modified();
    //    cin >> goOnChar;

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
void PndTrkLegendreNew::DrawNeighborings() {
  char goOnChar;
 
  for(int ihit = 0; ihit < stthitlist->GetNofHits(); ihit++) {
    cout << "new neigh hit?" << endl;
    cin >> goOnChar;
    PndTrkHit *hit = stthitlist->GetHit(ihit);
    DrawNeighboringsToHit(hit);
    cin >> goOnChar;
  }

}

void PndTrkLegendreNew::DrawNeighboringsToHit(PndTrkHit *hit) {

 
  Refresh(); 
  hit->DrawTube(kYellow);
   PndSttTube *tube = (PndSttTube*) fTubeArray->At(hit->GetTubeID());
 TObjArray *neighs = fHitMap->GetNeighboringsToHit(hit);


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

void PndTrkLegendreNew::DrawConfHit(double u, double v, double r, int marker) {
  display->cd(2);
  if(r >= 0) {
    TArc *arc = new TArc(u, v, r);
    arc->SetFillStyle(0);
    arc->Draw("SAME");
  }
  else {
    TMarker *mrk = new TMarker(u, v, marker);
    mrk->Draw("SAME");
  }
  //  display->Update();
  //   display->Modified();
}

Int_t PndTrkLegendreNew::FillConformalHitList(PndTrkCluster *cluster) { 

  conformalhitlist->SetConformalTransform(conform);
    
  for(int jhit = 0; jhit < cluster->GetNofHits(); jhit++) {
    PndTrkHit *hit = cluster->GetHit(jhit);
    if(hit == fRefHit) continue;
    PndTrkConformalHit * chit = NULL;
    //    cout << "HIT " << hit->GetHitID() << " " << hit->IsSttParallel() << " " << hit->IsSttSkew() << endl;
    if(hit->IsSttParallel() == kTRUE) chit = conform->GetConformalSttHit(hit);
    else continue; // chit = conform->GetConformalHit(hit);
    conformalhitlist->AddHit(chit);  
    //    cout << hit->GetPosition().X() << " " << hit->GetPosition().Y() << " " << hit->IsSttParallel() << " " << " to CONFORMAL " << chit->GetU() << " " << chit->GetV() << " " << chit->GetIsochrone() << endl;   

  }
  return conformalhitlist->GetNofHits();
}

// ----------------------- FOR SECONDARIES
PndTrkHit *PndTrkLegendreNew::FindSttReferenceHit(int isec) {
  int ntot = 0; 
  if(isec == -1) ntot = stthitlist->GetNofHits();
  else  ntot = stthitlist->GetNofHitsInSector(isec);
  
  if(ntot == 0) return NULL;


  int tmphitid = -1;
  Double_t tmpiso = 1.;
  PndTrkHit *refhit = NULL;
  for(int jhit = 0; jhit < ntot; jhit++) {
    PndTrkHit *hit = NULL;
    if(isec == -1) hit = stthitlist->GetHit(jhit);
    else  hit = stthitlist->GetHitFromSector(jhit, isec);
    
    if(hit->IsUsed()) { 
      if(fVerbose > 1) cout << "STT hit " << jhit << "already used " << endl; 
      continue; }
    if(hit->IsSttSkew()) continue;
    if(hit->GetIsochrone() < tmpiso) {
      tmphitid = jhit;
      tmpiso = hit->GetIsochrone();
      refhit = hit;
    }
  }   
  if(tmphitid == -1)  return NULL;
  
  // PndTrkHit *refhit =  &thishitlist[tmphitid];
  if(fVerbose > 1) cout << "STT REFERENCE HIT " <<  tmphitid << " " << refhit->GetIsochrone() << endl;
  return refhit;

}
 
PndTrkHit *PndTrkLegendreNew::FindMvdPixelReferenceHit()
  {
  if(mvdpixhitlist->GetNofHits() == 0) return NULL;
  // loop on mvd pix hits
  int tmphitid = -1;
  PndTrkHit *refhit = NULL;
  for(int jhit = 0; jhit < mvdpixhitlist->GetNofHits(); jhit++) {
    PndTrkHit *hit = mvdpixhitlist->GetHit(jhit);
    if(hit->IsUsed()) { 
      if(fVerbose > 1) cout << "already used V" << endl; 
      continue; 
    }
    tmphitid = jhit;
    break;
  }   
  if(tmphitid == -1)  return NULL;
  refhit = mvdpixhitlist->GetHit(tmphitid);
   if(fVerbose > 1) cout << "MVD PIXEL REFERENCE HIT " << refhit->GetHitID() << endl;
  return refhit;
}

PndTrkHit *PndTrkLegendreNew::FindMvdStripReferenceHit()
{
  if(mvdstrhitlist->GetNofHits() == 0) return NULL;
  // loop on mvd str hits
  int tmphitid = -1;
  PndTrkHit *refhit = NULL;
  for(int jhit = 0; jhit < mvdstrhitlist->GetNofHits(); jhit++) {
    PndTrkHit *hit = mvdstrhitlist->GetHit(jhit);
    if(hit->IsUsed()) { 
       if(fVerbose > 1) cout << "already used V" << endl; 
      continue; 
    }
    tmphitid = jhit;
    break;
  }   
  if(tmphitid == -1)  return NULL;
  refhit = mvdstrhitlist->GetHit(tmphitid);
   if(fVerbose > 1) cout << "MVD STRIP REFERENCE HIT " << refhit->GetHitID() << endl;
  return refhit;
}

PndTrkHit *PndTrkLegendreNew::FindMvdReferenceHit()
{
  PndTrkHit *refhit = NULL;
  refhit = FindMvdStripReferenceHit();
  // refhit = FindMvdPixelReferenceHit();
  if(refhit != NULL) return refhit;
  //  refhit = FindMvdStripReferenceHit();
  FindMvdPixelReferenceHit();
  return refhit;
}

PndTrkHit *PndTrkLegendreNew::FindReferenceHit()
{
  PndTrkHit *refhit = NULL;
  // refhit = FindMvdReferenceHit();
  refhit = FindSttReferenceHit();
  if(refhit != NULL) return refhit;
  // refhit = FindSttReferenceHit();
  FindMvdReferenceHit();

 return refhit;
}

PndTrkHit *PndTrkLegendreNew::FindReferenceHit(PndTrkCluster *cluster) {
  int ntot = cluster->GetNofHits();
  //  cout << "FIND REFERENCE HIT " << ntot << endl;
  
  if(ntot == 0) return NULL;


  int tmphitid = -1;
  Double_t tmpiso = 1.;
  PndTrkHit *refhit = NULL;
  for(int jhit = 0; jhit < ntot; jhit++) {
    PndTrkHit *hit = cluster->GetHit(jhit);
    
    //   if(hit->IsUsed()) { 
    //       if(fVerbose > 1) cout << "STT hit " << jhit << "already used " << endl; 
    //       continue; }
    
    if(hit->IsStt()) {
      if(hit->IsSttParallel()) {
	if(hit->GetIsochrone() < tmpiso) {
	  tmphitid = jhit;
	  tmpiso = hit->GetIsochrone();
	  refhit = hit;
	}
      }
    }
    else {
      tmphitid = jhit;
      break;
    }
  }

  if(tmphitid == -1)  return NULL;
  refhit = cluster->GetHit(tmphitid);
  if(fVerbose > 1) cout << "REFERENCE HIT " << refhit->GetHitID() << " " << refhit->GetDetectorID() << endl;
  return refhit;


}
 

void PndTrkLegendreNew::ComputeTraAndRot(PndTrkHit *hit, Double_t &delta, Double_t trasl[2]) {
  
  trasl[0] = hit->GetPosition().X();
  trasl[1] = hit->GetPosition().Y();

  delta = 0.; // TMath::ATan2(hit->GetPosition().Y() - 0., hit->GetPosition().X() - 0.); // CHECK 
  
}
ClassImp(PndTrkLegendreNew)


 
void PndTrkLegendreNew::FillLegendreHisto(PndTrkCluster *cluster)
{
  // ---------------------------------------------------------------
  //  cout << "FILL LEGENDRE HISTO " << cluster->GetNofHits() << endl;

  for(int ihit = 0; ihit < conformalhitlist->GetNofHits(); ihit++) {
    PndTrkConformalHit *chit = conformalhitlist->GetHit(ihit);
    PndTrkHit *hit = chit->GetHit();
    if(cluster->DoesContain(hit) == kFALSE) continue;
    legendre->FillLegendreHisto(chit->GetU(), chit->GetV(), chit->GetIsochrone());
    if(fDisplayOn) {
      DrawConfHit(chit->GetU(), chit->GetV(), chit->GetIsochrone());
      //      cout << "conformal2: " << chit->GetU() << " " << chit->GetV() << " " << chit->GetIsochrone() << endl;   
    }
  }
}
 
void PndTrkLegendreNew::RePrepareLegendre(PndTrkCluster *cluster) {

  //    cout << "RESETTING LEGENDRE HISTO" << endl;
  legendre->ResetLegendreHisto();
 
  if(fDisplayOn) {
    RefreshConf();
    if(fSecondary) DrawGeometryConf(-1, 1, -1, 1);
    else   DrawGeometryConf(-0.07, 0.07, -0.07, 0.07);
  }
  // cout << "%%%%%%%%%%%%%%%%%%%% XY FINDE %%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
  FillLegendreHisto(cluster);
}

// void PndTrkLegendreNew::PrepareLegendre() {

//   //    cout << "RESETTING LEGENDRE HISTO" << endl;
//   legendre->ResetLegendreHisto();
 
//   if(fDisplayOn) {
//     RefreshConf();
//     if(fSecondary) DrawGeometryConf(-1., 1., -1., 1.);
//     else   DrawGeometryConf(-0.07, 0.07, -0.07, 0.07);
//   }
//   if(fVerbose > 1) cout << "%%%%%%%%%%%%%%%%%%%% XY FINDER %%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
//   FillLegendreHisto(0);
// }

// Int_t PndTrkLegendreNew::ApplyLegendre(double &theta_max, double &r_max) {
//   PrepareLegendre();
//   return ExtractLegendre(0, theta_max, r_max);
// }

Int_t PndTrkLegendreNew::ApplyLegendre(PndTrkCluster *cluster, double &theta_max, double &r_max) {
  RePrepareLegendre(cluster);
  return ExtractLegendre(1, theta_max, r_max);
}



Int_t  PndTrkLegendreNew::ExtractLegendre(Int_t mode, double &theta_max, double &r_max) {
  if(fDisplayOn) {
    char goOnChar;
    //      cin >> goOnChar;
    DrawLegendreHisto();
    //    cout << "LEGENDRE (nof conf hits = " <<  conformalhitlist->GetNofHits() << ")" << endl;
    display->cd();
    //      cin >> goOnChar;
    display->Update();
    display->Modified();
  }
  
  // FIND MAXIMUM IN LEGENDRE HISTO

  //  legendre->ApplyThresholdLegendreHisto(0.3);
  int  maxpeak = legendre->ExtractLegendreMaximum(theta_max, r_max);

  bool alreadythere = false;
  //  if(mode == 0) 
  {

    if(maxpeak <= 3) {
      //      if(fVerbose > 1)
     	cout << "\033[1;31m MAXPEAK " << maxpeak <<  ", BREAK NOW! \033[0m" << endl;
      return maxpeak;
    }
  
    for(int ialready = 0; ialready < fFoundPeaks.size(); ialready++) {
      std::pair<double, double> foundthetar = fFoundPeaks.at(ialready);
      double foundtheta = foundthetar.first;
      double foundr = foundthetar.second;
      // IF THIS PEAK WAS ALREADY FOUND, DELETE THE PEAK AND GO ON (TO AVOID INFINITE LOOPS)
      if(theta_max == foundtheta && r_max == foundr) {
	legendre->DeleteZoneAroundXYLegendre(theta_max, r_max);
	maxpeak = legendre->ExtractLegendreMaximum(theta_max, r_max);
	alreadythere = true;
	if(fVerbose > 0) cout << "OH NO! THIS PEAK IS ALREADY THERE" << endl;
	return -1;
      }
    }
    
    if(alreadythere == false) {
      std::pair<double, double> tr(theta_max, r_max);
      fFoundPeaks.push_back(tr);
    }
  }
  // ZOOM LEGENDRE HISTO
  legendre->SetUpZoomHisto(theta_max, r_max, 3, 0.005);
  //   cout << "THETA/R " << theta_max << " " << r_max << " maxpeak " << maxpeak << endl;

  for(int ihit = 0; ihit < conformalhitlist->GetNofHits(); ihit++) {
    PndTrkConformalHit *chit = conformalhitlist->GetHit(ihit);
    legendre->FillZoomHisto(chit->GetU(), chit->GetV(), chit->GetIsochrone());
  }

  if(mode == 0 && alreadythere == true) {
     cout << "THIS PEAK IS ALREADY THERE" << endl;
    legendre->DeleteZoneAroundXYZoom(theta_max, r_max);
  }

 int maxpeakzoom = legendre->ExtractZoomMaximum(theta_max, r_max);
  //  cout << "THETA/R ZOOM " << theta_max << " " << r_max <<  " maxpeakzoom " << maxpeakzoom << endl;

  if(fDisplayOn) {
    char goOnChar;
    display->cd(3);
    TMarker *mrk = new TMarker(theta_max, r_max, 29);
    mrk->Draw("SAME");
    display->cd(4);
    legendre->DrawZoom();
    mrk->Draw("SAME");
    display->Update();
    display->Modified();
    //      cin >> goOnChar;
  }
  
  return maxpeak;
}

void PndTrkLegendreNew::FromConformalToRealTrack(double fitm, double fitp, double &x0, double &y0, double &R) {
 // CHECK if this needs to be kept --> change xc0 to xc etc
  // center and radius
  Double_t xcrot0, ycrot0;
  ycrot0 = 1 / (2 * fitp);
  xcrot0 = - fitm * ycrot0;
  R =  sqrt(xcrot0 * xcrot0 + ycrot0 * ycrot0);
  // re-rotation and re-traslation of xc and yc
  // rotation    
  x0 = TMath::Cos(conformalhitlist->GetConformalTransform()->GetRotation())*xcrot0 - TMath::Sin(conformalhitlist->GetConformalTransform()->GetRotation())*ycrot0;
  y0 = TMath::Sin(conformalhitlist->GetConformalTransform()->GetRotation())*xcrot0 + TMath::Cos(conformalhitlist->GetConformalTransform()->GetRotation())*ycrot0;
  // traslation 
  x0 += conformalhitlist->GetConformalTransform()->GetTranslation().X();
  y0 += conformalhitlist->GetConformalTransform()->GetTranslation().Y();
}

void PndTrkLegendreNew::FromRealToConformalTrack(double x0, double y0, double R, double &fitm, double &fitp) {
 // CHECK if this needs to be kept --> change xc0 to xc etc
  Double_t xcrot0, ycrot0;
 
  // traslation 
  x0 -= conformalhitlist->GetConformalTransform()->GetTranslation().X();
  y0 -= conformalhitlist->GetConformalTransform()->GetTranslation().Y();

  /**
     |x|  |c -s||xr|  ---> |xr|  | c s||x|
     |y|  |s  c||yr|       |yr|  |-s c||y|
  **/

  // rotation    
  xcrot0 = TMath::Cos(conformalhitlist->GetConformalTransform()->GetRotation())*x0 + TMath::Sin(conformalhitlist->GetConformalTransform()->GetRotation())*y0;
  ycrot0 = - TMath::Sin(conformalhitlist->GetConformalTransform()->GetRotation())*x0 + TMath::Cos(conformalhitlist->GetConformalTransform()->GetRotation())*y0;
  
  // yr = 1/ (2 p) --> p = 1 / (2 yr)
  // xr = - m / (2 p) --> m = - 2 p xr
  fitp = 1 / (2 * ycrot0);
  fitm = - 2 * fitp *  xcrot0;
}

void PndTrkLegendreNew::FillHitMap() {
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
//     if(fDisplayOn) { 
//       Refresh();
//     }
  
    fHitMap->AddNeighboringsToHit(hit, neighborings);

//     if(fDisplayOn) {
//       char goOnChar;
//       display->Update();
//       display->Modified();
//       cout << " go on?" << endl;
//       cin >> goOnChar;
//     }
  }
  //  neighborings = NULL;
  //   delete neighborings;

  if(fDisplayOn) {
    if(1 == 1)  DrawLists();
    if(1 == 2)  DrawNeighborings();
    // ================================================
    Refresh();
    char goOnChar;
    for(int ihit = 0; ihit < stthitlist->GetNofHits(); ihit++) {
      PndTrkHit *stthit = stthitlist->GetHit(ihit);
      TObjArray *indiv = fHitMap->GetIndivisiblesToHit(stthit);
      for(int jhit = 0; jhit < indiv->GetEntriesFast(); jhit++) {
	PndTrkHit *stthit2 = (PndTrkHit*) indiv->At(jhit);
	stthit2->Draw(kOrange);
 	stthit->Draw(kOrange);
      }
    }

    display->Update();
    display->Modified();
    cout << " STARTING" << endl;
    cin >> goOnChar;
    display->Update();
    display->Modified();
    // ================================================
  }
  cout << "PRINT INDIVISIBILE MAP" << endl;
  fHitMap->PrintIndivisibleMap();
}


PndTrkClusterList PndTrkLegendreNew::CreateFullClusterization() {
  PndTrkClusterList clusterlist;

  // get seeds *********************************************8
  TObjArray seeds = fHitMap->GetSeeds();
  TObjArray *neighborings = NULL;
  
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
	neighborings = fHitMap->GetNeighboringsToHit(addedhit);
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
  return clusterlist;
}

PndTrkClusterList PndTrkLegendreNew::CreateFullClusterization2() {
/**
   PndTrkClusterList clusterlist;

  // get seeds *********************************************8
  TObjArray seeds = fHitMap->GetSeeds();
  neighborings = NULL;
**/
}

Int_t PndTrkLegendreNew::CountTracksInSkewSector(PndTrkCluster *cluster) {
  // check how many neighboring tubes each skew 
  // tube on a layer has on that same layer
  // the total number of tubes on a layer minus the number 
  // of neighboring couples gives the number of tracks:
  // example 1 with 3 tracks:
  // OOO OO OOO are tube no.: 0 1 2   3 4  5 6 7
  // nof tubes on the layer = 8
  // calculation of neighborings:
  // 0 <--> 1
  // 1 <--> 2
  // 3 <--> 4
  // 5 <--> 6
  // 6 <--> 7
  // so, nof neigboging couples = 5
  // Then: noftubes (8) - nofcouples (5) = 3 tracks !!OK!!
  cout << "COUNT TRACKS IN SKEW SECTOR" << endl;
  int nofhitsinlay[30]; // CHECK initialize this
  for(int ilay = 0; ilay < 30; ilay++) nofhitsinlay[ilay] = 0;

  for(int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster->GetHit(ihit);
    PndSttTube *tube = (PndSttTube*) fTubeArray->At(hit->GetTubeID());
    hit->SetSortVariable(tube->GetLayerID());
    nofhitsinlay[tube->GetLayerID()]++;
    //    cout << "hit " << ihit << " " << tube->GetLayerID() << " " << nofhitsinlay[tube->GetLayerID()] << endl;

  }
  cluster->Sort();

  for(int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster->GetHit(ihit);
    PndSttTube *tube = (PndSttTube*) fTubeArray->At(hit->GetTubeID());
    //    cout << "SORTED " << ihit << " " << hit->GetHitID() << " " << tube->GetLayerID() << endl;
  }

  int maxnoftracks = 1;
  int tmplayid = -1;
  int counter = 0, counter1 = 0;;
  int isneigh = 0;
  // loop over cluster hits
  for(int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster->GetHit(ihit);
    counter++; 

    // skew sector?
    if(hit->IsSttParallel() == kTRUE) continue;
    PndSttTube *tube = (PndSttTube*) fTubeArray->At(hit->GetTubeID());

    int layid = tube->GetLayerID();
    if(nofhitsinlay[layid] <= 1) continue;

    // new layer?
    if(layid != tmplayid) {
      /**
	 int noftracks = nofhitsinlay[tmplayid] - isneigh;
	 if(tmplayid != -1) cout << "CLUSTER CONTAINS @ LAYER " << tmplayid << " ACTUALLY " << nofhitsinlay[tmplayid] << " - " << isneigh << " = " << noftracks << " TRACKS" << endl;
	 if(noftracks > maxnoftracks) maxnoftracks = noftracks;
      **/
      isneigh = 0;
      tmplayid = layid;
      counter1 = 0;
      //	continue; //	break;
    }
    //    cout << "hit " << ihit << " on layid " << layid << "/ " <<  nofhitsinlay[layid] << endl;

    // count processed hits 
    // in this same layer
    counter1++;

    // if it is the last hit ==> all its
    // neighborings have already been taken
    // into account
    if(counter1 == nofhitsinlay[layid]) continue;

    for(int jhit = counter; jhit < counter + nofhitsinlay[layid] -  counter1; jhit++) {
      PndTrkHit *hit2 = cluster->GetHit(jhit);
      int tubeid2 =   hit2->GetTubeID();
      PndSttTube *tube2 = (PndSttTube*) fTubeArray->At(tubeid2);



      if(tube->GetLayerID() != tube2->GetLayerID()) cout << "ERROR" << tube->GetLayerID()  << " " << tube2->GetLayerID() << endl;
      //      cout << "compare " << ihit << "(" << hit->GetHitID() << "- " << hit->GetTubeID() << ") with " << jhit << " (" << hit2->GetHitID() << "- " << tubeid2 << ") from " << counter << " to " << counter + nofhitsinlay[layid] - 1 << endl;
      if(tube->IsNeighboring(tubeid2) == kTRUE) {
	isneigh++;
	//	cout << "isneigh " << isneigh << endl;
	//	// break;
      }
    }

    // if all the hits in the layer have been processed
    if(counter1 == nofhitsinlay[layid] - 1) {
      int noftracks = nofhitsinlay[layid] - isneigh;
      cout << "CLUSTER CONTAINS @ LAYER " << layid << " ACTUALLY " << nofhitsinlay[layid] << " - " << isneigh << " = " << noftracks << " TRACKS" << endl;
      if(noftracks > maxnoftracks) maxnoftracks = noftracks;
    }

  }
    
  
  cout << "THIS CLUSTER HAS A TOTAL OF " << maxnoftracks << " TRACKS" << endl;
  return maxnoftracks;
}


Int_t PndTrkLegendreNew::ClusterToConformal(PndTrkCluster *cluster) {
  // ================ --> TO CONFORMAL PLANE
  conformalhitlist = new PndTrkConformalHitList(); // CHECK
  // translation and rotation
  Int_t nchits = 0;
  Double_t delta = 0, trasl[2] = {0., 0.};
  if(fSecondary) {
    // translation and rotation - CHECK
    //	cout << " REFERENCE HIT " << cluster->GetNofHits() << endl;
    fRefHit = FindReferenceHit(cluster);
    if(fRefHit == NULL)  {
      //	  cout << "REFHIT " << fRefHit << endl;
      //	Reset();  
      return 0;
    }
    ComputeTraAndRot(fRefHit, delta, trasl);
  }
  //    
  cout << "DELTA " << delta << " TRASL " << trasl[0] << " " << trasl[1] << endl;
  conform->SetOrigin(trasl[0], trasl[1], delta);
  nchits = FillConformalHitList(cluster);
  
  return nchits;
}

PndTrkTrack * PndTrkLegendreNew::LegendreFit(PndTrkCluster *cluster) {

  cout << "APPLY LEGENDRE =======================" << endl;

  // reset the legendre histo for a new legendre fit
  legendre->ResetLegendreHisto();
  
  if(fDisplayOn) {
    RefreshConf();
    if(fSecondary) DrawGeometryConf(-1, 1, -1, 1);
    else   DrawGeometryConf(-0.07, 0.07, -0.07, 0.07);
  }
  
  // fill legendre histo with the cluster hits
  FillLegendreHisto(cluster); 
  double theta_max, r_max;
  // get the peak
  int maxpeak = ExtractLegendre(1, theta_max, r_max); // CHECK mode??
          
  if(maxpeak == 0) return NULL;
  // from theta/r to line parameters in CONFORMAL plane 
  double fitm, fitq;
  legendre->ExtractLegendreSingleLineParameters(fitm, fitq);
  if(fDisplayOn) {
    display->cd(2);
    TLine *line = new TLine(-10.07, fitq + fitm * (-10.07), 10.07, fitq + fitm * (10.07));
    line->Draw("SAME");
  }
    
  // from line parameters to center/radius in REAL plane
  Double_t xc, yc, R;
  FromConformalToRealTrack(fitm, fitq, xc, yc, R);
  cout << "\033[1;33m XR, YC, R: " << xc << " " << yc << " " << R << "\033[0m" << endl;
	//	cout << "start hit " << ihit << " " << hit->GetHitID() << " " << endsecid << " " << endlayid << endl;

  // create a track from the cluster
  PndTrkTrack *track = new PndTrkTrack(cluster, xc, yc, R);

  return track;
}


PndTrkCluster * PndTrkLegendreNew::CreateClusterAroundTrack(PndTrkTrack *track) {

  double R = track->GetRadius();
  double xc = track->GetCenter().X();
  double yc = track->GetCenter().Y();
  double fitm, fitp;
  FromRealToConformalTrack(xc, yc, R, fitm, fitp);


 PndTrkCluster *cluster = track->GetCluster();

  // create cluster depending on fitting
  double rmin = R - R * 0.10; // CHECK 20%?
  double rmax = R + R * 0.10; // "      "
  
  
  if(fDisplayOn) {
    display->cd(1);
    track->Draw(kRed);
    
    TArc *arcmin = new TArc(xc, yc, rmin);
    TArc *arcmax = new TArc(xc, yc, rmax);
      
    arcmin->SetFillStyle(0);
    arcmax->SetFillStyle(0);
    arcmin->SetLineColor(kGreen);
    arcmax->SetLineColor(kBlue);

 //    arcmin->Draw("SAME");
//     arcmax->Draw("SAME");

    display->Update();
    display->Modified();
    char goOnChar;
    cout << "want to go to new cluster?" << endl;
    cin >> goOnChar;
  }
    
  // create cluster depending on fitting
  PndTrkCluster *thiscluster = new PndTrkCluster();
  int startsecid = 1000, endsecid = -1, startlayid = 1000, endlayid = -1;
  double totaldistanceconf = 0,  chi2 = 0;
  // clean existing cluster
  for(int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster->GetHit(ihit);
    PndTrkConformalHit *chit = NULL;
    if(hit->IsSttParallel()) chit = conform->GetConformalSttHit(hit);
    else chit = conform->GetConformalHit(hit); // CHECK if skew?
    double distanceconf = fabs((chit->GetV() - fitm * chit->GetU() - fitp)/ TMath::Sqrt(fitm * fitm + 1));
 

    double distance = hit->GetXYDistance(TVector3(xc, yc, 0.));
    //    cout << "distance " << distance << " " << rmin << " " << rmax << " " << distanceconf << endl;
    if(distance <= rmax && distance >= rmin) {
      thiscluster->AddHit(hit);
      PndSttTube *tube = (PndSttTube*) fTubeArray->At(hit->GetTubeID());
      
      if(tube->GetLayerID() < startlayid)  startlayid = tube->GetLayerID();
      if(tube->GetSectorID() < startsecid) startsecid = tube->GetSectorID();
      if(tube->GetLayerID() > endlayid)  endlayid = tube->GetLayerID();
      if(tube->GetSectorID() > endsecid) endsecid = tube->GetSectorID();
      
      //      cout << "hit " << ihit << " " << hit->GetHitID() << " " << endsecid << " " << endlayid << endl;
      totaldistanceconf += distanceconf;

    }
  }

  double meandistanceconf = totaldistanceconf/thiscluster->GetNofHits();

  cout << "START SECTOR " << startsecid << " END SECTOR " << endsecid << endl;
  cout << "START LAYER  " << startlayid << " END LAYER  " << endlayid << endl;

  if(fDisplayOn) {
    display->cd(1);
    thiscluster->Draw(kRed);
    display->Update();
    display->Modified();
    char goOnChar;
    cout << "want to go to next cluster1?" << endl;
    cin >> goOnChar;
  } 


  if(startlayid != 0 || endlayid != 23) {
    for(int ihit = 0; ihit < stthitlist->GetNofHits(); ihit++) {
      PndTrkHit *hit = stthitlist->GetHit(ihit);
      if(cluster->DoesContain(hit)) continue;     
      PndSttTube *tube = (PndSttTube*) fTubeArray->At(hit->GetTubeID());
     
      double distance = hit->GetXYDistance(TVector3(xc, yc, 0.));
      if(distance <= rmax && distance >= rmin) {
	//	cout << endl;
	//	cout << "other sector " << tube->GetSectorID() << " " << tube->GetLayerID();

	if(tube->GetSectorID() == 0 || tube->GetSectorID() == 5) {
	  if(startsecid != 5 && endsecid != 5 && startsecid != 0 && endsecid != 0) continue; 
	}
	else if(fabs(tube->GetSectorID() - startsecid) > 1 && fabs(tube->GetSectorID() - endsecid) > 1) continue;
	  
	if(tube->GetLayerID() > startlayid && tube->GetLayerID() < endlayid) continue;
	PndTrkConformalHit *chit = NULL;
	if(hit->IsSttParallel()) chit = conform->GetConformalSttHit(hit);
	else chit = conform->GetConformalHit(hit); // CHECK if skew?
	double distanceconf = fabs((chit->GetV() - fitm * chit->GetU() - fitp)/ TMath::Sqrt(fitm * fitm + 1));
   
	//	cout << "->distance " << distance << " (" << meandistanceconf << ") " << rmin << " " << rmax << " " << distanceconf << endl;
 


	//	cout << "tubeid " << hit->GetTubeID() << " " << tube->GetLayerID() << endl;
	thiscluster->AddHit(hit);
	//	cout << " ***";


      }
    }
  }
  cout << endl;

  if(fDisplayOn) {
    display->cd(1);
    thiscluster->Draw(kRed);
    display->Update();
    display->Modified();
    char goOnChar;
    cout << "want to go to next cluster2?" << endl;
    cin >> goOnChar;
  } 
  // ---------------------------

  return thiscluster;
}

void PndTrkLegendreNew::AnalyticalFit(PndTrkCluster *cluster, double xc, double yc, double R, double &fitm, double&fitq) {
  
  
  // fit with analytical chi2 -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~--
  fFitter->Reset();
  if(fDisplayOn) {
    display->cd(1);
    Refresh();
  }
  for(int ihit = 0; ihit < cluster->GetNofHits(); ihit++) 
    {
      PndTrkHit *hit = cluster->GetHit(ihit);
      if(hit->IsSttSkew()) continue;
      if(hit->IsSttParallel()) IntersectionFinder(hit, xc, yc, R);
      PndTrkConformalHit *chit = conform->GetConformalHit(hit);
      double sigma = 0.1; // CHECK
      fFitter->SetPointToFit(chit->GetPosition().X(), chit->GetPosition().Y(), sigma);
      //      cout << "set point to fit " << chit->GetPosition().X() << " " <<  chit->GetPosition().Y() << endl;
      //     if(fDisplayOn) {
      // 	display->cd(1);
      // 	TMarker *mrk = new TMarker(hit->GetPosition().X(), hit->GetPosition().Y(), 6);
      // 	mrk->SetMarkerColor(kRed);
      // 	mrk->Draw("SAME");

      // 	display->cd(2);
      // 	TMarker *mrk2 = new TMarker(chit->GetPosition().X(), chit->GetPosition().Y(), 6);
      // 	mrk2->SetMarkerColor(kRed);
      // 	mrk2->Draw("SAME");


      // 	display->Update();
      // 	display->Modified();
      //       } 
    }

  
  fFitter->StraightLineFit(fitm, fitq);
  cout << "previous " << xc << " " << yc << " " << R << endl;
  FromConformalToRealTrack(fitm, fitq, xc, yc, R);
  cout << "now " << xc << " " << yc << " " << R << endl;

  if(fDisplayOn) {
    display->cd(2);
    cout << "wanna see the line?" << endl;
    TLine *line = new TLine(-10.07, fitq + fitm * (-10.07), 10.07, fitq + fitm * (10.07));
    line->SetLineColor(2);
    line->Draw("SAME");
    char goOnChar;
    display->Update();
    display->Modified();
    cin >> goOnChar;
  }




}


void PndTrkLegendreNew::AnalyticalFit2(PndTrkCluster *cluster, double fitm, double fitp, double &fitm2, double&fitp2) {
  
  
  // fit with analytical chi2 -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~--
  fFitter->Reset();
  if(fDisplayOn) {
    display->cd(1);
    Refresh();
  }
  for(int ihit = 0; ihit < cluster->GetNofHits(); ihit++) 
    {
      PndTrkHit *hit = cluster->GetHit(ihit);
      if(hit->IsSttSkew()) continue;
  
      PndTrkConformalHit *chit = conform->GetConformalSttHit(hit);
      IntersectionFinder(chit, fitm, fitp);
      
      double sigma = chit->GetIsochrone();
      fFitter->SetPointToFit(chit->GetPosition().X(), chit->GetPosition().Y(), sigma);
      //      cout << "set point to fit " << chit->GetPosition().X() << " " <<  chit->GetPosition().Y() << endl;
      if(fDisplayOn) {
	display->cd(1);
	TMarker *mrk = new TMarker(hit->GetPosition().X(), hit->GetPosition().Y(), 6);
	mrk->SetMarkerColor(kRed);
	mrk->Draw("SAME");

	display->cd(2);
	TMarker *mrk2 = new TMarker(chit->GetPosition().X(), chit->GetPosition().Y(), 6);
	mrk2->SetMarkerColor(kRed);
	mrk2->Draw("SAME");


// 	display->Update();
// 	display->Modified();
      } 
    }

  
  fFitter->StraightLineFit(fitm2, fitp2);

  double xc, yc, R;
  FromConformalToRealTrack(fitm, fitp, xc, yc, R);
  cout << "previous " << xc << " " << yc << " " << R << endl;
  FromConformalToRealTrack(fitm2, fitp2, xc, yc, R);
  cout << "now " << xc << " " << yc << " " << R << endl;

  if(fDisplayOn) {
    display->cd(2);
    cout << "wanna see the line?" << endl;
    TLine *line = new TLine(-10.07, fitp2 + fitm2 * (-10.07), 10.07, fitp2 + fitm2 * (10.07));
    line->SetLineColor(2);
    line->Draw("SAME");

    display->cd(1);
    TArc *aline = new TArc(xc, yc, R);
    aline->SetFillStyle(0);
    aline->SetLineColor(2);
    aline->Draw("SAME");


    char goOnChar;
    display->Update();
    display->Modified();
    cin >> goOnChar;
  }




}
void PndTrkLegendreNew::IntersectionFinder(PndTrkConformalHit *chit, double fitm, double fitp) {
  
  double xi1 = chit->GetU() + fitm * chit->GetIsochrone()/ TMath::Sqrt(fitm * fitm + 1);
  double yi1 = chit->GetV() - chit->GetIsochrone() / TMath::Sqrt(fitm * fitm + 1);
  
  double xi2 = chit->GetU() - fitm * chit->GetIsochrone() / TMath::Sqrt(fitm * fitm + 1);
  double yi2 = chit->GetV() + chit->GetIsochrone()/ TMath::Sqrt(fitm * fitm + 1);
  
  double xi = 0, yi = 0;
  
  fabs(yi1 - (fitm * xi1 + fitp)) < fabs(yi2 - (fitm * xi2 + fitp)) ? (yi = yi1, xi = xi1) : (yi = yi2, xi = xi2);
  
  chit->SetPosition(xi, yi);

}

void PndTrkLegendreNew::IntersectionFinder(PndTrkHit *hit, double xc, double yc, double R) {

  TVector2 vec(xc, yc);
  
  // tubeID  CHECK added
    Int_t tubeID = hit->GetTubeID();
    PndSttTube *tube = (PndSttTube*) fTubeArray->At(tubeID);

    // [xp, yp] point = coordinates xy of the centre of the firing tube
    TVector2 point(tube->GetPosition().X(), tube->GetPosition().Y());
    double radius = hit->GetIsochrone();

    // the coordinates of the point are taken from the intersection
    // between the circumference from the drift time and the R radius of
    // curvature. -------------------------------------------------------
    // 2. find the intersection between the little circle and the line // R
    // 2.a
    // find the line passing throught [xc, yc] (centre of curvature) and [xp, yp] (first wire)
    // y = mx + q
    Double_t m = (point.Y() - vec.Y())/(point.X() - vec.X());
    Double_t q = point.Y() - m*point.X();

    Double_t x1 = 0, y1 = 0,
      x2 = 0, y2 = 0,
      xb1 = 0, yb1 = 0,
      xb2 = 0, yb2 = 0;

    // CHECK the vertical track
    if(fabs(point.X() - vec.X()) < 1e-6) {
      
      // 2.b
      // intersection little circle and line --> [x1, y1]
      // + and - refer to the 2 possible intersections
      // +
      x1 = point.X();
      y1 = point.Y() + sqrt(radius * radius - (x1 - point.X()) * (x1 - point.X()));
      // - 
      x2 = x1;
      y2 = point.Y() - sqrt(radius * radius - (x2 - point.X()) * (x2 - point.X()));
      
      // 2.c intersection between line and circle
      // +
      xb1 = vec.X();
      yb1 = vec.Y() + sqrt(R * R - (xb1 - vec.X()) * (xb1 - vec.X()));
      // -
      xb2 = xb1;
      yb2 = vec.Y() - sqrt(R * R - (xb2 - vec.X()) * (xb2 - vec.X()));

    }    // END CHECK
    else {
      
      // 2.b
      // intersection little circle and line --> [x1, y1]
      // + and - refer to the 2 possible intersections
      // +
      x1 = (-(m*(q - point.Y()) - point.X()) + sqrt((m*(q - point.Y()) - point.X())*(m*(q - point.Y()) - point.X()) - (m*m + 1)*((q - point.Y())*(q - point.Y()) + point.X()*point.X() - radius*radius))) / (m*m + 1);
      y1 = m*x1 + q;
      // - 
      x2 = (-(m*(q - point.Y()) - point.X()) - sqrt((m*(q - point.Y()) - point.X())*(m*(q - point.Y()) - point.X()) - (m*m + 1)*((q - point.Y())*(q - point.Y()) + point.X()*point.X() - radius*radius))) / (m*m + 1);
      y2 = m*x2 + q;
      
      // 2.c intersection between line and circle
      // +
      xb1 = (-(m*(q - vec.Y()) - vec.X()) + sqrt((m*(q - vec.Y()) - vec.X())*(m*(q - vec.Y()) - vec.X()) - (m*m + 1)*((q - vec.Y())*(q - vec.Y()) + vec.X()*vec.X() - R * R))) / (m*m + 1);
      yb1 = m*xb1 + q;
      // -
      xb2 = (-(m*(q - vec.Y()) - vec.X()) - sqrt((m*(q - vec.Y()) - vec.X())*(m*(q - vec.Y()) - vec.X()) - (m*m + 1)*((q - vec.Y())*(q - vec.Y()) + vec.X()*vec.X() - R * R))) / (m*m + 1);
      yb2 = m*xb2 + q;
    }
    
    // calculation of the distance between [xb, yb] and [xp, yp]
    Double_t distb1 = sqrt((yb1 - point.Y())*(yb1 - point.Y()) + (xb1 - point.X())*(xb1 - point.X()));
    Double_t distb2 = sqrt((yb2 - point.Y())*(yb2 - point.Y()) + (xb2 - point.X())*(xb2 - point.X()));
    
    // choice of [xb, yb]
    TVector2 xyb;
    if(distb1 > distb2) xyb.Set(xb2, yb2); 
    else xyb.Set(xb1, yb1); 

    // calculation of the distance between [x, y] and [xb. yb]
    Double_t dist1 = sqrt((xyb.Y() - y1)*(xyb.Y() - y1) + (xyb.X() - x1)*(xyb.X() - x1));
    Double_t dist2 = sqrt((xyb.Y() - y2)*(xyb.Y() - y2) + (xyb.X() - x2)*(xyb.X() - x2));

    // choice of [x, y]
    TVector2 *xy;
    if(dist1 > dist2) xy = new TVector2(x2, y2);
    else xy = new TVector2(x1, y1);   // <========= THIS IS THE NEW POINT to be used for the fit
    
    hit->SetPosition(TVector3(xy->X(), xy->Y(), 0.0));
    
    delete xy;
}
