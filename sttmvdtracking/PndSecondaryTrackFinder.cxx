////////////////////////////////////////////////////////////
//
// PndSecondaryTrackFinder
// 
// Class for secondary track pattern recognition
//
// authors: Lia Lavezzi - INFN Pavia (2011)
//                                   (restyled nov 2011)
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
#include "TMatrixT.h"
#include "TArrow.h"

#include <iostream>
#include <cmath>
#include <vector>
#include <iterator>
#include <algorithm>

#define skew 3. * TMath::DegToRad()
#define STRAWRADIUS 0.5
#define OUTRADIUS 42.
#define INRADIUS  15.
#define SKEWLIMIT 30.   // CHECK



using namespace std;


// -----   Default constructor   -------------------------------------------
PndSecondaryTrackFinder::PndSecondaryTrackFinder() : FairTask("HOUGH Tracking") { 
  fPersistence = kTRUE;
  fVerbose = 0;
  fDisplayOn = kFALSE;
  fChi2Limit = 0.5;
  fCountElemLimit = 100;
  fLimit = 10.;
  fUsePrimary = kFALSE;

  fRLimit = 10;
  fXLimit = 20;
  fYLimit = 20;

  fInSkewOut = kTRUE;
  fInSkew = kFALSE;
  fSkewOut = kFALSE;

  
  sprintf(fSttBranch,"STTHit");
  sprintf(fMvdPixelBranch,"MVDHitsPixel");
  sprintf(fMvdStripBranch,"MVDHitsStrip");
}
// -------------------------------------------------------------------------

PndSecondaryTrackFinder::PndSecondaryTrackFinder(Int_t verbose) : FairTask("HOUGH Tracking") { 
  fPersistence = kTRUE;
  fVerbose = verbose;
  fDisplayOn = kFALSE;
  fChi2Limit = 0.2;
  fCountElemLimit = 100;
  fLimit = 10.;
  fUsePrimary = kFALSE;

  fRLimit = 10;
  fXLimit = 20;
  fYLimit = 20;
  
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



//   // Get SttTrackCand array  dal pattern recognition di STT
//   fSttTrackCandArray  = (TClonesArray*) ioman->GetObject("STTTrackCand"); 
//   if ( ! fSttTrackCandArray) 
//     {
//       cout << "-E- PndSecondaryTrackFinder::Init: No SttTrack Cand  array, return!"
// 	   << endl;
//       return kERROR;
//     }
	
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
  fSecondaryTrackCandArray = new TClonesArray("PndTrackCand");
  ioman->Register("SttMvdSecTrackCand", "SttMvd", fSecondaryTrackCandArray, kTRUE);
  
  // Create and register output array for PndTrack of Stt+Mvd combined
  fSecondaryTrackArray = new TClonesArray("PndTrack");
  ioman->Register("SttMvdSecTrack", "SttMvd", fSecondaryTrackArray, kTRUE);



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

//   fMvdTrackCandArray = (TClonesArray*) ioman->GetObject("MVDRiemannTrackCand");
//   if ( !fMvdTrackCandArray){
//     std::cout << "-W- PndSecondaryTrackFinder::Init: " << "No MVD TrackCand Array, return!" << std::endl;
//     return kERROR;
//   }

  cout << "-I- PndSecondaryTrackFinder: Initialization successfull" << endl;
  
  //  -------------------------   get the Mvd MC points

  fMvdMCPointArray = (TClonesArray*) ioman->GetObject("MVDPoint");
  if ( !fMvdMCPointArray){
    std::cout << "-W- PndSecondaryTrackFinder::Init: " << "No MVD MC Point Array, return!" << std::endl;
    return kERROR;
  }

  cout << "-I- PndSecondaryTrackFinder: Initialization successfull" << endl;
  

//   // SttMvdGemTrackCand
//   fSttMvdGemTrackCandArray  = (TClonesArray*) ioman->GetObject("SttMvdGemTrackCand"); 
//   if ( ! fSttMvdGemTrackCandArray && fUsePrimary) 
//     {
//       cout << "-E- PndSecondaryTrackFinder::Init: No SttMvdGemTrackCand array, return!"
// 	   << endl;
//       return kERROR;
//     }
  
//   // SttMvdGemTrack
//   fSttMvdGemTrackArray  = (TClonesArray*) ioman->GetObject("SttMvdGemTrack"); 
//   if ( ! fSttMvdGemTrackArray  && fUsePrimary) 
//     {
//       cout << "-E- PndSecondaryTrackFinder::Init: No SttMvdGemTrack array, return!"
// 	   << endl;
//       return kERROR;
//     }
 
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

  fHa = new TH1F("fHa", "x center", 100, -800, 800);
  fHb = new TH1F("fHb", "y center", 100, -800, 800);
  fHr = new TH1F("fHr", "r center", 100,  0, 800);
 
  fgoodtriplet = new std::vector< std::vector<double> >;

  return kSUCCESS;

}

// -------------------------------------------------------------------------

void PndSecondaryTrackFinder::SetParContainers() {
  FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}

// -------------------------------------------------------------------------

void PndSecondaryTrackFinder::FillHitFullList() {

  fNofMvdPixHits = 0; // fMvdPixelHitArray->GetEntriesFast(); // CHECK
  fNofMvdStrHits = 0; // fMvdStripHitArray->GetEntriesFast(); // CHECK
  fNofSttHits = fSttHitArray->GetEntriesFast();

  int hitcounter = 0;
  // MVD PIXEL
  for(int ihit = 0; ihit < fNofMvdPixHits; ihit++)
    {
      PndSdsHit *hit = (PndSdsHit*) fMvdPixelHitArray->At(ihit);
      if(!hit) continue;
      fFullList[hitcounter][0] = ihit;
      fFullList[hitcounter][1] = -1;
      if(hit->GetX() < 0.) fFullList[hitcounter][2] = 1;   // wheels?? CHECK
      else fFullList[hitcounter][2] = 0;
      fFullList[hitcounter][3] = 1;
      hitcounter++;
    }

  // MVD /STRIP
  for(int ihit = 0; ihit < fNofMvdStrHits; ihit++)
    {
      PndSdsHit *hit = (PndSdsHit*) fMvdStripHitArray->At(ihit);
      if(!hit) continue;
      fFullList[hitcounter][0] = ihit;
      fFullList[hitcounter][1] = -1;
      if(hit->GetX() < 0.) fFullList[hitcounter][2] = 1;   // wheels?? CHECK
      else fFullList[hitcounter][2] = 0;
      fFullList[hitcounter][3] = 1;
       hitcounter++;
  }

  //  STT
  for(int ihit = 0; ihit < fNofSttHits; ihit++)
    {
//       cout << "size " << fSttHitArray->GetSize() << endl;
//       cout << "############ " << ihit << " " << fNofSttHits << " " << fSttHitArray->GetEntriesFast() << endl;
      PndSttHit *hit = (PndSttHit*) fSttHitArray->At(ihit);
      if(!hit) continue;
//       cout << hit << endl;
      fFullList[hitcounter][0] = ihit;

      int tubeID = hit->GetTubeID();
      fFullList[hitcounter][1] = tubeID;  

      PndSttTube *tube = (PndSttTube* ) fTubeArray->At(tubeID);
      // parallel tubes
      if(tube->GetWireDirection() == TVector3(0., 0., 1.)) {
	// x < 0
	if(tube->GetPosition().X() < 0) {
	  if(tube->GetPosition().Perp() < SKEWLIMIT) fFullList[hitcounter][2] = 3;
	  else fFullList[hitcounter][2] = 7;
	}
	// x > 0
	else {
	  if(tube->GetPosition().Perp() < SKEWLIMIT) fFullList[hitcounter][2] = 2;
	  else fFullList[hitcounter][2] = 6;
	}
      }
      // skewed tubes
      else {
	// x < 0
	if(tube->GetPosition().X() < 0) fFullList[hitcounter][2] = 5; 
	// x > 0
	else fFullList[hitcounter][2] = 4;
      }
      fFullList[hitcounter][3] = 1;

      //      cout << "STT HIT " << fFullList[hitcounter][0] << " " << fFullList[hitcounter][2] << endl;

      hitcounter++;
    }

  fNofHits = fNofSttHits + fNofMvdPixHits + fNofMvdStrHits;
}
      

Int_t PndSecondaryTrackFinder::FromFullListToSttList(int id) {
  return id - fNofMvdPixHits- fNofMvdStrHits;
}

