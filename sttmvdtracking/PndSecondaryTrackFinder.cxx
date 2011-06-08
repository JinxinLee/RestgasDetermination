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

#include <iostream>
#include <cmath>
#include <vector>
#include <iterator>
#include <algorithm>


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
    display = new TCanvas("display", "display", 0, 0, 600, 600);
  } 

  if(fDisplayOn) {
    int colors[10] = {2, 3, 4, kMagenta, kBlack, kGray, kViolet, kCyan, kYellow, kOrange};
    for(int icolor = 0; icolor < 10; icolor++) fColors[icolor] = colors[icolor];
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
  cout << "EVENTO with " << nstthits << endl;

  std::vector<int> stthits, sttskewedhits, mvdpixhits, mvdstriphits;
  fDetList.clear();
  fDetMap.clear();
  // stthits.clear();
  cout << "Stt hits" << endl;
  stthits = OrderHits(fSttHitArray, FairRootManager::Instance()->GetBranchId(fSttBranch), false);
  fDetList.push_back(stthits);
  fDetMap[0] =  FairRootManager::Instance()->GetBranchId(fSttBranch);
  cout << "Stt skewed hits" << endl;
  sttskewedhits = OrderHits(fSttHitArray, FairRootManager::Instance()->GetBranchId(fSttBranch), true);
  fDetList.push_back(sttskewedhits);
  fDetMap[1] =  FairRootManager::Instance()->GetBranchId(fSttBranch);
  cout << "mvd pix hits" << endl;
 mvdpixhits = OrderHits(fMvdPixelHitArray, FairRootManager::Instance()->GetBranchId(fMvdPixelBranch), false);
  fDetList.push_back(mvdpixhits);
  fDetMap[2] =  FairRootManager::Instance()->GetBranchId(fMvdPixelBranch);
  cout << "mvd str hits" << endl;
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
  std::vector<std::vector<int> > clusterlist;
  clusterlist = ClusterFinder(stthits,  FairRootManager::Instance()->GetBranchId(fSttBranch));
  cout << "after cluster finding" << endl;
  PrintClusters(clusterlist);
  DrawClusters(clusterlist);

  std::vector<TVector3> xyparameters;
  // GO TO CONFORMAL PLANE FOR FITTING ==================================
  int nclus = clusterlist.size();
  std::vector<int> deletecluster;
  for(int iclus = 0; iclus < nclus; iclus++) {
    std::vector<int> cluster = clusterlist[iclus];
    Double_t xc, yc, radius, chi2; 
    Int_t countelem;
    Bool_t fit = CompleteSttFit(cluster, iclus, xc, yc, radius, chi2, countelem);
    if(fit == kFALSE) {
      cout << "GOTTA DELETE THIS fit fails " << iclus << endl;
      deletecluster.push_back(iclus);
      continue;
    }

    
    Double_t newxc, newyc, newradius, newchi2 = 0;
    std::vector<int> newcluster;
    cout << "TEST CHI2" << endl;
    Bool_t testchi2 = TestChi2(cluster, xc, yc, radius,  FairRootManager::Instance()->GetBranchId(fSttBranch), iclus, chi2, countelem, newxc, newyc, newradius, &newcluster, newchi2);
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
      PrintClusters(clusterlist);
      std::replace(clusterlist.begin(), clusterlist.end(), cluster, newcluster);
      cout << "................. done" << endl;
      PrintClusters(clusterlist);
    }
    // save tracks
    xyparameters.push_back(TVector3(xc, yc, radius));
  }
  //  DrawTracks(xyparameters);

  // DELETE CLUSTER ================================ 
  DeleteCluster(&clusterlist, deletecluster);

  cout << "after replacing and deleting" << endl;
  PrintClusters(clusterlist);
  DrawClusters(clusterlist);


  // NEW HITS COLLECTION  ==============================================
  clusterlist.clear();
  // loop on the tracks
  for(int itrk = 0; itrk < xyparameters.size(); itrk++) {

    TVector3 xypar = xyparameters[itrk];
    double xc = xypar.X();
    double yc = xypar.Y();
    double radius = xypar.Z();
    std::vector<int> cluster = AddPoints(stthits, FairRootManager::Instance()->GetBranchId(fSttBranch), xc, yc, radius, itrk);
    std::vector<int> sorthits = OrderCluster2(cluster, FairRootManager::Instance()->GetBranchId(fSttBranch), xc, yc, radius);
    clusterlist.push_back(sorthits);
  }
  cout << "after adding the points" << endl;
  PrintClusters(clusterlist);
  DrawClusters(clusterlist);
 

  // FIT THE NEW CLUSTERS AGAIN ===========================================
  cout << " NEW FIT " << endl;
  Refresh();
  xyparameters.clear();
  // refit tracks
  for(int iclus = 0; iclus < clusterlist.size(); iclus++) {
    
    std::vector<int> cluster = clusterlist[iclus];

    double xc, yc, radius;
    Double_t firstdrift, delta, trasl[2];
    std::vector< std::vector<double> > conformalhits;  
    Bool_t conftras = ConformalPlaneStt4(cluster, iclus, conformalhits, firstdrift, delta, trasl);
    //    cout << "conform bis " << conftras << endl;
    Double_t conffit = ConformalFit(conformalhits,  iclus, delta,  trasl,  xc,  yc, radius);
    
    if(fDisplayOn) {
      char goOnChar;
      cout << "Go back to reak plane: cluster " << iclus << endl;
      Refresh();
      cout << "helix " << xc << " " << yc << " " << radius << endl;     
      TArc *arc = new TArc(xc, yc, radius);
      arc->SetLineColor(kCyan);
      arc->SetFillStyle(0);
      arc->Draw("SAME ONLY");
      display->Update();
      display->Modified();
    }
    
    xyparameters.push_back(TVector3(xc, yc, radius));
  }

  // 
  Refresh();


  // CLUSTER MERGING =======================================

  std::vector< std::vector<int> > newlist;
  newlist = MergeClusters(clusterlist);
  clusterlist.clear();
  clusterlist = newlist;
  Refresh();
  cout << "after merging" << endl;
  PrintClusters(clusterlist);
  DrawClusters(clusterlist);

  // FIT THE MERGED CLUSTERS ======================================================
  Refresh();
  xyparameters.clear();
  // refit tracks
  for(int iclus = 0; iclus < clusterlist.size(); iclus++) {
    std::vector<int> cluster = clusterlist[iclus];
    std::vector<std::vector<double> > conformalhits;
    Double_t firstdrift, delta, trasl[2];
    // GO TO CONF PLANE
    Bool_t conftras = ConformalPlaneStt4(cluster, iclus, conformalhits, firstdrift, delta, trasl);
    Double_t xc, yc, radius;
    // CONF FIT 1
    Bool_t conffit = ConformalFit(conformalhits,  iclus, delta,  trasl,  xc,  yc, radius);
    // chi2
    int countelem = 0;
    double redchi2 = CalculateRedChi2(cluster,  FairRootManager::Instance()->GetBranchId(fSttBranch), xc, yc, radius, countelem);
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
      Bool_t refit = RefitConformal(cluster, FairRootManager::Instance()->GetBranchId(fSttBranch), tmpxc, tmpyc, tmpradius, outxc[iter], outyc[iter], outradius[iter]);

      red2chi2[iter] = CalculateRedChi2(cluster,  FairRootManager::Instance()->GetBranchId(fSttBranch),  outxc[iter], outyc[iter], outradius[iter], countelem2[iter]);
      cout << "===> RED CHI2 no. " << iter + 1 << " = " << red2chi2[iter] << " " << countelem2[iter] << " <===" << endl;
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

    int tmpredchi2 = redchi2;
    for(int iter = 0; iter < 2; iter++) {
      if(red2chi2[iter] < tmpredchi2) {
	xc = outxc[iter];
	yc = outyc[iter];
	radius = outradius[iter];
	tmpredchi2 = red2chi2[iter];
      }
    }
    xyparameters.push_back(TVector3(xc, yc, radius));
  }






  if(fDisplayOn) {
    fDisName += ".pdf";
    display->SaveAs(fDisName);
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
      //       cout << "press any key" << endl;
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
  // delete the unes already gathered
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
      cout << "comparing " << iclus << " " << jclus << endl;
      position1first.Print();
      position1last.Print();

      std::vector<int> cluster2 = list[jclus];

      FairHit *hit2first = (FairHit*) array->At(cluster2.at(0));
      if(!hit2first) continue;
      TVector3 position2first;
      hit2first->Position(position2first);

      FairHit *hit2last = (FairHit*) array->At(cluster2.at(cluster2.size() - 1));
      if(!hit2last) continue;
      TVector3 position2last;
      hit2last->Position(position2last);

      position2first.Print();
      position2last.Print();

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
    //    cout << "HIT ----> " << hitid << " " << detId << endl;
    if(!hit) { 
      //  cout << "HIT NOT FOUND" << endl; 
      continue; 
    }
    TVector3 position;
    hit->Position(position);
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
	TArc *arc = new TArc(position.X(), position.Y(), tube->GetRadIn()); // ((PndSttHit* ) hit->GetIsochrone()));
	arc->SetLineColor(color);
	arc->SetFillStyle(0);
	arc->Draw("SAME");
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
  char goOnChar;
  cout << "Refresh?" << endl;
  cin >> goOnChar;
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

Bool_t PndSecondaryTrackFinder::ConformalPlane(std::vector<int> cluster, TMatrixT<double> boundaries, std::vector<int> hits, Int_t detId, Int_t iclus) {
  
  int nhits = cluster.size();
  int lasthitid = cluster[0];
  int firsthitid = cluster[nhits - 1];
  
  TClonesArray *array;
  
  if(detId == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) array = fMvdPixelHitArray;
  else if(detId == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) array = fMvdStripHitArray;
  else if(detId ==  FairRootManager::Instance()->GetBranchId(fSttBranch)) array = fSttHitArray;
  //  else if(detId ==  FairRootManager::Instance()->GetBranchId(fGemBranch)) array = fGemHitArray;
 
  
  FairHit *hitfirst = (FairHit*) array->At(firsthitid);
  if(!hitfirst) return kFALSE;
  TVector3 positionfirst;
  hitfirst->Position(positionfirst);

  FairHit *hitlast = (FairHit*) array->At(lasthitid);
  if(!hitlast) return kFALSE;
  TVector3 positionlast;
  hitlast->Position(positionlast);
  
    Double_t trasl[2] = {positionfirst.X(), positionfirst.Y()};

    Double_t  alpha = TMath::ATan2(positionlast.Y() - positionfirst.Y(),
				   positionlast.X() - positionfirst.X());

    

 if(fDisplayOn) {
    char goOnChar;
    cout << "Go to conformal plane: cluster " << iclus << endl;
    cin >> goOnChar;
    cout << "GOING ON" << endl;
    DrawGeometryConformal(-40, -40, 40, 40); 
//     double xmin = boundaries[iclus][0];
//     double ymin = boundaries[iclus][1];
//     double xmax = boundaries[iclus][2];
//     double ymax = boundaries[iclus][3];
//     Double_t x1, x2, y1, y2;
//     // traslation
//     x1 = xmin - trasl[0];
//     y1 = ymin - trasl[1];
//     x2 = xmax - trasl[0];
//     y2 = ymax - trasl[1];
	
//     Double_t xrot1, xrot2, yrot1, yrot2;
//     // rotation
//     xrot1 = TMath::Cos(alpha)*x1 + TMath::Sin(alpha)*y1;
//     yrot1 = -TMath::Sin(alpha)*x1 + TMath::Cos(alpha)*y1;
//     xrot2 = TMath::Cos(alpha)*x2 + TMath::Sin(alpha)*y2;
//     yrot2 = -TMath::Sin(alpha)*x2 + TMath::Cos(alpha)*y2;

//     double u1 = xrot1 / (xrot2 * xrot2 + yrot2 * yrot2);
//     double u2 = xrot2 / (xrot1 * xrot1 + yrot1 * yrot1);
//     double v1 = yrot1 / (xrot2 * xrot2 + yrot2 * yrot2);
//     double v2 = yrot2 / (xrot1 * xrot1 + yrot1 * yrot1);
  
//     double umin, umax, vmin, vmax;
//     if(u1 < u2) {
//       umin = u1; 
//       umax = u2;
//     }
//     else {
//      umin = u2; 
//      umax = u1;
//     }

//     if(v1 < v2) {
//       vmin = v1 - 0.5; 
//       vmax = v2 + 0.5;
//     }
//     else {
//      vmin = v2 - 0.5; 
//      vmax = v1 + 0.5;
//     }


//     DrawGeometryConformal(-10, 10, -10, 10); // umin, vmin, umax, vmax);
   }
  


    Double_t Suu, Su, Sv, Suv, S1, Suuu, Suuv, Suuuu;
    
    Su = 0.;
    Sv = 0.;
    Suu = 0.;
    Suv = 0.;
    Suuu = 0.;
    S1 = 0.;
    Suuv = 0.;
    Suuuu = 0.;
    


    TVector3 fitpoint;

    for(int ihit = 0; ihit < cluster.size(); ihit++)
      {
	Int_t hitid = cluster[ihit];
	
	FairHit *hit = (FairHit*) array->At(hitid);
	if(!hit) continue;

	hit->Position(fitpoint);
	
	Double_t sigx = 0.150; // CHECK
	Double_t sigy = 0.150;

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
	xtrasl = xrot;
	ytrasl = yrot;
	
	// change coordinate
	Double_t u, v, sigv2, sigu2;
	u = xtrasl / (xtrasl*xtrasl + ytrasl*ytrasl);
	v = ytrasl / (xtrasl*xtrasl + ytrasl*ytrasl);
	

	if(fDisplayOn) {

	  std::vector<int>::iterator it;
	  it = find(cluster.begin(), cluster.end(), hitid);
	  
	  TMarker *mrk = new TMarker(u, v, 3);
	  mrk->SetMarkerColor(kBlack);
	  if(it != cluster.end())   mrk->SetMarkerColor(kRed);
	  mrk->Draw("SAME");
	  display->Update();
	  display->Modified();  
	}
 


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

    double outxc = xc;
    double outyc = yc;
    double outradius = R;

}



Bool_t PndSecondaryTrackFinder::ConformalPlaneStt(std::vector<int> cluster, TMatrixT<double> boundaries, std::vector<int> hits, Int_t iclus) {
  
  int nhits = cluster.size();
  int lasthitid = cluster[0];
  int firsthitid = cluster[nhits - 1];
  
  TClonesArray *array = fSttHitArray;
  
  
  FairHit *hitfirst = (FairHit*) array->At(firsthitid);
  if(!hitfirst) return kFALSE;
  TVector3 positionfirst;
  hitfirst->Position(positionfirst);

  FairHit *hitlast = (FairHit*) array->At(lasthitid);
  if(!hitlast) return kFALSE;
  TVector3 positionlast;
  hitlast->Position(positionlast);
  
    Double_t trasl[2] = {positionfirst.X(), positionfirst.Y()};

    Double_t  alpha = TMath::ATan2(positionlast.Y() - positionfirst.Y(),
				   positionlast.X() - positionfirst.X());

    

 if(fDisplayOn) {
    char goOnChar;
    cout << "Go to conformal plane: cluster " << iclus << endl;
    cin >> goOnChar;
    cout << "GOING ON" << endl;
    DrawGeometryConformal(-1, -1, 1, 1); 
   }
  
 TVector3 centerposition;
 
 for(int ihit = 0; ihit < cluster.size(); ihit++)
   {
     Int_t hitid = cluster[ihit];
     if(hitid == firsthitid) continue;

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
     
     Double_t xrot, yrot;
     // rotation
     xrot = TMath::Cos(alpha)*xtrasl + TMath::Sin(alpha)*ytrasl;
     yrot = -TMath::Sin(alpha)*xtrasl + TMath::Cos(alpha)*ytrasl;
     
     // re-traslation
     xtrasl = xrot;
     ytrasl = yrot;
	
     // change coordinate of the center
     Double_t u, v, sigv2, sigu2, rc;
     u = xtrasl / (xtrasl*xtrasl + ytrasl*ytrasl - rd * rd);
     v = ytrasl / (xtrasl*xtrasl + ytrasl*ytrasl - rd * rd);
     rc = rd / (xtrasl*xtrasl + ytrasl*ytrasl - rd * rd);
     
     if(fDisplayOn) {
       
       std::vector<int>::iterator it;
       it = find(cluster.begin(), cluster.end(), hitid);
       
       TArc *arc = new TArc(u, v, rc);
       arc->SetLineColor(kBlack);
       if(it != cluster.end())   arc->SetLineColor(kRed);
       arc->SetFillStyle(0);
       arc->Draw("SAME");
       display->Update();
       display->Modified();  
     }
   }
 
 if(fDisplayOn) {
   TString fConName = fDisName;
   fConName += "_clus"; 
   fConName += iclus;
   fConName += ".pdf";
   display->SaveAs(fConName);
 }
 return kTRUE;
}


Bool_t PndSecondaryTrackFinder::ConformalPlaneStt2(std::vector<int> cluster, TMatrixT<double> boundaries, std::vector<int> hits, Int_t iclus) {
  
  int nhits = cluster.size();
  int lasthitid = cluster[0];
  int firsthitid = cluster[nhits - 1];
  
  TClonesArray *array = fSttHitArray;
  

  if(fDisplayOn) {
    char goOnChar;
    cout << "Go to conformal plane: cluster " << iclus << endl;
    cin >> goOnChar;
    cout << "GOING ON" << endl;
    DrawGeometryConformal(-1, -1, 1, 1); 
   }
  
  TVector3 centerposition;
  TVector2 uvfirst, uvlast;
  Double_t auxinfoparalConformal[nhits - 1][3];

 for(int ihit = 0; ihit < cluster.size(); ihit++)
   {
     Int_t hitid = cluster[ihit];

     PndSttHit *hit = (PndSttHit*) array->At(hitid);
     if(!hit) continue;
     hit->Position(centerposition);
     Double_t rd = hit->GetIsochrone();

     // change coordinate of the center
     Double_t u, v, rc;
     u = centerposition.X() / (centerposition.X()*centerposition.X() + centerposition.Y()*centerposition.Y() - rd * rd);
     v = centerposition.Y() / (centerposition.X()*centerposition.X() + centerposition.Y()*centerposition.Y() - rd * rd);
     rc = rd / (centerposition.X()*centerposition.X() + centerposition.Y()*centerposition.Y() - rd * rd);

     if(hitid != firsthitid) {
       auxinfoparalConformal[ihit][0] = u;
       auxinfoparalConformal[ihit][1] = v;
       auxinfoparalConformal[ihit][2] = rc;
     }
     if(hitid == firsthitid) uvfirst = TVector2(u, v);
     if(hitid == lasthitid)  uvlast = TVector2(u, v);
  
     if(fDisplayOn) {
       
       std::vector<int>::iterator it;
       it = find(cluster.begin(), cluster.end(), hitid);
       
       TArc *arc = new TArc(u, v, rc);
       arc->SetLineColor(kBlack);
       if(it != cluster.end())   arc->SetLineColor(kRed);
       arc->SetFillStyle(0);
       arc->Draw("SAME");
       display->Update();
       display->Modified();  
     }
   } 

 // ---------------------
 // traslation / rotation in conformal plane
 Double_t trasl[2] = {uvfirst.X(), uvfirst.Y()};
 Double_t delta = TMath::ATan2(uvlast.Y() - uvfirst.Y(),
			       uvlast.X() - uvfirst.X());
 // ---------------------
 Double_t m, p, alpha, beta, gamma;
 Bool_t typeConf;  
 Short_t fitting = FitHelixCylinder(nhits - 1,
				    auxinfoparalConformal,
				    delta, trasl,
				    m, p,
				    alpha, beta, gamma,
				    typeConf);
 
 cout << "CONFORMAL FITTING " << fitting << " " << typeConf << endl;
 cout << "ROT " << delta << " TRASL " << trasl[0] << " " << trasl[1] << endl;
 cout << "M/P " << m << " " << p << endl;
 cout  << "ALPHA/BETA/GAMMA " << alpha << " " << beta << " " << gamma << endl;

 if(fDisplayOn) {
   TLine *line = new TLine(-1, -m + p, 1, m + p);
   line->SetLineColor(3);
   line->Draw("SAME");

   TString fConName = fDisName;
   fConName += "_clus"; 
   fConName += iclus;
   fConName += ".pdf";
   display->SaveAs(fConName);
 }




 return kTRUE;
}


Bool_t PndSecondaryTrackFinder::ConformalPlaneStt3(std::vector<int> cluster, TMatrixT<double> boundaries, std::vector<int> hits, Int_t iclus, Double_t &xc, Double_t &yc, Double_t &radius) {
  
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

  cout << "tmp drift " << tmpdrift << endl;

  int lasthitid; //  = cluster[0];
  int firsthitid = tmphitid; // cluster[nhits - 1];
  if(tmphitid == cluster[0]) lasthitid = cluster[nhits - 1];
  else lasthitid = cluster[0];
  
  FairHit *hitfirst = (FairHit*) array->At(firsthitid);
  if(!hitfirst) return kFALSE;
  TVector3 positionfirst;
  hitfirst->Position(positionfirst);

  FairHit *hitlast = (FairHit*) array->At(lasthitid);
  if(!hitlast) return kFALSE;
  TVector3 positionlast;
  hitlast->Position(positionlast);
  
  Double_t trasl[2] = {positionfirst.X(), positionfirst.Y()};

  Double_t  delta = TMath::ATan2(positionlast.Y() - positionfirst.Y(),
				 positionlast.X() - positionfirst.X());

    

  if(fDisplayOn) {
    char goOnChar;
    cout << "Go to conformal plane: cluster " << iclus << endl;
    cin >> goOnChar;
    cout << "GOING ON" << endl;
    DrawGeometryConformal(-1, -1, 1, 1); 
  }
  
  TVector3 centerposition;
  Double_t auxinfoparalConformal[nhits][3];

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
     
      // change coordinate of the center
      Double_t u, v, sigv2, sigu2, rc;
      u = xtrasl / (xtrasl*xtrasl + ytrasl*ytrasl - rd * rd);
      v = ytrasl / (xtrasl*xtrasl + ytrasl*ytrasl - rd * rd);
      rc = rd / (xtrasl*xtrasl + ytrasl*ytrasl - rd * rd);


      if(hitid != firsthitid) {     
	auxinfoparalConformal[ihit][0] = u;
	auxinfoparalConformal[ihit][1] = v;
	auxinfoparalConformal[ihit][2] = rc;
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

  // ---------------------
  Double_t m, p, alpha, beta, gamma;
  Bool_t typeConf;  
  Short_t fitting = FitHelixCylinder(nhits - 1,
				     auxinfoparalConformal,
				     delta, trasl,
				     m, p,
				     alpha, beta, gamma,
				     typeConf);
 
  cout << "CONFORMAL FITTING " << fitting << " " << typeConf << endl;
  cout << "ROT " << delta << " TRASL " << trasl[0] << " " << trasl[1] << endl;
  cout << "M/P " << m << " " << p << endl;
  cout  << "ALPHA/BETA/GAMMA " << alpha << " " << beta << " " << gamma << endl;

  if(fDisplayOn) { 
    // double mnew = (m * TMath::Cos(delta) + TMath::Sin(delta))/(TMath::Cos(delta) - m * TMath::Sin(delta));
    // double pnew = p / (TMath::Cos(delta) - m * TMath::Sin(delta));
    TLine *line = new TLine(-1, -m + p, 1, m + p);
    line->SetLineColor(3);
    line->Draw("SAME");

    TString fConName = fDisName;
    fConName += "_clus"; 
    fConName += iclus;
    fConName += ".pdf";
    display->SaveAs(fConName);
  }

  xc = -alpha / 2.;
  yc = -beta / 2.;
  radius = TMath::Sqrt(- gamma + xc * xc + yc * yc);

  if(fDisplayOn) {
    char goOnChar;
    cout << "Go back to reak plane: cluster " << iclus << endl;
    Refresh();
    cout << "helix " << xc << " " << yc << " " << radius << endl;     
    TArc *arc = new TArc(xc, yc, radius);
    arc->SetLineColor(kGreen);
    arc->SetFillStyle(0);
    arc->Draw("SAME ONLY");
    display->Update();
    display->Modified();
  }
 
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
      chi2 += pow((fabs(distancepc - radius) - rd)/(rderror), 2);
    }
  double redchi2 = chi2 / cluster.size();
  cout << "===> RED CHI2 no. 0 = " << redchi2 << " <===" << endl;

  if(tmpdrift > 0.01) {
    // intersection finder & fit
    for(int iter = 0; iter < 2; iter++) {
      TMatrixT<double> points(cluster.size(), 11);
      for(int ihit = 0; ihit < cluster.size(); ihit++)
	{
	  Int_t hitid = cluster[ihit];
	  PndSttHit *hit = (PndSttHit*) array->At(hitid);
	  if(!hit) continue;
	  Double_t rd = hit->GetIsochrone();
	  TVector3 xyz, dxyz;
	  Bool_t inters = IntersectionFinder(xc, yc, radius, hit, xyz, dxyz);
	  
	  if(fDisplayOn) {
	    TMarker *mrk = new TMarker(xyz.X(), xyz.Y(), 6);  
	    mrk->SetMarkerColor(4);
	    mrk->Draw("SAME");
	    display->Update();
	    display->Modified();
	  }
	  if(inters == kFALSE) continue;
	  
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
      Double_t outxc, outyc, outradius;
      Bool_t fitting2 = Fit(points, outxc, outyc, outradius);
      
      if(fDisplayOn) {
	cout << "refit helix " << outxc << " " << outyc << " " << outradius << endl;     
	TArc *arc2 = new TArc(outxc, outyc, outradius);
	if(iter == 0) arc2->SetLineColor(kRed);
	else arc2->SetLineColor(kBlue);
	
	arc2->SetFillStyle(0);
	arc2->Draw("SAME ONLY");
	display->Update();
	display->Modified();
      }
      xc = outxc;
      yc = outyc;
      radius = outradius;
      
      // chi2
      chi2 = 0;
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
	  
	  chi2 += pow((fabs(distancepc - radius) - rd)/rderror, 2);
	  
	}
      redchi2 = chi2 / cluster.size();
      cout << "===> RED CHI2 no. " << iter + 1 << " = " << redchi2 << " <===" << endl;
    }
  }

  // add points
  for(int ihit = 0; ihit <  hits.size(); ihit++)
    {
      Int_t hitid = hits[ihit];
      std::vector<int>::iterator it;
      it = find(cluster.begin(), cluster.end(), hitid);
      if(it != cluster.end()) continue;
      PndSttHit *hit = (PndSttHit*) array->At(hitid);
      if(!hit) continue;
      TVector3 centerposition2;
      hit->Position(centerposition2);
      Double_t rd = hit->GetIsochrone();
      Double_t rderror = hit->GetIsochroneError();
      Double_t distancepc = TMath::Sqrt((centerposition2.X() - xc) * (centerposition2.X() - xc) +
					(centerposition2.Y() - yc) * (centerposition2.Y() - yc));
      
      double res = fabs(distancepc - radius) - rd;
      cout << "RES " << res << " " << " limit " << 3 * fLimit << endl;
      if(res < (3 * fLimit)) { 
	cout << "ADD " << hitid << " TO CLUSTER" << iclus << endl;
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


//   // the rest of the hits
//   for(int ihit = 0; ihit < hits.size(); ihit++)
//     {
//       Int_t hitid = hits[ihit];
//       cout << "new hit " << hitid << endl;
//       std::vector<int>::iterator it;
//       it = find(cluster.begin(), cluster.end(), hitid);
//       if(it != cluster.end()) continue;   
      
//       PndSttHit *hit = (PndSttHit*) array->At(hitid);
//       if(!hit) continue;
     
//       hit->Position(centerposition);
//       Double_t rd = hit->GetIsochrone();
 
//       Double_t sigx = 0.50; // CHECK
//       Double_t sigy = 0.50;
     
//       // to the fit ================================
//       Double_t xtrasl, ytrasl;
//       // traslation
//       xtrasl = centerposition.X() - trasl[0];
//       ytrasl = centerposition.Y() - trasl[1];
     
//       // change coordinate of the center
//       Double_t u, v, sigv2, sigu2, rc;
//       u = xtrasl / (xtrasl*xtrasl + ytrasl*ytrasl - rd * rd);
//       v = ytrasl / (xtrasl*xtrasl + ytrasl*ytrasl - rd * rd);
//       rc = rd / (xtrasl*xtrasl + ytrasl*ytrasl - rd * rd);
//       cout << "other hits " << hitid << " " << u << " " << v << " " << rc << endl;
//       if(fDisplayOn) {
//        	TArc *arc = new TArc(u, v, rc);
//  	arc->SetLineColor(kBlack);
//  	arc->SetFillStyle(0);
// 	arc->Draw("SAME");
//  	display->Update();
//  	display->Modified();  
//        }
//     } 
//   // -----------

  return kTRUE;
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

  cout << "tmp drift " << tmpdrift << endl;
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
    cout << "Go to conformal plane: cluster " << iclus << endl;
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
 
  cout << "CONFORMAL FITTING " << fitting << " " << typeConf << endl;
  cout << "ROT " << delta << " TRASL " << trasl[0] << " " << trasl[1] << endl;
  cout << "M/P " << m << " " << p << endl;
  cout  << "ALPHA/BETA/GAMMA " << alpha << " " << beta << " " << gamma << endl;
  if(fitting != 1 || typeConf != true) return kFALSE;

  if(fDisplayOn) { 
    // double mnew = (m * TMath::Cos(delta) + TMath::Sin(delta))/(TMath::Cos(delta) - m * TMath::Sin(delta));
    // double pnew = p / (TMath::Cos(delta) - m * TMath::Sin(delta));
    TLine *line = new TLine(-1, -m + p, 1, m + p);
    line->SetLineColor(3);
    line->Draw("SAME");
    display->Update();
    display->Modified();  
    
    TString fConName = fDisName;
    fConName += "_clus"; 
    fConName += iclus;
    fConName += ".pdf";
    display->SaveAs(fConName);
  }

  xc = -alpha / 2.;
  yc = -beta / 2.;
  radius = TMath::Sqrt(- gamma + xc * xc + yc * yc);

  if(fDisplayOn) {
    char goOnChar;
    cout << "Go back to reak plane: cluster " << iclus << endl;
    Refresh();
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
      cout << "element " <<  element  << endl;
    }
  double redchi2 = chi2 / cluster.size();
  // cout << "===> RED CHI2 no. 0 = " << redchi2 << " <===" << endl;
  return redchi2;
}

Bool_t PndSecondaryTrackFinder::RefitConformal(std::vector<int> cluster, Int_t detId, Double_t xc, Double_t yc, Double_t radius,
					       Double_t &outxc, Double_t &outyc, Double_t &outradius)

{
  TClonesArray *array;
 
  if(detId == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) array = fMvdPixelHitArray;
  else if(detId == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) array = fMvdStripHitArray;
  else if(detId ==  FairRootManager::Instance()->GetBranchId(fSttBranch)) array = fSttHitArray;
  //  else if(detId ==  FairRootManager::Instance()->GetBranchId(fGemBranch)) array = fGemHitArray;

  // intersection finder & fit
  TMatrixT<double> points(cluster.size(), 11);
  for(int ihit = 0; ihit < cluster.size(); ihit++)
    {
      Int_t hitid = cluster[ihit];
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

std::vector<int> PndSecondaryTrackFinder::AddPoints(std::vector<int> hits, Int_t detId,  Double_t xc, Double_t yc, Double_t radius, int iclus)
{

  cout << "==========> ADD POINT <=========" << endl;

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
	 	cout << "ADD " << hitid << " TO CLUS " << iclus << endl;
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
    cout << "-------------------> " << iclus << endl;
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
      cout << iclus << " " << jclus << " " << percij << " " << percji << " " << combination << endl;    

      if(perc > 0.5) {
	cout << "PERC " << iclus << " " << jclus << " " << perc << endl;
	bool done = false;
	for(int icom = 0; icom < combinations.size(); icom++) {
	  std::vector<int> knowncombination = combinations[icom];
	  std::vector<int>::iterator it, itj;
	  
	  for(int kclus = 0; kclus < knowncombination.size(); kclus++) {
	    cout << "checking combi " << icom << " clus " << knowncombination.at(kclus) << endl;
	    it = find(knowncombination.begin(), knowncombination.end(), iclus);
	    itj = find(knowncombination.begin(), knowncombination.end(), jclus);
	    if(it != knowncombination.end() || itj != knowncombination.end()) {
	      if(it == knowncombination.end()) knowncombination.push_back(iclus);
	      if(itj == knowncombination.end()) knowncombination.push_back(jclus);
	      cout << "update knowncombination " << icom << " " << iclus << " " << jclus << endl;
	      done = true;
	      break;
	    }
	  }
	  if(done == true) {
	    std::replace(combinations.begin(), combinations.end(), combinations[icom], knowncombination);
	    cout << "update knowncombinations " << knowncombination.size() << endl; 
}

	}
	if(false == done) {
	  cout << "newcombination " << iclus << " " << jclus << endl;
	  std::vector<int> newcombination;
	  newcombination.push_back(iclus);
	  newcombination.push_back(jclus);
	  combinations.push_back(newcombination);
	}
      }
    }
  }

  cout << "combination size " <<  combinations.size() << endl;

  std::vector< std::vector<int> > newlist;
  std::vector<int> usedclusters;

  for(int icom = 0; icom < combinations.size(); icom++) {
    std::vector<int> knowncombination = combinations[icom];
    std::vector<int> newcluster;
    cout << "combination " << icom << ": ";
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
    cout << endl;
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


void PndSecondaryTrackFinder::DrawClusters(std::vector< std::vector<int> > clusterlist) {
  for(int iclus = 0; iclus < clusterlist.size(); iclus++) {
    std::vector<int> cluster = clusterlist[iclus];
    DrawHitsColor(cluster, FairRootManager::Instance()->GetBranchId(fSttBranch), fColors[iclus]);
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
    Refresh();
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
      if(red2chi2[iter] < chosenchi2) {
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

Bool_t PndSecondaryTrackFinder::TestChi2(std::vector<int> cluster, Double_t xc, Double_t yc, Double_t radius,  Int_t detId, Int_t iclus, Double_t chi2, Int_t countelem, Double_t &newxc, Double_t &newyc, Double_t &newradius, std::vector<int> *newcluster, Double_t &newchi2) {
  if(chi2 < fChi2Limit) return kTRUE;

  if(countelem > fCountElemLimit) return kFALSE;

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



ClassImp(PndSecondaryTrackFinder)
