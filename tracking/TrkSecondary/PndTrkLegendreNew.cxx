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
 
  // initialize -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~
  Initialize();
  if(fVerbose > 1) {
    cout << "number of stt    hits " << fSttHitArray->GetEntriesFast() << endl;
    cout << "number of mvdpix hits " << fMvdPixelHitArray->GetEntriesFast() << endl;
    cout << "number of mvdstr hits " << fMvdStripHitArray->GetEntriesFast() << endl;
  }
 
 
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

  // clusterization -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-----
  PndTrkClusterList clusterlist = CreateFullClusterization();
  cout << "CLUSTERLIST " << clusterlist.GetNofClusters() << endl;

  // loop on clusterlist -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~
  for(int iclus = 0; iclus < clusterlist.GetNofClusters(); iclus++) {
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

    if(maxnoftracks > 1) continue; // CHECK

    // fitting procedure -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~
    Int_t nhits = ClusterToConformal(cluster);
    PndTrkTrack *track = LegendreFit(cluster);
    if(track == NULL) continue;
    PndTrkCluster *thiscluster = CreateClusterAroundTrack(track);

    cout << "CLSUTERING " << iclus << " " << endl;
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
    cout << hit->GetPosition().X() << " " << hit->GetPosition().Y() << " " << hit->IsSttParallel() << " " << " to CONFORMAL " << chit->GetU() << " " << chit->GetV() << " " << chit->GetIsochrone() << endl;   

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
      cout << "MAXPEAK " << maxpeak <<  ", BREAK NOW! "  << endl;
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


PndTrkClusterList PndTrkLegendreNew::CreateFullClusterization() {
  PndTrkClusterList clusterlist;
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
  return clusterlist;
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
    cout << "hit " << ihit << " " << tube->GetLayerID() << " " << nofhitsinlay[tube->GetLayerID()] << endl;

  }
  cluster->Sort();

  for(int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster->GetHit(ihit);
    PndSttTube *tube = (PndSttTube*) fTubeArray->At(hit->GetTubeID());
    cout << "SORTED " << ihit << " " << hit->GetHitID() << " " << tube->GetLayerID() << endl;
  }

  int maxnoftracks = 1;
  int tmplayid = -1;
  int counter = 0, counter1 = 0;;
  int isneigh = 0;
  for(int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster->GetHit(ihit);
    counter++; 
    if(hit->IsSttParallel() == kTRUE) continue;
    PndSttTube *tube = (PndSttTube*) fTubeArray->At(hit->GetTubeID());

    int layid = tube->GetLayerID();
    if(nofhitsinlay[layid] <= 1) continue;

    if(layid != tmplayid) {
      int noftracks = nofhitsinlay[tmplayid] - isneigh;
      if(tmplayid != -1) cout << "CLUSTER CONTAINS @ LAYER " << tmplayid << " ACTUALLY " << nofhitsinlay[tmplayid] << " - " << isneigh << " = " << noftracks << " TRACKS" << endl;
      if(noftracks > maxnoftracks) maxnoftracks = noftracks;
      isneigh = 0;
      tmplayid = layid;
      counter1 = 0;
      //	continue; //	break;
    }
    cout << "hit " << ihit << " on layid " << layid << "/ " <<  nofhitsinlay[layid] << endl;
    counter1++;
    if(counter1 == nofhitsinlay[layid]) continue;
    for(int jhit = counter; jhit < counter + nofhitsinlay[layid] -  counter1; jhit++) {
      PndTrkHit *hit2 = cluster->GetHit(jhit);
      int tubeid2 =   hit2->GetTubeID();
      PndSttTube *tube2 = (PndSttTube*) fTubeArray->At(tubeid2);



      if(tube->GetLayerID() != tube2->GetLayerID()) cout << "ERROR" << tube->GetLayerID()  << " " << tube2->GetLayerID() << endl;
      cout << "compare " << ihit << "(" << hit->GetHitID() << "- " << hit->GetTubeID() << ") with " << jhit << " (" << hit2->GetHitID() << "- " << tubeid2 << ") from " << counter << " to " << counter + nofhitsinlay[layid] - 1 << endl;
      if(tube->IsNeighboring(tubeid2) == kTRUE) {
	isneigh++;
	cout << "isneigh " << isneigh << endl;
	// break;
      }
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
  cout << "XR, YC, R: " << xc << " " << yc << " " << R << endl;

  // create a track from the cluster
  PndTrkTrack *track = new PndTrkTrack(cluster, xc, yc, R);

  return track;
}


PndTrkCluster * PndTrkLegendreNew::CreateClusterAroundTrack(PndTrkTrack *track) {

  double R = track->GetRadius();
  double xc = track->GetCenter().X();
  double yc = track->GetCenter().Y();
  PndTrkCluster *cluster = track->GetCluster();

  // create cluster depending on fitting
  double rmin = R - R * 0.05; // CHECK 20%?
  double rmax = R + R * 0.05; // "      "
  
  
  if(fDisplayOn) {
    display->cd(1);
    track->Draw(kRed);
    
    TArc *arcmin = new TArc(xc, yc, rmin);
    TArc *arcmax = new TArc(xc, yc, rmax);
      
    arcmin->SetFillStyle(0);
    arcmax->SetFillStyle(0);
    arcmin->SetLineColor(kGreen);
    arcmax->SetLineColor(kBlue);

    arcmin->Draw("SAME");
    arcmax->Draw("SAME");

    display->Update();
    display->Modified();
    char goOnChar;
    cout << "want to go to new cluster?" << endl;
    cin >> goOnChar;
  }
    
  // create cluster depending on fitting
  PndTrkCluster *thiscluster = new PndTrkCluster();
  bool started = false;
  int startsecid = -1, endsecid = -1, startlayid = -1, endlayid = -1;
  // clean existing cluster
  for(int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster->GetHit(ihit);
    double distance = hit->GetXYDistance(TVector3(xc, yc, 0.));
    if(distance <= rmax && distance >= rmin) {
      thiscluster->AddHit(hit);
      PndSttTube *tube = (PndSttTube*) fTubeArray->At(hit->GetTubeID());
      if(started == false) {
	startsecid = tube->GetSectorID();
	startlayid = tube->GetLayerID();
	started = true;
	cout << "start hit " << ihit << " " << hit->GetHitID() << " " << endsecid << " " << endlayid << endl;
      }
      endsecid = tube->GetSectorID();
      endlayid = tube->GetLayerID();
      cout << "hit " << ihit << " " << hit->GetHitID() << " " << endsecid << " " << endlayid << endl;

    }
  }

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
	cout << endl;
	cout << "other sector " << tube->GetSectorID() << " " << tube->GetLayerID();

	if(tube->GetSectorID() == 0 || tube->GetSectorID() == 5) {
	  if(startsecid != 5 && endsecid != 5 && startsecid != 0 && endsecid != 0) continue; 
	}
	else if(fabs(tube->GetSectorID() - startsecid) > 1 && fabs(tube->GetSectorID() - endsecid) > 1) continue;
	  
	if(tube->GetLayerID() > startlayid && tube->GetLayerID() < endlayid) continue;
	  
	thiscluster->AddHit(hit);
	cout << " ***";
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
