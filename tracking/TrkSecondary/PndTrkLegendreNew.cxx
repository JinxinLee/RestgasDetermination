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
      

  // loop over all the layers
  for(int ilay = 25; ilay >= 0; ilay--) {
    std::vector< PndTrkHit* > sttinlay = stthitlist->GetHitListFromLayer(ilay);
    cout << endl;
    cout << "======================== > layer " << ilay << " " << sttinlay.size() << endl;

    // loop over hits on this layer
    for(int ihit = 0; ihit < sttinlay.size(); ihit++) {
      // cout << "ihit " << ihit << endl;   
      cout << endl;
      bool associated = false;
      PndTrkHit *thishit = sttinlay.at(ihit);
      if(thishit->IsUsed() == kTRUE) continue;
      int thistubeID = thishit->GetTubeID();
      PndSttTube *thistube = (PndSttTube*) fTubeArray->At(thistubeID);

 //      if(fDisplayOn)  {
// 	Refresh();
// 	char goOnChar;
// 	//	cout << "new hit?" << endl;
// 	//	cin >> goOnChar;
// 	thishit->Draw(kYellow);
// 	display->Update();
// 	display->Modified();
//       }
      
      if(ilay == 25) {
	PndTrkCluster *newcluster0 = new PndTrkCluster();
	newcluster0->AddHit(thishit);
	cout << "START CLUSTER " << clusterlist.GetNofClusters() << " @ " << thishit->GetHitID() << endl;
	clusterlist.AddCluster(newcluster0);
	continue;
      }
      int nclusters = clusterlist.GetNofClusters();
      cout << "hit " << thishit->GetHitID() << " in nclusters: " <<  nclusters << endl;   
      for(int iclus = 0; iclus < nclusters; iclus++) {
	//
	cout << "iclust " << iclus << endl;   
	PndTrkCluster *cluster = clusterlist.GetCluster(iclus);
	bool alreadyinlay = false;

	for(int jhit = cluster->GetNofHits() - 1; jhit >= 0; jhit--) {
	  PndTrkHit *prehit = cluster->GetHit(jhit);

	  //  cout << "compare " << thishit->GetHitID() << " " << prehit->GetHitID() << endl;

	  int pretubeID = prehit->GetTubeID();
	  PndSttTube *pretube = (PndSttTube*) fTubeArray->At(pretubeID);
	  int prelayerID = pretube->GetLayerID();

	  //	  cout << "compare " << thishit->GetHitID() << " " << prehit->GetHitID() << " " << prelayerID << " " << ilay << endl;
// 	  if(fDisplayOn)  {
// 	    char goOnChar;
// 	    //  Refresh();
// 	    cout << "new compare?" << endl;
// 	    cin >> goOnChar;
// 	    Refresh(); 
// 	    thishit->DrawTube(kYellow);
// 	    prehit->DrawTube(kRed);
// 	    display->Update();
// 	    display->Modified();
// 	  }
	  // cout << "compare: yellow " << thishit->GetHitID() << " with red: " << prehit->GetHitID() << " " << prelayerID << " " << ilay << endl;

	  if(prelayerID > ilay + 1) break;
	  if(prelayerID == ilay) {
	    alreadyinlay = true;
	    continue;
	  }
 cout << "compare: yellow " << thishit->GetHitID() << " with red: " << prehit->GetHitID() << " " << prelayerID << " " << ilay << endl;

	  if(thistube->IsNeighboring(pretubeID)) {
	    if(alreadyinlay == false) {
	      cluster->AddHit(thishit);
	      associated = true;
	      cout << "ADDED TO CLUSTER" << endl;
	      break;
	    }
	    else {
	      PndTrkHit *samelayhit = cluster->GetHit(cluster->GetNofHits() - 1);
	      if(thistube->IsNeighboring(samelayhit->GetTubeID())) {
		cluster->AddHit(thishit);
		associated = true;
		cout << "ALREDY IN LAY, BUT NEIGH --> ADDED TO CLUSTER" << endl;
		break;

	      }
	      else {
		PndTrkCluster *newcluster = new PndTrkCluster(*cluster);
		cout << "NEW CLUSTER BEFPRE" << endl;
		newcluster->Print();
		newcluster->DeleteHitAndCompress(newcluster->GetNofHits() - 1);
		cout << "NEW CLUSTER CANDCELLED " << endl;
		newcluster->Print();

		newcluster->AddHit(thishit);
		cout << "add cluster" << endl;
		newcluster->Print();
		clusterlist.AddCluster(newcluster);
		associated = true;
		break;

	      }
	    }
	    //	    associated = true;
	  }
	}
      }
      if(associated == false) {
	cout << " START NEW CLUSTER" << clusterlist.GetNofClusters() << " @ " << thishit->GetHitID() << endl;
	PndTrkCluster *cluster0 = new PndTrkCluster();
	cluster0->AddHit(thishit);
	clusterlist.AddCluster(cluster0);
      }
    }
  }
  
  // ---------- PRINT ---------------------
  for(int iclus = 0; iclus < clusterlist.GetNofClusters(); iclus++) {
    PndTrkCluster *cluster = clusterlist.GetCluster(iclus);
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
  hxy->Draw();
  display->Update();
  display->Modified();  
 
}

  ClassImp(PndTrkLegendreNew)