void PndSecondaryTrackFinder::Exec(Option_t* opt) {
  //   fDisplayOn = kFALSE;
 fTrackList.clear();
  fParList.clear();
  fSecondaryTrackCandArray->Delete();
  fSecondaryTrackArray->Delete();


  cout << "+++++++++++++++++++ " << fEventCounter << " +++++++++++++++++" << endl;
  fEventCounter++;


  if(fDisplayOn) {
    char goOnChar;
    cout << "press any key" << endl;
    cin >> goOnChar;
    cout << "GOING ON" << endl;
   
    DrawGeometry();
    DrawHits();
  }

  //   cout << "START SEQ HISTO" << endl;
  FillHitFullList();
  //  cout << "++++++++" << endl;
  //   SequentialHistogramming();
  std::vector< std::vector<int> > cluster2 = SttClusterFinder(2);
  //  cout << "CLUSTER FOUND in region 2 has " << cluster2.size() << endl;
  std::vector< std::vector<int> > cluster4 = SttClusterFinder(6);
  //  cout << "CLUSTER FOUND in region 4 has " << cluster4.size() << endl;
  std::vector< std::vector<int> > cluster3 = SttClusterFinder(3);
  //  cout << "CLUSTER FOUND in region 3 has " << cluster3.size() << endl;
  std::vector< std::vector<int> > cluster5 = SttClusterFinder(7);
  //  cout << "CLUSTER FOUND in region 5 has " << cluster5.size() << endl;

  if(fInSkewOut) {
    RunListOfDoubleClusters(&cluster2, &cluster4, 2, 6);
    RunListOfDoubleClusters(&cluster3, &cluster5, 3, 7);
  }

 //  //# @#@#@#@#@#@#@#@#@# @
  //  fDisplayOn = kTRUE;
  if(fDisplayOn) 
    {
      
      for(int iclus = 0; iclus < fTrackList.size(); iclus++) {
	
	std::vector< std::pair<int, int> > thiscluster = fTrackList[iclus];
	std::vector<double> thisparams = fParList[iclus];
 
	char goOnChar;
	cout << "GO ON AND SEE THE RESULT" << endl;
	cin >> goOnChar;
	DrawGeometry();
	DrawHits();
	cout << "CLUSTER " << iclus << "has " << thiscluster.size() << " hits: ";
	for(int ihit = 0; ihit < thiscluster.size(); ihit++) {
	  std::pair<int, int> thispair = thiscluster[ihit];
	  int hitidinfulllist = thispair.second + (fNofMvdPixHits + fNofMvdStrHits);
	  TVector3 position;
	  PndSttHit *hit = (PndSttHit*) fSttHitArray->At(thispair.second);
	  if(fFullList[hitidinfulllist][2] == 4 || fFullList[hitidinfulllist][2] == 5) {
	    std::map< int, TVector3 >::iterator it = fskewintmap.find(thispair.second);
	    position = (*it).second;
	  }
	  else {
	    position.SetXYZ(hit->GetX(), hit->GetY(), hit->GetIsochrone());
	  }
	  cout << " " << thispair.second;
	  TArc *arc = new TArc(position.X(), position.Y(), hit->GetIsochrone());
	  arc->SetFillColor(4);
	  arc->Draw("SAME");
	  display->Update();
	  display->Modified();  
	    
	}
	cout << endl;
	TArc *barc = new TArc(thisparams[0], thisparams[1], thisparams[2]);
	barc->SetLineColor(fColors[iclus]);
	barc->SetFillStyle(0);
	barc->Draw("SAME");
	display->Update();
	display->Modified();  
      }

    }
  
 
  //# @#@#@#@#@#@#@#@#@# @
  if(fSkewOut) {
    RunListOfSingleClusters(&cluster4, 6);
    RunListOfSingleClusters(&cluster5, 7);
  }
  
  if(fInSkew) {
    RunListOfSingleClusters(&cluster2, 2);
    RunListOfSingleClusters(&cluster3, 3);
  }


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
    // mrk->Draw("SAME");

    TLine *line = new TLine(-45, -45 * rotm + rotp, 120, 120 * rotm + rotp);
    line->SetLineColor(kGray);
    //    line->Draw("SAME");

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
    //    ell1->Draw("SAME");

    TEllipse *ell2 = new TEllipse(x0b, y0, a, b);
    ell2->SetLineColor(kGreen);
    ell2->SetFillStyle(0);    
    //    ell2->Draw("SAME");

    TMarker *mrk1 = new TMarker(intxa, intya, 21);
    mrk1->SetMarkerColor(kOrange);
    //    mrk1->Draw("SAME");
     
    TMarker *mrk2 = new TMarker(intxb, intyb, 21);
    mrk2->SetMarkerColor(kGreen);
    //   mrk2->Draw("SAME");
     
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
 //    double degbeta = TMath::RadToDeg();
//     TEllipse *ell1 = new TEllipse(x0anew, y0anew, a, b, 0, 360, degbeta);
//     ell1->SetLineColor(kOrange);
//     ell1->SetFillStyle(0);    
//     ell1->Draw("SAME");
 
//     TEllipse *ell2 = new TEllipse(x0bnew, y0bnew, a, b, 0, 360, degbeta);
//     ell2->SetLineColor(kGreen);
//     ell2->SetFillStyle(0);    
//     ell2->Draw("SAME");

//     TMarker *mrk1 = new TMarker(intxa, intya, 21);
//     mrk1->SetMarkerColor(kOrange);
//     mrk1->Draw("SAME");

//     TMarker *mrk2 = new TMarker(intxb, intyb, 21);
//     mrk2->SetMarkerColor(kGreen);
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

Int_t PndSecondaryTrackFinder::FindCharge(Double_t oX, Double_t oY, std::vector<int> cluster)
{
  Int_t charge = 0;  

  UShort_t nleft = 0;
  UShort_t nright = 0;
  
  Double_t cross, disq, minl = 9999999., minr = 9999999.;
  
  // this methods works with the hypothesis that this track comes
  //  from (0,0)
  Int_t nParallelHits = 0;

  for(Int_t ihit=0; ihit < cluster.size(); ihit++){ 
    Int_t hitid = (Int_t) cluster[ihit];
    
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


// +++++++++++
std::vector<int> PndSecondaryTrackFinder::OrderByDistanceFromRefPointWithoutCharge(std::vector<int> cluster, TVector3 refposition)
 {

  std::vector<int> sorthits;

  std::vector<double> distances;
  std::multimap<double, int> mapdistances;

  for(int ihit = 0; ihit < cluster.size(); ihit++) {
    Int_t hitid = cluster[ihit];
    PndSttHit *hit = (PndSttHit*) fSttHitArray->At(hitid);

    TVector3 position(hit->GetX(), hit->GetY(), 0.);
    
    double distance = (refposition - position).Perp();
    
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
	int hitno = (*it).second;
	int hitid = cluster[hitno];
	sorthits.push_back(hitid);
	count++;
      }
  }

 
  return sorthits;
}

// +++++++++++
 std::vector< std::pair< int, int > > PndSecondaryTrackFinder::OrderByDistanceFromRefPointWithoutCharge(std::vector< std::pair<int, int> > track, TVector3 refposition)
{
  
  std::vector< std::pair< int, int > > sorthits;
  
  std::vector<double> distances;
  std::multimap<double, int> mapdistances;
  
  for(int ihit = 0; ihit < track.size(); ihit++) {
    std::pair<int, int> lpair = track[ihit];
    int detid = lpair.first;
    int hitid = lpair.second;
    
    int hitidinfulllist = hitid + (fNofMvdPixHits + fNofMvdStrHits);
    TVector3 position;
    if(fFullList[hitidinfulllist][2] == 4 || fFullList[hitidinfulllist][2] == 5) {
      std::map< int, TVector3 >::iterator it = fskewintmap.find(hitid);
      position = (*it).second;
    }
    else {
      PndSttHit *hit = (PndSttHit*) fSttHitArray->At(hitid);
      position.SetXYZ(hit->GetX(), hit->GetY(), 0.);
    }
    double distance = (refposition - position).Perp();
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
	int hitno = (*it).second;
	std::pair<int,int> lpair = track[hitno];
	sorthits.push_back(lpair);
	count++;
      }
  }
  
 
  return sorthits;
}




std::vector< std::vector<int> > PndSecondaryTrackFinder::SttClusterFinder(int iregion) {

  //  cout << "STT CLUST FINDER " << endl;
  std::vector< std::vector<int> > cluster; // CHECK THIS!!
  int nclus = 0;
  // region 2
  for(int hit1 = fNofMvdPixHits + fNofMvdStrHits; hit1 < fNofHits; hit1++) {
    std::vector<int> thiscluster;
    //  cout << "*** " << hit1 << " " << fFullList[hit1][3] << " " << fFullList[hit1][2]  << endl;
    if(fFullList[hit1][3] == 0) continue;
    if(fFullList[hit1][2] != iregion) continue;
    thiscluster.push_back(fFullList[hit1][0]);
    int counter = 1;
//     cout << "=========== HIT1: " << fFullList[hit1][0] << endl;

    for(int hit2 = fNofMvdPixHits + fNofMvdStrHits + 1; hit2 < fNofHits; hit2++) {
      if(fFullList[hit2][3] == 0) continue;
      if(fFullList[hit2][2] != iregion) continue;
//       cout << "----- HIT2: " << fFullList[hit2][0] << " for " << counter << endl;
      
      for(int hit3 = 0; hit3 < counter; hit3++) {
	int hitid = thiscluster[hit3];
	if(fFullList[hit2][0] == hitid) continue;
// 	cout << "time " << hit3 << endl;
// 	cout << "... HIT3 compare " << hitid << " to " << fFullList[hit2][0] << endl;

	Bool_t success = IsAssociate(fFullList[hit2][0], hitid);
// 	cout << "success? " << success << endl;
	if(!success) continue;
	fFullList[hitid + fNofMvdPixHits + fNofMvdStrHits][3] = 0;
	fFullList[hit2][3] = 0;
	thiscluster.push_back(fFullList[hit2][0]);
	counter++;
	break;
      }
    }
    //    cout << "CLUSTER " << nclus << " with " << counter << " hits" << endl;
//       for(int hit3 = 0; hit3 < counter; hit3++) {
// 	int hitid = thiscluster[hit3];
// 	//	cout << " " << hitid;
//       }
      //      cout <<  endl;
      std::vector<int> sortcluster = OrderByDistanceFromRefPointWithoutCharge(thiscluster, TVector3(0., 0., 0.));

      cluster.push_back(sortcluster);
      nclus++;
  }

  for(int hit1 = fNofMvdPixHits + fNofMvdStrHits; hit1 < fNofHits; hit1++) fFullList[hit1][3] = 1;

  return cluster;
}

Bool_t PndSecondaryTrackFinder::IsAssociate(Int_t hitID1, Int_t hitID2) {

  PndSttHit *hit1 = (PndSttHit*) fSttHitArray->At(hitID1);
  double xc1 = hit1->GetX();
  double yc1 = hit1->GetY();
  double rc1 = hit1->GetIsochrone();
  PndSttHit *hit2 = (PndSttHit*) fSttHitArray->At(hitID2);
  double xc2 = hit2->GetX();
  double yc2 = hit2->GetY();
 
  double d = TMath::Sqrt((xc1 - xc2) * (xc1 - xc2) + (yc1 - yc2) * (yc1 - yc2));
//   cout << "xc/yc1: "<< xc1 << " " << yc1 << endl;
//   cout << "xc/yc2: "<< xc2 << " " << yc2 << endl;

//   cout << "distance " << d << endl;;
  if(d < 1.2) return kTRUE;
  return kFALSE;

}


Int_t PndSecondaryTrackFinder::ComputeTraAndRot(std::vector<int> cluster, Double_t &delta, Double_t trasl[2]) {
  
  int nhits = cluster.size();
  double tmpdrift = 0.5; // CHECK
  int tmphitid = -1;

  for(int ihit = 0; ihit < nhits; ihit++)
    {
      int hitid = cluster[ihit];
      
      if(fFullList[hitid][3] == 0) continue;
      if(fFullList[hitid][0] != hitid) {
	cout << "ERROR HERE" << endl;
	continue;
      }
      
      PndSttHit *hit = (PndSttHit*) fSttHitArray->At(hitid);
      if(!hit) continue; 
      
      Double_t rd = hit->GetIsochrone();
      if(rd < tmpdrift) {
	tmpdrift = rd;
	tmphitid = hitid;
      }

      
    }


  Double_t firstdrift = tmpdrift;
  int firsthitid = tmphitid; 
  FairHit *hitfirst = (FairHit*) fSttHitArray->At(firsthitid);
  if(!hitfirst) return kFALSE;
  TVector3 positionfirst;
  hitfirst->Position(positionfirst);


  int innerhitid = cluster[0];
  int outerhitid = cluster[nhits - 1];
 
  FairHit *hitinner = (FairHit*) fSttHitArray->At(innerhitid);
  if(!hitinner) return kFALSE;
  TVector3 positioninner;
  hitinner->Position(positioninner);
  
  FairHit *hitouter = (FairHit*) fSttHitArray->At(outerhitid);
  if(!hitouter) return kFALSE;
  TVector3 positionouter;
  hitouter->Position(positionouter);

  trasl[0] = positionfirst.X();
  trasl[1] = positionfirst.Y();

  delta = TMath::ATan2(positionouter.Y() - positioninner.Y(),
		       positionouter.X() - positioninner.X());
  
  return firsthitid;
}

std::vector<std::vector<double> > PndSecondaryTrackFinder::ConformalPlaneHits(std::vector<int> cluster, int firsthitid, Double_t delta, Double_t trasl[2]) {


  std::vector<std::vector<double> > conformalhits;

  if(fVerbose) cout << "GOING TO FIT CLUSTER " << endl;
  if(fDisplayOn) {
    char goOnChar;
    cout << "light up the cluster, press any key" << endl;
    cin >> goOnChar;
 
    //   cout << "GOING ON" << endl;
    DrawGeometryConformal(-3, -3, 3, 3); 
  }
  
  TVector3 centerposition; 
  int nhits = cluster.size();

  for(int ihit = 0; ihit < nhits; ihit++)
    {
      int hitid = cluster[ihit];
      if(fFullList[hitid][3] == 0) continue;
      if(fFullList[hitid][0] != hitid) {
	cout << "ERROR HERE" << endl;
	continue;
      }

      PndSttHit *hit = (PndSttHit*) fSttHitArray->At(hitid);
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
	params.push_back(hitid);
	conformalhits.push_back(params);
      }
    
      if(fDisplayOn) {
            
 	TArc *arc = new TArc(u, v, rc);
	arc->SetLineColor(kRed);
 	arc->SetFillStyle(0);
	arc->Draw("SAME");
 	display->Update();
 	display->Modified();  
// 	char goOnChar;
// 	cout << "uv press any key" << endl;

      }
    }
 
  return conformalhits;

 
}




Bool_t PndSecondaryTrackFinder::ConformalFit(std::vector<std::vector<double> > conformalhits, Double_t delta, Double_t trasl[2], Double_t &xc, Double_t &yc, Double_t &radius, Double_t &m, Double_t &p)
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
  Double_t alpha, beta, gamma;
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
    cout << "Go back to real plane " << endl;
    cin >> goOnChar; 
    if(fDisplayOn) Refresh();
    cout << "helix " << xc << " " << yc << " " << radius << endl;     
    TArc *arc = new TArc(xc, yc, radius);
    arc->SetLineColor(kGreen);
    arc->SetFillStyle(0);
    arc->Draw("SAME ONLY");
    display->Update();
    display->Modified();
    cin >> goOnChar; 
  }

  return kTRUE;

}

Bool_t PndSecondaryTrackFinder::SelectHits(std::vector<std::vector<double> > conformalhits, Double_t m, Double_t p, Double_t &chi2, Double_t &fullchi2, std::vector<int> *goodhits,       std::vector< std::pair<int, int> > *lefthits) { 

  // sector 2, 6, 3, 7
  int goodhitscounter[2][4];
  for(int i = 0; i < 2; i++) for(int j = 0; j < 4; j++) goodhitscounter[i][j] = 0;

  chi2 = 0, fullchi2 = 0;
  double denominator = TMath::Sqrt(m * m + 1);
  int nhits = conformalhits.size();
  double limit = 0.8; // 0.5; // CHECK

  //  cout << "SELECT NHITS " <<  nhits << endl;
  for(int ihit = 0; ihit < nhits; ihit++) {
    std::vector<double> thishit = conformalhits[ihit];
    double perpdist = fabs(thishit[0] * m - thishit[1] + p) / denominator;
    double rdrift = thishit[2];
  
    double sigma;
    if(thishit[2] > 1.e-10) sigma = thishit[2]; // CHECK
    else sigma = STRAWRADIUS;

    double chi2element = fabs((perpdist - rdrift) / sigma);
    fullchi2 += chi2element;
    int hitid = (int) thishit[3];



   int iregion = fFullList[hitid][2];

    if(iregion == 2) goodhitscounter[0][0]++;
    else if(iregion == 6) goodhitscounter[0][1]++;
    else if(iregion == 3) goodhitscounter[0][2]++;
    else if(iregion == 7) goodhitscounter[0][3]++;

    if(limit > chi2element) {
      chi2 += chi2element;

      if(fFullList[hitid][3] == 1) {
	goodhits->push_back(fFullList[hitid][0]);
	
	if(iregion == 2) goodhitscounter[1][0]++;
	else if(iregion == 6) goodhitscounter[1][1]++;
	else if(iregion == 3) goodhitscounter[1][2]++;
	else if(iregion == 7) goodhitscounter[1][3]++;
	
      }
      else cout << "THIS HIT SHOULD NOT BE USED " << hitid << endl;
    }
    else {   
      std::pair<int, int> leftpair(fFullList[hitid][0], fFullList[hitid][2]);

      if(fFullList[hitid][3] == 1) lefthits->push_back(leftpair);
    }
  }
    
 
  chi2 /= nhits;
  fullchi2 /= nhits ;
  // cout << "chi2 RED " << chi2 << " " << fullchi2 << " " << goodhits->size() << endl;
  double perc2 = 1., perc6 = 1., perc3 = 1., perc7 = 1.;
  if(goodhitscounter[0][0] != 0) perc2 = (double) goodhitscounter[1][0]/goodhitscounter[0][0];
  if(goodhitscounter[0][1] != 0) perc6 = (double) goodhitscounter[1][1]/goodhitscounter[0][1];
  if(goodhitscounter[0][2] != 0) perc3 = (double) goodhitscounter[1][2]/goodhitscounter[0][2];
  if(goodhitscounter[0][3] != 0) perc7 = (double) goodhitscounter[1][3]/goodhitscounter[0][3];


//   cout << "PERCENTAGES " << perc2 << " " << perc6 << " " << perc3 << " " << perc7 << " " << perc2 * perc6 * perc3 * perc7 << endl;
//   for(int i = 0; i < 2; i++) {
//     cout << "TOT " << i << endl;
//     for(int j = 0; j < 4; j++) {
//       cout << " " << goodhitscounter[i][j];
//     }
//     cout << endl;
//   }

  double pertot = (double)(goodhitscounter[1][0] + goodhitscounter[1][1] + goodhitscounter[1][2] + goodhitscounter[1][3])/(goodhitscounter[0][0] + goodhitscounter[0][1] + goodhitscounter[0][2] + goodhitscounter[0][3]);
 //    cout << "PERTOT " << pertot << endl;

    // if((perc2 < 0.3 || perc6 < 0.3 || perc3 < 0.3 || perc7 < 0.3) || pertot < 0.6) return kFALSE;
    if(perc2 < 0.3 || perc6 < 0.3 || perc3 < 0.3 || perc7 < 0.3) return kFALSE;

  return kTRUE;
}




