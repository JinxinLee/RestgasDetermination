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
  sprintf(fSttBranch,"STTHit");
  sprintf(fMvdPixelBranch,"MVDHitsPixel");
  sprintf(fMvdStripBranch,"MVDHitsStrip");
}
// -------------------------------------------------------------------------

PndSecondaryTrackFinder::PndSecondaryTrackFinder(Int_t verbose) : FairTask("STT Stt-Mvd Tracking") { 
  fPersistence = kTRUE;
  fVerbose = verbose;
  fDisplayOn = kFALSE;
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
  
  
  fLimit = 2.;

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

  std::vector<int> stthits;
  // stthits.clear();
  stthits = OrderHits(fSttHitArray, FairRootManager::Instance()->GetBranchId(fSttBranch));


  if(fDisplayOn) {
    DrawHits(stthits, FairRootManager::Instance()->GetBranchId(fSttBranch));
    DrawFoundTracks();
    DrawMCTracks();
  }

  DeleteHits("STT", &stthits);
  if(fDisplayOn) {
    DrawUsableHits(stthits, FairRootManager::Instance()->GetBranchId(fSttBranch));
  }



  std::vector<std::vector<int> > clusterlist;
  clusterlist = ClusterFinder(stthits,  FairRootManager::Instance()->GetBranchId(fSttBranch));
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


std::vector<int> PndSecondaryTrackFinder::OrderHits(TClonesArray *hitarray, Int_t detId)
{
  std::vector<int> sorthits;
  std::vector<double> distances;
  std::multimap<double, int> mapdistances;

  for(int ihit = 0; ihit < hitarray->GetEntriesFast(); ihit++) {
    FairHit* hit = (FairHit*) hitarray->At(ihit);
    if(!hit) continue;
    if(detId == FairRootManager::Instance()->GetBranchId(fSttBranch)) {
      Int_t tubeID = ((PndSttHit*) hit)->GetTubeID();
      PndSttTube *tube = (PndSttTube* ) fTubeArray->At(tubeID);
      TVector3 wireDirection = tube->GetWireDirection();
      if(wireDirection != TVector3(0., 0., 1.)) continue;
    }
    
    TVector3 distance3;
    hit->Position(distance3);
    double distance = distance3.Perp();

    distances.push_back(distance);
    mapdistances.insert(std::pair<double, int>(distance, ihit));
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
  for(Int_t itrk = 0; itrk < fSttMvdGemTrackCandArray->GetEntriesFast(); itrk++) {
    PndTrackCand *trkCand = (PndTrackCand*) fSttMvdGemTrackCandArray->At(itrk);
    if(!trkCand) continue;
    Int_t nhits = trkCand->GetNHits(); 
    for(int ihit = 0; ihit < nhits; ihit++)
      {
	PndTrackCandHit candhit = trkCand->GetSortedHit(ihit);
	Int_t hitId = candhit.GetHitId();
	Int_t detId = candhit.GetDetId();

	if(detectors.Contains("STT") 
	   && 
	   detId == FairRootManager::Instance()->GetBranchId(fSttBranch)) {
	  cout << "deleting " << hitId << endl;
	  DeleteHit(hitId, hits);
	}
	else if(detectors.Contains("PIXEL") 
		&& 
		detId == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) DeleteHit(hitId, hits);
	else if(detectors.Contains("STRIP") 
		&& 
		detId == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) DeleteHit(hitId, hits);
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

    Double_t xc, yc, radius, fitm, fitp;
    GetInitialParams(trk, xc, yc, radius, fitm, fitp);
    TArc *arc = new TArc(xc, yc, radius);
    arc->SetLineColor(kRed);
    arc->SetFillStyle(0);
    arc->Draw("SAME");
    display->Update();
    display->Modified();  
  }
}

void PndSecondaryTrackFinder::DrawMCTracks() {
  for(Int_t itrk = 0; itrk < fMCTrackArray->GetEntriesFast(); itrk++) {
    PndMCTrack *mctrk = (PndMCTrack*) fMCTrackArray->At(itrk);
    if(!mctrk) continue;

    Double_t xc, yc, radius, fitm, fitp;
    GetInitialParamsMC(mctrk, xc, yc, radius, fitm, fitp);
    TArc *arc = new TArc(xc, yc, radius);
    arc->SetLineColor(kBlue);
    arc->SetFillStyle(0);
    arc->Draw("SAME");
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
  if(mctrack->GetMotherID() != -1) return;
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
      cout << ihit << " " <<  hitid  << "/" << hits[tmphit] << " distance " << distance << " " ;

      // to have 1 cm for radius = 15 cm, 3 cm for radius = 42 cm
      //      double fLimit = 2.7; // 0.075 * position.Perp() - 0125.; CHECK
      if(distance < fLimit && position.Perp() != tmpposition.Perp()) {
	cout << " YES" << endl;
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
      else cout << "NO" << endl;
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
    fDisplayOn = kFALSE; // CHECK
    FindBoundary(iclus, cluster, FairRootManager::Instance()->GetBranchId(fSttBranch), boundaries);
    fDisplayOn = kTRUE; // CHECK
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

  //  if(fDisplayOn) Refresh(hits, FairRootManager::Instance()->GetBranchId(fSttBranch));


  for(int iclus = 0; iclus < nclus; iclus++) {
    std::vector<int> cluster = list[iclus];
    if(fDisplayOn) DrawLinks(cluster, FairRootManager::Instance()->GetBranchId(fSttBranch), iclus);
  }

  //  if(fDisplayOn) Refresh(hits, FairRootManager::Instance()->GetBranchId(fSttBranch)); // CHECK

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
	FindBoundary(0, cluster1b, FairRootManager::Instance()->GetBranchId(fSttBranch), bounds);

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
    FindBoundary(iclus, cluster, FairRootManager::Instance()->GetBranchId(fSttBranch), boundaries);
  }

  // ******************
  // conformal map ****

//   for(int iclus = 0; iclus < nclus; iclus++) {
//     std::vector<int> cluster = list[iclus];
//    FindBoundary(iclus, cluster, FairRootManager::Instance()->GetBranchId(fSttBranch), boundaries);
//    bool conf = ConformalPlane(cluster, boundaries, hits, FairRootManager::Instance()->GetBranchId(fSttBranch), iclus);
//     cout << "conform " << conf << endl;
//   }

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

  cout << "sorted" << endl;
  for(int ihit = 0; ihit < sorthits.size(); ihit++) {
    cout << sorthits[ihit] << " ";
    int hitid = sorthits[ihit];
    
    FairHit* hit = (FairHit*) array->At(hitid);
    if(!hit) continue;
    
    TVector3 distance3;
    hit->Position(distance3);
    double distance = distance3.Perp();
    cout << distance << endl;     
 }
  cout << endl;

  return sorthits;
}



void PndSecondaryTrackFinder::FindBoundary(Int_t iclus, std::vector<int> cluster, Int_t detId, TMatrixT<double> &boundaries) {

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
    if(fDisplayOn) {

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

    h2 = new TH2F("h2", "XY plane", 100, -43, 43, 100, -43, 43);
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


void PndSecondaryTrackFinder::DrawHitsColor(std::vector<int> stthits, Int_t detId, Int_t color) {

  if(detId == FairRootManager::Instance()->GetBranchId(fSttBranch)) {
    int nstthits = stthits.size();
    for(int ihit = 0; ihit < nstthits; ihit++) {
      int hitid = stthits[ihit];
      PndSttHit *stthit = (PndSttHit *) fSttHitArray->At(hitid);
      if(!stthit) continue;
      TVector3 position;
      stthit->Position(position);
      cout << "distance " << hitid << " " << position.Perp() << endl;
      position.Print();
      
      Int_t tubeID = stthit->GetTubeID();
      PndSttTube *tube = (PndSttTube* ) fTubeArray->At(tubeID);
      
      TVector3 wireDirection = tube->GetWireDirection();
      if(wireDirection != TVector3(0., 0., 1.)) continue;
      
      if(fDisplayOn) {
	TArc *arc = new TArc(position.X(), position.Y(), tube->GetRadIn()); // stthit->GetIsochrone());
	arc->SetLineColor(color);
	arc->SetFillStyle(0);
	arc->Draw("SAME");
	display->Update();
	display->Modified();  
      }
    }
  }
}

void PndSecondaryTrackFinder::DrawHits(std::vector<int> hits, Int_t detId) {
  DrawHitsColor(hits, detId, kGray);
}

void PndSecondaryTrackFinder::DrawUsableHits(std::vector<int> hits, Int_t detId) {
  DrawHitsColor(hits, detId, kBlack);
}


void PndSecondaryTrackFinder::Refresh(std::vector<int> hits, Int_t detId) {
  char goOnChar;
  cout << "Refresh?" << endl;
  cin >> goOnChar;
  cout << "GOING ON" << endl;
  DrawGeometry();
  DrawUsableHits(hits, detId);
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
	cout << ihit << " " <<  hitid  << "/" << hits[tmphit] << " distance " << distance << " " ;
	
	// to have 1 cm for radius = 15 cm, 3 cm for radius = 42 cm
	//      double fLimit = 2.7; // 0.075 * position.Perp() - 0125.; CHECK
	if(distance < fLimit) { // && position.Perp() != position.Perp()) {
	  cout << " YES" << endl;
	  mapdistances.insert(std::pair<int, int>(hitid, hitid2));
	}
	else cout << "NO" << endl;
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
    DrawGeometryConformal(-1, -1, 10, 1); 
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





ClassImp(PndSecondaryTrackFinder)
