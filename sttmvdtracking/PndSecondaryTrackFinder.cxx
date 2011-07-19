////////////////////////////////////////////////////////////
//
// PndSecondaryTrackFinder
// 
// Class for secondary track pattern recognition
//
// authors: Lia Lavezzi - INFN Pavia (2011)
//
////////////////////////////////////////////////////////////


#include "glpk.h"

#include "PndSecondaryTrackFinder.h"

#include "PndSttHit.h"
#include "PndSttPoint.h"
#include "PndSttTube.h"
#include "PndSttMapCreator.h"

#include "PndSdsHit.h"
#include "PndSdsMCPoint.h"

#include "PndTrackCand.h"
#include "PndTrackCandHit.h"
#include "PndTrack.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TVector3.h"
#include "TMath.h"
#include "TArc.h"
#include "TBox.h"
#include "TLine.h"
#include "TPolyLine.h"
#include "TMarker.h"
#include "TParticlePDG.h"
#include "TF1.h"
#include "TLatex.h"

#include <iostream>
#include <cmath>
#include <vector>
#include <iterator>
#include <algorithm>

#define skew 3. * TMath::DegToRad()
using namespace std;


// -----   Default constructor   -------------------------------------------
PndSecondaryTrackFinder::PndSecondaryTrackFinder() : FairTask("STT Stt-Mvd Tracking") { 
  fPersistence = kTRUE;
  fVerbose = 0;
  fDisplayOn = kFALSE;
  fChi2Limit = 500;
  fCountElemLimit = 100;
  fLimit = 2.;

  sprintf(fSttBranch,"STTHit");
  sprintf(fMvdPixelBranch,"MVDHitsPixel");
  sprintf(fMvdStripBranch,"MVDHitsStrip");
}
// -------------------------------------------------------------------------

PndSecondaryTrackFinder::PndSecondaryTrackFinder(Int_t verbose) : FairTask("STT Stt-Mvd Tracking") { 
  fPersistence = kTRUE;
  fVerbose = verbose;
  fDisplayOn = kFALSE;
  fChi2Limit = 500;
  fCountElemLimit = 100;
  fLimit = 2.;

  sprintf(fSttBranch,"STTHit");
  sprintf(fMvdPixelBranch,"MVDHitsPixel");
  sprintf(fMvdStripBranch,"MVDHitsStrip");
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndSecondaryTrackFinder::~PndSecondaryTrackFinder() { 

}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndSecondaryTrackFinder::Init() {
  
  fEventCounter = 0;

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndSecondaryTrackFinder::Init: "
	 << "RootManager not instantiated, return!" << endl;
    return kFATAL;
  }
  //  -----   maps of STT tubes
  // CHECK added 
  PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
  fTubeArray = mapper->FillTubeArray();
  //----------------------------------------------------  end map




  //    get   the MCTrack  array
  fMCTrackArray = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( ! fMCTrackArray) 
    {
      cout << "-E- PndSecondaryTrackFinder::Init: No MCTrack array, return!"
	   << endl;
      return kERROR;
    }



  // Get SttTrackCand array  dal pattern recognition di STT
  fSttTrackCandArray  = (TClonesArray*) ioman->GetObject("STTTrackCand"); 
  if ( ! fSttTrackCandArray) 
    {
      cout << "-E- PndSecondaryTrackFinder::Init: No SttTrack Cand  array, return!"
	   << endl;
      return kERROR;
    }
	
  // Get input array   questi sono i MC point di STT
  fSttPointArray = (TClonesArray*) ioman->GetObject("STTPoint");
  if ( ! fSttPointArray ) {
    cout << "-W- PndSttHelixHitProducer::Init: "
	 << "No STTPoint array, return!" << endl;
    return kERROR;
  }

  // Get input array   hit di STT after digi
  fSttHitArray = (TClonesArray*) ioman->GetObject(fSttBranch);
  //  fSttHitArray = (TClonesArray*) ioman->GetObject("STTHit");
  if ( ! fSttHitArray ) {
    cout << "-W- PndSecondaryTrackFinder::Init: "
	 << "No STTHit array, return!" << endl;
    return kERROR;
  }
  // Create and register output array for PndTrackCand of Stt+Mvd combined

  fSttMvdPndTrackCandArray = new TClonesArray("PndTrackCand");
  ioman->Register("SttMvdTrackCand","SttMvd",fSttMvdPndTrackCandArray, kTRUE);


  // Create and register output array for PndTrack of Stt+Mvd combined

  fSttMvdPndTrackArray = new TClonesArray("PndTrack");
  ioman->Register("SttMvdTrack","SttMvd",fSttMvdPndTrackArray, kTRUE);



  //  -------------------------   get the Mvd hits
  fMvdPixelHitArray = (TClonesArray*) ioman->GetObject(fMvdPixelBranch);
  //  fMvdPixelHitArray = (TClonesArray*) ioman->GetObject("MVDHitsPixel");
  if ( !fMvdPixelHitArray){
    std::cout << "-W- PndSecondaryTrackFinder::Init: " << "No MVD Pixel hitArray, return!" << std::endl;
    return kERROR;
  }
  fMvdStripHitArray = (TClonesArray*) ioman->GetObject(fMvdStripBranch);
  //  fMvdStripHitArray = (TClonesArray*) ioman->GetObject("MVDHitsStrip");

  if ( !fMvdStripHitArray){
    std::cout << "-W- PndSecondaryTrackFinder::Init: " << "No MVD Strip hitArray, return!" << std::endl;
    return kERROR;
  }

  //  -------------------------   get the Mvd track candidates

  fMvdTrackCandArray = (TClonesArray*) ioman->GetObject("MVDRiemannTrackCand");
  if ( !fMvdTrackCandArray){
    std::cout << "-W- PndSecondaryTrackFinder::Init: " << "No MVD TrackCand Array, return!" << std::endl;
    return kERROR;
  }

  cout << "-I- PndSecondaryTrackFinder: Initialization successfull" << endl;
  
  //  -------------------------   get the Mvd MC points

  fMvdMCPointArray = (TClonesArray*) ioman->GetObject("MVDPoint");
  if ( !fMvdMCPointArray){
    std::cout << "-W- PndSecondaryTrackFinder::Init: " << "No MVD MC Point Array, return!" << std::endl;
    return kERROR;
  }

  cout << "-I- PndSecondaryTrackFinder: Initialization successfull" << endl;
  

  // SttMvdGemTrackCand
  fSttMvdGemTrackCandArray  = (TClonesArray*) ioman->GetObject("SttMvdGemTrackCand"); 
  if ( ! fSttMvdGemTrackCandArray) 
    {
      cout << "-E- PndSecondaryTrackFinder::Init: No SttMvdGemTrackCand array, return!"
	   << endl;
      return kERROR;
    }
  
  // SttMvdGemTrack
  fSttMvdGemTrackArray  = (TClonesArray*) ioman->GetObject("SttMvdGemTrack"); 
  if ( ! fSttMvdGemTrackArray) 
    {
      cout << "-E- PndSecondaryTrackFinder::Init: No SttMvdGemTrack array, return!"
	   << endl;
      return kERROR;
    }
 
  if(fDisplayOn) {
    display = new TCanvas("display", "display", 0, 0, 800, 800);
  } 

  if(fDisplayOn) {
    Color_t colors[14] = {kRed, kGreen, kBlue, kMagenta, 
		      kPink, kGray, kViolet, kCyan,
		      kYellow, kOrange, kSpring, kTeal,
		      kAzure, kViolet};
    
    for(int icolor = 0; icolor < 82; icolor++) {
    if(icolor < 14) fColors[icolor] = colors[icolor];
    else if(icolor < 28) fColors[icolor] = colors[icolor - 14] - 3;
    else if(icolor < 42) fColors[icolor] = colors[icolor - 28] - 6;
    else if(icolor < 56) fColors[icolor] = colors[icolor - 42] - 10;
    else if(icolor < 70) fColors[icolor] = colors[icolor - 56] + 3;
    else fColors[icolor] = colors[icolor - 70] + 6;
    cout << "SETUP COLOR " << icolor << " " << fColors[icolor] << endl;

    }
  }


  return kSUCCESS;

}

// -------------------------------------------------------------------------

// CHECK added 
void PndSecondaryTrackFinder::SetParContainers() {
  FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}




void PndSecondaryTrackFinder::WriteHistograms(){


}
void PndSecondaryTrackFinder::Exec(Option_t* opt) {

  cout << "++++++++++++++++++++++++++++++++++++" << endl;

  // MC Tracks ------------ CHECK MC INFO
  std::vector<int> mctracks;
  for(int ipnt = 0; ipnt < fSttPointArray->GetEntriesFast(); ipnt++) {
    PndSttPoint *pnt = (PndSttPoint*) fSttPointArray->At(ipnt);
    if(!pnt) continue;
    int trackid = pnt->GetTrackID();
    std::vector<int>::iterator iter = std::find(mctracks.begin(), mctracks.end(), trackid);
    if(iter == mctracks.end()) mctracks.push_back(trackid);
   
    Int_t tubeID = pnt->GetTubeID();
    PndSttTube *tube = (PndSttTube* ) fTubeArray->At(tubeID);
    TVector3 wireDirection = tube->GetWireDirection();
    if( wireDirection == TVector3(0., 0., 1.)) cout << "MCPOINT " << ipnt << " belongs to track " << trackid  << " not skewed " << endl;
    else cout << "MCPOINT " << ipnt << " belongs to track " << trackid << "     skewed " << endl;
  }
  for(int imc = 0; imc < mctracks.size(); imc++) {
    int mctrackid = mctracks[imc];
    PndMCTrack *mctrk = (PndMCTrack*) fMCTrackArray->At(mctrackid);
    cout << "MC TRACK No. " << mctrackid << endl;
    mctrk->GetMomentum().Print();
  }
  // -------------------------------------

  fDisName = "display_second"; fDisName += fEventCounter;

  fEventCounter++;

  if(fDisplayOn) {
    char goOnChar;
    cout << "press any key" << endl;
    cin >> goOnChar;
    cout << "GOING ON" << endl;
    
    //    h2 = new TH2F("h2", "XY plane", 100, -43, 43, 100, -43, 43);
    //     display->cd();
    //     h2->Draw();
    //     display->Update();
    //     display->Modified();  
    DrawGeometry();
  }

  Int_t nstthits = fSttHitArray->GetEntriesFast();
  //  cout << "EVENTO with " << nstthits << endl;

  std::vector<int> stthits, sttskewedhits, mvdpixhits, mvdstriphits;
  fDetList.clear();
  fDetMap.clear();
  // stthits.clear();
  // cout << "Stt hits" << endl;
  stthits = OrderHits(fSttHitArray, FairRootManager::Instance()->GetBranchId(fSttBranch), false);
  fDetList.push_back(stthits);
  fDetMap[0] =  FairRootManager::Instance()->GetBranchId(fSttBranch);
  //  cout << "Stt skewed hits" << endl;
  sttskewedhits = OrderHits(fSttHitArray, FairRootManager::Instance()->GetBranchId(fSttBranch), true);
  fDetList.push_back(sttskewedhits);
  fDetMap[1] =  FairRootManager::Instance()->GetBranchId(fSttBranch);
  //  cout << "mvd pix hits" << endl;
  mvdpixhits = OrderHits(fMvdPixelHitArray, FairRootManager::Instance()->GetBranchId(fMvdPixelBranch), false);
  fDetList.push_back(mvdpixhits);
  fDetMap[2] =  FairRootManager::Instance()->GetBranchId(fMvdPixelBranch);
  //  cout << "mvd str hits" << endl;
  mvdstriphits = OrderHits(fMvdStripHitArray, FairRootManager::Instance()->GetBranchId(fMvdStripBranch), false);
  fDetList.push_back(mvdstriphits);
  fDetMap[3] =  FairRootManager::Instance()->GetBranchId(fMvdStripBranch);




  if(fDisplayOn) {
    DrawAllHits();
    DrawFoundTracks();
    DrawMCTracks();
  }

  cout << "STT/MVD HITS " << stthits.size() << " "  << sttskewedhits.size() << " " 
       << mvdpixhits.size() << " " << mvdstriphits.size() << endl;


  /**
     DeleteHits("STT", &stthits);
     DeleteHits("STTSKEW", &sttskewedhits);
     DeleteHits("MVDPIXEL", &mvdpixhits);
     DeleteHits("MVDSTRIP", &mvdstriphits);
     fDetList.clear();
     fDetList.push_back(stthits);
     fDetList.push_back(sttskewedhits);
     fDetList.push_back(mvdpixhits);
     fDetList.push_back(mvdstriphits);
     cout << "DELETED STT/MVD HITS " << stthits.size() << " "  << sttskewedhits.size() << " " 
     << mvdpixhits.size() << " " << mvdstriphits.size() << endl;
  **/



  if(fDisplayOn) {
    DrawAllUsableHits();
  }

  // CLUSTER FINDING ====================================================
  //
  std::vector<std::vector< TMatrixT<double> > > tracklist;
  tracklist = ClusterFinder3b(stthits,  FairRootManager::Instance()->GetBranchId(fSttBranch));
  cout << "after cluster finding" << endl;
  PrintClustersBIS(tracklist);
  if(fDisplayOn) {
    DrawClustersBIS(tracklist);
    DrawMultipleAssignedHits(tracklist);
  }

  std::vector<TMatrixT<double> > trackparlist;
  int nclus = tracklist.size();
  for(int iclus = 0; iclus < nclus; iclus++) {
    TMatrixT<double> par(1, 7);
    for(int ipar = 0; ipar < 7; ipar++) par[0][ipar] = 0.;
    par[0][6] = -1;
    trackparlist.push_back(par);
  }
 
//   // +++++++++++++++++++++++++++++==================================++++++++++++++++++++++++++++++
//   // =============================++++++++++++++++++++++++++++++++++==============================
//   std::vector<std::vector<TMatrixT<double> > > tracklist;
//   std::vector<TMatrixT<double> > trackparlist;
//   int nclus = clusterlist.size();
//   for(int iclus = 0; iclus < nclus; iclus++) {
//     std::vector<int> cluster = clusterlist[iclus];
//     std::vector< TMatrixT<double> > track;
//     for(int ihit = 0; ihit < cluster.size(); ihit++) {
//       int hitid = cluster[ihit];
//       FairHit *hit = (FairHit*) fSttHitArray->At(hitid);
//       if(!hit) continue;
//       TVector3 position;
//       hit->Position(position);
//       TMatrixT<double> singlehit(1, 10);
//       singlehit[0][0] = 0; // CHECK useit flag
//       singlehit[0][1] = hitid;
//       singlehit[0][2] = FairRootManager::Instance()->GetBranchId(fSttBranch);
//       singlehit[0][3] = 0;      // parallel
//       singlehit[0][4] = position.X(); 
//       singlehit[0][5] = position.Y();
//       singlehit[0][6] = position.Z();
//       singlehit[0][7] = 1.; // CHECK
//       singlehit[0][8] = 1.; // CHECK
//       singlehit[0][9] = 1.; // CHECK
//       track.push_back(singlehit);
//     }

//     tracklist.push_back(track);
//     TMatrixT<double> par(1, 7);
//     for(int ipar = 0; ipar < 7; ipar++) par[0][ipar] = 0.;
//     par[0][6] = -1;
//     trackparlist.push_back(par);
//   }



  // ====================================================================
  cout << "***************** FIRST FIT *****************" << endl;

  std::vector< TMatrixT<double> > xyparameters;
  // GO TO CONFORMAL PLANE FOR FITTING ==================================
  //  int nclus = clusterlist.size();
  std::vector< int> deletecluster;
  for(int iclus = 0; iclus < nclus; iclus++) {
    //    std::vector<int> cluster = clusterlist[iclus]; // RESTYLE del
    std::vector< TMatrixT<double> > track = tracklist[iclus]; // RESTYLE add
    std::vector< TMatrixT<double> > newtrack = tracklist[iclus]; // RESTYLE add
    Double_t xc, yc, radius, chi2; 
    Int_t countelem;
    //  Bool_t fit = CompleteSttFit(cluster, iclus, xc, yc, radius, chi2, countelem);   // RESTYLE del
    cout << "==> COMPLETE STT FIT" << endl;
    Bool_t fit = CompleteSttFitBIS(&newtrack, iclus, xc, yc, radius, chi2, countelem); // RESTYLE add
    if(fit == kFALSE) {
      cout << "GOTTA DELETE THIS fit fails " << iclus << endl;
      deletecluster.push_back(iclus);
      continue;
    }
    else std::replace(tracklist.begin(), tracklist.end(), track, newtrack);
    
    //     // ++++++++++++++
    //     cout << "######## NEW TRACK ######## " << iclus << endl;
    //     for(int ihit = 0; ihit < newtrack.size(); ihit++)
    //       {
    // 	TMatrixT<double> singlehit = newtrack[ihit];
    // 	if(singlehit[0][0] == -1) continue;
    // 	Int_t hitid = singlehit[0][1];
    // 	Int_t detid = singlehit[0][2];
    // 	cout << "HITID/DETID " << hitid << " " << detid << " " << singlehit[0][3] << endl;
    // 	double x = singlehit[0][4];
    // 	double y = singlehit[0][5];
    // 	double z = singlehit[0][6];
    // 	double dx = singlehit[0][7];
    // 	double dy = singlehit[0][8];
    // 	double dz = singlehit[0][9];
    // 	cout << "xyz    " << x << " " << y << " " << z << endl;
    // 	cout << "dxdydz " << dx << " " << dy << " " << dz << endl;
    //       }
    //     // ++++++++++++++


    Double_t newxc, newyc, newradius, newchi2 = 0;
    std::vector< TMatrixT<double> > newtrack2;
    cout << "FIRST TEST CHI2 " << xc << " " << yc << " " << radius  << endl;
    Bool_t testchi2 = TestChi2BIS(track, xc, yc, radius, iclus, chi2, countelem, newxc, newyc, newradius, &newtrack2, newchi2);
    cout << "testchi2 = " << testchi2 << endl;
    if(testchi2 == kFALSE)  {
      cout << "GOTTA DELETE THIS chi2 fails " << iclus << endl;
      deletecluster.push_back(iclus);
      continue;
    }
    else if(newchi2 != 0) {
      xc = newxc;
      yc = newyc;
      radius = newradius;
      chi2 = newchi2;
      countelem = 0;
      cout << "................. replacing" << endl;
      PrintClustersBIS(tracklist);
      std::replace(tracklist.begin(), tracklist.end(), track, newtrack2);
      cout << "................. done" << endl;
      PrintClustersBIS(tracklist);
    }

    if(fDisplayOn) { 
      cout << "MY FINAL TRACK (FIRST) " << xc << " " << yc << " " << radius << endl;
      TArc *arc2 = new TArc(xc, yc, radius);
      arc2->SetLineColor(kCyan);
      arc2->SetFillStyle(0);
      arc2->Draw("SAME ONLY");
      display->Update();
      display->Modified();
    }


    // save tracks
    TMatrixT<double> param(1, 6);
    param[0][0] = xc;
    param[0][1] = yc;
    param[0][2] = radius;
    param[0][3] = 0;
    param[0][4] = 0;
    param[0][5] = 0;
    xyparameters.push_back(param);
  }
  //  DrawTracks(xyparameters);

  // DELETE CLUSTER ================================ 
  DeleteClusterBIS(&tracklist, deletecluster);
  deletecluster.clear();
  //   // ++++++++++++++
  //   for(int itrk = 0; itrk < tracklist.size(); itrk++) {
  //     cout << "######## FROM TRACK LIST ######## " << itrk <<  endl;
  //     std::vector< TMatrixT<double> > track = tracklist[itrk];
  //     for(int ihit = 0; ihit < track.size(); ihit++)
  //       {
  // 	TMatrixT<double> singlehit = track[ihit];
  // 	if(singlehit[0][0] == -1) continue;
  // 	Int_t hitid = singlehit[0][1];
  // 	Int_t detid = singlehit[0][2];
  // 	cout << "HITID/DETID " << hitid << " " << detid << " " << singlehit[0][3] << endl;
  // 	double x = singlehit[0][4];
  // 	double y = singlehit[0][5];
  // 	double z = singlehit[0][6];
  // 	double dx = singlehit[0][7];
  // 	double dy = singlehit[0][8];
  // 	double dz = singlehit[0][9];
  // 	cout << "xyz    " << x << " " << y << " " << z << endl;
  // 	cout << "dxdydz " << dx << " " << dy << " " << dz << endl;
  //       }
  //   }
  //   // ++++++++++++++



  cout << "after replacing and deleting" << endl;

//   PrintClustersBIS(tracklist);
//   if(fDisplayOn) DrawClustersBIS(tracklist);

  if(fDisplayOn) { 
    Brief(tracklist, xyparameters);
    char goOnChar;
    cout << "cluster found press any key" << endl;
    cin >> goOnChar;
    cout << "GOING ON" << endl;
  }
 

  // NEW HITS COLLECTION  ==============================================
  tracklist.clear();
  // loop on the tracks
  for(int itrk = 0; itrk < xyparameters.size(); itrk++) {

    TMatrixT<double> xypar = xyparameters[itrk];
    double xc = xypar[0][0];
    double yc = xypar[0][1];
    double radius = xypar[0][2];
    std::vector< TMatrixT<double> > track = AddPointsBIS(stthits, FairRootManager::Instance()->GetBranchId(fSttBranch), xc, yc, radius, itrk);
    std::vector< TMatrixT<double> > sorthits = OrderCluster2BIS(track, xc, yc, radius);
    tracklist.push_back(sorthits);
  }
  cout << "after adding the points" << endl;
//   PrintClustersBIS(tracklist);
//   if(fDisplayOn) DrawClustersBIS(tracklist);
 
  if(fDisplayOn) { 
    Brief(tracklist, xyparameters);
    char goOnChar;
    cout << "cluster again press any key" << endl;
    cin >> goOnChar;
    cout << "GOING ON" << endl;
  }
  // FIT THE NEW CLUSTERS AGAIN ===========================================
  cout << " NEW FIT " << endl;
  if(fDisplayOn) Refresh();
  xyparameters.clear();
  // refit tracks
  for(int iclus = 0; iclus < tracklist.size(); iclus++) {
    
    std::vector< TMatrixT<double> > track = tracklist[iclus];

    double xc, yc, radius;
    Double_t firstdrift, delta, trasl[2];
    std::vector< std::vector<double> > conformalhits;  
    Bool_t conftras = ConformalPlaneStt4BIS(track, iclus, conformalhits, firstdrift, delta, trasl);
    //    cout << "conform bis " << conftras << endl;
    Double_t conffit = ConformalFit(conformalhits,  iclus, delta,  trasl,  xc,  yc, radius);
    
    if(fDisplayOn) {
      char goOnChar;
      cout << "Go back to reak plane: cluster " << iclus << endl;
      cin >> goOnChar;
      if(fDisplayOn) Refresh();
      cout << "helix " << xc << " " << yc << " " << radius << endl;     
      TArc *arc = new TArc(xc, yc, radius);
      arc->SetLineColor(kCyan);
      arc->SetFillStyle(0);
      arc->Draw("SAME ONLY");
      display->Update();
      display->Modified();
    }

    // ------------------------------------------------
    int countelem = 0;
    double chi2 = CalculateRedChi2BIS(track, xc, yc, radius, countelem);
    Double_t newxc, newyc, newradius, newchi2 = 0;
    std::vector< TMatrixT<double> > newtrack2;
    cout << "SECOND TEST CHI2" << endl;
    Bool_t testchi2 = TestChi2BIS(track, xc, yc, radius, iclus, chi2, countelem, newxc, newyc, newradius, &newtrack2, newchi2);
    cout << "testchi2 = " << testchi2 << endl;
    if(testchi2 == kFALSE)  {
      cout << "GOTTA DELETE THIS chi2 fails " << iclus << endl;
      deletecluster.push_back(iclus);
      continue;
    }
    else if(newchi2 != 0) {
      xc = newxc;
      yc = newyc;
      radius = newradius;
      chi2 = newchi2;
      cout << "................. replacing" << endl;
      PrintClustersBIS(tracklist);
      std::replace(tracklist.begin(), tracklist.end(), track, newtrack2);
      cout << "................. done" << endl;
      PrintClustersBIS(tracklist);
    }
    // ------------------------------------------------

    TMatrixT<double> param(1, 6);
    param[0][0] = xc;
    param[0][1] = yc;
    param[0][2] = radius;
    param[0][3] = 0;
    xyparameters.push_back(param);
  }
  // DELETE CLUSTER ================================ 
  DeleteClusterBIS(&tracklist, deletecluster);
    deletecluster.clear();

  // 
  if(fDisplayOn) Refresh();

    if(fDisplayOn) { 
      char goOnChar;
      cout << "fit done press any key" << endl;
      cin >> goOnChar;
      cout << "GOING ON" << endl;
    }
  // CLUSTER MERGING =======================================
  std::vector< std::vector< TMatrixT<double> > > newlist;
  std::vector< std::vector<int> > combinations;
  newlist = MergeClustersBIS(tracklist, &combinations);
  if(fDisplayOn) Refresh();
  cout << "after merging" << endl;
  //   PrintClustersBIS(newlist);
  //   if(fDisplayOn)  DrawClustersBIS(newlist);
  
  if(fDisplayOn) { 
    Brief(newlist, xyparameters);
    char goOnChar;
    cout << "merging done any key" << endl;
    cin >> goOnChar;
    cout << "GOING ON" << endl;
  }

  // FIT THE MERGED CLUSTERS =====================================================
  if(fDisplayOn) Refresh();
  xyparameters.clear();

  std::vector<int> restorablecluster;
  // refit tracks
  for(int iclus = 0; iclus < newlist.size(); iclus++) {
    //   std::vector<int> cluster = clusterlist[iclus];
    std::vector< TMatrixT<double> > track = newlist[iclus]; // RESTYLE add
    std::vector< TMatrixT<double> > newtrack = newlist[iclus]; // RESTYLE add
  
    std::vector<std::vector<double> > conformalhits;
    Double_t firstdrift, delta, trasl[2];
    // GO TO CONF PLANE
    Bool_t conftras = ConformalPlaneStt4BIS(newtrack, iclus, conformalhits, firstdrift, delta, trasl);
    Double_t xc, yc, radius;
    // CONF FIT 1
    Bool_t conffit = ConformalFit(conformalhits,  iclus, delta,  trasl,  xc,  yc, radius);
    // chi2
    int countelem = 0;
    double redchi2 = CalculateRedChi2BIS(newtrack, xc, yc, radius, countelem);
    cout << "===> RED CHI2 no. 0 = " << redchi2 << " " << countelem << " <===" << endl;
    
    Double_t tmpxc = xc;
    Double_t tmpyc = yc;
    Double_t tmpradius = radius;
    // intersection finder & fit
    int niter = 2;
    Double_t outxc[niter], outyc[niter], outradius[niter], red2chi2[niter];
    Int_t countelem2[niter];
    for(int iter = 0; iter < 2; iter++) {
      red2chi2[iter] = 10000;
      countelem2[iter] = 0;
      Bool_t refit = RefitConformalBIS(&newtrack, tmpxc, tmpyc, tmpradius, outxc[iter], outyc[iter], outradius[iter]);
      // CHECK no if(refit) ??
      red2chi2[iter] = CalculateRedChi2BIS(newtrack,  outxc[iter], outyc[iter], outradius[iter], countelem2[iter]);
      cout << "===> RED CHI2 no. " << iter + 1 << " = " << red2chi2[iter] << " " << countelem2[iter] << " <===" << endl;
      tmpxc = outxc[iter];
      tmpyc = outyc[iter];
      tmpradius = outradius[iter];

      std::replace(newlist.begin(), newlist.end(), track, newtrack);
  
  
      if(fDisplayOn) {
	cout << "refit helix " << outxc[iter] << " " << outyc[iter] << " " << outradius[iter] << endl;     
	TArc *arc2 = new TArc(outxc[iter], outyc[iter], outradius[iter]);
	if(iter == 0) arc2->SetLineColor(kRed);
	else arc2->SetLineColor(kBlue);
	arc2->SetFillStyle(0);
	arc2->Draw("SAME ONLY");
	display->Update();
	display->Modified();
      }
    }

    double tmpredchi2 = redchi2;
    for(int iter = 0; iter < 2; iter++) {
      //  cout << "ITER " << iter << " " << red2chi2[iter] << " " << tmpredchi2 << endl;
      if(fabs(1. - red2chi2[iter]) < fabs(1. - tmpredchi2)) {
	xc = outxc[iter];
	yc = outyc[iter];
	radius = outradius[iter];
	tmpredchi2 = red2chi2[iter];
	// 	cout << "TAKEN " << iter << " " << red2chi2[iter] << " " << tmpredchi2 << endl;
      }
    }
    

   // ------------------------------------------------
    countelem = 0;
    double chi2 = CalculateRedChi2BIS(track, xc, yc, radius, countelem);
    Double_t newxc, newyc, newradius, newchi2 = 0;
    std::vector< TMatrixT<double> > newtrack2;
    cout << "THIRD TEST CHI2" << endl;
    Bool_t testchi2 = TestChi2BIS(track, xc, yc, radius, iclus, chi2, countelem, newxc, newyc, newradius, &newtrack2, newchi2);
    cout << "testchi2 = " << testchi2 << endl;
    if(testchi2 == kFALSE)  {
      cout << "GOTTA RESTORE the components cluster for chi2 fails " << iclus << endl;
      deletecluster.push_back(iclus);
      continue;
    }
    else if(newchi2 != 0) {
      xc = newxc;
      yc = newyc;
      radius = newradius;
      chi2 = newchi2;
      cout << "................. replacing" << endl;
      PrintClustersBIS(newlist);
      std::replace(newlist.begin(), newlist.end(), track, newtrack2);
      cout << "................. done" << endl;
      PrintClustersBIS(newlist);
    }
    // ------------------------------------------------

    TMatrixT<double> param(1, 6);
    param[0][0] = xc;
    param[0][1] = yc;
    param[0][2] = radius;
    param[0][3] = 0;

    cout << "XC/YC/RADIUS " << xc << " " << yc << " " << radius << endl;
    xyparameters.push_back(param);
  }
  // DELETE CLUSTER ================================ 
  DeleteClusterBIS(&newlist, deletecluster);
  cout << "newlist " << newlist.size() << " " << xyparameters.size() << endl;
  //  ForbidCrossingTracks(&newlist, xyparameters);

  // restore & fit
  for(int idel = 0; idel < deletecluster.size(); idel++) {
    int iclus = deletecluster[idel];
    std::vector<int> combi = combinations[iclus]; 
    for(int icl = 0; icl < combi.size(); icl++) {
      int jclus = combi[icl];
      std::vector< TMatrixT<double> > cluster = tracklist[jclus];
      Double_t xc, yc, radius, chi2; 
      Int_t countelem;
      Bool_t fit = CompleteSttFitBIS(&cluster, newlist.size(), xc, yc, radius, chi2, countelem); // RESTYLE add
      if(fit == kFALSE) {
	cout << "WILL NOT ADD THIS fit fails " << iclus << endl;
	continue;
      }
      else newlist.push_back(cluster);
      TMatrixT<double> param(1, 6);
      param[0][0] = xc;
      param[0][1] = yc;
      param[0][2] = radius;
      param[0][3] = 0;
      
      cout << "XC/YC/RADIUS " << xc << " " << yc << " " << radius << endl;
      xyparameters.push_back(param);
    }
  }
  deletecluster.clear();
  tracklist.clear();
  tracklist = newlist;

  // ------------- CHARGE --------
  for(int itrk = 0; itrk < xyparameters.size(); itrk++) {
    TMatrixT<double> par = xyparameters[itrk];
    std::vector< TMatrixT<double> > track = tracklist[itrk];
     

    //    // ++++++++++++++
    //      cout << "######## WHEN COMPUTING CHARGE ######## " << itrk << endl;
    //      for(int ihit = 0; ihit < track.size(); ihit++)
    //  	{
    //  	  TMatrixT<double> singlehit = track[ihit];
    // 	  if(singlehit[0][0] == -1) continue;
    // 	  Int_t hitid = singlehit[0][1];
    // 	  Int_t detid = singlehit[0][2];
    // 	  cout << "HITID/DETID " << hitid << " " << detid << " " << singlehit[0][3] << endl;
    // 	  double x = singlehit[0][4];
    // 	  double y = singlehit[0][5];
    // 	  double z = singlehit[0][6];
    // 	  double dx = singlehit[0][7];
    // 	  double dy = singlehit[0][8];
    // 	  double dz = singlehit[0][9];
    // 	  cout << "xyz    " << x << " " << y << " " << z << endl;
    // 	  cout << "dxdydz " << dx << " " << dy << " " << dz << endl;
    // 	}
    //       // ++++++++++++++



    Double_t xc = par[0][0];
    Double_t yc = par[0][1];
    
    TMatrixT<double> newpar = par;
    newpar[0][3] = (Double_t) FindChargeBIS(xc, yc, track);
    if(newpar[0][3] != 0) std::replace(xyparameters.begin(), xyparameters.end(), par, newpar);
    cout << "ITRK CHARGE " << itrk << " " << newpar[0][3] << endl;
  }

 if(fDisplayOn) { 
      char goOnChar;
      cout << "FINAL RECAP" << endl;
      cin >> goOnChar;
      Brief(tracklist, xyparameters);
      cin >> goOnChar;
 }

  // ---------------- Z fit -------------
  std::vector< std::vector<int> >  skewedclusterlist;
  std::map<int, int> skewedclustopar;
  for(int itrk = 0; itrk < xyparameters.size(); itrk++) {
    cout << "ITRK " << itrk << endl; 
    std::vector< TMatrixT<double> > track = tracklist[itrk];
    std::vector< TMatrixT<double> > newtrack = tracklist[itrk];
    TMatrixT<double> par = xyparameters[itrk];
    Double_t xc = par[0][0];
    Double_t yc = par[0][1];
    Double_t radius = par[0][2];

    Mat2x3 limits;
    FindLimits(track, limits);
    cout << "BIG LIMITS " << limits[0][0] << " " << limits[0][1] << " " << limits[0][2] << " " << limits[1][0] << " " << limits[1][1] << " " << limits[1][2] << endl;
    
    if(fDisplayOn) { 
      char goOnChar;
      cout << "Z FINDER press any key" << endl;
      cin >> goOnChar;
      cout << "GOING ON" << endl;
      DrawGeometry();
      DrawAllUsableHits();
  
      TArc *arc = new TArc(xc, yc, radius, 0, 360);
      arc->SetLineColor(kRed);
      arc->SetFillStyle(0);
      arc->Draw("SAME ONLY");

      display->Update();
      display->Modified();

    }
  
    std::vector<int> skewedcluster;
    std::vector< TVector3 > intersections;
    //    skewedcluster = ZFinder(sttskewedhits, xc, yc, radius); // RESTYLE add

    skewedcluster = ZFinderBIS(sttskewedhits, xc, yc, radius, limits, intersections); // RESTYLE delete
    if(skewedcluster.size() > 0) {
      skewedclustopar[skewedclusterlist.size()] = itrk;
      skewedclusterlist.push_back(skewedcluster);

      
      // add directly to track list
      for(int ihit = 0; ihit < skewedcluster.size(); ihit++) {
	TVector3 intersection = intersections[ihit];
	//	Bool_t belong = DoesHitBelong( skewedcluster[ihit], xc, yc, radius, intersection, kFALSE);
	TMatrixT<double> singlehit(1, 10);
	singlehit[0][0] = 0; // CHECK useit flag
	singlehit[0][1] = skewedcluster[ihit];
	singlehit[0][2] = FairRootManager::Instance()->GetBranchId(fSttBranch);
	singlehit[0][3] = 1;      // parallel
	singlehit[0][4] = intersection.X();
	singlehit[0][5] = intersection.Y();
	singlehit[0][6] = intersection.Z();
	singlehit[0][7] = 1.; // CHECK
	singlehit[0][8] = 1.; // CHECK
	singlehit[0][9] = 1.; // CHECK
	newtrack.push_back(singlehit);
      }
      std::replace(tracklist.begin(), tracklist.end(), track, newtrack);
    }
  

    //     // CHECK testing
    //     std::vector< TMatrixT<double> > szpar;
    //     std::vector<int> skewedcluster2 = AssociateSkewHitsToXYTrack(xc, yc, radius, sttskewedhits, &szpar);

    //     // CHECK delete these liens
    //     cout << "SKEWED CLUSTER " << itrk << endl;
    //     cout << "MY ONE " << endl;
    //     for(int ihit = 0; ihit < skewedcluster.size(); ihit++) cout <<  " " << skewedcluster[ihit];
    //     cout << endl;
    //     cout << "GIANLUIGI ONE " << endl;
    //     for(int ihit = 0; ihit < skewedcluster2.size(); ihit++) cout <<  " " << skewedcluster2[ihit];
    //     cout << endl;
  }


  for(int iclus = 0; iclus < tracklist.size(); iclus++) {

    if(fDisplayOn) { 
      char goOnChar;
      cout << "Z FIT press any key" << endl;
      cin >> goOnChar;
      cout << "GOING ON" << endl;
      DrawScosZGeometry();
    }
   
    std::vector< TMatrixT<double> > track = tracklist[iclus]; // RESTYLE add
    std::vector< TMatrixT<double> > newtrack = tracklist[iclus]; // RESTYLE add
    Int_t itrk = skewedclustopar[iclus];
    TMatrixT<double> par = xyparameters[itrk];
    Double_t xc = par[0][0];
    Double_t yc = par[0][1];
    Double_t radius = par[0][2];
    Int_t charge = (Int_t) TMath::Sign(1., par[0][3]);
    
    //    // CHECK testing
    //     std::vector< TMatrixT<double> > szpar;
    //     std::vector<int> skewedcluster2 = AssociateSkewHitsToXYTrack(xc, yc, radius, sttskewedhits, &szpar);

    //     for(int ihit = 0; ihit < szpar.size(); ihit++) {
    //       if(fDisplayOn) { 
    // 	TMatrixT<double> sz = szpar[ihit];
    // 	TMarker *m = new TMarker(sz[0][0], sz[0][1], 2);
    // 	m->SetMarkerColor(2);
    // 	m->Draw("SAME");
    //      	TMarker *m2 = new TMarker(sz[0][2], sz[0][3], 2);
    // 	m2->SetMarkerColor(4);
    // 	m2->Draw("SAME");
    // 	display->Update();
    //       display->Modified();  
    //       }
    //     }
    //   //   ZFinder2(skewedcluster, xc, yc, radius, charge);
    //    if(fDisplayOn) { 
    //      char goOnChar;
    //      cout << "Go to conformal plane: cluster " << iclus << endl;
    //      cin >> goOnChar;
    //      cout << "GOING ON" << endl;
    //    }

    Double_t fitm, fitp;

    //    // ++++++++++++++
    //      cout << "######## BEFORE ZFIT3 ######## " << itrk << endl;
    //      for(int ihit = 0; ihit < track.size(); ihit++)
    //  	{
    //  	  TMatrixT<double> singlehit = track[ihit];
    // 	  if(singlehit[0][0] == -1) continue;
    // 	  Int_t hitid = singlehit[0][1];
    // 	  Int_t detid = singlehit[0][2];
    // 	  cout << "HITID/DETID " << hitid << " " << detid << " " << singlehit[0][3] << endl;
    // 	  double x = singlehit[0][4];
    // 	  double y = singlehit[0][5];
    // 	  double z = singlehit[0][6];
    // 	  double dx = singlehit[0][7];
    // 	  double dy = singlehit[0][8];
    // 	  double dz = singlehit[0][9];
    // 	  cout << "xyz    " << x << " " << y << " " << z << endl;
    // 	  cout << "dxdydz " << dx << " " << dy << " " << dz << endl;
    // 	}
    //      // ++++++++++++++

    Bool_t zfit = ZFit3BIS(&newtrack, charge, xc, yc, radius, fitm, fitp);
    if(zfit == kFALSE) continue;
    std::replace(tracklist.begin(), tracklist.end(), track, newtrack);
    
    if(fDisplayOn) { 
      TLine *line = new TLine(-45, -45 * fitm + fitp, 120, 120 * fitm + fitp);
      line->SetLineColor(fColors[iclus]);
      line->Draw("SAME");
      display->Update();
      display->Modified();  
    }

    
    //     // ++++++++++++++
    //      cout << "######## AFTER ZFIT3 ######## " << itrk << endl;
    //      for(int ihit = 0; ihit < newtrack.size(); ihit++)
    //  	{
    //  	  TMatrixT<double> singlehit = newtrack[ihit];
    // 	  if(singlehit[0][0] == -1) continue;
    // 	  Int_t hitid = singlehit[0][1];
    // 	  Int_t detid = singlehit[0][2];
    // 	  cout << "HITID/DETID " << hitid << " " << detid << " " << singlehit[0][3] << endl;
    // 	  double x = singlehit[0][4];
    // 	  double y = singlehit[0][5];
    // 	  double z = singlehit[0][6];
    // 	  double dx = singlehit[0][7];
    // 	  double dy = singlehit[0][8];
    // 	  double dz = singlehit[0][9];
    // 	  cout << "xyz    " << x << " " << y << " " << z << endl;
    // 	  cout << "dxdydz " << dx << " " << dy << " " << dz << endl;
    // 	}
    //       // ++++++++++++++

    xyparameters.at(itrk)[0][4] = fitm;
    xyparameters.at(itrk)[0][5] = fitp;

  }

  // %%%%%%%%%%%%%%%%% PUT THESE INSTEAD OF THE FOLLOWING ONES
 
  // +++++++++ NOW ++++++++++
  std::vector< std::vector< std::pair<int, int> > > completeclusterlist;
  for(int itrk = 0; itrk < xyparameters.size(); itrk++) {
    TMatrixT<double> xypar = xyparameters[itrk];
    std::vector< TMatrixT<double> > completecluster;
    double xc = xypar[0][0];
    double yc = xypar[0][1];
    double radius = xypar[0][2];
    double charge = xypar[0][3];

    std::vector< TMatrixT<double> > track = tracklist[itrk];
    std::vector< TMatrixT<double> > newtrack = OrderClusterInPhiBIS(track, xc, yc, radius);
    std::replace(tracklist.begin(), tracklist.end(), track, newtrack);
  }



  for(int itrk = 0; itrk < xyparameters.size(); itrk++) {
    TMatrixT<double> xypar = xyparameters[itrk];
    std::vector< TMatrixT<double> > completecluster;
    double xc = xypar[0][0];
    double yc = xypar[0][1];
    double radius = xypar[0][2];
    double charge = xypar[0][3];
    std::vector< TMatrixT<double> > track = tracklist[itrk];
 
    cout << " =============== FINAL PARAMETERS " << itrk << endl;
    xypar.Print();
    for(int ihit = 0; ihit < track.size(); ihit++) {
      TMatrixT<double> singlehit = track[ihit];
      cout << singlehit[0][1] << " " << singlehit[0][2] << endl;
    }
  }


  // %%%%%%%%%%%%%%%%% CHECK delete this!!!!!!!!1
  /**
   // +++++++++ NOW ++++++++++
   // list of parallel cluster clusterlist
   // list of skewed cluster skewedclusterlist
   // list of track parameters xyparameters 1:1 with clusterlist
   // map slewed cluster to parameters: skewedclustopar

   // std::vector< std::vector< std::pair<int, int> > >  clusterlist
   // std::vector< std::vector< TMatrixT<double> > >     positionlist
   // std::vector< TMatrixT<double> >                    parameters
   std::vector< std::vector< std::pair<int, int> > > completeclusterlist;
   for(int itrk = 0; itrk < xyparameters.size(); itrk++) {
   TMatrixT<double> xypar = xyparameters[itrk];
   std::vector< std::pair<int, int> > completecluster;
   double xc = xypar[0][0];
   double yc = xypar[0][1];
   double radius = xypar[0][2];
   double charge = xypar[0][3];

   std::vector<int> parallel = clusterlist[itrk];

   // ++++++++++++++++++ skewid ++++++++++++++++++ 
   std::map<int, int>::iterator it;
   int skewid = -1;
   for(it = skewedclustopar.begin(); it != skewedclustopar.end(); it++) {
   int trackid = (*it).second;
   if(trackid != itrk) continue;
   skewid = (*it).first;
   break;
   }	
   if(skewid != -1) {
   std::vector<int> skewed = skewedclusterlist[skewid];
   for(int ihit = 0; ihit < skewed.size(); ihit++) {
   int hitid = skewed[ihit];
   std::pair<int, int> thispair(hitid, FairRootManager::Instance()->GetBranchId(fSttBranch));
   completecluster.push_back(thispair);
   }
   }	
   // ++++++++++++++++++++++++++++++++++++++++++++ 

   for(int ihit = 0; ihit < parallel.size(); ihit++) {
   int hitid = parallel[ihit];
   std::pair<int, int> thispair(hitid, FairRootManager::Instance()->GetBranchId(fSttBranch));
   completecluster.push_back(thispair);
   }
   
   completeclusterlist.push_back(completecluster);
   }

  
   for(int itrk = 0; itrk < xyparameters.size(); itrk++) {
   cout << " =============== FINAL PARAMETERS " << itrk << endl;
   TMatrixT<double> par = xyparameters[itrk];
   par.Print();
   std::vector< std::pair<int, int> > completecluster = completeclusterlist[itrk];
   for(int ihit = 0; ihit < completecluster.size(); ihit++) {
   std::pair<int, int> thispair = completecluster[ihit];
   cout << thispair.first << " " << thispair.second << endl;
   }
   }
  **/
  // %%%%%%%%%%%%%%%%%


  //   if(fDisplayOn) {
  //     fDisName += ".pdf";
  //     display->SaveAs(fDisName);
  //   }

  // CHECK MC INFO
  for(int imc = 0; imc < mctracks.size(); imc++) {
    int mctrackid = mctracks[imc];
    PndMCTrack *mctrk = (PndMCTrack*) fMCTrackArray->At(mctrackid);
    cout << "MC TRACK No. " << mctrackid << endl;
    mctrk->GetMomentum().Print();

    double mcfitm = mctrk->GetMomentum().Z() / mctrk->GetMomentum().Perp();
    double mcfitp = 0;
    cout << "MC FITM/FITP " << mcfitm << " " << mcfitp << endl;
    if(fDisplayOn) {
      TLine *line = new TLine(-45,  -45 * mcfitm + mcfitp, 120, 120* mcfitm + mcfitp);
      line->SetLineColor(4);
      line->SetLineStyle(2);
      line->Draw("SAME");
      display->Update();
      display->Modified();  
    }
  }

  if(fDisplayOn) {
    char goOnChar;
    cout << "NEXT ONE ? press any key" << endl;
    cin >> goOnChar;
    cout << "GOING ON" << endl;
  }


 for(int itrk = 0; itrk < tracklist.size(); itrk++) {

   cout << "COMPUTE PARAMETERS @ FIRST AND LAST" << endl;
   std::vector< TMatrixT<double> > track = tracklist[itrk];
   TMatrixT<double> par = xyparameters[itrk];
   
   TVector3 firstpos, firstmom, dfirstpos;
   Bool_t first = ComputeFirstParameters(track, par, firstpos, dfirstpos, firstmom);
   TVector3 lastpos, lastmom, dlastpos;
   Bool_t last = ComputeLastParameters(track, par, lastpos, dlastpos, lastmom);
   cout << "OUTCOME " << first << " " << last << endl;
   firstpos.Print();
   firstmom.Print();
   lastpos.Print();
   lastmom.Print();
   cout << "###" << endl;
 }



 }


std::vector<int> PndSecondaryTrackFinder::OrderHits(TClonesArray *hitarray, Int_t detId, Bool_t skewed)
{
  std::vector<int> sorthits;
  std::vector<double> distances;
  std::multimap<double, int> mapdistances;

  //  cout << "hitarray " << hitarray->GetEntriesFast() << endl;
  for(int ihit = 0; ihit < hitarray->GetEntriesFast(); ihit++) {
    FairHit* hit = (FairHit*) hitarray->At(ihit);
    if(!hit) continue;
    if(detId == FairRootManager::Instance()->GetBranchId(fSttBranch)) {
      Int_t tubeID = ((PndSttHit*) hit)->GetTubeID();
      PndSttTube *tube = (PndSttTube* ) fTubeArray->At(tubeID);
      TVector3 wireDirection = tube->GetWireDirection();
      if(skewed && wireDirection == TVector3(0., 0., 1.)) continue;
      else if(!skewed && wireDirection != TVector3(0., 0., 1.)) continue;
      //      cout << "reading " << ihit << endl;
    }
    
    TVector3 distance3;
    hit->Position(distance3);
    double distance = distance3.Perp();


    distances.push_back(distance);
    mapdistances.insert(std::pair<double, int>(distance, ihit));
  }

  std::sort(distances.begin(), distances.end());
  //  cout << "size " << distances.size() << endl;

  double tmpdistance = 0;

  for(int j = 0; j < distances.size(); j++) {
    double d = distances[j];

    if(tmpdistance < d) tmpdistance = d;
    else continue;

    std::multimap<double, int>::iterator it;
    int count = 0;
    int n = mapdistances.count(tmpdistance);

    for(it = mapdistances.begin(); it != mapdistances.end(); ++it)
      {
	if(count == n) break;
	if((*it).first != tmpdistance) continue;

	sorthits.push_back((*it).second);
	//	cout << "SORTED HIT " << (*it).second << endl;
	count++;
      }
  }
  return sorthits;
}
 
 
void PndSecondaryTrackFinder::DeleteHit(Int_t ihit, std::vector<int> *hits)
{
  std::vector<int>::iterator iter = std::find(hits->begin(), hits->end(), ihit);
  int where = iter - hits->begin();
  if(where == hits->size()) cout << "where " << where << endl;
  else hits->erase(iter);
}
 
void PndSecondaryTrackFinder::DeleteHitBIS(TMatrixT<double>  hit, std::vector< TMatrixT<double> > *hits)
{
  std::vector< TMatrixT<double> >::iterator iter = std::find(hits->begin(), hits->end(), hit);
  int where = iter - hits->begin();
  if(where == hits->size()) cout << "where " << where << endl;
  else hits->erase(iter);
}
 

 
void PndSecondaryTrackFinder::DeleteHits(TString detectors, std::vector<int> *hits) {
  //  cout << "DELETING FROM  " << detectors << endl;
  for(Int_t itrk = 0; itrk < fSttMvdGemTrackCandArray->GetEntriesFast(); itrk++) {
    PndTrackCand *trkCand = (PndTrackCand*) fSttMvdGemTrackCandArray->At(itrk);
    if(!trkCand) continue;
    Int_t nhits = trkCand->GetNHits(); 
    //    cout << "TRACK CAND " << itrk << " HAS " << nhits << endl;
    for(int ihit = 0; ihit < nhits; ihit++)
      {
	PndTrackCandHit candhit = trkCand->GetSortedHit(ihit);
	Int_t hitId = candhit.GetHitId();
	Int_t detId = candhit.GetDetId();
	//	cout << "hitId " << hitId << " detId " << detId << endl;
	if(detectors.Contains("STT") 
	   && 
	   detId == FairRootManager::Instance()->GetBranchId(fSttBranch)) 
	  {
	    PndSttHit *hit = (PndSttHit *) fSttHitArray->At(hitId);
	    Int_t tubeID = hit->GetTubeID();
	    PndSttTube *tube = (PndSttTube*) fTubeArray->At(tubeID);
	    TVector3 wiredirection = tube->GetWireDirection();
	    // if it is skewed and I am looking for skewed
	    if(TVector3(0., 0., 1.) != wiredirection &&  detectors.Contains("SKEW")) 
	      {
		//		cout << "stt skew deleting " << hitId << endl;
		DeleteHit(hitId, hits);
	      }
	    else  if(TVector3(0., 0., 1.) == wiredirection &&  !detectors.Contains("SKEW")) 
	      { // if it is parallel and I am looking for parallels
		//		cout << "stt paral deleting " << hitId << endl;
		DeleteHit(hitId, hits);
	      }
	    else {
	      // cout << "skipping " << detectors << " " << detId << " " << wiredirection.Z() << endl;
	      continue; // if it is skewed while looking for parallel or parallel while looking for skewed --> continue
	    }
	  }
	else if(detectors.Contains("PIXEL") 
		&& 
		detId == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) 
	  {
	    //	    cout << "mvdpix deleting " << hitId << endl;
	    DeleteHit(hitId, hits);
	  }
	else if(detectors.Contains("STRIP") 
		&& 
		detId == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) 
	  {
	    //	    cout << "mvd str deleting " << hitId << endl;
	    DeleteHit(hitId, hits);
	  }	

	//    else if(detectors.Contains("GEM") 
	// 	 && 
	// 	 detId == FairRootManager::Instance()->GetBranchId(fGemBranch)) DeleteHit(hitId, hits);

      }
  }
}

void PndSecondaryTrackFinder::DrawFoundTracks() {
  for(Int_t itrk = 0; itrk < fSttMvdGemTrackArray->GetEntriesFast(); itrk++) {
    PndTrack *trk = (PndTrack*) fSttMvdGemTrackArray->At(itrk);
    if(!trk) continue;
    //    PndTrackCand *cand = trk->GetTrackCandPtr();
    TVector3 pos1 = trk->GetParamFirst().GetPosition();
    TVector3 pos2 = trk->GetParamLast().GetPosition();
    Int_t charge = trk->GetParamFirst().GetQ();

    Double_t xc, yc, radius, fitm, fitp;
    GetInitialParams(trk, xc, yc, radius, fitm, fitp);

    /**
       int nstep = 30;
       double xmin, xmax;
       xmin = pos1.X();
       xmax = pos2.X();
       double step = (xmax - xmin)/nstep;
       
       Double_t x[nstep], y[nstep], y2[nstep];
       for(int i = 0; i < nstep; i++) {
       x[i] = xmin + step * i;
       y[i] = yc - TMath::Sqrt(radius * radius - (x[i] - xc) * (x[i] - xc));
       y2[i] = yc + TMath::Sqrt(radius * radius - (x[i] - xc) * (x[i] - xc));
       cout << x[i] << " " << y[i] << endl;
       }
       
       TPolyLine *p =  new TPolyLine(10, x, y);
       p->SetLineColor(kRed);
       p->Draw("SAME");
       
       TPolyLine *p2 =  new TPolyLine(10, x, y2);
       p2->Draw("SAME");
    **/

    TVector2 x1 = pos1.XYvector();
    TVector2 x2 = pos2.XYvector();
    TVector2 c(xc, yc);

    TVector2 x1c = x1 - c;
    TVector2 x2c = x2 - c;

    Double_t Phi1 = x1c.Phi() * TMath::RadToDeg();
    Double_t Phi2 = x2c.Phi() * TMath::RadToDeg();
    
    //     cout << "x1 " << x1.X() << " " << x1.Y() << endl;
    //     cout << "x2 " << x2.X() << " " << x2.Y() << endl;
    //     cout << "c  " << c.X() << " " << c.Y() << endl;
    //     cout << "x1c  " << x1c.X() << " " << x1c.Y() <<  endl;
    //     cout << "x2c  " << x2c.X() << " " << x2c.Y() <<  endl;
    //     cout << "Phi1 " << Phi1 << " Phi2 " << Phi2 << endl;
    //     cout << endl;
    TArc *arc2 = new TArc(xc, yc, radius, Phi1, Phi2);
    arc2->SetLineColor(kRed);
    arc2->SetFillStyle(0);
    arc2->Draw("SAME ONLY");

    //   TArc *arc = new TArc(xc, yc, radius);
    //     arc->SetLineColor(kGreen);
    //     arc->SetFillStyle(0);
    //     arc->Draw("SAME ONLY");

    display->Update();
    display->Modified();  
  }
}

void PndSecondaryTrackFinder::DrawFoundTracks(std::vector< TMatrixT<double> > xyparameters) {
  for(int itrk = 0; itrk < xyparameters.size(); itrk++) {
    
    TMatrixT<double> xypar = xyparameters[itrk];
    double xc = xypar[0][0];
    double yc = xypar[0][1];
    double radius = xypar[0][2];

    Color_t color = fColors[itrk];
    TArc *arc = new TArc(xc, yc, radius);
    arc->SetLineColor(color);
    arc->SetFillStyle(0);
    arc->Draw("SAME ONLY");
  }
    display->Update();
    display->Modified();  
}

void PndSecondaryTrackFinder::PrintFoundTracks(std::vector< TMatrixT<double> > xyparameters) {
  for(int itrk = 0; itrk < xyparameters.size(); itrk++) {
    
    TMatrixT<double> xypar = xyparameters[itrk];
    double xc = xypar[0][0];
    double yc = xypar[0][1];
    double radius = xypar[0][2];

    cout << "track " << itrk << ": " << xc << " " << yc << " " << radius << endl;

  }
}



void PndSecondaryTrackFinder::Brief(std::vector< std::vector< TMatrixT<double> > > clusterlist, std::vector< TMatrixT<double> > xyparameters) 
{
  Refresh();
  PrintClustersBIS(clusterlist);
  PrintFoundTracks(xyparameters);
  DrawClustersBIS(clusterlist);
  DrawFoundTracks(xyparameters);
}

void PndSecondaryTrackFinder::DrawMCTracks() {
  for(Int_t itrk = 0; itrk < fMCTrackArray->GetEntriesFast(); itrk++) {
    PndMCTrack *mctrk = (PndMCTrack*) fMCTrackArray->At(itrk);
    if(!mctrk) continue;
    //    if(mctrk->GetMotherID() != -1) continue;
    if(mctrk->GetMotherID() > 0) continue;
    Double_t xc, yc, radius, fitm, fitp;
    GetInitialParamsMC(mctrk, xc, yc, radius, fitm, fitp);

    TVector3 pos1 = mctrk->GetStartVertex();
    TVector2 x1 = pos1.XYvector();
    TVector2 c(xc, yc);

    TVector2 x1c = x1 - c;

    Double_t Phi1 = x1c.Phi() * TMath::RadToDeg();
    TParticlePDG *part = TDatabasePDG::Instance()->GetParticle(mctrk->GetPdgCode());
    int mccharge = 0;
    if(part == NULL) {
      cout << "wrong pdg " << mctrk->GetPdgCode() << endl;
      continue;
    }
    else mccharge = ((int) (part->Charge()/3.));
    Double_t Phi2 = Phi1 - 60 * mccharge;
    if(mctrk->GetMomentum().Mag() < 0.5) {
      Phi1 = 0;
      Phi2 = 360;
    }
    
    TArc *arc2 = new TArc(xc, yc, radius, Phi1, Phi2);
    arc2->SetLineColor(kBlue);
    if(mccharge < 0) arc2->SetLineStyle(2);
    else arc2->SetLineStyle(3);
    arc2->SetFillStyle(0);
    arc2->Draw("SAME ONLY");


    //     TArc *arc = new TArc(xc, yc, radius);
    //     arc->SetLineColor(kBlue);
    //     arc->SetFillStyle(0);
    //     arc->Draw("SAME");
    display->Update();
    display->Modified();  
    
  }
}

// =====================================================================================================
// CHECK :-)GOOD! THE SAME as in PndSttMvdGemTracking... already tested there, could me moved elsewhere.
void PndSecondaryTrackFinder::GetInitialParams(PndTrack * track, Double_t &xc, Double_t &yc, Double_t &radius, Double_t &fitm, Double_t &fitp)
{
  FairTrackParP recopar = track->GetParamFirst();
  TVector3 recomom = recopar.GetMomentum();
  TVector3 recopos = recopar.GetPosition();
  Int_t charge = recopar.GetQ();
  
  radius = recomom.Perp()/0.006;
  Double_t beta;
  
  if(fabs(recomom.X()) >  1e-10) {
    // track from tangent ---------------------
    double reco_m1 = recomom.Y() / recomom.X();
    double reco_q1 = recopos.Y() - recopos.X() * reco_m1;
    double reco_m2 = -1./reco_m1;
    double reco_q2 = recopos.Y() - recopos.X() * reco_m2;
    beta = TMath::ATan2(recomom.X(), recomom.Y());
  }
  else beta = TMath::Sign(1., recomom.Y()) * TMath::Pi(); 
  double recoX0, recoY0;
  if(charge > 0) { 
    xc = recopos.X() + radius * TMath::Cos(beta);
    yc = recopos.Y() - radius * TMath::Sin(beta);
  }
  else {
    xc = recopos.X() - radius * TMath::Cos(beta);
    yc = recopos.Y() + radius * TMath::Sin(beta);
  }
  
  // vector calculation (alternative): tested, it works!
  //   TVector2 direction(recomom.X(), recomom.Y());
  //   direction = direction.Unit();
  //   TVector2 rad(charge * direction.Y() * R, - charge * direction.X() * R);
  
  //   TVector2 center = recopos.XYvector() + rad;
  //   xc = center.X();
  //   yc = center.Y();
  

  // ---------------------------------------------------
  FairTrackParP recoparlast = track->GetParamLast();
  TVector3 recoposlast = recoparlast.GetPosition();


  //   cout << "GETINITPARAM " << " " << charge << " " << xc << " " << yc << " " << radius << endl;
  //   recomom.Print();
  //   recopos.Print(); 
  //   recoposlast.Print();


  fitm = recomom.Z() / recomom.Perp(); // CHECK fitm = pz / pt :-)GOOD!

  // x0 y0
  Double_t d = TMath::Sqrt(xc * xc + yc * yc) - radius;
  Double_t phi =  TMath::ATan2(yc, xc);
  
  Double_t x0 = d * TMath::Cos(phi);
  Double_t y0 = d * TMath::Sin(phi);

  Double_t Phi0 = TMath::ATan2((y0 - yc),(x0 - xc));
  Double_t scosfirst = 0, scoslast = 0.;

  //   cout << "Phi0 " << Phi0 * TMath::RadToDeg() << endl;
  // CHECK :-)GOOD! ...
  TVector2 v(x0 - xc, y0 - yc); 
  double alpha1 = TMath::ATan2(recopos.Y() - y0 + radius * TMath::Sin(Phi0), recopos.X() - x0 + radius * TMath::Cos(Phi0));
  TVector2 p1(recopos.X() - xc, recopos.Y() - yc);
  Double_t Fi1 = CalculatePhi(v, p1, alpha1, Phi0, charge);
  //   cout << "alpha1, Fi1 " << alpha1 * TMath::RadToDeg() << " " << Fi1 * TMath::RadToDeg() << endl;
  //   p1.Print();

  double alpha2 = TMath::ATan2(recoposlast.Y() - y0 + radius * TMath::Sin(Phi0), recoposlast.X() - x0 + radius * TMath::Cos(Phi0));
  TVector2 p2(recoposlast.X() - xc, recoposlast.Y() - yc);
  Double_t Fi2 = CalculatePhi(v, p2, alpha2, Phi0, charge);
  Fi2 = CompareToPreviousPhi(Fi2, Fi1, charge); // CHECK this!
  //   cout << "alpha2, Fi2 " << alpha2 * TMath::RadToDeg() << " " << Fi2 * TMath::RadToDeg() << endl;
  //   p2.Print();
 
  scosfirst = - charge * radius * Fi1; // scos = -q * R * phi CHECK :-)GOOD!
  scoslast = - charge * radius * Fi2; //                     CHECK :-)GOOD!
  // ............. :-)GOOD!

  // z = z0 + scos * fitm
  fitp = (recopos.Z() + recoposlast.Z() - fitm * (scosfirst + scoslast)) / 2.; // CHECK :-)GOOD!

  //   cout << "positions first/last" << endl;
  //   recopos.Print();
  //   recoposlast.Print();

  //   cout << "scosfirst/scoslast " << scosfirst << " " << scoslast << endl;
  //   cout << "fitm/fitp " << fitm << " " << fitp << endl;
  //   cout << "z1/z2 " << fitp + fitm * scosfirst << " " << fitp + fitm * scoslast << endl;

}

// CHECK :-)GOOD! this function has been tested and is ok! already tested there, could me moved elsewhere.
Double_t PndSecondaryTrackFinder::CalculatePhi(TVector2 v, TVector2 p, double alpha, double Phi0, int charge)
{
  Double_t Fi = - charge *  TMath::ACos(v * p / (v.Mod() * p.Mod()));
  double pi = TMath::Pi();
  double pi2 = 2 * pi;
     
  // Fi = h * (pi2 - h * Fi) // should be correct
  if((charge > 0 && (Phi0 > 0 && ((alpha > 0 && alpha > Phi0) ||
				  (alpha < 0 && alpha < Phi0 - pi))
		     ||
		     (Phi0 < 0 && ((alpha > 0 && alpha < pi + Phi0) ||
				   (alpha < 0 && alpha > Phi0))) ))) Fi = - (pi2 + Fi)  ;
  else if((charge < 0 && (Phi0 > 0 && ((alpha > 0 && alpha < Phi0) ||
				       (alpha < 0 && alpha > Phi0 - pi))
			  ||
			  (Phi0 < 0 && ((alpha > 0 && alpha > pi + Phi0) ||
					(alpha < 0 && alpha < Phi0))) ))) Fi = pi2 - Fi  ;
  
  return Fi;
}

// CHECK already tested there, could me moved elsewhere.
Double_t PndSecondaryTrackFinder::CompareToPreviousPhi(Double_t Fi, Double_t Fi_pre, int charge) 
{
  // if(fabs(Fi) < fabs(Fi_pre)) Fi += h * pi2 // CHECK should be ok
  double pi = TMath::Pi();
  double pi2 = 2 * pi;
  
  if(charge < 0 && Fi < Fi_pre) Fi += pi2;
  else if(charge > 0 && Fi > Fi_pre) Fi -= pi2;
  Fi_pre = Fi;
  return Fi;
}
// =====================================================================================================


void PndSecondaryTrackFinder::GetInitialParamsMC(PndMCTrack * mctrack, Double_t &xc, Double_t &yc, Double_t &radius, Double_t &fitm, Double_t &fitp)
{

  TVector3 mcmom = mctrack->GetMomentum();
  TVector3 mcpos = mctrack->GetStartVertex();
  TParticlePDG *part = TDatabasePDG::Instance()->GetParticle(mctrack->GetPdgCode());
  if(part == NULL) {
    cout << "wrong pdg " << mctrack->GetPdgCode() << endl;
    return;
  }
  Int_t charge = (Int_t (part->Charge()/3.));
  
  radius = mcmom.Perp()/0.006;
  Double_t beta;
  
  if(fabs(mcmom.X()) >  1e-10) {
    // track from tangent ---------------------
    //     double mc_m1 = mcmom.Y() / mcmom.X();
    //     double mc_q1 = mcpos.Y() - mcpos.X() * mc_m1;
    //     double mc_m2 = -1./mc_m1;
    //     double mc_q2 = mcpos.Y() - mcpos.X() * mc_m2;
    beta = TMath::ATan2(mcmom.X(), mcmom.Y());
  }
  else beta = TMath::Sign(1., mcmom.Y()) * TMath::Pi(); 
  double mcX0, mcY0;
  if(charge > 0) { 
    xc = mcpos.X() + radius * TMath::Cos(beta);
    yc = mcpos.Y() - radius * TMath::Sin(beta);
  }
  else {
    xc = mcpos.X() - radius * TMath::Cos(beta);
    yc = mcpos.Y() + radius * TMath::Sin(beta);
  }

  fitm = 0; // CHECK
  fitp = 0; // CHECK 
}


std::vector<std::vector<int> > PndSecondaryTrackFinder::ClusterFinder3(std::vector<int> hits, Int_t detId) 
{

  TClonesArray *array;
 
  if(detId == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) array = fMvdPixelHitArray;
  else if(detId == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) array = fMvdStripHitArray;
  else if(detId ==  FairRootManager::Instance()->GetBranchId(fSttBranch)) array = fSttHitArray;
  //  else if(detId ==  FairRootManager::Instance()->GetBranchId(fGemBranch)) array = fGemHitArray;

  int nhits = hits.size();
  int tmphit = -1;
  TVector3 tmpposition;
  std::vector<std::vector<int> > clusterlist;

  // hitid <-> associated (true/false)
  TMatrixT<double> associated(nhits, 2);
  std::map<int, int> associations;
  for(int ihit = 0; ihit < nhits; ihit++) {
    int hitid = hits[ihit];
    associated[ihit][0] = hitid;
    associated[ihit][1] = 0;
    associations.insert(pair<int, int>(hitid, 0));
  }

  for(int ihit = 0; ihit < nhits; ihit++) {
    
    std::vector<int> cluster;
    int hitid = hits[ihit];
   
    FairHit *hit = (FairHit*) array->At(hitid);
    if(!hit) continue;
    TVector3 position;
    hit->Position(position);
    
    for(int jhit = 0; jhit < nhits; jhit++) {
      int hitid2 = hits[jhit];
      if(hitid == hitid2) continue;
      FairHit *hit2 = (FairHit*) array->At(hitid2);
      if(!hit2) continue;
      TVector3 position2;
      hit2->Position(position2);
    
      double distance = (position - position2).Perp();
      if(distance < 1.1 && hitid != hitid2) {
	if(associated[jhit][0] == hitid2) associated[jhit][1]++;
	else cout << "ERROR 2 HERE " << endl;
	associations[hitid2]++;
	cluster.push_back(hitid2);
// 	cout << distance << " pushing back " << hitid << " " << hitid2 << endl;
// 	TArc *arc1 = new TArc(position.X(), position.Y(), 0.5);
// 	arc1->SetFillColor(3);
// 	arc1->Draw("SAME");
// 	TArc *arc2 = new TArc(position2.X(), position2.Y(), 0.5);
// 	arc2->SetFillColor(3);
// 	arc2->Draw("SAME");
// 	display->Update();
// 	display->Modified();
// 	cout << "cluster " << cluster.size() << endl;
// 	position.Print();
// 	char goOnChar;
// 	cout << "cluster finder, press any key" << endl;
// 	cin >> goOnChar;
// 	cout << "GOING ON" << endl;
      }
    }
    if(cluster.size() > 0) {
	if(associated[ihit][0] == hitid) associated[ihit][1]++;
	else cout << "ERROR 1 HERE " << endl;
	associations[hitid]++;
	cluster.push_back(hitid);

     //  cout << "clustering " << cluster.size()  << endl;
      clusterlist.push_back(cluster);
    //   Refresh();
    
    }
  }
  

//   for(int iclus = 0; iclus < clusterlist.size(); iclus++) {
//     std::vector<int> cluster = clusterlist[iclus];
//     cout << "A cluster " << iclus << " has ";
//     for(int ihit = 0; ihit < cluster.size(); ihit++) {
//       int hitid = cluster[ihit];
//       cout << hitid << " ";
//     }
//     cout << endl;
//   }
  cout << "after first finding" << endl;
  PrintClusters(clusterlist);
  if(fDisplayOn) {
    DrawClusters(clusterlist);

    char goOnChar;
    cout << "cluster finder, press any key" << endl;
    cin >> goOnChar;
    cout << "GOING ON" << endl;
  }
// // cut away vice versa, for couple clusters
//   std::vector<int> deleteclusterlist;
//   for(int iclus = 0; iclus < clusterlist.size(); iclus++) {
//     std::vector<int> cluster = clusterlist[iclus];
//     if(cluster.size() > 2) continue;
//     int first = cluster[0];
//     int second = cluster[1];

//     for(int jclus = iclus; jclus < clusterlist.size(); jclus++) {
//       std::vector<int> cluster2 = clusterlist[jclus];
//       if(cluster2.size() > 2) continue;
//       int first2 = cluster2[0];
//       int second2 = cluster2[1];
//       if(first == second2 || first2 == second) deleteclusterlist.push_back(jclus);
//     }
//   }

//   for(int iclus = 0; iclus < clusterlist.size(); iclus++) {
//     std::vector<int> cluster = clusterlist[iclus];
//     cout << "B cluster " << iclus << " has ";
//     for(int ihit = 0; ihit < cluster.size(); ihit++) {
//       int hitid = cluster[ihit];
//       cout << hitid << " ";
//     }
//     cout << endl;
//   }

  
// if a smaller cluster is inside a bigger one, deleteit! ...............
  std::vector<int> deleteclusterlist;
  for(int iclus = 0; iclus < clusterlist.size(); iclus++) {
    std::vector<int> cluster = clusterlist[iclus];
    
    for(int jclus = iclus + 1; jclus < clusterlist.size(); jclus++) {
      std::vector<int> cluster2 = clusterlist[jclus];
      if(cluster2.size() == cluster.size()) continue;


      std::vector<int> bigcluster, smallcluster;
      int small;
      if(cluster.size() > cluster2.size()) 
	{
	  bigcluster = cluster; 
	  smallcluster = cluster2; 
	  small = jclus;
	}
      else 
	{
	 bigcluster = cluster2;
	 smallcluster = cluster; 
	 small = iclus;
	}
      
      std::vector<int>::iterator it;
      int counter = 0;
      std::vector<int> deassociated;
      for(int ihit = 0; ihit < smallcluster.size(); ihit++) {
	int hitid = smallcluster[ihit];
	it = find(bigcluster.begin(), bigcluster.end(), hitid);
	if(it != bigcluster.end()) {
	  counter++;
	  deassociated.push_back(hitid);
	}
      }
      
      if(smallcluster.size() == counter) 
	{
	  cout << "comparing " << iclus << " to " << jclus << " and deleting " << small << endl;
	  it = find(deleteclusterlist.begin(), deleteclusterlist.end(), small);
	  if(it == deleteclusterlist.end()) deleteclusterlist.push_back(small);
	  for(int ide = 0; ide < deassociated.size(); ide++) {
	    int hitid = deassociated[ide];
	    associations[hitid]--;
	    cout << "deassociate " << hitid << endl;
	  }
	}
    }
  }
  // delete actually
  cout << "DELETE " << deleteclusterlist.size() << endl;
  DeleteCluster(&clusterlist, deleteclusterlist);
  for(int ihit = 0; ihit < nhits; ihit++) {
    int hitid = hits[ihit];
    cout << "hitid " << hitid << " associtate to ntracks " << associated[ihit][1] << endl;
  }

//   for(int iclus = 0; iclus < clusterlist.size(); iclus++) {
//     std::vector<int> cluster = clusterlist[iclus];
//     cout << "B cluster " << iclus << " has ";
//     for(int ihit = 0; ihit < cluster.size(); ihit++) {
//       int hitid = cluster[ihit];
//       cout << hitid << " ";
//     }
//     cout << endl;
//   }
  cout << "after deleting contained clusters" << endl;
  PrintClusters(clusterlist);
  if(fDisplayOn) DrawClusters(clusterlist);


  // ---------------------------
  // cluster merging
  std::vector< std::vector<int> > newlist;
  newlist = MergeClusters(clusterlist);
  clusterlist = newlist; 
  
  for(int iclus = 0; iclus < clusterlist.size(); iclus++) {
    std::vector<int> cluster = clusterlist[iclus];
    cout << "C cluster " << iclus << " has ";
    for(int ihit = 0; ihit < cluster.size(); ihit++) {
      int hitid = cluster[ihit];
      cout << hitid << " ";
    }
    cout << endl;
  }
 cout << "after merging" << endl;
  PrintClusters(clusterlist);
  if(fDisplayOn) DrawClusters(clusterlist);

  for(int ihit = 0; ihit < nhits; ihit++) {
    int hitid = hits[ihit];
    cout << "hitid " << hitid << " associtate to ntracks " << associated[ihit][1] << endl;
  }
  // ---------------------------
  // if a hit is inside two cluster, chose the closest one


 // ---------------------------
 if(fDisplayOn) {
   char goOnChar;
   cout << "cluster finder, press any key" << endl;
   cin >> goOnChar;
   cout << "GOING ON" << endl;
 }

  for(int iclus = 0; iclus < clusterlist.size(); iclus++) {
    std::vector<int> cluster = clusterlist[iclus];
    
    for(int ihit = 0; ihit < cluster.size(); ihit++) {
      int hitid = cluster[ihit];
      FairHit *hit = (FairHit*) array->At(hitid);
      if(!hit) continue;
      TVector3 position;
      hit->Position(position);
      
 //      TArc *arc = new TArc(position.X(), position.Y(), 0.5);
//       arc->SetFillColor(fColors[iclus]);
//       //      arc->SetFillStyle(0);
//       arc->Draw("SAME");
//       display->Update();
//       display->Modified();
      
//       cout << "cluster " << cluster.size() << endl;
//       position.Print();
//       char goOnChar;
//       cout << "cluster finder, press any key" << endl;
//       cin >> goOnChar;
//       cout << "GOING ON" << endl;


    }
  }

  return clusterlist;
}


std::vector<std::vector< TMatrixT<double> > > PndSecondaryTrackFinder::ClusterFinder3b(std::vector<int> hits, Int_t detId) 
{

  TClonesArray *array;
 
  if(detId == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) array = fMvdPixelHitArray;
  else if(detId == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) array = fMvdStripHitArray;
  else if(detId ==  FairRootManager::Instance()->GetBranchId(fSttBranch)) array = fSttHitArray;
  //  else if(detId ==  FairRootManager::Instance()->GetBranchId(fGemBranch)) array = fGemHitArray;

  int nhits = hits.size();
  int tmphit = -1;
  TVector3 tmpposition;
  std::vector<std::vector< TMatrixT<double> > > clusterlist;

  // hitid <-> associated (true/false)
  TMatrixT<double> associated(nhits, 2);
  std::map<int, int> associations;
  for(int ihit = 0; ihit < nhits; ihit++) {
    int hitid = hits[ihit];
    associated[ihit][0] = hitid;
    associated[ihit][1] = 0;
    associations.insert(pair<int, int>(hitid, 0));
  }

  for(int ihit = 0; ihit < nhits; ihit++) {
    
    std::vector< TMatrixT<double> > cluster;
    int hitid = hits[ihit];
   
    FairHit *hit = (FairHit*) array->At(hitid);
    if(!hit) continue;
    TVector3 position;
    hit->Position(position);
    
    for(int jhit = 0; jhit < nhits; jhit++) {
      int hitid2 = hits[jhit];
      if(hitid == hitid2) continue;
      FairHit *hit2 = (FairHit*) array->At(hitid2);
      if(!hit2) continue;
      TVector3 position2;
      hit2->Position(position2);
    
      double distance = (position - position2).Perp();
      if(distance < 1.1 && hitid != hitid2) {
	if(associated[jhit][0] == hitid2) associated[jhit][1]++;
	else cout << "ERROR 2 HERE " << endl;
	associations[hitid2]++;

	TMatrixT<double> singlehit2(1, 10);
	singlehit2[0][0] = 0; // CHECK useit flag
	singlehit2[0][1] = hitid2;
	singlehit2[0][2] = FairRootManager::Instance()->GetBranchId(fSttBranch);
	singlehit2[0][3] = 0;     // parallel
	singlehit2[0][4] = position2.X(); 
	singlehit2[0][5] = position2.Y();
	singlehit2[0][6] = position2.Z();
	singlehit2[0][7] = 1.; // CHECK
	singlehit2[0][8] = 1.; // CHECK
	singlehit2[0][9] = 1.; // CHECK
	cluster.push_back(singlehit2);
// 	cout << distance << " pushing back " << hitid << " " << hitid2 << endl;
// 	TArc *arc1 = new TArc(position.X(), position.Y(), 0.5);
// 	arc1->SetFillColor(3);
// 	arc1->Draw("SAME");
// 	TArc *arc2 = new TArc(position2.X(), position2.Y(), 0.5);
// 	arc2->SetFillColor(3);
// 	arc2->Draw("SAME");
// 	display->Update();
// 	display->Modified();
// 	cout << "cluster " << cluster.size() << endl;
// 	position.Print();
// 	char goOnChar;
// 	cout << "cluster finder, press any key" << endl;
// 	cin >> goOnChar;
// 	cout << "GOING ON" << endl;
      }
    }
    if(cluster.size() > 0) {
	if(associated[ihit][0] == hitid) associated[ihit][1]++;
	else cout << "ERROR 1 HERE " << endl;
	associations[hitid]++;

	TMatrixT<double> singlehit(1, 10);
	singlehit[0][0] = 0; // CHECK useit flag
	singlehit[0][1] = hitid;
	singlehit[0][2] = FairRootManager::Instance()->GetBranchId(fSttBranch);
	singlehit[0][3] = 0;     // parallel
	singlehit[0][4] = position.X(); 
	singlehit[0][5] = position.Y();
	singlehit[0][6] = position.Z();
	singlehit[0][7] = 1.; // CHECK
	singlehit[0][8] = 1.; // CHECK
	singlehit[0][9] = 1.; // CHECK
	cluster.push_back(singlehit);
	//  cout << "clustering " << cluster.size()  << endl;
	clusterlist.push_back(cluster);
    //   Refresh();
    
    }
  }
  

//   for(int iclus = 0; iclus < clusterlist.size(); iclus++) {
//     std::vector< TMatrixT<double> > cluster = clusterlist[iclus];
//     cout << "A cluster " << iclus << " has ";
//     for(int ihit = 0; ihit < cluster.size(); ihit++) {
//     TMatrixT<double> singlehit = cluster[ihit];
//     if(singlehit[0][0] == -1) continue;
//     Int_t hitid = singlehit[0][1];
//     Int_t detid = singlehit[0][2];[cluster[ihit];
//       cout << hitid << " ";
//     }
//     cout << endl;
//   }
  cout << "after first finding" << endl;
  PrintClustersBIS(clusterlist);
  if(fDisplayOn) {
    DrawClustersBIS(clusterlist);

    char goOnChar;
    cout << "cluster finder, press any key" << endl;
    cin >> goOnChar;
    cout << "GOING ON" << endl;
  }
// // cut away vice versa, for couple clusters
//   std::vector<int> deleteclusterlist;
//   for(int iclus = 0; iclus < clusterlist.size(); iclus++) {
//     std::vector< TMatrixT<double> > cluster = clusterlist[iclus];
//     if(cluster.size() > 2) continue;
//    TMatrixT<double> first = cluster[0];
//    TMatrixT<double> second = cluster[1];

//     for(int jclus = iclus; jclus < clusterlist.size(); jclus++) {
//       std::vector< TMatrixT<double> > cluster2 = clusterlist[jclus];
//       if(cluster2.size() > 2) continue;
//       TMatrixT<double> first2 = cluster2[0];
//       TMatrixT<double> second2 = cluster2[1];
//       if(first == second2 || first2 == second) deleteclusterlist.push_back(jclus);
//     }
//   }

//   for(int iclus = 0; iclus < clusterlist.size(); iclus++) {
//     std::vector< TMatrixT<double> > cluster = clusterlist[iclus];
//     cout << "B cluster " << iclus << " has ";
//     for(int ihit = 0; ihit < cluster.size(); ihit++) {
//         TMatrixT<double> singlehit = cluster[ihit];
//     if(singlehit[0][0] == -1) continue;
//     Int_t hitid = singlehit[0][1];
//     Int_t detid = singlehit[0][2];[cluster[ihit];
//       cout << hitid << " ";
//     }
//     cout << endl;
//   }
  
// if a smaller cluster is inside a bigger one, deleteit! ...............
  std::vector<int> deleteclusterlist;
  for(int iclus = 0; iclus < clusterlist.size(); iclus++) {
    std::vector< TMatrixT<double> > cluster = clusterlist[iclus];
    
    for(int jclus = iclus + 1; jclus < clusterlist.size(); jclus++) {
      std::vector< TMatrixT<double> > cluster2 = clusterlist[jclus];
      if(cluster2.size() == cluster.size()) continue;


      std::vector< TMatrixT<double> > bigcluster, smallcluster;
      int small;
      if(cluster.size() > cluster2.size()) 
	{
	  bigcluster = cluster; 
	  smallcluster = cluster2; 
	  small = jclus;
	}
      else 
	{
	 bigcluster = cluster2;
	 smallcluster = cluster; 
	 small = iclus;
	}
      
      std::vector<int>::iterator it;
      std::vector< TMatrixT<double> >::iterator it2;
      int counter = 0;
      std::vector< int > deassociated;
      for(int ihit = 0; ihit < smallcluster.size(); ihit++) {
	TMatrixT<double> singlehit = smallcluster[ihit];
	if(singlehit[0][0] == -1) continue;
	int hitid = singlehit[0][1];
	cout << "small " << iclus << " " << jclus << " " << hitid << endl;
	it2 = find(bigcluster.begin(), bigcluster.end(), singlehit);
	if(it2 != bigcluster.end()) {
	  counter++;
	  deassociated.push_back(hitid);
	  cout << "found " << hitid << endl;
	}
      }
      cout << "small " << smallcluster.size() << " big " << bigcluster.size() << "  " << counter << endl;
      
      if(smallcluster.size() == counter) 
	{
	  cout << "comparing " << iclus << " to " << jclus << " and deleting " << small << endl;
	  it = find(deleteclusterlist.begin(), deleteclusterlist.end(), small);
	  if(it == deleteclusterlist.end()) deleteclusterlist.push_back(small);
	  for(int ide = 0; ide < deassociated.size(); ide++) {
	    int hitid = deassociated[ide];
	    associations[hitid]--;
	    cout << "deassociate " << hitid << endl;
	  }
	}
    }
  }
  // delete actually
  cout << "DELETE " << deleteclusterlist.size() << endl;
  DeleteClusterBIS(&clusterlist, deleteclusterlist);
   deleteclusterlist.clear();
 for(int ihit = 0; ihit < nhits; ihit++) {
    int hitid = hits[ihit];
    cout << "hitid " << hitid << " associtate to ntracks " << associated[ihit][1] << endl;
  }

  cout << "after deleting contained clusters" << endl;
  PrintClustersBIS(clusterlist);
  if(fDisplayOn) DrawClustersBIS(clusterlist);


  // ---------------------------
  // cluster merging
  std::vector< std::vector< TMatrixT<double> > > newlist;
  std::vector< std::vector<int> > combinations;
  newlist = MergeClustersBIS(clusterlist, &combinations);
  clusterlist = newlist; 
  
  for(int iclus = 0; iclus < clusterlist.size(); iclus++) {
    std::vector< TMatrixT<double> > cluster = clusterlist[iclus];
    cout << "C cluster " << iclus << " has ";
    for(int ihit = 0; ihit < cluster.size(); ihit++) {
      TMatrixT<double> singlehit = cluster[ihit];
      if(singlehit[0][0] == -1) continue;
      Int_t hitid = singlehit[0][1];
      Int_t detid = singlehit[0][2];
      cout << hitid << " ";
    }
    cout << endl;
  }
  cout << "after merging" << endl;
  PrintClustersBIS(clusterlist);
  if(fDisplayOn) DrawClustersBIS(clusterlist);


  // ---------------------------
  // if a hit is inside two cluster, chose the closest one
  for(int iclus = 0; iclus < clusterlist.size(); iclus++) {
    std::vector< TMatrixT<double> > cluster = clusterlist[iclus];
    cout << "CLEAN AGGREGATION " << iclus << endl;
    std::vector< TMatrixT<double> >  newcluster = CleanAggregations(cluster, clusterlist);
    replace(clusterlist.begin(), clusterlist.end(), cluster, newcluster);
  }
  cout << "after cleaning aggregations" << endl;
  PrintClustersBIS(clusterlist);
  if(fDisplayOn) DrawClustersBIS(clusterlist);


 // ---------------------------
 if(fDisplayOn) {
   char goOnChar;
   cout << "cluster finder, press any key" << endl;
   cin >> goOnChar;
   cout << "GOING ON" << endl;
 }


  return clusterlist;
}



std::vector<std::vector<int> > PndSecondaryTrackFinder::ClusterFinder(std::vector<int> hits, Int_t detId) 
{
  TClonesArray *array;
 
  if(detId == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) array = fMvdPixelHitArray;
  else if(detId == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) array = fMvdStripHitArray;
  else if(detId ==  FairRootManager::Instance()->GetBranchId(fSttBranch)) array = fSttHitArray;
  //  else if(detId ==  FairRootManager::Instance()->GetBranchId(fGemBranch)) array = fGemHitArray;

  int nhits = hits.size();
  int tmphit = -1;
  TVector3 tmpposition;
  std::vector<std::vector<int> > list;
 
  //  int colors[10] = {2, 3, 4, kMagenta, kBlack, kGray, kViolet, kCyan, kYellow, kOrange};


  map<int, bool> usable;
  //  bool usable[nhits];
  for(int ihit = 0; ihit < nhits; ihit++) usable[hits[ihit]] = true;

  int assigned = 0;
  bool newclus = true;
  while(assigned != nhits) {
    std::vector<int> cluster;
    for(int ihit = 0; ihit < nhits; ihit++) {
      int hitid = hits[ihit];
      if(usable[hitid] == false) continue;
 
//       char goOnChar;
//       cout << "cluster finder, press any key" << endl;
//       cin >> goOnChar;
//       cout << "GOING ON" << endl;
 
      FairHit *hit = (FairHit*) array->At(hitid);
      if(!hit) continue;
      TVector3 position;
      hit->Position(position);
      //       if(ihit == 0) {
      // 	tmpposition = position;
      // 	tmphit = ihit;
      //       }

      if(newclus) {
	cluster.push_back(hitid); // CHECK fLimit
	// cout << "push " << hitid << endl;
	usable[hitid] = false;
	assigned++;
	newclus = false;
	tmpposition = position;
	tmphit = ihit;
      }

      double distance = (position - tmpposition).Perp();
      //      cout << ihit << " " <<  hitid  << "/" << hits[tmphit] << " distance " << distance << " " ;

      // to have 1 cm for radius = 15 cm, 3 cm for radius = 42 cm
      //      double fLimit = 2.7; // 0.075 * position.Perp() - 0125.; CHECK
      if(distance < fLimit && position.Perp() != tmpposition.Perp()) {
	// cout << " YES" << endl;
	cluster.push_back(hitid); // CHECK fLimit
	// cout << "push " << hitid << endl;
	usable[hitid] = false;
	assigned++;
	// 	if(fDisplayOn) {
	// 	  TLine *line = new TLine(position.X(), position.Y(), tmpposition.X(), tmpposition.Y());
	// 	  line->SetLineColor(fColors[list.size()]);
	// 	  line->Draw("SAME");
	// 	  display->Update();
	// 	  display->Modified();  
	// 	}
	tmpposition = position;
     	tmphit = ihit;
      }
      //      else cout << "NO" << endl;
    }

    newclus = true;
    if(cluster.size() > 2) {
      //     cout << "SAVE CLUS" << endl;
      list.push_back(cluster); // CHECK
    }
    //    cout << "---------- ASSIGNED " << 	assigned << " " << nhits << endl;;
  }

  // reset usable all the hits and then
  // delete the ones already gathered
  int remaining = nhits;
  for(int ihit = 0; ihit < nhits; ihit++) usable[hits[ihit]] = true;
  for(int iclus = 0; iclus < list.size(); iclus++) {
    std::vector<int> cluster = list[iclus];
    if(fDisplayOn) DrawLinks(cluster, FairRootManager::Instance()->GetBranchId(fSttBranch), iclus);
    int nhits2 = cluster.size();
    for(int ihit = 0; ihit < nhits2; ihit++) {
      int hitid = cluster[ihit];
      usable[hitid] = false;
      remaining--;
    }
  }

  int nclus = list.size();
  for(int iclus = 0; iclus < nclus; iclus++) {
    cout << "first found clu " << iclus << ": ";
    std::vector<int> cluster = list[iclus];
    for(int ihit = 0; ihit < cluster.size(); ihit++) {
      cout << cluster[ihit] << " ";
    }
    cout << endl;
  }


  // **************************************************
  // recompare distances of remaining hits ************
  // cout << "remaining hits " << remaining << endl;

  for(int ihit = 0; ihit < nhits; ihit++) {
    if(remaining == 0) break;
    int hitid = hits[ihit];
    if(usable[hitid] == false) continue;
    // cout << "hitid " << hitid << endl;
    FairHit *hit = (FairHit*) array->At(hitid);
    if(!hit) continue;
    TVector3 position;
    hit->Position(position);
    // cout << "list size " << list.size() << endl;
    remaining--;
    for(int iclus = 0; iclus < list.size(); iclus++) {
      std::vector<int> *cluster = &list[iclus];
      int nhits2 = cluster->size();

      // cout << "clu " << iclus << " " << nhits2 << endl;
      for(int jhit = 0; jhit < nhits2; jhit++) {
	int hitid2 = cluster->at(jhit);
	FairHit *hit2 = (FairHit*) array->At(hitid2);
	if(!hit2) continue;
	TVector3 position2;
	hit2->Position(position2);
    
	double distance = (position - position2).Perp();
	// cout << "hit " << hitid2 << " " << distance << endl;

	// to have 1 cm for radius = 15 cm, 3 cm for radius = 42 cm
	//	double fLimit = 2.7; // 0.075 * position.Perp() - 0125.; CHECK
	if(distance < fLimit && position2.Perp() != position.Perp()) {
	  // cout << "hit " << hitid << " to clus " << iclus << endl;
	  cluster->push_back(hitid);
	  usable[hitid] = false;

	  if(fDisplayOn) {
	    TLine *line = new TLine(position.X(), position.Y(), position2.X(), position2.Y());
	    line->SetLineColor(2);
	    line->SetLineStyle(2);
	    line->Draw("SAME");
	    display->Update();
	    display->Modified();  
	  }

	  break;
	}
      }
    }
  }
  nclus = list.size();
  for(int iclus = 0; iclus < nclus; iclus++) {
    cout << "with added hits " << iclus << ": ";
    std::vector<int> cluster = list[iclus];
    for(int ihit = 0; ihit < cluster.size(); ihit++) {
      cout << cluster[ihit] << " ";
    }
    cout << endl;
  }


  // **************************************************
  // find boundary                         ************
  nclus = list.size();
  TMatrixT<double> boundaries(nclus, 4);
  for(int iclus = 0; iclus < nclus; iclus++) {
    std::vector<int> cluster = list[iclus];
    //   fDisplayOn = kFALSE; // CHECK
    FindBoundary(iclus, cluster, FairRootManager::Instance()->GetBranchId(fSttBranch), boundaries, kFALSE);
    //    fDisplayOn = kTRUE; // CHECK
  }

  // ************************************
  // ri-ordering                      ***
  //   nclus = list.size();
  //   for(int iclus = 0; iclus < nclus; iclus++) {
  //     std::vector<int> cluster = list[iclus];
  //     std::vector<int> sorthits = OrderCluster(cluster, FairRootManager::Instance()->GetBranchId(fSttBranch), TVector3(0., 0., 0.));
  //     std::replace(list.begin(), list.end(), cluster, sorthits);
  //   }

  // ... according to the boundary
  nclus = list.size();
  for(int iclus = 0; iclus < nclus; iclus++) {
    std::vector<int> cluster = list[iclus];
    double xmin = boundaries[iclus][0];
    double ymin = boundaries[iclus][1];
    double xmax = boundaries[iclus][2];
    double ymax = boundaries[iclus][3];

    double large = fabs(xmax - xmin);
    double high = fabs(ymax - ymin);
    TVector3 point;
    if(large > high) point.SetXYZ(xmin, (ymin + ymax) / 2., 35.);
    else point.SetXYZ((xmin + xmax) / 2., ymin, 35.);
    
    std::vector<int> sorthits = OrderCluster(cluster, FairRootManager::Instance()->GetBranchId(fSttBranch), point);
    std::replace(list.begin(), list.end(), cluster, sorthits);
  }
  
  
  for(int iclus = 0; iclus < nclus; iclus++) {
    cout << "resorted clu " << iclus << ": ";
    std::vector<int> cluster = list[iclus];
    for(int ihit = 0; ihit < cluster.size(); ihit++) {
      cout << cluster[ihit] << " ";
    }
    cout << endl;
  }

  //  if(fDisplayOn) Refresh();


  for(int iclus = 0; iclus < nclus; iclus++) {
    std::vector<int> cluster = list[iclus];
    if(fDisplayOn) DrawLinks(cluster, FairRootManager::Instance()->GetBranchId(fSttBranch), iclus);
  }

  //  if(fDisplayOn) Refresh();

  // **************************************************
  // combine pieces           *************************
  
  for(int iclus = 0; iclus < list.size(); iclus++) {
    std::vector<int> cluster1 = list[iclus];

    FairHit *hit1first = (FairHit*) array->At(cluster1.at(0));
    if(!hit1first) continue;
    TVector3 position1first;
    hit1first->Position(position1first);

    FairHit *hit1last = (FairHit*) array->At(cluster1.at(cluster1.size() - 1));
    if(!hit1last) continue;
    TVector3 position1last;
    hit1last->Position(position1last);
   
    for(int jclus = iclus + 1; jclus < list.size(); jclus++) {
      //      cout << "comparing " << iclus << " " << jclus << endl;
      //       position1first.Print();
      //       position1last.Print();

      std::vector<int> cluster2 = list[jclus];

      FairHit *hit2first = (FairHit*) array->At(cluster2.at(0));
      if(!hit2first) continue;
      TVector3 position2first;
      hit2first->Position(position2first);

      FairHit *hit2last = (FairHit*) array->At(cluster2.at(cluster2.size() - 1));
      if(!hit2last) continue;
      TVector3 position2last;
      hit2last->Position(position2last);

      //       position2first.Print();
      //       position2last.Print();

      double distance1f2f = (position1first - position2first).Perp();      
      double distance1f2l = (position1first - position2last).Perp();      
      double distance1l2f = (position1last - position2first).Perp();      
      double distance1l2l = (position1last - position2last).Perp();      

      //      Double_t fLimit = 2.7; // CHECK
      if(
	 distance1f2f < fLimit || 
	 distance1f2l < fLimit || 
	 distance1l2f < fLimit || 
	 distance1l2l < fLimit) {

	std::vector<int> cluster1b;
	cluster1b.insert(cluster1b.end(), cluster1.begin(), cluster1.end()); 
	cluster1b.insert(cluster1b.end(), cluster2.begin(), cluster2.end()); 

	std::vector< std::vector<int> >::iterator iter = std::find(list.begin(), list.end(), cluster2);
	int where = iter - list.begin();
	list.erase(iter);

	TMatrixT<double> bounds(1, 4);
	FindBoundary(0, cluster1b, FairRootManager::Instance()->GetBranchId(fSttBranch), bounds, kFALSE);

	double xmin = bounds[0][0];
	double ymin = bounds[0][1];
	double xmax = bounds[0][2];
	double ymax = bounds[0][3];

	double large = fabs(xmin - xmax);
	double high = fabs(ymin - ymax);
	TVector3 point;
	if(large > high) point.SetXYZ(xmin, (ymin + ymax) / 2., 35.);
	else point.SetXYZ((xmin + xmax) / 2., ymin, 35.);

	std::vector<int> sorthits = OrderCluster(cluster1b, FairRootManager::Instance()->GetBranchId(fSttBranch), point);
	std::replace(list.begin(), list.end(), cluster1, sorthits);
	cout << "combination of " << iclus << " " << jclus << endl;


	hit1first = (FairHit*) array->At(sorthits.at(0));
	if(!hit1first) continue;
	hit1first->Position(position1first);
	hit1last = (FairHit*) array->At(sorthits.at(sorthits.size() - 1));
	if(!hit1last) continue;
	hit1last->Position(position1last);
	jclus = iclus; 
	cluster1 = list[iclus];
      }


    }

  }

  nclus = list.size();  
  for(int iclus = 0; iclus < nclus; iclus++) {
    cout << "combined clu " << iclus << ": ";
    std::vector<int> cluster = list[iclus];
    for(int ihit = 0; ihit < cluster.size(); ihit++) {
      cout << cluster[ihit] << " ";
    }
    cout << endl;
  }


  // find boundary                         ************
  nclus = list.size();
  boundaries.ResizeTo(nclus, 4);
  for(int iclus = 0; iclus < nclus; iclus++) {
    std::vector<int> cluster = list[iclus];
    FindBoundary(iclus, cluster, FairRootManager::Instance()->GetBranchId(fSttBranch), boundaries, kTRUE);
  }

  return list;
}

std::vector<int> PndSecondaryTrackFinder::OrderCluster(std::vector<int> cluster, Int_t detId, TVector3 point) {

  TClonesArray *array;
 
  if(detId == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) array = fMvdPixelHitArray;
  else if(detId == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) array = fMvdStripHitArray;
  else if(detId ==  FairRootManager::Instance()->GetBranchId(fSttBranch)) array = fSttHitArray;
  //  else if(detId ==  FairRootManager::Instance()->GetBranchId(fGemBranch)) array = fGemHitArray;

  std::vector<int> sorthits;
  std::vector<double> distances;
  std::multimap<double, int> mapdistances;
    
  for(int ihit = 0; ihit < cluster.size(); ihit++) {
    int hitid = cluster[ihit];
      
    FairHit* hit = (FairHit*) array->At(hitid);
    if(!hit) continue;

    TVector3 distance3;
    hit->Position(distance3);
    double distance = (point - distance3).Perp();
      
    distances.push_back(distance);
    mapdistances.insert(std::pair<double, int>(distance, hitid));
  }
    
  std::sort(distances.begin(), distances.end());

  double tmpdistance = 0;

  for(int j = 0; j < distances.size(); j++) {
    double d = distances[j];
      
    if(tmpdistance < d) tmpdistance = d;
    else continue;

    std::multimap<double, int>::iterator it;
    int count = 0;
    int n = mapdistances.count(tmpdistance);
      
    for(it = mapdistances.begin(); it != mapdistances.end(); ++it)
      {
	if(count == n) break;
	if((*it).first != tmpdistance) continue;
	  
	sorthits.push_back((*it).second);
	count++;
      }
  }

  //  cout << "sorted" << endl;
  for(int ihit = 0; ihit < sorthits.size(); ihit++) {
    //    cout << sorthits[ihit] << " ";
    int hitid = sorthits[ihit];
    
    FairHit* hit = (FairHit*) array->At(hitid);
    if(!hit) continue;
    
    TVector3 distance3;
    hit->Position(distance3);
    double distance = distance3.Perp();
    //    cout << distance << endl;     
  }
  //  cout << endl;

  return sorthits;
}




void PndSecondaryTrackFinder::FindBoundary(Int_t iclus, std::vector<int> cluster, Int_t detId, TMatrixT<double> &boundaries, Bool_t draw) {

  TClonesArray *array;
 
  if(detId == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) array = fMvdPixelHitArray;
  else if(detId == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) array = fMvdStripHitArray;
  else if(detId ==  FairRootManager::Instance()->GetBranchId(fSttBranch)) array = fSttHitArray;
  //  else if(detId ==  FairRootManager::Instance()->GetBranchId(fGemBranch)) array = fGemHitArray;

  //  TMatrixT<double> boundaries(1, 4);
  int nhits2 = cluster.size();

  double xmin = 42, ymin = 42, xmax = -42, ymax = -42;
  TVector3 vertices[4]; // CHECK not needed

  for(int ihit = 0; ihit < nhits2; ihit++) {
    int hitid = cluster.at(ihit);
    FairHit *hit = (FairHit*) array->At(hitid);
    if(!hit) continue;
    TVector3 position;
    hit->Position(position);
    if(position.X() < xmin) { xmin = position.X(); vertices[0] = position; }
    if(position.Y() < ymin) { ymin = position.Y(); vertices[3] = position; }
    if(position.X() > xmax) { xmax = position.X(); vertices[2] = position; }
    if(position.Y() > ymax) { ymax = position.Y(); vertices[1] = position; }
  }

  boundaries[iclus][0] = xmin - 0.5;
  boundaries[iclus][1] = ymin - 0.5;
  boundaries[iclus][2] = xmax + 0.5;
  boundaries[iclus][3] = ymax + 0.5;

  // cout << xmin << " "  << ymin << " " << xmax << " " << ymax << endl;
  if(fDisplayOn && draw) {

    TBox *b = new TBox(boundaries[iclus][0], boundaries[iclus][1], boundaries[iclus][2], boundaries[iclus][3]);
    b->SetFillStyle(0);
    b->SetLineColor(fColors[iclus]);
    b->Draw("SAME");
      
    //     double xpoly[5] = {xmin, xmin, xmax, xmax, xmin};
    //     double ypoly[5] = {ymin, ymax, ymax, ymin, ymin};
    //     TPolyLine *p = new TPolyLine(5, xpoly, ypoly);
    //     p->SetLineColor(fColors[0]);
    //     p->Draw("SAME");
      
    //    double xpoly[5] = {vertices[0].X() - 1, vertices[1].X(), vertices[2].X(), vertices[3].X(), vertices[0].X()};
    //      double ypoly[5] = {vertices[0].Y(), vertices[1].Y(), vertices[2].Y(), vertices[3].Y(), vertices[0].Y()};
    //     TPolyLine *p = new TPolyLine(5, xpoly, ypoly);
    //     p->SetLineColor(fColors[0]);
    //     p->Draw("SAME");
    
    display->Update();
    display->Modified();  
  }
}

void PndSecondaryTrackFinder::FindLimits(std::vector< TMatrixT<double> > cluster, Mat2x3 &m) {

  double xmin = 42, ymin = 42, xmax = -42, ymax = -42;
  double dmin = 1000, dmax = -1;
  
  for(int ihit = 0; ihit < cluster.size(); ihit++) {
    TMatrixT<double> singlehit = cluster[ihit];
    if(singlehit[0][0] == -1) continue;
    Int_t hitid = (Int_t) singlehit[0][1];
    Int_t detid = (Int_t) singlehit[0][2];
    if(singlehit[0][3] == 1) continue; // CHECK no skewed
    double x = singlehit[0][4];
    double y = singlehit[0][5];
    double d = TMath::Sqrt(x * x + y * y);
    cout << "xy " << x << " " << y << " " << d << endl;
    if(x < xmin) xmin = x; 
    if(y < ymin) ymin = y;
    if(x > xmax) xmax = x;
    if(y > ymax) ymax = y;
    if(d < dmin) dmin = d;
    if(d > dmax) dmax = d;
  }
  cout << "IN LIMITS " << xmin << " " << ymin << " " << dmin << " " << xmax << " " << ymax << " " << dmax << endl;
  m[0][0] = xmin;
  m[0][1] = ymin;
  m[0][2] = dmin;
  m[1][0] = xmax;
  m[1][1] = ymax;
  m[1][2] = dmax;

}


void PndSecondaryTrackFinder::DrawGeometry() {

  h2 = new TH2F(fDisName, fDisName, 100, -43, 43, 100, -43, 43);
  display->cd();
  h2->Draw();
  display->Update();
  display->Modified();  
 
}

void PndSecondaryTrackFinder::DrawGeometryConformal(Double_t umin, Double_t vmin, Double_t umax, Double_t vmax){
  h2 = new TH2F("h2", "XY plane", 100, umin, umax, 100, vmin, vmax);
  display->cd();
  h2->Draw();
  display->Update();
  display->Modified();  
 
}

void PndSecondaryTrackFinder::DrawScosZGeometry() {

  h2 = new TH2F("h2", "Z vs Scos", 100, -150, 150, 100, -245, 320);
  h2->GetXaxis()->SetTitle("Scos{#lambda}");
  h2->GetYaxis()->SetTitle("z");
  display->cd();
  h2->Draw();
  display->Update();
  display->Modified();  
  
}

void PndSecondaryTrackFinder::DrawHitsColor(std::vector<int> hits, Int_t detId, Int_t color) {
  TClonesArray* array;
  
  if(detId == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch))  array = fMvdPixelHitArray;
  else if(detId == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) array = fMvdStripHitArray;
  else if(detId ==  FairRootManager::Instance()->GetBranchId(fSttBranch)) array = fSttHitArray;
  //  else if(detId ==  FairRootManager::Instance()->GetBranchId(fGemBranch)) array = fGemHitArray;
  
  int nhits = hits.size();
  //  cout << "nhits " << nhits << endl;
  for(int ihit = 0; ihit < nhits; ihit++) {
    int hitid = hits[ihit];
    FairHit *hit = (FairHit *) array->At(hitid);
//     cout << "HIT ----> " << hitid << " " << detId << endl;
    if(!hit) { 
      //  cout << "HIT NOT FOUND" << endl; 
      continue; 
    }
    TVector3 position;
    hit->Position(position);
 //    position.Print();
    TMarker *mrk = new TMarker(position.X(), position.Y(), 3);
    mrk->SetMarkerColor(color);

    if(detId ==  FairRootManager::Instance()->GetBranchId(fSttBranch)) {
      Int_t tubeID = ((PndSttHit* ) hit)->GetTubeID();
      PndSttTube *tube = (PndSttTube* ) fTubeArray->At(tubeID);
      
      TVector3 wireDirection = tube->GetWireDirection();
    
      if(wireDirection != TVector3(0., 0., 1.))  {
	mrk->Draw("SAME");
      }
      else {

// 	cout << "DRAWTING " << hitid << " " << color << endl;
// 	position.Print();
	TArc *arc = new TArc(position.X(), position.Y(), tube->GetRadIn()); // ((PndSttHit* ) hit->GetIsochrone()));
// 	arc->SetLineColor(color);
// 	arc->SetFillStyle(0);
	arc->SetFillColor(color);

	arc->Draw("SAME");
	TString text = "";
	text += hitid;
	TLatex *latex = new TLatex(position.X() + 0.1, position.Y() + 0.1, text);
	latex->SetTextSize(0.01);
	latex->SetTextColor(kBlack);
	latex->Draw("SAME");

	/**
	   TArc *arcd = new TArc(position.X(), position.Y(), ((PndSttHit*) hit)->GetIsochrone());
	   arcd->SetLineColor(color);
	   arcd->SetFillStyle(1);
	   arcd->Draw("SAME");
	**/
      }
    }
    else if(detId == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) { mrk->SetMarkerStyle(21); mrk->Draw("SAME"); }
    else if(detId == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) { mrk->SetMarkerStyle(25); mrk->Draw("SAME"); }
  }
	
  display->Update();
  display->Modified();  
}

void PndSecondaryTrackFinder::DrawAllHits() {
  for(int idet = 0; idet < fDetList.size(); idet++) {
    std::vector<int> hits = fDetList[idet];
    int detId = fDetMap[idet];
    DrawHitsColor(hits, detId, kGray);
  }
}

void PndSecondaryTrackFinder::DrawAllUsableHits() {
  for(int idet = 0; idet < fDetList.size(); idet++) {
    std::vector<int> hits = fDetList[idet];
    int detId = fDetMap[idet];
    //     cout << "************************" << endl;
    //     cout << "DETLIST " << idet << " " << fDetMap[idet] << " " << hits.size() <<  endl;
    DrawHitsColor(hits, detId, kBlack);
  }
}

void PndSecondaryTrackFinder::DrawHits(std::vector<int> hits, Int_t detId) {
  DrawHitsColor(hits, detId, kGray);
}

void PndSecondaryTrackFinder::DrawUsableHits(std::vector<int> hits, Int_t detId) {
  DrawHitsColor(hits, detId, kBlack);
}


void PndSecondaryTrackFinder::Refresh()
{
  // CHECK
  char goOnChar;
  cout << "Refresh?" << endl;
  //  cin >> goOnChar;
  cout << "GOING ON" << endl;
  DrawGeometry();
  DrawAllUsableHits();
  DrawFoundTracks();
  DrawMCTracks();
}


void PndSecondaryTrackFinder::DrawLinks(std::vector<int> cluster, Int_t detId, Int_t iclus) {
  TClonesArray* array;
  
  if(detId == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) array = fMvdPixelHitArray;
  else if(detId == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) array = fMvdStripHitArray;
  else if(detId ==  FairRootManager::Instance()->GetBranchId(fSttBranch)) array = fSttHitArray;
  //  else if(detId ==  FairRootManager::Instance()->GetBranchId(fGemBranch)) array = fGemHitArray;
  
  int nhits = cluster.size();
  TVector3 tmpposition;
  for(int ihit = 0; ihit < nhits; ihit++) {
    int hitid = cluster[ihit];
     
    FairHit *hit = (FairHit*) array->At(hitid);
    if(!hit) continue;
    TVector3 position;
    hit->Position(position);
    
    if(ihit > 0) {


      TLine *line = new TLine(position.X(), position.Y(), tmpposition.X(), tmpposition.Y());
      line->SetLineColor(fColors[iclus]);
      line->Draw("SAME");
      display->Update();
      display->Modified();  
    }

    tmpposition = position;
  }
}


// not working
std::vector<std::vector<int> > PndSecondaryTrackFinder::ClusterFinder2(std::vector<int> hits, Int_t detId) 
{
  TClonesArray *array;
 
  if(detId == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) array = fMvdPixelHitArray;
  else if(detId == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) array = fMvdStripHitArray;
  else if(detId ==  FairRootManager::Instance()->GetBranchId(fSttBranch)) array = fSttHitArray;
  //  else if(detId ==  FairRootManager::Instance()->GetBranchId(fGemBranch)) array = fGemHitArray;

  int nhits = hits.size();
  int tmphit = -1;
  TVector3 tmpposition;
  std::vector<std::vector<int> > list;
  std::multimap<int, int> mapdistances;

  for(int ihit = 0; ihit < nhits; ihit++) {
    int hitid = hits[ihit];
  
    FairHit *hit = (FairHit*) array->At(hitid);
    if(!hit) continue;
    TVector3 position;
    hit->Position(position);
  
    for(int jhit = ihit; jhit < nhits; jhit++) {
      int hitid2 = hits[jhit];
	
      FairHit *hit2 = (FairHit*) array->At(hitid2);
      if(!hit2) continue;
      TVector3 position2;
      hit2->Position(position2);
	
      double distance = (position - position2).Perp();
      //	cout << ihit << " " <<  hitid  << "/" << hits[tmphit] << " distance " << distance << " " ;
	
      // to have 1 cm for radius = 15 cm, 3 cm for radius = 42 cm
      //      double fLimit = 2.7; // 0.075 * position.Perp() - 0125.; CHECK
      if(distance < fLimit) { // && position.Perp() != position.Perp()) {
	// 	  cout << " YES" << endl;
	mapdistances.insert(std::pair<int, int>(hitid, hitid2));
      }
      // 	else   cout << "NO" << endl;
    }
  }

  //   // read the map
  //   std::multimap<int, int>::iterator it = mapdistances.begin();
  //   int tmpfirst = (*it).first;
  //   for(it = mapdistances.begin(); it != mapdistances.end(); ++it)
  //     {
  //       std::vector<int> cluster;
  //       if((*it).first == tmpfirst) {
  //       int tmpfirst = (*it).first;
  //       cluster.push_back((*it).second);
  //       }
  
  //       for(int ifs = 0; ifs < mapdistances.size(); ifs++) {
  // 	std::vector<int> cluster;
  // 	if(ifs == tmpfirst) {
  // 	  cluster.push_back(mapdistances[ifs]);

  //       }


  //  std::vector<int> list;
  return list;  
}


Bool_t PndSecondaryTrackFinder::ConformalPlaneStt4(std::vector<int> cluster, Int_t iclus, std::vector<std::vector<double> > &conformalhits, Double_t &firstdrift, Double_t &delta, Double_t trasl[2]) {
  
  int nhits = cluster.size();
  TClonesArray *array = fSttHitArray;
  
  double tmpdrift = 0.5; // CHECK
  int tmphitid = -1;
  for(int ihit = 0; ihit < nhits; ihit++)
    {
      Int_t hitid = cluster[ihit];
      
      PndSttHit *hit = (PndSttHit*) array->At(hitid);
      if(!hit) continue;
      
      Double_t rd = hit->GetIsochrone();
      if(rd < tmpdrift) {
	tmpdrift = rd;
	tmphitid = hitid;
      }
    }

  //  cout << "tmp drift " << tmpdrift << endl;
  firstdrift = tmpdrift;
  int lasthitid; //  = cluster[0];
  int firsthitid = tmphitid; // cluster[nhits - 1];
  if(tmphitid == cluster[0]) lasthitid = cluster[nhits - 1];
  else lasthitid = cluster[0];
  //  cout << "first/lasthitid " << firsthitid << " " << lasthitid << endl;
  FairHit *hitfirst = (FairHit*) array->At(firsthitid);
  if(!hitfirst) return kFALSE;
  TVector3 positionfirst;
  hitfirst->Position(positionfirst);

  FairHit *hitlast = (FairHit*) array->At(lasthitid);
  if(!hitlast) return kFALSE;
  TVector3 positionlast;
  hitlast->Position(positionlast);
  
 
  trasl[0] = positionfirst.X();
  trasl[1] = positionfirst.Y();
  delta = TMath::ATan2(positionlast.Y() - positionfirst.Y(),
		       positionlast.X() - positionfirst.X());

  //   cout << "translation/rotation " << trasl[0] << " " << trasl[1] << " " << delta << endl;
  if(fDisplayOn) {
    char goOnChar;
    //   cout << "Go to conformal plane: cluster " << iclus << endl;
    cin >> goOnChar;
    cout << "GOING ON" << endl;
    DrawGeometryConformal(-1, -1, 1, 1); 
  }
  
  TVector3 centerposition;
  for(int ihit = 0; ihit < cluster.size(); ihit++)
    {
      Int_t hitid = cluster[ihit];
      //   if(hitid == firsthitid) continue;

      PndSttHit *hit = (PndSttHit*) array->At(hitid);
      if(!hit) continue;
     
      hit->Position(centerposition);
      Double_t rd = hit->GetIsochrone();
 
      Double_t sigx = 0.50; // CHECK
      Double_t sigy = 0.50;
     
      // to the fit ================================
      Double_t xtrasl, ytrasl;
      // traslation
      xtrasl = centerposition.X() - trasl[0];
      ytrasl = centerposition.Y() - trasl[1];
     

      //      centerposition.Print();

      // change coordinate of the center
      Double_t u, v, sigv2, sigu2, rc;
      u = xtrasl / (xtrasl*xtrasl + ytrasl*ytrasl - rd * rd);
      v = ytrasl / (xtrasl*xtrasl + ytrasl*ytrasl - rd * rd);
      rc = rd / (xtrasl*xtrasl + ytrasl*ytrasl - rd * rd);

      std::vector<double> params;
      if(hitid != firsthitid) { 
	// cout << hitid << " u/v/rc " << u << " " << v << " " << rc << endl;
	params.push_back(u);
    	params.push_back(v);
    	params.push_back(rc);
	conformalhits.push_back(params);
      }

      if(fDisplayOn) {
            
 	TArc *arc = new TArc(u, v, rc);
	arc->SetLineColor(kRed);
 	arc->SetFillStyle(0);
	arc->Draw("SAME");
 	display->Update();
 	display->Modified();  
      }
    } 
  return kTRUE;

 
}



Bool_t PndSecondaryTrackFinder::ConformalPlaneStt4BIS(std::vector< TMatrixT<double> > cluster, Int_t iclus, std::vector<std::vector<double> > &conformalhits, Double_t &firstdrift, Double_t &delta, Double_t trasl[2]) {
  
  cout << "GOING TO FIT CLUSTER " << iclus << endl;
 if(fDisplayOn) {
   char goOnChar;
   cout << "light up the cluster, press any key" << endl;
   cin >> goOnChar;
 }

  int nhits = cluster.size();
  TClonesArray *array = fSttHitArray;
  
  double tmpdrift = 0.5; // CHECK
  int tmphitid = -1;
  for(int ihit = 0; ihit < nhits; ihit++)
    {
      TMatrixT<double> singlehit = cluster[ihit];
      if(singlehit[0][0] == -1) continue;
      Int_t hitid = (Int_t) singlehit[0][1];
      Int_t detid = (Int_t) singlehit[0][2];
      if(detid != FairRootManager::Instance()->GetBranchId(fSttBranch)) continue; // CHECK THIS ONE!
      PndSttHit *hit = (PndSttHit*) array->At(hitid);
      if(!hit) continue;
      
      Double_t rd = hit->GetIsochrone();
      if(rd < tmpdrift) {
	tmpdrift = rd;
	tmphitid = hitid;
      }

      if(fDisplayOn) {
	TArc *arc = new TArc(singlehit[0][4], singlehit[0][5], 0.5);
	arc->SetFillColor(kYellow);
	//	arc->SetFillStyle(0);
	arc->Draw("SAME");
	display->Update();
	display->Modified();
      }

    }

  //  cout << "tmp drift " << tmpdrift << endl;
  firstdrift = tmpdrift;
  int lasthitid; //  = cluster[0];
  int firsthitid = tmphitid; // cluster[nhits - 1];
  TMatrixT<double> singlehit0 = cluster[0];
  TMatrixT<double> singlehit1 = cluster[nhits - 1];
  if(tmphitid == singlehit0[0][1]) lasthitid =  (Int_t) singlehit1[0][1];
  else lasthitid =  (Int_t) singlehit0[0][1];
  //  cout << "first/lasthitid " << firsthitid << " " << lasthitid << endl;
  FairHit *hitfirst = (FairHit*) array->At(firsthitid);
  if(!hitfirst) return kFALSE;
  TVector3 positionfirst;
  hitfirst->Position(positionfirst);

  FairHit *hitlast = (FairHit*) array->At(lasthitid);
  if(!hitlast) return kFALSE;
  TVector3 positionlast;
  hitlast->Position(positionlast);
  
 
  trasl[0] = positionfirst.X();
  trasl[1] = positionfirst.Y();
  delta = TMath::ATan2(positionlast.Y() - positionfirst.Y(),
		       positionlast.X() - positionfirst.X());

  //   cout << "translation/rotation " << trasl[0] << " " << trasl[1] << " " << delta << endl;
  if(fDisplayOn) {
     char goOnChar;
   cout << "Go to conformal plane: press any key" << endl;
   cin >> goOnChar;
   //   cout << "GOING ON" << endl;
    DrawGeometryConformal(-1, -1, 1, 1); 
  }
  
  TVector3 centerposition;
  for(int ihit = 0; ihit < cluster.size(); ihit++)
    {
      
      TMatrixT<double> singlehit = cluster[ihit];
      if(singlehit[0][0] == -1) continue;
      Int_t hitid =  (Int_t) singlehit[0][1];
      Int_t detid =  (Int_t) singlehit[0][2];
      if(detid !=  FairRootManager::Instance()->GetBranchId(fSttBranch)) continue; // CHECK THIS ONE!
      
      PndSttHit *hit = (PndSttHit*) array->At(hitid);
      if(!hit) continue;
     
      hit->Position(centerposition);
      Double_t rd = hit->GetIsochrone();
 
      Double_t sigx = 0.50; // CHECK
      Double_t sigy = 0.50;
     
      // to the fit ================================
      Double_t xtrasl, ytrasl;
      // traslation
      xtrasl = centerposition.X() - trasl[0];
      ytrasl = centerposition.Y() - trasl[1];
     

      //      centerposition.Print();

      // change coordinate of the center
      Double_t u, v, sigv2, sigu2, rc;
      u = xtrasl / (xtrasl*xtrasl + ytrasl*ytrasl - rd * rd);
      v = ytrasl / (xtrasl*xtrasl + ytrasl*ytrasl - rd * rd);
      rc = rd / (xtrasl*xtrasl + ytrasl*ytrasl - rd * rd);

      std::vector<double> params;
      if(hitid != firsthitid) { 
	// cout << hitid << " u/v/rc " << u << " " << v << " " << rc << endl;
	params.push_back(u);
    	params.push_back(v);
    	params.push_back(rc);
	conformalhits.push_back(params);
      }

      if(fDisplayOn) {
            
 	TArc *arc = new TArc(u, v, rc);
	arc->SetLineColor(kRed);
 	arc->SetFillStyle(0);
	arc->Draw("SAME");
 	display->Update();
 	display->Modified();  
      }
    } 
  return kTRUE;

 
}

Bool_t PndSecondaryTrackFinder::ConformalFit(std::vector<std::vector<double> > conformalhits, Int_t iclus, Double_t delta, Double_t trasl[2], Double_t &xc, Double_t &yc, Double_t &radius)
{
  int nhits = conformalhits.size();
  // CHECK change this!
  Double_t auxinfoparalConformal[nhits][3];
  for(int ihit = 0; ihit < nhits; ihit++) {
    std::vector<double> parameters = conformalhits[ihit];

    //     cout << "parameters " << parameters[0] << " " << parameters[1] << " " << parameters[2] << endl;

    auxinfoparalConformal[ihit][0] = parameters[0];
    auxinfoparalConformal[ihit][1] = parameters[1];
    auxinfoparalConformal[ihit][2] = parameters[2];
  }

  // ---------------------
  Double_t m, p, alpha, beta, gamma;
  Bool_t typeConf;  
  Short_t fitting = FitHelixCylinder(nhits,
				     auxinfoparalConformal,
				     delta, trasl,
				     m, p,
				     alpha, beta, gamma,
				     typeConf);
 
//   cout << "CONFORMAL FITTING " << fitting << " " << typeConf << endl;
//   cout << "ROT " << delta << " TRASL " << trasl[0] << " " << trasl[1] << endl;
//   cout << "M/P " << m << " " << p << endl;
//   cout  << "ALPHA/BETA/GAMMA " << alpha << " " << beta << " " << gamma << endl;
  if(fitting != 1 || typeConf != true) return kFALSE;

  if(fDisplayOn) { 
    // double mnew = (m * TMath::Cos(delta) + TMath::Sin(delta))/(TMath::Cos(delta) - m * TMath::Sin(delta));
    // double pnew = p / (TMath::Cos(delta) - m * TMath::Sin(delta));
    TLine *line = new TLine(-1, -m + p, 1, m + p);
    line->SetLineColor(3);
    line->Draw("SAME");
    display->Update();
    display->Modified();  
    
 //    TString fConName = fDisName;
//     fConName += "_clus"; 
//     fConName += iclus;
//     fConName += ".pdf";
    //    display->SaveAs(fConName);
  }

  xc = -alpha / 2.;
  yc = -beta / 2.;
  radius = TMath::Sqrt(- gamma + xc * xc + yc * yc);

  if(fDisplayOn) {
    char goOnChar;
    cout << "Go back to real plane: cluster " << iclus << endl;
    cin >> goOnChar; 
    if(fDisplayOn) Refresh();
    cout << "helix " << xc << " " << yc << " " << radius << endl;     
    TArc *arc = new TArc(xc, yc, radius);
    arc->SetLineColor(kGreen);
    arc->SetFillStyle(0);
    arc->Draw("SAME ONLY");
    display->Update();
    display->Modified();
  }

  return kTRUE;

}


Double_t PndSecondaryTrackFinder::CalculateRedChi2(std::vector<int> cluster, Int_t detId,  Double_t xc, Double_t yc, Double_t radius, Int_t &countelement) 
{
  TClonesArray *array;
  

  if(detId == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) array = fMvdPixelHitArray;
  else if(detId == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) array = fMvdStripHitArray;
  else if(detId ==  FairRootManager::Instance()->GetBranchId(fSttBranch)) array = fSttHitArray;
  //  else if(detId ==  FairRootManager::Instance()->GetBranchId(fGemBranch)) array = fGemHitArray;

  // chi2
  double chi2 = 0;

  for(int ihit = 0; ihit < cluster.size(); ihit++)
    {
      Int_t hitid = cluster[ihit];
      PndSttHit *hit = (PndSttHit*) array->At(hitid);
      if(!hit) continue;
      TVector3 centerposition2;
      hit->Position(centerposition2);
      Double_t rd = hit->GetIsochrone();
      Double_t rderror = hit->GetIsochroneError();
      Double_t distancepc = TMath::Sqrt((centerposition2.X() - xc) * (centerposition2.X() - xc) +
					(centerposition2.Y() - yc) * (centerposition2.Y() - yc));
      double element = pow((fabs(distancepc - radius) - rd)/(rderror), 2);
      if(element >= fCountElemLimit) countelement++;
      chi2 += element;
      //   cout << "element " <<  element  << endl;
    }
  double redchi2 = chi2 / cluster.size();
  // cout << "===> RED CHI2 no. 0 = " << redchi2 << " <===" << endl;
  return redchi2;
}

Double_t PndSecondaryTrackFinder::CalculateRedChi2BIS(std::vector< TMatrixT<double> > cluster, Double_t xc, Double_t yc, Double_t radius, Int_t &countelement) 
{
  TClonesArray *array;
  

  //   if(detId == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) array = fMvdPixelHitArray;
  //   else if(detId == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) array = fMvdStripHitArray;
  //   else if(detId ==  FairRootManager::Instance()->GetBranchId(fSttBranch)) array = fSttHitArray;
  //   //  else if(detId ==  FairRootManager::Instance()->GetBranchId(fGemBranch)) array = fGemHitArray;

  // chi2
  double chi2 = 0;

  for(int ihit = 0; ihit < cluster.size(); ihit++)
    {
      TMatrixT<double> singlehit = cluster[ihit];
      if(singlehit[0][0] == -1) continue;
      Int_t hitid =  (Int_t) singlehit[0][1];
      Int_t detid =  (Int_t) singlehit[0][2];
      if(detid != FairRootManager::Instance()->GetBranchId(fSttBranch)) continue; // CHECK THIS ONE
      array = fSttHitArray;

      PndSttHit *hit = (PndSttHit*) array->At(hitid);
      if(!hit) continue;
      TVector3 centerposition2;
      hit->Position(centerposition2);
      Double_t rd = hit->GetIsochrone();
      Double_t rderror = hit->GetIsochroneError();
      Double_t distancepc = TMath::Sqrt((centerposition2.X() - xc) * (centerposition2.X() - xc) +
					(centerposition2.Y() - yc) * (centerposition2.Y() - yc));
      double element = pow((fabs(distancepc - radius) - rd)/(rderror), 2);
      if(element >= fCountElemLimit) countelement++;
      chi2 += element;
      //   cout << "element " <<  element  << endl;
    }
  double redchi2 = chi2 / cluster.size();
  // cout << "===> RED CHI2 no. 0 = " << redchi2 << " <===" << endl;
  return redchi2;
}


Bool_t PndSecondaryTrackFinder::RefitConformalBIS(std::vector< TMatrixT<double> > cluster, Double_t xc, Double_t yc, Double_t radius,
						  Double_t &outxc, Double_t &outyc, Double_t &outradius)
  
{
  TClonesArray *array;
 
  //   if(detId == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) array = fMvdPixelHitArray;
  //   else if(detId == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) array = fMvdStripHitArray;
  //   else if(detId ==  FairRootManager::Instance()->GetBranchId(fSttBranch)) array = fSttHitArray;
  //  else if(detId ==  FairRootManager::Instance()->GetBranchId(fGemBranch)) array = fGemHitArray;

  // intersection finder & fit
  TMatrixT<double> points(cluster.size(), 11);
  for(int ihit = 0; ihit < cluster.size(); ihit++)
    {
      TMatrixT<double> singlehit = cluster[ihit];
      if(singlehit[0][0] == -1) continue;
      Int_t hitid = (Int_t) singlehit[0][1];
      Int_t detid = (Int_t) singlehit[0][2];
      if(detid != FairRootManager::Instance()->GetBranchId(fSttBranch)) continue; // CHECK THIS ONE
      array = fSttHitArray;
      PndSttHit *hit = (PndSttHit*) array->At(hitid);
      if(!hit) continue;
      Double_t rd = hit->GetIsochrone();
      TVector3 xyz, dxyz;
      Bool_t inters = IntersectionFinder(xc, yc, radius, hit, xyz, dxyz);
      if(inters == kFALSE) continue;
      if(fDisplayOn) {
	TMarker *mrk = new TMarker(xyz.X(), xyz.Y(), 6);  
	mrk->SetMarkerColor(4);
	mrk->Draw("SAME");
	display->Update();
	display->Modified();
      }
    
	  
      points[ihit][0] = hitid;
      points[ihit][2] = xyz.X();
      points[ihit][3] = xyz.Y();
      points[ihit][4] = xyz.Z();
      points[ihit][5] = dxyz.X();
      points[ihit][6] = dxyz.Y();
      points[ihit][7] = dxyz.Z();
      points[ihit][8] = hit->GetIsochrone();
      points[ihit][9] = hit->GetIsochroneError();
      points[ihit][10] = 0;
    }
      
  // xy fit
  Bool_t fitting2 = Fit(points, outxc, outyc, outradius);
  if(fitting2 == kFALSE) return kFALSE;
  return kTRUE;
}



Bool_t PndSecondaryTrackFinder::RefitConformalBIS(std::vector< TMatrixT<double> > *cluster, Double_t xc, Double_t yc, Double_t radius,
						  Double_t &outxc, Double_t &outyc, Double_t &outradius)
  
{
  TClonesArray *array;
 
  //   if(detId == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) array = fMvdPixelHitArray;
  //   else if(detId == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) array = fMvdStripHitArray;
  //   else if(detId ==  FairRootManager::Instance()->GetBranchId(fSttBranch)) array = fSttHitArray;
  //  else if(detId ==  FairRootManager::Instance()->GetBranchId(fGemBranch)) array = fGemHitArray;

  // intersection finder & fit
  TMatrixT<double> points(cluster->size(), 11);
  for(int ihit = 0; ihit < cluster->size(); ihit++)
    {
      TMatrixT<double> singlehit = cluster->at(ihit);
      TMatrixT<double> newsinglehit = singlehit;
      if(singlehit[0][0] == -1) continue;
      Int_t hitid = (Int_t) singlehit[0][1];
      Int_t detid = (Int_t) singlehit[0][2];
      if(detid != FairRootManager::Instance()->GetBranchId(fSttBranch)) continue; // CHECK THIS ONE
      array = fSttHitArray;
      PndSttHit *hit = (PndSttHit*) array->At(hitid);
      if(!hit) continue;
      Double_t rd = hit->GetIsochrone();
      TVector3 xyz, dxyz;
      Bool_t inters = IntersectionFinder(xc, yc, radius, hit, xyz, dxyz);
      if(inters == kFALSE) {
	newsinglehit[0][0] = -1; // CHECK useit flag
	std::replace(cluster->begin(), cluster->end(), singlehit, newsinglehit);
	continue;
      }
     
      if(fDisplayOn) {
	TMarker *mrk = new TMarker(xyz.X(), xyz.Y(), 6);  
	mrk->SetMarkerColor(4);
	mrk->Draw("SAME");
	display->Update();
	display->Modified();
      }

      // update intersections .....
      newsinglehit[0][4] = xyz.X(); 
      newsinglehit[0][5] = xyz.Y();
      newsinglehit[0][6] = xyz.Z();
      newsinglehit[0][7] = dxyz.X(); 
      newsinglehit[0][8] = dxyz.Y(); 
      newsinglehit[0][9] = dxyz.Z(); 
      std::replace(cluster->begin(), cluster->end(), singlehit, newsinglehit);
      //      cout << "NEW ONE" << endl;
      //       xyz.Print();
      //       dxyz.Print();
      // ..........................


	  
      points[ihit][0] = hitid;
      points[ihit][2] = xyz.X();
      points[ihit][3] = xyz.Y();
      points[ihit][4] = xyz.Z();
      points[ihit][5] = dxyz.X();
      points[ihit][6] = dxyz.Y();
      points[ihit][7] = dxyz.Z();
      points[ihit][8] = hit->GetIsochrone();
      points[ihit][9] = hit->GetIsochroneError();
      points[ihit][10] = 0;
    }
      
  // xy fit
  Bool_t fitting2 = Fit(points, outxc, outyc, outradius);
  if(fitting2 == kFALSE) return kFALSE;
  return kTRUE;
}




Bool_t PndSecondaryTrackFinder::AddRemainingPoints(std::vector<int> hits, Int_t detId,  Double_t xc, Double_t yc, Double_t radius, std::vector<int> *cluster, Int_t iclus)
{
  TClonesArray *array;
 
  if(detId == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) array = fMvdPixelHitArray;
  else if(detId == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) array = fMvdStripHitArray;
  else if(detId ==  FairRootManager::Instance()->GetBranchId(fSttBranch)) array = fSttHitArray;
  //  else if(detId ==  FairRootManager::Instance()->GetBranchId(fGemBranch)) array = fGemHitArray;



  // add points
  for(int ihit = 0; ihit <  hits.size(); ihit++)
    {
      Int_t hitid = hits[ihit];
      std::vector<int>::iterator it;
      it = find(cluster->begin(), cluster->end(), hitid);
      if(it != cluster->end()) continue;
      PndSttHit *hit = (PndSttHit*) array->At(hitid);
      if(!hit) continue;
      TVector3 centerposition2;
      hit->Position(centerposition2);
      Double_t rd = hit->GetIsochrone();
      Double_t rderror = hit->GetIsochroneError();
      Double_t distancepc = TMath::Sqrt((centerposition2.X() - xc) * (centerposition2.X() - xc) +
					(centerposition2.Y() - yc) * (centerposition2.Y() - yc));
      
      double res = fabs(distancepc - radius) - rd;
      //  cout << "RES " << res << " " << " limit " << 3 * fLimit << endl;
      if(res < (3 * fLimit)) { // 3 * fLimt CHECK
	// 	cout << "ADD " << hitid << " TO CLUS " << iclus << endl;
	cluster->push_back(hitid);


	if(fDisplayOn) {
	  TArc *arc3 = new TArc(centerposition2.X(), centerposition2.Y(), 0.5); // CHECK
	  arc3->SetLineColor(fColors[iclus]);
	  arc3->SetFillStyle(0);
	  arc3->Draw("SAME ONLY");
	  display->Update();
	  display->Modified();
	}
      }
    }

  return kTRUE;
}




// ============================================================================
// needs:
// nHitsinTrack = #hits nel cluster - 1st one
// auxinfoparalConformal all hits - 1st one:
// translate x, y -> compute u, v, rc (no rotation)
// trajectory_vertex & rotation are in the REAL plane 
Short_t PndSecondaryTrackFinder::FitHelixCylinder( UShort_t nHitsinTrack,
						   Double_t auxinfoparalConformal[][3],
						   Double_t rotationangle,
						   Double_t trajectory_vertex[2],
						   Double_t &slope,
						   Double_t &intercept,
						   Double_t &alpha,
						   Double_t &beta,
						   Double_t &gamma,
						   Bool_t &TypeConf
						   )
{

  int maximumTracks = 40; // CHECK
  int nmaxHits = maximumTracks * 30; // max hits total // CHECK
  int MINIMUMHITSPERTRACK = 3; // CHECK
  int NHITSINFIT = 15; // CHECK
  Double_t StrawRadius = 0.5; // CHECK

  //   definition of variables for the glpsol  solver
  //    ROWS (for read_rows  function)
  //
  UShort_t  NpointsInFit = nHitsinTrack-NHITSINFIT <0 ?  nHitsinTrack :  NHITSINFIT;
  int    nRows= NpointsInFit*9 +1;
  int typeRows[nRows];
  char * nameRows[nRows];
  char  auxnameRows[nRows][20];
  //-------  end ROWS information
  //--------begin COLUMNS information
  int  NStructVar=5+NpointsInFit*4;  //  number of  structural variables
  int  NStructRows = 8*NpointsInFit ;  //  maximum number of ROWS in which a structural variable can be found
  double final_values[NStructVar];
  int  NRowsInWhichStructVarArePresent[NStructVar];
  char *StructVarName[NStructVar];
  char auxStructVarName[NStructVar][20];
  //      char *AuxNameRowsInWhichStructVarArePresent[NStructVar][NStructRows];
  char *NameRowsInWhichStructVarArePresent[NStructVar*NStructRows];
  char aux[NStructVar*NStructRows][20];
  //      double Coefficients[NStructVar][NStructRows];
  double Coefficients[NStructVar*NStructRows];
  //--------end COLUMNS information
  //--------begin RHS information
  double ValueB[9*NpointsInFit];
  //--------end RHS information
  //--------begin RANGES information
  int nRanges = NpointsInFit;
  double ValueRanges[nRanges];
  char *NameRanges[nRanges];
  char auxNameRanges[nRanges][20];
  //--------end RANGES information
  //--------start BOUNDS information
  int nBounds=2*NpointsInFit+1;
  double BoundValue[nBounds];
  char *BoundStructVarName[nBounds];
  char auxBoundStructVarName[nBounds][20];
  char *TypeofBound[nBounds];
  char auxTypeofBound[nBounds][20];
  //--------end BOUNDS information




  Double_t M = 1.,
    m_result,
    q_result,
    A,
    alfetta,
    angle,
    offsety,
    Delta[nmaxHits],
    Ox[nmaxHits],
    Oy[nmaxHits];

  UShort_t  i, ii;
  Short_t Status;

  char nome[300], stringa[300], stringa2[300];

  //     FILE * MACRO ;

  float m1_result,m2_result, q1_result,q2_result, A1_result, A2_result;

  // --

  if( nHitsinTrack < MINIMUMHITSPERTRACK) {
    return -1;
  }

  //  use the trick of increasing the rotation angle by 10 degrees in order to obtain always a positive m
  rotationangle -= TMath::Pi()/18.;

  Double_t cose = cos(rotationangle), sine = sin(rotationangle);
  for(i=0;i<nHitsinTrack; i++){
    //       if( i== iExclude)  continue;
    Ox[i] = auxinfoparalConformal[ i ][0] *cose +
      auxinfoparalConformal[ i ][1]*sine;
    Oy[i] = -auxinfoparalConformal[ i ][0] *sine +
      auxinfoparalConformal[ i ][1]*cose;
    //  cout << "CONF FIT " << Ox[i] << " " << Oy[i] << " " << auxinfoparalConformal[ i ][2] << endl;

    //     if(fDisplayOn) {
    //       TArc *arc = new TArc(Ox[i], Oy[i], auxinfoparalConformal[i][2]);
    //       arc->SetLineColor(kBlack);
    //       arc->SetFillStyle(0);
    //       arc->Draw("SAME");
    //       display->Update();
    //       display->Modified();  
    //     }
    //         Delta[i] = auxinfoparalConformal[ i ][4];
         
    if( auxinfoparalConformal[ i ][2] > 1.e-10) {
      Delta[i] = 3.*auxinfoparalConformal[ i ][2];   //   3 times the Drift Radius
    } else {
      Delta[i] = 3.*StrawRadius;
    }
  }

 
  //--------
  //      nameRows[0]="OBJECT";
  sprintf(&(auxnameRows[0][0]),"OBJECT",i);  nameRows[0]=&auxnameRows[0][0];
  typeRows[0]=GLP_FR;
  for(i=0 ; i< NpointsInFit ; i++) {
    ii=9*i;

    typeRows[1+ii]=GLP_UP;typeRows[2+ii]=GLP_UP;typeRows[3+ii]=GLP_UP;typeRows[4+ii]=GLP_UP;
    typeRows[5+ii]=GLP_UP;typeRows[6+ii]=GLP_UP;typeRows[7+ii]=GLP_UP;typeRows[8+ii]=GLP_UP;
    typeRows[9+ii]=GLP_LO;

    sprintf(&(auxnameRows[1+ii][0]),"Ap%d",i);  nameRows[1+ii]=&auxnameRows[1+ii][0];
    sprintf(&(auxnameRows[2+ii][0]),"Bp%d",i);  nameRows[2+ii]=&auxnameRows[2+ii][0];
    sprintf(&(auxnameRows[3+ii][0]),"Cp%d",i);  nameRows[3+ii]=&auxnameRows[3+ii][0];
    sprintf(&(auxnameRows[4+ii][0]),"Dp%d",i);  nameRows[4+ii]=&auxnameRows[4+ii][0];
    sprintf(&(auxnameRows[5+ii][0]),"Am%d",i);  nameRows[5+ii]=&auxnameRows[5+ii][0];
    sprintf(&(auxnameRows[6+ii][0]),"Bm%d",i);  nameRows[6+ii]=&auxnameRows[6+ii][0];
    sprintf(&(auxnameRows[7+ii][0]),"Cm%d",i);  nameRows[7+ii]=&auxnameRows[7+ii][0];
    sprintf(&(auxnameRows[8+ii][0]),"Dm%d",i);  nameRows[8+ii]=&auxnameRows[8+ii][0];
    sprintf(&(auxnameRows[9+ii][0]),"LAMBDA%d",i);  nameRows[9+ii]=&auxnameRows[9+ii][0];
  }




  //-----------------  write the COLUMNS  section

  //      fprintf(MACRO,"COLUMNS\n");

  //  Column variable  m1


  for(i=0, ii=0 ; i< NpointsInFit ; i++) {
    ii++;
    //          fprintf(MACRO,"  m1 Ap%d  %g  Am%d  %g\n  m1 Bp%d  %g   Bm%d  %g\n",
    //                                  i,Ox[i],i,Ox[i],i,-Ox[i],i,-Ox[i]);
    Coefficients[i*4]=    Ox[i];
    Coefficients[i*4+1]=  Ox[i];
    Coefficients[i*4+2]= -Ox[i];
    Coefficients[i*4+3]= -Ox[i];
  }



  //  Column variable  m2
  for(i=0; i< NpointsInFit ; i++) {
    //          fprintf(MACRO,"  m2 Ap%d  %g  Am%d  %g\n  m2 Bp%d  %g   Bm%d  %g\n",
    //                                  i,-Ox[i],i,-Ox[i],i,Ox[i],i,Ox[i]);
    Coefficients[NStructRows+i*4]=   -Ox[i];
    Coefficients[NStructRows+i*4+1]= -Ox[i];
    Coefficients[NStructRows+i*4+2]= Ox[i];
    Coefficients[NStructRows+i*4+3]= Ox[i];

  }

  //  Column variable  q1
  for(i=0 ; i< NpointsInFit ; i++) {
    //          fprintf(MACRO,"  q1 Ap%d   1.  Am%d   1.\n  q1 Bp%d  -1.  Bm%d  -1.\n",
    //                                  i,i,i,i);
    Coefficients[2*NStructRows+i*4]=    1.;
    Coefficients[2*NStructRows+i*4+1]=  1.;
    Coefficients[2*NStructRows+i*4+2]= -1.;
    Coefficients[2*NStructRows+i*4+3]= -1.;
  }

  //  Column variable  q2
  for(i=0 ; i< NpointsInFit ; i++) {
    //          fprintf(MACRO,"  q2 Ap%d   -1.  Am%d   -1.\n  q2 Bp%d   1.   Bm%d   1.\n",
    //                                  i,i,i,i);
    Coefficients[3*NStructRows+i*4]=   -1.;
    Coefficients[3*NStructRows+i*4+1]= -1.;
    Coefficients[3*NStructRows+i*4+2]=  1.;
    Coefficients[3*NStructRows+i*4+3]=  1.;
  }

  //  Column variable  lambdap(i)
  for(i=0 ; i< NpointsInFit ; i++) {
    //          fprintf(MACRO,"  lamp%d  Ap%d  %g  Bp%d  %g\n  lamp%d  Cp%d  %g  Dp%d   %g\n  lamp%d  LAMBDA%d  1.\n",
    //                                                i,i,-M,i,-M, i , i,-M, i, M, i,i);
    Coefficients[(4+i)*NStructRows+0]= -M;
    Coefficients[(4+i)*NStructRows+1]= -M;
    Coefficients[(4+i)*NStructRows+2]= -M;
    Coefficients[(4+i)*NStructRows+3]=  M;
    Coefficients[(4+i)*NStructRows+4]=  1.;
  }
  //  Column variable  lambdam(i)
  for(i=0 ; i< NpointsInFit ; i++) {
    //          fprintf(MACRO,"  lamm%d  Am%d  %g  Bm%d  %g\n  lamm%d  Cm%d  %g  Dm%d %g\n  lamm%d  LAMBDA%d  1.\n",
    //                                                i,i,-M,i,-M, i,i , -M, i, M, i,i);
    Coefficients[(4+i+NpointsInFit)*NStructRows+0]= -M;
    Coefficients[(4+i+NpointsInFit)*NStructRows+1]= -M;
    Coefficients[(4+i+NpointsInFit)*NStructRows+2]= -M;
    Coefficients[(4+i+NpointsInFit)*NStructRows+3]=  M;
    Coefficients[(4+i+NpointsInFit)*NStructRows+4]=  1.;
  }
  //  Column variable  sigmap(i)
  for(i=0; i< NpointsInFit ; i++) {

    //          fprintf(MACRO,"  sigmap%d  OBJECT  %g  Ap%d  -1.\n  sigmap%d  Bp%d    -1. Cp%d  1.\n  sigmap%d  Dp%d -1.\n",
    //                                                i,1./Delta[i],i,i,i,i,i,i);
    Coefficients[(4+i+2*NpointsInFit)*NStructRows+0]=  1./Delta[i];
    Coefficients[(4+i+2*NpointsInFit)*NStructRows+1]= -1.;
    Coefficients[(4+i+2*NpointsInFit)*NStructRows+2]= -1.;
    Coefficients[(4+i+2*NpointsInFit)*NStructRows+3]=  1.;
    Coefficients[(4+i+2*NpointsInFit)*NStructRows+4]= -1.;
  }
  //  Column variable  sigmam(i)
  for(i=0 ; i< NpointsInFit ; i++) {
    //          fprintf(MACRO,"  sigmam%d  OBJECT %g  Am%d  -1.\n  sigmam%d  Bm%d   -1. Cm%d   1.\n  sigmam%d  Dm%d  -1.\n",
    //                                                i,1./Delta[i],i,i,i,i,i,i);
    Coefficients[(4+i+3*NpointsInFit)*NStructRows+0]=  1./Delta[i];
    Coefficients[(4+i+3*NpointsInFit)*NStructRows+1]= -1.;
    Coefficients[(4+i+3*NpointsInFit)*NStructRows+2]= -1.;
    Coefficients[(4+i+3*NpointsInFit)*NStructRows+3]=  1.;
    Coefficients[(4+i+3*NpointsInFit)*NStructRows+4]= -1.;
  }


  for(i=0 ; i< NStructRows ; i++) {
    Coefficients[(4+4*NpointsInFit)*NStructRows+i]= 1.;
  }
  //--------------------
  sprintf(&auxStructVarName[0][0],"m1",i);
  StructVarName[0] = &auxStructVarName[0][0];
  //      StructVarName[0]="m1";
  NRowsInWhichStructVarArePresent[0]= 4*NpointsInFit;

  sprintf(&auxStructVarName[1][0],"m2",i);
  StructVarName[1] = &auxStructVarName[1][0];
  //      StructVarName[1]="m2";
  NRowsInWhichStructVarArePresent[1]= 4*NpointsInFit;

  sprintf(&auxStructVarName[2][0],"q1",i);
  StructVarName[2] = &auxStructVarName[2][0];
  //      StructVarName[2]="q1";
  NRowsInWhichStructVarArePresent[2]= 4*NpointsInFit;

  sprintf(&auxStructVarName[3][0],"q2",i);
  StructVarName[3] = &auxStructVarName[3][0];
  //      StructVarName[3]="q2";
  NRowsInWhichStructVarArePresent[3]= 4*NpointsInFit;
  for(i=0; i< NpointsInFit ; i++) {
    sprintf(&auxStructVarName[3+i+1][0],"lamp%d",i);
    StructVarName[4+i] = &auxStructVarName[4+i][0];
    NRowsInWhichStructVarArePresent[4+i]= 5;

    sprintf(&auxStructVarName[4+NpointsInFit+i][0],"lamm%d",i);
    StructVarName[4+NpointsInFit+i] = &auxStructVarName[4+NpointsInFit+i][0];
    NRowsInWhichStructVarArePresent[4+NpointsInFit+i]= 5;

    sprintf(&auxStructVarName[4+2*NpointsInFit+i][0],"sigmap%d",i);
    StructVarName[4+2*NpointsInFit+i] = &auxStructVarName[4+2*NpointsInFit+i][0];
    NRowsInWhichStructVarArePresent[4+2*NpointsInFit+i]= 5;

    sprintf(&auxStructVarName[4+3*NpointsInFit+i][0],"sigmam%d",i);
    StructVarName[4+3*NpointsInFit+i] = &auxStructVarName[4+3*NpointsInFit+i][0];
    NRowsInWhichStructVarArePresent[4+3*NpointsInFit+i]= 5;

  }


  sprintf(&auxStructVarName[4+4*NpointsInFit][0],"DUMMY",i);
  StructVarName[4+4*NpointsInFit] = &auxStructVarName[4+4*NpointsInFit][0];
  //      StructVarName[4+4*NpointsInFit]="DUMMY";
  NRowsInWhichStructVarArePresent[4+4*NpointsInFit]= NStructRows;


  //  for m1, m2, q1, q2
  for(i=0; i< 4; i++){
    for(ii=0; ii< NpointsInFit;ii++){
      sprintf(&aux[i*NStructRows+ii*4][0],"Ap%d",ii);
      NameRowsInWhichStructVarArePresent[i*NStructRows+ii*4]=&aux[i*NStructRows+ii*4][0];
      sprintf(&aux[i*NStructRows+ii*4+1][0],"Am%d",ii);
      NameRowsInWhichStructVarArePresent[i*NStructRows+ii*4+1]=&aux[i*NStructRows+ii*4+1][0];
      sprintf(&aux[i*NStructRows+ii*4+2][0],"Bp%d",ii);
      NameRowsInWhichStructVarArePresent[i*NStructRows+ii*4+2]=&aux[i*NStructRows+ii*4+2][0];
      sprintf(&aux[i*NStructRows+ii*4+3][0],"Bm%d",ii);
      NameRowsInWhichStructVarArePresent[i*NStructRows+ii*4+3]=&aux[i*NStructRows+ii*4+3][0];
    }
  }

  //  now for the    lamp*   variables
  for(i=0; i< NpointsInFit;i++){
    sprintf(&aux[(i+4)*NStructRows+0][0],"Ap%d",i);
    NameRowsInWhichStructVarArePresent[(i+4)*NStructRows+0]= &aux[(i+4)*NStructRows+0][0];
    sprintf(&aux[(i+4)*NStructRows+1][0],"Bp%d",i);
    NameRowsInWhichStructVarArePresent[(i+4)*NStructRows+1]= &aux[(i+4)*NStructRows+1][0];
    sprintf(&aux[(i+4)*NStructRows+2][0],"Cp%d",i);
    NameRowsInWhichStructVarArePresent[(i+4)*NStructRows+2]= &aux[(i+4)*NStructRows+2][0];
    sprintf(&aux[(i+4)*NStructRows+3][0],"Dp%d",i);
    NameRowsInWhichStructVarArePresent[(i+4)*NStructRows+3]= &aux[(i+4)*NStructRows+3][0];
    sprintf(&aux[(i+4)*NStructRows+4][0],"LAMBDA%d",i);
    NameRowsInWhichStructVarArePresent[(i+4)*NStructRows+4]= &aux[(i+4)*NStructRows+4][0];
  }

  //  now for the    lamm*   variables
  for(i=0; i< NpointsInFit;i++){
    sprintf(&aux[(i+4+NpointsInFit)*NStructRows+0][0],"Am%d",i);
    NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit)*NStructRows+0]= &aux[(i+4+NpointsInFit)*NStructRows+0][0];
    sprintf(&aux[(i+4+NpointsInFit)*NStructRows+1][0],"Bm%d",i);
    NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit)*NStructRows+1]= &aux[(i+4+NpointsInFit)*NStructRows+1][0];
    sprintf(&aux[(i+4+NpointsInFit)*NStructRows+2][0],"Cm%d",i);
    NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit)*NStructRows+2]= &aux[(i+4+NpointsInFit)*NStructRows+2][0];
    sprintf(&aux[(i+4+NpointsInFit)*NStructRows+3][0],"Dm%d",i);
    NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit)*NStructRows+3]= &aux[(i+4+NpointsInFit)*NStructRows+3][0];
    sprintf(&aux[(i+4+NpointsInFit)*NStructRows+4][0],"LAMBDA%d",i);
    NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit)*NStructRows+4]= &aux[(i+4+NpointsInFit)*NStructRows+4][0];
  }

  //  now for the    sigmap*   variables
  for(i=0; i< NpointsInFit;i++){
    sprintf(&aux[(i+4+2*NpointsInFit)*NStructRows+0][0],"OBJECT",i);
    NameRowsInWhichStructVarArePresent[(i+4+2*NpointsInFit)*NStructRows+0]= &aux[(i+4+2*NpointsInFit)*NStructRows+0][0];
    sprintf(&aux[(i+4+2*NpointsInFit)*NStructRows+1][0],"Ap%d",i);
    NameRowsInWhichStructVarArePresent[(i+4+2*NpointsInFit)*NStructRows+1]= &aux[(i+4+2*NpointsInFit)*NStructRows+1][0];
    sprintf(&aux[(i+4+2*NpointsInFit)*NStructRows+2][0],"Bp%d",i);
    NameRowsInWhichStructVarArePresent[(i+4+2*NpointsInFit)*NStructRows+2]= &aux[(i+4+2*NpointsInFit)*NStructRows+2][0];
    sprintf(&aux[(i+4+2*NpointsInFit)*NStructRows+3][0],"Cp%d",i);
    NameRowsInWhichStructVarArePresent[(i+4+2*NpointsInFit)*NStructRows+3]= &aux[(i+4+2*NpointsInFit)*NStructRows+3][0];
    sprintf(&aux[(i+4+2*NpointsInFit)*NStructRows+4][0],"Dp%d",i);
    NameRowsInWhichStructVarArePresent[(i+4+2*NpointsInFit)*NStructRows+4]= &aux[(i+4+2*NpointsInFit)*NStructRows+4][0];
  }

  //  now for the    sigmam*   variables
  for(i=0; i< NpointsInFit;i++){
    sprintf(&aux[(i+4+3*NpointsInFit)*NStructRows+0][0],"OBJECT",i);
    NameRowsInWhichStructVarArePresent[(i+4+3*NpointsInFit)*NStructRows+0]= &aux[(i+4+3*NpointsInFit)*NStructRows+0][0];
    sprintf(&aux[(i+4+3*NpointsInFit)*NStructRows+1][0],"Am%d",i);
    NameRowsInWhichStructVarArePresent[(i+4+3*NpointsInFit)*NStructRows+1]= &aux[(i+4+3*NpointsInFit)*NStructRows+1][0];
    sprintf(&aux[(i+4+3*NpointsInFit)*NStructRows+2][0],"Bm%d",i);
    NameRowsInWhichStructVarArePresent[(i+4+3*NpointsInFit)*NStructRows+2]= &aux[(i+4+3*NpointsInFit)*NStructRows+2][0];
    sprintf(&aux[(i+4+3*NpointsInFit)*NStructRows+3][0],"Cm%d",i);
    NameRowsInWhichStructVarArePresent[(i+4+3*NpointsInFit)*NStructRows+3]= &aux[(i+4+3*NpointsInFit)*NStructRows+3][0];
    sprintf(&aux[(i+4+3*NpointsInFit)*NStructRows+4][0],"Dm%d",i);
    NameRowsInWhichStructVarArePresent[(i+4+3*NpointsInFit)*NStructRows+4]= &aux[(i+4+3*NpointsInFit)*NStructRows+4][0];
  }

  //  now for the    DUMMY   variable
  for(i=0; i< NpointsInFit;i++){
    sprintf(&aux[(4+4*NpointsInFit)*NStructRows  +8*i][0],"Ap%d",i);
    NameRowsInWhichStructVarArePresent[(4+4*NpointsInFit)*NStructRows+i*8  ]= &aux[(4+4*NpointsInFit)*NStructRows  +8*i][0];

    sprintf(&aux[(4+4*NpointsInFit)*NStructRows+1+8*i][0],"Am%d",i);
    NameRowsInWhichStructVarArePresent[(4+4*NpointsInFit)*NStructRows+1+8*i]= &aux[(4+4*NpointsInFit)*NStructRows+1+8*i][0];
    sprintf(&aux[(4+4*NpointsInFit)*NStructRows+2+8*i][0],"Bp%d",i);
    NameRowsInWhichStructVarArePresent[(4+4*NpointsInFit)*NStructRows+2+8*i]= &aux[(4+4*NpointsInFit)*NStructRows+2+8*i][0];
    sprintf(&aux[(4+4*NpointsInFit)*NStructRows+3+8*i][0],"Bm%d",i);
    NameRowsInWhichStructVarArePresent[(4+4*NpointsInFit)*NStructRows+3+8*i]= &aux[(4+4*NpointsInFit)*NStructRows+3+8*i][0];
    sprintf(&aux[(4+4*NpointsInFit)*NStructRows+4+8*i][0],"Cp%d",i);
    NameRowsInWhichStructVarArePresent[(4+4*NpointsInFit)*NStructRows+4+8*i]= &aux[(4+4*NpointsInFit)*NStructRows+4+8*i][0];
    sprintf(&aux[(4+4*NpointsInFit)*NStructRows+5+8*i][0],"Cm%d",i);
    NameRowsInWhichStructVarArePresent[(4+4*NpointsInFit)*NStructRows+5+8*i]= &aux[(4+4*NpointsInFit)*NStructRows+5+8*i][0];
    sprintf(&aux[(4+4*NpointsInFit)*NStructRows+6+8*i][0],"Dp%d",i);
    NameRowsInWhichStructVarArePresent[(4+4*NpointsInFit)*NStructRows+6+8*i]= &aux[(4+4*NpointsInFit)*NStructRows+6+8*i][0];
    sprintf(&aux[(4+4*NpointsInFit)*NStructRows+7+8*i][0],"Dm%d",i);
    NameRowsInWhichStructVarArePresent[(4+4*NpointsInFit)*NStructRows+7+8*i]= &aux[(4+4*NpointsInFit)*NStructRows+7+8*i][0];
  }



  //-----------------  write the RHS  section

  //      fprintf(MACRO,"RHS\n");
  for(i=0 ; i< NpointsInFit ; i++) {
    //          fprintf(MACRO,"  BOUND  Ap%d  %g  Bp%d  %g\n  BOUND  Cp%d  %g  Dp%d  %g\n",
    //              i, Oy[i]+auxinfoparalConformal[ i ][2]+2.*M,i,
    //                -Oy[i]-auxinfoparalConformal[ i ][2]+2.*M,i,
    //                 Delta[i]+2.*M,i,M-Delta[i]+2.*M);
    ValueB[i*9]  =  Oy[i]+auxinfoparalConformal[ i ][2]+2.*M;
    ValueB[i*9+1]= -Oy[i]-auxinfoparalConformal[ i ][2]+2.*M;
    ValueB[i*9+2]= Delta[i]+2.*M;
    ValueB[i*9+3]= M-Delta[i]+2.*M;


    ValueB[i*9+4]=  Oy[i]-auxinfoparalConformal[ i ][2]+2.*M;
    ValueB[i*9+5]= -Oy[i]+auxinfoparalConformal[ i ][2]+2.*M;
    ValueB[i*9+6]= Delta[i]+2.*M;
    ValueB[i*9+7]= M-Delta[i]+2.*M;
    ValueB[i*9+8]= 1.;


  }


  //-----------------  write the RANGES  section

  //      fprintf(MACRO,"RANGES\n");
  for(i=0 ; i< NpointsInFit ; i++) {
    //          fprintf(MACRO,"  RANGE  LAMBDA%d  1.\n",i);
    //---
    ValueRanges[i]=1.;
    sprintf(&auxNameRanges[i][0],"LAMBDA%d",i);
    NameRanges[i]=&auxNameRanges[i][0];
  }

  //-----------------  write the BOUNDS  section

  //      fprintf(MACRO,"BOUNDS\n");

  for(i=0 ; i< NpointsInFit ; i++) {
    //          fprintf(MACRO," BV  Bounds  lamp%d\n",  i);

    sprintf(&auxTypeofBound[i][0],"BV");   TypeofBound[i]= &auxTypeofBound[i][0];
    //          TypeofBound[i]="BV";
    sprintf(&auxBoundStructVarName[i][0],"lamp%d",i);
    BoundStructVarName[i]=&auxBoundStructVarName[i][0];
    BoundValue[i]=0.;
  }

  for(i=0 ; i< NpointsInFit ; i++) {
    //          fprintf(MACRO," BV  Bounds  lamm%d\n", i);
    sprintf(&auxTypeofBound[i+NpointsInFit][0],"BV");
    TypeofBound[i+NpointsInFit]= &auxTypeofBound[i+NpointsInFit][0];
    //          TypeofBound[i+NpointsInFit]="BV";
    sprintf(&auxBoundStructVarName[i+NpointsInFit][0],"lamm%d",i);
    BoundStructVarName[i+NpointsInFit]=&auxBoundStructVarName[i+NpointsInFit][0];
    BoundValue[i+NpointsInFit]=0.;
  }

  //          fprintf(MACRO," FX  Bounds  DUMMY  %g\n",2.*M);
  sprintf(&auxTypeofBound[2*NpointsInFit][0],"FX");
  TypeofBound[2*NpointsInFit]= &auxTypeofBound[2*NpointsInFit][0];
  //          TypeofBound[2*NpointsInFit]="FX";

  sprintf(&auxTypeofBound[2*NpointsInFit][0],"FX");
  TypeofBound[2*NpointsInFit]= &auxTypeofBound[2*NpointsInFit][0];

  sprintf(&auxBoundStructVarName[2*NpointsInFit][0],"DUMMY");
  BoundStructVarName[2*NpointsInFit]=&auxBoundStructVarName[2*NpointsInFit][0];
  //          BoundStructVarName[2*NpointsInFit]="DUMMY";
  BoundValue[2*NpointsInFit]=2.;
  //-----


  int status= glp_main(
		       nRows,nameRows,typeRows, //  ROWS info
		       NStructVar, NStructRows, NRowsInWhichStructVarArePresent,  //  COLUMNS info
		       StructVarName, NameRowsInWhichStructVarArePresent,  //  COLUMNS info
		       Coefficients,  //  COLUMNS info
		       ValueB,  // RHS  info
		       nRanges, ValueRanges, NameRanges, //  RANGES  info
		       nBounds, BoundValue, BoundStructVarName, TypeofBound //  BOUNDS info
		       //         ,final_values, TIMEOUT  //  timeout is in seconds.
		       ,final_values
		       );
  if (status != 0) return -100;	// fit failed

  m1_result = final_values[0];
  m2_result = final_values[1];
  q1_result = final_values[2];
  q2_result = final_values[3];



  //------------------------  transformation of the result in terms of alpha, beta, gamma


  intercept = q1_result - q2_result;
  //     intercept = q1_result;
  //     slope = m1_result ;
  slope = m1_result-m2_result ;
  //    if(fDisplayOn) {
  //       TLine *line = new TLine(-1, -slope + intercept, 1, slope + intercept);
  //       line->SetLineColor(3);
  //       line->Draw("SAME");
  //       display->Update();
  //       display->Modified();  
  //     }

  gamma = 0.;
  if( fabs( intercept ) > 1.e-10) {    //  trajectory is a circle in XY space
    alpha = slope/(intercept);
    beta = -1./(intercept);
    TypeConf=true;
    //  now take into account the rotation and correct; the only affected quantities are alpha and beta
    alfetta = alpha;
    alpha = alpha*cose - beta*sine;
    beta = alfetta*sine + beta*cose;
  }  else if(fabs(slope)> 1.e-10)  {    //  trajectory is a straight line in XY space of equation y= m*x
    //  the rotation first
    angle = atan(slope) + rotationangle;
    if( fabs(cos(angle)) > 1.e-10 ) {
      alpha = 999999.;
      beta = -alpha/tan(angle);

    } else {  //  in this case the equation is y = 0.
      alpha = 999999.;
      beta = 0.;
      TypeConf=false;
    }
  }  else {   //  in this case also the equation in XY plane is  y = 0.
    alpha = 999999.;
    beta = 0.;
    TypeConf=false;
  }


  // now take into account the displacement and correct
  gamma += (trajectory_vertex[0]*trajectory_vertex[0]+ trajectory_vertex[1]*trajectory_vertex[1]
	    -alpha*trajectory_vertex[0]-beta*trajectory_vertex[1]);
  alpha -=  2.*trajectory_vertex[0];
  beta -=  2.*trajectory_vertex[1];


  //------------------------ end of transformation of the result in terms of alpha, beta, gamma



  //--------   end of taking into account the traslation that was performed and undoing that


  // taking into account the rotation that was performed and calculate emme and qu in the normal conformal plane

  if(fabs(cose-slope*sine)> 1.e-10) {
    intercept=intercept/(cose-slope*sine);
    slope=(slope*cose+sine)/(cose-slope*sine);

    //   if(fDisplayOn) {
    //       TLine *line = new TLine(-1, -slope + intercept, 1, slope + intercept);
    //       line->SetLineColor(3);
    //       line->Draw("SAME");
    //       display->Update();
    //       display->Modified();  
    //     }
    return 1;
  } else {    //  in this case the equation is   0 = x+intercept .
    if(fabs(sine+slope*cose) < 1.e-10)  {
      cout<<" From FitHelixCylinder, situation impossible in principle! Returning -1"
	  <<endl;
      return -1;
    }

    slope=1.;
    intercept = intercept/(sine+slope*cose);
    return 99;    //  in this case the equation is   0 = x+intercept .
  }




}



Bool_t PndSecondaryTrackFinder::IntersectionFinder(Double_t xc, Double_t yc, Double_t radius, PndSttHit* stthit, TVector3 &xyz, TVector3 &dxyz) {

  // tubeID  CHECK added
  Int_t tubeID = stthit->GetTubeID();
  PndSttTube *tube = (PndSttTube*) fTubeArray->At(tubeID);
  TVector3 wiredirection = tube->GetWireDirection();
      
  if(wiredirection != TVector3(0.,0.,1.)) { 
    // cout << "wire skewed" << endl; 
    return false; 
  }
      
  // [xp, yp] point = coordinates xy of the centre of the firing tube
  TVector2 point;
  point.Set(tube->GetPosition().X(), tube->GetPosition().Y());
  Double_t isochrone = stthit->GetIsochrone();
      
  // the coordinates of the point are taken from the intersection
  // between the circumference from the drift time and the R radius of
  // curvature. -------------------------------------------------------
  // 2. find the intersection between the little circle and the line // R
  // 2.a
  // find the line passing throught [xc, yc] (centre of curvature) and [xp, yp] (first wire)
  // y = mx + q
  Double_t m = (point.Y() - yc)/(point.X() - xc);
  Double_t q = point.Y() - m*point.X();
   
  // cut on radius CHECK
  // if the simulated radius is too small, the stthit
  // is not used because rouning errors may occur
  // if(isochrone < 0.7e-3) { cout << "isochrone < 0.7e-3" << endl; return false; } // CHECK

  Double_t x1 = 0, y1 = 0,
    x2 = 0, y2 = 0,
    xb1 = 0, yb1 = 0,
    xb2 = 0, yb2 = 0;
      
  // CHECK the vertical track
  if(fabs(point.X() - xc) < 1e-6) {
	
    // 2.b
    // intersection little circle and line --> [x1, y1]
    // + and - refer to the 2 possible intersections
    // +
    x1 = point.X();
    y1 = point.Y() + sqrt(isochrone * isochrone - (x1 - point.X()) * (x1 - point.X()));
    // - 
    x2 = x1;
    y2 = point.Y() - sqrt(isochrone * isochrone - (x2 - point.X()) * (x2 - point.X()));
	
    // 2.c intersection between line and circle
    // +
    xb1 = xc;
    yb1 = yc + sqrt(radius * radius - (xb1 - xc) * (xb1 - xc));
    // -
    xb2 = xb1;
    yb2 = yc - sqrt(radius * radius - (xb2 - xc) * (xb2 - xc));
       
  }    // END CHECK
  else {
	
    // 2.b
    // intersection little circle and line --> [x1, y1]
    // + and - refer to the 2 possible intersections
    // +

    if(((m*(q - point.Y()) - point.X())*(m*(q - point.Y()) - point.X()) - (m*m + 1)*((q - point.Y())*(q - point.Y()) + point.X()*point.X() - isochrone*isochrone)) < 0.) {  if(fVerbose > 0) cout << "IntersectionFinder round errors: " << isochrone << endl; 
      return false; }

    x1 = (-(m*(q - point.Y()) - point.X()) + sqrt((m*(q - point.Y()) - point.X())*(m*(q - point.Y()) - point.X()) - (m*m + 1)*((q - point.Y())*(q - point.Y()) + point.X()*point.X() - isochrone*isochrone))) / (m*m + 1);
    y1 = m*x1 + q;
    // - 
    x2 = (-(m*(q - point.Y()) - point.X()) - sqrt((m*(q - point.Y()) - point.X())*(m*(q - point.Y()) - point.X()) - (m*m + 1)*((q - point.Y())*(q - point.Y()) + point.X()*point.X() - isochrone*isochrone))) / (m*m + 1);
    y2 = m*x2 + q;
      
    // 2.c intersection between line and circle
    // +
    xb1 = (-(m*(q - yc) - xc) + sqrt((m*(q - yc) - xc)*(m*(q - yc) - xc) - (m*m + 1)*((q - yc)*(q - yc) + xc*xc - (radius) *(radius) ))) / (m*m + 1);
    yb1 = m*xb1 + q;
    // -
    xb2 = (-(m*(q - yc) - xc) - sqrt((m*(q - yc) - xc)*(m*(q - yc) - xc) - (m*m + 1)*((q - yc)*(q - yc) + xc*xc - (radius) *(radius)))) / (m*m + 1);
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
  if(dist1 > dist2)  xyz.SetXYZ(x2, y2, stthit->GetZ());
  else xyz.SetXYZ(x1, y1, stthit->GetZ());  // <========= THIS IS THE NEW POINT to be used for the fit

  Double_t sigr = stthit->GetIsochroneError();
  Double_t sigx = sigr; // fabs(sigr * TMath::Cos(m));
  Double_t sigy = sigr; // fabs(sigr * TMath::Sin(m));
  dxyz.SetXYZ(sigx, sigy, 0);

  return kTRUE;
}


Bool_t PndSecondaryTrackFinder::Fit(TMatrixT<double> points, Double_t &outxc, Double_t &outyc, Double_t &outradius)
{

  Int_t nhits = points.GetRowUpb() + 1;
  int lasthitid = -1, firsthitid = -1;
  for(int ihit = 0; ihit < nhits; ihit++) {
    if(points[ihit][0] != -1 && points[ihit][10] != -1 && points[ihit][10] != 1) {
      if(firsthitid == -1) firsthitid = ihit;
      lasthitid = ihit;
    }
  }

  if(firsthitid == -1 || lasthitid == -1) return kFALSE; // CHECK

  Double_t trasl[2] = {points[firsthitid][2], points[firsthitid][3]};

  //  cout << "first/last " << firsthitid << " " << lasthitid << endl;
  if(firsthitid >= lasthitid) return false;
  Double_t  alpha = TMath::ATan2(points[lasthitid][3] - points[firsthitid][3],
				 points[lasthitid][2] - points[firsthitid][2]);
  Double_t Suu, Su, Sv, Suv, S1, Suuu, Suuv, Suuuu;
  
  Su = 0.;
  Sv = 0.;
  Suu = 0.;
  Suv = 0.;
  Suuu = 0.;
  S1 = 0.;
  Suuv = 0.;
  Suuuu = 0.;
  Double_t s = 0.001; // CHECK
  // ..............................................

  TVector3 fitpoint;
  for(int ihit = 0; ihit < nhits; ihit++)
    {
      Int_t hitId = (Int_t) points[ihit][0];
      Int_t detId = (Int_t) points[ihit][1];
      if(hitId == -1) continue;
      Int_t fitflag = (Int_t) points[ihit][10];
      if(fitflag == 1 || fitflag == -1) continue;
      if(detId == FairRootManager::Instance()->GetBranchId(fSttBranch) && points[ihit][8] < 0.1) continue;
      fitpoint.SetXYZ(points[ihit][2], points[ihit][3], points[ihit][4]);
      Double_t sigx = points[ihit][5];
      Double_t sigy = points[ihit][6];
      //       cout << "fitpoint" << endl;
      //       fitpoint.Print();
      // to the fit ================================
      Double_t xtrasl, ytrasl;
      // traslation
      xtrasl = fitpoint.X() - trasl[0];
      ytrasl = fitpoint.Y() - trasl[1];

      Double_t xrot, yrot;
      // rotation
      xrot = TMath::Cos(alpha)*xtrasl + TMath::Sin(alpha)*ytrasl;
      yrot = -TMath::Sin(alpha)*xtrasl + TMath::Cos(alpha)*ytrasl;
   
      // re-traslation
      xtrasl = xrot + s;
      ytrasl = yrot;

      // change coordinate
      Double_t u, v, sigv2, sigu2;
      u = xtrasl / (xtrasl*xtrasl + ytrasl*ytrasl);
      v = ytrasl / (xtrasl*xtrasl + ytrasl*ytrasl);
    
      Double_t dvdx = (-2 * xtrasl * ytrasl)/pow((xtrasl*xtrasl + ytrasl*ytrasl),2);
      Double_t dvdy = (xtrasl*xtrasl - ytrasl*ytrasl) / pow((xtrasl*xtrasl + ytrasl*ytrasl),2);
      Double_t dudx = (ytrasl*ytrasl - xtrasl*xtrasl) / pow((xtrasl*xtrasl + ytrasl*ytrasl),2);
      Double_t dudy = (-2 * xtrasl * ytrasl)/pow((xtrasl*xtrasl + ytrasl*ytrasl),2);
    
      sigu2 = dudx * dudx * sigx * sigx + dudy * dudy * sigy * sigy + 2 * dudx * dudy * sigx * sigy; 
      sigv2 = dvdx * dvdx * sigx * sigx + dvdy * dvdy * sigy * sigy + 2 * dvdx * dvdy * sigx * sigy; 

      if(sigv2 == 0) sigv2 = 1e-5; // CHECK MVD covariance

      Su = Su + (u/sigv2);
      Sv = Sv + (v/sigv2);
    
      Suv = Suv + ((u*v)/sigv2);
      Suu = Suu + ((u*u)/sigv2);
    
      Suuu = Suuu + ((u*u*u)/sigv2);
      Suuv = Suuv + ((u*u*v)/sigv2);  
    
      Suuuu = Suuuu + ((u*u*u*u)/sigv2);  
      
      S1 = S1 + 1/sigv2;
    }

  
  TMatrixT<double> matrix(3,3);
  matrix[0][0] = S1;
  matrix[0][1] = Su;
  matrix[0][2] = Suu;
  
  matrix[1][0] = Su;
  matrix[1][1] = Suu;
  matrix[1][2] = Suuu;
  
  matrix[2][0] = Suu;
  matrix[2][1] = Suuu;
  matrix[2][2] = Suuuu;
  
  Double_t determ;
  
  determ = matrix.Determinant();
  
  if (determ != 0) {
    matrix.Invert();
  }
  else {
    //    cout << "DET 0" << endl; // CHECK what to do
    return false;
  }
  
  TMatrixT<double> column(3,1);
  column[0][0] = Sv;
  column[1][0] = Suv;
  column[2][0] = Suuv;
  
  TMatrixT<double> column2(3,1);
  column2.Mult(matrix, column);
  
  Double_t a, b, c;
  a = column2[0][0];
  b = column2[1][0];
  c = column2[2][0];
  
  if(fabs(a)<0.000001) { 
    // cout << "A < 1e-**" << endl;
    return kFALSE;
  }

  // center and radius
  Double_t xcrot, ycrot, xc, yc, epsilon, R;
  ycrot = 1/(2*a);
  xcrot = -b/(2*a);
  epsilon = -c*pow((1+(b*b)), -3/2);
  R = epsilon + sqrt((xcrot*xcrot)+(ycrot*ycrot));

  // re-rotation and re-traslation of xc and yc
  // translation
  xcrot = xcrot - s;
  // rotation    
  xc = TMath::Cos(alpha)*xcrot - TMath::Sin(alpha)*ycrot;
  yc = TMath::Sin(alpha)*xcrot + TMath::Cos(alpha)*ycrot;
  // traslation
  xc = xc + trasl[0];
  yc = yc + trasl[1];
  Double_t phi = TMath::ATan2(yc, xc); 
  Double_t d;
  d = ((xc + yc) - R*(TMath::Cos(phi) + TMath::Sin(phi)))/(TMath::Cos(phi) + TMath::Sin(phi)); 
  
  //  cout << "REFITTED FIT: " << xc << " " << yc << endl;
  //  cout << "RAGGIO: " << R << endl;

  outxc = xc;
  outyc = yc;
  outradius = R;
  return true;
}




std::vector<int> PndSecondaryTrackFinder::OrderCluster2(std::vector<int> cluster, Int_t detId, double xc, double yc, double radius) {

  TClonesArray *array;
 
  if(detId == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) array = fMvdPixelHitArray;
  else if(detId == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) array = fMvdStripHitArray;
  else if(detId ==  FairRootManager::Instance()->GetBranchId(fSttBranch)) array = fSttHitArray;
  //  else if(detId ==  FairRootManager::Instance()->GetBranchId(fGemBranch)) array = fGemHitArray;

  std::vector<int> sorthits;
  std::vector<double> distances, phiangles, distancesphi;
  std::multimap<double, int> mapdistances, mapphiangles, mapdistancesphi;
    

  
  // x0 y0
  Double_t d = TMath::Sqrt(xc * xc + yc * yc) - radius;
  Double_t phi =  TMath::ATan2(yc, xc);
  
  Double_t x0 = d * TMath::Cos(phi);
  Double_t y0 = d * TMath::Sin(phi);
  
  //   int firsthitid = cluster[0];
  //   FairHit* firsthit = (FairHit*) array->At(firsthitid);
  //   //  if(!firsthit) return;
  //   double x0 = firsthit->GetX();
  //   double y0 = firsthit->GetY(); 

  Double_t Phi0 = TMath::ATan2((y0 - yc),(x0 - xc));
  Double_t scosfirst = 0, scoslast = 0.;
  TVector2 v(x0 - xc, y0 - yc); 

  for(int ihit = 0; ihit < cluster.size(); ihit++) {
    int hitid = cluster[ihit];
      
    FairHit* hit = (FairHit*) array->At(hitid);
    if(!hit) continue;

    TVector3 distance3;
    hit->Position(distance3);
    TVector3 point(xc, yc, 0.);
    double distance = (point - distance3).Perp();
    distances.push_back(distance);
    mapdistances.insert(std::pair<double, int>(distance, hitid));
    
    double alpha = TMath::ATan2(hit->GetY() - y0 + radius * TMath::Sin(Phi0), hit->GetX() - x0 + radius * TMath::Cos(Phi0));
    TVector2 p(hit->GetX() - xc, hit->GetY() - yc);
    int charge = -1; // CHECK
    Double_t phi2 = CalculatePhi(v, p, alpha, Phi0, charge);
    phiangles.push_back(phi2);
    mapphiangles.insert(std::pair<double, int>(phi2, hitid));
 
    if(fabs(radius - distance) > 1.) continue;
    double disphi = distance * phi2;
    distancesphi.push_back(disphi);
    //    cout << "DISPHI " << distance << " " << phi2 << " " << disphi << " " << radius << endl;
    mapdistancesphi.insert(std::pair<double, int>(disphi, hitid));
  }
    
  std::sort(distances.begin(), distances.end());
  std::sort(phiangles.begin(), phiangles.end());
  std::sort(distancesphi.begin(), distancesphi.end());


  double tmpdistancephi = 0;

  for(int j = 0; j < distancesphi.size(); j++) {
    double pd = distancesphi[j];
      
    if(tmpdistancephi < pd) tmpdistancephi = pd;
    else continue;

    std::multimap<double, int>::iterator it;
    int count = 0;
    int n = mapdistancesphi.count(tmpdistancephi);
      
    for(it = mapdistancesphi.begin(); it != mapdistancesphi.end(); ++it)
      {
	if(count == n) break;
	if((*it).first != tmpdistancephi) continue;
	  
	sorthits.push_back((*it).second);
	count++;
      }
  }

  // cout << "sorted" << endl;
  for(int ihit = 0; ihit < sorthits.size(); ihit++) {
    Int_t hitid = sorthits[ihit];
    //    cout << hitid << " " ;
    
    FairHit* hit = (FairHit*) array->At(hitid);
    if(!hit) continue;
    
    TVector3 position;
    hit->Position(position);
    
    if(fDisplayOn) {
      TArc *arc3 = new TArc(position.X(), position.Y(), 0.5); // CHECK
      arc3->SetLineColor(kOrange);
      arc3->SetFillStyle(0);
      arc3->Draw("SAME ONLY");
      display->Update();
      display->Modified();
    }
    
  }
  // cout << endl;

  return sorthits;
}





std::vector< TMatrixT<double> > PndSecondaryTrackFinder::OrderCluster2BIS(std::vector< TMatrixT<double> > cluster, double xc, double yc, double radius) {

  TClonesArray *array;
 
  //   if(detId == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) array = fMvdPixelHitArray;
  //   else if(detId == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) array = fMvdStripHitArray;
  //   else if(detId ==  FairRootManager::Instance()->GetBranchId(fSttBranch)) array = fSttHitArray;
  //   //  else if(detId ==  FairRootManager::Instance()->GetBranchId(fGemBranch)) array = fGemHitArray;

  std::vector< TMatrixT<double> > sorthits;
  std::vector<double> distances, phiangles, distancesphi;
  std::multimap<double, int> mapdistances, mapphiangles, mapdistancesphi;
    

  
  // x0 y0
  Double_t d = TMath::Sqrt(xc * xc + yc * yc) - radius;
  Double_t phi =  TMath::ATan2(yc, xc);
  
  Double_t x0 = d * TMath::Cos(phi);
  Double_t y0 = d * TMath::Sin(phi);
  
  //   int firsthitid = cluster[0];
  //   FairHit* firsthit = (FairHit*) array->At(firsthitid);
  //   //  if(!firsthit) return;
  //   double x0 = firsthit->GetX();
  //   double y0 = firsthit->GetY(); 

  Double_t Phi0 = TMath::ATan2((y0 - yc),(x0 - xc));
  //  Double_t scosfirst = 0, scoslast = 0.;
  TVector2 v(x0 - xc, y0 - yc); 

  for(int ihit = 0; ihit < cluster.size(); ihit++) {
  
    TMatrixT<double> singlehit = cluster[ihit];
    if(singlehit[0][0] == -1) continue;
    int hitid = (int) singlehit[0][1];
    int detid = (int) singlehit[0][2];
    if(detid == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) array = fMvdPixelHitArray;
    else if(detid == FairRootManager::Instance()->GetBranchId(fMvdStripBranch))  array = fMvdStripHitArray;
    else if(detid == FairRootManager::Instance()->GetBranchId(fSttBranch)) array = fSttHitArray;

    FairHit* hit = (FairHit*) array->At(hitid);
    if(!hit) continue;

    TVector3 distance3;
    hit->Position(distance3);
    TVector3 point(xc, yc, 0.);
    double distance = (point - distance3).Perp();
    distances.push_back(distance);
    mapdistances.insert(std::pair<double, int>(distance, ihit));
    
    double alpha = TMath::ATan2(hit->GetY() - y0 + radius * TMath::Sin(Phi0), hit->GetX() - x0 + radius * TMath::Cos(Phi0));
    TVector2 p(hit->GetX() - xc, hit->GetY() - yc);
    int charge = -1; // CHECK
    Double_t phi2 = CalculatePhi(v, p, alpha, Phi0, charge);
    phiangles.push_back(phi2);
    mapphiangles.insert(std::pair<double, int>(phi2, ihit));
 
    if(fabs(radius - distance) > 1.) continue;
    double disphi = distance * phi2;
    distancesphi.push_back(disphi);
    //    cout << "DISPHI " << distance << " " << phi2 << " " << disphi << " " << radius << endl;
    mapdistancesphi.insert(std::pair<double, int>(disphi, ihit));
  }
    
  std::sort(distances.begin(), distances.end());
  std::sort(phiangles.begin(), phiangles.end());
  std::sort(distancesphi.begin(), distancesphi.end());


  double tmpdistancephi = 0;

  for(int j = 0; j < distancesphi.size(); j++) {
    double pd = distancesphi[j];
      
    if(tmpdistancephi < pd) tmpdistancephi = pd;
    else continue;

    std::multimap<double, int>::iterator it;
    int count = 0;
    int n = mapdistancesphi.count(tmpdistancephi);
      
    for(it = mapdistancesphi.begin(); it != mapdistancesphi.end(); ++it)
      {
	if(count == n) break;
	if((*it).first != tmpdistancephi) continue;
	int hitno = (*it).second;
	TMatrixT<double> singlehit = cluster[hitno];
	sorthits.push_back(singlehit);
	count++;
      }
  }
  
  // cout << "sorted" << endl;
  for(int ihit = 0; ihit < sorthits.size(); ihit++) {
    TMatrixT<double> singlehit = sorthits[ihit];
    Int_t hitid = (Int_t) singlehit[0][1];
    //    cout << hitid << " " ;
    
    FairHit* hit = (FairHit*) array->At(hitid);
    if(!hit) continue;
    
    TVector3 position;
    hit->Position(position);
    
    if(fDisplayOn) {
      TArc *arc3 = new TArc(position.X(), position.Y(), 0.5); // CHECK
      arc3->SetLineColor(kOrange);
      arc3->SetFillStyle(0);
      arc3->Draw("SAME ONLY");
      display->Update();
      display->Modified();
    }
    
  }
  // cout << endl;

  return sorthits;
}


std::vector<int> PndSecondaryTrackFinder::OrderClusterInPhi(std::vector<int> cluster, std::vector<TVector3> positions, std::map<int, int> hitidtointersection, double xc, double yc, double radius) {

  std::vector<int> sorthits;
  std::vector<double> phiangles;
  std::multimap<double, int> mapphiangles;

  // x0 y0
  Double_t d = TMath::Sqrt(xc * xc + yc * yc) - radius;
  Double_t phi =  TMath::ATan2(yc, xc);
  
  Double_t x0 = d * TMath::Cos(phi);
  Double_t y0 = d * TMath::Sin(phi);
  

  Double_t Phi0 = TMath::ATan2((y0 - yc),(x0 - xc));
  TVector2 v(x0 - xc, y0 - yc); 

  for(int ihit = 0; ihit < cluster.size(); ihit++) {
    int hitid = cluster[ihit];
    int pointid  = hitidtointersection[hitid]; 
    //    cout << "size " << positions.size() << endl;
    TVector3 position = positions[pointid];

    double alpha = TMath::ATan2(position.Y() - y0 + radius * TMath::Sin(Phi0), position.X() - x0 + radius * TMath::Cos(Phi0));
    TVector2 p(position.X() - xc, position.Y() - yc);
    int charge = -1; // CHECK
    Double_t phi2 = CalculatePhi(v, p, alpha, Phi0, charge);
    phiangles.push_back(phi2);
    mapphiangles.insert(std::pair<double, int>(phi2, hitid));
  }
    
  std::sort(phiangles.begin(), phiangles.end());

  double tmpphi = 0;

  for(int j = 0; j < phiangles.size(); j++) {
    double pd = phiangles[j];
      
    if(tmpphi < pd) tmpphi = pd;
    else continue;

    std::multimap<double, int>::iterator it;
    int count = 0;
    int n = mapphiangles.count(tmpphi);
      
    for(it = mapphiangles.begin(); it != mapphiangles.end(); ++it)
      {
	if(count == n) break;
	if((*it).first != tmpphi) continue;
	  
	sorthits.push_back((*it).second);
	count++;
      }
  }

  return sorthits;
}





std::vector< TMatrixT<double> > PndSecondaryTrackFinder::OrderClusterInPhiBIS(std::vector< TMatrixT<double> > cluster, double xc, double yc, double radius) {

  std::vector< TMatrixT<double> > sorthits;
  std::vector<double> phiangles;
  std::multimap<double, int> mapphiangles;

  // x0 y0
  Double_t d = TMath::Sqrt(xc * xc + yc * yc) - radius;
  Double_t phi =  TMath::ATan2(yc, xc);
  
  Double_t x0 = d * TMath::Cos(phi);
  Double_t y0 = d * TMath::Sin(phi);
  

  Double_t Phi0 = TMath::ATan2((y0 - yc),(x0 - xc));
  TVector2 v(x0 - xc, y0 - yc); 

  for(int ihit = 0; ihit < cluster.size(); ihit++) {
    TMatrixT<double> singlehit = cluster[ihit];
    if(singlehit[0][0] == -1) continue;
    Int_t hitid = (Int_t) singlehit[0][1];
    Int_t detid = (Int_t) singlehit[0][2];
   
    TVector3 position(singlehit[0][4], singlehit[0][5], singlehit[0][6]);
   
    double alpha = TMath::ATan2(position.Y() - y0 + radius * TMath::Sin(Phi0), position.X() - x0 + radius * TMath::Cos(Phi0));
    TVector2 p(position.X() - xc, position.Y() - yc);
    int charge = -1; // CHECK
    Double_t phi2 = CalculatePhi(v, p, alpha, Phi0, charge);
    phiangles.push_back(phi2);
    mapphiangles.insert(std::pair<double, int>(phi2, ihit));
  }
    
  std::sort(phiangles.begin(), phiangles.end());

  double tmpphi = 0;

  for(int j = 0; j < phiangles.size(); j++) {
    double pd = phiangles[j];
      
    if(tmpphi < pd) tmpphi = pd;
    else continue;

    std::multimap<double, int>::iterator it;
    int count = 0;
    int n = mapphiangles.count(tmpphi);
      
    for(it = mapphiangles.begin(); it != mapphiangles.end(); ++it)
      {
	if(count == n) break;
	if((*it).first != tmpphi) continue;
	int hitno = (*it).second;
	//	cout << "HITNO " << hitno << endl;
	TMatrixT<double> singlehit = cluster[hitno];
	sorthits.push_back(singlehit);
	count++;
      }
  }

  return sorthits;
}



std::vector<int> PndSecondaryTrackFinder::OrderClusterInZ(std::vector<int> cluster, std::vector<TVector3> positions, std::map<int, int> hitidtointersection, double xc, double yc, double radius) {

  std::vector<int> sorthits;
  std::vector<double> zpositions;
  std::multimap<double, int> mapzpositions;

  // x0 y0
  Double_t d = TMath::Sqrt(xc * xc + yc * yc) - radius;
  Double_t phi =  TMath::ATan2(yc, xc);
  
  Double_t x0 = d * TMath::Cos(phi);
  Double_t y0 = d * TMath::Sin(phi);
  

  Double_t Phi0 = TMath::ATan2((y0 - yc),(x0 - xc));
  TVector2 v(x0 - xc, y0 - yc); 

  for(int ihit = 0; ihit < cluster.size(); ihit++) {
    int hitid = cluster[ihit];
    int pointid  = hitidtointersection[hitid]; 
    TVector3 position = positions[pointid];

    zpositions.push_back(position.Z());
    mapzpositions.insert(std::pair<double, int>(position.Z(), hitid));
  }
    
  std::sort(zpositions.begin(), zpositions.end());

  double tmpphi = 0;

  for(int j = 0; j < zpositions.size(); j++) {
    double pd = zpositions[j];
      
    if(tmpphi < pd) tmpphi = pd;
    else continue;

    std::multimap<double, int>::iterator it;
    int count = 0;
    int n = mapzpositions.count(tmpphi);
      
    for(it = mapzpositions.begin(); it != mapzpositions.end(); ++it)
      {
	if(count == n) break;
	if((*it).first != tmpphi) continue;
	  
	sorthits.push_back((*it).second);
	count++;
      }
  }

  return sorthits;
}

std::vector<int> PndSecondaryTrackFinder::AddPoints(std::vector<int> hits, Int_t detId,  Double_t xc, Double_t yc, Double_t radius, int iclus)
{

  //  cout << "==========> ADD POINT <=========" << endl;

  std::vector<int> cluster;
  TClonesArray *array;
 
  if(detId == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) array = fMvdPixelHitArray;
  else if(detId == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) array = fMvdStripHitArray;
  else if(detId ==  FairRootManager::Instance()->GetBranchId(fSttBranch)) array = fSttHitArray;
  //  else if(detId ==  FairRootManager::Instance()->GetBranchId(fGemBranch)) array = fGemHitArray;

  // add points
  for(int ihit = 0; ihit <  hits.size(); ihit++)
    {
      Int_t hitid = hits[ihit];

      PndSttHit *hit = (PndSttHit*) array->At(hitid);
      if(!hit) continue;
      TVector3 centerposition2;
      hit->Position(centerposition2);

      Double_t rd = hit->GetIsochrone();
      Double_t rderror = hit->GetIsochroneError();
      Double_t distancepc = TMath::Sqrt((centerposition2.X() - xc) * (centerposition2.X() - xc) +
					(centerposition2.Y() - yc) * (centerposition2.Y() - yc));
      
      double res = fabs(distancepc - radius) - rd;
      //  cout << "RES " << res << " " << " limit " << 3 * fLimit << endl;
      if(res < (fLimit)) { // 3 * fLimt CHECK
	//	 	cout << "ADD " << hitid << " TO CLUS " << iclus << endl;
	cluster.push_back(hitid);

	if(fDisplayOn) {
	  TArc *arc3 = new TArc(centerposition2.X(), centerposition2.Y(), 0.5); // CHECK
	  arc3->SetLineColor(fColors[iclus]);
	  arc3->SetFillStyle(0);
	  arc3->Draw("SAME ONLY");
	  display->Update();
	  display->Modified();
	}
      }
    }

  return cluster;
}


std::vector< TMatrixT<double> > PndSecondaryTrackFinder::AddPointsBIS(std::vector<int> hits, Int_t detId,  Double_t xc, Double_t yc, Double_t radius, int iclus)
{

  //  cout << "==========> ADD POINT <=========" << endl;

  std::vector< TMatrixT<double> > cluster;
  TClonesArray *array;
 
  if(detId == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) array = fMvdPixelHitArray;
  else if(detId == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) array = fMvdStripHitArray;
  else if(detId ==  FairRootManager::Instance()->GetBranchId(fSttBranch)) array = fSttHitArray;
  //  else if(detId ==  FairRootManager::Instance()->GetBranchId(fGemBranch)) array = fGemHitArray;

  // add points
  for(int ihit = 0; ihit <  hits.size(); ihit++)
    {
      Int_t hitid = hits[ihit];

      PndSttHit *hit = (PndSttHit*) array->At(hitid);
      if(!hit) continue;
      TVector3 centerposition2;
      hit->Position(centerposition2);

      Double_t rd = hit->GetIsochrone();
      Double_t rderror = hit->GetIsochroneError();
      Double_t distancepc = TMath::Sqrt((centerposition2.X() - xc) * (centerposition2.X() - xc) +
					(centerposition2.Y() - yc) * (centerposition2.Y() - yc));
      
      double res = fabs(distancepc - radius) - rd;
      //  cout << "RES " << res << " " << " limit " << 3 * fLimit << endl;
      if(res < (fLimit)) { // 3 * fLimt CHECK
	//	 	cout << "ADD " << hitid << " TO CLUS " << iclus << endl;
	
	TMatrixT<double> singlehit(1, 10);
	singlehit[0][0] = 0; // CHECK useit flag
	singlehit[0][1] = hitid;
	singlehit[0][2] = detId;
	singlehit[0][3] = 0;      // parallel
	singlehit[0][4] = centerposition2.X(); 
	singlehit[0][5] = centerposition2.Y();
	singlehit[0][6] = centerposition2.Z();
	singlehit[0][7] = 1.; // CHECK
	singlehit[0][8] = 1.; // CHECK
	singlehit[0][9] = 1.; // CHECK
	cluster.push_back(singlehit);
	
	if(fDisplayOn) {
	  TArc *arc3 = new TArc(centerposition2.X(), centerposition2.Y(), 0.5); // CHECK
	  arc3->SetLineColor(fColors[iclus]);
	  arc3->SetFillStyle(0);
	  arc3->Draw("SAME ONLY");
	  display->Update();
	  display->Modified();
	}
      }
    }

  return cluster;
}



std::vector< std::vector<int> > PndSecondaryTrackFinder::MergeClusters(std::vector< std::vector<int> > clusterlist) {

  int nclus = clusterlist.size();
  /**
     std:vector<int> doublecoll;
     TMatrixT<double> mapcoincidences(nclus, nclus);
     for(int iclus = 0; iclus < nclus; iclus++) for(int jclus = 0; jclus < nclus; jclus++) mapcoincidences[iclus][jclus] = 0;
     
     for(int iclus = 0; iclus < nclus; iclus++) {
     std::vector<int> cluster = clusterlist[iclus];
     int nhits = cluster.size();
     for(int ihit = 0; ihit < nhits; ihit++) {
     Int_t hitid = cluster[ihit];	  
     
     // for each hit
     for(int jclus = 0; jclus < nclus; jclus++) {
     if(iclus == jclus) continue;
     std::vector<int> clusterj = clusterlist[jclus];
     int nhitsj = clusterj.size();
     for(int jhit = 0; jhit < nhitsj; jhit++) {
     Int_t hitidj = clusterj[jhit];
     if(hitid == hitidj) {
     mapcoincidences[iclus][jclus]++;
     std::vector<int>::iterator it;
     it = std::find(doublecoll.begin(), doublecoll.end(), hitid);
     if(it == doublecoll.end()) doublecoll.push_back(hitid);
     break;
     }
     }
     }
     }
     }

     cout << "double collected hits" << endl;
     for(int i = 0; i < doublecoll.size(); i++) {
     cout << doublecoll[i] << " ";
     }
     cout << endl;

     mapcoincidences.Print();
     bool usablecluster[nclus]; 
     for(int iclus = 0; iclus < nclus; iclus++) usablecluster[iclus] = true;


     for(int iclus = 0; iclus < nclus; iclus++) {
     std::vector<int> cluster = clusterlist[iclus];
     if(usablecluster[iclus] == false) continue;
     std::vector<int> tmpcluster, tmpcombinations;
     for(int ihit = 0; ihit < cluster.size(); ihit++) tmpcluster.push_back(cluster[ihit]);    

     for(int jclus = iclus + 1; jclus < nclus; jclus++) {
     std::vector<int> clusterj = clusterlist[jclus];
     int percij = mapcoincidences[iclus][jclus]/tmpcluster.size() ;
     int percji = mapcoincidences[iclus][jclus]/clusterj.size() ;
     cout << iclus << " has in common with " << jclus << " the " << 100 * percij << " % of its hits" << endl;
     cout << jclus << " has in common with " << iclus << " the " << 100 * percji << " % of its hits" << endl;
     if(percij > 0.5 || percji > 0.5) {
     tmpcombinations.push_back(jclus);
     usablecluster[iclus] = false;
     usablecluster[jclus] = false;
     std::vector<int>::iterator it;
     for(int jhit = 0; jhit < clusterj.size(); jhit++) {
     it = find(tmpcluster.begin(), tmpcluster.end(), clusterj[jhit]);
     if(it == tmpcluster.end()) tmpcluster.push_back(clusterj[jhit]);
     }
     }
     }
     cout << endl;
     }

  **/

  std::vector< std::vector<int> > combinations;

  for(int iclus = 0; iclus < nclus; iclus++) {
    std::vector<int> cluster = clusterlist[iclus];
    int nhits = cluster.size();
    //     cout << "-------------------> " << iclus << endl;
    for(int jclus = iclus + 1; jclus < nclus; jclus++) {
      std::vector<int> clusterj = clusterlist[jclus];
      int nhitsj = clusterj.size();
      int combination = 0;
      for(int jhit = 0; jhit < nhitsj; jhit++) {
	Int_t hitidj = clusterj[jhit];
	std::vector<int>::iterator it;
	it = find(cluster.begin(), cluster.end(), hitidj);
	if(it != cluster.end()) combination++;
      }
     
      double percij = ((double) combination) / nhits;
      double percji = ((double) combination) / nhitsj;
      double perc = 0;
      if(percij < percji) perc = percji;
      else perc = percij;
      //       cout << iclus << " " << jclus << " " << percij << " " << percji << " " << combination << endl;    

      if(perc > 0.5) {
	// 	cout << "PERC " << iclus << " " << jclus << " " << perc << endl;
	bool done = false;
	for(int icom = 0; icom < combinations.size(); icom++) {
	  std::vector<int> knowncombination = combinations[icom];
	  std::vector<int>::iterator it, itj;
	  
	  for(int kclus = 0; kclus < knowncombination.size(); kclus++) {
	    // 	    cout << "checking combi " << icom << " clus " << knowncombination.at(kclus) << endl;
	    it = find(knowncombination.begin(), knowncombination.end(), iclus);
	    itj = find(knowncombination.begin(), knowncombination.end(), jclus);
	    if(it != knowncombination.end() || itj != knowncombination.end()) {
	      if(it == knowncombination.end()) knowncombination.push_back(iclus);
	      if(itj == knowncombination.end()) knowncombination.push_back(jclus);
	      // 	      cout << "update knowncombination " << icom << " " << iclus << " " << jclus << endl;
	      done = true;
	      break;
	    }
	  }
	  if(done == true) {
	    std::replace(combinations.begin(), combinations.end(), combinations[icom], knowncombination);
	    // 	    cout << "update knowncombinations " << knowncombination.size() << endl; 
	  }

	}
	if(false == done) {
	  // 	  cout << "newcombination " << iclus << " " << jclus << endl;
	  std::vector<int> newcombination;
	  newcombination.push_back(iclus);
	  newcombination.push_back(jclus);
	  combinations.push_back(newcombination);
	}
      }
    }
  }

  //   cout << "combination size " <<  combinations.size() << endl;

  std::vector< std::vector<int> > newlist;
  std::vector<int> usedclusters;

  for(int icom = 0; icom < combinations.size(); icom++) {
    std::vector<int> knowncombination = combinations[icom];
    std::vector<int> newcluster;
    //    cout << "combination " << icom << ": ";
    for(int kclus = 0; kclus < knowncombination.size(); kclus++) {
      int clusno = knowncombination[kclus];
      cout << knowncombination[kclus] << " ";
      usedclusters.push_back(clusno);
      std::vector<int> cluster = clusterlist[clusno];
      for(int ihit = 0; ihit < cluster.size(); ihit++) 
	{
	  int hitid = cluster[ihit];
	  std::vector<int>::iterator it;
	  it = find(newcluster.begin(), newcluster.end(), hitid);
	  if(newcluster.end() == it) newcluster.push_back(hitid);
	}

    }
    //     cout << endl;
    newlist.push_back(newcluster);
  }
  

  for(int iclus = 0; iclus < clusterlist.size(); iclus++) 
    {
      std::vector<int>::iterator it;
      it = find(usedclusters.begin(), usedclusters.end(), iclus);
      if(usedclusters.end() == it) {
	std::vector<int> cluster = clusterlist[iclus];
	newlist.push_back(cluster);
      }
    }

	




  //   for(int ihit = 0; ihit < doublecoll.size(); ihit++) {
  //     int hitid = doublecoll[ihit];
    
  //     std::vector<int>::iterator it;
  //     for(int iclus = 0; iclus < nclus; iclus++) {
  //       std::vector<int> cluster = clusterlist[iclus];
  //       it = std::find(cluster.begin(), cluster.end(), hitid);
  //       if(it == cluster.end()) continue;

  //       FairHit *hit = (FariHit*) array->At(hitid);
  //       if(!hit) continue;

  //       hit->Position();

  //     }	 


  //   }

  return newlist;

}

std::vector< std::vector< TMatrixT<double> > > PndSecondaryTrackFinder::MergeClustersBIS(std::vector< std::vector< TMatrixT<double> > > clusterlist, std::vector< std::vector<int> > *combinations) {

  int nclus = clusterlist.size();

  combinations->clear();

  for(int iclus = 0; iclus < nclus; iclus++) {
    std::vector< TMatrixT<double> > cluster = clusterlist[iclus];
    int nhits = cluster.size();
    //     cout << "-------------------> " << iclus << endl;

    // for each cluster in clusterlist I loop over the next ones
    // (not to try the same combinations twice). For each of them
    // I count the hits in common. 
    for(int jclus = iclus + 1; jclus < nclus; jclus++) {
      std::vector< TMatrixT<double> > clusterj = clusterlist[jclus];
      int nhitsj = clusterj.size();
      int combination = 0;
      // (count here)
      for(int jhit = 0; jhit < nhitsj; jhit++) {
	TMatrixT<double> singlehitj = clusterj[jhit];
	std::vector< TMatrixT<double> >::iterator it;
	it = find(cluster.begin(), cluster.end(), singlehitj);
	if(it != cluster.end()) combination++;
      }
     
      // compute the % of hits in cluster and clusterj (the
      // clusters I am comparing).
      // if one of the two % is higher than 50%, I combine them
      double percij = ((double) combination) / nhits;
      double percji = ((double) combination) / nhitsj;
      double perc = 0;
      if(percij < percji) perc = percji;
      else perc = percij;
      //       cout << iclus << " " << jclus << " " << percij << " " << percji << " " << combination << endl;    

      if(perc > 0.5) {
	// 	cout << "PERC " << iclus << " " << jclus << " " << perc << endl;
	bool done = false;
	// combine the clusters: loop over the existing combinations
	for(int icom = 0; icom < combinations->size(); icom++) {
	  std::vector<int> knowncombination = combinations->at(icom);
	  std::vector<int>::iterator it, itj;
	  // for each existing combination check if one of the two clusters
	  // I am combining now is already there. If one of the two is already there
	  // attach the other to the same combination.
	  for(int kclus = 0; kclus < knowncombination.size(); kclus++) {
	    // 	    cout << "checking combi " << icom << " clus " << knowncombination.at(kclus) << endl;
	    it = find(knowncombination.begin(), knowncombination.end(), iclus);
	    itj = find(knowncombination.begin(), knowncombination.end(), jclus);
	    if(it != knowncombination.end() || itj != knowncombination.end()) {
	      if(it == knowncombination.end()) knowncombination.push_back(iclus);
	      if(itj == knowncombination.end()) knowncombination.push_back(jclus);
	      // 	      cout << "update knowncombination " << icom << " " << iclus << " " << jclus << endl;
	      done = true;
	      break;
	    }
	  }
	  if(done == true) {
	    std::replace(combinations->begin(), combinations->end(), combinations->at(icom), knowncombination);
	    // 	    cout << "update knowncombinations " << knowncombination.size() << endl; 
	  }

	}
	// if no existing combination includes any of the two clusters
	// start a new combination
	if(false == done) {
	  // 	  cout << "newcombination " << iclus << " " << jclus << endl;
	  std::vector<int> newcombination;
	  newcombination.push_back(iclus);
	  newcombination.push_back(jclus);
	  combinations->push_back(newcombination);
	}
      }
    }
  }

  //   cout << "combination size " <<  combinations->size() << endl;

  std::vector< std::vector< TMatrixT<double> > > newlist;
  std::vector<int> usedclusters;
 
  // once the list of combinations is ready, loop over it
  for(int icom = 0; icom < combinations->size(); icom++) {
    std::vector<int> knowncombination = combinations->at(icom);
    std::vector< TMatrixT<double> > newcluster;
    //    cout << "combination " << icom << ": ";
    // for each combination loop over the clusters and inside each
    // cluster loop over the hits. Create, for each combination, 
    // the newcluster with all the hits, counting them only once.
    for(int kclus = 0; kclus < knowncombination.size(); kclus++) {
      int clusno = knowncombination[kclus];
      cout << knowncombination[kclus] << " ";

      // fill usedcluster vector with the cluster no you are
      // about to delete because it entered a combination
      usedclusters.push_back(clusno);
      std::vector< TMatrixT<double> > cluster = clusterlist[clusno];
      for(int ihit = 0; ihit < cluster.size(); ihit++) 
	{
	  TMatrixT<double> singlehit = cluster[ihit];
	  std::vector< TMatrixT<double> >::iterator it;
	  it = find(newcluster.begin(), newcluster.end(), singlehit);
	  if(newcluster.end() == it) newcluster.push_back(singlehit);
	}

    }
    //     cout << endl;
    // fill the newlist of clusters with the new compiled cluster
    newlist.push_back(newcluster);
  }
  

  for(int iclus = 0; iclus < clusterlist.size(); iclus++) 
    {
      std::vector<int>::iterator it;
      it = find(usedclusters.begin(), usedclusters.end(), iclus);
      if(usedclusters.end() == it) {
	std::vector< TMatrixT<double> > cluster = clusterlist[iclus];
	newlist.push_back(cluster);
      }
    }

  // TEST: first combinations then original clusters ==>
  // 0 combination = 0 cluster
  // 1    "        = 1    "
  // -------------------------
  //                 2 cluster
  //                 3 cluster
  cout << "CLUSTERS BEFORE COMBINATIONS" << endl;
  PrintClustersBIS(clusterlist);

  for(int icom = 0; icom < combinations->size(); icom++) {
    cout << "COMBINATION " << icom << ": ";
    std::vector<int> knowncombination = combinations->at(icom);
    for(int kclus = 0; kclus < knowncombination.size(); kclus++) {
      int clusno = knowncombination[kclus];
      cout << clusno << " ";
    }
    cout << endl;
  }
  cout << "CLUSTERS AFTER COMBINATIONS" << endl;
 PrintClustersBIS(newlist);
 if(fDisplayOn) {
   char goOnChar;
   cout << ", press any key" << endl;
   cin >> goOnChar;
   cout << "GOING ON" << endl;
 }
  //   for(int ihit = 0; ihit < doublecoll.size(); ihit++) {
  //     int hitid = doublecoll[ihit];
    
  //     std::vector<int>::iterator it;
  //     for(int iclus = 0; iclus < nclus; iclus++) {
  //       std::vector<int> cluster = clusterlist[iclus];
  //       it = std::find(cluster.begin(), cluster.end(), hitid);
  //       if(it == cluster.end()) continue;

  //       FairHit *hit = (FariHit*) array->At(hitid);
  //       if(!hit) continue;

  //       hit->Position();

  //     }	 


  //   }

  return newlist;

}



void PndSecondaryTrackFinder::PrintClusters(std::vector< std::vector<int> > clusterlist) {
  cout << "# of clusters " << clusterlist.size() << endl;
  for(int iclus = 0; iclus < clusterlist.size(); iclus++) {
    std::vector<int> cluster = clusterlist[iclus];
    int nhits = cluster.size();
    cout << "cluster no. " << iclus << " has " << nhits << " hits: ";
    for(int ihit = 0; ihit < nhits; ihit++) {
      int hitid = cluster[ihit];
      cout << hitid << " " ;
    }
    cout << endl;
  }
}

void PndSecondaryTrackFinder::PrintClustersBIS(std::vector< std::vector< TMatrixT<double> > > clusterlist) {
  cout << "# of clusters " << clusterlist.size() << endl;
  for(int iclus = 0; iclus < clusterlist.size(); iclus++) {
    std::vector< TMatrixT<double> > cluster = clusterlist[iclus];
    int nhits = cluster.size();
    cout << "cluster no. " << iclus << " has " << nhits << " hits: ";
    for(int ihit = 0; ihit < nhits; ihit++) {
      TMatrixT<double> singlehit = cluster[ihit];
      int hitid = (int) singlehit[0][1];
      cout << hitid << " " ;
    }
    cout << endl;
  }
}


void PndSecondaryTrackFinder::DrawClusters(std::vector< std::vector<int> > clusterlist) {
  Refresh();
  for(int iclus = 0; iclus < clusterlist.size(); iclus++) {
    std::vector<int> cluster = clusterlist[iclus];
    Color_t color = GetColor(iclus);
    DrawHitsColor(cluster, FairRootManager::Instance()->GetBranchId(fSttBranch), color);
  }
}

Color_t  PndSecondaryTrackFinder::GetColor(int iclus) {
  if(iclus < 82) return fColors[iclus];
  else {
    int times = floor(iclus / 82.);
    int newiclus = iclus - times * 82;
    return fColors[newiclus];
  }
}


void PndSecondaryTrackFinder::DrawClustersBIS(std::vector< std::vector< TMatrixT<double> > > clusterlist) {
  Refresh();
  for(int iclus = 0; iclus < clusterlist.size(); iclus++) {
    std::vector< TMatrixT<double> > cluster = clusterlist[iclus];
    std::vector<int> stt, mvdpix, mvdstr;

    for(int ihit = 0; ihit < cluster.size(); ihit++) {
      TMatrixT<double> singlehit = cluster[ihit];
      if(singlehit[0][0] == -1) continue;
      int hitid = (Int_t) singlehit[0][1];
      int detid = (Int_t) singlehit[0][2];
      if(detid == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) mvdpix.push_back(hitid);
      else if(detid == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) mvdstr.push_back(hitid);
      else if(detid == FairRootManager::Instance()->GetBranchId(fSttBranch)) stt.push_back(hitid);
    }

    Color_t color = GetColor(iclus);
    if(mvdpix.size() > 0) DrawHitsColor(mvdpix, FairRootManager::Instance()->GetBranchId(fMvdPixelBranch), color);
    if(mvdstr.size() > 0) DrawHitsColor(mvdstr, FairRootManager::Instance()->GetBranchId(fMvdStripBranch), color);
    if(stt.size() > 0)    DrawHitsColor(stt, FairRootManager::Instance()->GetBranchId(fSttBranch), color);
  }
}

Bool_t PndSecondaryTrackFinder::CompleteSttFit(std::vector<int> cluster, Int_t iclus, Double_t &xc, Double_t &yc, Double_t &radius, Double_t &chosenchi2, Int_t &chosencountelem) {
  

  //   TMatrixT<double> boundaries(nclus, 4);
  //   // find boundary
  //   FindBoundary(iclus, cluster, FairRootManager::Instance()->GetBranchId(fSttBranch), boundaries, kFALSE);
  
  // CONFORMAL HITS ==================================
  std::vector<std::vector<double> > conformalhits;
  Double_t firstdrift, delta, trasl[2];
  Bool_t conftras = ConformalPlaneStt4(cluster, iclus, conformalhits, firstdrift, delta, trasl);
  if(conftras == kFALSE) return conftras;

  // CONFORMAL FIT 1 =================================
  Bool_t conffit = ConformalFit(conformalhits,  iclus, delta,  trasl,  xc,  yc, radius);
  if(conffit == kFALSE) return conffit;

  if(fDisplayOn) {
    char goOnChar;
    cout << "Go back to reak plane: cluster " << iclus << endl;
    if(fDisplayOn) Refresh();
    cout << "helix " << xc << " " << yc << " " << radius << endl;     
    TArc *arc = new TArc(xc, yc, radius);
    arc->SetLineColor(kGreen);
    arc->SetFillStyle(0);
    arc->Draw("SAME ONLY");
    display->Update();
    display->Modified();
  }

  // chi2
  int countelement = 0;
  double redchi2 = CalculateRedChi2(cluster,  FairRootManager::Instance()->GetBranchId(fSttBranch), xc, yc, radius, countelement);
  cout << "===> RED CHI2 no. 0 = " << redchi2 << " " << countelement << " <===" << endl;
  chosenchi2 = redchi2;
  chosencountelem = countelement;
  
  // REFIT ============================================
  Int_t niter = 2;
  double red2chi2[niter];
  int countelement2[niter];
  if(firstdrift > 0.01) { // CHECK
    Double_t tmpxc = xc;
    Double_t tmpyc = yc;
    Double_t tmpradius = radius;
    // intersection finder & fit
    Double_t outxc[niter], outyc[niter], outradius[niter];
    for(int iter = 0; iter < niter; iter++) {
      red2chi2[iter] = 10000;
      countelement2[iter] = 0;

      Bool_t refit = RefitConformal(cluster, FairRootManager::Instance()->GetBranchId(fSttBranch), tmpxc, tmpyc, tmpradius, outxc[iter], outyc[iter], outradius[iter]);
      if(refit == kFALSE) continue;
      red2chi2[iter] = CalculateRedChi2(cluster,  FairRootManager::Instance()->GetBranchId(fSttBranch), outxc[iter], outyc[iter], outradius[iter], countelement2[iter]);
      cout << "===> RED CHI2 no. " << iter + 1 << " = " << red2chi2[iter] << " " << countelement2[iter]  << " <===" << endl;
      tmpxc = outxc[iter];
      tmpyc = outyc[iter];
      tmpradius = outradius[iter];
      
      if(fDisplayOn) {
	cout << "refit helix " << outxc[iter] << " " << outyc[iter] << " " << outradius[iter] << endl;     
	TArc *arc2 = new TArc(outxc[iter], outyc[iter], outradius[iter]);
	if(iter == 0) arc2->SetLineColor(kRed);
	else arc2->SetLineColor(kBlue);
	arc2->SetFillStyle(0);
	arc2->Draw("SAME ONLY");
	display->Update();
	display->Modified();
      }
    }
  
    for(int iter = 0; iter < 2; iter++) {
      if(fabs(1. - red2chi2[iter]) < fabs(1. - chosenchi2)) {
	xc = outxc[iter];
	yc = outyc[iter];
	radius = outradius[iter];
	chosenchi2 = red2chi2[iter];
	chosencountelem = countelement2[iter];
      }
    }
  }

  cout << "kept " << xc << " " << yc << " " << radius << endl;     
  return kTRUE;
}

void PndSecondaryTrackFinder::DeleteCluster(std::vector< std::vector<int> > *clusterlist, std::vector<int> deletecluster) {
  sort(deletecluster.begin(), deletecluster.end());
  reverse(deletecluster.begin(), deletecluster.end());
  std::vector< std::vector<int> >::iterator it = clusterlist->begin();

  for(int iclus = 0; iclus < deletecluster.size(); iclus++) {
    clusterlist->erase(it + deletecluster[iclus]);
    cout << "DELETING CLUSTER " << deletecluster[iclus] << endl;
  }
}

void PndSecondaryTrackFinder::DeleteClusterBIS(std::vector< std::vector< TMatrixT<double> > > *clusterlist, std::vector<int> deletecluster) {
  sort(deletecluster.begin(), deletecluster.end());
  reverse(deletecluster.begin(), deletecluster.end());
  std::vector< std::vector< TMatrixT<double> > >::iterator it = clusterlist->begin();

  for(int iclus = 0; iclus < deletecluster.size(); iclus++) {
    clusterlist->erase(it + deletecluster[iclus]);
    cout << "DELETING CLUSTER " << deletecluster[iclus] << endl;
  }
}

void PndSecondaryTrackFinder::DeleteParametersBIS(std::vector< TMatrixT<double> > *xyparameters, std::vector<int> deletecluster) {
  sort(deletecluster.begin(), deletecluster.end());
  reverse(deletecluster.begin(), deletecluster.end());
  std::vector< TMatrixT<double> >::iterator it = xyparameters->begin();

  for(int iclus = 0; iclus < deletecluster.size(); iclus++) {
    xyparameters->erase(it + deletecluster[iclus]);
    cout << "DELETING PARAMETERS " << deletecluster[iclus] << endl;
  }
}


void PndSecondaryTrackFinder::DeleteTrackBIS(std::vector< std::vector< TMatrixT<double> > > *clusterlist, std::vector< TMatrixT<double> > *xyparameters, std::vector<int> deletecluster) {

  DeleteClusterBIS(clusterlist, deletecluster);
  DeleteParametersBIS(xyparameters, deletecluster);
}


Bool_t PndSecondaryTrackFinder::CompleteSttFitBIS(std::vector< TMatrixT<double> > cluster, Int_t iclus, Double_t &xc, Double_t &yc, Double_t &radius, Double_t &chosenchi2, Int_t &chosencountelem) {
  
  // CONFORMAL HITS ==================================
  std::vector<std::vector<double> > conformalhits;
  Double_t firstdrift, delta, trasl[2];
  cout << "==> CONFORMAL PLANE STT + CONFORMAL FIT" << endl;
  Bool_t conftras = ConformalPlaneStt4BIS(cluster, iclus, conformalhits, firstdrift, delta, trasl);
  if(conftras == kFALSE) return conftras;

  // CONFORMAL FIT 1 =================================
  Bool_t conffit = ConformalFit(conformalhits,  iclus, delta,  trasl,  xc,  yc, radius);
  if(conffit == kFALSE) return conffit;

  if(fDisplayOn) {
    char goOnChar;
    cout << "Go back to reak plane: cluster " << iclus << endl;
    cin >> goOnChar;
    if(fDisplayOn) Refresh();
    cout << "helix " << xc << " " << yc << " " << radius << endl;     
    TArc *arc = new TArc(xc, yc, radius);
    arc->SetLineColor(kGreen);
    arc->SetFillStyle(0);
    arc->Draw("SAME ONLY");
    display->Update();
    display->Modified();
  }

  // chi2
  int countelement = 0;
  double redchi2 = CalculateRedChi2BIS(cluster, xc, yc, radius, countelement);
  cout << "===> RED CHI2 no. 0 = " << redchi2 << " " << countelement << " <===" << endl;
 
 // --------------
  Double_t newxc, newyc, newradius, newchi2 = 0;
  std::vector< TMatrixT<double> > newcluster2;
  cout << "CompleteSttFit FIRST TEST CHI2 " << xc << " " << yc << " " << radius  << endl;
  Bool_t testchi2 = TestChi2BIS(cluster, xc, yc, radius, iclus, redchi2, countelement, newxc, newyc, newradius, &newcluster2, newchi2);
  cout << "testchi2 = " << testchi2 << endl;
  if(testchi2 == kTRUE && newchi2 != 0) {
    xc = newxc;
    yc = newyc;
    radius = newradius;
    redchi2 = newchi2;
    countelement = 0;
    cout << "................. replacing" << endl;
    cluster.clear();
    cluster = newcluster2;
  }
  // --------------

 chosenchi2 = redchi2;
  chosencountelem = countelement;
  int chosenchoice = 0;
  // REFIT ============================================
  Int_t niter = 2;
  double red2chi2[niter];
  int countelement2[niter];
  if(firstdrift > 0.01) { // CHECK
    Double_t tmpxc = xc;
    Double_t tmpyc = yc;
    Double_t tmpradius = radius;
    // intersection finder & fit
    Double_t outxc[niter], outyc[niter], outradius[niter];
    for(int iter = 0; iter < niter; iter++) {
      red2chi2[iter] = 10000;
      countelement2[iter] = 0;

      Bool_t refit = RefitConformalBIS(cluster, tmpxc, tmpyc, tmpradius, outxc[iter], outyc[iter], outradius[iter]);
      if(refit == kFALSE) continue;
      red2chi2[iter] = CalculateRedChi2BIS(cluster, outxc[iter], outyc[iter], outradius[iter], countelement2[iter]);
      cout << "===> RED CHI2 no. " << iter + 1 << " = " << red2chi2[iter] << " " << countelement2[iter]  << " <===" << endl;

      // --------------
      newchi2 = 0;
      newcluster2.clear();
      cout << "CompleteSttFit " << iter << " TEST CHI2 " << outxc[iter] << " " << outyc[iter] << " " << outradius[iter]  << endl;
      testchi2 = TestChi2BIS(cluster, outxc[iter], outyc[iter], outradius[iter], iclus,  red2chi2[iter], countelement2[iter], newxc, newyc, newradius, &newcluster2, newchi2);
      cout << "testchi2 = " << testchi2 << endl;
      if(testchi2 == kTRUE && newchi2 != 0) {
	outxc[iter] = newxc;
	outyc[iter] = newyc;
	outradius[iter] = newradius;
	red2chi2[iter] = newchi2;
	countelement2[iter] = 0;
	cout << "................. replacing" << endl;
	cluster.clear();
	cluster = newcluster2;
      }
      // --------------

      tmpxc = outxc[iter];
      tmpyc = outyc[iter];
      tmpradius = outradius[iter];
      
      if(fDisplayOn) {
	cout << "refit helix " << outxc[iter] << " " << outyc[iter] << " " << outradius[iter] << endl;     
	TArc *arc2 = new TArc(outxc[iter], outyc[iter], outradius[iter]);
	if(iter == 0) arc2->SetLineColor(kRed);
	else arc2->SetLineColor(kBlue);
	arc2->SetFillStyle(0);
	arc2->Draw("SAME ONLY");
	display->Update();
	display->Modified();
      }
    }
  
    for(int iter = 0; iter < 2; iter++) {
      if(fabs(1. - red2chi2[iter]) < fabs(1. - chosenchi2)) {
	xc = outxc[iter];
	yc = outyc[iter];
	radius = outradius[iter];
	chosenchi2 = red2chi2[iter];
	chosencountelem = countelement2[iter];
	chosenchoice = iter + 1;
      }
    }
  }

  cout << "kept " << chosenchoice << " " << xc << " " << yc << " " << radius << endl;     
  return kTRUE;
}


Bool_t PndSecondaryTrackFinder::CompleteSttFitBIS(std::vector< TMatrixT<double> > *cluster, Int_t iclus, Double_t &xc, Double_t &yc, Double_t &radius, Double_t &chosenchi2, Int_t &chosencountelem) {
  
  // CONFORMAL HITS ==================================
  std::vector<std::vector<double> > conformalhits;
  Double_t firstdrift, delta, trasl[2];
  Bool_t conftras = ConformalPlaneStt4BIS(*cluster, iclus, conformalhits, firstdrift, delta, trasl);
  if(conftras == kFALSE) return conftras;

  // CONFORMAL FIT 1 =================================
  Bool_t conffit = ConformalFit(conformalhits,  iclus, delta,  trasl,  xc,  yc, radius);
  if(conffit == kFALSE) return conffit;

  if(fDisplayOn) {
    char goOnChar;
    cout << "Go back to reak plane: cluster " << iclus << endl;
    if(fDisplayOn) Refresh();
    cout << "helix " << xc << " " << yc << " " << radius << endl;     
    TArc *arc = new TArc(xc, yc, radius);
    arc->SetLineColor(kGreen);
    arc->SetFillStyle(0);
    arc->Draw("SAME ONLY");
    display->Update();
    display->Modified();
  }

  // chi2
  int countelement = 0;
  double redchi2 = CalculateRedChi2BIS(*cluster, xc, yc, radius, countelement);
  cout << "===> RED CHI2 no. 0 = " << redchi2 << " " << countelement << " <===" << endl;
  chosenchi2 = redchi2;
  chosencountelem = countelement;
  
  // REFIT ============================================
  Int_t niter = 2;
  double red2chi2[niter];
  int countelement2[niter];
  if(firstdrift > 0.01) { // CHECK
    Double_t tmpxc = xc;
    Double_t tmpyc = yc;
    Double_t tmpradius = radius;
    // intersection finder & fit
    Double_t outxc[niter], outyc[niter], outradius[niter];
    for(int iter = 0; iter < niter; iter++) {
      red2chi2[iter] = 10000;
      countelement2[iter] = 0;

      //       // ++++++++++++++
      //       cout << "######## INTERS BEFORE ########" << endl;
      //       for(int ihit = 0; ihit < cluster->size(); ihit++)
      // 	{
      // 	  TMatrixT<double> singlehit = cluster->at(ihit);
      // 	  if(singlehit[0][0] == -1) continue;
      // 	  Int_t hitid = singlehit[0][1];
      // 	  Int_t detid = singlehit[0][2];
      // 	  cout << "HITID/DETID " << hitid << " " << detid << " " << singlehit[0][3] << endl;
      // 	  double x = singlehit[0][4];
      // 	  double y = singlehit[0][5];
      // 	  double z = singlehit[0][6];
      // 	  double dx = singlehit[0][7];
      // 	  double dy = singlehit[0][8];
      // 	  double dz = singlehit[0][9];
      // 	  cout << "xyz    " << x << " " << y << " " << z << endl;
      // 	  cout << "dxdydz " << dx << " " << dy << " " << dz << endl;
      // 	}
      //       // ++++++++++++++


      Bool_t refit = RefitConformalBIS(cluster, tmpxc, tmpyc, tmpradius, outxc[iter], outyc[iter], outradius[iter]);
      if(refit == kFALSE) continue;

      //     // ++++++++++++++
      //       cout << "######## INTERS AFTER ########" << endl;
      //       for(int ihit = 0; ihit < cluster->size(); ihit++)
      // 	{
      // 	  TMatrixT<double> singlehit = cluster->at(ihit);
      // 	  if(singlehit[0][0] == -1) continue;
      // 	  Int_t hitid = singlehit[0][1];
      // 	  Int_t detid = singlehit[0][2];
      // 	  cout << "HITID/DETID " << hitid << " " << detid << " " << singlehit[0][3] << endl;
      // 	  double x = singlehit[0][4];
      // 	  double y = singlehit[0][5];
      // 	  double z = singlehit[0][6];
      // 	  double dx = singlehit[0][7];
      // 	  double dy = singlehit[0][8];
      // 	  double dz = singlehit[0][9];
      // 	  cout << "xyz    " << x << " " << y << " " << z << endl;
      // 	  cout << "dxdydz " << dx << " " << dy << " " << dz << endl;
      // 	}
      //       // ++++++++++++++

      red2chi2[iter] = CalculateRedChi2BIS(*cluster, outxc[iter], outyc[iter], outradius[iter], countelement2[iter]);
      cout << "===> RED CHI2 no. " << iter + 1 << " = " << red2chi2[iter] << " " << countelement2[iter]  << " <===" << endl;
      tmpxc = outxc[iter];
      tmpyc = outyc[iter];
      tmpradius = outradius[iter];
      
      if(fDisplayOn) {
	cout << "refit helix " << outxc[iter] << " " << outyc[iter] << " " << outradius[iter] << endl;     
	TArc *arc2 = new TArc(outxc[iter], outyc[iter], outradius[iter]);
	if(iter == 0) arc2->SetLineColor(kRed);
	else arc2->SetLineColor(kBlue);
	arc2->SetFillStyle(0);
	arc2->Draw("SAME ONLY");
	display->Update();
	display->Modified();
      }
    }
  
    for(int iter = 0; iter < 2; iter++) {
      if(fabs(1. - red2chi2[iter]) < fabs(1. - chosenchi2)) {
	xc = outxc[iter];
	yc = outyc[iter];
	radius = outradius[iter];
	chosenchi2 = red2chi2[iter];
	chosencountelem = countelement2[iter];
      }
    }
  }

  cout << "kept " << xc << " " << yc << " " << radius << endl;     
  return kTRUE;
}

Bool_t PndSecondaryTrackFinder::TestChi2(std::vector<int> cluster, Double_t xc, Double_t yc, Double_t radius,  Int_t detId, Int_t iclus, Double_t chi2, Int_t countelem, Double_t &newxc, Double_t &newyc, Double_t &newradius, std::vector<int> *newcluster, Double_t &newchi2) {
  if(chi2 < fChi2Limit) return kTRUE;

  if(countelem > cluster.size() * 0.5) return kFALSE;

  // retry
  TClonesArray *array;
  if(detId == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) array = fMvdPixelHitArray;
  else if(detId == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) array = fMvdStripHitArray;
  else if(detId ==  FairRootManager::Instance()->GetBranchId(fSttBranch)) array = fSttHitArray;
  //  else if(detId ==  FairRootManager::Instance()->GetBranchId(fGemBranch)) array = fGemHitArray;

  for(int ihit = 0; ihit < cluster.size(); ihit++)
    {
      Int_t hitid = cluster[ihit];
      PndSttHit *hit = (PndSttHit*) array->At(hitid);
      if(!hit) continue;
      TVector3 centerposition2;
      hit->Position(centerposition2);
      Double_t rd = hit->GetIsochrone();
      Double_t rderror = hit->GetIsochroneError();
      Double_t distancepc = TMath::Sqrt((centerposition2.X() - xc) * (centerposition2.X() - xc) +
					(centerposition2.Y() - yc) * (centerposition2.Y() - yc));
      double element = pow((fabs(distancepc - radius) - rd)/(rderror), 2);
      cout << "TESTCHI2 " << fCountElemLimit << " " << element << endl;
      if(element >= fCountElemLimit) continue;
      newcluster->push_back(hitid);
      cout << "pushing back " << hitid << endl;
    }
  
  int countelem2 = 0;
  Bool_t fit = CompleteSttFit(* newcluster, iclus, newxc, newyc, newradius, newchi2, countelem2);
  if(fit == kFALSE || newchi2 > fChi2Limit) return kFALSE;

  return kTRUE;

}

Bool_t PndSecondaryTrackFinder::TestChi2BIS(std::vector< TMatrixT<double> > cluster, Double_t xc, Double_t yc, Double_t radius,  Int_t iclus, Double_t chi2, Int_t countelem, Double_t &newxc, Double_t &newyc, Double_t &newradius, std::vector< TMatrixT<double> > *newcluster, Double_t &newchi2) {
  if(chi2 < fChi2Limit) return kTRUE;

  if(countelem > cluster.size() * 0.5) return kFALSE;

  // retry
  TClonesArray *array;
  //  if(detId == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) array = fMvdPixelHitArray;
  //   else if(detId == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) array = fMvdStripHitArray;
  //   else if(detId ==  FairRootManager::Instance()->GetBranchId(fSttBranch)) array = fSttHitArray;
  //  else if(detId ==  FairRootManager::Instance()->GetBranchId(fGemBranch)) array = fGemHitArray;

  for(int ihit = 0; ihit < cluster.size(); ihit++)
    {
      TMatrixT<double> singlehit = cluster[ihit];
      if(singlehit[0][0] == -1) continue;
      Int_t hitid = (Int_t) singlehit[0][1];
      Int_t detid = (Int_t) singlehit[0][2];
      if(detid != FairRootManager::Instance()->GetBranchId(fSttBranch)) continue; // CHECK THIS ONE
      array = fSttHitArray;

      PndSttHit *hit = (PndSttHit*) array->At(hitid);
      if(!hit) continue;
      TVector3 centerposition2;
      hit->Position(centerposition2);
      Double_t rd = hit->GetIsochrone();
      Double_t rderror = hit->GetIsochroneError();
      Double_t distancepc = TMath::Sqrt((centerposition2.X() - xc) * (centerposition2.X() - xc) +
					(centerposition2.Y() - yc) * (centerposition2.Y() - yc));
      double element = pow((fabs(distancepc - radius) - rd)/(rderror), 2);
      //   cout << "TESTCHI2 " << fCountElemLimit << " " << element << endl;
      if(fDisplayOn) {
	char goOnChar;
	cout << "testchi2, press any key" << endl;
	cin >> goOnChar;
	cout << "GOING ON" << endl;
	display->Update();
	display->Modified();
      }
      if(element >= fCountElemLimit) {
	if(fDisplayOn) {
	  TArc *arc = new TArc(centerposition2.X(), centerposition2.Y(), 0.5);
	  arc->SetFillColor(2);
	  //	  arc->SetFillStyle(0);
	  arc->Draw("SAME");
	  display->Update();
	  display->Modified();
	}
	continue;
      }

      if(fDisplayOn) {
	TArc *arc = new TArc(centerposition2.X(), centerposition2.Y(), 0.5);
	arc->SetFillColor(3);
	//	arc->SetFillStyle(0);
	arc->Draw("SAME");
	display->Update();
	display->Modified();
      }
      newcluster->push_back(singlehit);
      cout << "pushing back " << hitid << endl;
    }

//   if(fDisplayOn) {
//     char goOnChar;
//     cout << "testchi2, press any key" << endl;
//     cin >> goOnChar;
//     cout << "GOING ON" << endl;
//     display->Update();
//     display->Modified();
//   }
  int countelem2 = 0;
  Bool_t fit = CompleteSttFitBIS(* newcluster, iclus, newxc, newyc, newradius, newchi2, countelem2);
  if(fit == kFALSE || newchi2 > fChi2Limit) return kFALSE;

  return kTRUE;

}


// ======================= SKEW TIME! =======================
Bool_t PndSecondaryTrackFinder::DoesHitBelong(Int_t hitId, Double_t xc, Double_t yc, Double_t radius, Double_t limits[2][3], TVector3 &intersection, Bool_t draw) {

  if(fDisplayOn && draw) {
    
    TBox *b = new TBox(limits[0][0], limits[0][1], limits[1][0], limits[1][1]);
    b->SetFillStyle(0);
    b->SetLineColor(2);
    b->Draw("SAME");

    //     TMarker *l1 = new TMarker(limits[0][0], limits[0][1], 4);
    //     l1->SetMarkerColor(2);
    //     l1->Draw("SAME");

    //     TMarker *l2 = new TMarker(limits[1][0], limits[1][1], 4);
    //     l2->SetMarkerColor(2);
    //     l2->Draw("SAME");

    display->Update();
    display->Modified();
  }

  Bool_t belongs = DoesHitBelong(hitId, xc, yc, radius, intersection, draw);
  if(belongs == kFALSE) return belongs;

  cout << "LIMITS " << limits[0][0] << " " << limits[0][1] << " " << limits[0][2] << " " << limits[1][0] << " " << limits[1][1] << " " << limits[1][2] << endl;
  

  Bool_t ins = IsInsideLimits(intersection, limits);
  cout << "IS INSIDE LIM? " << ins << endl;

  //   if((intersection.X() < limits[0][0] || intersection.X() > limits[1][0]) ||
  //      (intersection.Y() < limits[0][1] || intersection.Y() > limits[1][1])) return kFALSE;
  if(ins == kFALSE) return ins;
  if(fDisplayOn && draw) {
    TMarker *m = new TMarker(intersection.X(), intersection.Y(), 21);
    m->SetMarkerColor(5);
    m->Draw("SAME");
    display->Update();
    display->Modified();
  }

  return kTRUE;
}

Bool_t PndSecondaryTrackFinder::DoesHitBelong(Int_t hitId, Double_t xc, Double_t yc, Double_t radius, TVector3 &intersection, Bool_t draw)
{
 
  if(fDisplayOn && draw) {
    char goOnChar;
    cout << "press any key" << endl;
    cin >> goOnChar;
    cout << "GOING ON" << endl;
    display->Update();
    display->Modified();
  }
  //   cout << "hitId " << hitId << endl;
  // y = mx + q
  PndSttHit *hit = (PndSttHit* ) fSttHitArray->At(hitId);
  if(!hit) return kFALSE;

  Int_t tubeID = hit->GetTubeID();
     
  PndSttTube *tube = (PndSttTube* ) fTubeArray->At(tubeID);
  TVector3 pos = tube->GetPosition();
  TVector3 wireDirection = tube->GetWireDirection();
  Double_t halflength = tube->GetHalfLength();

  TVector3 first  = pos + wireDirection * halflength; // CHECK
  TVector3 second = pos - wireDirection * halflength; // CHECK

  if(fDisplayOn && draw) {
    //      cout << "1st " << first.X() << " " << first.Y() << endl;
    //      cout << "2nd " << second.X() << " " << second.Y() << endl;
    //      first.Print();
    //      second.Print();

    TLine *l = new TLine(first.X(), first.Y(), second.X(), second.Y());
    l->Draw("SAME");
    display->Update();
    display->Modified();
   
  }

  double xint, yint, x1, y1, x2, y2, delta;

  // when tube is vertical
  if(fabs(second.X() - first.X()) < 1.e-5) {
    x1 = first.X();
    x2 = x1;

    delta = radius * radius - (x1 - xc) * (x1 - xc);
    if(delta < 0) { cout << "delta1" << endl; return kFALSE; }
    y1 = yc + TMath::Sqrt(delta);
    y2 = yc - TMath::Sqrt(delta);

  }
  else {

    Double_t m = (second.Y() - first.Y())/(second.X() - first.X());
    Double_t q = first.Y() - m * first.X();

    // center of trajectory xc, yc, radius
    delta = (m * (q - yc) - xc) * (m * (q - yc) - xc) - (m * m + 1) * ((q - yc) * (q - yc) + xc * xc - radius * radius);
    if(delta < 0) { 
      // cout << "delta2" << endl;  
      return kFALSE; }
	
    x1 = (- (m * (q - yc) - xc) + TMath::Sqrt(delta)) / (m * m + 1);
    y1 = m * x1 + q;
    x2 = (- (m * (q - yc) - xc) - TMath::Sqrt(delta)) / (m * m + 1);
    y2 = m * x2 + q;
  }

  double d1 = 0, d2 = 0;
  d1 = TMath::Sqrt((y1 - first.Y()) * (y1 - first.Y()) + (x1 - first.X()) * (x1 - first.X()));
  d2 = TMath::Sqrt((y2 - first.Y()) * (y2 - first.Y()) + (x2 - first.X()) * (x2 - first.X()));

  if(d1 < d2) {
    xint = x1;
    yint = y1;
  }
  else {
    xint = x2;
    yint = y2;
  }

  //      // APPROXIMATION: take the z of this intersection point // CHECK
  //       Double_t ll = ((xint - first.X()) + (yint - first.Y())) / (wireDirection.X() + wireDirection.Y());
  //       double zint =  first.Z() + wireDirection.Z() * ll;

  //      cout << "inters " << xint << " " << yint << endl;

  // 
  double xmin, ymin, xmax, ymax;
  if(first.X() < second.X()) {
    xmin = first.X(); 
    xmax = second.X();
  }
  else {
    xmax = first.X(); 
    xmin = second.X();
  }
  if(first.Y() < second.Y()) {
    ymin = first.Y(); 
    ymax = second.Y();
  }
  else {
    ymax = first.Y(); 
    ymin = second.Y();
  }

  if(xint < xmin || xint > xmax || yint < ymin || yint > ymax) { 
    //	cout << "OUT OF BOUNDS" << endl;
    return kFALSE;
  }

  intersection.SetX(xint);
  intersection.SetY(yint);

  return kTRUE;
}

std::vector<int> PndSecondaryTrackFinder::ZFinder(std::vector<int> hits, Double_t xc, Double_t yc, Double_t radius) {

  std::vector<int> skewedcluster;
  TVector3 intersection(0., 0., 0.);
  int nhits = hits.size();
  for(int ihit = 0; ihit < nhits; ihit++) {
    Int_t hitid = hits[ihit];
    Bool_t belong = DoesHitBelong(hitid, xc, yc, radius, intersection, kTRUE);
    if(belong == kTRUE) {
      skewedcluster.push_back(hitid);
    }
  }
  //  cout << "CLUSTER FOUND WITH " << skewedcluster.size() << " HITS" << endl;
  return  skewedcluster;
}


std::vector<int> PndSecondaryTrackFinder::ZFinderBIS(std::vector<int> hits, Double_t xc, Double_t yc, Double_t radius, Double_t limits[2][3], std::vector< TVector3 > &intersections) {

  std::vector<int> skewedcluster;
  TVector3 intersection(0., 0., 0.);
  int nhits = hits.size();
  for(int ihit = 0; ihit < nhits; ihit++) {
    Int_t hitid = hits[ihit];
    Bool_t belong = DoesHitBelong(hitid, xc, yc, radius, limits, intersection, kTRUE);
    if(belong == kTRUE) {
      skewedcluster.push_back(hitid);
      intersections.push_back(intersection);
    }
  }
  //  cout << "CLUSTER FOUND WITH " << skewedcluster.size() << " HITS" << endl;
  return  skewedcluster;
}

void PndSecondaryTrackFinder::ReCalculateXY(Int_t hitId, Double_t z, Double_t &x, Double_t &y) {
  cout << "RECALCULATION <<" << " " << hitId << endl;
  PndSttHit *hit = (PndSttHit* ) fSttHitArray->At(hitId);
  if(!hit) return;
  
  Int_t tubeID = hit->GetTubeID();
  
  PndSttTube *tube = (PndSttTube* ) fTubeArray->At(tubeID);
  TVector3 pos = tube->GetPosition();
  TVector3 wireDirection = tube->GetWireDirection();
  Double_t halflength = tube->GetHalfLength();
  
  TVector3 first  = pos + wireDirection * halflength; // CHECK
  TVector3 second = pos - wireDirection * halflength; // CHECK
  
  Double_t t = (z - first.Z()) / (second.Z() - first.Z());
  cout << "RECALCULATION " << t << endl;
  first.Print();
  second.Print();
  x = first.X() + (second.X() - first.X()) * t;
  y = first.Y() + (second.Y() - first.Y()) * t;
}


// void   PndSttTrackFinderReal::FindCharge(
// 					 Double_t oX,  // center of track
// 					 Double_t oY,  // center of track
// 					 Double_t info[][7],  // x & y of center of tubes info[][0], [][1]
// 					 UShort_t nParallelHits, // # dei paralleli
// 					 UShort_t *ListParallelHits, // in/output lista degli hitid mappa paralleli 
// 					 UShort_t *Infoparal,  // lista degli hitid paralleli 
// 					 Short_t  * Charge // out
// 					 )


Bool_t PndSecondaryTrackFinder::ZFit3(std::vector<int> cluster, Int_t charge, Double_t xc, Double_t yc, Double_t radius, Double_t &fitm, Double_t &fitp)
{

  // recalculate z - s fit only from MVD
  Double_t Sxx, Sx, Sz, Sxz, S1z;
  Double_t Detz = 0.;
  
  Sx = 0.;
  Sz = 0.;
  Sxx = 0.;
  Sxz = 0.;
  S1z = 0.;

  // x0 y0
  Double_t d = TMath::Sqrt(xc * xc + yc * yc) - radius;
  Double_t phi =  TMath::ATan2(yc, xc);
  
  Double_t x0 = d * TMath::Cos(phi);
  Double_t y0 = d * TMath::Sin(phi);
  TVector2 v(x0 - xc, y0 - yc); 
  Double_t Phi0 = TMath::ATan2((y0 - yc),(x0 - xc));
  int zcounter = 0;

  TVector3 intersection(0., 0., 0.);
  TVector3 intersection1(0., 0., 0.);
  TVector3 intersection2(0., 0., 0.);
  int nhits = cluster.size();
 
  std::map<int, int> hitidtointersections;
  std::vector<TVector3> intersectionpoints, intersectionpoints1, intersectionpoints2;
  std::vector<double> zerrors;
  //  cout << "CLUSTER WITH " << nhits << " HITS" << endl;
  for(int ihit = 0; ihit < nhits; ihit++) {
    Int_t hitid = cluster[ihit];
    Bool_t belong = DoesHitBelong(hitid, xc, yc, radius, intersection, kFALSE);
    if(belong == kFALSE) continue;
    Double_t zint1, zint2, errz;
    //     if(fDisplayOn) {
    //       char goOnChar;
    //       cout << "press any key" << endl;
    //       cin >> goOnChar;
    //       cout << "GOING ON" << endl;
    //     }  
 
    TVector3 int1(0, 0, 0), int2(0., 0., 0.);
    CalculateZ2(hitid, intersection.X(), intersection.Y(), xc, yc, radius, int1, int2, errz);
    hitidtointersections[hitid] = intersectionpoints.size();
    intersectionpoints.push_back(intersection);
    intersectionpoints1.push_back(int1);
    intersectionpoints2.push_back(int2);
    zerrors.push_back(errz);
    zcounter++; 
  }
  
  cout << "ZCOUNTER " << zcounter << endl;
  std::vector<int> sortedhits = OrderClusterInPhi(cluster, intersectionpoints, hitidtointersections, xc, yc, radius);

  TH1F *hz = new TH1F("hz", "", 17, -50, 120);
  for(int ihit = 0; ihit < nhits; ihit++) {
    int hitid = sortedhits[ihit];
    int pointid  = hitidtointersections[hitid]; 
    TVector3 point = intersectionpoints[pointid];
    TVector3 point1 = intersectionpoints1[pointid];
    TVector3 point2 = intersectionpoints2[pointid];
    double errz = zerrors[pointid]/2.;
    
    Double_t alpha = TMath::ATan2(point.Y() - y0 + radius * TMath::Sin(Phi0), point.X() - x0 + radius * TMath::Cos(Phi0));
    TVector2 p(point.X() - xc, point.Y() - yc);
    Double_t Fi = CalculatePhi(v, p, alpha, Phi0, charge);
    Double_t scos = - charge * radius * Fi; // scos = -q * R * phi CHECK :-)GOOD!

    hz->Fill(point1.Z());
    hz->Fill(point2.Z());

    if(fDisplayOn) {
      //    cout << "scos/z 0 : " << scos << " " << point.Z() << " " << errz << endl;
      TLine *l = new TLine(scos, point.Z() - errz, scos, point.Z() + errz);
      l->SetLineColor(4);
      l->Draw("SAME");
      //     cout << "scos/z I : " << scos << " " << zint1 << endl;
      
      TMarker *mrk1 = new TMarker(scos, point1.Z(), 21);
      mrk1->SetMarkerColor(2);
      mrk1->Draw("SAME");
      //        cout << "scos/z II : " << scos << " " << zint2 << endl;
      TMarker *mrk2 = new TMarker(scos, point2.Z(), 21);
      mrk2->SetMarkerColor(3);
      mrk2->Draw("SAME");
      

      //    hz->Draw();


      display->Update();
      display->Modified();  
    }
  }



  Double_t tmpfitm, tmpfitp;
  std::map<int, int> hitidtolistmap;
  std::vector<TVector3> realintersections = FindRealIntersections(&sortedhits, intersectionpoints, intersectionpoints1, intersectionpoints2, hitidtointersections, charge, xc, yc, radius, tmpfitm, tmpfitp, hitidtolistmap);


  nhits = sortedhits.size();
  Double_t Fi_pre = 0.; 
  for(int ihit = 0; ihit < nhits; ihit++) {
    int hitid = sortedhits[ihit];
    int realid = hitidtolistmap[hitid];
    TVector3 point = realintersections[realid];

    int pointid  = hitidtointersections[hitid]; 
    double errz = zerrors[pointid]/2.;
   
    Double_t alpha = TMath::ATan2(point.Y() - y0 + radius * TMath::Sin(Phi0), point.X() - x0 + radius * TMath::Cos(Phi0));
    TVector2 p(point.X() - xc, point.Y() - yc);

    //    cout << "alpha " << alpha << endl;
    //    point.Print();

    Double_t Fi = CalculatePhi(v, p, alpha, Phi0, charge);
    //    cout << "Fi " << Fi * TMath::RadToDeg() << " " << endl;
    //    if(ihit > 0) Fi = CompareToPreviousPhi(Fi, Fi_pre, charge);  // CHECK This
    //    cout << "after compare Fi " << Fi * 360 / (6.28) << " " << endl;
    Fi_pre = Fi;
    Double_t scos = - charge * radius * Fi; // scos = -q * R * phi CHECK :-)GOOD!
    //  cout << charge << " scosl " << scos << endl;
    //     cout << "FITTING POINT " << scos << " " << point.Z() << " " << errz <<endl;

    Double_t sigz2 = 1.; // errz * errz; // CHECK THIS!!

  
    if(sigz2 == 0) sigz2 = 1e-5; // CHECK MVD covariance
    //      cout << "scosl " << scos << " " << points[ihit][4] << " " << sigz2 <<  " " <<  points[ihit][7] << endl;
    zcounter++;

    if(fDisplayOn) {
      //    cout << "scos/z 0 : " << scos << " " << point.Z() << " " << errz << endl;
      TLine *l = new TLine(scos, point.Z() - errz, scos, point.Z() + errz);
      l->SetLineColor(kOrange);
      l->Draw("SAME");
      //     cout << "scos/z I : " << scos << " " << zint1 << endl;
      TMarker *mrk1 = new TMarker(scos, point.Z(), 20);
      mrk1->SetMarkerColor(kOrange);
      mrk1->Draw("SAME");
       
      display->Update();
      display->Modified();  
    }
     
    Sx = Sx + (scos /(sigz2));
    Sz = Sz + (point.Z()/(sigz2));
    Sxz = Sxz + ((scos * point.Z())/(sigz2));
    Sxx = Sxx + ((scos * scos)/(sigz2));
    S1z = S1z + 1/(sigz2);
     
  }
  
  if(zcounter <= 1) return kFALSE;  //  CHECK 
  Detz = S1z*Sxx - Sx*Sx;
  if(Detz == 0) { 
    cout << "DET Z = 0" << endl; 
    return kFALSE;
  } // CHECK
  fitp = (1/Detz)*(Sxx*Sz - Sx*Sxz);
  fitm = (1/Detz)*(S1z*Sxz - Sx*Sz);
  cout << "z fit " << fitm << " " << fitp << endl;
  double pt = radius * 0.006;
  double pl = fitm * pt;
  double ptot = TMath::Sqrt(pt * pt + pl * pl);
  cout << "MOMENTUM pt: " << pt << " pl: " << pl << " ptot: " << ptot << endl;
  return true;
}



Bool_t PndSecondaryTrackFinder::ZFit3BIS(std::vector< TMatrixT<double> > *cluster, Int_t charge, Double_t xc, Double_t yc, Double_t radius, Double_t &fitm, Double_t &fitp)
{

  // recalculate z - s fit only from MVD
  Double_t Sxx, Sx, Sz, Sxz, S1z;
  Double_t Detz = 0.;
  
  Sx = 0.;
  Sz = 0.;
  Sxx = 0.;
  Sxz = 0.;
  S1z = 0.;

  // x0 y0
  Double_t d = TMath::Sqrt(xc * xc + yc * yc) - radius;
  Double_t phi =  TMath::ATan2(yc, xc);
  
  Double_t x0 = d * TMath::Cos(phi);
  Double_t y0 = d * TMath::Sin(phi);
  TVector2 v(x0 - xc, y0 - yc); 
  Double_t Phi0 = TMath::ATan2((y0 - yc),(x0 - xc));
  int zcounter = 0;

  TVector3 intersection(0., 0., 0.);
  TVector3 intersection1(0., 0., 0.);
  TVector3 intersection2(0., 0., 0.);
  int nhits = cluster->size();
 
  std::map<std::pair<int, int> , int> hitidtointersections;
  std::vector<TVector3> intersectionpoints, intersectionpoints1, intersectionpoints2;
  std::vector<double> zerrors;
  // cout << "CLUSTER WITH " << nhits << " HITS" << endl;
  for(int ihit = 0; ihit < nhits; ihit++) {
    TMatrixT<double> singlehit = cluster->at(ihit);

    if(singlehit[0][0] == -1) continue;
    Int_t hitid = (Int_t) singlehit[0][1];
    Int_t detid = (Int_t) singlehit[0][2];
    // cout << "here " <<  detid << " " << FairRootManager::Instance()->GetBranchId(fSttBranch) << endl;
    if(detid != FairRootManager::Instance()->GetBranchId(fSttBranch)) continue; // CHECK THIS ONE
    //  cout << "here2 " << singlehit[0][3] << endl;
    // if it is parallel stt
    if(detid == FairRootManager::Instance()->GetBranchId(fSttBranch) && singlehit[0][3] == 0) continue; 
    // cout << "here3" << endl;
    intersection.SetXYZ(singlehit[0][4], singlehit[0][5], singlehit[0][6]); // CHECK THIS ONE

    Double_t zint1, zint2, errz;
    //     if(fDisplayOn) {
    //       char goOnChar;
    //       cout << "press any key" << endl;
    //       cin >> goOnChar;
    //       cout << "GOING ON" << endl;
    //     }  
 
    TVector3 int1(0, 0, 0), int2(0., 0., 0.); // CHECK THIS ONE
    CalculateZ2(hitid, intersection.X(), intersection.Y(), xc, yc, radius, int1, int2, errz); // CHECK THIS ONE

    //    hitidtointersections[hitid] = intersectionpoints.size(); // CHECK THIS ONE

    std::pair<int, int> thispair(hitid, detid);
    hitidtointersections.insert(std::pair<std::pair<int, int>, int>(thispair, intersectionpoints.size())); // CHECK THIS ONE
    intersectionpoints.push_back(intersection); // CHECK THIS ONE
    intersectionpoints1.push_back(int1); // CHECK THIS ONE
    intersectionpoints2.push_back(int2); // CHECK THIS ONE
    zerrors.push_back(errz); // CHECK THIS ONE
    //    cout << "intersections" << endl;
    //     intersection.Print();
    zcounter++; 
  }
  if(zcounter == 0) return kFALSE;  // CHECK put this outside

  cout << "ZCOUNTER " << zcounter << endl;
  std::vector< TMatrixT<double> > sortedhits = OrderClusterInPhiBIS(*cluster, xc, yc, radius);

  TH1F *hz = new TH1F("hz", "", 17, -50, 120);
  nhits = sortedhits.size();
  // cout << "NHITS " << nhits << endl;
  for(int ihit = 0; ihit < nhits; ihit++) {
    //  cout << "ihit " << ihit << endl;
    TMatrixT<double> singlehit = sortedhits[ihit];
    if(singlehit[0][0] == -1) continue;
    int hitid = (Int_t) singlehit[0][1];
    int detid = (Int_t) singlehit[0][2];
    if(detid == FairRootManager::Instance()->GetBranchId(fSttBranch) && singlehit[0][3] == 0) continue; 
    
    std::pair<int, int> pointpair(hitid, detid);
    Int_t pointid = FindInMap(hitidtointersections, pointpair); 

    TVector3 point = intersectionpoints[pointid];
    TVector3 point1 = intersectionpoints1[pointid];
    TVector3 point2 = intersectionpoints2[pointid];
    double errz = zerrors[pointid]/2.;
    
    Double_t alpha = TMath::ATan2(point.Y() - y0 + radius * TMath::Sin(Phi0), point.X() - x0 + radius * TMath::Cos(Phi0));
    TVector2 p(point.X() - xc, point.Y() - yc);
    Double_t Fi = CalculatePhi(v, p, alpha, Phi0, charge);
    Double_t scos = - charge * radius * Fi; // scos = -q * R * phi CHECK :-)GOOD!

    hz->Fill(point1.Z());
    hz->Fill(point2.Z());

    if(fDisplayOn) {
      //    cout << "scos/z 0 : " << scos << " " << point.Z() << " " << errz << endl;
      TLine *l = new TLine(scos, point.Z() - errz, scos, point.Z() + errz);
      l->SetLineColor(4);
      l->Draw("SAME");
      //     cout << "scos/z I : " << scos << " " << zint1 << endl;
      
      TMarker *mrk1 = new TMarker(scos, point1.Z(), 21);
      mrk1->SetMarkerColor(2);
      mrk1->Draw("SAME");
      //        cout << "scos/z II : " << scos << " " << zint2 << endl;
      TMarker *mrk2 = new TMarker(scos, point2.Z(), 21);
      mrk2->SetMarkerColor(3);
      mrk2->Draw("SAME");
      

      //    hz->Draw();


      display->Update();
      display->Modified();  
    }
  }



  Double_t tmpfitm, tmpfitp;
  std::map<std::pair<int, int>, int> hitidtolistmap;
  std::vector<TVector3> realintersections = FindRealIntersectionsBIS(&sortedhits, intersectionpoints, intersectionpoints1, intersectionpoints2, hitidtointersections, charge, xc, yc, radius, tmpfitm, tmpfitp, hitidtolistmap);

  //  cout << "++++ REAL INTERSECTIONS ++++" << endl;
  nhits = sortedhits.size();
  Double_t Fi_pre = 0.; 
  for(int ihit = 0; ihit < nhits; ihit++) {
    TMatrixT<double> singlehit = sortedhits[ihit];
    TMatrixT<double> newsinglehit = sortedhits[ihit];
    if(singlehit[0][0] == -1) continue;
    int hitid = (Int_t) singlehit[0][1];
    int detid = (Int_t) singlehit[0][2];
 
    if(detid == FairRootManager::Instance()->GetBranchId(fSttBranch) && singlehit[0][3] == 0) continue; 
   
    std::pair<int, int> pointpair(hitid, detid);
    Int_t realid = FindInMap(hitidtolistmap, pointpair); 
    TVector3 point = realintersections[realid];
    //     point.Print();
    Int_t pointid = FindInMap(hitidtointersections, pointpair); 
    double errz = zerrors[pointid]/2.;
    // update intersections ...
    newsinglehit[0][4] = point.X();
    newsinglehit[0][5] = point.Y();
    newsinglehit[0][6] = point.Z();
    //    newsinglehit[0][7] = 1; // CHECK
    //    newsinglehit[0][8] = 1; // CHECK
    newsinglehit[0][9] = errz;
    std::replace(cluster->begin(), cluster->end(), singlehit, newsinglehit);
    //    cout << "NEW ONE" << endl;
    //     point.Print();
    
    Double_t alpha = TMath::ATan2(point.Y() - y0 + radius * TMath::Sin(Phi0), point.X() - x0 + radius * TMath::Cos(Phi0));
    TVector2 p(point.X() - xc, point.Y() - yc);

    //    cout << "alpha " << alpha << endl;
    //    point.Print();

    Double_t Fi = CalculatePhi(v, p, alpha, Phi0, charge);
    //    cout << "Fi " << Fi * TMath::RadToDeg() << " " << endl;
    //    if(ihit > 0) Fi = CompareToPreviousPhi(Fi, Fi_pre, charge);  // CHECK This
    //    cout << "after compare Fi " << Fi * 360 / (6.28) << " " << endl;
    Fi_pre = Fi;
    Double_t scos = - charge * radius * Fi; // scos = -q * R * phi CHECK :-)GOOD!
    //  cout << charge << " scosl " << scos << endl;
    //     cout << "FITTING POINT " << scos << " " << point.Z() << " " << errz <<endl;

    Double_t sigz2 = 1.; // errz * errz; // CHECK THIS!!

  
    if(sigz2 == 0) sigz2 = 1e-5; // CHECK MVD covariance
    //      cout << "scosl " << scos << " " << points[ihit][4] << " " << sigz2 <<  " " <<  points[ihit][7] << endl;
    zcounter++;

    if(fDisplayOn) {
      //    cout << "scos/z 0 : " << scos << " " << point.Z() << " " << errz << endl;
      TLine *l = new TLine(scos, point.Z() - errz, scos, point.Z() + errz);
      l->SetLineColor(kOrange);
      l->Draw("SAME");
      //     cout << "scos/z I : " << scos << " " << zint1 << endl;
      TMarker *mrk1 = new TMarker(scos, point.Z(), 20);
      mrk1->SetMarkerColor(kOrange);
      mrk1->Draw("SAME");
       
      display->Update();
      display->Modified();  
    }
     
    Sx = Sx + (scos /(sigz2));
    Sz = Sz + (point.Z()/(sigz2));
    Sxz = Sxz + ((scos * point.Z())/(sigz2));
    Sxx = Sxx + ((scos * scos)/(sigz2));
    S1z = S1z + 1/(sigz2);
     
  }
  
  if(zcounter <= 1) return kFALSE;  //  CHECK 
  Detz = S1z*Sxx - Sx*Sx;
  if(Detz == 0) { 
    cout << "DET Z = 0" << endl; 
    return kFALSE;
  } // CHECK
  fitp = (1/Detz)*(Sxx*Sz - Sx*Sxz);
  fitm = (1/Detz)*(S1z*Sxz - Sx*Sz);
  cout << "z fit " << fitm << " " << fitp << endl;
  double pt = radius * 0.006;
  double pl = fitm * pt;
  double ptot = TMath::Sqrt(pt * pt + pl * pl);
  cout << "MOMENTUM pt: " << pt << " pl: " << pl << " ptot: " << ptot << endl;
  return true;
}



Int_t PndSecondaryTrackFinder::FindCharge(Double_t oX, Double_t oY, std::vector<int> cluster)
{
  Int_t charge = 0;  

  UShort_t nleft = 0;
  UShort_t nright = 0;
  
  Double_t cross, disq, minl = 9999999., minr = 9999999.;
  
  
  // this methods works with the hypothesis that this track comes
  //  from (0,0)
  Int_t nParallelHits = cluster.size();

  for(Int_t ihit=0; ihit < nParallelHits; ihit++){ 
    Int_t hitid = cluster[ihit];
    PndSttHit *hit = (PndSttHit* ) fSttHitArray->At(hitid);
    if(!hit) continue;
    
    // find the Z component of the cross product between the vector from (0,0) to center of
    // circular trajectory [namely, (oX,oY) ]  and the Position vector of the center of the
    // parallel Hits [namely, (x,y)].
    
    cross = oX * hit->GetY() - oY * hit->GetX();
    
    // if  cross >0  hits stays 'on the left' (which means clockwise to go from the origin
    // to the hit following the smaller path) otherwise it stays 'on the right'.
    
    if (cross > 0.) {
      disq = hit->GetX()* hit->GetX() +	hit->GetY() * hit->GetY();
      nleft++;
    } 
    else nright++;
  }

  if( nright > nleft) {
    charge = -1;
  } 
  else {
    // if ( nleft > nright) {
    charge = 1;
  } 
  //   else {	// then choose according the closest hit to the center
  //     if( minr < minl ) charge = -1;
  //     else  charge = 1;
  //   }

  return charge;
}


Int_t PndSecondaryTrackFinder::FindChargeBIS(Double_t oX, Double_t oY, std::vector< TMatrixT<double> > cluster)
{
  Int_t charge = 0;  

  UShort_t nleft = 0;
  UShort_t nright = 0;
  
  Double_t cross, disq, minl = 9999999., minr = 9999999.;
  
  // this methods works with the hypothesis that this track comes
  //  from (0,0)
  Int_t nParallelHits = 0;

  for(Int_t ihit=0; ihit < cluster.size(); ihit++){ 
    TMatrixT<double> singlehit = cluster[ihit];
    if(singlehit[0][0] == -1) continue;
    Int_t hitid = (Int_t) singlehit[0][1];
    Int_t detid = (Int_t) singlehit[0][2];
    
    if(detid !=  FairRootManager::Instance()->GetBranchId(fSttBranch)) continue; // CHECK THIS ONE!
    if(singlehit[0][3] != 0) continue; // CHECK THIS ONE!

    PndSttHit *hit = (PndSttHit* ) fSttHitArray->At(hitid);
    if(!hit) continue;
    nParallelHits++;

    // find the Z component of the cross product between the vector from (0,0) to center of
    // circular trajectory [namely, (oX,oY) ]  and the Position vector of the center of the
    // parallel Hits [namely, (x,y)].
      
    cross = oX * hit->GetY() - oY * hit->GetX();
      
    // if  cross >0  hits stays 'on the left' (which means clockwise to go from the origin
    // to the hit following the smaller path) otherwise it stays 'on the right'.
    
    if (cross > 0.) {
      disq = hit->GetX()* hit->GetX() +	hit->GetY() * hit->GetY();
      nleft++;
    } 
    else nright++;
  }

  if( nright > nleft) {
    charge = -1;
  } 
  else {
    // if ( nleft > nright) {
    charge = 1;
  } 
  //   else {	// then choose according the closest hit to the center
  //     if( minr < minl ) charge = -1;
  //     else  charge = 1;
  //   }

  return charge;
}


// 	void   PndSttTrackFinderReal::PndSttOrderingParallel(
// 		Double_t oX,
// 		Double_t oY,
// 		Double_t info[][7],
// 		UShort_t nParallelHits,
// 		UShort_t *ListParallelHits,
// 		UShort_t *Infoparal,
// 		Short_t  Charge,
// 		Double_t *Fi_initial_helix_referenceframe,
// 		Double_t *Fi_final_helix_referenceframe,
// 		Double_t *U,
// 		Double_t *V



std::vector<TVector3>  PndSecondaryTrackFinder::FindRealIntersections(std::vector<int> *cluster, std::vector<TVector3> intersectionpoints, std::vector<TVector3>  intersectionpoints1, std::vector<TVector3> intersectionpoints2, std::map<int, int> hitidtointersections, Int_t charge, Double_t xc, Double_t yc, Double_t radius, Double_t &fitm, Double_t &fitp,  std::map<int, int> &hitidtolistmap) 
{
 
  Double_t d = TMath::Sqrt(xc * xc + yc * yc) - radius;
  Double_t phi =  TMath::ATan2(yc, xc);
  
  Double_t x0 = d * TMath::Cos(phi);
  Double_t y0 = d * TMath::Sin(phi);
  TVector2 v(x0 - xc, y0 - yc); 
  Double_t Phi0 = TMath::ATan2((y0 - yc),(x0 - xc));

  int nhits = cluster->size();

  // find min and max scos and the two correspondig z (2 each, then 4 in total);
  Double_t scosmin = 999999;
  Double_t scosmax = -999999;
  Double_t zcoordmin[2], zcoordmax[2];

  for(int ihit = 0; ihit < nhits; ihit++) {
    int hitid = cluster->at(ihit);
    //  cout << "INITIAL " << hitid << endl;    
    int pointid  = hitidtointersections[hitid]; 
    TVector3 point = intersectionpoints[pointid];
    TVector3 point1 = intersectionpoints1[pointid];
    TVector3 point2 = intersectionpoints2[pointid];
    
    Double_t alpha = TMath::ATan2(point.Y() - y0 + radius * TMath::Sin(Phi0), point.X() - x0 + radius * TMath::Cos(Phi0));
    TVector2 p(point.X() - xc, point.Y() - yc);

    Double_t Fi = CalculatePhi(v, p, alpha, Phi0, charge);
    Double_t scos = - charge * radius * Fi; // scos = -q * R * phi CHECK :-)GOOD!

    if(scos < scosmin) {
      scosmin = scos;
      zcoordmin[0] = point1.Z();
      zcoordmin[1] = point2.Z();
    }

    if(scos > scosmax) {
      scosmax = scos;
      zcoordmax[0] = point1.Z();
      zcoordmax[1] = point2.Z();
    }
  }
  //   cout << "SCOS MIN/MAX " << scosmin << " " << scosmax << endl;
  //  cout << "ZCOOR MIN " << zcoordmin[0] << " " << zcoordmin[1] << endl;
  //  cout << "ZCOOR MAX " << zcoordmax[0] << " " << zcoordmax[1] << endl;

  // compute 4 lines
  // min1 max1
  Double_t m11, p11;
  m11 = (zcoordmin[0] - zcoordmax[0]) / (scosmin - scosmax);
  p11 = zcoordmin[0] - m11 * scosmin;
  TF1 *f11 = new TF1("f11", "[0] * x + [1]", -100, 100);
  f11->SetParameters(m11, p11);
  //  cout << "11 " << m11 << " " << p11 << endl;
  // min1 max12
  Double_t m12, p12;
  m12 = (zcoordmin[0] - zcoordmax[1]) / (scosmin - scosmax);
  p12 = zcoordmin[0] - m12 * scosmin;
  TF1 *f12 = new TF1("f12", "[0] * x + [1]", -100, 100);
  f12->SetParameters(m12, p12);
  //   cout << "12 " << m12 << " " << p12 << endl;
  // min2 max1
  Double_t m21, p21;
  m21 = (zcoordmin[1] - zcoordmax[0]) / (scosmin - scosmax);
  p21 = zcoordmin[1] - m21 * scosmin;
  TF1 *f21 = new TF1("f21", "[0] * x + [1]", -100, 100);
  f21->SetParameters(m21, p21);
  //   cout << "21 " << m21 << " " << p21 << endl;
  // min2 max2
  Double_t m22, p22;
  m22 = (zcoordmin[1] - zcoordmax[1]) / (scosmin - scosmax);
  p22 = zcoordmin[1] - m22 * scosmin;
  TF1 *f22 = new TF1("f22", "[0] * x + [1]", -100, 100);
  f22->SetParameters(m22, p22);
  //  cout << "22 " << m22 << " " << p22 << endl;


  // loop again and fill the list
  std::vector<TVector3> listofrealintersections[4];
  std::vector<int> realhits[4];

  for(int ihit = 0; ihit < nhits; ihit++) {
    int hitid = cluster->at(ihit);
    int pointid  = hitidtointersections[hitid]; 
    TVector3 point = intersectionpoints[pointid];
    TVector3 point1 = intersectionpoints1[pointid];
    TVector3 point2 = intersectionpoints2[pointid];
    Double_t err1 = fabs(point1.Z() - point2.Z())/4.;
    Double_t err2 = fabs(point1.Z() - point2.Z())/4.;
    if(err1 < 1) { err1 = 1.; err2 = 1; } // CHECK THIS !
    //  cout << "ERR1/2 " << hitid << " " << err1 << " " << err2 << endl;

    Double_t alpha = TMath::ATan2(point.Y() - y0 + radius * TMath::Sin(Phi0), point.X() - x0 + radius * TMath::Cos(Phi0));
    TVector2 p(point.X() - xc, point.Y() - yc);
    
    Double_t Fi = CalculatePhi(v, p, alpha, Phi0, charge);
    Double_t scos = - charge * radius * Fi; // scos = -q * R * phi CHECK :-)GOOD!

    Double_t d1 = -1;
    Double_t d2 = -1;

    // distance from 11
    d1 = fabs(point1.Z() - f11->Eval(scos));
    d2 = fabs(point2.Z() - f11->Eval(scos));
    //     cout << "for hit " << point1.Z() << " " << point2.Z() << " " << scos << " " << err1 << " " << err2 << endl;
    //     cout << "11: " << d1 << " " << d2 << endl;
    if(d1 <= err1 && d2 > err2) {
      realhits[0].push_back(hitid);
      listofrealintersections[0].push_back(point1);
      //       cout << "takin1 " << endl;;
    }
    else if(d1 > err1 && d2 <= err2) {
      realhits[0].push_back(hitid);
      listofrealintersections[0].push_back(point2);
      //        cout << "takin2 " << endl;;
    }
    else if(d1 <= err1 && d2 <= err2 && d1 < d2) {
      realhits[0].push_back(hitid);
      listofrealintersections[0].push_back(point1);
      //        cout << "takin3 " << endl;;
    }
    else if(d1 <= err1 && d2 <= err2 && d2 < d1) {
      realhits[0].push_back(hitid);
      listofrealintersections[0].push_back(point2);
      //         cout << "takin4 " << endl;;
    }

    if(fDisplayOn) { 
      TLine *line11 = new TLine(-45,  -45 * m11 + p11, 120, 120* m11 + p11);
      line11->SetLineStyle(3);
      line11->SetLineColor(1);
      line11->Draw("SAME");
    }
    
    d1 = -1;
    d2 = -1;
    // distance from 12
    d1 = fabs(point1.Z() - f12->Eval(scos));
    d2 = fabs(point2.Z() - f12->Eval(scos));
    //     cout << "12: " << d1 << " " << d2 << endl;  
    if(d1 <= err1 && d2 > err2) {
      realhits[1].push_back(hitid);
      listofrealintersections[1].push_back(point1);
    }
    else if(d1 > err1 && d2 <= err2)  {
      realhits[1].push_back(hitid);
      listofrealintersections[1].push_back(point2);
    }
    else if(d1 <= err1 && d2 > err2 && d1 < d2)  {
      realhits[1].push_back(hitid);
      listofrealintersections[1].push_back(point1);
    }
    else if(d1 <= err1 && d2 > err2 && d2 < d1)  {
      realhits[1].push_back(hitid);
      listofrealintersections[1].push_back(point2);
    }

    if(fDisplayOn) { 
      TLine *line12 = new TLine(-45,  -45 * m12 + p12, 120, 120* m12 + p12);
      line12->SetLineStyle(3);
      line12->SetLineColor(2);
      line12->Draw("SAME");
    }
    d1 = -1;
    d2 = -1;
    // distance from 21
    d1 = fabs(point1.Z() - f21->Eval(scos));
    d2 = fabs(point2.Z() - f21->Eval(scos));
    //    cout << "21: " << d1 << " " << d2 << endl;  
    if(d1 <= err1 && d2 > err2) {
      realhits[2].push_back(hitid);
      listofrealintersections[2].push_back(point1);
    }
    else if(d1 > err1 && d2 <= err2) {
      realhits[2].push_back(hitid);
      listofrealintersections[2].push_back(point2); 
    }
    else if(d1 <= err1 && d2 > err2 && d1 < d2) {
      realhits[2].push_back(hitid);
      listofrealintersections[2].push_back(point1);
    }
    else if(d1 <= err1 && d2 > err2 && d2 < d1) {
      realhits[2].push_back(hitid);
      listofrealintersections[2].push_back(point2); 
    }
    if(fDisplayOn) { 
      TLine *line21 = new TLine(-45,  -45 * m21 + p21, 120, 120* m21 + p21);
      line21->SetLineStyle(3);
      line21->SetLineColor(3);
      line21->Draw("SAME");
    }
    d1 = -1;
    d2 = -1;
    // distance from 22
    d1 = fabs(point1.Z() - f22->Eval(scos));
    d2 = fabs(point2.Z() - f22->Eval(scos));
    //      cout << "22: " << d1 << " " << d2 << endl;  
    if(d1 <= err1 && d2 > err2) {
      realhits[3].push_back(hitid);
      listofrealintersections[3].push_back(point1);
    }
    else if(d1 > err1 && d2 <= err2)  {
      realhits[3].push_back(hitid);
      listofrealintersections[3].push_back(point2);
    }
    else if(d1 <= err1 && d2 > err2 && d1 < d2)  {
      realhits[3].push_back(hitid);
      listofrealintersections[3].push_back(point1);
    }
    else if(d1 <= err1 && d2 > err2 && d2 < d1)  {
      realhits[3].push_back(hitid);
      listofrealintersections[3].push_back(point2);
    }
    if(fDisplayOn) { 
      TLine *line22 = new TLine(-45,  -45 * m22 + p22, 120, 120* m22 + p22);
      line22->SetLineStyle(3);
      line22->SetLineColor(4);
      line22->Draw("SAME");
    }
  }

  //   cout << "HOUGH YPOO " << listofrealintersections[0].size() << " " << listofrealintersections[1].size() << " " << listofrealintersections[2].size() << " " << listofrealintersections[3].size() << endl;
  int tmpchoice = -1, tmpnum = -1;  
  for(int ich = 0; ich < 4; ich++) {
    int no =  listofrealintersections[ich].size();
    //  cout << "TEST " << no << " " << tmpnum << endl;
    if(no > tmpnum) {
      //    cout << "YES" << endl;
      tmpnum = no;
      tmpchoice = ich;
    }
  }

  //  cout << "THE CHOICE IS " << tmpchoice << endl;
  // delete wrong hits from cluster
  int nrealhits =  realhits[tmpchoice].size();
  std::vector<int>  realcluster = realhits[tmpchoice];
  //  cout << "NHITS vs REALHITS " << nhits << " " << nrealhits << endl;
  if(nhits != nrealhits) {
    std::vector<int> deleting;
    for(int ihit = 0; ihit < nhits; ihit++)
      { 
	int hitid = cluster->at(ihit);
	std::vector<int>::iterator it;
	it = find(realcluster.begin(), realcluster.end(), hitid);
	if(it == realcluster.end()) {
	  deleting.push_back(hitid);
	  //   cout << "DELETING " << hitid << endl;
	}
      }

    for(int ihit = 0; ihit < deleting.size(); ihit++) 
      {
	int hitid = deleting[ihit];
	DeleteHit(hitid, cluster);
      }
  }



  for(int ipnt = 0; ipnt < tmpnum; ipnt++) {
    TVector3 point = listofrealintersections[tmpchoice].at(ipnt);
    Double_t alpha = TMath::ATan2(point.Y() - y0 + radius * TMath::Sin(Phi0), point.X() - x0 + radius * TMath::Cos(Phi0));
    TVector2 p(point.X() - xc, point.Y() - yc);
 
    Double_t Fi = CalculatePhi(v, p, alpha, Phi0, charge);
    Double_t scos = - charge * radius * Fi; // scos = -q * R * phi CHECK :-)GOOD!
   
    if(fDisplayOn) { 
      cout << "NEW POINT " << scos << " " << point.Z() << " " << realcluster.at(ipnt) << endl;
      TMarker *mrk = new TMarker(scos, point.Z(), 4);
      mrk->SetMarkerColor(kOrange);
      mrk->SetMarkerSize(4.);
      mrk->Draw("SAME");
      display->Update();
      display->Modified();
    }
  }


  switch(tmpchoice) {
  case 0: fitm = m11; fitp = p11; break;
  case 1: fitm = m12; fitp = p12; break;
  case 2: fitm = m21; fitp = p21; break;
  case 3: fitm = m22; fitp = p22; break;
  }
  cout << "THE CHOICE IS fitmfip " << fitm << " " << fitp << endl;
  if(fDisplayOn) { 
    TLine *line2 = new TLine(-45,  -45 * fitm + fitp, 120, 120* fitm + fitp);
    line2->SetLineStyle(2);
    line2->SetLineColor(kCyan);
    line2->Draw("SAME");
    display->Update();
    display->Modified();  
  }

  for(int ihit = 0; ihit < nrealhits; ihit++)
    { 
      int hitid = realcluster[ihit];
      hitidtolistmap[hitid] = ihit;
    }

  return listofrealintersections[tmpchoice];

}

std::vector<TVector3>  PndSecondaryTrackFinder::FindRealIntersectionsBIS(std::vector< TMatrixT<double> > *cluster, std::vector<TVector3> intersectionpoints, std::vector<TVector3>  intersectionpoints1, std::vector<TVector3> intersectionpoints2,  std::map<std::pair<int, int> , int> hitidtointersections, Int_t charge, Double_t xc, Double_t yc, Double_t radius, Double_t &fitm, Double_t &fitp, std::map<std::pair<int, int>, int> &hitidtolistmap) 
{
 
  Double_t d = TMath::Sqrt(xc * xc + yc * yc) - radius;
  Double_t phi =  TMath::ATan2(yc, xc);
  
  Double_t x0 = d * TMath::Cos(phi);
  Double_t y0 = d * TMath::Sin(phi);
  TVector2 v(x0 - xc, y0 - yc); 
  Double_t Phi0 = TMath::ATan2((y0 - yc),(x0 - xc));

  int nhits = cluster->size();

  // find min and max scos and the two correspondig z (2 each, then 4 in total);
  Double_t scosmin = 999999;
  Double_t scosmax = -999999;
  Double_t zcoordmin[2], zcoordmax[2];

  for(int ihit = 0; ihit < nhits; ihit++) {
    TMatrixT<double> singlehit = cluster->at(ihit);
    if(singlehit[0][0] == -1) continue;
    Int_t hitid = (Int_t) singlehit[0][1];
    Int_t detid = (Int_t) singlehit[0][2];
    if(detid == FairRootManager::Instance()->GetBranchId(fSttBranch) && singlehit[0][3] == 0) continue; 

    //  cout << "INITIAL " << hitid << endl;    
    std::pair<int, int> pointpair(hitid, detid);
    Int_t pointid = FindInMap(hitidtointersections, pointpair); 
    
    TVector3 point = intersectionpoints[pointid];
    TVector3 point1 = intersectionpoints1[pointid];
    TVector3 point2 = intersectionpoints2[pointid];
    
    Double_t alpha = TMath::ATan2(point.Y() - y0 + radius * TMath::Sin(Phi0), point.X() - x0 + radius * TMath::Cos(Phi0));
    TVector2 p(point.X() - xc, point.Y() - yc);

    Double_t Fi = CalculatePhi(v, p, alpha, Phi0, charge);
    Double_t scos = - charge * radius * Fi; // scos = -q * R * phi CHECK :-)GOOD!

    if(scos < scosmin) {
      scosmin = scos;
      zcoordmin[0] = point1.Z();
      zcoordmin[1] = point2.Z();
    }

    if(scos > scosmax) {
      scosmax = scos;
      zcoordmax[0] = point1.Z();
      zcoordmax[1] = point2.Z();
    }
  }
  //   cout << "SCOS MIN/MAX " << scosmin << " " << scosmax << endl;
  //  cout << "ZCOOR MIN " << zcoordmin[0] << " " << zcoordmin[1] << endl;
  //  cout << "ZCOOR MAX " << zcoordmax[0] << " " << zcoordmax[1] << endl;

  // compute 4 lines
  // min1 max1
  Double_t m11, p11;
  m11 = (zcoordmin[0] - zcoordmax[0]) / (scosmin - scosmax);
  p11 = zcoordmin[0] - m11 * scosmin;
  TF1 *f11 = new TF1("f11", "[0] * x + [1]", -100, 100);
  f11->SetParameters(m11, p11);
  //  cout << "11 " << m11 << " " << p11 << endl;
  // min1 max12
  Double_t m12, p12;
  m12 = (zcoordmin[0] - zcoordmax[1]) / (scosmin - scosmax);
  p12 = zcoordmin[0] - m12 * scosmin;
  TF1 *f12 = new TF1("f12", "[0] * x + [1]", -100, 100);
  f12->SetParameters(m12, p12);
  //   cout << "12 " << m12 << " " << p12 << endl;
  // min2 max1
  Double_t m21, p21;
  m21 = (zcoordmin[1] - zcoordmax[0]) / (scosmin - scosmax);
  p21 = zcoordmin[1] - m21 * scosmin;
  TF1 *f21 = new TF1("f21", "[0] * x + [1]", -100, 100);
  f21->SetParameters(m21, p21);
  //   cout << "21 " << m21 << " " << p21 << endl;
  // min2 max2
  Double_t m22, p22;
  m22 = (zcoordmin[1] - zcoordmax[1]) / (scosmin - scosmax);
  p22 = zcoordmin[1] - m22 * scosmin;
  TF1 *f22 = new TF1("f22", "[0] * x + [1]", -100, 100);
  f22->SetParameters(m22, p22);
  //  cout << "22 " << m22 << " " << p22 << endl;


  // loop again and fill the list
  std::vector<TVector3> listofrealintersections[4];
  std::vector< TMatrixT<double> > realhits[4];

  for(int ihit = 0; ihit < nhits; ihit++) {
    TMatrixT<double> singlehit = cluster->at(ihit);
    if(singlehit[0][0] == -1) continue;
    Int_t hitid = (Int_t) singlehit[0][1];
    Int_t detid = (Int_t) singlehit[0][2];

    std::pair<int, int> pointpair(hitid, detid);
    Int_t pointid = FindInMap(hitidtointersections, pointpair); 

    TVector3 point = intersectionpoints[pointid];
    TVector3 point1 = intersectionpoints1[pointid];
    TVector3 point2 = intersectionpoints2[pointid];
    Double_t err1 = fabs(point1.Z() - point2.Z())/4.;
    Double_t err2 = fabs(point1.Z() - point2.Z())/4.;
    if(err1 < 1) { err1 = 1.; err2 = 1; } // CHECK THIS !
    //  cout << "ERR1/2 " << hitid << " " << err1 << " " << err2 << endl;

    Double_t alpha = TMath::ATan2(point.Y() - y0 + radius * TMath::Sin(Phi0), point.X() - x0 + radius * TMath::Cos(Phi0));
    TVector2 p(point.X() - xc, point.Y() - yc);
    
    Double_t Fi = CalculatePhi(v, p, alpha, Phi0, charge);
    Double_t scos = - charge * radius * Fi; // scos = -q * R * phi CHECK :-)GOOD!

    Double_t d1 = -1;
    Double_t d2 = -1;

    // distance from 11
    d1 = fabs(point1.Z() - f11->Eval(scos));
    d2 = fabs(point2.Z() - f11->Eval(scos));
    //     cout << "for hit " << point1.Z() << " " << point2.Z() << " " << scos << " " << err1 << " " << err2 << endl;
    //     cout << "11: " << d1 << " " << d2 << endl;
    if(d1 <= err1 && d2 > err2) {
      realhits[0].push_back(singlehit);
      listofrealintersections[0].push_back(point1);
      //       cout << "takin1 " << endl;;
    }
    else if(d1 > err1 && d2 <= err2) {
      realhits[0].push_back(singlehit);
      listofrealintersections[0].push_back(point2);
      //        cout << "takin2 " << endl;;
    }
    else if(d1 <= err1 && d2 <= err2 && d1 < d2) {
      realhits[0].push_back(singlehit);
      listofrealintersections[0].push_back(point1);
      //        cout << "takin3 " << endl;;
    }
    else if(d1 <= err1 && d2 <= err2 && d2 < d1) {
      realhits[0].push_back(singlehit);
      listofrealintersections[0].push_back(point2);
      //         cout << "takin4 " << endl;;
    }

    if(fDisplayOn) { 
      TLine *line11 = new TLine(-45,  -45 * m11 + p11, 120, 120* m11 + p11);
      line11->SetLineStyle(3);
      line11->SetLineColor(1);
      line11->Draw("SAME");
    }
    
    d1 = -1;
    d2 = -1;
    // distance from 12
    d1 = fabs(point1.Z() - f12->Eval(scos));
    d2 = fabs(point2.Z() - f12->Eval(scos));
    //     cout << "12: " << d1 << " " << d2 << endl;  
    if(d1 <= err1 && d2 > err2) {
      realhits[1].push_back(singlehit);
      listofrealintersections[1].push_back(point1);
    }
    else if(d1 > err1 && d2 <= err2)  {
      realhits[1].push_back(singlehit);
      listofrealintersections[1].push_back(point2);
    }
    else if(d1 <= err1 && d2 > err2 && d1 < d2)  {
      realhits[1].push_back(singlehit);
      listofrealintersections[1].push_back(point1);
    }
    else if(d1 <= err1 && d2 > err2 && d2 < d1)  {
      realhits[1].push_back(singlehit);
      listofrealintersections[1].push_back(point2);
    }

    if(fDisplayOn) { 
      TLine *line12 = new TLine(-45,  -45 * m12 + p12, 120, 120* m12 + p12);
      line12->SetLineStyle(3);
      line12->SetLineColor(2);
      line12->Draw("SAME");
    }
    d1 = -1;
    d2 = -1;
    // distance from 21
    d1 = fabs(point1.Z() - f21->Eval(scos));
    d2 = fabs(point2.Z() - f21->Eval(scos));
    //    cout << "21: " << d1 << " " << d2 << endl;  
    if(d1 <= err1 && d2 > err2) {
      realhits[2].push_back(singlehit);
      listofrealintersections[2].push_back(point1);
    }
    else if(d1 > err1 && d2 <= err2) {
      realhits[2].push_back(singlehit);
      listofrealintersections[2].push_back(point2); 
    }
    else if(d1 <= err1 && d2 > err2 && d1 < d2) {
      realhits[2].push_back(singlehit);
      listofrealintersections[2].push_back(point1);
    }
    else if(d1 <= err1 && d2 > err2 && d2 < d1) {
      realhits[2].push_back(singlehit);
      listofrealintersections[2].push_back(point2); 
    }
    if(fDisplayOn) { 
      TLine *line21 = new TLine(-45,  -45 * m21 + p21, 120, 120* m21 + p21);
      line21->SetLineStyle(3);
      line21->SetLineColor(3);
      line21->Draw("SAME");
    }
    d1 = -1;
    d2 = -1;
    // distance from 22
    d1 = fabs(point1.Z() - f22->Eval(scos));
    d2 = fabs(point2.Z() - f22->Eval(scos));
    //      cout << "22: " << d1 << " " << d2 << endl;  
    if(d1 <= err1 && d2 > err2) {
      realhits[3].push_back(singlehit);
      listofrealintersections[3].push_back(point1);
    }
    else if(d1 > err1 && d2 <= err2)  {
      realhits[3].push_back(singlehit);
      listofrealintersections[3].push_back(point2);
    }
    else if(d1 <= err1 && d2 > err2 && d1 < d2)  {
      realhits[3].push_back(singlehit);
      listofrealintersections[3].push_back(point1);
    }
    else if(d1 <= err1 && d2 > err2 && d2 < d1)  {
      realhits[3].push_back(singlehit);
      listofrealintersections[3].push_back(point2);
    }
    if(fDisplayOn) { 
      TLine *line22 = new TLine(-45,  -45 * m22 + p22, 120, 120* m22 + p22);
      line22->SetLineStyle(3);
      line22->SetLineColor(4);
      line22->Draw("SAME");
    }
  }

  //   cout << "HOUGH YPOO " << listofrealintersections[0].size() << " " << listofrealintersections[1].size() << " " << listofrealintersections[2].size() << " " << listofrealintersections[3].size() << endl;
  int tmpchoice = -1, tmpnum = -1;  
  for(int ich = 0; ich < 4; ich++) {
    int no =  listofrealintersections[ich].size();
    //  cout << "TEST " << no << " " << tmpnum << endl;
    if(no > tmpnum) {
      //    cout << "YES" << endl;
      tmpnum = no;
      tmpchoice = ich;
    }
  }

  //  cout << "THE CHOICE IS " << tmpchoice << endl;
  // delete wrong hits from cluster
  int nrealhits =  realhits[tmpchoice].size();
  std::vector< TMatrixT<double> >  realcluster = realhits[tmpchoice];
  //  cout << "NHITS vs REALHITS " << nhits << " " << nrealhits << endl;
  if(nhits != nrealhits) {
    std::vector<  TMatrixT<double> > deleting;
    for(int ihit = 0; ihit < nhits; ihit++)
      { 
	TMatrixT<double> singlehit = cluster->at(ihit);
	if(singlehit[0][0] == -1) continue;
	Int_t hitid = (Int_t) singlehit[0][1];
	Int_t detid = (Int_t) singlehit[0][2];

	std::vector< TMatrixT<double> >::iterator it;
	it = find(realcluster.begin(), realcluster.end(), singlehit);
	if(it == realcluster.end()) {
	  deleting.push_back(singlehit);
	  //   cout << "DELETING " << hitid << endl;
	}
      }

    for(int ihit = 0; ihit < deleting.size(); ihit++) 
      {
	TMatrixT<double> singlehit = deleting[ihit];
	DeleteHitBIS(singlehit, cluster);
      }
  }



  for(int ipnt = 0; ipnt < tmpnum; ipnt++) {
    TVector3 point = listofrealintersections[tmpchoice].at(ipnt);
    Double_t alpha = TMath::ATan2(point.Y() - y0 + radius * TMath::Sin(Phi0), point.X() - x0 + radius * TMath::Cos(Phi0));
    TVector2 p(point.X() - xc, point.Y() - yc);
 
    Double_t Fi = CalculatePhi(v, p, alpha, Phi0, charge);
    Double_t scos = - charge * radius * Fi; // scos = -q * R * phi CHECK :-)GOOD!
   
    if(fDisplayOn) { 
      TMatrixT<double> singlehit = realcluster[ipnt];
      Int_t hitid = (Int_t) singlehit[0][1];
      Int_t detid = (Int_t) singlehit[0][2];
      cout << "NEW POINT " << scos << " " << point.Z() << " " << hitid << " " << detid << endl;
      TMarker *mrk = new TMarker(scos, point.Z(), 4);
      mrk->SetMarkerColor(kOrange);
      mrk->SetMarkerSize(4.);
      mrk->Draw("SAME");
      display->Update();
      display->Modified();
    }
  }


  switch(tmpchoice) {
  case 0: fitm = m11; fitp = p11; break;
  case 1: fitm = m12; fitp = p12; break;
  case 2: fitm = m21; fitp = p21; break;
  case 3: fitm = m22; fitp = p22; break;
  }
  cout << "THE CHOICE IS fitmfip " << fitm << " " << fitp << endl;
  if(fDisplayOn) { 
    TLine *line2 = new TLine(-45,  -45 * fitm + fitp, 120, 120* fitm + fitp);
    line2->SetLineStyle(2);
    line2->SetLineColor(kCyan);
    line2->Draw("SAME");
    display->Update();
    display->Modified();  
  }

  for(int ihit = 0; ihit < nrealhits; ihit++)
    { 
      TMatrixT<double> singlehit = realcluster[ihit];
      Int_t hitid = (Int_t) singlehit[0][1];
      Int_t detid = (Int_t) singlehit[0][2]; 
      std::pair<int, int> thispair(hitid, detid);
      hitidtolistmap.insert(std::pair<std::pair<int, int>, int>(thispair, ihit)); // CHECK THIS ONE
    }
  
  return listofrealintersections[tmpchoice];

}


TVector3 PndSecondaryTrackFinder::FindTangentInPoint(Double_t xc, Double_t yc, Double_t radius, Double_t x, Double_t y, Double_t &m, Double_t &p) {

  TVector3 c(xc, yc, 0.);
  TVector3 point(x, y, 0.);

  TVector3 r = c - point;

  // rotate 90 deg counterclockwise
  TVector3 t(-r.Y(), r.X(), 0.);
  t.Unit();

  m = t.Y()/t.X();
  p = y - m * x;

  //  cout << "TANGENT IN " << x << " " << y << " " << m << " " << p << endl;
  //  if(fDisplayOn) { 
  //     TLine *line = new TLine(-45, -45 * m + p, 120, 120 * m + p);
  //     line->SetLineColor(2);
  //     line->Draw("SAME");
  //     display->Update();
  //     display->Modified();  
  //   }

  return t;
}

void PndSecondaryTrackFinder::CalculateZ2(Int_t hitId, Double_t x, Double_t y, Double_t xc, Double_t yc, Double_t radius, TVector3 &int1, TVector3 &int2, Double_t &errz) {

  PndSttHit *hit = (PndSttHit* ) fSttHitArray->At(hitId);
  if(!hit) return;

  Int_t tubeID = hit->GetTubeID();
     
  PndSttTube *tube = (PndSttTube* ) fTubeArray->At(tubeID);
  TVector3 pos = tube->GetPosition();
  TVector3 wireDirection = tube->GetWireDirection();
  Double_t halflength = tube->GetHalfLength();

  TVector3 first  = pos + wireDirection * halflength; // CHECK
  TVector3 second = pos - wireDirection * halflength; // CHECK

  //    cout << "first/second " << endl;
  //    first.Print();
  //    second.Print();

  if(fDisplayOn) {
    TMarker *mrk = new TMarker(x, y, 4);  
    mrk->SetMarkerColor(2);
    //     mrk->Draw("SAME");
    display->Update();
    display->Modified();
  }
    
  // tangent approximation
  Double_t m, p;
  TVector3 tangent = FindTangentInPoint(xc, yc, radius, x, y, m, p) ;
   
  // rotate clockwise on z axis
  double beta = wireDirection.Phi();
  if(beta < 0) beta += TMath::Pi();
  // ... the tangent
  double rtx = TMath::Cos(beta) * tangent.X() + TMath::Sin(beta) * tangent.Y();
  double rty = TMath::Cos(beta) * tangent.Y() - TMath::Sin(beta) * tangent.X();
  TVector3 rottangent(rtx, rty, 0.0);
  rottangent.Unit();
  // ... the point
  double rx = TMath::Cos(beta) * x + TMath::Sin(beta) * y;
  double ry = TMath::Cos(beta) * y - TMath::Sin(beta) * x;

  // translation
  Double_t deltay = ry;
  rty -= deltay;
  ry -= deltay;

  // rotm, rotp
  Double_t rotm = rottangent.Y()/rottangent.X();
  Double_t rotp = ry - rotm * rx;

  if(fDisplayOn) { 
    TMarker *mrk = new TMarker(rx, ry, 20);  
    mrk->SetMarkerColor(kGray);
    //     mrk->Draw("SAME");

    TLine *line = new TLine(-45, -45 * rotm + rotp, 120, 120 * rotm + rotp);
    line->SetLineColor(kGray);
    //     line->Draw("SAME");

    display->Update();
    display->Modified();  
  }


  // ellipsis
  double a = hit->GetIsochrone() * TMath::Cos(skew);
  double b = hit->GetIsochrone();

  // center of ellipsis
  Double_t x0a, x0b, y0;
  y0 = 0.;
  x0a = (-rotp + TMath::Sqrt(b * b + a * a * rotm * rotm)) / rotm;
  x0b = (-rotp - TMath::Sqrt(b * b + a * a * rotm * rotm)) / rotm;

   
  // intersection point
  double intxa = (x0a * b * b - rotm * rotp * a * a) / (b * b + rotm * rotm * a * a);
  double intya = rotm * intxa + rotp;
  double intxb = (x0b * b * b - rotm * rotp * a * a) / (b * b + rotm * rotm * a * a);
  double intyb = rotm * intxb + rotp;
   
   
  if(fDisplayOn) { 
    TEllipse *ell1 = new TEllipse(x0a, y0, a, b);
    ell1->SetLineColor(kOrange);
    ell1->SetFillStyle(0);    
    //     ell1->Draw("SAME");

    TEllipse *ell2 = new TEllipse(x0b, y0, a, b);;
    ell2->SetLineColor(kGreen);
    ell2->SetFillStyle(0);    
    //     ell2->Draw("SAME");

    TMarker *mrk1 = new TMarker(intxa, intya, 21);
    mrk1->SetMarkerColor(kOrange);
    //     mrk1->Draw("SAME");
     
    TMarker *mrk2 = new TMarker(intxb, intyb, 21);
    mrk2->SetMarkerColor(kGreen);
    //     mrk2->Draw("SAME");
     
    display->Update();
    display->Modified();  
  }
   
   
  // retranslate
  y0 += deltay; 
  intya  += deltay; 
  intyb  += deltay; 
   
  // rerotate
  double x0anew = TMath::Cos(beta) * x0a - TMath::Sin(beta) * y0;
  double y0anew = TMath::Cos(beta) * y0 + TMath::Sin(beta) * x0a;
  double x0bnew = TMath::Cos(beta) * x0b - TMath::Sin(beta) * y0;
  double y0bnew = TMath::Cos(beta) * y0 + TMath::Sin(beta) * x0b;
   
  //    cout << "RETRANSLATED/ROTATEa " << x0anew << " " << y0anew << endl;
  //    cout << "RETRANSLATED/ROTATEb " << x0bnew << " " << y0bnew << endl;
   
  //    cout << "DELTA " << - 2 * x0a * rotm * rotp - rotp * rotp + b * b - rotm * rotm * x0a * x0a + a * a * rotm * rotm << " " 
  // 	<< - 2 * x0b * rotm * rotp - rotp * rotp + b * b - rotm * rotm * x0b * x0b + a * a * rotm * rotm << endl;
   
   
  double intxanew = TMath::Cos(beta) * intxa - TMath::Sin(beta) * intya;
  double intyanew = TMath::Cos(beta) * intya + TMath::Sin(beta) * intxa;
  double intxbnew = TMath::Cos(beta) * intxb - TMath::Sin(beta) * intyb;
  double intybnew = TMath::Cos(beta) * intyb + TMath::Sin(beta) * intxb;
   
  intxa = intxanew;
  intya = intyanew;
  intxb = intxbnew;
  intyb = intybnew;

  if(fDisplayOn) { 
    double degbeta = TMath::RadToDeg();
    TEllipse *ell1 = new TEllipse(x0anew, y0anew, a, b, 0, 360, degbeta);
    ell1->SetLineColor(kOrange);
    ell1->SetFillStyle(0);    
    //     ell1->Draw("SAME");

    TEllipse *ell2 = new TEllipse(x0bnew, y0bnew, a, b, 0, 360, degbeta);
    ell2->SetLineColor(kGreen);
    ell2->SetFillStyle(0);    
    //     ell2->Draw("SAME");

    TMarker *mrk1 = new TMarker(intxa, intya, 21);
    mrk1->SetMarkerColor(kOrange);
    //     mrk1->Draw("SAME");

    TMarker *mrk2 = new TMarker(intxb, intyb, 21);
    mrk2->SetMarkerColor(kGreen);
    //     mrk2->Draw("SAME");



    display->Update();
    display->Modified();  
  }

  x0a = x0anew;
  double y0a = y0anew;
  x0b = x0bnew;
  double y0b = y0bnew;


  // calculate z0a, z0b
  Double_t t = ((x0a + y0a) - (first.X() + first.Y())) /  ((second.X() - first.X()) + (second.Y() - first.Y()));
  Double_t z0a = first.Z() + (second.Z() - first.Z()) * t;
  //    cout << "0 : calculate t, z0a " << t << " " << z0a << endl;
 
  t = ((x0b + y0b) - (first.X() + first.Y())) /  ((second.X() - first.X()) + (second.Y() - first.Y()));
  Double_t z0b = first.Z() + (second.Z() - first.Z()) * t;
  //    cout << "0 : calculate t, z0b " << t << " " << z0b << endl;
 

  // int1.SetXYZ(x0a, y0a, z0a);
  // int1.SetXYZ(x0b, y0b, z0b);


  double dx = intxa - x0a;
  double dy = intya - y0a;
  TVector3 dxdy(dx, dy, 0.0);

  TVector3 tfirst = first + dxdy;
  TVector3 tsecond = second + dxdy;

  t = ((intxa + intya) - (tfirst.X() + tfirst.Y())) /  ((tsecond.X() - tfirst.X()) + (tsecond.Y() - tfirst.Y()));
  double intza = tfirst.Z() + (tsecond.Z() - tfirst.Z()) * t;
  //    cout << "I : calculate t, z " << t << " " << intza << endl;
 

  tfirst = first - dxdy;
  tsecond = second - dxdy;

  t = ((intxb + intyb) - (tfirst.X() + tfirst.Y())) /  ((tsecond.X() - tfirst.X()) + (tsecond.Y() - tfirst.Y()));
  double intzb = tfirst.Z() + (tsecond.Z() - tfirst.Z()) * t;
  //   cout << "II : calculate t, z " << t << " " << intzb << endl;
 
  int1.SetXYZ(intxa, intya, intza);
  int2.SetXYZ(intxb, intyb, intzb);
 
  errz = fabs(intza - intzb)/2.   ;
}

Int_t PndSecondaryTrackFinder::FindInMap(std::map< std::pair<int, int>, int> m, std::pair<int, int> p) {
  std::map< std::pair<int, int>, int>::iterator it;
  for(it = m.begin(); it != m.end(); it++) { 
    if((*it).first.first == p.first &&  (*it).first.second == p.second) return (*it).second;
  }
  return -1;
}

// +++++++++++++ CLEANUP STUFF ++++++++++++++++++++

Bool_t PndSecondaryTrackFinder::IsInsideLimits(TVector3 intersection, Double_t limits[2][3]) {
  
  double dmin = limits[0][2];
  double dmax = limits[1][2];
  
  TVector2 first, last;
  if( fabs(TMath::Sqrt(limits[0][0]* limits[0][0] + limits[0][1] * limits[0][1]) - dmin) < 1) {
    first = TVector2(limits[0][0], limits[0][1]);
    last = TVector2(limits[1][0], limits[1][1]);
  }
  else if( fabs(TMath::Sqrt(limits[0][0]* limits[0][0] + limits[0][1] * limits[0][1]) - dmax) < 1) {
    last = TVector2(limits[0][0], limits[0][1]);
    first = TVector2(limits[1][0], limits[1][1]);
  }
  else if( fabs(TMath::Sqrt(limits[0][0] * limits[0][0] + limits[1][1] * limits[1][1]) - dmin) < 1) {
    first = TVector2(limits[0][0], limits[1][1]);
    last = TVector2(limits[1][0], limits[0][1]);
  } 
  else if( fabs(TMath::Sqrt(limits[0][0] * limits[0][0] + limits[1][1] * limits[1][1]) - dmax) < 1) {
    last = TVector2(limits[0][0], limits[1][1]);
    first = TVector2(limits[1][0], limits[0][1]);
  }
  else cout << "WHAT??" << endl;
  
  double skewlimit     = 28.; // CHECK THIS LIMIT
  double skewthickness = 8.6; // CHECK THIS LIMIT
  
  if(dmin < skewlimit && dmax > skewlimit) {
    cout << "1: dmin " << dmin << " dmax " << dmax << endl;
    if((intersection.X() < limits[0][0] || intersection.X() > limits[1][0]) ||
       (intersection.Y() < limits[0][1] || intersection.Y() > limits[1][1])) {
      cout  << "OUTSIDE" << endl;
      return kFALSE;
    }
  }
  else if(dmax < skewlimit) {
    cout << "2: dmin " << dmin << " dmax " << dmax << endl;
    if(TMath::Sqrt((intersection.X() - last.X()) * (intersection.X() - last.X()) + (intersection.Y() - last.Y()) * (intersection.Y() - last.Y())) > skewthickness) {


      cout  << "OUTSIDE 2" << TMath::Sqrt((intersection.X() - last.X()) * (intersection.X() - last.X()) + 
					  (intersection.Y() - last.Y()) * (intersection.Y() - last.Y())) << endl;
      cout << "intersection " << intersection.X() << " " << intersection.Y() << endl;
      cout << "first " << first.X() << " " << first.Y() << endl;
      cout << "last " << last.X() << " " << last.Y() << endl;

      return kFALSE;
    }
  }
  else if( dmin > skewlimit) {
    cout << "3: dmin " << dmin << " dmax " << dmax << endl;
    if(TMath::Sqrt((intersection.X() - first.X()) * (intersection.X() - first.X()) + (intersection.Y() - first.Y()) * (intersection.Y() - first.Y())) > skewthickness) {


      cout  << "OUTSIDE 2" << TMath::Sqrt((intersection.X() - first.X()) * (intersection.X() - first.X()) + 
					  (intersection.Y() - first.Y()) * (intersection.Y() - first.Y())) << endl;
      cout << "intersection " << intersection.X() << " " << intersection.Y() << endl;
      cout << "first " << first.X() << " " << first.Y() << endl;
      cout << "first " << first.X() << " " << first.Y() << endl;

      return kFALSE;
    }
  }
  else cout << "WHAT NUMBER 2 ??" << endl;
  
  
  return kTRUE;
}
 
 
// assign each it to at most one track
void PndSecondaryTrackFinder::ForbidCrossingTracks(std::vector< std::vector< TMatrixT<double> > > * clusterlist, std::vector< TMatrixT<double> > parlist) {

  // list of deletions
  std::vector< TMatrixT<double> > deletelist;

  for(int itrk = 0; itrk < clusterlist->size(); itrk++) {
    std::vector< TMatrixT<double> > cluster = clusterlist->at(itrk);
    Mat2x3 limits;
    FindLimits(cluster, limits);

    TMatrixT<double> par = parlist.at(itrk);
    Double_t xc = par[0][0];
    Double_t yc = par[0][1];
    Double_t radius = par[0][2];

    // through IP?
    double d0 = fabs(TMath::Sqrt(xc * xc  + yc * yc) - radius);

    if(d0 < 0.5)   { // CHECK THIS LIMIT
      if(limits[0][0] * limits[1][0] < 0 ||
	 limits[1][0] * limits[1][1] < 0) cout << "TRANSVERSING ***********************" << endl;
    }


    //        for(int ihit = 0; ihit < cluster1.size(); ihit++) {
    // 	 TMatrixT<double> singlehit = cluster1[ihit];
    //        }
  }
}
 
void PndSecondaryTrackFinder::SetUpUsableHitMap() {

  // TMatrixT<double> hitid | driftradius
  TMatrixT<double> tubeoccupancy(fTubeArray->GetEntriesFast(), 2);
  // init
  for(Int_t itube = 0; itube < fTubeArray->GetEntriesFast(); itube++) {
    PndSttTube *tube = (PndSttTube*) fTubeArray->At(itube);
    tubeoccupancy[itube][0] = -1;
    tubeoccupancy[itube][1] = -1;
  }


  // fSttUsable | usable | base hit |
  // loop on stt hits
  fSttUsableHits.ResizeTo(fSttHitArray->GetEntriesFast(), 2);

  for(Int_t ihit = 0; ihit < fSttHitArray->GetEntriesFast(); ihit++) {
    PndSttHit *hit = (PndSttHit*) fSttHitArray->At(ihit);
    Int_t tubeid = hit->GetTubeID();
    PndSttTube *tube = (PndSttTube*) fTubeArray->At(tubeid);


    if(tubeoccupancy[tubeid][0] == -1) { 
      // it the tube is not occupied --> fill it and ok
      tubeoccupancy[tubeid][0] = ihit;
      tubeoccupancy[tubeid][1] = hit->GetIsochrone();
      // fill map
      fSttUsableHits[ihit][0] = 1;
      fSttUsableHits[ihit][1] = ihit;
    }
    else if(hit->GetIsochrone() < tubeoccupancy[tubeid][1]) { 
      // if the tube is occupied but with an higher drift radius

      // associate old hit to this one
      int oldihit = tubeoccupancy[tubeid][0];
      fSttUsableHits[oldihit][0] = 1;
      fSttUsableHits[oldihit][1] = ihit;

      // occupy the tube with this new hit
      tubeoccupancy[tubeid][0] = ihit;
      tubeoccupancy[tubeid][1] = hit->GetIsochrone();

      // fill the map accordingly with this new hit
      fSttUsableHits[ihit][0] = 1;
      fSttUsableHits[ihit][1] = ihit;
    }
    else {
      // if the tube is occupied but with a lower drift radius
      // that one must be used

      // fill the map with the old hit information
      fSttUsableHits[ihit][0] = 1;
      fSttUsableHits[ihit][1] = tubeoccupancy[tubeid][0];
    }
  }
}


std::vector<int> PndSecondaryTrackFinder::FindAssignedClusters(TMatrixT<double> singlehit, std::vector< std::vector< TMatrixT<double> > > clusterlist) {
  
  std::vector<int> assignlist;
  if(singlehit[0][0] == -1) return assignlist;
  std::vector< TMatrixT<double> >::iterator it;
  for(int iclus = 0; iclus < clusterlist.size(); iclus++) {
    std::vector< TMatrixT<double> > cluster = clusterlist[iclus];
    it = find(cluster.begin(), cluster.end(), singlehit);
    if(it != cluster.end()) assignlist.push_back(iclus);
  }
  return assignlist;

}

Bool_t PndSecondaryTrackFinder::IsMultiplyAssigned(TMatrixT<double> singlehit, std::vector< std::vector< TMatrixT<double> > > clusterlist, std::vector<int> &assignlist) {
  
  if(singlehit[0][0] == -1) return -1;
  assignlist = FindAssignedClusters(singlehit, clusterlist);
  if(assignlist.size() > 1) return kTRUE;
  return kFALSE;
}

void  PndSecondaryTrackFinder::DrawMultipleAssignedHits(std::vector< std::vector< TMatrixT<double> > > clusterlist) {
  cout << "DRAW MULTIPLE HIT " << endl;
  
  for(int iclus = 0; iclus < clusterlist.size(); iclus++) {
    std::vector< TMatrixT<double> > cluster = clusterlist[iclus];
    for(int ihit = 0; ihit < cluster.size(); ihit++) {
      TMatrixT<double> singlehit = cluster[ihit];
      if(singlehit[0][0] == -1) continue;
      std::vector<int> assignlist;
      bool multi = IsMultiplyAssigned(singlehit, clusterlist, assignlist);
      if(multi == kFALSE) continue;
      TMarker *mrk = new TMarker(singlehit[0][4], singlehit[0][5], 30);
      mrk->SetMarkerColor(2);
      mrk->Draw("SAME");
    }
    display->Update();
    display->Modified();  
    
  }
}

std::vector< TMatrixT<double> > PndSecondaryTrackFinder::FindConnections(TMatrixT<double> singlehit, std::vector< TMatrixT<double> > cluster) {

  std::vector< TMatrixT<double> > connectionslist;
  TVector3 position(singlehit[0][4], singlehit[0][5], singlehit[0][6]);
  
  for(int ihit = 0; ihit < cluster.size(); ihit++) {
    TMatrixT<double> singlehit2 = cluster[ihit];
    if(singlehit2[0][0] == -1 || singlehit2 == singlehit) continue;
    TVector3 position2(singlehit2[0][4], singlehit2[0][5], singlehit2[0][6]);
    double distance = (position - position2).Perp();
    if(distance < 1.1) {
      cout << "connect " << singlehit[0][1] << " to " << singlehit2[0][1] << endl;
      connectionslist.push_back(singlehit2);
    }
  }
  
  return connectionslist;
}

Bool_t PndSecondaryTrackFinder::IsAggregation(TMatrixT<double> singlehit, std::vector<TMatrixT<double> > cluster,  std::vector< TMatrixT<double> > &connectionslist)
{
  connectionslist = FindConnections(singlehit, cluster);
  if(connectionslist.size() > 2) return kTRUE;
  return kFALSE;
}

// cluster -> computelimits, cleanaggregations


std::vector< TMatrixT<double> >  PndSecondaryTrackFinder::CleanAggregations(std::vector<TMatrixT<double> > cluster, std::vector< std::vector<TMatrixT<double> > > clusterlist) {
  std::vector< TMatrixT<double> >  newcluster;

  std::vector< TMatrixT<double> >  aggregator;
  std::vector< TMatrixT<double> >::iterator it;

  for(int ihit = 0; ihit < cluster.size(); ihit++) {
    TMatrixT<double> singlehit = cluster[ihit];
    if(singlehit[0][0] == -1) continue;
    
    // get its list of connections
    std::vector< TMatrixT<double> > connectionslist;
    Bool_t aggregation = IsAggregation(singlehit, cluster, connectionslist);
    if(aggregation == kFALSE) continue;
    it = find(aggregator.begin(), aggregator.end(), singlehit);
    if(it == aggregator.end()) aggregator.push_back(singlehit);

    cout << "HIT " <<  singlehit[0][1] << " is aggregation" << endl;
    // otherwise
    TVector3 position(singlehit[0][4], singlehit[0][5], singlehit[0][6]);
    // loop over the connections
    for(int icon = 0; icon < connectionslist.size(); icon++) {
     TMatrixT<double> singlehit2 = connectionslist[icon];
     it = find(aggregator.begin(), aggregator.end(), singlehit2);
     if(it == aggregator.end()) aggregator.push_back(singlehit2);
 
      std::vector<int> assignlist;
      bool multi = IsMultiplyAssigned(singlehit2, clusterlist, assignlist);
      cout << "CONNECTION " << singlehit2[0][1] << " MULTI " << multi << endl;
      if(multi == kFALSE) {
	it = find(newcluster.begin(), newcluster.end(), singlehit2);
	if(it ==  newcluster.end()) newcluster.push_back(singlehit2); 
	continue;
      }

      bool goodhit  = kTRUE;
      for(int ias = 0; ias < assignlist.size(); ias++) {
	int iasl = assignlist[ias];
	cout << "ASSOCIATION TO CLUS " << iasl << endl;
	std::vector<TMatrixT<double> > cluster2 = clusterlist[iasl];
	std::vector< TMatrixT<double> > toclus1 = FindConnections(singlehit2, cluster);
	std::vector< TMatrixT<double> > toclus2 = FindConnections(singlehit2, cluster2);
	cout << toclus1.size() << " " << toclus2.size() << endl;
	if(toclus1.size() < toclus2.size()) {
	  cout << "HIT " << singlehit2[0][1] << " is not good" << endl;
	  goodhit = kFALSE;
	}
      }
      if(goodhit != kFALSE) {
	it = find(newcluster.begin(), newcluster.end(), singlehit2);
	if(it ==  newcluster.end()) newcluster.push_back(singlehit2); 
	}
    }
    
    connectionslist.clear();
    aggregation = IsAggregation(singlehit, newcluster, connectionslist);
    if(aggregation == kFALSE) {
      it = find(newcluster.begin(), newcluster.end(), singlehit);
      if(it ==  newcluster.end()) newcluster.push_back(singlehit); 
    }
  }
    
 for(int ihit = 0; ihit < cluster.size(); ihit++) {
    TMatrixT<double> singlehit = cluster[ihit];
    if(singlehit[0][0] == -1) continue;
    
    // get its list of connections
    std::vector< TMatrixT<double> > connectionslist;
    Bool_t aggregation = IsAggregation(singlehit, cluster, connectionslist);
    it = find(aggregator.begin(), aggregator.end(), singlehit);
    if(aggregation == kFALSE && it == aggregator.end()) {
      it = find(newcluster.begin(), newcluster.end(), singlehit);
      if(it ==  newcluster.end()) newcluster.push_back(singlehit);
    }
 }
 return newcluster;
}

TVector3 PndSecondaryTrackFinder::ComputePositionAtParallelHit(Int_t hitid, Double_t xc, Double_t yc, Double_t radius, TVector3 &dxyz) {
  
  TVector3 xyz(-999., -999., -999.);
  dxyz.SetXYZ(0., 0., 0.);

  PndSttHit *hit = (PndSttHit*) fSttHitArray->At(hitid);
  if(!hit) return xyz;
  
  Bool_t intersection = IntersectionFinder(xc, yc, radius, hit, xyz, dxyz);
  if(intersection == kFALSE) cout << "ERROR NO INTERSECTION FOUND " << hitid << endl;
//   cout << "INTERSECTION " << xc << " " << yc << " " << radius << endl;
//   xyz.Print();
  return xyz;
}

TVector3 PndSecondaryTrackFinder::ComputeMomentumAtPos(Double_t xc, Double_t yc, Double_t radius, Double_t tanl, Int_t charge, TVector3 position) {

  TVector3 momentum(-999, -999, -999);
  TVector2 center(xc, yc);
  TVector2 pos(position.X(), position.Y());
  TVector2 myrad = center - pos;

  double distance = TMath::Abs(myrad.Mod() - radius);
  if(distance > 0.5) {
    cout << "ComputeMomentumAtPos: POINT NOT ON THE TRACK " << distance << endl;
    return momentum; 
  }

  Double_t rotx, roty;
  if(charge == 0) {
    cout << "ComputeMomentumAtPos: CHARGE = 0!" << endl;
    return momentum;
  }
  rotx = charge * myrad.Y();
  roty = - charge * myrad.X();

  Double_t pt = 0.006 * radius;
  Double_t pl = pt * tanl;
  Double_t ptot = TMath::Sqrt(pt * pt + pl * pl);

  momentum.SetX(rotx);
  momentum.SetY(roty);
  momentum.SetZ(0.);
  momentum.SetMag(pt);
  momentum.SetZ(pl);

  return momentum;


}


Bool_t PndSecondaryTrackFinder::ComputeParametersAtHit(Int_t ihit, std::vector< TMatrixT<double> > cluster, TMatrixT<double> par, TVector3 &position, TVector3 &dposition, TVector3 &momentum) {

  TMatrixT<double> singlehit = cluster[ihit];
  if(singlehit[0][0] != 0) {
    cout << "NOT USABLE HIT!!! " << ihit << endl;
    return kFALSE;
  }
 
  Double_t xc = par[0][0];
  Double_t yc = par[0][1];
  Double_t radius = par[0][2];
  Int_t charge = (Int_t) TMath::Sign(1., par[0][3]);
  Double_t tanl = par[0][4];

  Int_t hitid = (Int_t) singlehit[0][1];
  Int_t detid = (Int_t) singlehit[0][2];
  Int_t isskew = (Int_t) singlehit[0][3];
  // cout << "USABLE HIT " << ihit << " " << hitid << " " << detid << " " << isskew << endl;

  position.SetXYZ(singlehit[0][4], singlehit[0][5], singlehit[0][6]);

  // if it is mvd, gem, or stt skewed keep the positon, else compute it
  if(detid == FairRootManager::Instance()->GetBranchId(fSttBranch) && isskew == 0) {
    TVector3 xyz = ComputePositionAtParallelHit(hitid, xc, yc, radius, dposition);
    if(xyz.X() != -999) position = xyz;
  }

  if(position.X() != -999) momentum = ComputeMomentumAtPos(xc, yc, radius, tanl, charge, position);

  if(position.X() == -999 || momentum.X() == -999) {
    cout << "FAILED" << endl;
    position.Print();
    momentum.Print();
    return kFALSE;
  }
  return kTRUE; 
}


Bool_t PndSecondaryTrackFinder::ComputeFirstParameters(std::vector< TMatrixT<double> > cluster, TMatrixT<double> par, TVector3 &position, TVector3 &dposition, TVector3 &momentum) {
  return ComputeParametersAtHit(0, cluster, par, position, dposition, momentum) ;
}

Bool_t PndSecondaryTrackFinder::ComputeLastParameters(std::vector< TMatrixT<double> > cluster, TMatrixT<double> par, TVector3 &position, TVector3 &dposition, TVector3 &momentum) {
  return ComputeParametersAtHit(cluster.size() - 1, cluster, par, position, dposition, momentum) ;
}


ClassImp(PndSecondaryTrackFinder)


// 1) delta : missing ^2
//	delta = (m * (q - yc) - xc) * (m * (q - yc) - xc) - (m * m + 1) * ((q - yc) * (q - yc) + xc * xc - radius * radius);
// 2) missing sqrt	
//	x1 = (- (m * (q - yc) - xc) + TMath::Sqrt(delta)) / (m * m + 1);
//	x2 = (- (m * (q - yc) - xc) - TMath::Sqrt(delta)) / (m * m + 1);
// 3) CompareToPreviousPhi in ZFit 
