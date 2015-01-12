////////////////////////////////////////////////////////////
//
// PndTrkFwdTrackFinder
// 
// Class for secondary track pattern recognition
//
// authors: Lia Lavezzi - University of Torino (2014)
//                                   
////////////////////////////////////////////////////////////

#include "PndTrkFwdTrackFinder.h"

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
PndTrkFwdTrackFinder::PndTrkFwdTrackFinder() : FairTask("secondary track finder", 0), fDisplayOn2(kFALSE), fPersistence(kTRUE), fUseMVDPix(kTRUE), fUseMVDStr(kTRUE), fUseSTT(kTRUE), fUseSCIT(kTRUE), fUseGEM(kTRUE), fSecondary(kFALSE), fMvdPix_RealDistLimit(1000), fMvdStr_RealDistLimit(1000), fStt_RealDistLimit(1000), fMvdPix_ConfDistLimit(1000), fMvdStr_ConfDistLimit(1000), fStt_ConfDistLimit(1000), fInitDone(kFALSE), fUmin(-0.07), fUmax(0.07), fVmin(-0.07), fVmax(0.07), fRmin(-1.5), fRmax(1.5), fThetamin(0), fThetamax(180), fRefHit(NULL) {
  sprintf(fSttBranch,"STTHit");
  sprintf(fMvdPixelBranch,"MVDHitsPixel");
  sprintf(fMvdStripBranch,"MVDHitsStrip");
  sprintf(fSciTBranch,"SciTHit");
  sprintf(fGemBranch,"GEMHit");
  PndGeoHandling::Instance();
}

PndTrkFwdTrackFinder::PndTrkFwdTrackFinder(int verbose) : FairTask("secondary track finder", verbose), fDisplayOn2(kFALSE), fPersistence(kTRUE), fUseMVDPix(kTRUE), fUseMVDStr(kTRUE), fUseSTT(kTRUE), fUseSCIT(kTRUE), fUseGEM(kTRUE), fSecondary(kFALSE), fMvdPix_RealDistLimit(1000), fMvdStr_RealDistLimit(1000), fStt_RealDistLimit(1000), fMvdPix_ConfDistLimit(1000), fMvdStr_ConfDistLimit(1000), fStt_ConfDistLimit(1000), fInitDone(kFALSE), fUmin(-0.07), fUmax(0.07), fVmin(-0.07), fVmax(0.07), fRmin(-1.5), fRmax(1.5), fThetamin(0), fThetamax(180), fRefHit(NULL) {
  sprintf(fSttBranch,"STTHit");
  sprintf(fMvdPixelBranch,"MVDHitsPixel");
  sprintf(fMvdStripBranch,"MVDHitsStrip");
  sprintf(fSciTBranch,"SciTHit");
  sprintf(fGemBranch,"GEMHit");
  PndGeoHandling::Instance();
}

// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndTrkFwdTrackFinder::~PndTrkFwdTrackFinder() { 

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
InitStatus PndTrkFwdTrackFinder::Init() {
  
  fEventCounter = 0;

   

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndTrkFwdTrackFinder::Init: "
	 << "RootManager not instantiated, return!" << endl;
    return kFATAL;
  }

  // -- HITS -------------------------------------------------
  //
  // STT
  fSttHitArray = (TClonesArray*) ioman->GetObject(fSttBranch);
  if ( ! fSttHitArray ) {
    cout << "-W- PndTrkFwdTrackFinder::Init: "
	 << "No STTHit array, return!" << endl;
    return kERROR;
  }
  //
  // MVD PIXEL
  fMvdPixelHitArray = (TClonesArray*) ioman->GetObject(fMvdPixelBranch);
  if ( !fMvdPixelHitArray){
    std::cout << "-W- PndTrkFwdTrackFinder::Init: " << "No MVD Pixel hitArray, return!" << std::endl;
    return kERROR;
  }
  //
  // MVD STRIP
  fMvdStripHitArray = (TClonesArray*) ioman->GetObject(fMvdStripBranch);
  if ( !fMvdStripHitArray){
    std::cout << "-W- PndTrkFwdTrackFinder::Init: " << "No MVD Strip hitArray, return!" << std::endl;
    return kERROR;
  }
  //
  // SciT 
  fSciTHitArray = (TClonesArray*) ioman->GetObject(fSciTBranch);
  if ( !fSciTHitArray){
    std::cout << "-W- PndTrkFwdTrackFinder::Init: " << "No SciT hitArray, return!" << std::endl;
    return kERROR;
  }
  //
  // GEM
  fGemHitArray = (TClonesArray*) ioman->GetObject(fGemBranch);
  if ( !fGemHitArray){
    std::cout << "-W- PndTrkFwdTrackFinder::Init: " << "No GEM hitArray, return!" << std::endl;
    return kERROR;
  }
  //
  // long tracks
  fLongTrackArray = (TClonesArray*) ioman->GetObject("Track");
  if ( !fLongTrackArray) {
    std::cout << "-W- PndTrkFwdTrackFinder::Init: " << "No long track Array, return!" << std::endl;
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

  legendre = new PndTrkLegendreTransform();
  
  conform = new PndTrkConformalTransform();

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

void PndTrkFwdTrackFinder::SetParContainers() {
  FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}
 
// -------------------------------------------------------------------------
 

void PndTrkFwdTrackFinder::Initialize() {
  
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


void PndTrkFwdTrackFinder::Exec(Option_t* opt)  {
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

    for(int itrk = 0; itrk < fLongTrackArray->GetEntriesFast(); itrk++) {
      PndTrack *trk = (PndTrack*) fLongTrackArray->At(itrk);
      PndTrkTrack track(trk);
      track.Draw();
    }


    char goOnChar;
    display->Update();
    display->Modified();
    cin >> goOnChar;
  }

  // CHECK 
  PndTrkCluster gemcluster;

  int nofgemhits_on_layer[NOFLAYERS] = {0, 0, 0, 0, 0, 0}; // CHECK
  for(int ihit = 0; ihit < gemhitlist->GetNofHits(); ihit++) {
    hit = gemhitlist->GetHit(ihit);
    if(hit->IsUsed() == kTRUE) continue;
    int layerid = hit->GetSensorID();
    hit->SetSortVariable(layerid);
    gemcluster.AddHit(hit);
    nofgemhits_on_layer[layerid]++;
  }
  gemcluster.Sort();


//   int nhits = 0;
//   for(int ilay = 0; ilay < NOFLAYERS - 1; ilay++) {

//     for(int ihit = 0; ihit < nofgemhits_on_layer[ilay]; ihit++) {
//       int hitid = ihit + nhits;
//       PndTrkHit *hiti = gemcluster.GetHit(hitid);
      
//       for(int jhit = 0; jhit < nofgemhits_on_layer[ilay + 1]; jhit++) {
// 	int hitjd = jhit + nhits + nofgemhits_on_layer[ilay];
// 	PndTrkHit *hitj = gemcluster.GetHit(hitjd);
    
// 	double distance = hiti->GetXYDistance(hitj);
// 	cout << ilay << " " << hiti->GetSensorID() << " " << hitj->GetSensorID() << " " << distance << endl;


// 	if(fDisplayOn2)  {
// 	  Refresh();
// 	  hiti->Draw(kRed);
// 	  hitj->Draw(kBlue);
// 	  char goOnChar;
// 	  display->Update();
// 	  display->Modified();
// 	  cin >> goOnChar;
// 	}

//       }
//     }
//     nhits += nofgemhits_on_layer[ilay];
//   }
  for(int ihit = 0; ihit < gemcluster.GetNofHits(); ihit++) {
    PndTrkHit *hiti = gemcluster.GetHit(ihit);
    hiti->SetUsedFlag(kFALSE);
  }

  PndTrkClusterList clusterlist;

  cout << "GEM CKLUSTER " << gemcluster.GetNofHits() << endl;
  for(int ihit = 0; ihit < gemcluster.GetNofHits(); ihit++) {
    PndTrkHit *hiti = gemcluster.GetHit(ihit);
    if(hiti->IsUsed() == kTRUE) continue;
    PndTrkCluster cluster;
    cluster.AddHit(hiti);

    if(fDisplayOn2)  {
      Refresh();
      hiti->Draw(kRed);
      char goOnChar;
      display->Update();
      display->Modified();
      //  cin >> goOnChar;
    }



    hiti->SetUsedFlag(kTRUE);
    int layerid = hiti->GetSensorID();
    PndTrkHit *tmphiti = hiti;

    bool goOn = true;
    PndTrkHit *tmphitj = hiti;

    while(goOn == true) {

      double tmpdistance = 1000;
      for(int jhit = 0; jhit < gemcluster.GetNofHits(); jhit++) {
	PndTrkHit *hitj = gemcluster.GetHit(jhit);
	if(hitj->IsUsed() == kTRUE) continue;
	int layerjd = hitj->GetSensorID();
	
	if(layerjd != tmphitj->GetSensorID() + 1) continue;
	
	double distance = hitj->GetXYDistance(tmphiti);
	if(distance < tmpdistance) {
	  tmphitj = hitj;
	  tmpdistance = distance;
	  cout << "distance " << distance << " " << tmpdistance << endl;
	}
      }

      if(tmpdistance < 20) {
	cluster.AddHit(tmphitj);
	tmphitj->SetUsedFlag(kTRUE);
	tmphiti = tmphitj;

	if(fDisplayOn2)  {
	  Refresh();
	  tmphitj->Draw(kBlue);
	  char goOnChar;
	  display->Update();
	  display->Modified();
	  //  cin >> goOnChar;
	}
      }
      else goOn = false;
    }

    cout << "CLUSTER " << cluster.GetNofHits() << endl;
    if(cluster.GetNofHits() >= 3) {
      clusterlist.AddCluster(&cluster);
    }
  }

  // --------------------------------------------------------------
  // FIT THE CLUSTER 
  //
  for(int iclus = 0; iclus < clusterlist.GetNofClusters(); iclus++) {
    PndTrkCluster* cluster = clusterlist.GetCluster(iclus);
    cout << "cluster " << cluster->GetNofHits() << endl;

    if(fDisplayOn2)  {
      Refresh();
      cluster->LightUp();
      char goOnChar;
      display->Update();
      display->Modified();
      cin >> goOnChar;
    }

    // go to the conformal plane
    PndTrkHit *refhit = cluster->GetHit(0); // cluster->GetNofHits() - 1);
    fConformalHitList->Clear("C");
    double trasl[2] = {0, 0};
    double delta;
    ComputeTraAndRot(refhit, delta, trasl);
    cout << "translation/rotation " << trasl[0] << " " << trasl[1] << " " << delta << endl;
    conform->SetOrigin(trasl[0], trasl[1], delta);
    fConformalHitList->SetConformalTransform(conform);
    // fill conformal hits
    int nofconfhits = FillConformalHitList(cluster);


      // compute conformal plane extremities ---------------------------- this must go to a fctn CHECK
      fUmin =  1000, fVmin =  1000, fRmin =  1000;
      fUmax = -1000, fVmax = -1000, fRmax = -1000;
      double rc_of_min, rc_of_max;
    
      for(int jhit = 0; jhit < fConformalHitList->GetNofHits(); jhit++) {
	PndTrkConformalHit *chit = fConformalHitList->GetHit(jhit);
	if(TMath::IsNaN(chit->GetPosition().X())) continue; // prevents the nan of the ref hit
	double u = chit->GetU();
	double v = chit->GetV();
	double rc = chit->GetIsochrone();
	if(rc < 0) rc = 0;
	// cout << "conf hit " << jhit << " u, v " << u << " " << v << " " << rc << endl;
	u - rc < fUmin ? fUmin = u - rc : fUmin;
	v - rc < fVmin ? fVmin = v - rc : fVmin;
	u + rc > fUmax ? fUmax = u + rc : fUmax;
	v + rc > fVmax ? fVmax = v + rc : fVmax;
      
	double theta1 = TMath::ATan2(v, u);
	double theta2 = theta1 + TMath::Pi();
      
	double r1 = u * TMath::Cos(theta1) + v * TMath::Sin(theta1);
	double r2 = u * TMath::Cos(theta2) + v * TMath::Sin(theta2);
      
	double rimin, rimax;
	r1 < r2 ? (rimin = r1, rimax = r2) : (rimin = r2, rimax = r1);
      
	rimin < fRmin ? (rc_of_min = rc, fRmin = rimin) : fRmin;
	rimax > fRmax ? (rc_of_max = rc, fRmax = rimax) : fRmax;
      }
    
      fRmin -= rc_of_min;
      fRmax += rc_of_max;
    
      // to square the conformal plane
      double du = fUmax - fUmin;
      double dv = fVmax - fVmin;
      double delt = fabs(dv - du)/2.;
      du < dv ? (fUmin -= delt, fUmax += delt) : (fVmin -= delt, fVmax += delt);
 
      if(fDisplayOn2) {
	DrawGeometryConf(fUmin, fUmax, fVmin, fVmax);
	cout << fUmin << " " << fUmax << endl;
	cout << fVmin << " " << fVmax << endl;
	// ----------------------------------------------------------------------
	for(int ihit = 0; ihit < nofconfhits; ihit++) {
	  PndTrkConformalHit *chit = fConformalHitList->GetHit(ihit);
	  if(TMath::IsNaN(chit->GetPosition().X())) continue; // prevents the nan of the ref hit
	  cout << "chit " << chit->GetU() << " " << chit->GetV() << endl;
	  display->cd(2);
	  chit->Draw(kRed);
	  display->Update();
	  display->Modified();
	}

	char goOnChar;
	display->Update();
	display->Modified();
	cin >> goOnChar;
      }	  

      // ====== FIT CLUSTER ANALYTICALLY
      // fit with analytical chi2 -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~--
      fFitter->Reset();
      if(fDisplayOn2) {
	display->cd(1);
	Refresh();
      }
      for(int ihit = 0; ihit < cluster->GetNofHits(); ihit++) 
	{
	  hit = cluster->GetHit(ihit);
	  if(hit == fRefHit) continue;
	  cout << "SECTOR --> " << hit->GetSector() << endl;
	  PndTrkConformalHit chit = conform->GetConformalHit(hit);
	  double sigma = hit->GetSensorID() * 1e-5;
	  if(TMath::IsNaN(chit.GetPosition().X())) continue; // prevents the nan of the ref hit
	  fFitter->SetPointToFit(chit.GetPosition().X(), chit.GetPosition().Y(), sigma);
	  if(fDisplayOn2) {
	    display->cd(1);
	    TMarker *mrk = new TMarker(hit->GetPosition().X(), hit->GetPosition().Y(), 6);
	    mrk->SetMarkerColor(kRed);
	    mrk->Draw("SAME");
	    display->cd(2);
	    TMarker *mrk2 = new TMarker(chit.GetPosition().X(), chit.GetPosition().Y(), 6);
	    mrk2->SetMarkerColor(kRed);
	    mrk2->Draw("SAME");
	    display->Update();
	    display->Modified();
	  } 
	}
      double fitm, fitq;
      fFitter->StraightLineFit(fitm, fitq);
      double xc, yc, R;
      FromConformalToRealTrack(fitm, fitq, xc, yc, R);
      if(fDisplayOn2)  {
	char goOnChar;
	display->cd(1);
	TArc *arcm = new TArc(xc, yc, R);
	arcm->SetFillStyle(0);
	arcm->SetLineColor(5);
	arcm->Draw("SAME");
	display->Update();
	display->Modified();
	cin >> goOnChar;
      }

      // GET SECTOR ID ---------------------------------------------
      std::map< int, int > sectorids;
      for(int ihit = 0; ihit < cluster->GetNofHits(); ihit++) 
	{
	  hit = cluster->GetHit(ihit);
	  if(hit == fRefHit) continue;
	  int secid = hit->GetSector();

	  if(sectorids.find(secid) == sectorids.end()) sectorids[secid] = 1;
	  else sectorids[secid]++;
	}
      
      int tmpsecentries = 0, tmpsec = -1;
      for(int isec = 0; isec < sectorids.size(); isec++) {
	if(tmpsecentries < sectorids[isec]) {
	  tmpsecentries = sectorids[isec];
	  tmpsec = isec;
	}
      }
      int sectorID = tmpsec;
      //    cout << "SECTOR " << sectorID << " " << sectorids.size() << endl;

      // border?
      bool border = false;
      int othersecID = -1;
      for(int isec = 0; isec < sectorids.size(); isec++) {
	if(sectorids[isec] > 0 && isec != sectorID) {
	  border = true;
	  othersecID = isec;
	}
      }
      cout << "SECTOR " << sectorID << " " << sectorids.size() << " " << othersecID << " " << border << endl;
      // --------------------------------------------------------------

      // EVALUATE WHICH SECTORS are interesting
      // due to the curvature radius

      double xydistance = TMath::Sqrt(xc * xc + yc * yc);

      cout << CTINRADIUS - R << endl;
      bool searchnearby = false;
      if(xydistance < CTINRADIUS - R) {
	cout << "************* INTERNAL TO INNER *****************" << endl;
	continue;
      }
      else if(xydistance < CTINRADIUS + R && xydistance < CTOUTRADIUS - R) {
	cout << "************* INTERSECTING INNER *****************" << endl;
	searchnearby = true;
      }
      else if(xydistance < CTINRADIUS + R) {
	cout << "************* INTERSECTING BOTH *****************" << endl;
      }
      else if(xydistance < CTOUTRADIUS - R) {
	cout << "************* INTERNAL *****************" << endl;
	searchnearby = true;
      }
      else if(xydistance < CTOUTRADIUS + R) {
	cout << "************* INTERSECTING EXTERNAL *****************" << endl;
	searchnearby = true;
      }
      else {
	cout << "************* EXTERNAL TO OUTER *****************" << endl;
	continue;
      }
 
  
      // ----------------------------------------- 
      // === CREATE CLUSTER - taken from PndTrkFinder ---> will be put in a fctn
      // in the same sector or 
      // nearby if at the limit of two sectors <<<<<<<<<<<<------------------
      PndTrkCluster cluster2(*cluster);
      
      
      //       for(int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
      // 	hit = cluster->GetHit(ihit);
      // 	cluster2.AddHit(hit);
      //       }
      
      //      // create cluster depending on fitting
      //       double rmin = r - r * 0.05; // CHECK 5%?
      //       double rmax = r + r * 0.05; // "      "
   
      // create cluster depending on fitting
      for(int ihit = 0; ihit < stthitlist->GetNofHits(); ihit++) {
	hit = stthitlist->GetHit(ihit);
	if(hit->IsUsed() == kTRUE) continue;
	if(hit->IsSttParallel() == kFALSE) continue;
	if(searchnearby == true) { // search in this sector and +/- 1
	  if(fabs(hit->GetSector() - sectorID) > 1 && !(hit->GetSector() == 5 && sectorID == 0) && !(hit->GetSector() == 0 && sectorID == 5)) continue;
	}
	else {                     // search only in this sector and possible othersector
	  if(border == false && hit->GetSector() != sectorID) continue;
	  else if(border == true && (hit->GetSector() != sectorID && hit->GetSector() != othersecID)) continue;
	}  
	double distance = hit->GetXYDistance(TVector3(xc, yc, 0.));
      
	//    if(distance <= rmax && distance >= rmin) {
	//	cluster.AddHit(hit);
	//      }

	double distance_hit_center = (hit->GetPosition().XYvector() - TVector2(xc, yc)).Mod();
	double recoiso = fabs(distance_hit_center - R);

	//    hit->GetPosition().Print();
	//       TVector2(x, y).Print();
	//  cout << r << " " << hit->GetIsochrone() << endl;


	//   cout << "recoiso " << recoiso << endl;
	if(recoiso < 2.) cluster2.AddHit(hit);
	
      }
      
      /**
       // REFIT ANALYTICALLY ....again taken from-..
       // -------------------------- 
       // if there is a scitil lets use it as seed hit
       // for the conformal map
       fConformalHitList->Reset();
       nofconfhits = FillConformalHitList(&cluster2);
      
       // compute conformal plane extremities ---------------------------- this must go to a fctn CHECK
       fUmin =  1000, fVmin =  1000, fRmin =  1000;
       fUmax = -1000, fVmax = -1000, fRmax = -1000;
       rc_of_min, rc_of_max;
    
       for(int jhit = 0; jhit < fConformalHitList->GetNofHits(); jhit++) {
       PndTrkConformalHit *chit = fConformalHitList->GetHit(jhit);
       double u = chit->GetU();
       double v = chit->GetV();
       double rc = chit->GetIsochrone();
       if(rc < 0) rc = 0;
       cout << "conf hit " << jhit << " u, v " << u << " " << v << " " << rc << endl;
       u - rc < fUmin ? fUmin = u - rc : fUmin;
       v - rc < fVmin ? fVmin = v - rc : fVmin;
       u + rc > fUmax ? fUmax = u + rc : fUmax;
       v + rc > fVmax ? fVmax = v + rc : fVmax;
      
       double theta1 = TMath::ATan2(v, u);
       double theta2 = theta1 + TMath::Pi();
      
       double r1 = u * TMath::Cos(theta1) + v * TMath::Sin(theta1);
       double r2 = u * TMath::Cos(theta2) + v * TMath::Sin(theta2);
      
       double rimin, rimax;
       r1 < r2 ? (rimin = r1, rimax = r2) : (rimin = r2, rimax = r1);
      
       rimin < fRmin ? (rc_of_min = rc, fRmin = rimin) : fRmin;
       rimax > fRmax ? (rc_of_max = rc, fRmax = rimax) : fRmax;
       }
    
       fRmin -= rc_of_min;
       fRmax += rc_of_max;
    
       // to square the conformal plane
       du = fUmax - fUmin;
       dv = fVmax - fVmin;
       delt = fabs(dv - du)/2.;
       du < dv ? (fUmin -= delt, fUmax += delt) : (fVmin -= delt, fVmax += delt);
 
       if(fDisplayOn2) {
       DrawGeometryConf(fUmin, fUmax, fVmin, fVmax);
       // ----------------------------------------------------------------------
       for(int ihit = 0; ihit < nofconfhits; ihit++) {
       PndTrkConformalHit *chit = fConformalHitList->GetHit(ihit);
       chit->Draw(kRed);
       }
       }
      **/
  
      /**
       // ====== REFIT CLUSTER ANALYTICALLY
       
       double fitm2, fitq2;
       AnalyticalFit(&cluster2, xc, yc, R, fitm2, fitq2);
       
       double xc2, yc2, R2;
       FromConformalToRealTrack(fitm2, fitq2, xc2, yc2, R2);
      **/
      
      /**
       // LEGENDRE ----------------------------------------------------------
       legendre->SetUpLegendreHisto(180, 0, 180, 1000, -0.07, 0.07);
          
       // reset the legendre histo for a new legendre fit
       legendre->ResetLegendreHisto();
    
       if(fDisplayOn2) {
       DrawGeometryConf(fUmin, fUmax, fVmin, fVmax);
       }
    
       // fill legendre histo with the cluster hits
       FillLegendreHisto(&cluster2); 
       std::vector< double > theta_max, r_max;
       std::vector < int > content_max;
       // get the peak
       legendre->ExtractLegendreMaxima(1, theta_max, r_max, content_max);
    
       // from theta/r to line parameters in CONFORMAL plane 
       double fitm2, fitq2;
       legendre->ExtractLineParameters(theta_max[0], r_max[0], fitm2, fitq2);
       if(fDisplayOn2) {
       display->cd(2);
       TLine *line = new TLine(-10.07, fitq2 + fitm2 * (-10.07), 10.07, fitq2 + fitm2 * (10.07));
       line->Draw("SAME");
	
       display->cd(3);
       DrawLegendreHisto();
       display->Update();
       display->Modified();
       }
    
       // from line parameters to center/radius in REAL plane
       Double_t xc2, yc2, R2;
       FromConformalToRealTrack(fitm2, fitq2, xc2, yc2, R2);
       // --------------------------------------------------------------
  
       if(fDisplayOn2)  {
       char goOnChar;
       display->cd(1);
       TArc *arcm = new TArc(xc2, yc2, R2);
       arcm->SetFillStyle(0);
       arcm->SetLineColor(4);
       arcm->Draw("SAME");
       display->Update();
       display->Modified();
       cin >> goOnChar;
       }
      **/



      // GET SECTOR ID ---------------------------------------------
      sectorids.clear();
      for(int ihit = 0; ihit < cluster2.GetNofHits(); ihit++) 
	{
	  hit = cluster2.GetHit(ihit);
	  if(hit == fRefHit) continue;
	  int secid = hit->GetSector();
	  
	  if(sectorids.find(secid) == sectorids.end()) sectorids[secid] = 1;
	  else sectorids[secid]++;
	}
      
      tmpsecentries = 0, tmpsec = -1;
      for(int isec = 0; isec < sectorids.size(); isec++) {
	if(tmpsecentries < sectorids[isec]) {
	  tmpsecentries = sectorids[isec];
	  tmpsec = isec;
	}
      }
      sectorID = tmpsec;
      //    cout << "SECTOR " << sectorID << " " << sectorids.size() << endl;

      // border?
      border = false;
      othersecID = -1;
      for(int isec = 0; isec < sectorids.size(); isec++) {
	if(sectorids[isec] > 0 && isec != sectorID) {
	  border = true;
	  othersecID = isec;
	}
      }
      cout << "SECTOR " << sectorID << " " << sectorids.size() << " " << othersecID << " " << border << endl;
      // --------------------------------------------------------------


    // what about adding the mvd hits now? lets do it!
    // ... and put mvd pixel hits to conformal plane --------------- 
    for(int ihit = 0; ihit < mvdpixhitlist->GetNofHits(); ihit++) {
      hit = mvdpixhitlist->GetHit(ihit);
      if(hit->IsUsed() == kTRUE) continue;
      if(searchnearby == true) { // search in this sector and +/- 1
	if(fabs(hit->GetSector() - sectorID) > 1 && !(hit->GetSector() == 5 && sectorID == 0) && !(hit->GetSector() == 0 && sectorID == 5)) continue;
      }
      else {                     // search only in this sector and possible othersector
	if(border == false && hit->GetSector() != sectorID) continue;
	else if(border == true && (hit->GetSector() != sectorID && hit->GetSector() != othersecID)) continue;
      }  
      //     if(hit->GetSector() != sectorID) continue;
      double distance_hit_center = (hit->GetPosition().XYvector() - TVector2(xc, yc)).Mod();

      double recoiso = fabs(distance_hit_center - R);
      cout << "recoiso pix " << recoiso << endl;
      if(recoiso < 2.) {
	cluster2.AddHit(hit);
	if(fDisplayOn2)  {
	  hit->Draw(kOrange);
	  display->Update();
	  display->Modified();
	}  
	//     PndTrkConformalHit *chit = conform->GetConformalHit(hit);
	//        conformalhitlist->AddHit(chit);  
      }
    }

    // ... and put mvd strip hits to conformal plane --------------- 
    for(int ihit = 0; ihit < mvdstrhitlist->GetNofHits(); ihit++) {
      hit = mvdstrhitlist->GetHit(ihit);
      if(hit->IsUsed() == kTRUE) continue;
      if(searchnearby == true) { // search in this sector and +/- 1
	if(fabs(hit->GetSector() - sectorID) > 1 && !(hit->GetSector() == 5 && sectorID == 0) && !(hit->GetSector() == 0 && sectorID == 5)) continue;
      }
      else {                     // search only in this sector and possible othersector
	if(border == false && hit->GetSector() != sectorID) continue;
	else if(border == true && (hit->GetSector() != sectorID && hit->GetSector() != othersecID)) continue;
      }  
     // if(hit->GetSector() != sectorID) continue;
      double distance_hit_center = (hit->GetPosition().XYvector() - TVector2(xc, yc)).Mod();
      double recoiso = fabs(distance_hit_center - R);
      cout << "recoiso str " << recoiso << endl;
      if(recoiso < 2.) {
	cluster2.AddHit(hit);
	if(fDisplayOn2)  {
	  hit->Draw(kOrange);
	  display->Update();
	  display->Modified();
	}  
	PndTrkConformalHit chit = conform->GetConformalHit(hit); 
	fConformalHitList->AddHit(&chit);  
      }
    }
    
    if(fDisplayOn2) {
	display->cd(1);
	cluster2.Draw(kRed);
	display->Update();
	display->Modified();
	char goOnChar;
	cout << "want to go on?" << endl;
	cin >> goOnChar;
      } 
  

//     // -------- sorting DA QUI
//     // get the last mvd
//     for(int ihit = 0; ihit < cluster2.GetNofHits(); ihit++) {
//       hit = cluster2.GetHit(ihit);
//       int detId = hit->GetDetectorID();
//       if(detId == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) {
// 	refhit = hit;
// 	break;
//       }
//     }
    
    // REFIT ANALYTICALLY ....again taken from-..
    // -------------------------- 
    // if there is a scitil lets use it as seed hit
    // for the conformal map
    fConformalHitList->Clear("C");
    ComputeTraAndRot(refhit, delta, trasl);
    conform->SetOrigin(trasl[0], trasl[1], delta);
    fConformalHitList->SetConformalTransform(conform);

    nofconfhits = FillConformalHitList(&cluster2);
      
    // compute conformal plane extremities ---------------------------- this must go to a fctn CHECK
    fUmin =  1000, fVmin =  1000, fRmin =  1000;
    fUmax = -1000, fVmax = -1000, fRmax = -1000;
    rc_of_min, rc_of_max;
    
    for(int jhit = 0; jhit < fConformalHitList->GetNofHits(); jhit++) {
      PndTrkConformalHit *chit = fConformalHitList->GetHit(jhit);
      double u = chit->GetU();
      double v = chit->GetV();
      double rc = chit->GetIsochrone();
      if(rc < 0) rc = 0;
      cout << "conf hit " << jhit << " u, v " << u << " " << v << " " << rc << endl;
      u - rc < fUmin ? fUmin = u - rc : fUmin;
      v - rc < fVmin ? fVmin = v - rc : fVmin;
      u + rc > fUmax ? fUmax = u + rc : fUmax;
      v + rc > fVmax ? fVmax = v + rc : fVmax;
      
      double theta1 = TMath::ATan2(v, u);
      double theta2 = theta1 + TMath::Pi();
      
      double r1 = u * TMath::Cos(theta1) + v * TMath::Sin(theta1);
      double r2 = u * TMath::Cos(theta2) + v * TMath::Sin(theta2);
      
      double rimin, rimax;
      r1 < r2 ? (rimin = r1, rimax = r2) : (rimin = r2, rimax = r1);
      
      rimin < fRmin ? (rc_of_min = rc, fRmin = rimin) : fRmin;
      rimax > fRmax ? (rc_of_max = rc, fRmax = rimax) : fRmax;
    }
    
    fRmin -= rc_of_min;
    fRmax += rc_of_max;
    
    // to square the conformal plane
    du = fUmax - fUmin;
    dv = fVmax - fVmin;
    delt = fabs(dv - du)/2.;
    du < dv ? (fUmin -= delt, fUmax += delt) : (fVmin -= delt, fVmax += delt);
 
    if(fDisplayOn2) {
      DrawGeometryConf(fUmin, fUmax, fVmin, fVmax);
      // ----------------------------------------------------------------------
      for(int ihit = 0; ihit < nofconfhits; ihit++) {
	PndTrkConformalHit *chit = fConformalHitList->GetHit(ihit);
	chit->Draw(kRed);
      }
    }
  
    // ====== REFIT CLUSTER ANALYTICALLY
       
    double fitm3, fitq3;
    AnalyticalFit(&cluster2, xc, yc, R, fitm3, fitq3);
       
    double xc3, yc3, R3;
    FromConformalToRealTrack(fitm3, fitq3, xc3, yc3, R3);
  
    if(fDisplayOn2)  {
      char goOnChar;
      display->cd(1);
      TArc *arcm = new TArc(xc3, yc3, R3);
      arcm->SetFillStyle(0);
      arcm->SetLineColor(4);
      arcm->Draw("SAME");
      display->Update();
      display->Modified();
      cin >> goOnChar;
    }
  }


  
  // -----------------------------------------------------------
  //   Reset();
  //   fFinalCluster->Clear("C");
  //   fTrackList->Clear("C");
  //   fConformalHitList->Clear("C"); 
}


void PndTrkFwdTrackFinder::FillLegendreHisto(PndTrkCluster *cluster)
{
  // ---------------------------------------------------------------
  //  cout << "FILL LEGENDRE HISTO " << cluster->GetNofHits() << endl;

  for(int ihit = 0; ihit < fConformalHitList->GetNofHits(); ihit++) {
    PndTrkConformalHit *chit = fConformalHitList->GetHit(ihit);
    PndTrkHit *hit = chit->GetHit();
    if(cluster->DoesContain(hit) == kFALSE) continue;
    legendre->FillLegendreHisto(chit->GetU(), chit->GetV(), chit->GetIsochrone());
    if(fDisplayOn2) {
      DrawConfHit(chit->GetU(), chit->GetV(), chit->GetIsochrone());
      //      cout << "conformal2: " << chit->GetU() << " " << chit->GetV() << " " << chit->GetIsochrone() << endl;   
    }
  }
}

void PndTrkFwdTrackFinder::DrawLegendreHisto() {
  display->cd(3);
  legendre->Draw();
  display->Update();
  display->Modified();
}

void PndTrkFwdTrackFinder::DrawConfHit(double u, double v, double r, int marker) {
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

void PndTrkFwdTrackFinder::AnalyticalFit(PndTrkCluster *cluster, double xc, double yc, double R, double &fitm, double&fitq) {
  
  
  // fit with analytical chi2 -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~--
  fFitter->Reset();
  if(fDisplayOn2) {
    display->cd(1);
    Refresh();
  }
  for(int ihit = 0; ihit < cluster->GetNofHits(); ihit++) 
    {
      PndTrkHit *hit = cluster->GetHit(ihit);
      if(hit == fRefHit) continue;
      if(hit->IsSttSkew()) continue;
      if(hit->IsSttParallel()) IntersectionFinder(hit, xc, yc, R);
      PndTrkConformalHit chit = conform->GetConformalHit(hit);
      PndTrkConformalHit chitstt = conform->GetConformalSttHit(hit);
      double sigma = 1e-5;
      if(hit->IsSttParallel()) sigma = chitstt.GetIsochrone(); // 0.1; // CHECK

      if(TMath::IsNaN(chit.GetPosition().X())) continue; // prevents the nan of the ref hit

      fFitter->SetPointToFit(chit.GetPosition().X(), chit.GetPosition().Y(), sigma);
      // cout << "set point to fit " << chit.GetPosition().X() << " " <<  chit.GetPosition().Y() << " " << sigma << endl;
      if(fDisplayOn2) {
       	display->cd(1);
       	TMarker *mrk = new TMarker(hit->GetPosition().X(), hit->GetPosition().Y(), 6);
       	mrk->SetMarkerColor(kRed);
       	mrk->Draw("SAME");

       	display->cd(2);
       	TMarker *mrk2 = new TMarker(chit.GetPosition().X(), chit.GetPosition().Y(), 6);
       	mrk2->SetMarkerColor(kRed);
       	mrk2->Draw("SAME");


       	display->Update();
       	display->Modified();
      } 
    }

  
  fFitter->StraightLineFit(fitm, fitq);
  //  cout << "previous " << xc << " " << yc << " " << R << endl;
  FromConformalToRealTrack(fitm, fitq, xc, yc, R);
  // cout << "now " << xc << " " << yc << " " << R << endl;

  if(fDisplayOn2) {
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

void PndTrkFwdTrackFinder::IntersectionFinder(PndTrkHit *hit, double xc, double yc, double R) {

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

void PndTrkFwdTrackFinder::FromConformalToRealTrack(double fitm, double fitp, double &x0, double &y0, double &R) {
 // CHECK if this needs to be kept --> change xc0 to xc etc
  // center and radius
  Double_t xcrot0, ycrot0;
  ycrot0 = 1 / (2 * fitp);
  xcrot0 = - fitm * ycrot0;
  R =  sqrt(xcrot0 * xcrot0 + ycrot0 * ycrot0);
  // re-rotation and re-traslation of xc and yc
  // rotation    
  x0 = TMath::Cos(fConformalHitList->GetConformalTransform()->GetRotation())*xcrot0 - TMath::Sin(fConformalHitList->GetConformalTransform()->GetRotation())*ycrot0;
  y0 = TMath::Sin(fConformalHitList->GetConformalTransform()->GetRotation())*xcrot0 + TMath::Cos(fConformalHitList->GetConformalTransform()->GetRotation())*ycrot0;
  // traslation 
  x0 += fConformalHitList->GetConformalTransform()->GetTranslation().X();
  y0 += fConformalHitList->GetConformalTransform()->GetTranslation().Y();
}



void PndTrkFwdTrackFinder::DrawGeometryConf(double x1, double x2, double y1, double y2) {
  // CHECK
  char goOnChar;
  //  cout << "DRAWING GEOMETRY CONF" << endl;
  // cin >> goOnChar;
  
  // CHECK previous calculations, now not used;
  if(huv == NULL)  huv = new TH2F("huv", "uv plane", 100, x1, x2, 100, y1, y2);
  else {
    huv->Reset(); 
    huv->GetXaxis()->SetLimits(x1, x2);
    huv->GetYaxis()->SetLimits(y1, y2);
   }
  display->cd(2);
  huv->Draw();
  display->Update();
  display->Modified();  
 
}
void PndTrkFwdTrackFinder::ComputeTraAndRot(PndTrkHit *hit, Double_t &delta, Double_t trasl[2]) {
  
  trasl[0] = hit->GetPosition().X();
  trasl[1] = hit->GetPosition().Y();

  delta = 0.; // TMath::ATan2(hit->GetPosition().Y() - 0., hit->GetPosition().X() - 0.); // CHECK 
  
}

Int_t PndTrkFwdTrackFinder::FillConformalHitList(PndTrkCluster *cluster) { 
  
  // set the conformal transformation, where the
  // translation and rotation must be already set
  fConformalHitList->SetConformalTransform(conform);
    
  // loop over the cluster hits 
  // and port them to conf plane
  for(int jhit = 0; jhit < cluster->GetNofHits(); jhit++) {
    PndTrkHit *hit = cluster->GetHit(jhit);
    if(hit == fRefHit) continue;
    PndTrkConformalHit chit;
    cout << "HIT " << hit->GetHitID() << " " << hit->IsSttParallel() << " " << hit->IsSttSkew() << endl;
    if(hit->IsSttParallel() == kTRUE) chit = conform->GetConformalSttHit(hit);
    else chit = conform->GetConformalHit(hit); // CHECK
    fConformalHitList->AddHit(&chit);  
    // cout << hit->GetPosition().X() << " " << hit->GetPosition().Y() << " " << hit->GetIsochrone() << " " << hit->IsSttParallel() << " " << " to CONFORMAL " << chit->GetU() << " " << chit->GetV() << " " << chit->GetIsochrone() << endl;

  }
  return fConformalHitList->GetNofHits();
}

void PndTrkFwdTrackFinder::Reset()
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


void PndTrkFwdTrackFinder::Refresh(){
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

void PndTrkFwdTrackFinder::DrawGeometry() {
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

void PndTrkFwdTrackFinder::DrawHits(PndTrkHitList *hitlist) {
  display->cd(1);
  hitlist->Draw();
  display->Update();
  display->Modified();
}

void PndTrkFwdTrackFinder::FillHitMap() {
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
void PndTrkFwdTrackFinder::DrawLists() {
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



ClassImp(PndTrkFwdTrackFinder)