Bool_t PndSecondaryTrackFinder::FullFit(std::vector<int> cluster, Double_t &xc, Double_t &yc, Double_t &radius, std::vector<int> *goodhits, std::vector< std::pair<int, int> > *lefthits) {

  if(cluster.size() < 3) return kFALSE;
  
  Double_t delta, trasl[2];
  Int_t firsthitid = ComputeTraAndRot(cluster, delta, trasl);
  std::vector<std::vector<double> > conformal = ConformalPlaneHits(cluster, firsthitid, delta, trasl);
  Double_t m, p;
  bool conffit = kFALSE;
  if(conformal.size() == 0) return kFALSE;

  conffit = ConformalFit(conformal, delta, trasl, xc, yc, radius, m, p);
  if(conffit == kFALSE) return kFALSE;
  
  Double_t chi2 = 0, fullchi2 = 0;
  Bool_t selection = SelectHits(conformal, m, p, chi2, fullchi2, goodhits, lefthits); // CHECK
  goodhits->push_back(firsthitid);
  
  if(fDisplayOn) {
    char goOnChar;
    cout << "press any key to light it up" << endl;
    cin >> goOnChar;
    cout << "GOING ON" << endl;
    LightSttCluster(*goodhits);
    display->Update();
    display->Modified();
    cin >> goOnChar;
    cout << "GOING ON" << endl;
  }
  
  if(fullchi2 <= fChi2Limit) {                                // ........... GOOD
    return kTRUE;
  } 
  else if(fullchi2 > fChi2Limit && chi2 < fChi2Limit) {      // ............ NOT GOOD, BUT RECOVERABLE
    if(goodhits->size() >= 0.5 * cluster.size() && selection == kTRUE) {
      firsthitid = ComputeTraAndRot(*goodhits, delta, trasl);
      conformal.clear();
      conformal = ConformalPlaneHits(*goodhits, firsthitid, delta, trasl);
      if(conformal.size() == 0) return kFALSE;
      conffit = ConformalFit(conformal, delta, trasl, xc, yc, radius, m, p);
      if(conffit == kFALSE) return kFALSE;
      return kTRUE;
    }
  }
  return kFALSE;                                             // ............ UNRECOVERABLE


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
		       //         ,final_values, TIMEOUT  //  TIMEOUT timeout is in seconds. glp_main_t
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
void PndSecondaryTrackFinder::SequentialHistogramming(std::vector<int> thiscluster) {

  int nhits = thiscluster.size();
  // n * (n - 1) * (n - 2) / (3 * 2)
  fNofTriplets = nhits * (nhits - 1) * (nhits - 2) / 6;
  fNofTriplets *= 8;

  CalculateHr(thiscluster);
  bool tripl = false;
  
  cout << "nhits " << nhits << endl;
  while(nhits > 0) 
    {
      if(tripl == true) {
	CalculateHr(thiscluster);
	tripl = false;
      }
      double r;
      bool foundR = FindNextRPeak(r);

      if(foundR == false) break;
      cout << "=============== findR " << r << endl;
      
      CalculateHa(r);
      while(tripl == false && fHa->Integral() > 0)
	{   
	  double a;
	  bool foundA = FindNextAPeak(a);
	  if(foundA == false) break;
	  cout << "=============== finda " << a << endl;
	  CalculateHb(r, a);
	  
	  while(tripl == false  && fHb->Integral() > 0)
	    {
	      if(fDisplayOn) {
		char goOnChar;
		fHb->Draw();
		display->Update();
		display->Modified();
		cin >> goOnChar;
	      }
	     double b;
	      bool foundB = FindNextBPeak(b);
	      if(foundB == false) break;
	      cout << "=============== findb " << b << endl;

	      std::vector<int> foundhits;
	      tripl = Test(a, b, r, &foundhits);

	      cout << "trip " << tripl << " " << foundhits.size() << endl;
	      if(tripl == true) {
		// SaveTriplet(a, b, r);
		cout << "ARC " << a << " " << b << " " << r << endl;

		DrawGeometry();
		DrawHits();
		TArc *arc = new TArc(a, b, r);
		arc->SetFillStyle(0);
		arc->Draw("SAME");
		display->Update();
		display->Modified();
		char goOnChar;
		cin >> goOnChar;

		DeleteSttHits(&foundhits);
		nhits -= foundhits.size();
	      }
	    }
	}
    }


//   TArc *arc = new TArc(centerx, centery, radius);
//   arc->SetFillStyle(0);
//   arc->Draw("SAME");
//   display->Update();
//   display->Modified();
//   cin >> goOnChar;
}

void PndSecondaryTrackFinder::LightSttCluster(std::vector<int> sttcluster) {

  cout << "LIGHT UP" << endl;
  DrawGeometry();
  DrawHits();
  for(int ihit = 0; ihit < sttcluster.size(); ihit++) {
    int hitid = sttcluster[ihit];
    
    PndSttHit *hit = (PndSttHit*) fSttHitArray->At(hitid);
    double xc = hit->GetX();
    double yc = hit->GetY();

    TVector3 position(xc, yc, 0.);
    //    cout << "phi " <<     position.Phi() * 360./TMath::Pi() << endl;
    // cout << "HITID " << hitid << endl;


    TArc *arc = new TArc(xc, yc, 0.5);
    arc->SetFillColor(kYellow);
    //	arc->SetFillStyle(0);
    arc->Draw("SAME");
    display->Update();
    display->Modified();
  }
}


void PndSecondaryTrackFinder::CalculateHr(std::vector<int> thiscluster) {
  fHr->Reset();
  fgoodtriplet->clear();
  cout << "CALCULATE R" << endl;
  int rcounter = 0;

  // find the big circle  X CENTER ------------------------------------------>
  for(int i = 0; i < thiscluster.size(); i++) {
    for(int j = i + 1; j < thiscluster.size(); j++) {
      for(int k = j + 1; k < thiscluster.size(); k++) {
	int a = thiscluster[i];
	int b = thiscluster[j];
	int c = thiscluster[k];

	if(fFullList[a][3] == 0 || fFullList[b][3] == 0 || fFullList[c][3] == 0) {
	  //	  cout << "NOT USABLE" << endl;
	  continue;
	}

	if(fFullList[a][2] == 4 || fFullList[a][2] == 5 || fFullList[b][2] == 4 || fFullList[b][2] == 5 || fFullList[c][2] == 4 || fFullList[c][2] == 5) continue;

	int hitID1 = fFullList[a][0];
	int hitID2 = fFullList[b][0];
	int hitID3 = fFullList[c][0];
	 	cout << "USABLE " << a << " " << b << " " << c << " " << hitID1 << " " << hitID2 << " " << hitID3 << endl;
	
	Int_t rcountertmp = Find8Circles(hitID1, hitID2, hitID3);
	//	cout << "rcountertmp " << rcountertmp << endl;  
	for(int tri = fgoodtriplet->size() - rcountertmp; tri < fgoodtriplet->size(); tri++) {
	  std::vector<double> thistriplet = fgoodtriplet->at(tri);
	  fHr->Fill(thistriplet[5]);
	   cout << tri << " " << thistriplet[5] << endl;
	}
      }
    }
  }
  fNofTriplets = fgoodtriplet->size();
  if(fDisplayOn) {
    fHr->Draw();
    display->Update();
    display->Modified();
    char goOnChar;
    cin >> goOnChar;
  }

}

// +++++++++++



Int_t PndSecondaryTrackFinder::Find8Circles(int hitID1, int hitID2, int hitID3) {

  cout << "FIND 8 CIRL " << hitID1 << " " << hitID2 << " " << hitID3 << endl;
  int rcounter = 0;
  double circle[3][5];
  PndSttHit *hit1 = (PndSttHit*) fSttHitArray->At(hitID1);
  double xc1 = hit1->GetX();
  double yc1 = hit1->GetY();
  double rc1 = hit1->GetIsochrone();
  PndSttHit *hit2 = (PndSttHit*) fSttHitArray->At(hitID2);
  double xc2 = hit2->GetX();
  double yc2 = hit2->GetY();
  double rc2 = hit2->GetIsochrone();
  PndSttHit *hit3 = (PndSttHit*) fSttHitArray->At(hitID3);
  double xc3 = hit3->GetX();
  double yc3 = hit3->GetY();
  double rc3 = hit3->GetIsochrone();

  cout << "HIT" << hitID1 << ": " << xc1 << " " << yc1 << " " << endl;
  cout << "HIT" << hitID2 << ": " << xc2 << " " << yc2 << " " << endl;
  cout << "HIT" << hitID3 << ": " << xc3 << " " << yc3 << " " << endl;

  // SOME TESTS
  if((xc2 - xc1) == 0) { cout << "BLA1" << endl; return rcounter; }
  if((yc2 - yc1) == 0)  { cout << "BLA2" << endl; return rcounter; }
//   if(TMath::Sqrt((xc1 - xc2) * (xc1 - xc2) + (yc1 - yc2) * (yc1 - yc2)) > 30. || TMath::Sqrt((xc1 - xc2) * (xc1 - xc2) + (yc1 - yc2) * (yc1 - yc2)) < 10. ) return rcounter;
  
  if((xc3 - xc1) == 0) { cout << "BLA3" << endl;  return rcounter; }
  if((yc3 - yc1) == 0)  { cout << "BLA4" << endl; return rcounter; }
 //  if(TMath::Sqrt((xc1 - xc3) * (xc1 - xc3) + (yc1 - yc3) * (yc1 - yc3)) > 30. || TMath::Sqrt((xc1 - xc3) * (xc1 - xc3) + (yc1 - yc3) * (yc1 - yc3)) < 10.) return rcounter;

  // find the big circle  X CENTER ------------------------------------------>
  for(int csign1 = -1; csign1 <= 1; csign1 += 2) {
    for(int csign2 = -1; csign2 <= 1; csign2 += 2) {
      double avar   = 2 * (xc1 - xc2);
      double bvar   = 2 * (yc1 - yc2);
      double cvar = 2 * (csign1 * rc1 + csign2 * rc2);
      double dvar = (xc1 * xc1 + yc1 * yc1 - rc1 * rc1) - (xc2 * xc2 + yc2 * yc2 - rc2 * rc2);
	    
      for(int csignp = -1; csignp <= 1; csignp += 2) {
	double avarp   = 2 * (xc1 - xc3);
	double bvarp   = 2 * (yc1 - yc3);
	double cvarp = 2 * (csign1 * rc1 + csignp * rc3);
	double dvarp = (xc1 * xc1 + yc1 * yc1 - rc1 * rc1) - (xc3 * xc3 + yc3 * yc3 - rc3 * rc3);
	      
	// ======================================
	double thisq = (dvar * bvarp - dvarp * bvar) / (avar * bvarp - avarp * bvar);
	double thism = (bvar * cvarp - bvarp * cvar) / (avar * bvarp - avarp * bvar);
	  
	double thisp = (avar * dvarp - avarp * dvar) / (avar * bvarp - avarp * bvar);
	double thiss = (cvar * avarp - cvarp * avar) / (avar * bvarp - avarp * bvar);
	if(fabs(thism) == fabs(TMath::Infinity()) || (fabs(thisq) == fabs(TMath::Infinity())) || (fabs(thiss) == fabs(TMath::Infinity())) || (fabs(thisp) == fabs(TMath::Infinity()))) continue;

	if(TMath::IsNaN(thism) || TMath::IsNaN(thisq) || TMath::IsNaN(thiss) || TMath::IsNaN(thisp)) continue;

// 	cout << "var " << avar << " " << bvar << " " << cvar << " " << dvar << endl;
// 	cout << "varp " << avarp << " " << bvarp << " " << cvarp << " " << dvarp << endl;
//	cout << "this " << thism  << " " << thisq << " " << thiss << " " << thisp << endl;
	// ======================================
	// thisa x**2 + 2 * thisb x + thisc = 0
	  
	double thisa = thism * thism + thiss * thiss - 1;
	double thisb = thism * (thisq - xc1) + thiss * (thisp - yc1) - csign1 * rc1;
	double thisc =  (thisq - xc1) *  (thisq - xc1) + (thisp - yc1) * (thisp - yc1) - rc1 * rc1;

	if((thisb * thisb - thisa * thisc) < 0) continue;
	double Ri = (- thisb + TMath::Sqrt(thisb * thisb - thisa * thisc))/ thisa;
	// cout << "r1 " << Ri << endl;
	if(Ri < 0) {
	  Ri = (- thisb - TMath::Sqrt(thisb * thisb - thisa * thisc))/ thisa;
	  // cout << "r2 " << Ri << endl;
	}
	cout << rcounter << " => " << Ri << endl;

	if(Ri < 0) continue; // CHECK
	      
	if(Ri > (15./0.006) || Ri < (0.1/0.006)) continue; // CHECK
	//       cout << rcounter << " => " << thism << " " << thisq << " " << thiss << " " << thisp << endl;

	double Xi = thism * Ri + thisq;
	double Yi  = thiss * Ri + thisp;
	std::vector<double> tmpgoodtriplet;
	tmpgoodtriplet.push_back(hitID1);
	tmpgoodtriplet.push_back(hitID2);
	tmpgoodtriplet.push_back(hitID3);
	tmpgoodtriplet.push_back(Xi);
	tmpgoodtriplet.push_back(Yi);
	tmpgoodtriplet.push_back(Ri);
	tmpgoodtriplet.push_back(thism);
	tmpgoodtriplet.push_back(thisq);
	tmpgoodtriplet.push_back(thiss);
	tmpgoodtriplet.push_back(thisp);
	//	cout << rgoodtriplet->size() << " => " << Ri << " " << hitID1 << " " << hitID2 << " " << hitID3 << endl;
	fgoodtriplet->push_back(tmpgoodtriplet);
	rcounter++;
	      
      }
    }
  }
  return rcounter;
}


void PndSecondaryTrackFinder::SequentialHistogramming() {

  int nhits = fNofHits;
  // n * (n - 1) * (n - 2) / (3 * 2)
  fNofTriplets = nhits * (nhits - 1) * (nhits - 2) / 6;
  fNofTriplets *= 8;

  CalculateHr();
  bool tripl = false;
  
  cout << "nhits " << nhits << endl;
  while(nhits > 0) 
    {
      if(tripl == true) {
	CalculateHr();
	tripl = false;
      }
      double r;
      bool foundR = FindNextRPeak(r);

      if(foundR == false) break;
      cout << "=============== findR " << r << endl;
      
      CalculateHa(r);
      while(tripl == false && fHa->Integral() > 0)
	{   
	  double a;
	  bool foundA = FindNextAPeak(a);
	  if(foundA == false) break;
	  cout << "=============== finda " << a << endl;
	  CalculateHb(r, a);
	  
	  while(tripl == false  && fHb->Integral() > 0)
	    {
	      if(fDisplayOn) {
		char goOnChar;
		fHb->Draw();
		display->Update();
		display->Modified();
		cin >> goOnChar;
	      }
	     double b;
	      bool foundB = FindNextBPeak(b);
	      if(foundB == false) break;
	      cout << "=============== findb " << b << endl;

	      std::vector<int> foundhits;
	      tripl = Test(a, b, r, &foundhits);

	      cout << "trip " << tripl << " " << foundhits.size() << endl;
	      if(tripl == true) {
		// SaveTriplet(a, b, r);
		cout << "ARC " << a << " " << b << " " << r << endl;

		DrawGeometry();
		DrawHits();
		TArc *arc = new TArc(a, b, r);
		arc->SetFillStyle(0);
		arc->Draw("SAME");
		display->Update();
		display->Modified();
		char goOnChar;
		cin >> goOnChar;

		DeleteSttHits(&foundhits);
		nhits -= foundhits.size();
	      }
	    }
	}
    }


//   TArc *arc = new TArc(centerx, centery, radius);
//   arc->SetFillStyle(0);
//   arc->Draw("SAME");
//   display->Update();
//   display->Modified();
//   cin >> goOnChar;
}



void PndSecondaryTrackFinder::CalculateHr() {
  fHr->Reset();
  fgoodtriplet->clear();
  cout << "CALCULATE R" << endl;
  int rcounter = 0;

  // find the big circle  X CENTER ------------------------------------------>
  for(int a = fNofMvdPixHits + fNofMvdStrHits; a < fNofHits; a++) {
    for(int b = a + 1; b < fNofHits; b++) {
      for(int c = b + 1; c < fNofHits; c++) {

	if(fFullList[a][3] == 0 || fFullList[b][3] == 0 || fFullList[c][3] == 0) {
	  cout << "NOT USABLE" << endl;
	  continue;
	}

	if(fFullList[a][2] == 4 || fFullList[a][2] == 5 || fFullList[b][2] == 4 || fFullList[b][2] == 5 || fFullList[c][2] == 4 || fFullList[c][2] == 5) continue;

	int hitID1 = fFullList[a][0];
	int hitID2 = fFullList[b][0];
	int hitID3 = fFullList[c][0];
	cout << "USABLE " << a << " " << b << " " << c << " " << hitID1 << " " << hitID2 << " " << hitID3 << endl;

	Int_t rcountertmp = Find8Circles(hitID1, hitID2, hitID3);
	  
	for(int tri = fgoodtriplet->size() - rcountertmp; tri < fgoodtriplet->size(); tri++) {
	  std::vector<double> thistriplet = fgoodtriplet->at(tri);
	  fHr->Fill(thistriplet[5]);
	  // cout << tri << " " << thistriplet[5] << endl;
	}
      }
    }
  }
  fNofTriplets = fgoodtriplet->size();
  if(fDisplayOn) {
    fHr->Draw();
    display->Update();
    display->Modified();
    char goOnChar;
    cin >> goOnChar;
  }

}
  


void PndSecondaryTrackFinder::CalculateHa(Double_t r1) {

  fHa->Reset();
  cout << "n triplet " << fNofTriplets << endl; 
  std::vector< std::vector<double> > newgoodtriplet;

  for(int t = 0; t < fNofTriplets; t++) {

    std::vector<double> thistriplet = fgoodtriplet->at(t);

    int a = (int) thistriplet[0];
    int b = (int) thistriplet[1];
    int c = (int) thistriplet[2];
    double Ri = thistriplet[3];
    double Xi = thistriplet[4];
    double Yi = thistriplet[5];
    double thism = thistriplet[6];
    double thisq = thistriplet[7];
    double thiss = thistriplet[8];
    double thisp = thistriplet[9];

    
    //     cout << t << " => " << thism << " " << thisq << " " << thiss << " " << thisp << endl;
    //     cout << r1 << endl;
    Ri = r1;
    Xi = thism * Ri + thisq;
    
    PndSttHit *hit1 = (PndSttHit*) fSttHitArray->At(a);
    double xc1 = hit1->GetX();
    double yc1 = hit1->GetY();
    double rc1 = hit1->GetIsochrone();
    PndSttHit *hit2 = (PndSttHit*) fSttHitArray->At(b);
    double xc2 = hit2->GetX();
    double yc2 = hit2->GetY();
    double rc2 = hit2->GetIsochrone();
    PndSttHit *hit3 = (PndSttHit*) fSttHitArray->At(c);
    double xc3 = hit3->GetX();
    double yc3 = hit3->GetY();
    double rc3 = hit3->GetIsochrone();

    double d1c = TMath::Sqrt((xc1 - Xi) * (xc1 - Xi) + (yc1 - Yi) * (yc1 - Yi));
    if(d1c > Ri && fabs(d1c - rc1 - Ri) > fLimit) continue;
    else if(d1c < Ri && fabs(d1c + rc1 - Ri) > fLimit) continue;
    
    double d2c = TMath::Sqrt((xc2 - Xi) * (xc2 - Xi) + (yc2 - Yi) * (yc2 - Yi));
    if(d2c > Ri && fabs(d2c - rc2 - Ri) > fLimit) continue;
    else if(d2c < Ri && fabs(d2c + rc2 - Ri) > fLimit) continue;
    
    double d3c = TMath::Sqrt((xc3 - Xi) * (xc3 - Xi) + (yc3 - Yi) * (yc3 - Yi));
    if(d3c > Ri && fabs(d3c - rc3 - Ri) > fLimit) continue;
    else if(d3c < Ri && fabs(d3c + rc3 - Ri) > fLimit) continue;

    if(fabs(Xi) > 2500.) continue; // CHECK




  
    fHa->Fill(Xi);
    //     cout << t << " => " << Xi << endl;

    std::vector<double> tmpgoodtriplet;
    tmpgoodtriplet.push_back(a);
    tmpgoodtriplet.push_back(b);
    tmpgoodtriplet.push_back(c);
    tmpgoodtriplet.push_back(Ri);
    tmpgoodtriplet.push_back(Xi);
    tmpgoodtriplet.push_back(Yi);
    tmpgoodtriplet.push_back(thism);
    tmpgoodtriplet.push_back(thisq);
    tmpgoodtriplet.push_back(thiss);
    tmpgoodtriplet.push_back(thisp);
    newgoodtriplet.push_back(tmpgoodtriplet);
  }
  *fgoodtriplet = newgoodtriplet;
  fNofTriplets = fgoodtriplet->size();

  if(fDisplayOn) {
    char goOnChar;
    fHa->Draw();
    display->Update();
    display->Modified();
    cin >> goOnChar;
  }
}


void PndSecondaryTrackFinder::CalculateHb(Double_t r1, Double_t x1) {

  fHb->Reset();
  std::vector< std::vector<double> > newgoodtriplet;
  for(int t = 0; t < fNofTriplets; t++) {
    
    std::vector<double> thistriplet = fgoodtriplet->at(t);
    
    int a = (int) thistriplet[0];
    int b = (int) thistriplet[1];
    int c = (int) thistriplet[2];
    double Ri = thistriplet[3];
    double Xi = thistriplet[4];
    double Yi = thistriplet[5];
    double thism = thistriplet[6];
    double thisq = thistriplet[7];
    double thiss = thistriplet[8];
    double thisp = thistriplet[9];

//     cout << t << " => " << thism << " " << thisq << " " << thiss << " " << thisp << endl;
//     cout << r1 << endl;
    Ri = r1;
    Xi = x1;
    Yi = thiss * Ri + thisp;
    
    
    if(fabs(Yi) > 2500.) continue; // CHECK
    
    PndSttHit *hit1 = (PndSttHit*) fSttHitArray->At(a);
    double xc1 = hit1->GetX();
    double yc1 = hit1->GetY();
    double rc1 = hit1->GetIsochrone();
    PndSttHit *hit2 = (PndSttHit*) fSttHitArray->At(b);
    double xc2 = hit2->GetX();
    double yc2 = hit2->GetY();
    double rc2 = hit2->GetIsochrone();
    PndSttHit *hit3 = (PndSttHit*) fSttHitArray->At(c);
    double xc3 = hit3->GetX();
    double yc3 = hit3->GetY();
    double rc3 = hit3->GetIsochrone();

    double d1c = TMath::Sqrt((xc1 - Xi) * (xc1 - Xi) + (yc1 - Yi) * (yc1 - Yi));
    if(d1c > Ri && fabs(d1c - rc1 - Ri) > fLimit) continue;
    else if(d1c < Ri && fabs(d1c + rc1 - Ri) > fLimit) continue;
    
    double d2c = TMath::Sqrt((xc2 - Xi) * (xc2 - Xi) + (yc2 - Yi) * (yc2 - Yi));
    if(d2c > Ri && fabs(d2c - rc2 - Ri) > fLimit) continue;
    else if(d2c < Ri && fabs(d2c + rc2 - Ri) > fLimit) continue;
    
    double d3c = TMath::Sqrt((xc3 - Xi) * (xc3 - Xi) + (yc3 - Yi) * (yc3 - Yi));
    if(d3c > Ri && fabs(d3c - rc3 - Ri) > fLimit) continue;
    else if(d3c < Ri && fabs(d3c + rc3 - Ri) > fLimit) continue;

    fHb->Fill(Yi);
//     cout << t << " => " << Yi << endl;
    
    std::vector<double> tmpgoodtriplet;
    tmpgoodtriplet.push_back(a);
    tmpgoodtriplet.push_back(b);
    tmpgoodtriplet.push_back(c);
    tmpgoodtriplet.push_back(Ri);
    tmpgoodtriplet.push_back(Xi);
    tmpgoodtriplet.push_back(Yi);
    tmpgoodtriplet.push_back(thism);
    tmpgoodtriplet.push_back(thisq);
    tmpgoodtriplet.push_back(thiss);
    tmpgoodtriplet.push_back(thisp);
    newgoodtriplet.push_back(tmpgoodtriplet);
  }
  *fgoodtriplet = newgoodtriplet;
  fNofTriplets = fgoodtriplet->size();



 if(fDisplayOn) {
   char goOnChar;
   fHb->Draw();
   display->Update();
   display->Modified();
   cin >> goOnChar;
 }
}


Bool_t PndSecondaryTrackFinder::FindNextPeak(double &maxvalue, TH1F *h) {

  double thresh = 30.;
//   double verymin = h->GetBinCenter(h->FindFirstBinAbove(thresh));
//   double verymax = h->GetBinCenter(h->FindLastBinAbove(thresh));
//   h->SetAxisRange(verymin, verymax);
                  
//   ha.Draw();


  // max
  int maxbin = h->GetMaximumBin();
  int max = (int) h->GetMaximum();
  if(max < thresh) return kFALSE;
  maxvalue = h->GetBinCenter(maxbin);
  if(maxbin == -1) return kFALSE;
  int stopbinup = -1, stopbindown = -1;
  for(int ibin = maxbin + 1; ibin < (maxbin + 100); ibin++) {
    if(max < thresh) {
      stopbinup = ibin;
      break;
    }
    // cout << max << " " <<  h->GetBinContent(ibin) << endl;
    if(h->GetBinContent(ibin) <= max) {
      max = (int) h->GetBinContent(ibin);
      h->SetBinContent(ibin, 0);
    }
    else {
      stopbinup = ibin;
      break;
    }
  }

  if(stopbinup == -1) return kFALSE;

  max = (int) h->GetMaximum();
  for(int ibin = maxbin - 1; ibin > (maxbin - 100); ibin--) {
    if(max < thresh)  {
      stopbindown = ibin;
      break;  
    }
    //  cout << max << " " <<  h->GetBinContent(ibin) << endl;
    if(h->GetBinContent(ibin) <= max) {
      max = (int) h->GetBinContent(ibin);
      h->SetBinContent(ibin, 0);
    }
    else {
      stopbindown = ibin;
      break;
    }
  }
  cout << "stop " << stopbindown << " " << stopbinup << endl;
  
  if(stopbindown == -1) return kFALSE;

  h->SetBinContent(maxbin, 0); 

  if(stopbinup - stopbindown < 3) return kFALSE;

//   TLine up(h->GetBinCenter(stopbinup), 0, h->GetBinCenter(stopbinup), 10000);
//   TLine down(h->GetBinCenter(stopbindown), 0, h->GetBinCenter(stopbindown), 10000);
//   if(fDisplayOn) {
//     char goOnChar;
//     cin >> goOnChar;
//     up.Draw("SAME");
//     down.Draw("SAME");
//     display->Update();
//     display->Modified();
//     cin >> goOnChar;
//   }  
  return kTRUE;
}

Bool_t PndSecondaryTrackFinder::FindNextRPeak(double &maxr) {
  return FindNextPeak(maxr, fHr);
}

Bool_t PndSecondaryTrackFinder::FindNextAPeak(double &maxa) {
  return FindNextPeak(maxa, fHa);
}

Bool_t PndSecondaryTrackFinder::FindNextBPeak(double &maxb) {
  return FindNextPeak(maxb, fHb);
}


Bool_t PndSecondaryTrackFinder::Test(Double_t x1, Double_t y1, Double_t r1, std::vector<int> *foundhits) {
  foundhits->clear();
  cout << "GOODTRIPLET SIZE " << fgoodtriplet->size() << endl;
  if(fgoodtriplet->size() == 0) return kFALSE;

  cout << "TRACK " << r1 << " " << x1 << " " << y1 << endl;
  for(int itr = 0; itr < fgoodtriplet->size(); itr++) {

    std::vector<double> thistriplet = fgoodtriplet->at(itr);
    int a = (int) thistriplet[0];
    int b = (int) thistriplet[1];
    int c = (int) thistriplet[2];
    double Ri = thistriplet[3];
    double Xi = thistriplet[4];
    double Yi = thistriplet[5];
    //    cout << "tripletta " << Ri << " " << Xi << " " << Yi << endl;
    if(fabs(Ri - r1) > fRLimit) {
      //  cout << "tripletta r " << Ri << " " << r1 << " " << fRLimit << endl;
      continue;
    }
    if(fabs(Xi - x1) > fXLimit) {
      //  cout << "tripletta x " << Xi << " " << x1 << " " << fXLimit << endl;
      continue;
    }
    if(fabs(Yi - y1) > fYLimit) {
      //  cout << "tripletta y " << Yi << " " << y1 << " " << fYLimit << endl;
      continue;
    }

    //     PndSttHit *hit1 = (PndSttHit*) fSttHitArray->At(a);
    //     double xc1 = hit1->GetX();
    //     double yc1 = hit1->GetY();
    //     double rc1 = hit1->GetIsochrone();
    //     PndSttHit *hit2 = (PndSttHit*) fSttHitArray->At(b);
    //     double xc2 = hit2->GetX();
    //     double yc2 = hit2->GetY();
    //     double rc2 = hit2->GetIsochrone();
    //     PndSttHit *hit3 = (PndSttHit*) fSttHitArray->At(c);
    //     double xc3 = hit3->GetX();
    //     double yc3 = hit3->GetY();
    //     double rc3 = hit3->GetIsochrone();

    //   double d1c = TMath::Sqrt((xc1 - Xi) * (xc1 - Xi) + (yc1 - Yi) * (yc1 - Yi));
    //     if(d1c > Ri && fabs(d1c - rc1 - Ri) > fLimit) continue;
    //     else if(d1c < Ri && fabs(d1c + rc1 - Ri) > fLimit) continue;
    
    //     double d2c = TMath::Sqrt((xc2 - Xi) * (xc2 - Xi) + (yc2 - Yi) * (yc2 - Yi));
    //     if(d2c > Ri && fabs(d2c - rc2 - Ri) > fLimit) continue;
    //     else if(d2c < Ri && fabs(d2c + rc2 - Ri) > fLimit) continue;
    
    //     double d3c = TMath::Sqrt((xc3 - Xi) * (xc3 - Xi) + (yc3 - Yi) * (yc3 - Yi));
    //     if(d3c > Ri && fabs(d3c - rc3 - Ri) > fLimit) continue;
    //     else if(d3c < Ri && fabs(d3c + rc3 - Ri) > fLimit) continue;

    std::vector<int>::iterator it;
    it = find(foundhits->begin(), foundhits->end(), a);
    if(it == foundhits->end()) {
      cout << "accepted " << a << endl;
      foundhits->push_back(a);
    }
    it = find(foundhits->begin(), foundhits->end(), b);
    if(it == foundhits->end()) {
      foundhits->push_back(b);
    cout << "accepted " << b << endl;

    }
    it = find(foundhits->begin(), foundhits->end(), c);
    if(it == foundhits->end()) {
      foundhits->push_back(c);
      cout << "accepted " << c << endl;
    }

  }
  if(foundhits->size() > 0) return kTRUE;
  return kFALSE;
}

void PndSecondaryTrackFinder::DeleteSttHits(std::vector<int> *foundhits) {
  
  for(int ihit = 0; ihit < foundhits->size(); ihit++) {
    int hitid = foundhits->at(ihit);
    hitid += (fNofMvdPixHits + fNofMvdStrHits);
    
    if(hitid != fFullList[hitid][0] || fFullList[hitid][1] == -1) cout << hitid << " is not correct " << endl;
    else { cout << hitid << " NOMORE USABLE" << endl; fFullList[hitid][3] = 0; }
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


void PndSecondaryTrackFinder::DrawScosZGeometry() {

  h2 = new TH2F("h2", "Z vs Scos", 100, 0, 80, 100, -40, 110);
  h2->GetXaxis()->SetTitle("Scos{#lambda}");
  h2->GetYaxis()->SetTitle("z");
  display->cd();
  h2->Draw();
  display->Update();
  display->Modified();  
  
}



void PndSecondaryTrackFinder::DrawHits() {
  TClonesArray* array;
  

 //  cout << "TOT HITS " << fNofHits << " as sum of " << fNofMvdPixHits << " " << fNofMvdStrHits << " " << fNofSttHits << endl;
//   for(int ihit = 0; ihit < fNofHits; ihit++) {
//     cout << fFullList[ihit][0] << " " << fFullList[ihit][3] << endl;
//  }



  for(int idet = 0; idet < 3; idet++) {
    if(idet == 0) array = fMvdPixelHitArray;
    else if(idet == 1) array = fMvdStripHitArray;
    else if(idet == 2) array = fSttHitArray;
    //  else if  array = fGemHitArray;
  
    int nhits = array->GetEntriesFast();

    for(int ihit = 0; ihit < nhits; ihit++) {
      FairHit *hit = (FairHit *) array->At(ihit);
      if(!hit) continue; 

      int hitidinfulllist = ihit;
      if(idet == 2) hitidinfulllist += (fNofMvdPixHits + fNofMvdStrHits);

      TVector3 position;
      hit->Position(position);
      TMarker *mrk = new TMarker(position.X(), position.Y(), 3);
      mrk->SetMarkerColor(idet + 1);
      //      if(fFullList[hitidinfulllist][3] == 0)  cout << ihit << " " << idet << endl;
      if(idet == 2) {
	Int_t tubeID = ((PndSttHit* ) hit)->GetTubeID();
	PndSttTube *tube = (PndSttTube* ) fTubeArray->At(tubeID);
	      
	TVector3 wireDirection = tube->GetWireDirection();
    
	if(wireDirection != TVector3(0., 0., 1.))  {
	  if(fFullList[hitidinfulllist][3] == 0) mrk->SetMarkerColor(kGray);
	  mrk->Draw("SAME");
	}
	else {

	  TArc *arc = new TArc(position.X(), position.Y(), tube->GetRadIn()); // ((PndSttHit* ) hit->GetIsochrone()));
	  if(fFullList[hitidinfulllist][3] == 0) arc->SetLineColor(kGray);
	  arc->SetFillStyle(0);
	  arc->Draw("SAME");

	}
      }
      else {
	if(idet == 0) mrk->SetMarkerStyle(21); 
	else if(idet == 1) mrk->SetMarkerStyle(25);
// 	if(fFullList[hitidinfulllist][3] == 0) {    // CHECK uncomment these lines
// 	  //  cout << ihit << " " << idet << endl;  // when in FillHitList the mvd
// 	  mrk->SetMarkerColor(kGray);               // will be not set to 0 by hand
// 	}
	mrk->Draw("SAME"); 
      }
    }
  }
  display->Update();
  display->Modified();  
}



void PndSecondaryTrackFinder::Refresh()
{
  // CHECK
  char goOnChar;
  cout << "Refresh?" << endl;
  //  cin >> goOnChar;
  cout << "GOING ON" << endl;
  DrawGeometry();
  DrawHits();
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

void PndSecondaryTrackFinder::FillTrack(Int_t itrk, Int_t nclusters) {

    if(fVerbose) cout << "COMPUTE PARAMETERS @ FIRST AND LAST" << endl;
    std::vector<double> parameters = fParList[itrk];   
    std::vector< std::pair<int, int> > track = fTrackList[itrk];

    TVector3 firstpos, firstmom, dfirstpos, dfirstmom; // CHECK error
    Bool_t first = ComputeFirstParameters(track, parameters, firstpos, firstmom);
    TVector3 lastpos, lastmom, dlastpos, dlastmom; // CHECK error
    Bool_t last = ComputeLastParameters(track, parameters, lastpos, lastmom);

 
//       cout << "1st/last parameters" << endl;
//       firstpos.Print();
//       firstmom.Print();
//      lastpos.Print();
//      lastmom.Print();
    
    if(fDisplayOn) {
      TLine* lfirst = new TLine(firstpos.X(), firstpos.Y(), firstpos.X() + 10 * firstmom.X(), firstpos.Y() + 10 * firstmom.Y());
      lfirst->Draw("SAME");
      TLine* llast = new TLine(lastpos.X(), lastpos.Y(), lastpos.X() + 10 * lastmom.X(), lastpos.Y() + 10 * lastmom.Y());
      llast->Draw("SAME");
    }
    
    int charge = (int) parameters[3];
    
    TVector3 dj(1, 0, 0), dk(0, 0, 1);   // CHECK
    dfirstpos.SetXYZ(2., 2., 10.);
    dlastpos.SetXYZ(2., 2., 10.);
    dfirstmom.SetXYZ(0.2, 0.2, 0.5);
    dlastmom.SetXYZ(0.2, 0.2, 0.5);

    
    FairTrackParP firstpar(firstpos, firstmom,
			   dfirstpos, dfirstmom, charge,
			   firstpos, dj, dk);
    
    FairTrackParP lastpar(lastpos, lastmom,
			  dlastpos, dlastmom, charge,
			  lastpos, dj, dk);
    
    TClonesArray& clref = *fSecondaryTrackCandArray;
    Int_t size = clref.GetEntriesFast();
    PndTrackCand *secCand = new(clref[size]) PndTrackCand();

    for(int ihit = 0; ihit < track.size(); ihit++) {
      std::pair<int, int> lpair = track[ihit];
      int detid = lpair.first;
      int hitid = lpair.second;
      secCand->AddHit(detid, hitid, ihit);
    }
    
    TClonesArray& clref2 = *fSecondaryTrackArray;
    PndTrack *secTrack = new(clref2[size]) PndTrack(firstpar, lastpar, *secCand);

    //    // flags
    //     if(fabs(par[0][2]) < 1.e-9) secTrack->SetFlag(-1); // -1 radius = 0
    //     if(fabs(par[0][4]) < 1.e-9) secTrack->SetFlag(-2); // -2 fitm = 0
    //     if(lastpos.Z() > 110.) secTrack->SetFlag(-3); // -3 z > 110
    //     if(lastpos.Z() < -40.) secTrack->SetFlag(-4); // -4 z < -40
    //     if(charge == 0) secTrack->SetFlag(-5); // -5 charge 0

    if(nclusters == 1) secTrack->SetFlag(0);
    else secTrack->SetFlag(1);

 

}


Bool_t PndSecondaryTrackFinder::ComputeFirstParameters(std::vector< std::pair<int, int> > cluster, std::vector<double> par, TVector3 &position, TVector3 &momentum) {
  return ComputeParametersAtHit(0, cluster, par, position, momentum) ;
}

Bool_t PndSecondaryTrackFinder::ComputeLastParameters(std::vector< std::pair<int, int> > cluster, std::vector<double> par, TVector3 &position, TVector3 &momentum) {
  return ComputeParametersAtHit(cluster.size() - 1, cluster, par, position, momentum) ;
}

Bool_t PndSecondaryTrackFinder::ComputeParametersAtHit(int ihit, std::vector< std::pair<int, int> > cluster, std::vector<double> par, TVector3 &position, TVector3 &momentum) {
  // cout << endl; cout << "ComputeParametersAtHit " << ihit << endl;
  std::pair<int, int> lpair = cluster[ihit];
  int detid = lpair.first;
  int hitid = lpair.second;
  
  Double_t xc = par[0];
  Double_t yc = par[1];
  Double_t radius = par[2];
  Int_t charge = (Int_t) TMath::Sign(1., par[3]);
  Double_t tanl = par[4];
  Double_t z0 = par[5];

  //  if(ihit == 0) cout << "PARAMETERS " << xc << " " << yc << " " << radius << " " << charge << " " << tanl << " " << z0 << endl;
  
  // CHECK ADD MVD !!!
  int hitidinfulllist = hitid;

  if(detid == FairRootManager::Instance()->GetBranchId(fSttBranch)) hitidinfulllist += (fNofMvdPixHits + fNofMvdStrHits);
  int iregion = fFullList[hitidinfulllist][2];
  
  //  cout << "HIT/hitid/detid/iregion " << ihit << " " << hitid << " " << detid << " " << iregion << endl;

  TVector3 pcatopoint;
  if(iregion != 4 && iregion != 5) // NON SKEWED TUBE
    {
      PndSttHit *hit = (PndSttHit*) fSttHitArray->At(hitid); 
      if(!hit) {
	cout << "CANNOT FIND THIS HIT " << detid << " " << hitid << endl;
	return kFALSE; 
      }
      pcatopoint.SetXYZ(hit->GetX(), hit->GetY(), 0.0);
    }
  else {
    cout << "THIS IS A SKEW TUBE" << endl;
    std::map< int, TVector3 >::iterator it = fskewintmap.find(lpair.second);
    pcatopoint = (*it).second;
    
  }
  
  TVector3 pca(-999, -999, -999);
  Bool_t findpca = FindXYpca(xc, yc, radius, pcatopoint.X(), pcatopoint.Y(), pca);

  if(findpca){

    Double_t d = TMath::Sqrt(xc * xc + yc * yc) - radius;
    Double_t phi =  TMath::ATan2(yc, xc);
    Double_t x0 = d * TMath::Cos(phi);
    Double_t y0 = d * TMath::Sin(phi);
    TVector2 v(x0 - xc, y0 - yc); 
    Double_t Phi0 = TMath::ATan2((y0 - yc),(x0 - xc));
    Double_t alpha = TMath::ATan2(pca.Y() - y0 + radius * TMath::Sin(Phi0), pca.X() - x0 + radius * TMath::Cos(Phi0));
    //     cout << "### " << alpha << endl;
    //     cout << pca.Y() << " " << y0 << " " << radius << endl;
    //     cout << pca.X() << " " << x0 << " " << radius << endl;
    TVector2 p(pca.X() - xc, pca.Y() - yc);
    Double_t Fi = CalculatePhi(v, p, alpha, Phi0, charge);
    //     Double_t Fi = CalculatePhi(v, p);
    //     Fi = BringPhiInto2Pi(Fi, charge);
    Double_t scos = - charge * radius * Fi; // scos = -q * R * phi CHECK :-)GOOD!
    double z = z0 + scos * tanl;
    //    cout << "scos " << scos << " " << radius << " " << Fi << " " << Phi0 << " " << alpha <<  " " << charge << endl;
    //     cout << x0 << " " << y0 << endl;

    position.SetXYZ(pca.X(), pca.Y(), z);

  }

  if(position.X() != -999) momentum = ComputeMomentumAtPos(xc, yc, radius, tanl, charge, position);
  return kTRUE; 

 
}

Bool_t PndSecondaryTrackFinder::FindXYpca(Double_t xc, Double_t yc, Double_t radius, Double_t x, Double_t y, TVector3 &xyz) {

       
  // [xp, yp] point = coordinates xy of the centre of the firing
      
  // the coordinates of the point are taken from the intersection
  // between the circumference from the drift time and the R radius of
  // curvature. -------------------------------------------------------
  // 2. find the intersection between the little circle and the line // R
  // 2.a
  // find the line passing throught [xc, yc] (centre of curvature) and [xp, yp] (first wire)
  // y = mx + q
  TVector3 point(x, y, 0.0);
  Double_t m = (point.Y() - yc)/(point.X() - xc);
  Double_t q = point.Y() - m*point.X();

  Double_t x1 = 0, y1 = 0,
    x2 = 0, y2 = 0,
    xb1 = 0, yb1 = 0,
    xb2 = 0, yb2 = 0;
      
  // CHECK the vertical track
  if(fabs(point.X() - xc) < 1e-6) {
	
    // 2.b
    // intersection circle and line --> [x1, y1]
    // + and - refer to the 2 possible intersections
    // +
    x1 = point.X();
    y1 = point.Y() + sqrt(radius * radius - (x1 - point.X()) * (x1 - point.X()));
    // - 
    x2 = x1;
    y2 = point.Y() - sqrt(radius * radius - (x2 - point.X()) * (x2 - point.X()));
	
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
    // intersection circle and line --> [x1, y1]
    // + and - refer to the 2 possible intersections
    // +

    if(((m*(q - point.Y()) - point.X())*(m*(q - point.Y()) - point.X()) - (m*m + 1)*((q - point.Y())*(q - point.Y()) + point.X()*point.X() - radius*radius)) < 0.) {  if(fVerbose > 0) cout << "IntersectionFinder round errors: " << radius << endl; 
      return false; }

    x1 = (-(m*(q - point.Y()) - point.X()) + sqrt((m*(q - point.Y()) - point.X())*(m*(q - point.Y()) - point.X()) - (m*m + 1)*((q - point.Y())*(q - point.Y()) + point.X()*point.X() - radius*radius))) / (m*m + 1);
    y1 = m*x1 + q;
    // - 
    x2 = (-(m*(q - point.Y()) - point.X()) - sqrt((m*(q - point.Y()) - point.X())*(m*(q - point.Y()) - point.X()) - (m*m + 1)*((q - point.Y())*(q - point.Y()) + point.X()*point.X() - radius*radius))) / (m*m + 1);
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
  if(distb1 > distb2) xyz.SetXYZ(xb2, yb2, 0.); 
  else xyz.SetXYZ(xb1, yb1, 0.); 

  /**
     if(fDisplayOn) {
     Refresh();
     TMarker *l1 = new TMarker(xyz.X(), xyz.Y(), 29);
     l1->SetMarkerSize(2.5);
     l1->SetMarkerColor(kYellow);
     l1->Draw("SAME");

     TMarker *l2 = new TMarker(xyb.X(), xyb.Y(), 29);
     l2->SetMarkerSize(2.5);
     l2->SetMarkerColor(kCyan);
     l2->Draw("SAME");



     display->Update();
     display->Modified();

     char goOnChar;
     cout << ", press any key" << endl;
     cin >> goOnChar;
     cout << "GOING ON" << endl;

     }
  **/

  return kTRUE;
}


TVector3 PndSecondaryTrackFinder::ComputeMomentumAtPos(Double_t xc, Double_t yc, Double_t radius, Double_t tanl, Int_t charge, TVector3 position) {

  TVector3 momentum(-999, -999, -999);
  TVector2 center(xc, yc);
  TVector2 pos(position.X(), position.Y());
  TVector2 myrad = center - pos;
//   cout << "INPUTP" << endl;
//   cout << position.X() << " " << position.Y() << " " << charge << " " << endl;
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
  rotx = - charge * myrad.Y();
  roty = charge * myrad.X();
//   cout << "xc, yc " << xc << " " << yc << endl;
//   cout << "rotx/y " << rotx << " " << roty << endl;
  Double_t pt = 0.006 * radius;
  Double_t pl = pt * tanl;
  Double_t ptot = TMath::Sqrt(pt * pt + pl * pl);

  momentum.SetX(rotx);
  momentum.SetY(roty);
  momentum.SetZ(0.);
  momentum.SetMag(pt);
  momentum.SetZ(pl);

//   cout << "##### mom " << endl;
//   cout << myrad.Mod() << " " << pt  << " " << pl << " " << ptot << endl;


  return momentum;


}

void PndSecondaryTrackFinder::FindLimits(std::vector< std::pair<int, int> > cluster, Mat2x3 &m) {

  double xmin = 42, ymin = 42, xmax = -42, ymax = -42;
  double dmin = 1000, dmax = -1;
  
  for(int ihit = 0; ihit < cluster.size(); ihit++) {
    std::pair<int, int> lpair = cluster[ihit];
    int detid = lpair.first;
    int hitid = lpair.second;
    PndSttHit *hit = (PndSttHit*) fSttHitArray->At(hitid);
    double x = hit->GetX();
    double y = hit->GetY();
    double d = TMath::Sqrt(x * x + y * y);
    if(fVerbose) cout << "xy " << x << " " << y << " " << d << endl;
    if(x < xmin) xmin = x; 
    if(y < ymin) ymin = y;
    if(x > xmax) xmax = x;
    if(y > ymax) ymax = y;
    if(d < dmin) dmin = d;
    if(d > dmax) dmax = d;
  }
  if(fVerbose) cout << "IN LIMITS " << xmin << " " << ymin << " " << dmin << " " << xmax << " " << ymax << " " << dmax << endl;
  m[0][0] = xmin;
  m[0][1] = ymin;
  m[0][2] = dmin;
  m[1][0] = xmax;
  m[1][1] = ymax;
  m[1][2] = dmax;

}


std::vector<int> PndSecondaryTrackFinder::ZFinderBIS(int iregion, Double_t xc, Double_t yc, Double_t radius, Double_t limits[2][3], std::vector< TVector3 > &intersections, std::vector< TVector3 > &intersections1, std::vector< TVector3 > &intersections2, Double_t &z0, Double_t &tanl, Bool_t &success) {

  std::vector<int> skewedcluster;
  TVector3 intersection(0., 0., 0.), int1(0., 0., 0.), int2(0., 0., 0.);
  std::vector< std::pair< TVector3, double > > int1scosl, int2scosl;

  int charge = (int) TMath::Sign(1., radius); // CHECK
  radius = fabs(radius);
  // x0 y0
  Double_t d = TMath::Sqrt(xc * xc + yc * yc) - radius;
  Double_t phi =  TMath::ATan2(yc, xc);
  
  Double_t x0 = d * TMath::Cos(phi);
  Double_t y0 = d * TMath::Sin(phi);
  TVector2 v(x0 - xc, y0 - yc); 
  Double_t Phi0 = TMath::ATan2((y0 - yc),(x0 - xc));
  int zcounter = 0;

  for(int ihit = fNofMvdPixHits + fNofMvdStrHits + 1; ihit < fNofHits; ihit++) {
    if(fFullList[ihit][3] == 0) continue;
    if(fFullList[ihit][2] != iregion) continue;

    int hitid = fFullList[ihit][0];

      
    Bool_t belong = DoesHitBelong(hitid, xc, yc, radius, limits, intersection, kFALSE);
    //    cout << "YES/NO BELONG" << belong << endl;
    //    intersection.Print();
    if(belong == kTRUE) {
      double errz = 0.;
      CalculateZ2(hitid, intersection.X(), intersection.Y(), xc, yc, radius, int1, int2, errz); // CHECK THIS ONE
      double scos1, scos2;
      for(int jhit = 0; jhit < 2; jhit++) 
      {
	TVector3 point = int1;
	if(jhit == 1) point  = int2;

	Double_t alpha = TMath::ATan2(point.Y() - y0 + radius * TMath::Sin(Phi0), point.X() - x0 + radius * TMath::Cos(Phi0));
	TVector2 p(point.X() - xc, point.Y() - yc);
	Double_t Fi = CalculatePhi(v, p, alpha, Phi0, charge);
	//     Double_t Fi = CalculatePhi(v, p);
	//     Fi = BringPhiInto2Pi(Fi, charge);
	Double_t scos = - charge * radius * Fi; // scos = -q * R * phi CHECK :-)GOOD!
	if(jhit == 0) scos1 = scos;
	else scos2 = scos;

	if(fDisplayOn) {
	  //    cout << "scos/z 0 : " << scos << " " << point.Z() << " " << errz << endl;
	  TLine *l = new TLine(scos, point.Z() - errz, scos, point.Z() + errz);
	  l->SetLineColor(4);
	  l->Draw("SAME");
	  //     cout << "scos/z I : " << scos << " " << zint1 << endl;
	  TMarker *mrk2 = new TMarker(scos, point.Z(), 21);
	  mrk2->SetMarkerColor(jhit + 2);
	  mrk2->Draw("SAME");
      
	  display->Update();
	  display->Modified();  
	}
      }

      skewedcluster.push_back(hitid);
      intersections.push_back(intersection);
      intersections1.push_back(int1);
      intersections2.push_back(int2);

      std::pair< TVector3, double > thispair1(int1, scos1);
      int1scosl.push_back(thispair1);
      std::pair< TVector3, double > thispair2(int2, scos2);
      int2scosl.push_back(thispair2);
    }
    
  }

  TObject* old = gDirectory->GetList()->FindObject("hhough");
  gDirectory->GetList()->Remove(old);
// hhough->Reset();
//  hhough = new TH2F("hhough", "", 80, -60, 60, 150, -40, 110);
  double xbins[91];
  int ibincounter = 0;
  for(int ibin = -90; ibin <= 90; ibin+=2) {
    double angle = (double) ibin;
    if(fabs(ibin) == 90) angle -= 0.5 * ibin/fabs(ibin);
    double tan = TMath::Tan(TMath::DegToRad() * angle);
    xbins[ibincounter] = tan;
    ibincounter++;
    /**  cout << ibincounter << " " << angle  << " " << tan << endl; **/
  }
  
  hhough = new TH2F("hhough", "", 90, xbins, 75, -40, 110);



  TVector3 point1, point2;
  double scos1, scos2;
  for(int iskhit = 0; iskhit < skewedcluster.size(); iskhit++) {
    for(int ich = 0; ich < 2; ich++) {
      std::pair< TVector3, double > thispair;
      if(ich == 0) thispair = int1scosl[iskhit];
      else thispair = int2scosl[iskhit];
      point1 = thispair.first;
      scos1 = thispair.second;

      for(int jskhit = iskhit + 1; jskhit < skewedcluster.size(); jskhit++) {

	for(int jch = 0; jch < 2; jch++) {
	  if(jch == 0) thispair = int1scosl[jskhit];
	  else thispair = int2scosl[jskhit];
	  point2 = thispair.first;
	  scos2 = thispair.second;

	  double slope = (point2.Z() - point1.Z()) / (scos2 - scos1);
	  double intercept = point2.Z() - slope * scos2;
	      
// 	  point1.Print();
// 	  point2.Print();

//        	  cout << "===>>> " << slope << " " << intercept << endl;
	  if(intercept < 110 && intercept > -40) hhough->Fill(slope, intercept);

	//   if(fDisplayOn) { 
// 	    TLine *line = new TLine(-150, -150 * slope + intercept, 150, 150 * slope + intercept);
// 	    line->SetLineColor(kOrange);
// 	    line->Draw("SAME");
// 	    display->Update();
// 	    display->Modified();  
	    
// 	    char goOnChar;
// 	    cout << "END OF Z FIT press any key" << endl;
// 	    cin >> goOnChar;
// 	    cout << "GOING ON" << endl;
	    
// 	  }
	  
	}
      }
    }
  }


// hhough->Draw("colz");
  int binx, biny, binz;
  hhough->GetMaximumBin(binx, biny, binz);
  double slopechoice = hhough->GetXaxis()->GetBinCenter(binx);
  double interceptchoice = hhough->GetYaxis()->GetBinCenter(biny);
  //  cout << "choice " << slopechoice << " " << interceptchoice << endl;
  if(fDisplayOn) { 
    TLine *line = new TLine(-150, -150 * slopechoice + interceptchoice, 150, 150 * slopechoice + interceptchoice);
    line->SetLineColor(kOrange);
       line->Draw("SAME");
    display->Update();
    display->Modified();  
    
    char goOnChar;
    cout << "END OF Z FIT press any key" << endl;
    cin >> goOnChar;
    cout << "GOING ON" << endl;
  }
  


  // recalculate z - s fit only from MVD
  Double_t Sxx, Sx, Sz, Sxz, S1z;
  Double_t Detz = 0.;
  
  Sx = 0.;
  Sz = 0.;
  Sxx = 0.;
  Sxz = 0.;
  S1z = 0.;

  std::vector< std::pair< TVector3, double > > intersectionsfinal;
  std::vector<int> skewedclusterfinal;

  for(int iskhit = 0; iskhit < skewedcluster.size(); iskhit++) {
    double tmpdist = 99999.;
    int tmprealint = -1;
    double tmpz = 0, tmpscos = 0, tmperr = 0;
    for(int ich = 0; ich < 2; ich++) {
      std::pair< TVector3, double > thispair;
      if(ich == 0) thispair = int1scosl[iskhit];
      else thispair = int2scosl[iskhit];
      TVector3 point = thispair.first;
      double scos = thispair.second;
      double fitz =  scos * slopechoice + interceptchoice;
      double thisdist = fabs(point.Z() - fitz);
      if(thisdist < tmpdist) {
	tmperr = 2./fabs(int1scosl[iskhit].first.Z() - int2scosl[iskhit].first.Z());
	tmpdist = thisdist;
	tmpscos = scos;
	tmpz = point.Z();
	tmprealint = ich;
      }
    }

    if(tmpdist > 5.) continue;

    int hitid = skewedcluster[iskhit];

    int hitidinfulllist = hitid + (fNofMvdPixHits + fNofMvdStrHits);
    fFullList[hitidinfulllist][3] = 0;

    skewedclusterfinal.push_back(hitid);
    std::pair< TVector3, double > thispair;
    if(tmprealint == 0) thispair = int1scosl[iskhit];
    else thispair = int2scosl[iskhit];
    intersectionsfinal.push_back(thispair);
    fskewintmap[hitid] = thispair.first;
    //    cout << "~~~> " << tmpdist << " " << tmperr << " " << tmpscos << " " << tmpz << endl;

    if(fDisplayOn) { 
      TArc *arc = new TArc(tmpscos, tmpz, 0.5);
      arc->SetFillStyle(0);

      arc->Draw("SAME");
      display->Update();
      display->Modified();  
    }

  
    Sx = Sx + (tmpscos /(tmperr * tmperr));
    Sz = Sz + (tmpz/(tmperr * tmperr));
    Sxz = Sxz + ((tmpscos * tmpz)/(tmperr * tmperr));
    Sxx = Sxx + ((tmpscos * tmpscos)/(tmperr * tmperr));
    S1z = S1z + 1/(tmperr * tmperr);
  
  }
  
  Detz = S1z*Sxx - Sx*Sx;
  if(Detz == 0) { 
    cout << "DET Z = 0" << endl; 
    success = kFALSE;
    return skewedclusterfinal;
  } // CHECK
  double  fitp = (1/Detz)*(Sxx*Sz - Sx*Sxz);
  double  fitm = (1/Detz)*(S1z*Sxz - Sx*Sz);
  if(fDisplayOn) { 
    TLine *line = new TLine(-150, -150 * fitm + fitp, 150, 150 * fitm + fitp);
    line->SetLineColor(kGreen);
    line->Draw("SAME");
    display->Update();
    display->Modified();  
    
     char goOnChar;
     cout << "END OF Z FIT press any key" << endl;
     cin >> goOnChar;
     cout << "GOING ON" << endl;
  }
  
  
  //   cout << "PR FIT : " << radius * 0.006 * slopechoice << endl;
  //   cout << "FIT    : " << radius * 0.006 * fitm << endl;
  
  if(fDisplayOn) { 
    char goOnChar;
    cout << "END OF Z FIT press any key" << endl;
    cin >> goOnChar;
    cout << "GOING ON" << endl;
  }
  
  tanl = fitm;
  z0 = fitp;
  

  // CHECK better not to use this!
  //  tanl = slopechoice;
  //  z0 = interceptchoice;
    
    //  cout << "CLUSTER FOUND WITH " << skewedcluster.size() << " HITS" << endl;
    return skewedclusterfinal;
}


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
  //  fDisplayOn = kTRUE;
  Bool_t belongs = DoesHitBelong(hitId, xc, yc, radius, intersection, draw);
  if(belongs == kFALSE) return belongs;

  if(fVerbose) cout << "LIMITS " << limits[0][0] << " " << limits[0][1] << " " << limits[0][2] << " " << limits[1][0] << " " << limits[1][1] << " " << limits[1][2] << endl;
  

  Bool_t ins = IsInsideLimits(intersection, limits);
  //  if(fVerbose) cout << "IS INSIDE LIM? " << ins << endl;

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

Bool_t PndSecondaryTrackFinder::IsInsideLimits(TVector3 intersection, Double_t limits[2][3]) {
  
  double dmin = limits[0][2];
  double dmax = limits[1][2];
  
  TVector2 first, last;
  /** NOT WORKING FINE
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
  **/
  // =========================
  // |   dmin    |   dmax    |
  // | xmin ymin | xmax ymax |
  // | xmin ymax | xmax ymin |
  // | xmax ymin | xmin ymax |
  // | xmax ymax | xmin ymin |

  if( fabs(TMath::Sqrt(limits[0][0] * limits[0][0] + limits[0][1] * limits[0][1]) - dmin) < 1 ||
      fabs(TMath::Sqrt(limits[1][0] * limits[1][0] + limits[1][1] * limits[1][1]) - dmax) < 1) {
    first = TVector2(limits[0][0], limits[0][1]);
    last = TVector2(limits[1][0], limits[1][1]);
  }
  else if( fabs(TMath::Sqrt(limits[0][0] * limits[0][0] + limits[1][1] * limits[1][1]) - dmin) < 1 ||
	   fabs(TMath::Sqrt(limits[1][0] * limits[1][0] + limits[0][1] * limits[0][1]) - dmax) < 1) {
    first = TVector2(limits[0][0], limits[1][1]);
    last = TVector2(limits[1][0], limits[0][1]);
  }
  else if( fabs(TMath::Sqrt(limits[1][0] * limits[1][0] + limits[0][1] * limits[0][1]) - dmin) < 1 ||
	   fabs(TMath::Sqrt(limits[0][0] * limits[0][0] + limits[1][1] * limits[1][1]) - dmax) < 1) {
    first = TVector2(limits[1][0], limits[0][1]);
    last = TVector2(limits[0][0], limits[1][1]);
  } 
  else if( fabs(TMath::Sqrt(limits[1][0] * limits[1][0] + limits[1][1] * limits[1][1]) - dmin) < 1 ||
	   fabs(TMath::Sqrt(limits[0][0] * limits[0][0] + limits[0][1] * limits[0][1]) - dmax) < 1) {
    first = TVector2(limits[1][0], limits[1][1]);
    last = TVector2(limits[0][0], limits[0][1]);
  }
  else {
    cout << "WHAT?? " << dmin << " " << dmax << endl;
    cout << "limit 0 " << limits[0][0] << " " << limits[0][1] << endl;
    cout << "limit 1 " << limits[1][0] << " " << limits[1][1] << endl;
    // take the most internal
    if(TMath::Sqrt(limits[0][0]* limits[0][0] + limits[0][1] * limits[0][1]) < TMath::Sqrt(limits[1][0] * limits[1][0] + limits[1][1] * limits[1][1])) {
      first = TVector2(limits[0][0], limits[0][1]);
      last = TVector2(limits[1][0], limits[1][1]);
    }
    else {
      last = TVector2(limits[0][0], limits[0][1]);
      first = TVector2(limits[1][0], limits[1][1]);
    }
  }
  
  double skewlimit     = 28.; // CHECK THIS LIMIT
  double skewthickness = 8.6; // CHECK THIS LIMIT
  
  if(dmin < skewlimit && dmax > skewlimit) {
    if(fVerbose) cout << "1: dmin " << dmin << " dmax " << dmax << endl;
    if((intersection.X() < limits[0][0] || intersection.X() > limits[1][0]) ||
       (intersection.Y() < limits[0][1] || intersection.Y() > limits[1][1])) {
      if(fVerbose) cout  << "OUTSIDE" << endl;
      return kFALSE;
    }
  }
  else if(dmax < skewlimit) {
    if(fVerbose) cout << "2: dmin " << dmin << " dmax " << dmax << endl;
    if(TMath::Sqrt((intersection.X() - last.X()) * (intersection.X() - last.X()) + (intersection.Y() - last.Y()) * (intersection.Y() - last.Y())) > skewthickness) {

      if(fVerbose) {
	cout  << "OUTSIDE 2" << TMath::Sqrt((intersection.X() - last.X()) * (intersection.X() - last.X()) + 
					    (intersection.Y() - last.Y()) * (intersection.Y() - last.Y())) << endl;
	cout << "intersection " << intersection.X() << " " << intersection.Y() << endl;
	cout << "first " << first.X() << " " << first.Y() << endl;
	cout << "last " << last.X() << " " << last.Y() << endl;
      }
      return kFALSE;
    }
  }
  else if( dmin > skewlimit) {
    if(fVerbose) cout << "3: dmin " << dmin << " dmax " << dmax << endl;
    if(TMath::Sqrt((intersection.X() - first.X()) * (intersection.X() - first.X()) + (intersection.Y() - first.Y()) * (intersection.Y() - first.Y())) > skewthickness) {

      if(fVerbose) {
 
	cout  << "OUTSIDE 2" << TMath::Sqrt((intersection.X() - first.X()) * (intersection.X() - first.X()) + 
					    (intersection.Y() - first.Y()) * (intersection.Y() - first.Y())) << endl;
	cout << "intersection " << intersection.X() << " " << intersection.Y() << endl;
	cout << "first " << first.X() << " " << first.Y() << endl;
	cout << "first " << first.X() << " " << first.Y() << endl;
      }
      return kFALSE;
    }
  }
  else cout << "WHAT NUMBER 2 ??" << endl;
  
  
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

//   Bool_t isinsidelim1 = IsInsideLimits(first, limits);
//   Bool_t isinsidelim2 = IsInsideLimits(second, limits);
//   if(!isinsidelim1 && !isinsidelim2) return kFALSE;


  if(fDisplayOn && draw) {
    cout << "1st " << first.X() << " " << first.Y() << endl;
    cout << "2nd " << second.X() << " " << second.Y() << endl;
    first.Print();
    second.Print();

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
    if(delta < 0) return kFALSE; 
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
    //  cout << "OUT OF BOUNDS" << endl;
    return kFALSE;
  }

  intersection.SetX(xint);
  intersection.SetY(yint);
  //  cout << "HEREEEEEEEEEEEEEEE" << endl;
  return kTRUE;
}

Bool_t PndSecondaryTrackFinder::ProcessCluster(std::vector<int> thiscluster, int izregion, std::vector< std::pair<int, int> > *lefthits, int nclusters)
{
  
  if(fDisplayOn) {
    LightSttCluster(thiscluster);
    char goOnChar;
    cout << "press any key" << endl;
    cin >> goOnChar;
    cout << "GOING ON" << endl;
  }
  int size2 = thiscluster.size();
    int last2 = thiscluster[size2 - 1];
    PndSttHit *hitlast2 = (PndSttHit*) fSttHitArray->At(last2);
    TVector3 pos2last(hitlast2->GetX(), hitlast2->GetY(), 0.0);
 
    std::vector< std::pair<int, int> > finaltrack;
    std::vector<double> finalparams;
    std::vector<int> finalcluster;
//    std::vector< std::pair<int, int> > lefthits;
    Double_t xc, yc, radius;
    Bool_t fullfit = FullFit(thiscluster, xc, yc, radius, &finalcluster, lefthits);
    if(fullfit == kTRUE) {
      finalparams.push_back(xc);
      finalparams.push_back(yc);
      finalparams.push_back(radius);
      
      for(int ihit = 0; ihit < finalcluster.size(); ihit++) {
	std::pair<int, int> thispair( FairRootManager::Instance()->GetBranchId(fSttBranch), finalcluster[ihit]);
	finaltrack.push_back(thispair);
	if(thispair.first == FairRootManager::Instance()->GetBranchId(fSttBranch)) { // CHECK
	  int hitid = thispair.second + fNofMvdPixHits + fNofMvdStrHits;
	  fFullList[hitid][3] = 0;
	}
	  
	//	  if(lefthits.size() > 3) 
	//	    clusterN = lefthits;
	
      }
      
      Int_t charge = FindCharge(finalparams[0], finalparams[1], finalcluster);
      //	cout << "CHARGE " << charge << endl;
      finalparams.push_back(charge);
      fTrackList.push_back(finaltrack);
      fParList.push_back(finalparams);

      if(fDisplayOn) {
	for(int ihit = 0; ihit < finaltrack.size(); ihit++) {
	  std::pair<int, int> thispair = finaltrack[ihit];
	    
	  PndSttHit *hit = (PndSttHit*) fSttHitArray->At(thispair.second);
	  //      cout << " " << thispair.second;   

	  TArc *arc = new TArc(hit->GetX(), hit->GetY(), hit->GetIsochrone());
	  arc->SetFillStyle(0);
	  arc->SetLineColor(3); // fColors[iclus]);
	  arc->Draw("SAME");
	  display->Update();
	  display->Modified();  
	  char goOnChar;
	  cout << "press any key" << endl;
	  cin >> goOnChar;
	  // 	char goOnChar;
	}
	cout << endl;
	TArc *barc = new TArc(finalparams[0], finalparams[1], finalparams[2]);
	barc->SetLineColor(5);
	barc->SetFillStyle(0);
	barc->Draw("SAME");
	display->Update();
	display->Modified();  
      }

	Mat2x3 limits;
	FindLimits(finaltrack, limits);

	std::vector<int> skewedcluster;
	std::vector< TVector3 > intersections, intersections1, intersections2;
	
	if(fDisplayOn) { 
	  char goOnChar;
	  cout << "Z FIT press any key" << endl;
	  cin >> goOnChar;
	  cout << "GOING ON" << endl;
	  DrawScosZGeometry();
	}
	  double tanl, z0;
	  bool success = kTRUE;
	  skewedcluster = ZFinderBIS(izregion, finalparams[0], finalparams[1], finalparams[3] * finalparams[2], limits, intersections,  intersections1, intersections2, z0, tanl, success); // CHECK
	  
	  if(success) {
	    // **********************************
	    for(int ihit = 0; ihit < skewedcluster.size(); ihit++) {
	      std::pair<int, int> thispair( FairRootManager::Instance()->GetBranchId(fSttBranch), skewedcluster[ihit]);
	      
	      finaltrack.push_back(thispair);
	    }
	    
	    std::vector< std::pair<int, int> > thisorderedcluster = OrderByDistanceFromRefPointWithoutCharge(finaltrack, TVector3(0,0, 0));
	    
	    
	    std::vector< std::pair<int, int> > oldthiscluster = *(fTrackList.end() - 1);
	    std::replace(fTrackList.begin(), fTrackList.end(), oldthiscluster, thisorderedcluster);
	    
	    finalparams.push_back(tanl);
	    finalparams.push_back(z0);
	    std::vector<double> oldthisparams = *(fParList.end() - 1);
	    std::replace(fParList.begin(), fParList.end(), oldthisparams, finalparams);
      
	    FillTrack(fParList.size() - 1, nclusters);
	    return kTRUE;
	  }
	  else {  // CHECK all else
	    std::vector< std::vector< std::pair<int, int> > >::iterator it1;
	    std::vector< std::vector<double> >::iterator it2;
	    std::vector< std::pair<int, int> > oldthiscluster = *(fTrackList.end() - 1);
	     std::vector<double>  oldthisparams = *(fParList.end() - 1);

	    for(int ihit = 0; ihit < oldthiscluster.size(); ihit++) {
	      std::pair<int, int> lpair = oldthiscluster[ihit];
	      int detid = lpair.first;
	      int hitid = lpair.second;
	      int hitidinfulllist = hitid + (fNofMvdPixHits + fNofMvdStrHits);
	      fFullList[hitidinfulllist][3] = 1;
	    }

	    for(int ihit = 0; ihit < skewedcluster.size(); ihit++) {
	      int hitid = skewedcluster[ihit];
	      int hitidinfulllist = hitid + (fNofMvdPixHits + fNofMvdStrHits);
	      fFullList[hitidinfulllist][3] = 1;
	    }
	    
	    it1 = find(fTrackList.begin(), fTrackList.end(), oldthiscluster);
	    fTrackList.erase(it1);
	    it2 = find(fParList.begin(), fParList.end(), oldthisparams);
	    fParList.erase(it2);

	  }
    }
    return kFALSE;
}






void PndSecondaryTrackFinder::RunListOfSingleClusters(std::vector< std::vector<int> > *clusterN, int iregion) {

  int izregion = -1;
  switch(iregion) {
  case 2:
  case 6: izregion = 4; break;
  case 3:
  case 7: izregion = 5; break;
  }

  for(int iclus = 0; iclus < clusterN->size(); iclus++) {
    std::vector<int> thiscluster = clusterN->at(iclus);
  
    int size = thiscluster.size();
    int last = thiscluster[size - 1];
    PndSttHit *hitlast = (PndSttHit*) fSttHitArray->At(last);
    TVector3 poslast(hitlast->GetX(), hitlast->GetY(), 0.0);

    std::vector< std::pair<int, int> > lefthits;
    Bool_t isfit = ProcessCluster(thiscluster, izregion, &lefthits, 1);
  
    if(isfit) {
      std::vector<int> left;
      for(int ileft = 0; ileft < lefthits.size(); ileft++) {
	std::pair<int, int> leftpair = lefthits.at(ileft);
	left.push_back(leftpair.first);
      }
      if(lefthits.size() > 3)  {
	replace(clusterN->begin(), clusterN->end(), thiscluster, left);
      }
      else {
	std::vector< std::vector<int> >::iterator it;
	it = find(clusterN->begin(), clusterN->end(), thiscluster);
	clusterN->erase(it);
      }
    }
  }
}
 
  
void PndSecondaryTrackFinder::RunListOfDoubleClusters(std::vector< std::vector<int> > *clusterA, std::vector< std::vector<int> > *clusterB, int iregionA, int  iregionB) {

  int izregion = -1;
  switch(iregionA) {
  case 2:
  case 6: izregion = 4; break;
  case 3:
  case 7: izregion = 5; break;
  }

  for(int iclus = 0; iclus < clusterA->size(); iclus++) {
    std::vector<int> thisclusterA = clusterA->at(iclus);
    if(thisclusterA.size() < 3 || thisclusterA.size() > 15) continue; // CHECK
    
    int sizeA = thisclusterA.size();
    int lastA = thisclusterA[sizeA - 1];
    PndSttHit *hitlastA = (PndSttHit*) fSttHitArray->At(lastA);
    TVector3 posAlast(hitlastA->GetX(), hitlastA->GetY(), 0.0);
    
    for(int jclus = 0; jclus < clusterB->size(); jclus++) {
      std::vector<int> thisclusterB = clusterB->at(jclus);
      if(thisclusterB.size() < 3) continue;
      
      int firstB = thisclusterB[0];
      PndSttHit *hitfirstB = (PndSttHit*) fSttHitArray->At(firstB);
      TVector3 posBfirst(hitfirstB->GetX(), hitfirstB->GetY(), 0.0);

      double distanceAB = (posAlast - posBfirst).Perp();
    
      if(distanceAB > 15.) continue;
      

      std::vector<int> fullcluster = thisclusterA;
      fullcluster.insert(fullcluster.end(), thisclusterB.begin(), thisclusterB.end());
 
      std::vector< std::pair<int, int> > lefthits;
      Bool_t isfit = ProcessCluster(fullcluster, izregion, &lefthits, 2);

      if(isfit) {
	std::vector<int> leftA, leftB;
	
	for(int ileft = 0; ileft < lefthits.size(); ileft++) {
	  std::pair<int, int> leftpair = lefthits.at(ileft);
	  if(leftpair.second == iregionB) leftB.push_back(leftpair.first);
	  else if(leftpair.second == iregionA) leftA.push_back(leftpair.first);
	  else cout << "ERROR HERE " << endl; // CHECK delete this line
	}
	if(leftB.size() > 3) replace(clusterB->begin(), clusterB->end(), thisclusterB, leftB);
	else {
	  std::vector< std::vector<int> >::iterator it;
	  it = find(clusterB->begin(), clusterB->end(), thisclusterB);
	  clusterB->erase(it);
	  
	}
	if(leftA.size() > 3) replace(clusterA->begin(), clusterA->end(), thisclusterA, leftA);
	else {
	  std::vector< std::vector<int> >::iterator it;
	  it = find(clusterA->begin(), clusterA->end(), thisclusterA);
	  clusterA->erase(it);
	  iclus--;
	}
	break;
      }
    }
  }
}  



ClassImp(PndSecondaryTrackFinder)

