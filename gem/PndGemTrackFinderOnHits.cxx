// -------------------------------------------------------------------------
// -----                PndGemTrackFinderOnHits source file             -----
// -----                Created 02.06.2009 by R. Karabowicz            -----
// -------------------------------------------------------------------------

#include "PndGemTrackFinderOnHits.h"

// Pnd includes
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairTrackParam.h"
#include "PndGemMCPoint.h"
#include "FairRootManager.h"
#include "PndDetectorList.h"
// ROOT includes
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TMatrixFSym.h"

// C++ includes
#include <iostream>
#include <map>
#include <cmath>
using std::cout;
using std::endl;
using std::map;

// -----   Default constructor   -------------------------------------------
PndGemTrackFinderOnHits::PndGemTrackFinderOnHits() {
  fMCTrackArray = NULL;
  fMCPointArray = NULL;
  fNofEvents    = 0;
}

// -----   Destructor   ----------------------------------------------------
PndGemTrackFinderOnHits::~PndGemTrackFinderOnHits() { }

// -----   Init  -----------------------------------------------------------
void PndGemTrackFinderOnHits::Init() {
  
  fNofRecoTracks = 0;
  fGoodRecoTracks = 0;
  fGhostRecoTracks = 0;
  fCloneRecoTracks = 0;

  // Get and check FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if( !ioman ) {
    cout << "-E- "<< GetName() <<"::Init: "
	 << "RootManager not instantised!" << endl;
    return;
  }
  
  // Get the pointer to the singleton FairRunAna object
  FairRunAna* ana = FairRunAna::Instance();
  if(NULL == ana) {
    cout << "-E- "<< GetName() <<"::Init :"
	 <<" no FairRunAna object!" << endl;
    return;
  }
  // Get the pointer to run-time data base
  FairRuntimeDb* rtdb = ana->GetRuntimeDb();
  if(NULL == rtdb) {
    cout << "-E- "<< GetName() <<"::Init :"
	 <<" no runtime database!" << endl;
    return;
  }
  
  // Get MCTrack array
  fMCTrackArray  = (TClonesArray*) ioman->ActivateBranch("MCTrack");
  if( !fMCTrackArray ) {
    cout << "-E- "<< GetName() <<"::Init: No MCTrack array!"
	 << endl;
    return;
  }
  
  // Get PndGemPoint (MCPoint) array
  fMCPointArray  = (TClonesArray*) ioman->GetObject("GEMPoint");
  if( !fMCPointArray ) {
    cout << "-E- "<< GetName() <<"::Init: No MCPoint array!"
	 << endl;
    return;
  }
  cout <<"fMCPointArray #: "<< fMCPointArray->GetEntriesFast() << endl;
  
  // Geometry loading
  //TFile *infile = ioman->GetInFile();
  //TGeoManager *geoMan = (TGeoManager*) infile->Get("FAIRGeom");
  //fGemStructure = PndGemStructure::Instance(geoMan);
  
  std::cout << "-I- "<< GetName() <<": Intialization successfull" << std::endl;
}

