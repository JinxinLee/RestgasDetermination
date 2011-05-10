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

#include <iostream>
#include <cmath>
#include <vector>
#include <iterator>



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
    
    h2 = new TH2F("h2", "XY plane", 100, -43, 43, 100, -43, 43);
    display->cd();
    h2->Draw();
    display->Update();
    display->Modified();  
  }

  Int_t nstthits = fSttHitArray->GetEntriesFast();
  cout << "EVENTO with " << nstthits << endl;

  std::vector<int> stthits;
  // stthits.clear();
  stthits = OrderHits(fSttHitArray);

  for(int ihit = 0; ihit < nstthits; ihit++) {
    int hitid = stthits[ihit];
    PndSttHit *stthit = (PndSttHit *) fSttHitArray->At(hitid);
    if(!stthit) continue;
    TVector3 position;
    stthit->Position(position);
    cout << "distance " << hitid << " " << position.Perp() << endl;

    Int_t tubeID = stthit->GetTubeID();
    PndSttTube *tube = (PndSttTube* ) fTubeArray->At(tubeID);
    
    TVector3 wireDirection = tube->GetWireDirection();
    if(wireDirection != TVector3(0., 0., 1.)) continue;
     
    if(fDisplayOn) {
      TArc *arc = new TArc(position.X(), position.Y(), stthit->GetIsochrone());
      arc->SetLineColor(kGray);
      arc->SetFillStyle(0);
      arc->Draw("SAME");
      display->Update();
      display->Modified();  
    }
  }

  if(fDisplayOn) {
    DrawFoundTracks();
    DrawMCTracks();
  }

  DeleteHits("STT", &stthits);

  nstthits = stthits.size();
  for(int ihit = 0; ihit < nstthits; ihit++) {
    int hitid = stthits[ihit];
    PndSttHit *stthit = (PndSttHit *) fSttHitArray->At(hitid);
    if(!stthit) continue;
    TVector3 position;
    stthit->Position(position);
    //    cout << "distance " << hitid << " " << position.Perp() << endl;
    Int_t tubeID = stthit->GetTubeID();
    PndSttTube *tube = (PndSttTube* ) fTubeArray->At(tubeID);
    TVector3 wireDirection = tube->GetWireDirection();
    if(wireDirection != TVector3(0., 0., 1.)) continue;
     
    if(fDisplayOn) {
      TArc *arc = new TArc(position.X(), position.Y(), stthit->GetIsochrone());
      arc->SetLineColor(kBlack);
      arc->SetFillStyle(0);
      arc->Draw("SAME");
      display->Update();
      display->Modified();  
    }
  }

}


//   switch()    {
//     case FairRootManager::Instance()->GetBranchId(fMvdPixelBranch):
//       hit = (FairHit*) fMvdPixelHitArray->At(ihit);
//       break;
//     case FairRootManager::Instance()->GetBranchId(fMvdStripBranch):
//       hit = (FairHit*) fMvdStripHitArray->At(ihit);
//       break;
//     case FairRootManager::Instance()->GetBranchId(fSttBranch):
//       hit = (FairHit*) fSttHitArray->At(ihit);
//       break;
//     case FairRootManager::Instance()->GetBranchId(fGemBranch):
//       hit = (FairHit*) fGemHitArray->At(ihit);
//   }


std::vector<int> PndSecondaryTrackFinder::OrderHits(TClonesArray *hitarray)
{
  std::vector<int> sorthits;
  std::vector<double> distances;
  std::multimap<double, int> mapdistances;

  for(int ihit = 0; ihit < hitarray->GetEntriesFast(); ihit++) {
    FairHit* hit = (FairHit*) hitarray->At(ihit);
    if(!hit) continue;
    
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
  Int_t charge = (Int_t) TDatabasePDG::Instance()->GetParticle(mctrack->GetPdgCode())->Charge()/3.;
  
  radius = mcmom.Perp()/0.006;
  Double_t beta;
  
  if(fabs(mcmom.X()) >  1e-10) {
    // track from tangent ---------------------
    double mc_m1 = mcmom.Y() / mcmom.X();
    double mc_q1 = mcpos.Y() - mcpos.X() * mc_m1;
    double mc_m2 = -1./mc_m1;
    double mc_q2 = mcpos.Y() - mcpos.X() * mc_m2;
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


ClassImp(PndSecondaryTrackFinder)
