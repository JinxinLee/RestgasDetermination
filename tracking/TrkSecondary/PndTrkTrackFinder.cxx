////////////////////////////////////////////////////////////
//
// PndTrkTrackFinder
// 
// Class for secondary track pattern recognition
//
// authors: Lia Lavezzi - University of Torino (2014)
//                                   
////////////////////////////////////////////////////////////

#include "PndTrkTrackFinder.h"

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
PndTrkTrackFinder::PndTrkTrackFinder() : FairTask("secondary track finder", 0), fDisplayOn(kFALSE), fPersistence(kTRUE), fUseMVDPix(kTRUE), fUseMVDStr(kTRUE), fUseSTT(kTRUE), fUseSCIT(kTRUE), fUseGEM(kTRUE), fSecondary(kFALSE), fMvdPix_RealDistLimit(1000), fMvdStr_RealDistLimit(1000), fStt_RealDistLimit(1000), fMvdPix_ConfDistLimit(1000), fMvdStr_ConfDistLimit(1000), fStt_ConfDistLimit(1000), fInitDone(kFALSE), fUmin(-0.07), fUmax(0.07), fVmin(-0.07), fVmax(0.07), fRmin(-1.5), fRmax(1.5), fThetamin(0), fThetamax(180), fRefHit(NULL) {
  sprintf(fSttBranch,"STTHit");
  sprintf(fMvdPixelBranch,"MVDHitsPixel");
  sprintf(fMvdStripBranch,"MVDHitsStrip");
  sprintf(fSciTBranch,"SciTHit");
  sprintf(fGemBranch,"GEMHit");
  PndGeoHandling::Instance();
}

PndTrkTrackFinder::PndTrkTrackFinder(int verbose) : FairTask("secondary track finder", verbose), fDisplayOn(kFALSE), fPersistence(kTRUE), fUseMVDPix(kTRUE), fUseMVDStr(kTRUE), fUseSTT(kTRUE), fUseSCIT(kTRUE), fUseGEM(kTRUE), fSecondary(kFALSE), fMvdPix_RealDistLimit(1000), fMvdStr_RealDistLimit(1000), fStt_RealDistLimit(1000), fMvdPix_ConfDistLimit(1000), fMvdStr_ConfDistLimit(1000), fStt_ConfDistLimit(1000), fInitDone(kFALSE), fUmin(-0.07), fUmax(0.07), fVmin(-0.07), fVmax(0.07), fRmin(-1.5), fRmax(1.5), fThetamin(0), fThetamax(180), fRefHit(NULL) {
  sprintf(fSttBranch,"STTHit");
  sprintf(fMvdPixelBranch,"MVDHitsPixel");
  sprintf(fMvdStripBranch,"MVDHitsStrip");
  sprintf(fSciTBranch,"SciTHit");
  sprintf(fGemBranch,"GEMHit");
  PndGeoHandling::Instance();
}

// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndTrkTrackFinder::~PndTrkTrackFinder() { 

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
  
  delete legendre;

  delete conform;
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
InitStatus PndTrkTrackFinder::Init() {
  
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
    cout << "-E- PndTrkTrackFinder::Init: "
	 << "RootManager not instantiated, return!" << endl;
    return kFATAL;
  }

  // -- HITS -------------------------------------------------
  //
  // STT
  fSttHitArray = (TClonesArray*) ioman->GetObject(fSttBranch);
  if ( ! fSttHitArray ) {
    cout << "-W- PndTrkTrackFinder::Init: "
	 << "No STTHit array, return!" << endl;
    return kERROR;
  }
  //
  // MVD PIXEL
  fMvdPixelHitArray = (TClonesArray*) ioman->GetObject(fMvdPixelBranch);
  if ( !fMvdPixelHitArray){
    std::cout << "-W- PndTrkTrackFinder::Init: " << "No MVD Pixel hitArray, return!" << std::endl;
    return kERROR;
  }
  //
  // MVD STRIP
  fMvdStripHitArray = (TClonesArray*) ioman->GetObject(fMvdStripBranch);
  if ( !fMvdStripHitArray){
    std::cout << "-W- PndTrkTrackFinder::Init: " << "No MVD Strip hitArray, return!" << std::endl;
    return kERROR;
  }
  //
  // SciT 
  fSciTHitArray = (TClonesArray*) ioman->GetObject(fSciTBranch);
  if ( !fSciTHitArray){
    std::cout << "-W- PndTrkTrackFinder::Init: " << "No SciT hitArray, return!" << std::endl;
    return kERROR;
  }
  //
  // GEM
  fGemHitArray = (TClonesArray*) ioman->GetObject(fGemBranch);
  if ( !fGemHitArray){
    std::cout << "-W- PndTrkTrackFinder::Init: " << "No GEM hitArray, return!" << std::endl;
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

void PndTrkTrackFinder::SetParContainers() {
  FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}
 
// -------------------------------------------------------------------------
 

void PndTrkTrackFinder::Initialize() {
  
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


void PndTrkTrackFinder::Exec(Option_t* opt)  {
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

  fDisplayOn = kFALSE;
  // ##########################################################
  
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // L      OOO  N   N  GGG
  // L     O   O NN  N G   
  // L     O   O N N N G  GG
  // L     O   O N  NN G   G
  // LLLLL  OOO  N   N  GGG 

  PndTrkHit *stthit = NULL;
  TObjArray indiv;
  // calculate the indivisible parallel hits and
  // their center of mass
  //  std::vector< PndTrkIndivisibleHit > listofindhits;
  double numx, numy, den;
  for(int ihit = 0; ihit < stthitlist->GetNofHits(); ihit++) {
    stthit = stthitlist->GetHit(ihit);
    if(stthit->IsSttSkew()) continue;
    numx = stthit->GetPosition().X() * stthit->GetIsochrone();
    numy = stthit->GetPosition().Y() * stthit->GetIsochrone();
    den = stthit->GetIsochrone();
    indiv = fHitMap->GetIndivisiblesToHit(stthit);
    if(indiv.GetEntriesFast() == 0) continue;
    int nind = indiv.GetEntriesFast();
    if(nind == 0) continue;
    
    TArrayI indhitids(nind + 1);
    indhitids[0] = stthit->GetHitID();
    int counter = 1;
    for(int jhit = 0; jhit < nind; jhit++) {
      PndTrkHit *stthit2 = (PndTrkHit*) indiv.At(jhit);
      if(stthit2->IsSttSkew()) continue;
      numx += stthit2->GetPosition().X() * stthit2->GetIsochrone();
      numy += stthit2->GetPosition().Y() * stthit2->GetIsochrone();
      den += stthit2->GetIsochrone();
      indhitids[counter] = stthit2->GetHitID();
      counter++;
    }
    numx/=den;
    numy/=den;
    
    // create combined hit
    TVector3 indpos(numx, numy, 0.);
    PndTrkIndivisibleHit indhit(indhitids, indpos);
    //    cout << "indhit " << indhit.GetPosition().X() << " " << indhit.GetPosition().Y() << " " << fIndivisibleHitList->GetNofHits() << endl;
    //    listofindhits.push_back(indhit);
    //    listofindhits.at(listofindhits.size() -1).GetPosition().Print();
    fIndivisibleHitList->AddHit(&indhit);
  }
 
  // +++++++++++++++++++++++++++++

  std::map< int, std::vector< int > > maplay2hits;
  PndTrkHit *hit = NULL;
  for(int ihit = 0; ihit < stthitlist->GetNofHits(); ihit++) {
    hit = stthitlist->GetHit(ihit);
    PndSttTube *tube = (PndSttTube*) fTubeArray->At(hit->GetTubeID());
    int layerID = tube->GetLayerID();
    std::map< int, std::vector< int > >::iterator it = maplay2hits.begin();
    it = maplay2hits.find(layerID);
    if(it == maplay2hits.end()) {
      std::vector< int > hits;
      hits.push_back(ihit);
      maplay2hits[layerID] = hits;
      //   maplay2hits.insert(std::pair<int, std::vector<int> >(layerID, hits));
      std::vector< int > hits2 = maplay2hits[layerID];
    }
    else {
      std::vector< int > hits = maplay2hits[layerID];
      hits.push_back(ihit);
      maplay2hits[layerID] = hits;
    }
  }


  for(int ilay = 0; ilay < maplay2hits.size(); ilay++) {
    std::vector< int > hits = maplay2hits[ilay];
    //    cout << "layer " << ilay << " has nof hits " << hits.size() << endl;
  }

  std::vector< std::vector < int > > trackcandidates;
  // first inner - last inner layers ------------------------------------------------------------ 
  // couples
  // CUTS:
  // xy distance < 10 cm
  // cosalpha < 0.94

  std::vector< int > hits0 =  maplay2hits[0];
  for(int ihit = 0; ihit < hits0.size(); ihit++) {
    std::vector< int > hits7 =  maplay2hits[7];
    for(int jhit = 0; jhit < hits7.size(); jhit++) {
      // cut on distance
      PndTrkHit *hiti = stthitlist->GetHit(hits0[ihit]);
      PndTrkHit *hitj = stthitlist->GetHit(hits7[jhit]);
      double distance = hiti->GetXYDistance(hitj);
      // cout << hits0[ihit] << " and " << hits7[jhit];
      if(distance > 10) { 
	// cout << " too distant " << distance << endl; 
	continue;
      }

      // cut on angle
      TVector2 hitipos = hiti->GetPosition().XYvector();
      TVector2 hitjpos = hitj->GetPosition().XYvector();
      double cosalpha = hitipos * hitjpos / (hitipos.Mod() * hitjpos.Mod());
      if(cosalpha < 0.94) { 
	// cout << " have bad cosine " << cosalpha << endl; 
	continue;
      }
      
      //      cout << " are fine! " << cosalpha << endl;

 //      if(fDisplayOn)  {
// 	char goOnChar;
// 	hiti->Draw(kRed);
// 	hitj->Draw(kOrange);
// 	display->Update();
// 	display->Modified();
// 	//	cin >> goOnChar;
// 	Refresh();
// 	display->Update();
// 	display->Modified();
//       }

      std::vector< int > couple;
      couple.push_back(hits0[ihit]);
      couple.push_back(hits7[jhit]);
      trackcandidates.push_back(couple);
    }
  }

  //  cout << endl;
  // triplets
  std::vector< std::vector < int > > trackcandidates2;
  for(int ipair = 0; ipair < trackcandidates.size(); ipair++) {
    std::vector< int > couple = trackcandidates[ipair];
    
    std::vector< int > hits16 =  maplay2hits[16];
    for(int jhit = 0; jhit < hits16.size(); jhit++) {
      PndTrkHit *hit0 = stthitlist->GetHit(couple[0]);
      PndTrkHit *hit1 = stthitlist->GetHit(couple[1]);
      
      PndTrkHit *hit16 = stthitlist->GetHit(hits16[jhit]);
      double distance = hit1->GetXYDistance(hit16);
      // cout << couple[0] << ", " << couple[1] <<  " and " << hits16[jhit];
      if(distance > 15) { 
	// cout << " too distant " << distance << endl; 
	continue; }
      
      // cut on angle
      TVector2 hit1_0pos = hit1->GetPosition().XYvector() - hit0->GetPosition().XYvector();
      TVector2 hit1_16pos = hit16->GetPosition().XYvector() - hit1->GetPosition().XYvector();
      double cosalpha = hit1_0pos *  hit1_16pos/ (hit1_0pos.Mod() * hit1_16pos.Mod());
      if(cosalpha < 0.94) { 
	// cout << " have bad cosine " << cosalpha << endl; 
	continue; }
      
      //      cout << " are fine! " << cosalpha << endl;
      
      std::vector< int > triplet;
      triplet.push_back(couple[0]);
      triplet.push_back(couple[1]);
      triplet.push_back(hits16[jhit]);
      
      trackcandidates2.push_back(triplet);
    }
  }

  // fourth hit
  std::vector< std::vector< int > >  trackcandidates3;
  for(int iqua = 0; iqua < trackcandidates2.size(); iqua++) {
    std::vector< int > triplet = trackcandidates2[iqua];
    
    PndTrkHit *hit0 = stthitlist->GetHit(triplet[0]);
    PndTrkHit *hit1 = stthitlist->GetHit(triplet[1]);
    PndTrkHit *hit2 = stthitlist->GetHit(triplet[2]);
    //  cout << "TRIPLET: " << triplet[0] << ", " << triplet[1] <<  " and " << triplet[2] << endl;

    std::vector< int > hits20 =  maplay2hits[20];
    for(int jhit = 0; jhit < hits20.size(); jhit++) {
      PndTrkHit *hit20 = stthitlist->GetHit(hits20[jhit]);
      double distance = hit2->GetXYDistance(hit20);
      // cout << triplet[0] << ", " << triplet[1] <<  ", " << triplet[2] << " and " << hits20[jhit];
      if(distance > 15) { 
	// cout << " too distant " << distance << endl; 
	continue; }
      
      // cut on angle
      TVector2 hit2_1pos = hit2->GetPosition().XYvector() - hit1->GetPosition().XYvector();
      TVector2 hit2_20pos = hit20->GetPosition().XYvector() - hit1->GetPosition().XYvector();
      double cosalpha = hit2_1pos *  hit2_20pos/ (hit2_1pos.Mod() * hit2_20pos.Mod());
      if(cosalpha < 0.94) {
	// cout << " have bad cosine " << cosalpha << endl; 
	continue; }
      
      //      cout << " are fine! " << cosalpha << endl;
      
      std::vector< int > quadriplet;
      quadriplet.push_back(triplet[0]);
      quadriplet.push_back(triplet[1]);
      quadriplet.push_back(triplet[2]);
      quadriplet.push_back(hits20[jhit]);
      
      trackcandidates3.push_back(quadriplet);
    }
  }



  // circle through three points
  // find the tracks
  std::vector < std::vector < double > > tracks; // x0, y0, R
  std::vector< std::vector< int > >  trackcandidates4;
  for(int iqua = 0; iqua < trackcandidates3.size(); iqua++) {
    std::vector< int > quadriplet = trackcandidates3[iqua];
    PndTrkHit *hit0 = stthitlist->GetHit(quadriplet[0]);
    PndTrkHit *hit1 = stthitlist->GetHit(quadriplet[1]);
    PndTrkHit *hit2 = stthitlist->GetHit(quadriplet[2]);
    PndTrkHit *hit3 = stthitlist->GetHit(quadriplet[3]);

//     TH3F *hapollonius = new TH3F("hapollonius", "Apollonius field", 100, -500, 500, 100, -500, 500, 100, 0., 400);
//     std::vector< double > X1, Y1, R1;
//     Apollonius(hit0, hit1, hit2, X1, Y1, R1);
//     for(int itrk = 0; itrk < X1.size(); itrk++) hapollonius->Fill(X1[itrk], Y1[itrk], R1[iitrk]);
//     std::vector< double > X2, Y2, R2;
//     Apollonius(hit0, hit1, hit3, X2, Y2, R2);
//     for(int itrk = 0; itrk < X2.size(); itrk++) hapollonius->Fill(X2[itrk], Y2[itrk], R2[itrk]);
//     std::vector< double > X3, Y3, R3;
//     Apollonius(hit3, hit1, hit2, X3, Y3, R3);
//     for(int itrk = 0; itrk < X3.size(); itrk++) hapollonius->Fill(X3[itrk], Y3[itrk], R3[itrk]);

    double x01, y01, rad1;
    CircleBy3Points(hit0, hit1, hit2, x01, y01, rad1);
    double x02, y02, rad2;
    CircleBy3Points(hit0, hit1, hit3, x02, y02, rad2);
    double x03, y03, rad3;
    CircleBy3Points(hit3, hit1, hit2, x03, y03, rad3);
   

    cout << endl;
    hit0->GetPosition().Print();
    cout << hit0->GetIsochrone() << " " << hit0->GetPosition().Mag() << endl;
    hit1->GetPosition().Print();
    cout << hit1->GetIsochrone() <<  " " << hit1->GetPosition().Mag() << endl;
    hit2->GetPosition().Print();
    cout << hit2->GetIsochrone() <<  " " << hit2->GetPosition().Mag() << endl;
    hit3->GetPosition().Print();
    cout << hit3->GetIsochrone() <<  " " << hit3->GetPosition().Mag() << endl;
 
    cout << "dist " << hit0->GetXYDistance(hit1) << endl;
    cout << "dist " << hit1->GetXYDistance(hit2) << endl;
    cout << "dist " << hit2->GetXYDistance(hit3) << endl;


     double xm = 0, ym = 0, rm = 0;
      int positive = 0, negative = 0;
      if(x01 > 0) positive++; 
      else negative++;
      if(x02 > 0) positive++;
      else negative++;
      if(x03 > 0) positive++;
      else negative++;
      bool posit = false;
      positive > negative ? posit = true : posit = false;
      int count = 0;
      if(posit == true) {
	if(x01 > 0)  { xm += x01; ym += y01; rm += rad1; count++; }
	if(x02 > 0)  { xm += x02; ym += y02; rm += rad2; count++; } 
	if(x03 > 0)  { xm += x03; ym += y03; rm += rad3; count++; }
      }
      else {
	if(x01 < 0)  { xm += x01; ym += y01; rm += rad1; count++; }
	if(x02 < 0)  { xm += x02; ym += y02; rm += rad2; count++; } 
	if(x03 < 0)  { xm += x03; ym += y03; rm += rad3; count++; }
      }

      xm /= count;
      ym /= count;
      rm /= count;

      cout << posit << " " << count << " " << (x01 + x02 + x03)/count << endl;
      cout << "01 " << x01 << " " << y01 << " " << rad1 << endl;
      cout << "02 " << x02 << " " << y02 << " " << rad2 << endl;
      cout << "03 " << x03 << " " << y03 << " " << rad3 << endl;

      
      //  cout << quadriplet[0] << " " << quadriplet[1] << " " << quadriplet[2] << " " << quadriplet[3] << " MEAN " << xm << " " << ym << " " << rm << endl;
      
      if(fDisplayOn)  {
	char goOnChar;
	// //       display->cd(2);
	//       hapollonius->Draw("colz");
	//       display->cd(1);
	//       Refresh();
	//       hit0->Draw(kOrange);
	//       hit1->Draw(kOrange);
	//       hit2->Draw(kOrange);
	//      for(int itrk = 0; itrk < X1.size(); itrk++) {
	// 	TArc *arc1 = new TArc(X1[itrk], Y1[itrk], R1[itrk]);
	// 	arc1->SetFillStyle(0);
	// 	//	arc1->Draw("SAME");
	//       }
	//       for(int itrk = 0; itrk < X2.size(); itrk++) {
	// 	TArc *arc2 = new TArc(X2[itrk], Y2[itrk], R2[itrk]);
	// 	arc2->SetFillStyle(0);
	// 	//	arc2->Draw("SAME");
	//       }
	//       for(int itrk = 0; itrk < X3.size(); itrk++) {
	// 	TArc *arc3 = new TArc(X3[itrk], Y3[itrk], R3[itrk]);
	// 	arc3->SetFillStyle(0);
	// 	//	arc3->Draw("SAME");
	//       }
	Refresh();
	// 	TArc *arc1 = new TArc(x01, y01, rad1);
	// 	arc1->SetFillStyle(0);
	// 	arc1->SetLineColor(1);
	// 	arc1->Draw("SAME");
	// 	TArc *arc2 = new TArc(x02, y02, rad2);
	// 	arc2->SetFillStyle(0);
	// 	arc2->SetLineColor(2);
	// 	arc2->Draw("SAME");
	// 	TArc *arc3 = new TArc(x03, y03, rad3);
	// 	arc3->SetFillStyle(0);
	// 	arc3->SetLineColor(4);
	// 	arc3->Draw("SAME");
	
	TArc *arcm = new TArc(xm, ym, rm);
	arcm->SetFillStyle(0);
	arcm->SetLineColor(3);
	arcm->Draw("SAME");
	display->Update();
	display->Modified();
	cout << "next quadriplet?" << endl;    
	cin >> goOnChar;
      }


      std::vector< double > track;
      track.push_back(xm);
      track.push_back(ym);
      track.push_back(rm);

      // suppress identical clones
      std::vector< std::vector < double > >::iterator itr = tracks.begin();
      itr = std::find(tracks.begin(), tracks.end(), track);
      if(itr == tracks.end()) {
	//	cout << "miss" << endl;

	// request 10 < rm < 2500
	// . 10 because it touches layers 0 and 20 -->
	// minimum diameter = 20 * 1 cm = 20 cm --> rm > 10
	// . 2500 [cm] = 15 [GeV/c] / 0.006
	if(rm > 10 && rm < 2500) { 
	  tracks.push_back(track);
	  trackcandidates4.push_back(quadriplet);
	}
      }
      //      else cout << "already" << endl;
  }
  //  cout << "nof tracks reduced from " << trackcandidates3.size() << " to " << trackcandidates4.size() << endl;
  PndTrkClusterList clusterlist;
  
  for(int itrk = 0; itrk < tracks.size(); itrk++) {
    std::vector< double > track = tracks[itrk];
    double x = track[0];
    double y = track[1];
    double r = track[2];
    //    cout << x << " " << y << " " << r << endl;
    if(fDisplayOn)  {
      char goOnChar;
      display->cd(1);
      TArc *arcm = new TArc(x, y, r);
      arcm->SetFillStyle(0);
      arcm->SetLineColor(3);
      arcm->Draw("SAME");
      display->Update();
      display->Modified();
    }

    // find sector
    std::vector < int > quadriplet = trackcandidates4[itrk];
    PndTrkHit *hit0 = stthitlist->GetHit(quadriplet[0]);
    PndSttTube *tube0 = (PndSttTube*) fTubeArray->At(hit0->GetTubeID());
    PndTrkHit *hit1 = stthitlist->GetHit(quadriplet[1]);
    PndSttTube *tube1 = (PndSttTube*) fTubeArray->At(hit1->GetTubeID());
    PndTrkHit *hit2 = stthitlist->GetHit(quadriplet[2]);
    PndSttTube *tube2 = (PndSttTube*) fTubeArray->At(hit2->GetTubeID());
    PndTrkHit *hit3 = stthitlist->GetHit(quadriplet[3]);
    PndSttTube *tube3 = (PndSttTube*) fTubeArray->At(hit3->GetTubeID());

    std::map< int, int > sectorids;
    int sec[4];
    sec[0] = tube0->GetSectorID();
    sec[1] = tube1->GetSectorID();
    sec[2] = tube2->GetSectorID();
    sec[3] = tube3->GetSectorID();
    for(int isec = 0; isec < 4; isec++) {
      if(sectorids.find(sec[isec]) == sectorids.end()) sectorids[sec[isec]] = 1;
      else sectorids[sec[isec]]++;
      //      cout << "settori " << sec[0] << " " << sec[1] << " " << sec[2] << " " << sec[3] << endl; 

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
    //      cout << "sector " << sectorID << " at border? " << border << " " << othersecID << endl;

    // === CREATE CLUSTER
    // in the same sector or 
    // nearby if at the limit of two sectors <<<<<<<<<<<<------------------
    PndTrkCluster cluster;
    
    // create cluster depending on fitting
    double rmin = r - r * 0.05; // CHECK 5%?
    double rmax = r + r * 0.05; // "      "
  
    // create cluster depending on fitting
    for(int ihit = 0; ihit < stthitlist->GetNofHits(); ihit++) {
      hit = stthitlist->GetHit(ihit);
      if(hit->IsSttParallel() == kFALSE) continue;
      PndSttTube *tube = (PndSttTube*) fTubeArray->At(hit->GetTubeID());
      if(border == false && tube->GetSectorID() != sectorID) continue;
      else if(border == true && (tube->GetSectorID() != sectorID && tube->GetSectorID() != othersecID)) continue;
      
      double distance = hit->GetXYDistance(TVector3(x, y, 0.));
      
      //    if(distance <= rmax && distance >= rmin) {
      //	cluster.AddHit(hit);
      //      }

      double distance_hit_center = (hit->GetPosition().XYvector() - TVector2(x, y)).Mod();
      double recoiso = fabs(distance_hit_center - r);

      //    hit->GetPosition().Print();
      //       TVector2(x, y).Print();
      //  cout << r << " " << hit->GetIsochrone() << endl;

      //   cout << "recoiso " << recoiso << endl;
      if(recoiso < 1.) cluster.AddHit(hit);

    }
    if(fDisplayOn) {
      display->cd(1);
      cluster.Draw(kRed);
      display->Update();
      display->Modified();
      char goOnChar;
      cout << "want to go on?" << endl;
      cin >> goOnChar;
    } 

    // get indivisible hits
    PndTrkCluster indcluster;
    //    cout << "indivisibles " << fIndivisibleHitList->GetNofHits() << endl;
    if(fDisplayOn) {
      Refresh();
    }
    for(int ihit = 0; ihit < fIndivisibleHitList->GetNofHits(); ihit++) {
      PndTrkIndivisibleHit *indhit = (PndTrkIndivisibleHit*) fIndivisibleHitList->GetHit(ihit);
      double distance_hit_center = (indhit->GetPosition().XYvector() - TVector2(x, y)).Mod();
      double recoiso = fabs(distance_hit_center - r);
      // cout << "recoiso indhit " << recoiso << endl;
      if(recoiso < 1.) {
	indcluster.AddHit(indhit);
	if(fDisplayOn) {
	  //	  cout << "recoiso indhit " << indhit->GetPosition().X() << " " << indhit->GetPosition().Y()  << endl;
	  indhit->Draw(kRed);
	}
      }
    }
    if(fDisplayOn) {
      display->Update();
      display->Modified();
      char goOnChar;
      cin >> goOnChar;
    }
  
    // conformal map on last indivisible hit
    double tmpdistance = -1;
    int tmphitID = -1;
    for(int ihit = 0; ihit < indcluster.GetNofHits(); ihit++) {
      PndTrkIndivisibleHit *indhit2 = (PndTrkIndivisibleHit*) indcluster.GetHit(ihit);
      TVector3 position = indhit2->GetPosition();
      if(position.Perp() > tmpdistance) {
	tmpdistance = position.Perp();
	tmphitID = ihit;
      }
    }

    if(tmphitID == -1) continue; // CHECK what if there is no indivisible hit??
    //    cout << "nof indivisible hits " << indcluster.GetNofHits() << " " << tmphitID << endl;
    // set up conformal map
    fConformalHitList->Clear("C");
    Double_t delta = 0, trasl[2] = {0., 0.};
    PndTrkHit *refhit = indcluster.GetHit(tmphitID);
    ComputeTraAndRot(refhit, delta, trasl);
    conform->SetOrigin(trasl[0], trasl[1], delta);
    fConformalHitList->SetConformalTransform(conform);

    // fill conformal hits
    Int_t nofconfhits = FillConformalHitList(&cluster);


    // compute conformal plane extremities ----------------------------
    fUmin =  1000, fVmin =  1000, fRmin =  1000;
    fUmax = -1000, fVmax = -1000, fRmax = -1000;
    double rc_of_min, rc_of_max;
    
    for(int jhit = 0; jhit < fConformalHitList->GetNofHits(); jhit++) {
      PndTrkConformalHit *chit = fConformalHitList->GetHit(jhit);
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
 
    if(fDisplayOn) {
      DrawGeometryConf(fUmin, fUmax, fVmin, fVmax);
      // ----------------------------------------------------------------------
      for(int ihit = 0; ihit < nofconfhits; ihit++) {
	PndTrkConformalHit *chit = fConformalHitList->GetHit(ihit);
	if(fDisplayOn) {
	  chit->Draw(kBlack);
	}
      }
    }
    
    // ====== REFIT CLUSTER LEGENDRE
    double fitm, fitq;
    //    AnalyticalFit(&cluster, x, y, r, fitm, fitq);
    // LEGENDRE ----------------------------------------------------------
    legendre->SetUpLegendreHisto(180, 0, 180, 1000, -0.07, 0.07);
    //    PndTrkTrack *legendretrack = LegendreFit(&cluster);
    
    //    cout << "APPLY LEGENDRE =======================" << endl;
    //     cout << "nof hits " << cluster.GetNofHits() << endl;
    
    // reset the legendre histo for a new legendre fit
    legendre->ResetLegendreHisto();
    
    if(fDisplayOn) {
      RefreshConf();
      DrawGeometryConf(fUmin, fUmax, fVmin, fVmax);
    }
    
    // fill legendre histo with the cluster hits
    FillLegendreHisto(&cluster); 
    std::vector< double > theta_max, r_max;
    std::vector < int > content_max;
    // get the peak
    legendre->ExtractLegendreMaxima(1, theta_max, r_max, content_max);
    // ExtractLegendre(1, theta_max, r_max); // CHECK mode??
    // if(maxpeak < 4) return NULL; // CHECK
    
    // from theta/r to line parameters in CONFORMAL plane 
    double fitm2, fitq2;
    legendre->ExtractLineParameters(theta_max[0], r_max[0], fitm2, fitq2);
    if(fDisplayOn) {
      display->cd(2);
      TLine *line = new TLine(-10.07, fitq2 + fitm2 * (-10.07), 10.07, fitq2 + fitm2 * (10.07));
      line->Draw("SAME");

      display->cd(3);
      DrawLegendreHisto();
      display->Update();
      display->Modified();
  
      
    }
    
    // from line parameters to center/radius in REAL plane
    Double_t xc, yc, R;
    FromConformalToRealTrack(fitm2, fitq2, xc, yc, R);
    //    cout << "\033[1;33m MAXPEAK " << content_max[0] << " XR, YC, R: " << xc << " " << yc << " " << R << "\033[0m" << endl;
    //	cout << "start hit " << ihit << " " << hit->GetHitID() << " " << endsecid << " " << endlayid << endl;
    // --------------------------------------------------------------
    
    if(fDisplayOn)  {
      char goOnChar;
      display->cd(1);
      TArc *arcm = new TArc(xc, yc, R);
      arcm->SetFillStyle(0);
      arcm->SetLineColor(4);
      arcm->Draw("SAME");
      display->Update();
      display->Modified();
      cin >> goOnChar;
    }
    
    // what about adding the mvd hits now? lets do it!
    // ... and put mvd pixel hits to conformal plane --------------- 
    for(int ihit = 0; ihit < mvdpixhitlist->GetNofHits(); ihit++) {
      hit = mvdpixhitlist->GetHit(ihit);
      if(border == false && hit->GetSector() != sectorID) continue;
      else if(border == true && (hit->GetSector() != sectorID && hit->GetSector() != othersecID)) continue;
      //     if(hit->GetSector() != sectorID) continue;
      double distance_hit_center = (hit->GetPosition().XYvector() - TVector2(xc, yc)).Mod();
      double recoiso = fabs(distance_hit_center - R);
      // cout << "recoiso pix " << recoiso << endl;
      if(recoiso < 1.) {
	cluster.AddHit(hit);
	if(fDisplayOn)  {
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
      if(border == false && hit->GetSector() != sectorID) continue;
      else if(border == true && (hit->GetSector() != sectorID && hit->GetSector() != othersecID)) continue;
      // if(hit->GetSector() != sectorID) continue;
      double distance_hit_center = (hit->GetPosition().XYvector() - TVector2(xc, yc)).Mod();
      double recoiso = fabs(distance_hit_center - R);
      // cout << "recoiso str " << recoiso << endl;
      if(recoiso < 1.) {
	cluster.AddHit(hit);
	if(fDisplayOn)  {
	  hit->Draw(kOrange);
	  display->Update();
	  display->Modified();
	}  
	PndTrkConformalHit chit = conform->GetConformalHit(hit); 
	fConformalHitList->AddHit(&chit);  
      }
    }

    // and now: gem time!
    for(int ihit = 0; ihit < gemhitlist->GetNofHits(); ihit++) {
      hit = gemhitlist->GetHit(ihit);
      if(border == false && hit->GetSector() != sectorID) continue;
      else if(border == true && (hit->GetSector() != sectorID && hit->GetSector() != othersecID)) continue;
      //     if(hit->GetSector() != sectorID) continue;
      double distance_hit_center = (hit->GetPosition().XYvector() - TVector2(xc, yc)).Mod();
      double recoiso = fabs(distance_hit_center - R);
      // cout << "recoiso pix " << recoiso << endl;
      if(recoiso < 1. && hit->GetPosition().Perp() > CTOUTRADIUS) {
	cluster.AddHit(hit);
	if(fDisplayOn)  {
	  hit->Draw(kOrange);
	  display->Update();
	  display->Modified();
	}  
	//     PndTrkConformalHit *chit = conform->GetConformalHit(hit);
	//        conformalhitlist->AddHit(chit);  
      }
    }


  
    // and the scitil? did we forget about that? here it comes...
    // ... and put scit hits to conformal plane ------------------------------
    // the scitil are far away so give a loose request - 10 cm
    // choose however the closest one!
    // remember to check the distance from hit20
    int tmphit = -1; 
    tmpdistance = 1000;
    for(int ihit = 0; ihit < scithitlist->GetNofHits(); ihit++) {
      hit = scithitlist->GetHit(ihit);
      if(hit->GetXYDistance(hit3) > 30) continue;
      //  if(border == false && hit->GetSector() != sectorID) continue;
      // else if(border == true && (hit->GetSector() != sectorID && hit->GetSector() != othersecID)) continue;
      // if(hit->GetSector() != sectorID) continue;
      double distance_hit_center = (hit->GetPosition().XYvector() - TVector2(xc, yc)).Mod();
      double recoiso = fabs(distance_hit_center - R);
      // cout << "recoiso scitil " << recoiso << " " <<  hit->GetXYDistance(hit3) << endl;
      if(recoiso < 10.) {
	if(recoiso < tmpdistance) {
	  tmphit = ihit;
	  tmpdistance = recoiso;
	}
      }
    }
    // if there is at least one: yuppi!
    if(tmphit > -1) {
      hit = scithitlist->GetHit(tmphit);
      // cluster.AddHit(hit);
      if(fDisplayOn)  {
	hit->Draw(kOrange);
	cout << "herh" << endl;
	char goOnChar;
	cin >> goOnChar;
	display->Update();
	display->Modified();
      } 
      //     PndTrkConformalHit *chit = conform->GetConformalHit(hit); 
      //     fConformalHitList->AddHit(chit);  
    }

    // -------------------------- 
    // if there is a scitil lets use it as seed hit
    // for the conformal map
    if(tmphit > -1) {
      refhit = scithitlist->GetHit(tmphit);
      fConformalHitList->Reset();
      ComputeTraAndRot(refhit, delta, trasl);
      conform->SetOrigin(trasl[0], trasl[1], delta);
      fConformalHitList->SetConformalTransform(conform);
      // cout << "conformal hits " << fConformalHitList->GetNofHits() << endl;
   
      // fill conformal hits
      nofconfhits = FillConformalHitList(&cluster);

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
      du = fUmax - fUmin;
      dv = fVmax - fVmin;
      delt = fabs(dv - du)/2.;
      du < dv ? (fUmin -= delt, fUmax += delt) : (fVmin -= delt, fVmax += delt);
 
      if(fDisplayOn) {
	DrawGeometryConf(fUmin, fUmax, fVmin, fVmax);
	// ----------------------------------------------------------------------
	for(int ihit = 0; ihit < nofconfhits; ihit++) {
	  PndTrkConformalHit *chit = fConformalHitList->GetHit(ihit);
	  if(fDisplayOn) {
	    chit->Draw(kRed);
	  }
	}
      }
    
    }
    else {    // otherwise lets refit in the same conf map
      fConformalHitList->Reset(); // CHECK maybe you can just add hits
      // fill conformal hits
      nofconfhits = FillConformalHitList(&cluster);
    }
    
    // ====== REFIT CLUSTER ANALYTICALLY
    AnalyticalFit(&cluster, xc, yc, R, fitm, fitq);
    
    double xc2, yc2, R2;
    FromConformalToRealTrack(fitm, fitq, xc2, yc2, R2);
    if(fDisplayOn)  {
      char goOnChar;
      display->cd(1);
      TArc *arcm = new TArc(xc2, yc2, R2);
      arcm->SetFillStyle(0);
      arcm->SetLineColor(5);
      arcm->Draw("SAME");
      display->Update();
      display->Modified();
    }

    // ==================
    // MAKE the FINAL CLUSTER (at least in xy)
    //    finalcluster = new PndTrkCluster();
    fFinalCluster->Clear("C");
    // STT
    for(int ihit = 0; ihit < stthitlist->GetNofHits(); ihit++) {
      hit = stthitlist->GetHit(ihit);
      if(hit->IsSttParallel() == kFALSE) continue;
      PndSttTube* tube = (PndSttTube*) fTubeArray->At(hit->GetTubeID());
      if(border == false && tube->GetSectorID() != sectorID) continue;
      else if(border == true && (tube->GetSectorID() != sectorID && tube->GetSectorID() != othersecID)) continue;
      double distance_hit_center = (tube->GetPosition().XYvector() - TVector2(xc2, yc2)).Mod();
      double recoiso = fabs(distance_hit_center - R2);
      if(recoiso < 0.5) {
	// 	cout << "add " << hit->GetDetectorID() << " " << hit->GetHitID() << " " <<tube->GetSectorID() << " " << sectorID <<  endl;
	fFinalCluster->AddHit(hit);
      }
    }
    // MVD PIX
    for(int ihit = 0; ihit < mvdpixhitlist->GetNofHits(); ihit++) {
      hit = mvdpixhitlist->GetHit(ihit);
      if(border == false && hit->GetSector() != sectorID) continue;
      else if(border == true && (hit->GetSector() != sectorID && hit->GetSector() != othersecID)) continue;
      double distance_hit_center = (hit->GetPosition().XYvector() - TVector2(xc2, yc2)).Mod();
      double recoiso = fabs(distance_hit_center - R2);
      if(recoiso < 0.5) {
	// 	cout << "add " << hit->GetDetectorID() << " " << hit->GetHitID()  << endl;
	fFinalCluster->AddHit(hit);
      }
    }
    // MVD STR
    for(int ihit = 0; ihit < mvdstrhitlist->GetNofHits(); ihit++) {
      hit = mvdstrhitlist->GetHit(ihit);
      if(border == false && hit->GetSector() != sectorID) continue;
      else if(border == true && (hit->GetSector() != sectorID && hit->GetSector() != othersecID)) continue;
      double distance_hit_center = (hit->GetPosition().XYvector() - TVector2(xc2, yc2)).Mod();
      double recoiso = fabs(distance_hit_center - R2);
      if(recoiso < 0.5)  {
	// 	cout << "add " << hit->GetDetectorID() << " " << hit->GetHitID() <<  endl;
	fFinalCluster->AddHit(hit);
      }
    }
    // GEM
    for(int ihit = 0; ihit < gemhitlist->GetNofHits(); ihit++) {
      hit = gemhitlist->GetHit(ihit);
      if(border == false && hit->GetSector() != sectorID) continue;
      else if(border == true && (hit->GetSector() != sectorID && hit->GetSector() != othersecID)) continue;
      double distance_hit_center = (hit->GetPosition().XYvector() - TVector2(xc2, yc2)).Mod();
      double recoiso = fabs(distance_hit_center - R2);
      if(recoiso < 0.5  && hit->GetPosition().Perp() > CTOUTRADIUS)  {
	// 	cout << "add " << hit->GetDetectorID() << " " << hit->GetHitID()  << endl;
	fFinalCluster->AddHit(hit);
      }
    }
    // SCITIL
    tmphit = -1; 
    tmpdistance = 1000;
    for(int ihit = 0; ihit < scithitlist->GetNofHits(); ihit++) {
      hit = scithitlist->GetHit(ihit);
      if(hit->GetXYDistance(hit3) > 30) continue;
      double distance_hit_center = (hit->GetPosition().XYvector() - TVector2(xc2, yc2)).Mod();
      double recoiso = fabs(distance_hit_center - R2);
      if(recoiso < 0.5) {
	if(recoiso < tmpdistance) {
	  tmphit = ihit;
	  tmpdistance = recoiso;
	}
      }
    }
    if(tmphit > -1) {
      hit = scithitlist->GetHit(tmphit);
      //      cout << "add " << hit->GetDetectorID() << " " << hit->GetHitID() <<  endl;
      fFinalCluster->AddHit(hit);
    }

    if(fFinalCluster->GetNofHits() == 0) {
      continue;
    }

    //           cout << "FINAL CLUSTER HAS " << fFinalCluster->GetNofHits() << endl;
    if(fDisplayOn) {
      Refresh();
      fFinalCluster->LightUp();
      display->Update();
      display->Modified();
      char goOnChar;
      cin >> goOnChar;
    }
    
    // sorting
    for(int ihit = 0; ihit < fFinalCluster->GetNofHits(); ihit++) {
      hit = fFinalCluster->GetHit(ihit);
      hit->SetSortVariable(hit->GetPosition().Perp());
      /**
	 if(hit->IsMvdPixel())
	 else if(hit->IsMvdStrip())
	 else if(hit->IsStt())
	 else if(hit->IsGem())
	 else if(hit->IsScitil())
      **/
    }
    fFinalCluster->Sort();

    PndTrkTrack finaltrack(fFinalCluster, xc2, yc2, R2);

    // =========================== Z PART ==================================
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // SKEWED ASSOCIATION ********* CHECK *********
    // -------------------------------------------------------
    //    cout << " %%%%%%%%%%%%%%%%%%%% ZFINDER %%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;

    if(fDisplayOn) DrawZGeometry();
    
    // create cluster for z finding

    // lets start from the skewed --------------------------------
    //    PndTrkCluster skewhitlist = CreateSkewHitList(finaltrack);
    PndTrkSkewHitList skewhitlist;
    double phimin = 400, phimax = -1, zmin = 1000, zmax = -1;
    for(int ihit = 0; ihit < stthitlist->GetNofHits(); ihit++) {
      hit = stthitlist->GetHit(ihit);	
      //      cout << hit->IsSttSkew() << " " << hit->GetSector() << " " << TMath::RadToDeg() * hit->GetPosition().Phi() << " " << sectorID << " " << othersecID << endl;
      if(hit->IsSttSkew() == kFALSE) continue;
 
      if(border == false && hit->GetSector() != sectorID) continue;
      else if(border == true && (hit->GetSector() != sectorID && hit->GetSector() != othersecID)) continue;

     int tubeID = hit->GetTubeID();
     PndSttTube *tube = (PndSttTube*) fTubeArray->At(tubeID);
   
     TVector3 wireDirection = tube->GetWireDirection();
     Double_t halflength = tube->GetHalfLength();
      
     TVector3 first  = tube->GetPosition() + wireDirection * halflength; // CHECK
     TVector3 second = tube->GetPosition() - wireDirection * halflength; // CHECK
    
     //    double m1 = (first - second).Y()/(first - second).X();
     //    double q1 = first.Y() - m1 * first.X();
      
     // 1. compute intersection between the track circle and the wire
     TVector2 intersection1, intersection2;
     Int_t nofintersections = tools->ComputeSegmentCircleIntersection(TVector2(first.X(), first.Y()), TVector2(second.X(), second.Y()), xc2, yc2, R2, intersection1, intersection2);
      
     if(nofintersections == 0) continue;
     if(nofintersections >= 2) {
     cout << "ERROR: MORE THAN 1 INTERSECTION!!" << endl;
     continue; // CHECK
     }

     // 2. find the tangent to the track in the intersection point
     // tangent approximation
     TVector2 tangent = tools->ComputeTangentInPoint(xc2, yc2, intersection1);
       
     // 3. rotate clockwise the tangent/point/(wire, not explicitely)
     // in order to have the wire parallel to the x axis;
     // then translate everything to have the wire ON the x axis
     double beta = wireDirection.Phi();
     if(beta < 0) beta += TMath::Pi();
     // ... rotate the tangent
     double rtx = TMath::Cos(beta) * tangent.X() + TMath::Sin(beta) * tangent.Y();
     double rty = TMath::Cos(beta) * tangent.Y() - TMath::Sin(beta) * tangent.X();
    TVector2 rottangent(rtx, rty);
     rottangent = rottangent.Unit();
     // ... rotate the point
     double rx = TMath::Cos(beta) * intersection1.X() + TMath::Sin(beta) * intersection1.Y();
     double ry = TMath::Cos(beta) * intersection1.Y() - TMath::Sin(beta) * intersection1.X();
       
     // translation
     Double_t deltay = ry;
     rty -= deltay;
     ry -= deltay;

     // rotm, rotp
     Double_t rotm = rottangent.Y()/rottangent.X();
     Double_t rotp = ry - rotm * rx;

     // ellipsis
     double a = hit->GetIsochrone() * TMath::Cos(SKEW_ANGLE); // CHECK skew angle hard coded
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
   
     // 4. retraslate/rerotate all back to the original plane
     // retranslate
     y0 += deltay; 
     intya  += deltay; 
     intyb  += deltay; 
   
     // rerotate
     double x0anew = TMath::Cos(beta) * x0a - TMath::Sin(beta) * y0;
     double y0anew = TMath::Cos(beta) * y0 + TMath::Sin(beta) * x0a;
     double x0bnew = TMath::Cos(beta) * x0b - TMath::Sin(beta) * y0;
     double y0bnew = TMath::Cos(beta) * y0 + TMath::Sin(beta) * x0b;
   
     double intxanew = TMath::Cos(beta) * intxa - TMath::Sin(beta) * intya;
     double intyanew = TMath::Cos(beta) * intya + TMath::Sin(beta) * intxa;
     double intxbnew = TMath::Cos(beta) * intxb - TMath::Sin(beta) * intyb;
     double intybnew = TMath::Cos(beta) * intyb + TMath::Sin(beta) * intxb;
   
     intxa = intxanew;
     intya = intyanew;
     intxb = intxbnew;
     intyb = intybnew;

     // now we have x0a, y0a, center of the 1st ellipse
     // and x0b, y0b, center of the 2nd ellipse
     x0a = x0anew;
     double y0a = y0anew;
     x0b = x0bnew;
     double y0b = y0bnew;

     if(fDisplayOn) {
     char goOnChar;
     display->cd(1);

     TEllipse *ell1 = new TEllipse(x0a, y0a, a, b, 0, 360, -beta);
     ell1->SetFillStyle(0);
     ell1->SetLineColor(4);
     ell1->Draw("SAME");
     TEllipse *ell2 = new TEllipse(x0b, y0b, a, b, 0, 360, -beta);
     ell2->SetFillStyle(0);
     ell2->SetLineColor(6);
     ell2->Draw("SAME");

     TMarker *mrkinta = new TMarker(intxa, intya, 20);
     mrkinta->SetMarkerColor(4);
     mrkinta->Draw("SAME");
     TMarker *mrkintb = new TMarker(intxb, intyb, 20);
     mrkintb->SetMarkerColor(6);
     mrkintb->Draw("SAME");
     }

     // 5. calculate z coordinate for each intersection
       
     // calculate z0a, z0b of the center of the ellipse
     Double_t t = ((x0a + y0a) - (first.X() + first.Y())) /  ((second.X() - first.X()) + (second.Y() - first.Y()));
     Double_t z0a = first.Z() + (second.Z() - first.Z()) * t;
     //    cout << "0 : calculate t, z0a " << t << " " << z0a << endl;
 
     t = ((x0b + y0b) - (first.X() + first.Y())) /  ((second.X() - first.X()) + (second.Y() - first.Y()));
     Double_t z0b = first.Z() + (second.Z() - first.Z()) * t;
 
     TVector3 center1(x0a, y0a, z0a);
     TVector3 center2(x0b, y0b, z0b);

     // calculate the z of the intersection ON the ellipse (CHECK this step calculations!)
     double dx = intxa - x0a;
     double dy = intya - y0a;
     TVector3 dxdy(dx, dy, 0.0);

     TVector3 tfirst = first + dxdy;
     TVector3 tsecond = second + dxdy;

     t = ((intxa + intya) - (tfirst.X() + tfirst.Y())) /  ((tsecond.X() - tfirst.X()) + (tsecond.Y() - tfirst.Y()));
     double intza = tfirst.Z() + (tsecond.Z() - tfirst.Z()) * t;
     if(fDisplayOn) {
     char goOnChar;
     display->cd(3);
     TLine *linezx1 = new TLine(tfirst.X(), tfirst.Z(), tsecond.X(), tsecond.Z());
     linezx1->SetLineStyle(1);
     linezx1->Draw("SAME");
     TMarker *mrkza1 = new TMarker(intxa, intza, 20);
     mrkza1->SetMarkerColor(kBlue - 9);
     mrkza1->Draw("SAME");
     // cin >> goOnChar;   
     }

     tfirst = first - dxdy;
     tsecond = second - dxdy;

     t = ((intxb + intyb) - (tfirst.X() + tfirst.Y())) /  ((tsecond.X() - tfirst.X()) + (tsecond.Y() - tfirst.Y()));
     double intzb = tfirst.Z() + (tsecond.Z() - tfirst.Z()) * t;
 
     TVector3 fin_intersection1(intxa, intya, intza);
     TVector3 fin_intersection2(intxb, intyb, intzb);

     // CHECK to be changed
     int trackID = 1;
     double phi1 = finaltrack.ComputePhi(fin_intersection1);
     double phi2 = finaltrack.ComputePhi(fin_intersection2);

     
     // skewhit = new PndTrkSkewHit(*hit, trackID, center1, fin_intersection1, phi1, center2, fin_intersection2, phi2, a, b, -1, beta);
     //    //      skewhit->Print();
     //       skewhitlist.AddHit(skewhit);

     skewhitlist.AddHit(PndTrkSkewHit(*hit, trackID, center1, fin_intersection1, phi1, center2, fin_intersection2, phi2, a, b, -1, beta));
     }
     // --------------------------

     // add hits which have a z info
     // and belong to the cluster
     for(int ihit = 0; ihit <  fFinalCluster->GetNofHits(); ihit++) {
       hit = fFinalCluster->GetHit(ihit);
       if(hit->IsStt() == kFALSE) skewhitlist.AddHit(PndTrkSkewHit(*hit));
     }
     // skewhitlist = CleanUpSkewHitList(&skewhitlist);
    
     // ========================================================
     std::vector < int > first, second;
     for(int ihit = 0; ihit < skewhitlist.GetNofHits(); ihit++) {
       hit = skewhitlist.GetHit(ihit);
       if(!hit) continue;
       if(hit->IsStt() == kFALSE) continue;

       PndTrkSkewHit *skewhit = (PndTrkSkewHit*) hit;
       
       TVector3 fin_intersection1 = skewhit->GetIntersection1();
       TVector3 fin_intersection2 = skewhit->GetIntersection2();
       double phi1 = skewhit->GetPhi1();
       double phi2 = skewhit->GetPhi2();
       

       if(fDisplayOn) {
	 display->cd(4);
	 TLine *linezphi = new TLine(phi1, fin_intersection1.Z(), phi2, fin_intersection2.Z());
	 linezphi->SetLineStyle(1);
	 linezphi->Draw("SAME");
	 TMarker *mrkzphi1 = new TMarker(phi1, fin_intersection1.Z(), 20);
	 mrkzphi1->SetMarkerColor(kBlue - 9);
	 mrkzphi1->Draw("SAME");
	 TMarker *mrkzphi2 = new TMarker(phi2, fin_intersection2.Z(), 20);
	 mrkzphi2->SetMarkerColor(kMagenta - 7);
	 mrkzphi2->Draw("SAME");
	 display->Update();
	 display->Modified();
       }


       // check neighborings @ layer 8 & 15
       PndSttTube *tube = (PndSttTube*) fTubeArray->At(hit->GetTubeID());
       if(tube->GetLayerID() == 8) {
	 PndTrkHit *thit = stthitlist->GetHit(hit->GetHitID());
	 TObjArray neighs = fHitMap->GetNeighboringsToHit(thit);
	 //	 cout << thit->GetHitID() << " on layer " << tube->GetLayerID() << " " << neighs.GetEntriesFast() << endl;
	 for(int j = 0; j < neighs.GetEntriesFast(); j++) {
	   PndTrkHit *nhit = (PndTrkHit*) neighs.At(j);
	   PndSttTube *ntube = (PndSttTube*) fTubeArray->At(nhit->GetTubeID());
	   if(ntube->GetLayerID() == 7) {
	     //	     cout << "hit close to 7" << endl;
	     first.push_back(ihit);
	   }
	 }
       }
       if(tube->GetLayerID() == 15) 
	 {
	   PndTrkHit *thit = stthitlist->GetHit(hit->GetHitID());
	   TObjArray neighs = fHitMap->GetNeighboringsToHit(thit);
	   // cout << hit->GetHitID() << " on layer " << tube->GetLayerID() << " " << neighs.GetEntriesFast() << endl;
	   for(int j = 0; j < neighs.GetEntriesFast(); j++) {
	     PndTrkHit *nhit = (PndTrkHit*) neighs.At(j);
	     PndSttTube *ntube = (PndSttTube*) fTubeArray->At(nhit->GetTubeID());
	     if(ntube->GetLayerID() == 16) {
	       //	       cout << "hit close to 16" << endl;
	       second.push_back(ihit);
	     }
	   }
	 }
     }

     double zdistance = 1000;
     int tmpi = -1, tmpj = -1;
     double tmpiz, tmpjz, tmpiphi, tmpjphi;


     for(int ihit = 0; ihit < first.size(); ihit++) {
       int hitiskeid = first[ihit];
       PndTrkSkewHit *skewhiti = (PndTrkSkewHit*) skewhitlist.GetHit(hitiskeid);
       TVector3 fin_intersectioni = 0.5 * (skewhiti->GetIntersection1() + skewhiti->GetIntersection2());
       double phii = 0.5 * (skewhiti->GetPhi1() + skewhiti->GetPhi2()) ;
       for(int jhit = 0; jhit < second.size(); jhit++) {
	 int hitjskeid = second[jhit];
	 PndTrkSkewHit *skewhitj = (PndTrkSkewHit*) skewhitlist.GetHit(hitjskeid);
	 TVector3 fin_intersectionj = 0.5 * (skewhitj->GetIntersection1() + skewhitj->GetIntersection2());
	 double phij = 0.5 * (skewhitj->GetPhi1() + skewhitj->GetPhi2()) ;
       
	 double tmpzdistance = fabs(fin_intersectioni.Z() - fin_intersectionj.Z());
	 if(tmpzdistance < zdistance) {
	   zdistance = tmpzdistance;
	   tmpi = hitiskeid;
	   tmpj = hitjskeid;
	   tmpiz = fin_intersectioni.Z(); 
	   tmpjz = fin_intersectionj.Z();
	   tmpiphi = phii;
	   tmpjphi = phij;
	 }
       }
     }

     double linem = (tmpiz - tmpjz) / (tmpiphi - tmpjphi);
     double lineq = tmpiz - linem * tmpiphi;

     // =








     /**


     // -------- fit with a line
     fFitter->Reset();    
     // see the hits
     for(int ihit = 0; ihit < skewhitlist.GetNofHits(); ihit++) {
       hit = skewhitlist.GetHit(ihit);
       if(!hit) continue;
       if(hit->IsStt() == kFALSE) {
	 TVector3 position = hit->GetPosition();
	 double phi = finaltrack.ComputePhi(position);
	 fFitter->SetPointToFit(phi, position.Z(), 0.1); // CHECK the error?
	 // cout << "point " << phi << " " << position.Z() << " " << hit->IsMvdPixel() << " " << hit->IsMvdStrip() << " " << hit->IsGem() << " " << hit->IsSciTil() << endl;
	 if(fDisplayOn)	{
	   display->cd(4);
	   TMarker *mrkzphi = new TMarker(phi, position.Z(), 20);
	   mrkzphi->SetMarkerColor(kGreen - 9);
	   mrkzphi->Draw("SAME");
	   display->Update();
	   display->Modified();
	 }
       }
       else {

	 PndTrkSkewHit *skewhit = (PndTrkSkewHit*) hit;

	 TVector3 fin_intersection1 = skewhit->GetIntersection1();
	 TVector3 fin_intersection2 = skewhit->GetIntersection2();
	 double phi1 = skewhit->GetPhi1();
	 double phi2 = skewhit->GetPhi2();
	//  cout << "driftI  " << phi1 << " " << fin_intersection1.Z() << endl;
// 	 cout << "driftII " << phi2 << " " << fin_intersection2.Z() << endl;
	 fFitter->SetPointToFit(phi1, fin_intersection1.Z(), fabs(fin_intersection1.Z() - fin_intersection2.Z())/sqrt(12.)); // CHECK the error?
	 fFitter->SetPointToFit(phi2, fin_intersection2.Z(), fabs(fin_intersection1.Z() - fin_intersection2.Z())/sqrt(12.)); // CHECK the error?

	 if(fDisplayOn) {
	   display->cd(4);
	   TLine *linezphi = new TLine(phi1, fin_intersection1.Z(), phi2, fin_intersection2.Z());
	   linezphi->SetLineStyle(1);
	   linezphi->Draw("SAME");
	   TMarker *mrkzphi1 = new TMarker(phi1, fin_intersection1.Z(), 20);
	   mrkzphi1->SetMarkerColor(kBlue - 9);
	   mrkzphi1->Draw("SAME");
	   TMarker *mrkzphi2 = new TMarker(phi2, fin_intersection2.Z(), 20);
	   mrkzphi2->SetMarkerColor(kMagenta - 7);
	   mrkzphi2->Draw("SAME");
	   display->Update();
	   display->Modified();
	 }
       }
     }

     double fitm3 = 0, fitq3 = 0;
     fFitter->StraightLineFit(fitm3, fitq3);
     **/


     double fitm3 = linem, fitq3 = lineq; // CHECK 

     if(fDisplayOn) {
       display->cd(4);
       TLine *line = new TLine(0, fitq3, 360, 360 * fitm3 + fitq3);
       line->SetLineColor(2);
       line->Draw("SAME");

   //     TLine *tlinezphi = new TLine(0, lineq, 360, 360 * linem + lineq);
//        tlinezphi->SetLineColor(5);
//        tlinezphi->Draw("SAME");

       display->Update();
       display->Modified();
       char goOnChar;
       cin >> goOnChar;



     }
     // choose the fin_intersection of the skew
     PndTrkSkewHitList skewhitlist2;
     for(int ihit = 0; ihit < skewhitlist.GetNofHits(); ihit++) {
       hit = (PndTrkHit*) skewhitlist.GetHit(ihit);
       if(!hit) continue;
       if(hit->IsSttSkew() == kFALSE) {
	 skewhitlist2.AddHit(hit);
	 continue;
       }
       else {
	 TVector3 fin_intersection1 = ((PndTrkSkewHit*) hit)->GetIntersection1();
	 TVector3 fin_intersection2 = ((PndTrkSkewHit*) hit)->GetIntersection2();
	 double phi1 = ((PndTrkSkewHit*) hit)->GetPhi1();
	 double phi2 = ((PndTrkSkewHit*) hit)->GetPhi2();
   
	 double dist1 = fabs(fitm3 * phi1 - fin_intersection1.Z() + fitq3) / TMath::Sqrt(fitm3 * fitm3 + 1); // CHECK ortho distance or not?
	 double dist2 = fabs(fitm3 * phi2 - fin_intersection2.Z() + fitq3) / TMath::Sqrt(fitm3 * fitm3 + 1); // CHECK    "           "
     
	 double distance = 1000;
	 dist1 < dist2 ? (distance = dist1, hit->SetPosition(fin_intersection1), hit->SetPhi(phi1)) : (distance = dist2, hit->SetPosition(fin_intersection2), hit->SetPhi(phi2)); 

	 if(distance < 3) skewhitlist2.AddHit(hit);
       }
     }
     // -------- refit with a line
     fFitter->Reset();    
     
     // see the hits
     for(int ihit = 0; ihit < skewhitlist2.GetNofHits(); ihit++) {
       hit = skewhitlist2.GetHit(ihit);
       if(!hit) continue;
       TVector3 position = hit->GetPosition();
       double phi = finaltrack.ComputePhi(position);
       fFitter->SetPointToFit(phi, position.Z(), 0.1); // CHECK the error?
       //      cout << "final point " << phi << " " << position.Z() << endl;
       if(fDisplayOn)	{
	 display->cd(4);
	 TMarker *mrkzphi = new TMarker(phi, position.Z(), 20);
	 mrkzphi->SetMarkerColor(kGreen);
	 if(hit->IsStt() == kTRUE) mrkzphi->SetMarkerColor(kOrange);
	 mrkzphi->Draw("SAME");
	 display->Update();
	 display->Modified();
       }
     }

     double fitm4, fitq4;
     fFitter->StraightLineFit(fitm4, fitq4);

     if(fDisplayOn) {
       display->cd(4);
       TLine *line = new TLine(0, fitq4, 360, 360 * fitm4 + fitq4);
       line->SetLineColor(4);
       line->Draw("SAME");
       display->Update();
       display->Modified();
       char goOnChar;
       cin >> goOnChar;
     }

     // finalize the cluster
     for(int ihit = 0; ihit < skewhitlist2.GetNofHits(); ihit++) {
       hit = skewhitlist2.GetHit(ihit);
       if(fFinalCluster->DoesContain(hit) == kFALSE) fFinalCluster->AddHit((PndTrkHit*) hit);
     }

    // sorting
    for(int ihit = 0; ihit < fFinalCluster->GetNofHits(); ihit++) {
      hit = fFinalCluster->GetHit(ihit);
      hit->SetSortVariable(hit->GetPosition().Perp());
      /**
	 if(hit->IsMvdPixel())
	 else if(hit->IsMvdStrip())
	 else if(hit->IsStt())
	 else if(hit->IsGem())
	 else if(hit->IsScitil())
      **/

      // cout << "before sorting " << hit->GetDetectorID() << " " << hit->GetSortVariable() << " " << hit->GetPosition().X() << " " << hit->GetPosition().Y() << " " << hit->GetPosition().Z() << endl;
    }
    fFinalCluster->Sort();
    // sorting
    for(int ihit = 0; ihit < fFinalCluster->GetNofHits(); ihit++) {
      hit = fFinalCluster->GetHit(ihit);
      // cout << "becomes  sorting " << hit->GetDetectorID() << " " << hit->GetSortVariable() << " " << hit->GetPosition().X() << " " << hit->GetPosition().Y() << " " << hit->GetPosition().Z() << endl;
    }

    // compute charge
    finaltrack.ComputeCharge();
    
    // last two parameters in real plane
    double tanl = - finaltrack.GetCharge() * fitm4 * (180./TMath::Pi())/R2;
    double z0 = fitq4;
    


     // PndTrkTrack *finaltrack2 = new PndTrkTrack(finalcluster, xc2, yc2, R2);

     finaltrack.SetCluster(fFinalCluster);
     finaltrack.SetCenter(xc2, yc2);
     finaltrack.SetRadius(R2);
     finaltrack.SetZ0(z0);
     finaltrack.SetTanL(tanl);

     // clusterlist.AddCluster(finalcluster);
     fTrackList->AddTrack(&finaltrack);
     for(int jtrk = 0; jtrk < fTrackList->GetNofTracks(); jtrk++) {
       PndTrkTrack *tmptrack = fTrackList->GetTrack(jtrk);
     }
  }

  
 

  // ====== MERGIN ==========================

  PndTrkTrackList *mergedtracklist = new PndTrkTrackList();
  // choose what to merge
  std::vector< int > merged;
  merged.resize(fTrackList->GetNofTracks());
  for(int itrk = 0; itrk < fTrackList->GetNofTracks(); itrk++)  merged[itrk] = 0;
  std::map < int , std::vector < int > > mergingtracks;

  for(int itrk = 0; itrk < fTrackList->GetNofTracks(); itrk++) {
    if(merged[itrk] == 1) continue;
    PndTrkTrack *tracki = fTrackList->GetTrack(itrk);
    PndTrkCluster clusteri = tracki->GetCluster();
//     if(fDisplayOn)  {
//       char goOnChar;
//       display->cd(1);
//       Refresh();
//       clusteri.LightUp();
//       display->Update();
//       display->Modified();
//       cin >> goOnChar;
//     }
    std::vector< int > mtracks;
    for(int jtrk = itrk + 1; jtrk < fTrackList->GetNofTracks() ; jtrk++) {
      if(merged[jtrk] == 1) continue;
      PndTrkTrack *trackj = fTrackList->GetTrack(jtrk);
      PndTrkCluster clusterj = trackj->GetCluster();
      if(clusterj.SharedAt(&clusteri, 0.70) == kTRUE) {
	mtracks.push_back(jtrk);
	//	cout << "track " << itrk << " needs merging with track " << jtrk << endl;
	merged[itrk] = 1; // CHECK save time!
	merged[jtrk] = 1;
     
// 	if(fDisplayOn)  {
// 	  char goOnChar;
// 	  display->cd(1);
// 	  Refresh();
// 	  clusterj.LightUp();
// 	  display->Update();
// 	  display->Modified();
// 	  cin >> goOnChar;
// 	}
      }
    }
   if(mtracks.size() == 0)  mtracks.push_back(-1);
   mergingtracks.insert(std::pair<int , std::vector < int > >(itrk, mtracks));
   //   cout << "pair of " << itrk << " and " << mtracks.size() << endl;
  }

  //  cout << "merging " << mergingtracks.size() << endl;
  // do the actual merging
  std::map< int, std::vector < int > >::iterator it;
  for(int itrk = 0; itrk < fTrackList->GetNofTracks(); itrk++) {
    PndTrkTrack *tracki = fTrackList->GetTrack(itrk);
    PndTrkCluster clusteri = tracki->GetCluster();
    it = mergingtracks.find(itrk);
    if(it == mergingtracks.end()) continue;

    std::vector< int > mtracks = it->second;
    // if it has merged tracks
    if(mtracks[0] != -1) {
      //  cout << it->first << "/" << mergingtracks.size() << endl; // " last fit " <<  clusteri.GetNofHits() << endl;
      // cout << "i " << clusteri.GetNofHits() << endl;
      for(int ktrk = 0; ktrk < mtracks.size(); ktrk++) {
	int jtrk = mtracks[ktrk];
	PndTrkTrack *trackj = fTrackList->GetTrack(jtrk);
	PndTrkCluster clusterj = trackj->GetCluster();
	// cout << "j " << clusterj.GetNofHits() << endl;
	// 	if(fDisplayOn)  {
	// 	  char goOnChar;
	// 	  display->cd(1);
	// 	  Refresh();
	// 	  clusteri.LightUp();
	// 	  display->Update();
	// 	  display->Modified();
	//   cin >> goOnChar;
	  
	// 	  Refresh();
	// 	  clusterj.LightUp(); 
	// 	  display->Update();
	// 	  display->Modified();
	//   cin >> goOnChar;
    
	// 	  display->Update();
	// 	  display->Modified();
	// 	}
	clusteri.MergeTo(&clusterj);

    
	// cout << "dopo " << clusteri.GetNofHits() << endl;
	//
      }
    }
    //    else {
    //       cout << it->first << " not merged" << endl; // " last fit " <<  clusteri.GetNofHits() << endl;
    //     }
    //--------------------------------------
    // to conformal
    fRefHit = clusteri.GetHit(clusteri.GetNofHits() - 1);
    fConformalHitList->Reset();
    double delta, trasl[2];
    ComputeTraAndRot(fRefHit, delta, trasl);
    conform->SetOrigin(trasl[0], trasl[1], delta);
    fConformalHitList->SetConformalTransform(conform);
    // cout << "conformal hits " << fConformalHitList->GetNofHits() << endl;
    
    // fill conformal hits
    int nofconfhits = FillConformalHitList(&clusteri);
  
    // compute conformal plane extremities ---------------------------- this must go to a fctn CHECK
    fUmin =  1000, fVmin =  1000, fRmin =  1000;
    fUmax = -1000, fVmax = -1000, fRmax = -1000;
    double rc_of_min, rc_of_max;
    
    for(int jhit = 0; jhit < fConformalHitList->GetNofHits(); jhit++) {
      PndTrkConformalHit *chit = fConformalHitList->GetHit(jhit);
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
    double delt = 0.5 * fabs(dv - du);
    du < dv ? (fUmin -= delt, fUmax += delt) : (fVmin -= delt, fVmax += delt);
    
    if(fDisplayOn) {
      DrawGeometryConf(fUmin, fUmax, fVmin, fVmax);
      // ----------------------------------------------------------------------
      for(int ihit = 0; ihit < nofconfhits; ihit++) {
	PndTrkConformalHit *chit = fConformalHitList->GetHit(ihit);
	if(fDisplayOn) {
	  chit->Draw(kRed);
	}
      }
    }
    
  
    // last fit
    double xc2 = tracki->GetCenter().X();
    double yc2 = tracki->GetCenter().Y();
    double R2 = tracki->GetRadius();
    double fitm, fitq;

    AnalyticalFit(&clusteri, xc2, yc2, R2, fitm, fitq);
    
    double xc3, yc3, R3;
    FromConformalToRealTrack(fitm, fitq, xc3, yc3, R3);
    if(fDisplayOn)  {
      char goOnChar;
      Refresh();
      display->cd(1);
      clusteri.LightUp();
      TArc *arcm = new TArc(xc3, yc3, R3);
      arcm->SetFillStyle(0);
      arcm->SetLineColor(kGreen);
      arcm->Draw("SAME");
      display->Update();
      display->Modified();
      cin >> goOnChar;
    }
    
    tracki->SetCluster(&clusteri);
    tracki->SetCenter(xc3, yc3);
    tracki->SetRadius(R3);
  
    mergedtracklist->AddTrack(tracki);
    
  }
  // =============================================
  // CLEANING
  // rms
  PndTrkTrackList *cleanedtracklist = new PndTrkTrackList();
  for(int itrk = 0; itrk < mergedtracklist->GetNofTracks(); itrk++) {
    PndTrkTrack *track = mergedtracklist->GetTrack(itrk);
    double xc3 = track->GetCenter().X();
    double yc3 = track->GetCenter().Y();
    double R3 = track->GetRadius();

     bool vote = true;
    // CHI2 ___________________
    double gap[5] = {0, 0, 0, 0, 0};
    
    PndTrkCluster clusteri = track->GetCluster();
    
    // check if it is a long track
    hit = clusteri.GetHit(0);
    cout << "FIRST LAY " << hit->GetDetectorID() << endl;
    if(hit->GetDetectorID() == FairRootManager::Instance()->GetBranchId(fSttBranch)) {
      PndSttTube *tube0 = (PndSttTube* ) fTubeArray->At(hit->GetTubeID());
      cout << tube0->GetLayerID() << endl;
      if(tube0->GetLayerID() > 4) continue; // CHECK
    }
    hit = clusteri.GetHit(clusteri.GetNofHits() - 1);
    cout << "LAST LAY " << hit->GetDetectorID() << endl;
    if(hit->GetDetectorID() == FairRootManager::Instance()->GetBranchId(fSttBranch)) {
      PndSttTube *tubeN = (PndSttTube* ) fTubeArray->At(hit->GetTubeID());
      cout << tubeN->GetLayerID() << endl;
      if(tubeN->GetLayerID() < 16) continue; // CHECK
    }

    for(int ihit = 0; ihit < clusteri.GetNofHits(); ihit++) {
      hit = clusteri.GetHit(ihit);
      int detID = hit->GetDetectorID();
      TVector3 pos = hit->GetPosition();
    
      // mvd pix
      if(detID == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) {
	gap[0] += fabs(TMath::Sqrt((pos.X() - xc3) * (pos.X() - xc3) + (pos.Y() - yc3) * (pos.Y() - yc3)) - R3);
      }
      // mvd str
      else if(detID == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) {
	gap[1] += fabs(TMath::Sqrt((pos.X() - xc3) * (pos.X() - xc3) + (pos.Y() - yc3) * (pos.Y() - yc3)) - R3);
      }
      // stt
      else if(detID == FairRootManager::Instance()->GetBranchId(fSttBranch)) {
	gap[2] += fabs(TMath::Sqrt((pos.X() - xc3) * (pos.X() - xc3) + (pos.Y() - yc3) * (pos.Y() - yc3)) - R3) - hit->GetIsochrone();
      }
      // scitil;
      else if(detID == FairRootManager::Instance()->GetBranchId(fSciTBranch)) {
	gap[3] += fabs(TMath::Sqrt((pos.X() - xc3) * (pos.X() - xc3) + (pos.Y() - yc3) * (pos.Y() - yc3)) - R3);
      }
      // gem
      else if(detID == FairRootManager::Instance()->GetBranchId(fGemBranch)) {
	gap[4] += fabs(TMath::Sqrt((pos.X() - xc3) * (pos.X() - xc3) + (pos.Y() - yc3) * (pos.Y() - yc3)) - R3);
      }
    }

    // give large cuts CHECK
    if(gap[0] > 5) vote = false; // mvd pix
    if(gap[1] > 5) vote = false; // mvd str
    if(gap[2] > 50) vote = false; // stt
    if(gap[3] > 1) vote = false; // scit
    if(gap[4] > 5) vote = false; // gem
    cout << "VOTE " << vote << endl;

    if(vote == true) cleanedtracklist->AddTrack(track);
  }
  delete mergedtracklist;

  // at last, add the INDIVISIBLES
  PndTrkTrackList *indivtracklist = new PndTrkTrackList();
  for(int itrk = 0; itrk < cleanedtracklist->GetNofTracks(); itrk++) {
    PndTrkTrack *tracki = cleanedtracklist->GetTrack(itrk);
    PndTrkCluster clusteri = tracki->GetCluster();
    if(fDisplayOn) {
      char goOnChar;
      display->cd(1);
      Refresh();
      clusteri.Draw(kRed);
      display->Update();
      display->Modified();
      cin >> goOnChar ;
    }
    for(int ihit = 0; ihit < clusteri.GetNofHits(); ihit++) {
      hit = clusteri.GetHit(ihit);
      if(hit->GetDetectorID() != FairRootManager::Instance()->GetBranchId(fSttBranch)) continue;
      TObjArray indiv2 = fHitMap->GetIndivisiblesToHit(hit);

      if(fDisplayOn) {
	  hit->Draw(kBlue);
	  display->Update();
	  display->Modified();
	}	
	for(int jhit = 0; jhit < indiv2.GetEntriesFast(); jhit++) {
	  PndTrkHit *hit2 = (PndTrkHit*) indiv2.At(jhit);
	  hit2->Draw(kGreen);
	  hit2->SetSortVariable(hit2->GetPosition().Perp());
	  if(!clusteri.DoesContain(hit2)) {
	    clusteri.AddHit(hit2);
	  }
	}
	if(fDisplayOn) {
	  char goOnChar;
	  cin >> goOnChar ;
	}
    }
    clusteri.Sort();
    tracki->SetCluster(&clusteri);
    if(fDisplayOn) {
      char goOnChar;
      display->cd(1);
      Refresh();
      clusteri.Draw(kMagenta);
      display->Update();
      display->Modified();
      cin >> goOnChar ;
    }
    indivtracklist->AddTrack(tracki);
  }
  delete cleanedtracklist;



  fTrackList = indivtracklist;
  
  //   fDisplayOn = kTRUE;
  if(fDisplayOn) {
    char goOnChar;
    display->cd(1);
    Refresh();
  }
  // PndTrkTrack --> PndTrack
  for(int itrk = 0; itrk < fTrackList->GetNofTracks(); itrk++) {
    PndTrkTrack *track = fTrackList->GetTrack(itrk);
  
    PndTrack theTrack = track->ConvertToPndTrack();

    TClonesArray& clref1 = *fTrackArray;
    Int_t size = clref1.GetEntriesFast();
    PndTrack *outputtrack = new(clref1[size]) PndTrack(theTrack.GetParamFirst(),theTrack.GetParamLast(), theTrack.GetTrackCand());

    TClonesArray& clref2 = *fTrackCandArray;
    size = clref2.GetEntriesFast();
    PndTrackCand *outputtrackcand = new(clref2[size]) PndTrackCand(theTrack.GetTrackCand());
    
    //         cout << "MOM FIRST: TOT, PT, PL " << outputtrack->GetParamFirst().GetMomentum().Mag() << " " << outputtrack->GetParamFirst().GetMomentum().Perp() << " " << outputtrack->GetParamFirst().GetMomentum().Z() << endl;
    //     cout << "MOM LAST: TOT, PT, PL " << outputtrack->GetParamLast().GetMomentum().Mag() << " " << outputtrack->GetParamLast().GetMomentum().Perp() << " " << outputtrack->GetParamLast().GetMomentum().Z() << endl;
    
    if(fDisplayOn) {
      char goOnChar;
      display->cd(1);
      track->Draw(kRed);
      display->Update();
      display->Modified();
     
      cout << "TRACK " << itrk << endl;
      //      cout << "MOM FIRST: TOT, PT, PL " << outputtrack->GetParamFirst().GetMomentum().Mag() << " " << outputtrack->GetParamFirst().GetMomentum().Perp() << " " << outputtrack->GetParamFirst().GetMomentum().Z() << " nofhits " << outputtrackcand->GetNHits() <<  endl;
      cout << "X, Y, R " << track->GetCenter().X() << " " << track->GetCenter().Y() << " " << track->GetRadius() << endl;
      //       cout << "Z0, TANL " << track->GetZ0() << " " << track->GetTanL() << endl;
      //       cout << "CHARGE " <<  track->GetCharge() << endl;
      cin >> goOnChar;
    }
    
  }

  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // FFFFF W   W DDD 
  // F     W   W D  D
  // FFFF  W W W D   D
  // F     W W W D   D
  // F      W W  DDDD

  fDisplayOn = kTRUE;
  // set unusable the hits assigned to long tracks ....................................
  for(int itrk = 0; itrk < fTrackArray->GetEntriesFast(); itrk++) {
    PndTrack *trk = (PndTrack*) fTrackArray->At(itrk);
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
      }
      // mvd str
      else if(detId == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) {
	hit = mvdstrhitlist->GetHitByID(hitId);
      }
      // stt
      else if(detId == FairRootManager::Instance()->GetBranchId(fSttBranch)) {
	hit = stthitlist->GetHitByID(hitId);
      }
      // scitil;
      else if(detId == FairRootManager::Instance()->GetBranchId(fSciTBranch)) {
	hit = scithitlist->GetHitByID(hitId);
      }
      // gem
      else if(detId == FairRootManager::Instance()->GetBranchId(fGemBranch)) {
	hit = gemhitlist->GetHitByID(hitId);
      }
      hit->SetUsedFlag(true);

    } 
    
  }

  if(fDisplayOn)  {
    Refresh();

    for(int itrk = 0; itrk < fTrackArray->GetEntriesFast(); itrk++) {
      PndTrack *trk = (PndTrack*) fTrackArray->At(itrk);
      PndTrkTrack track(trk);
      track.Draw();
    }


    char goOnChar;
    display->Update();
    display->Modified();
    cin >> goOnChar;
  }
  // ........................................................................................

  // Lets start from the GEM stations
  PndTrkCluster gemcluster;

  // sort by layer id 1, 2, 3, 4, 5, 6
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

  for(int ihit = 0; ihit < gemcluster.GetNofHits(); ihit++) {
    PndTrkHit *hiti = gemcluster.GetHit(ihit);
    hiti->SetUsedFlag(kFALSE);
  }

  PndTrkClusterList newclusterlist;

  // make tracklets out of the gem hits ..............................
  cout << "GEM CKLUSTER " << gemcluster.GetNofHits() << endl;
  for(int ihit = 0; ihit < gemcluster.GetNofHits(); ihit++) {
    PndTrkHit *hiti = gemcluster.GetHit(ihit);
    if(hiti->IsUsed() == kTRUE) continue;
    PndTrkCluster cluster;
    cluster.AddHit(hiti);

    if(fDisplayOn)  {
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

	if(fDisplayOn)  {
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


      // --------------------------------------------------------------
      // FIT THE CLUSTER 
      //
      if(fDisplayOn)  {
	Refresh();
	cluster.LightUp();
	char goOnChar;
	display->Update();
	display->Modified();
	cin >> goOnChar;
      }
	
      TVector3 stat1(0, 0, 0), stat2(0, 0, 0), stat3(0, 0, 0);
      int nstat1 = 0, nstat2 = 0, nstat3 = 0;
      for(int jhit = 0; jhit < cluster.GetNofHits(); jhit++) {
	hit = cluster.GetHit(jhit);
	int sensid = hit->GetSensorID();
	
	if(sensid <= 1) { stat1 += hit->GetPosition(); nstat1++; }
	else if(sensid <= 3) { stat2 += hit->GetPosition(); nstat2++; }
	else if(sensid <= 5) { stat3 += hit->GetPosition(); nstat3++; }
      }
      if(nstat1 == 0 || nstat2 == 0 || nstat3 == 0) continue;
      
      stat1 *= (1./nstat1);
      stat2 *= (1./nstat2);
      stat3 *= (1./nstat3);
      
      double alpha = 0.5 * (stat1.X() * stat1.X() - stat2.X() * stat2.X() + stat1.Y() * stat1.Y() - stat2.Y() * stat2.Y()) / (stat1.X() - stat2.X());
      double beta = (stat1.Y() - stat2.Y())/(stat1.X() - stat2.X());
      
      double yc0 = (alpha * (stat1.X() - stat3.X()) - 0.5 * (stat1.Y() * stat1.Y() - stat3.Y() * stat3.Y()) - 0.5 * (stat1.X() * stat1.X() - stat3.X() * stat3.X())) / ((stat1.X() - stat3.X()) * beta - (stat1.Y() - stat3.Y()));
      
      double xc0 = alpha - beta * yc0;
      
      double R0 = TMath::Sqrt((stat1.X() - xc0) * (stat1.X() - xc0) + (stat1.Y() - yc0) * (stat1.Y() - yc0));
      if(fDisplayOn)  {
	char goOnChar;
	display->cd(1);
	TArc *arcm = new TArc(xc0, yc0, R0);
	arcm->SetFillStyle(0);
	arcm->SetLineColor(3);
	arcm->Draw("SAME");
	display->Update();
	display->Modified();
	cin >> goOnChar;
      }

      newclusterlist.AddCluster(&cluster);
    
      // ..............................................................
      
      /**
       // --------------------------------------------------------------
       // FIT THE CLUSTER 
       //
       for(int iclus = 0; iclus < newclusterlist.GetNofClusters(); iclus++) {
       PndTrkCluster* cluster = newclusterlist.GetCluster(iclus);
       cout << "cluster " << cluster->GetNofHits() << endl;
	
       if(fDisplayOn)  {
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
 
       if(fDisplayOn) {
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
       if(fDisplayOn) {
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
       if(fDisplayOn) {
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
      **/


      double xc = xc0;
      double yc = yc0;
      double R = R0;
      if(fDisplayOn)  {
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
      for(int jhit = 0; jhit < cluster.GetNofHits(); jhit++) 
	{
	  hit = cluster.GetHit(jhit);
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
 
      // does it cross the target pipe?  
      bool crossL = false;
      double y1left = 0, y2left = 0;
      double delta1 = R * R - (0.5 * PIPEDIAMETER - xc) * (0.5 * PIPEDIAMETER - xc);
      cout << "delta1 " << delta1 << endl;
      if(delta1 == 0) {
	y1left =  yc;
	crossL = true;
      }
      else if(delta1 > 0) {
	y1left =  yc + TMath::Sqrt(delta1);
	y2left =  yc - TMath::Sqrt(delta1);
	crossL = true;
      }

      bool crossR = false;
      double y1right = 0, y2right = 0;
      double delta2 = R * R - (-0.5 * PIPEDIAMETER - xc) * (-0.5 * PIPEDIAMETER - xc);
      if(delta2 == 0) {
	y1right =  yc;
	crossR = true;
      }
      else if(delta2 > 0) {
	y1right =  yc + TMath::Sqrt(delta2);
	y2right =  yc - TMath::Sqrt(delta2);
	crossR = true;
      }
      cout << "CROSS? " << crossL << " " << crossR << endl;
      bool crossingpipe = false;
      if(crossL && crossR) {
	double ycross = 0.5 * (y1left + y1right);
	if(fabs(ycross) < CTOUTRADIUS  && fabs(ycross) > CTINRADIUS)  crossingpipe = true;
	ycross = 0.5 * (y2left + y2right);
	if(fabs(ycross) < CTOUTRADIUS  && fabs(ycross) > CTINRADIUS)  crossingpipe = true;

	cout << ycross << " " << y1left << " " << y1right << " " << y2left << " " << y2right << endl;
      }


      if(crossingpipe) {
	cout << "************* CROSSING PIPE *****************" << endl;
	crossingpipe = true;
      }

      // ----------------------------------------- 
      // === CREATE CLUSTER - taken from PndTrkFinder ---> will be put in a fctn
      // in the same sector or 
      // nearby if at the limit of two sectors <<<<<<<<<<<<------------------
      PndTrkCluster cluster2(cluster);
   
      // create cluster depending on fitting
      for(int jhit = 0; jhit < stthitlist->GetNofHits(); jhit++) {
	hit = stthitlist->GetHit(jhit);
	if(hit->IsUsed() == kTRUE) continue;
	if(hit->IsSttParallel() == kFALSE) continue;

	if(crossingpipe == true) {
	  if(((hit->GetSector() == 0 && sectorID == 5) && !(hit->GetSector() == 2 && sectorID == 3) && !(hit->GetSector() == 5 && sectorID == 0) && !(hit->GetSector() == 3 && sectorID == 2)) && searchnearby == false) continue;
	}
	else if(searchnearby == true) { // search in this sector and +/- 1
	  if(fabs(hit->GetSector() - sectorID) > 1 && !(hit->GetSector() == 5 && sectorID == 0) && !(hit->GetSector() == 0 && sectorID == 5)) continue;
	}
	else {                     // search only in this sector and possible othersector
	  if(border == false && hit->GetSector() != sectorID) continue;
	  else if(border == true && (hit->GetSector() != sectorID && hit->GetSector() != othersecID)) continue;
	}  
	double distance = hit->GetXYDistance(TVector3(xc, yc, 0.));
      
	double distance_hit_center = (hit->GetPosition().XYvector() - TVector2(xc, yc)).Mod();
	double recoiso = fabs(distance_hit_center - R);

	if(recoiso < 3.) cluster2.AddHit(hit);
	
      }
      

      // GET SECTOR ID ---------------------------------------------
      sectorids.clear();
      for(int jhit = 0; jhit < cluster2.GetNofHits(); jhit++) 
	{
	  hit = cluster2.GetHit(jhit);
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
      for(int jhit = 0; jhit < mvdpixhitlist->GetNofHits(); jhit++) {
	hit = mvdpixhitlist->GetHit(jhit);
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
	if(recoiso < 3.) {
	  cluster2.AddHit(hit);
	  if(fDisplayOn)  {
	    hit->Draw(kOrange);
	    display->Update();
	    display->Modified();
	  }  
	  //     PndTrkConformalHit *chit = conform->GetConformalHit(hit);
	  //        conformalhitlist->AddHit(chit);  
	}
      }

      // ... and put mvd strip hits to conformal plane --------------- 
      for(int jhit = 0; jhit < mvdstrhitlist->GetNofHits(); jhit++) {
	hit = mvdstrhitlist->GetHit(jhit);
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
	if(recoiso < 3.) {
	  cluster2.AddHit(hit);
	  if(fDisplayOn)  {
	    hit->Draw(kOrange);
	    display->Update();
	    display->Modified();
	  }  
	  PndTrkConformalHit chit = conform->GetConformalHit(hit); 
	  fConformalHitList->AddHit(&chit);  
	}
      }
    
      if(fDisplayOn) {
	display->cd(1);
	cluster2.Draw(kRed);
	display->Update();
	display->Modified();
	char goOnChar;
	cout << "want to go on?" << endl;
	cin >> goOnChar;
      } 
  

      // -------- sorting 

      // sorting with getPhi
      //      PndTrkTrack track2(&cluster2, xc, yc, R);

      //       for(int jhit = 0; jhit < cluster2.GetNofHits(); jhit++) {
      // 	hit = cluster2.GetHit(jhit);
      // 	double phi = track2.ComputePhi(hit->GetPosition());
      // 	hit->SetPhi(phi);
      // 	int detId = hit->GetDetectorID();
      // 	hit->SetSortVariable(phi);

      // // 	if(detId == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) hit->SetSortVariable(phi);
      // // 	else if(detId == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) hit->SetSortVariable(phi + 361);
      // // 	else if(detId == FairRootManager::Instance()->GetBranchId(fSttBranch)) hit->SetSortVariable(phi + 621);
      // // 	else if(detId == FairRootManager::Instance()->GetBranchId(fGemBranch)) hit->SetSortVariable(phi + 983);
      //       }
      //       cluster2.Sort();
    
      //       // suppose the tracks are coming from inside to outside
      //       if(cluster2.GetHit(0)->IsGem() && (cluster2.GetHit(cluster2.GetNofHits() - 1)->IsMvdPixel() || cluster2.GetHit(cluster2.GetNofHits() - 1)->IsMvdStrip()) {
      // 	cluster2.ReverseSort();
      //       }
      
      // more complex sorting
      // sort in z ....
      for(int jhit = 0; jhit < cluster2.GetNofHits(); jhit++) {
	hit = cluster2.GetHit(jhit);
	hit->SetSortVariable(hit->GetPosition().Z());
      }
      cluster2.Sort();
      // ... then sort in phi from the 1st hit
      PndTrkTrack track2(&cluster2, xc, yc, R);
      cout << "xc, yc, R " << xc << " " << yc << " " << R << endl;
      for(int jhit = 0; jhit < cluster2.GetNofHits(); jhit++) {
	hit = cluster2.GetHit(jhit);
	double phi = track2.ComputePhiFrom(hit->GetPosition(), cluster2.GetHit(0)->GetPosition());
	//	double phi = track2.ComputePhi(hit->GetPosition());
	hit->SetPhi(phi);
	hit->SetSortVariable(phi);
      }
      cluster2.Sort();

      // if the track is positive --> add 360 (otherwise is 0 followed by 350, 350...)
      track2.ComputeCharge();
      double charge = track2.GetCharge();
      cout << "charge " << charge << endl;
      if(charge > 0)      cluster2.GetHit(0)->SetPhi(360.) ; // CHECK

      // further check, but maybe nomore necessary CHECK
      if(cluster2.GetHit(0)->IsGem() && (cluster2.GetHit(cluster2.GetNofHits() - 1)->IsMvdPixel() || cluster2.GetHit(cluster2.GetNofHits() - 1)->IsMvdStrip())) {
	PndTrkCluster cluster2b;
	cluster2b.AddHit(cluster2.GetHit(0));
	cluster2.ReverseSort();
	for(int jhit = 1; jhit < cluster2.GetNofHits(); jhit++) {
	  cluster2b.AddHit(cluster2.GetHit(jhit));
	}
	cluster2 = cluster2b;
      }
      // ..........................................


      for(int jhit = 0; jhit < cluster2.GetNofHits(); jhit++) {
	hit = cluster2.GetHit(jhit);
	cout << "hit " << hit->GetHitID() << " " << hit->GetDetectorID() << " " << hit->GetPhi() << " " << hit->GetSortVariable() << endl;
     }

      // first hypothesis of z slope
      if(fDisplayOn) DrawZGeometry(-360, 360, -40, 200);
      
      // -------- fit with a line
      /**
	 fFitter->Reset();    
	 for(int jhit = 0; jhit < cluster2.GetNofHits(); jhit++) {
	 hit = cluster2.GetHit(jhit);
	 if(hit->IsStt()) continue;
	 TVector3 position = hit->GetPosition();
	 double phi = hit->GetPhi();
	 cout << " z part " << phi << " " << position.Z() << endl;
	 if(fDisplayOn) {
	 char goOnChar;
	 display->cd(4);
	 TMarker *mrkz = NULL;
	 if(hit->IsMvdPixel()) mrkz = new TMarker(phi, position.Z(), 21);
	 else if(hit->IsMvdStrip()) mrkz = new TMarker(phi, position.Z(), 25);
	 else if(hit->IsStt()) mrkz = new TMarker(phi, position.Z(), 6);
	 else if(hit->IsGem()) mrkz = new TMarker(phi, position.Z(), 24);

	 mrkz->SetMarkerColor(kBlue);
	 mrkz->Draw("SAME");
	 display->Update();
	 display->Modified();
	 // 	  cin >> goOnChar;   
	 }
	 
	 fFitter->SetPointToFit(phi, position.Z(), 0.1); // CHECK the error?
	 }
      
	 double fitm4, fitq4;
	 fFitter->StraightLineFit(fitm4, fitq4);

	 if(fDisplayOn) {
	 display->cd(4);
	 TLine *line = new TLine(0, fitq4, 360, 360 * fitm4 + fitq4);
	 line->SetLineColor(4);
	 line->Draw("SAME");
	 display->Update();
	 display->Modified();
	 char goOnChar;
	 cin >> goOnChar;
	 }
      **/

      TH2F *fLineHisto = new TH2F("fLineHisto", "hl", 720, -360, 360, 8000, -400, 400);

      for(int jhit = 0; jhit < cluster2.GetNofHits(); jhit++) {
    	hit = cluster2.GetHit(jhit);
	if(hit->IsStt()) continue;
	TVector3 position = hit->GetPosition();
	double phi = hit->GetPhi();
	if(fDisplayOn) {
	  char goOnChar;
	  display->cd(4);
	  TMarker *mrkz = NULL;
	  if(hit->IsMvdPixel()) mrkz = new TMarker(phi, position.Z(), 21);
	  else if(hit->IsMvdStrip()) mrkz = new TMarker(phi, position.Z(), 25);
	  else if(hit->IsStt()) mrkz = new TMarker(phi, position.Z(), 6);
	  else if(hit->IsGem()) mrkz = new TMarker(phi, position.Z(), 24);
	  
	  mrkz->SetMarkerColor(kBlue);
	  mrkz->Draw("SAME");
	  display->Update();
	  display->Modified();
	  // 	  cin >> goOnChar;   
	}
	for(int khit = jhit + 1; khit < cluster2.GetNofHits(); khit++) {
	  PndTrkHit*  hitk = cluster2.GetHit(khit);
	  if(hitk->IsStt()) continue; 
	  TVector3 positionk = hitk->GetPosition();
	  double phik = hitk->GetPhi();
	  
	  double cost = (positionk.Z() - position.Z()) / TMath::Sqrt((phi - phik) * (phi - phik) + (positionk.Z() - position.Z()) * (positionk.Z() - position.Z()));
	  double theta = TMath::ACos(cost);
	  double r1 = phi * cost + position.Z() * TMath::Sin(theta);
	  double r2 = phik * cost + positionk.Z() * TMath::Sin(theta);
	  double r = r1;
	  if(fabs(r1 - r2) > 1.e-9) {
	    theta = -TMath::ACos(cost);
	    r = phi * cost + position.Z() * TMath::Sin(theta);
	  }
	  fLineHisto->Fill(theta * TMath::RadToDeg(), r);
	  //	  cout << theta * TMath::RadToDeg() << " " <<  r << endl;
	}
      }
      int bin = fLineHisto->GetMaximumBin();
      int binx, biny, binz;
      fLineHisto->GetBinXYZ(bin, binx, biny, binz);
      double tpeak = fLineHisto->GetXaxis()->GetBinCenter(binx);
      double rpeak = fLineHisto->GetYaxis()->GetBinCenter(biny);
      //  cout << "tpeak " << tpeak << " rpeak " << rpeak << endl;
      double fitm4 = -TMath::Cos(tpeak * TMath::DegToRad())/TMath::Sin(tpeak * TMath::DegToRad());
      double fitq4 = rpeak/TMath::Sin(tpeak * TMath::DegToRad());
 
      if(fDisplayOn) {
	display->cd(3);
	fLineHisto->Draw("colz");
	display->cd(4);
	TLine *line = new TLine(-360, -360 * fitm4 + fitq4, 360, 360 * fitm4 + fitq4);
	line->SetLineColor(3);
	line->Draw("SAME");
	display->Update();
	display->Modified();
	char goOnChar;
	cin >> goOnChar;
      }

      // retrieve the hits shifted by 360 deg
      for(int jhit = 0; jhit < cluster2.GetNofHits(); jhit++) {
	hit = cluster2.GetHit(jhit);
	if(hit->IsStt()) continue;
	TVector3 position = hit->GetPosition();
	double phi = hit->GetPhi();
	double distance =  fabs(position.Z() - fitm4 * phi - fitq4);
	
	// retrieve the hits shifted by 360
	if(distance > 100) {
	  // try with 360 deg shift
	  double comp_phi = (position.Z() - fitq4)/fitm4;
	  if(fabs(comp_phi - phi) < 361 &&  fabs(comp_phi - phi) > 359){
	    if(comp_phi > phi) phi += 360.;
	    else phi -= 360.;
	    hit->SetPhi(phi);
	    distance =  fabs(position.Z() - fitm4 * phi - fitq4);
	    cout << "PHI possible! " << fabs(comp_phi - phi) << endl;
	    
	  }
	}
      }

      // associate only the right hits in z for gem & mvd
      PndTrkCluster cluster3;
      for(int jhit = 0; jhit < cluster2.GetNofHits(); jhit++) {
	hit = cluster2.GetHit(jhit);
	cout << " hit in cluster2: " << jhit << " " << hit->GetDetectorID() << endl;
	if(hit->IsStt()) {
	  cluster3.AddHit(hit);
	  cout << " add hit " << jhit << endl;
	}
	else {
	  TVector3 position = hit->GetPosition();
	  double phi = hit->GetPhi();
	  
	  double distance =  fabs(position.Z() - fitm4 * phi - fitq4);
	  // cout << " z part " << phi << " " << position.Z() << " " << distance << endl;

	  if(distance > 3 && hit->IsMvd() == kTRUE) {
	    int hitId = hit->GetHitID();
	    PndTrkHit *thishit = NULL;
	    if(hit->IsMvdPixel()) thishit = mvdpixhitlist->GetHitByID(hitId);
	    else if(hit->IsMvdStrip()) thishit = mvdstrhitlist->GetHitByID(hitId);
	    thishit->SetUsedFlag(kFALSE);
	    cout << "distance " << distance << " of hit " << hit->GetDetectorID() << endl;
	    continue;
	  }
	  else if(distance > 20 && hit->IsGem() == kTRUE) {
	    int hitId = hit->GetHitID();
	    PndTrkHit *thishit = gemhitlist->GetHitByID(hitId);
	    thishit->SetUsedFlag(kFALSE);
	    cout << "distance " << distance << " of hit " << hit->GetDetectorID() << endl;
	    continue;
	  }


	  if(fDisplayOn) {
	    char goOnChar;
	    display->cd(4);
	    TMarker *mrkz = new TMarker(phi, position.Z(), 20);
	    mrkz->SetMarkerColor(kOrange);
	    mrkz->Draw("SAME");
	    display->Update();
	    display->Modified();
	  }
	  cout << " add hit " << jhit << endl;
	  cluster3.AddHit(hit);
	}
      }
      cluster3.Sort();

      // -------- fit with a line
      fFitter->Reset();    
      for(int jhit = 0; jhit < cluster3.GetNofHits(); jhit++) {
	hit = cluster3.GetHit(jhit);
	if(hit->IsStt()) continue;
	TVector3 position = hit->GetPosition();
	double phi = hit->GetPhi();
	cout << " z part " << phi << " " << position.Z() << endl;
	if(fDisplayOn) {
	  char goOnChar;
	  display->cd(4);
	  TMarker *mrkz = NULL;
	  if(hit->IsMvdPixel()) mrkz = new TMarker(phi, position.Z(), 21);
	  else if(hit->IsMvdStrip()) mrkz = new TMarker(phi, position.Z(), 25);
	  else if(hit->IsStt()) mrkz = new TMarker(phi, position.Z(), 6);
	  else if(hit->IsGem()) mrkz = new TMarker(phi, position.Z(), 24);
	  
	  mrkz->SetMarkerColor(kBlue);
	  mrkz->Draw("SAME");
	  display->Update();
	  display->Modified();
	  // 	  cin >> goOnChar;   
	}
	
	fFitter->SetPointToFit(phi, position.Z(), 0.1); // CHECK the error?
      }
      
      double fitm5, fitq5;
      fFitter->StraightLineFit(fitm5, fitq5);
      
      if(fDisplayOn) {
	display->cd(4);
	TLine *line = new TLine(0, fitq4, 360, 360 * fitm4 + fitq4);
	line->SetLineColor(4);
	line->Draw("SAME");
	display->Update();
	display->Modified();
	char goOnChar;
	cin >> goOnChar;
      }
      
      fitm4 = fitm5;
      fitq4 = fitq5;
      // REFIT ANALYTICALLY ....again taken from-..
      // -------------------------- 
      // if there is a scitil lets use it as seed hit
      // for the conformal map
      fConformalHitList->Clear("C");
      PndTrkHit *refhit = cluster3.GetHit(0); // cluster3.GetNofHits() - 1);
      double trasl[2] = {0, 0};
      double delta;

      cout << "refhit ---> " << refhit << endl;

      ComputeTraAndRot(refhit, delta, trasl);
      conform->SetOrigin(trasl[0], trasl[1], delta);
      fConformalHitList->SetConformalTransform(conform);

      int nofconfhits = FillConformalHitList(&cluster3);
      
      // compute conformal plane extremities ---------------------------- this must go to a fctn CHECK
      fUmin =  1000, fVmin =  1000, fRmin =  1000;
      fUmax = -1000, fVmax = -1000, fRmax = -1000;
      double rc_of_min, rc_of_max;
    
      for(int jhit = 0; jhit < fConformalHitList->GetNofHits(); jhit++) {
	PndTrkConformalHit *chit = fConformalHitList->GetHit(jhit);
	double u = chit->GetU();
	double v = chit->GetV();
	double rc = chit->GetIsochrone();
	if(TMath::IsNaN(u)) continue; // prevents the nan of the ref hit
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
      double du = fUmax - fUmin;
      double dv = fVmax - fVmin;
      double delt = fabs(dv - du)/2.;
      du < dv ? (fUmin -= delt, fUmax += delt) : (fVmin -= delt, fVmax += delt);
      cout << "min/max " << fUmin << " " << fUmax << " " << fVmin << " " << fVmax << endl;
      if(fDisplayOn) {
	DrawGeometryConf(fUmin, fUmax, fVmin, fVmax);
	// ----------------------------------------------------------------------
	for(int jhit = 0; jhit < nofconfhits; jhit++) {
	  PndTrkConformalHit *chit = fConformalHitList->GetHit(jhit);
	  chit->Draw(kRed);
	}
      }
  
      // ====== REFIT CLUSTER ANALYTICALLY
       
      double fitm3, fitq3;
      AnalyticalFit(&cluster3, xc, yc, R, fitm3, fitq3);
       
      double xc3, yc3, R3;
      FromConformalToRealTrack(fitm3, fitq3, xc3, yc3, R3);
  
      if(fDisplayOn)  {
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
  }
  
  
  // -----------------------------------------------------------
  //   Reset();
  //   fFinalCluster->Clear("C");
  //   fTrackList->Clear("C");
  //   fConformalHitList->Clear("C"); 
  

  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //  SSS  H   H  OOO  RRRR  TTTTT
  // S     H   H O   O R   R   T
  //  SSS  HHHHH O   O RRRR    T
  //     S H   H O   O R  R    T
  //  SSS  H   H  OOO  R   R   T
  
  if(fDisplayOn) {
    char goOnChar;
    display->Update();
    display->Modified();
    cout << "Finish? ";
    cin >> goOnChar;
  }
  Reset();
  //  delete fConformalHitList;
  //  delete indhit;
  fFinalCluster->Clear("C");
  fTrackList->Clear("C");
  fConformalHitList->Clear("C"); 
  
//   delete stthit;
//   delete indiv;
}



void PndTrkTrackFinder::Reset()
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
    if(stthitlist)    delete stthitlist;
    if(mvdpixhitlist) delete mvdpixhitlist; 
    if(mvdstrhitlist) delete mvdstrhitlist;
    if(scithitlist)   delete scithitlist;
    if(gemhitlist)    delete gemhitlist;
    if(fTimer) {
      fTimer->Stop();
      fTime += fTimer->RealTime();
      
      if(fVerbose > 0) cerr << fEventCounter << " Real time " << fTime << " s" << endl;
    }
  }

  fInitDone = kFALSE;
}


// void PndTrkTrackFinder::Apollonius(PndTrkCluster *cluster, double *X, double *Y, double *R)
void PndTrkTrackFinder::Apollonius(PndTrkCluster *cluster, std::vector< double > &X,  std::vector< double > &Y,  std::vector< double > &R)
{ 

  if(fDisplayOn) {
    TH2F *h2 = new TH2F("h2", "", 100, -70, 70, 100, -70, 70);
    display->cd(2);
    h2->Draw();

    display->Update();
    display->Modified();
  }

  int ncircles = cluster->GetNofHits();
  double circle[ncircles][3];
  
  for(int i = 0; i < ncircles; i++)
    {
      PndTrkHit *hit = cluster->GetHit(i);
      double xc = hit->GetPosition().X();
      double yc = hit->GetPosition().Y();
      double rc = hit->GetIsochrone();

      circle[i][0] = xc;
      circle[i][1] = yc;
      circle[i][2] = rc;
      //      cout << xc << " " << yc << "  " << rc << endl;

      if(fDisplayOn) {
	char goOnChar;
	display->cd(2);  
	TArc *arc = new TArc(xc, yc, rc);
	arc->SetFillStyle(0);
	arc->Draw("SAME");
	display->Update();
	display->Modified();
      }
    }

  int counter = 0;
  for(int a = 0; a < ncircles; a++) {
    for(int b = a + 1; b < ncircles; b++) {
      for(int c = b + 1; c < ncircles; c++) {
	for(int csign1 = -1; csign1 <= 1; csign1 += 2) {
	  for(int csign2 = -1; csign2 <= 1; csign2 += 2) {
	    double avar   = 2 * (circle[a][0] - circle[b][0]);
	    double bvar   = 2 * (circle[a][1] - circle[b][1]);
	    double cvar = 2 * (csign1 * circle[a][2] + csign2 * circle[b][2]);
	    double dvar = (circle[a][0] * circle[a][0] + circle[a][1] * circle[a][1] - circle[a][2] * circle[a][2]) - (circle[b][0] * circle[b][0] + circle[b][1] * circle[b][1] - circle[b][2] * circle[b][2]);
	    
	    for(int csignp = -1; csignp <= 1; csignp += 2) {
	      double avarp   = 2 * (circle[a][0] - circle[c][0]);
	      double bvarp   = 2 * (circle[a][1] - circle[c][1]);
	      double cvarp = 2 * (csign1 * circle[a][2] + csignp * circle[c][2]);
	      double dvarp = (circle[a][0] * circle[a][0] + circle[a][1] * circle[a][1] - circle[a][2] * circle[a][2]) - (circle[c][0] * circle[c][0] + circle[c][1] * circle[c][1] - circle[c][2] * circle[c][2]);
	      
	      // ======================================
	      double thisq = (dvar * bvarp - dvarp * bvar) / (avar * bvarp - avarp * bvar);
	      double thism = (bvar * cvarp - bvarp * cvar) / (avar * bvarp - avarp * bvar);
	  
	      double thisp = (avar * dvarp - avarp * dvar) / (avar * bvarp - avarp * bvar);
	      double thiss = (cvar * avarp - cvarp * avar) / (avar * bvarp - avarp * bvar);
	      //	    cout << thism  << " " << thisq << " " << thiss << " " << thisp << endl;
	      // ======================================
	      // thisa x**2 + 2 * thisb x + thisc = 0
	  
	      double thisa = thism * thism + thiss * thiss - 1;
	      double thisb = thism * (thisq - circle[a][0]) + thiss * (thisp - circle[a][1]) - csign1 * circle[a][2];
	      double thisc =  (thisq - circle[a][0]) *  (thisq - circle[a][0]) + (thisp - circle[a][1]) * (thisp - circle[a][1]) - circle[a][2] * circle[a][2];

	      if((thisb * thisb - thisa * thisc) < 0) continue;
	      double r = (- thisb + TMath::Sqrt(thisb * thisb - thisa * thisc))/ thisa;
// 	      cout << "r1 " << r << endl;
	      if(r < 0) {
		r = (- thisb - TMath::Sqrt(thisb * thisb - thisa * thisc))/ thisa;
	// 	cout << "r2 " << r << endl;
	      }

	      if(r < 0) continue; // CHECK

	     //  R[counter] = r;
// 	      X[counter] = thism * R[counter] + thisq;
// 	      Y[counter]  = thiss * R[counter] + thisp;

	      R.push_back(r);
	      X.push_back(thism * R[counter] + thisq);
	      Y.push_back(thiss * R[counter] + thisp);
	      cout << R[counter] << " " << X[counter] << " " << Y[counter] << endl;
	     //  if(fDisplayOn) {
// 		char goOnChar;
// 		display->cd(2); 
// 		TArc *arc0 = new TArc(X[counter], Y[counter], R[counter]);
// 		arc0->SetFillStyle(0);
// 		arc0->SetLineColor(2);
// 		arc0->Draw("SAME");
// 		display->Update();
// 		display->Modified();
// 		//	cin >> goOnChar;
// 	      }


	  //     cout << "ctr/dist/R " << counter << " " << TMath::Sqrt((circle[a][0] - X[counter]) * (circle[a][0] - X[counter]) + (circle[a][1] - Y[counter]) * (circle[a][1] - Y[counter])) - R[counter] << " " << R[counter] << endl;
	      counter++;
	     //  cout << endl;
	    }
	  }
	}
      }
    }
  }
}

void PndTrkTrackFinder::CircleBy3Points(PndTrkHit* hit1, PndTrkHit * hit2, PndTrkHit * hit3, double &X,  double &Y,  double &R)
{ 

  int ncircles = 3;
  double circle[ncircles][3];
  
  circle[0][0] = hit1->GetPosition().X();
  circle[0][1] = hit1->GetPosition().Y();
  circle[0][2] = hit1->GetIsochrone();

  circle[1][0] = hit2->GetPosition().X();
  circle[1][1] = hit2->GetPosition().Y();
  circle[1][2] = hit2->GetIsochrone();

  circle[2][0] = hit3->GetPosition().X();
  circle[2][1] = hit3->GetPosition().Y();
  circle[2][2] = hit3->GetIsochrone();

  for(int a = 0; a < ncircles; a++) {
    for(int b = a + 1; b < ncircles; b++) {
      for(int c = b + 1; c < ncircles; c++) {
	Y = 0.5 * ((circle[a][0] - circle[b][0]) * (circle[a][0] * circle[a][0] - circle[c][0] * circle[c][0] + circle[a][1] * circle[a][1] - circle[c][1] * circle[c][1]) - (circle[a][0] - circle[c][0]) * (circle[a][0] * circle[a][0] - circle[b][0] * circle[b][0] + circle[a][1] * circle[a][1] - circle[b][1] * circle[b][1])) / ((circle[a][1] - circle[c][1]) * (circle[a][0] - circle[b][0]) - (circle[a][1] - circle[b][1]) * (circle[a][0] - circle[c][0]));

	if(circle[a][0] != circle[b][0]) {
	  X = 0.5 * (circle[a][0] * circle[a][0] - circle[b][0] * circle[b][0] + circle[a][1] * circle[a][1] - circle[b][1] * circle[b][1] ) / (circle[a][0] - circle[b][0]) - Y * (circle[a][1] - circle[b][1])/(circle[a][0] - circle[b][0]);
	}
	else if(circle[a][0] != circle[c][0]) {
	  X = 0.5 * (circle[a][0] * circle[a][0] - circle[c][0] * circle[c][0] + circle[a][1] * circle[a][1] - circle[c][1] * circle[c][1] ) / (circle[a][0] - circle[c][0]) - Y * (circle[a][1] - circle[c][1])/(circle[a][0] - circle[c][0]);
	}
	
	R = TMath::Sqrt((circle[a][0] - X) * (circle[a][0] - X) + (circle[a][1] - Y) * (circle[a][1] - Y));
      }
    }
  }
}

void PndTrkTrackFinder::Apollonius(PndTrkHit* hit1, PndTrkHit * hit2, PndTrkHit * hit3, std::vector< double > &X,  std::vector< double > &Y,  std::vector< double > &R)
{ 

  int ncircles = 3;
  double circle[ncircles][3];
  
  circle[0][0] = hit1->GetPosition().X();
  circle[0][1] = hit1->GetPosition().Y();
  circle[0][2] = hit1->GetIsochrone();

  circle[1][0] = hit2->GetPosition().X();
  circle[1][1] = hit2->GetPosition().Y();
  circle[1][2] = hit2->GetIsochrone();

  circle[2][0] = hit3->GetPosition().X();
  circle[2][1] = hit3->GetPosition().Y();
  circle[2][2] = hit3->GetIsochrone();
  
  int counter = 0;
  for(int a = 0; a < ncircles; a++) {
    for(int b = a + 1; b < ncircles; b++) {
      for(int c = b + 1; c < ncircles; c++) {
	for(int csign1 = -1; csign1 <= 1; csign1 += 2) {
	  for(int csign2 = -1; csign2 <= 1; csign2 += 2) {
	    double avar   = 2 * (circle[a][0] - circle[b][0]);
	    double bvar   = 2 * (circle[a][1] - circle[b][1]);
	    double cvar = 2 * (csign1 * circle[a][2] + csign2 * circle[b][2]);
	    double dvar = (circle[a][0] * circle[a][0] + circle[a][1] * circle[a][1] - circle[a][2] * circle[a][2]) - (circle[b][0] * circle[b][0] + circle[b][1] * circle[b][1] - circle[b][2] * circle[b][2]);
	    
	    for(int csignp = -1; csignp <= 1; csignp += 2) {
	      double avarp   = 2 * (circle[a][0] - circle[c][0]);
	      double bvarp   = 2 * (circle[a][1] - circle[c][1]);
	      double cvarp = 2 * (csign1 * circle[a][2] + csignp * circle[c][2]);
	      double dvarp = (circle[a][0] * circle[a][0] + circle[a][1] * circle[a][1] - circle[a][2] * circle[a][2]) - (circle[c][0] * circle[c][0] + circle[c][1] * circle[c][1] - circle[c][2] * circle[c][2]);
	      
	      // ======================================
	      double thisq = (dvar * bvarp - dvarp * bvar) / (avar * bvarp - avarp * bvar);
	      double thism = (bvar * cvarp - bvarp * cvar) / (avar * bvarp - avarp * bvar);
	  
	      double thisp = (avar * dvarp - avarp * dvar) / (avar * bvarp - avarp * bvar);
	      double thiss = (cvar * avarp - cvarp * avar) / (avar * bvarp - avarp * bvar);
	      //	    cout << thism  << " " << thisq << " " << thiss << " " << thisp << endl;
	      // ======================================
	      // thisa x**2 + 2 * thisb x + thisc = 0
	  
	      double thisa = thism * thism + thiss * thiss - 1;
	      double thisb = thism * (thisq - circle[a][0]) + thiss * (thisp - circle[a][1]) - csign1 * circle[a][2];
	      double thisc =  (thisq - circle[a][0]) *  (thisq - circle[a][0]) + (thisp - circle[a][1]) * (thisp - circle[a][1]) - circle[a][2] * circle[a][2];

	      if((thisb * thisb - thisa * thisc) < 0) continue;
	      double r = (- thisb + TMath::Sqrt(thisb * thisb - thisa * thisc))/ thisa;
	      // 	      cout << "r1 " << r << endl;
	      if(r < 0) {
		r = (- thisb - TMath::Sqrt(thisb * thisb - thisa * thisc))/ thisa;
		// 	cout << "r2 " << r << endl;
	      }

	      if(r < 0) continue; // CHECK

	      //  R[counter] = r;
	      // 	      X[counter] = thism * R[counter] + thisq;
	      // 	      Y[counter]  = thiss * R[counter] + thisp;

	      R.push_back(r);
	      X.push_back(thism * R[counter] + thisq);
	      Y.push_back(thiss * R[counter] + thisp);
	      cout << R[counter] << " " << X[counter] << " " << Y[counter] << endl;
	      //  if(fDisplayOn) {
	      // 		char goOnChar;
	      // 		display->cd(2); 
	      // 		TArc *arc0 = new TArc(X[counter], Y[counter], R[counter]);
	      // 		arc0->SetFillStyle(0);
	      // 		arc0->SetLineColor(2);
	      // 		arc0->Draw("SAME");
	      // 		display->Update();
	      // 		display->Modified();
	      // 		//	cin >> goOnChar;
	      // 	      }

	      
	      //     cout << "ctr/dist/R " << counter << " " << TMath::Sqrt((circle[a][0] - X[counter]) * (circle[a][0] - X[counter]) + (circle[a][1] - Y[counter]) * (circle[a][1] - Y[counter])) - R[counter] << " " << R[counter] << endl;
	      counter++;
	      //  cout << endl;
	    }
	  }
	}
      }
    }
  }
}



// ============================================================================================
// DISPLAY  ***********************************************************************************
// ============================================================================================

void PndTrkTrackFinder::Refresh(){
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

void PndTrkTrackFinder::DrawLegendreHisto() {
  display->cd(3);
  legendre->Draw();
  display->Update();
  display->Modified();
}

void PndTrkTrackFinder::RefreshConf() { // CHECK delete
  // CHECK
  char goOnChar;
  //  cout << "RefreshConf?" << endl;
  //  cin >> goOnChar;
  //  cout << "REFRESHING CONF" << endl;
}

void PndTrkTrackFinder::DrawHits(PndTrkHitList *hitlist) {
  display->cd(1);
  hitlist->Draw();
  display->Update();
  display->Modified();
}

void PndTrkTrackFinder::DrawGeometry() {
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

void PndTrkTrackFinder::DrawGeometryConf(double x1, double x2, double y1, double y2) {
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

// draw lists ---------
void PndTrkTrackFinder::DrawLists() {
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


// draw neighborings ---------
void PndTrkTrackFinder::DrawNeighborings() {
  char goOnChar;
 
  for(int ihit = 0; ihit < stthitlist->GetNofHits(); ihit++) {
    cout << "new neigh hit?" << endl;
    cin >> goOnChar;
    PndTrkHit *hit = stthitlist->GetHit(ihit);
    DrawNeighboringsToHit(hit);
    cin >> goOnChar;
  }

}

void PndTrkTrackFinder::DrawNeighboringsToHit(PndTrkHit *hit) {

 
  Refresh(); 
  hit->DrawTube(kYellow);
  PndSttTube *tube = (PndSttTube*) fTubeArray->At(hit->GetTubeID());
  TObjArray neighs = fHitMap->GetNeighboringsToHit(hit);


  //  cout << "HIT " << hit->GetHitID() << "(" << hit->GetTubeID() << "/" << tube->GetLayerID() << ")" << " has " << neighs.GetEntriesFast() << " neighborings: ";
  for(int i = 0; i < neighs.GetEntriesFast(); i++) {
    PndTrkHit *hitA = (PndTrkHit*) neighs.At(i);
    hitA->DrawTube(kCyan);
    PndSttTube *tubeA = (PndSttTube*) fTubeArray->At(hitA->GetTubeID());
    cout << " " << hitA->GetHitID() << "(" << hitA->GetTubeID() << "/" << tubeA->GetLayerID() << ")";
  }
  cout << endl;
  display->Update();
  display->Modified();
  
}

void PndTrkTrackFinder::DrawConfHit(double u, double v, double r, int marker) {
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

Int_t PndTrkTrackFinder::FillConformalHitList(PndTrkCluster *cluster) { 
  
  // set the conformal transformation, where the
  // translation and rotation must be already set
  fConformalHitList->SetConformalTransform(conform);
    
  // loop over the cluster hits 
  // and port them to conf plane
  for(int jhit = 0; jhit < cluster->GetNofHits(); jhit++) {
    PndTrkHit *hit = cluster->GetHit(jhit);
    if(hit == fRefHit) continue;
    PndTrkConformalHit chit;
    // cout << "HIT " << hit->GetHitID() << " " << hit->IsSttParallel() << " " << hit->IsSttSkew() << endl;
    if(hit->IsSttParallel() == kTRUE) chit = conform->GetConformalSttHit(hit);
    else chit = conform->GetConformalHit(hit); // CHECK
    fConformalHitList->AddHit(&chit);  
    // cout << hit->GetPosition().X() << " " << hit->GetPosition().Y() << " " << hit->GetIsochrone() << " " << hit->IsSttParallel() << " " << " to CONFORMAL " << chit->GetU() << " " << chit->GetV() << " " << chit->GetIsochrone() << endl;

  }
  return fConformalHitList->GetNofHits();
}

// ----------------------- FOR SECONDARIES
PndTrkHit *PndTrkTrackFinder::FindSttReferenceHit(int isec) {
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
 
PndTrkHit *PndTrkTrackFinder::FindMvdPixelReferenceHit()
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

PndTrkHit *PndTrkTrackFinder::FindMvdStripReferenceHit()
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

PndTrkHit *PndTrkTrackFinder::FindMvdReferenceHit()
{
  PndTrkHit *refhit = NULL;
  refhit = FindMvdStripReferenceHit();
  // refhit = FindMvdPixelReferenceHit();
  if(refhit != NULL) return refhit;
  //  refhit = FindMvdStripReferenceHit();
  FindMvdPixelReferenceHit();
  return refhit;
}

PndTrkHit *PndTrkTrackFinder::FindReferenceHit()
{
  PndTrkHit *refhit = NULL;
  // refhit = FindMvdReferenceHit();
  refhit = FindSttReferenceHit();
  if(refhit != NULL) return refhit;
  // refhit = FindSttReferenceHit();
  FindMvdReferenceHit();

 return refhit;
}

PndTrkHit *PndTrkTrackFinder::FindReferenceHit(PndTrkCluster *cluster) {
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
 

void PndTrkTrackFinder::ComputeTraAndRot(PndTrkHit *hit, Double_t &delta, Double_t trasl[2]) {
  
  trasl[0] = hit->GetPosition().X();
  trasl[1] = hit->GetPosition().Y();

  delta = 0.; // TMath::ATan2(hit->GetPosition().Y() - 0., hit->GetPosition().X() - 0.); // CHECK 
  
}

void PndTrkTrackFinder::ComputePlaneExtremities(PndTrkCluster *cluster) {
  fUmin =  1000, fVmin =  1000, fRmin =  1000;
  fUmax = -1000, fVmax = -1000, fRmax = -1000;
  double rc_of_min, rc_of_max;
  
  
  for(int ihit = 0; ihit < fConformalHitList->GetNofHits(); ihit++) {
    PndTrkConformalHit *chit = fConformalHitList->GetHit(ihit);
    double u = chit->GetU();
    double v = chit->GetV();
    double rc = chit->GetIsochrone();
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
  double delta = fabs(dv - du)/2.;
  du < dv ? (fUmin -= delta, fUmax += delta) : (fVmin -= delta, fVmax += delta);
  
  cout << "u_min " << fUmin << " u_max " << fUmax << endl;
  cout << "v_min " << fVmin << " v_max " << fVmax << endl;
  cout << "r_min " << fRmin << " r_max " << fRmax << endl;
  cout << "theta_min 0 theta_max 180" << endl;
  
  
}

void PndTrkTrackFinder::FillLegendreHisto(PndTrkCluster *cluster)
{
  // ---------------------------------------------------------------
  //  cout << "FILL LEGENDRE HISTO " << cluster->GetNofHits() << endl;

  for(int ihit = 0; ihit < fConformalHitList->GetNofHits(); ihit++) {
    PndTrkConformalHit *chit = fConformalHitList->GetHit(ihit);
    PndTrkHit *hit = chit->GetHit();
    if(cluster->DoesContain(hit) == kFALSE) continue;
    legendre->FillLegendreHisto(chit->GetU(), chit->GetV(), chit->GetIsochrone());
    if(fDisplayOn) {
      DrawConfHit(chit->GetU(), chit->GetV(), chit->GetIsochrone());
      //      cout << "conformal2: " << chit->GetU() << " " << chit->GetV() << " " << chit->GetIsochrone() << endl;   
    }
  }
}
 
void PndTrkTrackFinder::RePrepareLegendre(PndTrkCluster *cluster) {

  //    cout << "RESETTING LEGENDRE HISTO" << endl;
  legendre->ResetLegendreHisto();
 
  if(fDisplayOn) {
    RefreshConf();
    DrawGeometryConf(fUmin, fUmax, fVmin, fVmax);
  }
  // cout << "%%%%%%%%%%%%%%%%%%%% XY FINDE %%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
  FillLegendreHisto(cluster);
}

Int_t PndTrkTrackFinder::ApplyLegendre(PndTrkCluster *cluster, double &theta_max, double &r_max) {
  RePrepareLegendre(cluster);
  return ExtractLegendre(1, theta_max, r_max);
}



Int_t  PndTrkTrackFinder::ExtractLegendre(Int_t mode, double &theta_max, double &r_max) {
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
      if(fVerbose > 1)  cout << "\033[1;31m MAXPEAK " << maxpeak <<  ", BREAK NOW! \033[0m" << endl;
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

  for(int ihit = 0; ihit < fConformalHitList->GetNofHits(); ihit++) {
    PndTrkConformalHit *chit = fConformalHitList->GetHit(ihit);
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

void PndTrkTrackFinder::FromConformalToRealTrack(double fitm, double fitp, double &x0, double &y0, double &R) {
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

void PndTrkTrackFinder::FromRealToConformalTrack(double x0, double y0, double R, double &fitm, double &fitp) {
 // CHECK if this needs to be kept --> change xc0 to xc etc
  Double_t xcrot0, ycrot0;
 
  // traslation 
  x0 -= fConformalHitList->GetConformalTransform()->GetTranslation().X();
  y0 -= fConformalHitList->GetConformalTransform()->GetTranslation().Y();

  /**
     |x|  |c -s||xr|  ---> |xr|  | c s||x|
     |y|  |s  c||yr|       |yr|  |-s c||y|
  **/

  // rotation    
  xcrot0 = TMath::Cos(fConformalHitList->GetConformalTransform()->GetRotation())*x0 + TMath::Sin(fConformalHitList->GetConformalTransform()->GetRotation())*y0;
  ycrot0 = - TMath::Sin(fConformalHitList->GetConformalTransform()->GetRotation())*x0 + TMath::Cos(fConformalHitList->GetConformalTransform()->GetRotation())*y0;
  
  // yr = 1/ (2 p) --> p = 1 / (2 yr)
  // xr = - m / (2 p) --> m = - 2 p xr
  fitp = 1 / (2 * ycrot0);
  fitm = - 2 * fitp *  xcrot0;
}

void PndTrkTrackFinder::FillHitMap() {
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


PndTrkClusterList PndTrkTrackFinder::CreateFullClusterization() {
  PndTrkClusterList clusterlist;

  cout << "CLUSTERIZATION <---------------" << endl;

  // get seeds *********************************************8
  TObjArray seeds = fHitMap->GetSeeds();
  TObjArray neighborings;
  int clusterizedhits = 0;
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

    if(fDisplayOn) {
      display->cd(1);
      char goOnChar;
      cin >> goOnChar;
      cout << "SEED " << seedtubeID << endl; 
      seedhit->Draw(kRed);
      display->Update();
      display->Modified();
    }

    //    // add cluster to clusterlist
    //    clusterlist.AddCluster(cluster);

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
	if(neighborings.GetEntriesFast() == 0) continue;
	//	cout << "hit " << addedhit->GetHitID() << "(" << addedhit->GetTubeID() << ")" << " has " << neighborings->GetEntriesFast()  << " neighborigns: " << endl;

	// loop over the neighborings and add them all
	for(int ineigh = 0; ineigh < neighborings.GetEntriesFast(); ineigh++)
	  {
	    PndTrkHit *neighhit = (PndTrkHit*) neighborings.At(ineigh);
	    //  cout << " " << neighhit->GetHitID() << "(" << neighhit->GetTubeID() << ")";
	    if(cluster->DoesContain(neighhit) == kTRUE) {
	      //  cout << "UN-ADDED, in cluster already" << endl;
	      continue;
	    }
	    cluster->AddHit(neighhit);
	    addedcounter++;
	    //  cout << " - ADDED; ";

	    if(fDisplayOn) {
	      display->cd(1);
	      char goOnChar;
	      cin >> goOnChar;
	      neighhit->Draw(kGreen);
		cout << "nof hits " << cluster->GetNofHits() << endl;
	      // cluster->LightUp();
	      display->Update();
	      display->Modified();
	      cin >> goOnChar;
	    }
	    
	  }


      }
      //       cout << endl;
      nlastadded = addedcounter;
    }
    //     cout << "NEXT seed " << endl;
    // add cluster to clusterlist
    if(cluster->GetNofHits() > 3) {
      clusterlist.AddCluster(cluster); // CHECK
      clusterizedhits += cluster->GetNofHits();
    }
    
  }

  // -----------------------------------------
  cout << "NOF TOTAL HITS " << stthitlist->GetNofHits() << " NOF CLUSTERIZED HITS " <<  clusterizedhits << endl;
  if(stthitlist->GetNofHits() - clusterizedhits > 6) {
  // get candseeds *********************************************8
  TObjArray candseeds = fHitMap->GetCandseeds();
  // ----------------- loop over cand seeds

  cout << "we have " << candseeds.GetEntriesFast() << " canduidate seeds" << endl;
  for(int jseed = 0; jseed < candseeds.GetEntriesFast(); jseed++) {
    PndTrkCluster *cluster = new PndTrkCluster();
    PndTrkHit *cseedhit = (PndTrkHit*) candseeds.At(jseed);
    
    // is it already used

    if(fDisplayOn) {
      display->cd(1);
      char goOnChar;
      cin >> goOnChar;
      cout << "CSEED " << endl;
      cseedhit->Draw(kBlue);
      display->Update();
      display->Modified();
    }


    if(cseedhit->IsUsed() == kTRUE) { cout << "already" << endl ; continue; }
    
    int cseedtubeID = cseedhit->GetTubeID();
    PndSttTube *cseedtube = (PndSttTube*) fTubeArray->At(cseedtubeID);
    int cseedlayerID = cseedtube->GetLayerID();
    
    // add hit to cluster
    cluster->AddHit(cseedhit);

    if(fDisplayOn) {
      display->cd(1);
      char goOnChar;
      cin >> goOnChar;
      cout << "SEED " << cseedtubeID << endl; 
      cseedhit->Draw(kRed);
      display->Update();
      display->Modified();
    }

    //    // add cluster to clusterlist
    //    clusterlist.AddCluster(cluster);

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
	if(neighborings.GetEntriesFast() == 0) continue;
	//	cout << "hit " << addedhit->GetHitID() << "(" << addedhit->GetTubeID() << ")" << " has " << neighborings->GetEntriesFast()  << " neighborigns: " << endl;

	// loop over the neighborings and add them all
	for(int ineigh = 0; ineigh < neighborings.GetEntriesFast(); ineigh++)
	  {
	    PndTrkHit *neighhit = (PndTrkHit*) neighborings.At(ineigh);
	    //  cout << " " << neighhit->GetHitID() << "(" << neighhit->GetTubeID() << ")";
	    if(cluster->DoesContain(neighhit) == kTRUE) {
	      //  cout << "UN-ADDED, in cluster already" << endl;
	      continue;
	    }
	    cluster->AddHit(neighhit);
	    addedcounter++;
	    //  cout << " - ADDED; ";

	    if(fDisplayOn) {
	      display->cd(1);
	      char goOnChar;
	      cin >> goOnChar;
	      neighhit->Draw(kGreen);
		cout << "nof hits " << cluster->GetNofHits() << endl;
	      // cluster->LightUp();
	      display->Update();
	      display->Modified();
	      cin >> goOnChar;
	    }
	    
	  }


      }
      //       cout << endl;
      nlastadded = addedcounter;
    }   

    if(cluster->GetNofHits() > 3) {
      clusterlist.AddCluster(cluster); // CHECK
    }

  }
  }
  // -----------------------------------------
  return clusterlist;
}

PndTrkClusterList PndTrkTrackFinder::CreateFullClusterization2() {
  /**
     PndTrkClusterList clusterlist;

     // get seeds *********************************************8
     TObjArray seeds = fHitMap->GetSeeds();
     neighborings = NULL;
  **/
}

Int_t PndTrkTrackFinder::CountPossibleTracks() {
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

  int nofhitsinlay[30]; // CHECK initialize this
  for(int ilay = 0; ilay < 30; ilay++) nofhitsinlay[ilay] = 0;
  PndTrkCluster cluster;
  for(int ihit = 0; ihit < stthitlist->GetNofHits(); ihit++) {
    PndTrkHit *hit = stthitlist->GetHit(ihit);
    PndSttTube *tube = (PndSttTube*) fTubeArray->At(hit->GetTubeID());
    hit->SetSortVariable(tube->GetLayerID());
    nofhitsinlay[tube->GetLayerID()]++;
    cluster.AddHit(hit);
    //    cout << "hit " << ihit << " " << tube->GetLayerID() << " " << nofhitsinlay[tube->GetLayerID()] << endl;
  }
  cluster.Sort();

  for(int ihit = 0; ihit < cluster.GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster.GetHit(ihit);
    PndSttTube *tube = (PndSttTube*) fTubeArray->At(hit->GetTubeID());
    //    cout << "SORTED " << ihit << " " << hit->GetHitID() << " " << tube->GetLayerID() << endl;
  }

  int maxnoftracks = 1;
  int tmplayid = -1;
  int counter = 0, counter1 = 0;;
  int isneigh = 0;
  // loop over cluster hits
  for(int ihit = 0; ihit < cluster.GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster.GetHit(ihit);
    counter++; 

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
      PndTrkHit *hit2 = cluster.GetHit(jhit);
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





Int_t PndTrkTrackFinder::CountTracksInCluster(PndTrkCluster *cluster) {
  return CountTracksInCluster(cluster, 0);
}


Int_t PndTrkTrackFinder::CountTracksInSkewSector(PndTrkCluster *cluster) {
  return CountTracksInCluster(cluster, 1); 
}

Int_t PndTrkTrackFinder::CountTracksInCluster(PndTrkCluster *cluster, Int_t where) {
  // where means:
  // 0 all: parallel & skewed sectors
  // 1: only skewed

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

    // which sector?
    if(where == 1 && hit->IsSttParallel() == kTRUE) continue;
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


Int_t PndTrkTrackFinder::ClusterToConformal(PndTrkCluster *cluster) {
  // ================ --> TO CONFORMAL PLANE
  fConformalHitList = new PndTrkConformalHitList(); // CHECK
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

PndTrkTrack * PndTrkTrackFinder::LegendreFit(PndTrkCluster *cluster) {

  //  cout << "APPLY LEGENDRE =======================" << endl;
  //  cout << "nof hits " << cluster->GetNofHits() << endl;

  // reset the legendre histo for a new legendre fit
  legendre->ResetLegendreHisto();
  
  if(fDisplayOn) {
    RefreshConf();
    DrawGeometryConf(fUmin, fUmax, fVmin, fVmax);
  }
  
  // fill legendre histo with the cluster hits
  FillLegendreHisto(cluster); 
  double theta_max, r_max;
  // get the peak
  int maxpeak = ExtractLegendre(1, theta_max, r_max); // CHECK mode??
          
  if(maxpeak < 4) return NULL; // CHECK
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
  //  cout << "\033[1;33m MAXPEAK " << maxpeak << " XR, YC, R: " << xc << " " << yc << " " << R << "\033[0m" << endl;
	//	cout << "start hit " << ihit << " " << hit->GetHitID() << " " << endsecid << " " << endlayid << endl;

  // create a track from the cluster
  PndTrkTrack *track = new PndTrkTrack(cluster, xc, yc, R);

  return track;
}


PndTrkCluster * PndTrkTrackFinder::CreateClusterAroundTrack(PndTrkTrack *track) {

  double R = track->GetRadius();
  double xc = track->GetCenter().X();
  double yc = track->GetCenter().Y();
  double fitm, fitp;
  FromRealToConformalTrack(xc, yc, R, fitm, fitp);


 PndTrkCluster cluster = track->GetCluster();

  // create cluster depending on fitting
  double rmin = R - R * 0.05; // CHECK 5%?
  double rmax = R + R * 0.05; // "      "
  
  
  if(fDisplayOn) {
    display->cd(1);
    track->Draw(kBlue);
    
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
  for(int ihit = 0; ihit < cluster.GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster.GetHit(ihit);
    PndTrkConformalHit chit;
    if(hit->IsSttParallel()) chit = conform->GetConformalSttHit(hit);
    else chit = conform->GetConformalHit(hit); // CHECK if skew?
    double distanceconf = fabs((chit.GetV() - fitm * chit.GetU() - fitp)/ TMath::Sqrt(fitm * fitm + 1));
 

    double distance = hit->GetXYDistance(TVector3(xc, yc, 0.));
    //    cout << "distance " << distance << " " << rmin << " " << rmax << " " << distanceconf << endl;
    if(distance <= rmax && distance >= rmin) {
      thiscluster->AddHit(hit);
      if(fDisplayOn) {
	display->cd(1);
	//	hit->DrawTube(kGreen);
	display->Update();
	display->Modified();
	//	  char goOnChar;
	//	  cout << "want to go to next hitcluster2?" << endl;
	//	  cin >> goOnChar;
      } 
      
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
      if(cluster.DoesContain(hit)) continue;     
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
	PndTrkConformalHit chit;
	if(hit->IsSttParallel()) chit = conform->GetConformalSttHit(hit);
	else chit = conform->GetConformalHit(hit); // CHECK if skew?
	double distanceconf = fabs((chit.GetV() - fitm * chit.GetU() - fitp)/ TMath::Sqrt(fitm * fitm + 1));
   
	//	cout << "->distance " << distance << " (" << meandistanceconf << ") " << rmin << " " << rmax << " " << distanceconf << endl;
 


	if(fDisplayOn) {
	  display->cd(1);
	  //	  hit->DrawTube(kBlue);
	  display->Update();
	  display->Modified();
	  char goOnChar;
	  cout << "want to go to next?" << endl;
	  // cin >> goOnChar;
	} 

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

void PndTrkTrackFinder::AnalyticalFit(PndTrkCluster *cluster, double xc, double yc, double R, double &fitm, double&fitq) {
  
  
  // fit with analytical chi2 -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~--
  fFitter->Reset();
  if(fDisplayOn) {
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
      // if(chit.GetPosition().Mod() > 2) continue; // CHECK THIS OUT!

      if(TMath::IsNaN(chit.GetPosition().X())) continue; // prevents the nan of the ref hit

      fFitter->SetPointToFit(chit.GetPosition().X(), chit.GetPosition().Y(), sigma);
      //      cout << "set point to fit " << chit.GetHit()->GetDetectorID() << " " << chit.GetPosition().X() << " " <<  chit.GetPosition().Y() << " " << sigma << endl;
      if(fDisplayOn) {
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


void PndTrkTrackFinder::AnalyticalFit2(PndTrkCluster *cluster, double fitm, double fitp, double &fitm2, double&fitp2) {
  
  
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
  
      PndTrkConformalHit chit = conform->GetConformalSttHit(hit);
      IntersectionFinder(&chit, fitm, fitp);
      
      double sigma = chit.GetIsochrone();
      fFitter->SetPointToFit(chit.GetPosition().X(), chit.GetPosition().Y(), sigma);
      //      cout << "set point to fit " << chit.GetPosition().X() << " " <<  chit.GetPosition().Y() << endl;
      if(fDisplayOn) {
	display->cd(1);
	TMarker *mrk = new TMarker(hit->GetPosition().X(), hit->GetPosition().Y(), 6);
	mrk->SetMarkerColor(kRed);
	mrk->Draw("SAME");

	display->cd(2);
	chit.Draw(1);

	display->cd(2);
	TMarker *mrk2 = new TMarker(chit.GetPosition().X(), chit.GetPosition().Y(), 6);
	mrk2->SetMarkerColor(kRed);
	mrk2->Draw("SAME");


	display->Update();
	display->Modified();
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
void PndTrkTrackFinder::IntersectionFinder(PndTrkConformalHit *chit, double fitm, double fitp) {
  
  double xi1 = chit->GetU() + fitm * chit->GetIsochrone()/ TMath::Sqrt(fitm * fitm + 1);
  double yi1 = chit->GetV() - chit->GetIsochrone() / TMath::Sqrt(fitm * fitm + 1);
  
  double xi2 = chit->GetU() - fitm * chit->GetIsochrone() / TMath::Sqrt(fitm * fitm + 1);
  double yi2 = chit->GetV() + chit->GetIsochrone()/ TMath::Sqrt(fitm * fitm + 1);
  
  double xi = 0, yi = 0;
  
  fabs(yi1 - (fitm * xi1 + fitp)) < fabs(yi2 - (fitm * xi2 + fitp)) ? (yi = yi1, xi = xi1) : (yi = yi2, xi = xi2);
  
  chit->SetPosition(xi, yi);

}

void PndTrkTrackFinder::IntersectionFinder(PndTrkHit *hit, double xc, double yc, double R) {

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


// ================================== ZFINDER
PndTrkCluster PndTrkTrackFinder::CreateSkewHitList(PndTrkTrack *track) {

  double xc = track->GetCenter().X();
  double yc = track->GetCenter().Y();
  double R = track->GetRadius();
  PndTrkCluster skewhitlist;

    double phimin = 400, phimax = -1, zmin = 1000, zmax = -1;
    for(int ihit = 0; ihit < stthitlist->GetNofHits(); ihit++) {
      PndTrkHit *hit = stthitlist->GetHit(ihit);	
      if(hit->IsSttSkew() == kFALSE) continue;
      
      int tubeID = hit->GetTubeID();
      PndSttTube *tube = (PndSttTube*) fTubeArray->At(tubeID);
   
      TVector3 wireDirection = tube->GetWireDirection();
      Double_t halflength = tube->GetHalfLength();
      
      TVector3 first  = tube->GetPosition() + wireDirection * halflength; // CHECK
      TVector3 second = tube->GetPosition() - wireDirection * halflength; // CHECK
      //    if(fDisplayOn) {
      // 	char goOnChar;
      //      	display->cd(1);
      // 	TLine *l = new TLine(first.X(), first.Y(), second.X(), second.Y());
      // 	l->SetLineColor(kBlue);
      // 	l->Draw("SAME");
      // 	display->Update();
      // 	display->Modified();
      // 	cin >> goOnChar;   
      //       }
      //    double m1 = (first - second).Y()/(first - second).X();
      //    double q1 = first.Y() - m1 * first.X();
      
      // 1. compute intersection between the track circle and the wire
      TVector2 intersection1, intersection2;
      Int_t nofintersections = tools->ComputeSegmentCircleIntersection(TVector2(first.X(), first.Y()), TVector2(second.X(), second.Y()), xc, yc, R, intersection1, intersection2);
      
      if(nofintersections == 0) continue;
      if(nofintersections >= 2) {
	cout << "ERROR: MORE THAN 1 INTERSECTION!!" << endl;
	continue; // CHECK
      }
    
      if(fDisplayOn) {
	char goOnChar;
     	display->cd(1);
	TLine *l = new TLine(first.X(), first.Y(), second.X(), second.Y());
	l->SetLineColor(kBlue);
	l->Draw("SAME");

	TMarker *mrk = new TMarker(intersection1.X(), intersection1.Y(), 20);
	mrk->SetMarkerColor(kBlue);
	mrk->Draw("SAME");

	display->Update();
	display->Modified();


	//	cin >> goOnChar;   
      }
  

      // 2. find the tangent to the track in the intersection point
      // tangent approximation
      TVector2 tangent = tools->ComputeTangentInPoint(xc, yc, intersection1);
       
      // 3. rotate clockwise the tangent/point/(wire, not explicitely)
      // in order to have the wire parallel to the x axis;
      // then translate everything to have the wire ON the x axis
      double beta = wireDirection.Phi();
      if(beta < 0) beta += TMath::Pi();
      // ... rotate the tangent
      double rtx = TMath::Cos(beta) * tangent.X() + TMath::Sin(beta) * tangent.Y();
      double rty = TMath::Cos(beta) * tangent.Y() - TMath::Sin(beta) * tangent.X();
      TVector2 rottangent(rtx, rty);
      rottangent = rottangent.Unit();
      // ... rotate the point
      double rx = TMath::Cos(beta) * intersection1.X() + TMath::Sin(beta) * intersection1.Y();
      double ry = TMath::Cos(beta) * intersection1.Y() - TMath::Sin(beta) * intersection1.X();
       
      // translation
      Double_t deltay = ry;
      rty -= deltay;
      ry -= deltay;

      // rotm, rotp
      Double_t rotm = rottangent.Y()/rottangent.X();
      Double_t rotp = ry - rotm * rx;

      // ellipsis
      double a = hit->GetIsochrone() * TMath::Cos(SKEW_ANGLE); // CHECK skew angle hard coded
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
   
      // 4. retraslate/rerotate all back to the original plane
      // retranslate
      y0 += deltay; 
      intya  += deltay; 
      intyb  += deltay; 
   
      // rerotate
      double x0anew = TMath::Cos(beta) * x0a - TMath::Sin(beta) * y0;
      double y0anew = TMath::Cos(beta) * y0 + TMath::Sin(beta) * x0a;
      double x0bnew = TMath::Cos(beta) * x0b - TMath::Sin(beta) * y0;
      double y0bnew = TMath::Cos(beta) * y0 + TMath::Sin(beta) * x0b;
   
      double intxanew = TMath::Cos(beta) * intxa - TMath::Sin(beta) * intya;
      double intyanew = TMath::Cos(beta) * intya + TMath::Sin(beta) * intxa;
      double intxbnew = TMath::Cos(beta) * intxb - TMath::Sin(beta) * intyb;
      double intybnew = TMath::Cos(beta) * intyb + TMath::Sin(beta) * intxb;
   
      intxa = intxanew;
      intya = intyanew;
      intxb = intxbnew;
      intyb = intybnew;

      // now we have x0a, y0a, center of the 1st ellipse
      // and x0b, y0b, center of the 2nd ellipse
      x0a = x0anew;
      double y0a = y0anew;
      x0b = x0bnew;
      double y0b = y0bnew;

      if(fDisplayOn) {
	char goOnChar;
	display->cd(1);

	TEllipse *ell1 = new TEllipse(x0a, y0a, a, b, 0, 360, -beta);
	ell1->SetFillStyle(0);
	ell1->SetLineColor(4);
	ell1->Draw("SAME");
	TEllipse *ell2 = new TEllipse(x0b, y0b, a, b, 0, 360, -beta);
	ell2->SetFillStyle(0);
	ell2->SetLineColor(6);
	ell2->Draw("SAME");

	TMarker *mrkinta = new TMarker(intxa, intya, 20);
	mrkinta->SetMarkerColor(4);
	mrkinta->Draw("SAME");
	TMarker *mrkintb = new TMarker(intxb, intyb, 20);
	mrkintb->SetMarkerColor(6);
	mrkintb->Draw("SAME");
	//	 cin >> goOnChar;   
      }

      // 5. calculate z coordinate for each intersection
       
      // calculate z0a, z0b of the center of the ellipse
      Double_t t = ((x0a + y0a) - (first.X() + first.Y())) /  ((second.X() - first.X()) + (second.Y() - first.Y()));
      Double_t z0a = first.Z() + (second.Z() - first.Z()) * t;
      //    cout << "0 : calculate t, z0a " << t << " " << z0a << endl;
 
      t = ((x0b + y0b) - (first.X() + first.Y())) /  ((second.X() - first.X()) + (second.Y() - first.Y()));
      Double_t z0b = first.Z() + (second.Z() - first.Z()) * t;
 
      TVector3 center1(x0a, y0a, z0a);
      TVector3 center2(x0b, y0b, z0b);
    //   if(fDisplayOn) {
// 	char goOnChar;
// 	display->cd(3);
// 	//	cout << "COMPUTE Z COORDINATE" << endl;
// 	DrawZGeometry();
// 	TLine *linezx = new TLine(first.X(), first.Z(), second.X(), second.Z());
// 	linezx->Draw("SAME");
// 	TMarker *mrkza = new TMarker(x0a, z0a, 20);
// 	mrkza->SetMarkerColor(4);
// 	mrkza->Draw("SAME");
// 	TMarker *mrkzb = new TMarker(x0b, z0b, 20);
// 	mrkzb->SetMarkerColor(6);
// 	mrkzb->Draw("SAME");
// 	//	 cin >> goOnChar;   
//       }

      // calculate the z of the intersection ON the ellipse (CHECK this step calculations!)
      double dx = intxa - x0a;
      double dy = intya - y0a;
      TVector3 dxdy(dx, dy, 0.0);

      TVector3 tfirst = first + dxdy;
      TVector3 tsecond = second + dxdy;

      t = ((intxa + intya) - (tfirst.X() + tfirst.Y())) /  ((tsecond.X() - tfirst.X()) + (tsecond.Y() - tfirst.Y()));
      double intza = tfirst.Z() + (tsecond.Z() - tfirst.Z()) * t;
      if(fDisplayOn) {
	char goOnChar;
	display->cd(3);
	TLine *linezx1 = new TLine(tfirst.X(), tfirst.Z(), tsecond.X(), tsecond.Z());
	linezx1->SetLineStyle(1);
	linezx1->Draw("SAME");
	TMarker *mrkza1 = new TMarker(intxa, intza, 20);
	mrkza1->SetMarkerColor(kBlue - 9);
	mrkza1->Draw("SAME");
	// cin >> goOnChar;   
      }

      tfirst = first - dxdy;
      tsecond = second - dxdy;

      t = ((intxb + intyb) - (tfirst.X() + tfirst.Y())) /  ((tsecond.X() - tfirst.X()) + (tsecond.Y() - tfirst.Y()));
      double intzb = tfirst.Z() + (tsecond.Z() - tfirst.Z()) * t;
 
      TVector3 fin_intersection1(intxa, intya, intza);
      TVector3 fin_intersection2(intxb, intyb, intzb);

    //   if(fDisplayOn) {
// 	char goOnChar;
// 	display->cd(3);
// 	TLine *linezx2 = new TLine(tfirst.X(), tfirst.Z(), tsecond.X(), tsecond.Z());
// 	linezx2->SetLineStyle(1);
// 	linezx2->Draw("SAME");
// 	TMarker *mrkzb1 = new TMarker(intxb, intzb, 20);
// 	mrkzb1->SetMarkerColor(kMagenta - 7);
// 	mrkzb1->Draw("SAME");
// 	//	 cin >> goOnChar;   
//       }
      //        int1.SetXYZ(intxa, intya, intza);
      //        int2.SetXYZ(intxb, intyb, intzb);
      //        errz = fabs(intza - intzb)/2.   ;
   
      // CHECK to be changed
      int trackID = 1;
      double phi1 = track->ComputePhi(fin_intersection1);
      double phi2 = track->ComputePhi(fin_intersection2);

      PndTrkSkewHit *skewhit = new PndTrkSkewHit(*hit, trackID, center1, fin_intersection1, phi1, center2, fin_intersection2, phi2, a, b, -1, beta);
      //      skewhit->Print();
      skewhitlist.AddHit(skewhit);

   
    }
    //     cout << "PHI: " << phimin << " " << phimax << endl;
    //     cout << "Z  : " << zmin << " " << zmax << endl;


    return skewhitlist;
}
 
PndTrkCluster PndTrkTrackFinder::CleanUpSkewHitList(PndTrkCluster *skewhitlist) {


  // 6. FIRST CLEANING of the track skewed associations
  // find most probable z - CHECK what happens if a track is very fwd peaked?
  //                        TRY WITH DELTA z = COST
  //    cout  << "##################### FIND MOST PROBABLE Z" << endl; 
  double phimin = 400, phimax = -1, zmin = 1000, zmax = -1;

  for(int ihit = 0; ihit < skewhitlist->GetNofHits(); ihit++) {
    PndTrkSkewHit *skewhit = (PndTrkSkewHit*) skewhitlist->GetHit(ihit);
    if(!skewhit) continue;
    TVector3 fin_intersection1 = skewhit->GetIntersection1();
    TVector3 fin_intersection2 = skewhit->GetIntersection2();

    double phi1 = skewhit->GetPhi1();
    double phi2 = skewhit->GetPhi2();

    if(phi1 < phimin) phimin = phi1;
    if(phi2 < phimin) phimin = phi2;
    if(fin_intersection1.Z() < zmin) zmin = fin_intersection1.Z();
    if(fin_intersection2.Z() < zmin) zmin = fin_intersection2.Z();
      
    if(phi1 > phimax) phimax = phi1;
    if(phi2 > phimax) phimax = phi2;
    if(fin_intersection1.Z() > zmax) zmax = fin_intersection1.Z();
    if(fin_intersection2.Z() > zmax) zmax = fin_intersection2.Z();
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~ CHECK
  }
  //     cout << "PHI: " << phimin << " " << phimax << endl;
  //     cout << "Z  : " << zmin << " " << zmax << endl;

  // DISPLAY ----------------------------
  if(fDisplayOn) {
    char goOnChar;
    display->cd(4);
    phimin -= 30;
    phimax += 30;
    zmin -= 13;
    zmax += 13;
    DrawZGeometry(phimin, phimax, zmin, zmax);
    hzphi->SetXTitle("#phi");
    hzphi->SetYTitle("#z");
    hzphi->Draw();
    display->Update();
    display->Modified();  
   
     
    for(int ihit = 0; ihit < skewhitlist->GetNofHits(); ihit++) {
      PndTrkSkewHit *skewhit = (PndTrkSkewHit*) skewhitlist->GetHit(ihit);
      if(!skewhit) continue;

      TVector3 fin_intersection1 = skewhit->GetIntersection1();
      TVector3 fin_intersection2 = skewhit->GetIntersection2();

      double phi1 = skewhit->GetPhi1();
      double phi2 = skewhit->GetPhi2();

      TLine *linezphi = new TLine(phi1, fin_intersection1.Z(), phi2, fin_intersection2.Z());
      // TLine *linezphi = new TLine(fin_intersection1.Z(), phi1, fin_intersection2.Z(), phi2);
      linezphi->SetLineStyle(1);
      linezphi->Draw("SAME");

      TMarker *mrkzphi1 = new TMarker(phi1, fin_intersection1.Z(), 20);
      // TMarker *mrkzphi1 = new TMarker(fin_intersection1.Z(), phi1, 20);

      mrkzphi1->SetMarkerColor(kBlue - 9);
      mrkzphi1->Draw("SAME");
	
      TMarker *mrkzphi2 = new TMarker(phi2, fin_intersection2.Z(), 20);
      //	 TMarker *mrkzphi2 = new TMarker(fin_intersection2.Z(), phi2, 20);
      mrkzphi2->SetMarkerColor(kMagenta - 7);
      mrkzphi2->Draw("SAME");
    }
    display->Update();
    display->Modified();  
    //      cin >> goOnChar;   
  }
  // DISPLAY ----------------------------

  TH1F hz("hz", "z",  (zmax - zmin)/10., zmin, zmax); // CHECK
  for(int ihit = 0; ihit < skewhitlist->GetNofHits(); ihit++) {
    PndTrkSkewHit *skewhit = (PndTrkSkewHit*) skewhitlist->GetHit(ihit);
    if(!skewhit) continue;
    TVector3 fin_intersection1 = skewhit->GetIntersection1();
    TVector3 fin_intersection2 = skewhit->GetIntersection2();
    hz.Fill(fin_intersection1.Z());
    hz.Fill(fin_intersection2.Z());
  }
  int maxbinz = hz.GetMaximumBin();
  double mostprobZ = hz.GetBinCenter(maxbinz);
  //    cout << mostprobZ << endl;
    
  // delete hits too far away ..........   
  //    cout  << "##################### DELETE HITS" << endl; 
  PndTrkCluster tmpskewhitlist;
  for(int ihit = 0; ihit < skewhitlist->GetNofHits(); ihit++) {
    PndTrkSkewHit *skewhit = (PndTrkSkewHit*) skewhitlist->GetHit(ihit);
    if(!skewhit) continue;
    TVector3 fin_intersection1 = skewhit->GetIntersection1();
    TVector3 fin_intersection2 = skewhit->GetIntersection2();
    double phi1 = skewhit->GetPhi1();
    double phi2 = skewhit->GetPhi2();
      
    if(fabs(fin_intersection1.Z() - mostprobZ) > 30. && fabs(fin_intersection2.Z() - mostprobZ) > 30.) {
      // 	cout << "THROW AWAY " << ihit << " " << fabs(fin_intersection1.Z() - mostprobZ) << "  " << fabs(fin_intersection2.Z() - mostprobZ) << endl;
      continue;
    }
    skewhit->SetSortVariable((phi1 + phi2)/2.);
    tmpskewhitlist.AddHit(skewhit);
  }
  tmpskewhitlist.Sort();
  return tmpskewhitlist;
}


void PndTrkTrackFinder::DrawZGeometry(double phimin, double phimax, double zmin, double zmax)
{
  if(hzphi == NULL) hzphi = new TH2F("hzphi", "z - phi plane", phimax - phimin, phimin, phimax, zmax - zmin, zmin, zmax);
  else {
    hzphi->Reset();
    hzphi->GetXaxis()->SetLimits(phimin, phimax);
    hzphi->GetYaxis()->SetLimits(zmin, zmax);
  }
  display->cd(4);
  hzphi->Draw();
  
  display->Update();
  display->Modified();  
  
}



ClassImp(PndTrkTrackFinder)