// -----   Public method DoFind   ------------------------------------------
Int_t PndGemTrackFinderOnHits::DoFind(TClonesArray* hitArray,
				      TClonesArray* trackArray) {
  
  // Count events
  fNofEvents++;
  cout << endl << endl<< endl << endl;
  cout << "=======================================================" << endl;
  cout << "-I-        Event No: " <<  fNofEvents << endl;
  cout << "=======================================================" << endl;
  
  
  cout <<"-I- "<< GetName() <<"::DoFind "<< endl;
  cout << "-------------------------------------------------------" << endl;
  cout << "     ### Start DoFind" << endl;
  cout << "-------------------------------------------------------" << endl;
  
  // Check pointers
  if( !fMCTrackArray ) {
    cout << "-E- PndGemTrackFinderOnHits::DoFind: "
	 << "MCTrack array missing! " << endl;
    return -1;
  }
  
  if( !fMCPointArray ) {
    cout << "-E- "<< GetName() <<"::DoFind: "
	 << "MCPoint array missing! " << endl;
    return -1;
  }
  
  if( !hitArray ) {
    cout << "-E- "<< GetName() <<"::DoFind: "
	 << "Hit arrays missing! "<< endl;
    return -1;
  }
  
  // Initialise control counters
  Int_t nNoMCTrack   = 0;
  Int_t nNoTrack     = 0;
  Int_t nNoGemPoint  = 0;
  Int_t nNoGemHit    = 0;
  
  // Create pointers to GemHit and GemPoint
  PndGemHit*   gemHit   = NULL;
  PndGemHit*   gemHit2   = NULL;
  FairMCPoint*  mcPoint  = NULL;
  PndMCTrack*  mcTrack  = NULL;
  PndGemTrack* gemTrack = NULL;

  // Declare variables outside the loop
  Int_t ptIndex = 0;       // MC point index
  Int_t mcTrackIndex = 0;  // MC track index
  Int_t trackIndex = 0;    // Gem track index
  Int_t relDetID = -1;//3000;   // 

  // Size of fMCTrackArray
  cout <<"# MC Tracks: "<< fMCTrackArray->GetEntriesFast() << endl;
  // Size of fMCTrackArray
  cout <<"# MC Points: "<< fMCPointArray->GetEntriesFast() << endl;
  
  // Number of Gem hits
  Int_t nGemHits = hitArray->GetEntriesFast();
  if(fVerbose > 2) cout <<"# GemHits: "<< nGemHits << endl;
  
  for(Int_t iHit = 0; iHit < nGemHits; iHit++){
    // Get the pointer to Gem hit
    gemHit = (PndGemHit*) hitArray->At(iHit);
    cout << "Hit " << iHit << " -> " << gemHit->GetX() << " " << gemHit->GetY() << " " << gemHit->GetZ() << " -> " << gemHit->GetRefIndex() << endl;
    gemHit->SetNDigiHits(-1);
  }
  
  for(Int_t iHit = 0; iHit < nGemHits; iHit++){
    // Get the pointer to Gem hit
    gemHit = (PndGemHit*) hitArray->At(iHit);
    Double_t x1 = gemHit->GetX();
    Double_t y1 = gemHit->GetY();
    for(Int_t iHit2 = 0; iHit2 < nGemHits; iHit2++){
      gemHit2 = (PndGemHit*) hitArray->At(iHit2);
      Int_t closestHit = -1;
      Double_t closestDist = 1000.;
      if ( gemHit2->GetZ() > gemHit->GetZ()+0.5 && gemHit2->GetZ() < gemHit->GetZ()+10. ) {
	Double_t x2 = gemHit2->GetX();
	Double_t y2 = gemHit2->GetY();
	
	Double_t distSq = (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2);
	if ( distSq > 2. ) continue;
	if ( closestDist > distSq ) {
	  closestDist = distSq;
	  closestHit  = iHit2;
	}
      }
      if ( closestHit != -1 ) {
	gemHit->SetNDigiHits(closestHit);
	gemHit2 = (PndGemHit*) hitArray->At(closestHit);
	gemHit2->SetNDigiHits(iHit);
	cout << "matching hits " << iHit << " and " << iHit2 << endl;
      }
    }
  } 

  Int_t nofTrackCand = 0;
  Int_t trackCandHitIndex[100][4];
  Int_t trackCandMCIndex [100];
  Double_t trackCandMom[100];
  Double_t trackCandPhi[100];
  Double_t trackCandThe[100];

  Double_t parThetaA =  0.66;
  Double_t parThetaB = -0.0576;
  Double_t parRadPhi = 1./(0.96*0.96);
  Double_t parRadStat2 = 120./90.;
  Double_t parStat1_2Dist = 90./(120.-90.);
  for(Int_t iHit = 0; iHit < nGemHits; iHit++){
    // Get the pointer to Gem hit
    gemHit = (PndGemHit*) hitArray->At(iHit);
    if ( gemHit->GetZ() > 90. ) continue;
    if ( gemHit->GetNDigiHits() < 0 ) continue;
    cout << "possible tracks starts here " << gemHit->GetX() << " " << gemHit->GetY() << " " << gemHit->GetZ() << endl;
    Double_t radius = TMath::Sqrt(gemHit->GetX()*gemHit->GetX()+gemHit->GetY()*gemHit->GetY());
    Double_t pangle = TMath::ACos(gemHit->GetX()/radius);
    if ( gemHit->GetY() < 0 )
      pangle = 2.*TMath::Pi() - pangle;
    Double_t theta = parThetaA * radius + parThetaB;
    cout << "     -> with theta of " << theta << " (radius = " << radius << " and phi angle = " << pangle*TMath::RadToDeg() << ")" << endl;
    for(Int_t iHit2 = 0; iHit2 < nGemHits; iHit2++){
      gemHit2 = (PndGemHit*) hitArray->At(iHit2);
      if ( TMath::Abs(gemHit2->GetZ()-119.38) > 0.3 ) continue;
      if ( gemHit2->GetNDigiHits() < 0 ) continue;
      cout << "trying to match it with " << gemHit2->GetX() << " " << gemHit2->GetY() << " " << gemHit2->GetZ() << endl;
      Double_t radius2 = TMath::Sqrt(gemHit2->GetX()*gemHit2->GetX()+gemHit2->GetY()*gemHit2->GetY());
      Double_t pangle2 = TMath::ACos(gemHit2->GetX()/radius2);
      if ( gemHit2->GetY() < 0 )
	pangle2 = 2.*TMath::Pi() - pangle2;

      if ( pangle < TMath::Pi()/2. && pangle2 > TMath::Pi()*3./2. ) pangle2 -= TMath::Pi()*2.;
      if ( pangle > TMath::Pi()*3./2. && pangle2 < TMath::Pi()/2. ) pangle2 += TMath::Pi()*2.;

      if ( TMath::Abs(pangle-pangle2)*TMath::RadToDeg() > 40 ) continue;

      cout << "       (radius = " << radius2 << " and phi angle = " << pangle2*TMath::RadToDeg() << ")" << endl;
      
      Double_t expectedRad = TMath::Sqrt((1-(pangle-pangle2)*(pangle-pangle2)*parRadPhi)*parRadStat2*parRadStat2*radius*radius);

      cout << " -> while expected radius was " << expectedRad << endl;
      if ( TMath::Abs(expectedRad-radius2) > 2. ) continue;
      cout << "STRONG CORRELATION FOR THIS HIT!!!" << endl;
      // calculate phi and momentum basing on the pangle-pangle2;
      //358.008 , 357.338 -> 8.0,  0.670
      //355.859 , 354.533 -> 4.0,  1.326
      //352.008 , 349.350 -> 2.0,  2.658
      //343.829 , 338.547 -> 1.0,  5.283
      //328.815 , 318.216 -> 0.5, 10.599
      Double_t trackMomentum = 5.3/((pangle-pangle2)*TMath::RadToDeg());
      cout << "calculated track momentum is " << trackMomentum << endl;
      Double_t trackPhiAngle = pangle+(pangle-pangle2)*parStat1_2Dist; 
      if ( trackPhiAngle < 0. ) trackPhiAngle += TMath::Pi()*2.;
      if ( trackPhiAngle > TMath::Pi()*2. ) trackPhiAngle -= TMath::Pi()*2.;
      cout << "calculated phi is " << trackPhiAngle*TMath::RadToDeg() << endl;

      trackCandHitIndex[nofTrackCand][0] = iHit;
      trackCandHitIndex[nofTrackCand][1] = gemHit->GetNDigiHits();
      trackCandHitIndex[nofTrackCand][2] = iHit2;
      trackCandHitIndex[nofTrackCand][3] = gemHit2->GetNDigiHits();
      trackCandMom[nofTrackCand] = trackMomentum;
      trackCandPhi[nofTrackCand] = trackPhiAngle*TMath::RadToDeg();
      trackCandThe[nofTrackCand] = theta;

      nofTrackCand++;
    }
  }

  //if ( nofTrackCand != 1 ) cout << "SOMETHING WRONG, " << nofTrackCand << " track candidates" << endl;
  cout << "===>>> " << nofTrackCand << " track candidates" << endl;

  Int_t goodRecoTracks  = 0;
  Int_t ghostRecoTracks = 0;
  Int_t cloneRecoTracks = 0;

  for ( Int_t itrc = 0 ; itrc < nofTrackCand ; itrc++ ) {
    cout << "track " << itrc << ": " << flush;
    for ( Int_t ihit = 0 ; ihit < 4 ; ihit++ ) {
      if ( trackCandHitIndex[itrc][ihit] == -1 ) continue;
      gemHit = (PndGemHit*) hitArray->At(trackCandHitIndex[itrc][ihit]);
      mcPoint = (FairMCPoint*) fMCPointArray->At(gemHit->GetRefIndex());
      cout << mcPoint->GetTrackID() << " " << flush;
    }
    cout << endl;
  }

  for ( Int_t itrc = 0 ; itrc < nofTrackCand ; itrc++ ) {
    trackCandMCIndex[itrc] = -1;

    Int_t mcTrackIndex = -1;
    for ( Int_t ihit = 0 ; ihit < 4 ; ihit++ ) {
      cout << "trackCandHitIndex = " << trackCandHitIndex[itrc][ihit] << endl;
      if ( trackCandHitIndex[itrc][ihit] == -1 ) continue;
      gemHit = (PndGemHit*) hitArray->At(trackCandHitIndex[itrc][ihit]);
      mcPoint = (FairMCPoint*) fMCPointArray->At(gemHit->GetRefIndex());
      cout << "hit " << ihit << " track mc = " << mcPoint->GetTrackID() << endl;
      if ( !mcPoint ) { cout << "no mc point" << endl; continue;}
      if ( trackCandMCIndex[itrc] == -1 ) trackCandMCIndex[itrc] = mcPoint->GetTrackID();
      if ( trackCandMCIndex[itrc] != mcPoint->GetTrackID() ) {
	cout << "breaking cause " << trackCandMCIndex[itrc] << " != " << mcPoint->GetTrackID() << endl; 
	trackCandMCIndex[itrc] = -666;
	break;
      }
      cout << "track with mc " << trackCandMCIndex[itrc] << endl;
    }

    if ( trackCandMCIndex[itrc] < 0 )
      ghostRecoTracks ++;
    else
      goodRecoTracks ++;
  }

  for ( Int_t itrc = 0 ; itrc < nofTrackCand ; itrc++ ) {
    cout << itrc << " --> " << trackCandMCIndex[itrc] << endl;
    if ( trackCandMCIndex[itrc] < 0 ) continue;
    for ( Int_t itrc2 = itrc+1 ; itrc2 < nofTrackCand ; itrc2++ ) {
      if ( trackCandMCIndex[itrc] == trackCandMCIndex[itrc2] ) {
	goodRecoTracks--;
	cloneRecoTracks++;
      }
    }
  }

  fNofRecoTracks += nofTrackCand;
  fGoodRecoTracks += goodRecoTracks;
  fGhostRecoTracks += ghostRecoTracks;
  fCloneRecoTracks += cloneRecoTracks;

  for ( Int_t itrc = 0 ; itrc < nofTrackCand ; itrc++ ) {
    if ( trackCandMCIndex[itrc] < 0 ) continue;
    mcTrack = (PndMCTrack*) fMCTrackArray->At(trackCandMCIndex[itrc]);
    TVector3 mcmom = mcTrack->GetMomentum();
    cout << "mc:   " 
	 << mcmom.Mag() << " " 
	 << TMath::RadToDeg()*mcmom.Phi() << " "
	 << TMath::RadToDeg()*mcmom.Theta() << endl;
    cout << "reco: " 
	 << trackCandMom[itrc] << " " 
	 << trackCandPhi[itrc] << " "
	 << trackCandThe[itrc] << endl;
  }

 cout << "in this event: " << nofTrackCand << " tracks:" << endl;
  cout << goodRecoTracks << " good tracks" << endl;
  cout << ghostRecoTracks << " ghost tracks" << endl;
  cout << cloneRecoTracks << " clone tracks" << endl;

  cout << "------------------------------------------------" << endl;
 cout << "ALTOGETHER: " << fNofRecoTracks << " tracks:" << endl;
  cout << fGoodRecoTracks << " good tracks" << endl;
  cout << fGhostRecoTracks << " ghost tracks" << endl;
  cout << fCloneRecoTracks << " clone tracks" << endl;
  cout << "------------------------------------------------" << endl;

  return 0;
}


ClassImp(PndGemTrackFinderOnHits)
