//* $Id: */

// -------------------------------------------------------------------------
// -----                    PndBarrelTrackFinder source file                 -----
// -----                  Created 12/02/2009 by R. Karabowicz          -----
// -------------------------------------------------------------------------

// Includes from GEM
#include "PndBarrelTrackFinder.h"

// Includes from base
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLink.h"

// Includes from ROOT
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TMath.h"
#include "TGeoManager.h"
#include "TGeoNode.h"
#include "TRandom.h"

#include "PndDetectorList.h"
#include "PndGemHit.h"
#include "PndSdsHit.h"
#include "PndSttHit.h"
#include "PndSttMapCreator.h"
#include "PndSttTube.h"

#include <iostream>
#include <iomanip>
#include <map>

using std::cout;
using std::cerr;
using std::flush;
using std::endl;
using std::pair;
using std::setw;
using std::left;
using std::right;
using std::fixed;
using std::setprecision;
using std::map;



// -----   Default constructor   ------------------------------------------
PndBarrelTrackFinder::PndBarrelTrackFinder() : FairTask("Barrel Track Finder", 1) {
  for ( Int_t idet = 0 ; idet < 4 ; idet++ ) {
    fIncludeDet[idet] = kFALSE;
    fHitArray  [idet] = NULL;
  }
  fTNofEvents    = 0;
  fTNofTracks    = 0;
  fTracksVector.clear();
  Reset();
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndBarrelTrackFinder::PndBarrelTrackFinder(Int_t iVerbose) 
  : FairTask("Barrel Track Finder", iVerbose) { 
  for ( Int_t idet = 0 ; idet < 4 ; idet++ ) {
    fIncludeDet[idet] = kFALSE;
    fHitArray  [idet] = NULL;
  }
  fTNofEvents    = 0;
  fTNofTracks    = 0;
  fTracksVector.clear();
  Reset();
}
// -------------------------------------------------------------------------



// -----   Constructor with name   -----------------------------------------
PndBarrelTrackFinder::PndBarrelTrackFinder(const char* name, Int_t iVerbose) 
  : FairTask(name, iVerbose) { 
  for ( Int_t idet = 0 ; idet < 4 ; idet++ ) {
    fIncludeDet[idet] = kFALSE;
    fHitArray  [idet] = NULL;
  }
  fTNofEvents    = 0;
  fTNofTracks    = 0;
  fTracksVector.clear();
  Reset();
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndBarrelTrackFinder::~PndBarrelTrackFinder() { 
  Reset();
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void PndBarrelTrackFinder::UseMvdSttTpcGem(const Bool_t useMvd, const Bool_t useStt, const Bool_t useTpc, const Bool_t useGem) {
  fIncludeDet[0] = useMvd;
  fIncludeDet[1] = useMvd;
  fIncludeDet[2] = useStt;
  fIncludeDet[3] = useTpc;
  fIncludeDet[4] = useGem;
};
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndBarrelTrackFinder::Exec(Option_t* opt) {
  Reset();

  if ( fVerbose > 0 ) 
    cout << "=============== EVENT " << fTNofEvents << " =================" << endl;

  Int_t nofHits = 0;
  Int_t firstDH[5];
  
  Double_t sH1[3] = {0.,0.,0.};
  Double_t sH2[3] = {0.,0.,0.};
  Double_t sH3[3] = {0.,0.,0.};
  Double_t sH4[3] = {0.,0.,0.};
  Double_t circPar[3] = {0.,0.,0.};
      
  std::vector<Int_t>    hitDetId;
  std::vector<Int_t>    hitDetNo;
  std::vector<FairHit*> hitVector;
  std::vector<Int_t>    hitVectDI;
  std::vector<Int_t>    hitVectHN;

  Int_t INTERESTINGTRACK = -1;
  // putting hits into one common array
  for ( Int_t idet = 0 ; idet < 5 ; idet++ ) {
    firstDH[idet] = nofHits;
    if ( fIncludeDet[idet] == kTRUE ) {
      nofHits += fHitArray[idet]->GetEntriesFast();
      //      cout << fDetName[idet].Data() << " hits from " << firstDH[idet] << " to " << nofHits << "   ( " << fHitArray[idet]->GetEntriesFast() << " hits )" << endl;
      for ( Int_t ihit = firstDH[idet] ; ihit < nofHits ; ihit++ ) { // hit mixing could go here
	hitDetId.push_back(idet);
	hitDetNo.push_back(ihit-firstDH[idet]);
      }
    }
  }
  std::vector<Bool_t> hitUsed(hitDetId.size(),kFALSE);

  FairHit* detHit;
  
  Int_t hitN = 0;
  // looping over hits as they are in the common array
  for ( Int_t ihit = 0 ; ihit < nofHits ; ihit++ ) {

    hitN = (hitN+gRandom->Integer(nofHits))%nofHits;
    while ( 1==1 ) {
      if ( hitUsed[hitN] == kFALSE ) break;
      hitN = (hitN+1)%nofHits;
    }
    hitUsed[hitN] = kTRUE;
    //    cout << " for hit " << ihit << " would like to take hit " << hitN << " >> " << hitDetId[hitN] << "." << hitDetNo[hitN] << endl;

    //    hitN = (firstDH[2]+ihit)%nofHits; // hit mixing could go here

    if ( hitDetId[hitN] == 0 || hitDetId[hitN] == 1 )
      detHit = (PndSdsHit*)fHitArray[hitDetId[hitN]]->At(hitDetNo[hitN]);
    else                   if ( hitDetId[hitN] == 2 )
      detHit = (PndSttHit*)fHitArray[hitDetId[hitN]]->At(hitDetNo[hitN]);
    else                   if ( hitDetId[hitN] == 4 )
      detHit = (PndGemHit*)fHitArray[hitDetId[hitN]]->At(hitDetNo[hitN]);
    
    Double_t resDist = 0.2;
    Double_t resP_ZD = 0.0005;

    Double_t maxDist = 0.01;//05;
//     if ( hitDetId[hitN] <= 1 ) maxDist = 0.15; // MVD
//     if ( hitDetId[hitN] == 4 ) maxDist = 0.1;  // GEM
        
    Bool_t skewedSttHit = kFALSE;
    Int_t  iTube      = -1;
    if ( hitDetId[hitN] == 2 ) {
      iTube = ((PndSttHit*)detHit)->GetTubeID(); 
      PndSttTube *sttTube = (PndSttTube*) fTubeArray->At(iTube);
      //      cout << "tube " << iTube << " wiredir = " << sttTube->GetWireDirection().X() << " " << sttTube->GetWireDirection().Y() << " " << sttTube->GetWireDirection().Z() << endl;
      if ( sttTube->GetWireDirection().Z() < 1. ) {
	skewedSttHit = kTRUE;
      }
    }

    sH1[0] = detHit->GetX();
    sH1[1] = detHit->GetY();
    sH1[2] = 0.;
    if ( iTube != -1 ) // this is a STT hit
      sH1[2] = ((PndSttHit*)detHit)->GetIsochrone();
    
    if ( fVerbose > 3 ) {
      cout << "--------------------------------------------------" << endl;
      cout << ihit << "." << hitN << " > " << fDetName[hitDetId[hitN]].Data() << " hit " << hitDetNo[hitN] << " position: " 
	   << detHit->GetX() << " " << detHit->GetY() << " " << detHit->GetZ() 
	   << "    error " << detHit->GetDx() << " " << detHit->GetDy() << " " << detHit->GetDz() << " >>> " << sH1[0] << " " << sH1[1] << " " << sH1[2] << endl;
    }

    Double_t phi1 = CalcPhi(detHit->GetX(),detHit->GetY());
    
    // loop over known tracks to check if the hit belongs to one
    Bool_t hitBelongsToAnyTrack = kFALSE;
    Int_t nTracks = fTracksVector.size();
    Int_t nPars = 0;
    for ( Int_t itr = 0 ; itr < nTracks ; itr++ ) { 
      nPars += fTracksVector[itr].trackPars.size();
    }
    //    cout << "there are " << nTracks << " tracks and " << nPars << " parameters" << endl;

    if ( skewedSttHit == kFALSE ) {
      for ( Int_t itr = 0 ; itr < nTracks ; itr++ ) { 
	Bool_t hitBelongsToTrack = kFALSE;
      
	Double_t meanPhi = 0.;
	Double_t meanRad = 0.;
	Double_t variPhi = 0.;
	Double_t variRad = 0.;
	Double_t goodPNR = 0.;

 	Double_t phiT = CalcPhi(fTracksVector[itr].trackHits[0]->GetX(),fTracksVector[itr].trackHits[0]->GetY());
	Double_t phi1TDiffr = TMath::Abs(phi1-phiT);
	if ( phi1TDiffr > TMath::Pi()/4. && phi1TDiffr < TMath::Pi()*7./4. ) continue;

	for ( Int_t ipar = 0 ; ipar < fTracksVector[itr].trackPars.size() ; ipar++ ) {
	  if ( fTracksVector[itr].trackPars[ipar].n < 0.9*fTracksVector[itr].trackHits.size() ) continue;
	  
	  circPar[0] = fTracksVector[itr].trackPars[ipar].x;
	  circPar[1] = fTracksVector[itr].trackPars[ipar].y;
	  circPar[2] = fTracksVector[itr].trackPars[ipar].r;
	  
	  if ( fVerbose > 4 ) {
	    //ihit == 82 || ihit == 90 || ihit == 97 || ihit == 112 || ihit == 119 || ihit == 120 || ihit == 128 || ihit == 138 ) {
	    cout << " ..... to parameter " << CalcPhi(circPar[0],circPar[1]) << " " << circPar[2] << ": distance is " << FindCircDist(sH1,circPar) << flush;
	    if ( sH1[2] < 1.e-5 ) 
	      cout << "   new p_z is " << CalcP_Z(circPar,sH1[0],sH1[1],detHit->GetZ()) << " vs " << fTracksVector[itr].trackPars[ipar].p_z << " old" << flush;
	    cout << endl;
	  }

	  if ( FindCircDist(sH1,circPar) < maxDist ||
	       ( sH1[2] < 1.e-5 && FindCircDist(sH1,circPar) < resDist &&
		 TMath::Abs(CalcP_Z(circPar,sH1[0],sH1[1],detHit->GetZ())-fTracksVector[itr].trackPars[ipar].p_z) < resP_ZD ) ) { 
	    hitBelongsToTrack = kTRUE;

	    meanPhi += CalcPhi(circPar[0],circPar[1]);
	    meanRad += circPar[2];
	    variPhi += CalcPhi(circPar[0],circPar[1])*CalcPhi(circPar[0],circPar[1]);
	    variRad += circPar[2]*circPar[2];
	    goodPNR += 1.;
	    
	    fTracksVector[itr].trackPars[ipar].n += 1; // because another hit is close to this parameter

	    if ( fVerbose > 4 ) 
	      cout << " par: " << CalcPhi(circPar[0],circPar[1]) << " | " << circPar[2] << " of track " << itr << " matches to this hit with dist " << FindCircDist(sH1,circPar) << endl;
	  }	    
	}
	if ( !hitBelongsToTrack ) continue;

	// we should put the hit in the track
	// but I want to check, if the trackPars created using this hit and all other track hits are good enough...
	for ( Int_t ihitP = 0 ; ihitP < fTracksVector[itr].trackHitD.size() ; ihitP++ ) {
	  sH2[0] = fTracksVector[itr].trackHits[ihitP]->GetX();
	  sH2[1] = fTracksVector[itr].trackHits[ihitP]->GetY();
	  sH2[2] = 0.;
	  if ( fTracksVector[itr].trackHitD[ihitP] == 2 )
	    sH2[2] = ((PndSttHit*)fTracksVector[itr].trackHits[ihitP])->GetIsochrone();
	  
	  Int_t stepCno = 1; 
	  if ( sH1[2] < 1.e-5 ) stepCno  = 2;
	  if ( sH2[2] < 1.e-5 ) stepCno += 2; 
	  if ( fVerbose > 5 ) 
	    cout << "stepCno = " << stepCno << " cause " << sH1[2] << " and " << sH2[2] << endl;

	  for ( Int_t icirc = 0 ; icirc < 4 ; icirc += stepCno ) {
	    if ( FindCircPar(sH1,sH2,sH3,icirc,circPar) ) {
	      if ( circPar[2] < 10. || circPar[2] > 100000. ) continue;
	      //	      cout << " phi: " << meanPhi << " vs " << CalcPhi(circPar[0],circPar[1]) 
	      //		   << " rad: " << meanRad << " vs " << circPar[2] << endl;
	      Double_t newParP_Z = 0.;	  
	      if      ( sH1[2] < 1.e-5 ) newParP_Z = CalcP_Z(circPar,sH1[0],sH1[1],detHit->GetZ());
	      else if ( sH2[2] < 1.e-5 ) newParP_Z = CalcP_Z(circPar,sH2[0],sH2[1],fTracksVector[itr].trackHits[ihitP]->GetZ());

	      if ( fVerbose > 5 ) //itr == INTERESTINGTRACK ) 
		cout << "should compare (" << CalcPhi(circPar[0],circPar[1]) << " " << circPar[2] << ") with " 
		     << " (" << meanPhi << "+-" << variPhi << " " << meanRad << "+-" << variRad << ")" << endl;
// 	      if ( goodPNR > 4 &&
// 		   ( TMath::Abs(meanPhi - CalcPhi(circPar[0],circPar[1])) > 0.05 ||
// 		     TMath::Abs(meanRad - circPar[2])                     > 0.2 *meanRad+10. ) )
// 		continue;
	      

	      Int_t nofCloseHits = 2;
	      for ( Int_t ihitF = 0 ; ihitF < fTracksVector[itr].trackHitD.size() ; ihitF++ ) {
		if ( ihitF == ihitP ) continue;
		sH4[0] = fTracksVector[itr].trackHits[ihitF]->GetX();
		sH4[1] = fTracksVector[itr].trackHits[ihitF]->GetY();
		sH4[2] = 0.;
		if ( fTracksVector[itr].trackHitD[ihitF] == 2 )
		  sH4[2] = ((PndSttHit*)fTracksVector[itr].trackHits[ihitF])->GetIsochrone();

		Double_t maxDistComp = 0.01;
// 		Double_t maxDistComp = 0.1;
// 		if ( fTracksVector[itr].trackHitD[ihitF] <= 1 ) maxDistComp = 0.15; // MVD
// 		if ( fTracksVector[itr].trackHitD[ihitF] == 4 ) maxDistComp = 0.1;  // GEM

		if ( FindCircDist(sH4,circPar) < maxDist ||
		     ( TMath::Abs(newParP_Z) > 1.e-6 && FindCircDist(sH4,circPar) < resDist &&
		       TMath::Abs(CalcP_Z(circPar,sH4[0],sH4[1],fTracksVector[itr].trackHits[ihitF]->GetZ())-newParP_Z) < resP_ZD ) ) { 
		  
		  //		if ( FindCircDist(sH4,circPar) < maxDistComp ) 
		  nofCloseHits += 1;
		}
	      }

	      if ( fVerbose > 5 ) //( itr == INTERESTINGTRACK ) 
		cout << "there are " << nofCloseHits << " with limit set to " << 0.6*fTracksVector[itr].trackHits.size() << " (0.6*" << fTracksVector[itr].trackHits.size() << ")" << endl;
	      if ( nofCloseHits < 0.6*fTracksVector[itr].trackHits.size() ) continue; // the track param has not enough close hits

	      TrackParameter tp0;
	      tp0.x = circPar[0];
	      tp0.y = circPar[1];
	      tp0.r = circPar[2];
	      tp0.p_z = 0.;
	      // temporary recognition of non-STT hit
	      // should take only parameters created using hit with VALID z information
	      if      ( sH1[2] < 1.e-5 ) tp0.p_z = CalcP_Z(circPar,sH1[0],sH1[1],detHit->GetZ());
	      else if ( sH2[2] < 1.e-5 ) tp0.p_z = CalcP_Z(circPar,sH2[0],sH2[1],fTracksVector[itr].trackHits[ihitP]->GetZ());
	      tp0.n = nofCloseHits;

	      fTracksVector[itr].trackPars.push_back(tp0);

	      hitBelongsToTrack = kTRUE;
	    }
	  }
	}
	if ( fVerbose > 5 ) {//itr == INTERESTINGTRACK || fTracksVector[itr].trackHits.size() == 2 ) {//hitDetId[hitN] == 4 ) { //2 && hitDetNo[hitN] == 52 ) {
	  cout << "  (" << fTracksVector[itr].trackHits[0]->GetX() << "," << fTracksVector[itr].trackHits[0]->GetY() << ") +"
	       << "  (" << fTracksVector[itr].trackHits[1]->GetX() << "," << fTracksVector[itr].trackHits[1]->GetY() << ") +"
	       << "? (" << sH1[0] << "," << sH1[1] << ")" << endl;
	  cout << "HIT DOES " << (hitBelongsToTrack?"":"NOT ") << "BELONG TO TRACK " << itr << " WITH (pars " << goodPNR << " of " << ") ("
	       << fTracksVector[itr].trackHits.size() << " hits, " << fTracksVector[itr].trackPars.size() << " pars) phi,rad,n: " << endl;
	  for ( Int_t ipar = 0 ; ipar < fTracksVector[itr].trackPars.size() ; ipar++ ) 
	    cout << CalcPhi(fTracksVector[itr].trackPars[ipar].x,fTracksVector[itr].trackPars[ipar].y) << " " 
		 << fTracksVector[itr].trackPars[ipar].r << " "
		 << fTracksVector[itr].trackPars[ipar].n << "  |  " << flush;
	  cout << endl;
	}
	
	if ( !hitBelongsToTrack ) continue;
	fTracksVector[itr].trackHits.push_back(detHit);
	fTracksVector[itr].trackHitD.push_back(hitDetId[hitN]);
	fTracksVector[itr].trackHitN.push_back(hitDetNo[hitN]);
	hitBelongsToAnyTrack = kTRUE;
      }
      if ( hitBelongsToAnyTrack ) continue;
    }
    
    // skewed STT hit can not be used for track circle finding
    if ( skewedSttHit == kTRUE ) {
 
      PndSttTube *sttTube = (PndSttTube*) fTubeArray->At(iTube);
      
      Double_t ra = TMath::ATan(TMath::Sqrt(sttTube->GetWireDirection().X()*sttTube->GetWireDirection().X()+
					    sttTube->GetWireDirection().Y()*sttTube->GetWireDirection().Y())/
				sttTube->GetWireDirection().Z());
      Double_t a  = 0.5*((PndSttHit*)detHit)->GetIsochrone()*(1.+1./TMath::Cos(ra));
      
      Double_t tubePar[8];
      tubePar[0] = sttTube->GetPosition().X();
      tubePar[1] = sttTube->GetPosition().Y();
      tubePar[2] = sttTube->GetPosition().Z();
      tubePar[3] = sttTube->GetHalfLength();
      tubePar[4] = sttTube->GetWireDirection().X();
      tubePar[5] = sttTube->GetWireDirection().Y();
      tubePar[6] = sttTube->GetWireDirection().Z();
      tubePar[7] = a;

      Bool_t hitFits = kFALSE;

      for ( Int_t itr = 0 ; itr < fTracksVector.size() ; itr++ ) { 
	if ( fTracksVector[itr].trackHits.size() <= 3 ) continue;

	Bool_t trackFits = kFALSE;
	Int_t nofPars = fTracksVector[itr].trackPars.size(); // do it like this, cause can ADD parameters in the loop
	
	Double_t meanP_Z = 0.;
	Double_t goodP_Z = 0.;
	for ( Int_t ipar = 0 ; ipar < nofPars ; ipar++ ) {
	  if ( fTracksVector[itr].trackPars[ipar].n < 0.9*fTracksVector[itr].trackHits.size() ) continue;
	  if ( fTracksVector[itr].trackPars[ipar].p_z < 1.e-6 ) continue;
	  meanP_Z += fTracksVector[itr].trackPars[ipar].p_z < 1.e-6;
	  goodP_Z += 1.;
	}
	if ( goodP_Z > 0.5 ) 
	  meanP_Z/goodP_Z;
	
	for ( Int_t itp = 0 ; itp < nofPars ; itp++ ) {
	  if ( fTracksVector[itr].trackPars[itp].n < 0.9*fTracksVector[itr].trackHits.size() ) continue;
	  Double_t tempRad = fTracksVector[itr].trackPars[itp].r;
	  Double_t tempPhi = CalcPhi(fTracksVector[itr].trackPars[itp].x,fTracksVector[itr].trackPars[itp].y);
	  
	  Double_t tempCirc[4] = {fTracksVector[itr].trackPars[itp].x,
				  fTracksVector[itr].trackPars[itp].y,
				  fTracksVector[itr].trackPars[itp].r,
				  tempPhi};
	  Double_t intReg[4];
	  
	  Int_t nofReg = FindInterestingRegions(tempCirc,tubePar,intReg);
	  if ( nofReg==0 ) continue;

	  /*cout << "hit " << ihit << "." << hitN << " (" << hitDetId[hitN] << "." << hitDetNo[hitN] << ") connects to " << itr << "/" << itp << " with " << tempRad << " " << tempPhi << " " << fTracksVector[itr].trackPars[itp].p_z << ")" << itr << " at p_z = " << flush;
	  for ( Int_t ireg = 0 ; ireg < nofReg ; ireg++ ) 
	    cout << intReg[ireg] << "   " << flush;
	    cout << endl;*/
	  
	  Bool_t parFits = kFALSE;
	  // this par has no p_z information
	  if ( TMath::Abs(fTracksVector[itr].trackPars[itp].p_z) < 1.e-6 ) {
	    // check if there exist reasonable choice
	    Double_t smallestDiff = 666.;
	    Int_t    smallestRegD = -1;
	    if ( goodP_Z > 0.5 ) {
 	      for ( Int_t ireg = 0 ; ireg < nofReg ; ireg++ ) {
		if ( TMath::Abs(intReg[ireg]-meanP_Z ) < smallestDiff ) {
		  smallestDiff = TMath::Abs(intReg[ireg]-meanP_Z);
		  smallestRegD = ireg;
		}
	      }
	    }
	    if ( smallestDiff < resP_ZD ) {
	      fTracksVector[itr].trackPars[itp].p_z  = intReg[smallestRegD];
	      parFits = kTRUE;
	    }
	    else {
	      fTracksVector[itr].trackPars[itp].p_z  = intReg[0];
	      parFits = kTRUE;
	      
	      if ( nofReg > 1 ) {
		for ( Int_t ireg = 1 ; ireg < nofReg ; ireg++ ) {
		  TrackParameter tp0;
		  tp0.x = tempCirc[0];
		  tp0.y = tempCirc[1];
		  tp0.r = tempCirc[2];
		  tp0.p_z = intReg[ireg];
		  tp0.n   = fTracksVector[itr].trackPars[itp].n+1;
		  fTracksVector[itr].trackPars.push_back(tp0);
		  //		  cout << "creating another trackPar to track " << itr << " with alternative p_z -> " << intReg[ireg] << endl;
		}
	      }
	    }
	  }
	  else {	  // this par has already some p_z, check which found one corresponds to the one already there
	    Double_t smallestDiff = 666.;
	    Int_t    smallestRegD = -1;
	    for ( Int_t ireg = 0 ; ireg < nofReg ; ireg++ ) {
	      if ( TMath::Abs(intReg[ireg]-fTracksVector[itr].trackPars[itp].p_z ) < smallestDiff ) {
		smallestDiff = TMath::Abs(intReg[ireg]-fTracksVector[itr].trackPars[itp].p_z);
		smallestRegD = ireg;
	      }
	    }
	    if ( smallestRegD > resP_ZD ) continue;
// 	    cout << "updating existing trackPar " << itr << "." << itp << " by putting " 
// 		 << (fTracksVector[itr].trackPars[itp].p_z+intReg[smallestRegD])/2. << " = ( " 
// 		 << fTracksVector[itr].trackPars[itp].p_z << " + " 
// 		 << intReg[smallestRegD] << " ) / 2." << endl;
	    fTracksVector[itr].trackPars[itp].p_z = (fTracksVector[itr].trackPars[itp].p_z+intReg[smallestRegD])/2.;

	    parFits = kTRUE;
	  }
	  if ( !parFits ) continue;
	  
	  fTracksVector[itr].trackPars[itp].n += 1;
	  trackFits = kTRUE;
	}
	if ( !trackFits ) continue;

	//	cout << "!!!!! ADDING HIT " << hitDetId[hitN] << "." << hitDetNo[hitN] << " to track " << itr << endl;
	fTracksVector[itr].trackHits.push_back(detHit);
	fTracksVector[itr].trackHitD.push_back(hitDetId[hitN]);
	fTracksVector[itr].trackHitN.push_back(hitDetNo[hitN]);
	hitFits = kTRUE;
// 	cout << "hit " << ihit << " connects to track (" << meanRad << " " << meanPhi << " " << meanP_Z << ")" << itr << " at p_z = " << flush;
// 	for ( Int_t ireg = 0 ; ireg < nofReg ; ireg++ ) 
// 	  cout << intReg[ireg] << "   " << flush;
// 	cout << endl;
      }
      if ( hitFits ) continue;

      hitVector.push_back(detHit);
      hitVectDI.push_back(hitDetId[hitN]);
      hitVectHN.push_back(hitDetNo[hitN]);
      continue;
    }

    // prepare arrays to fit circles to this hit and all previous from hitVector
    Int_t nHits = hitVector.size();
    //    cout << "just checking, got " << nHits << " hits at z= " << flush; 
//     Double_t circPar[3];
//     if ( iTube != -1 ) // this is a STT hit
//       sH1[2] = ((PndSttHit*)detHit)->GetIsochrone();

    std::vector<Int_t> removeHitArray(0);

    // loop over previous hits
    for ( Int_t iph = 0 ; iph < nHits ; iph++ ) {
      //      if ( ihit == 52 || ihit == 54 ) cout << "ph XY = " << hitVector[iph]->GetX() << " " << hitVector[iph]->GetY() << endl;
      sH2[0] = hitVector[iph]->GetX();
      sH2[1] = hitVector[iph]->GetY();
      sH2[2] = 0.;
      //      cout << hitVector[iph]->GetZ() << " " << flush;
      if ( hitVectDI[iph] == 2 )
	sH2[2] = ((PndSttHit*)hitVector[iph])->GetIsochrone();
      else {
	if ( hitDetId[hitN] == hitVectDI[iph] ) 
	  if ( TMath::Abs(detHit->GetZ()-hitVector[iph]->GetZ()) < 3. ) 
	    continue;
      }
    
      Double_t phi2 = CalcPhi(sH2[0],sH2[1]);
      
      Double_t phiDiffr = TMath::Abs(phi1-phi2);
      if ( phiDiffr > TMath::Pi()/10. && phiDiffr < TMath::Pi()*19./10. ) continue;

      Int_t stepCno = 1; 
      if ( sH1[2] < 1.e-5 ) stepCno  = 2;
      if ( sH2[2] < 1.e-5 ) stepCno += 2; 

      Int_t fTN = -1;
      for ( Int_t icirc = 0 ; icirc < 4 ; icirc += stepCno ) {
	//	cout << icirc << " -------------------------------------------------" << endl;
	if ( FindCircPar(sH1,sH2,sH3,icirc,circPar) ) {
	      if ( circPar[2] < 10. || circPar[2] > 100000. ) continue;
//  	  cout << "  " << sH1[0] << " " << sH1[1] << " " << sH1[2] << endl;
//  	  cout << "+ " << sH2[0] << " " << sH2[1] << " " << sH2[2] << endl;
//  	  cout << "+ " << sH3[0] << " " << sH3[1] << " " << sH3[2] << endl;
//  	  cout << "= " << CalcPhi(circPar[0],circPar[1]) << " " << circPar[2] << endl;

	  TrackParameter tp0;
	  tp0.x = circPar[0];
	  tp0.y = circPar[1];
	  tp0.r = circPar[2];
	  tp0.p_z = 0.;
	  // temporary recognition of non-STT hit
	  // should take only parameters created using hit with VALID z information
	  if      ( sH1[2] < 1.e-5 ) tp0.p_z = CalcP_Z(circPar,sH1[0],sH1[1],detHit->GetZ());
	  else if ( sH2[2] < 1.e-5 ) tp0.p_z = CalcP_Z(circPar,sH2[0],sH2[1],hitVector[iph]->GetZ());
	  tp0.n = 2;

	  if ( fTN == -1 ) {
	    TrackBasis tb0;
	    tb0.trackPars.push_back(tp0);
	    tb0.trackHits.push_back(hitVector[iph]);
	    tb0.trackHitD.push_back(hitVectDI[iph]);
	    tb0.trackHitN.push_back(hitVectHN[iph]);
	    tb0.trackHits.push_back(detHit);
	    tb0.trackHitD.push_back(hitDetId[hitN]);
	    tb0.trackHitN.push_back(hitDetNo[hitN]);
	    tb0.meanX   = tp0.x;
	    tb0.meanY   = tp0.y;
	    tb0.meanR   = tp0.r;
	    tb0.meanP_Z = tp0.p_z;

	    fTN = fTracksVector.size();
	    fTracksVector.push_back(tb0);
	    removeHitArray.push_back(iph);
	    if ( fVerbose > 3 ) {
	      cout << "created another track" << endl;
	      //	    if ( ihit == 54 ) {
	      cout << "   >> with " << CalcPhi(tp0.x,tp0.y) << " (" << tp0.x << "," << tp0.y << ") " << tp0.r << " " << tp0.p_z << "(" << CalcPhi(tp0.x-sH1[0],tp0.y-sH1[1]) << " on " << detHit->GetZ() << ")" << endl;
	      // 	      Double_t tmpx = sH1[0]-tp0.x;
	      // 	      Double_t tmpy = sH1[1]-tp0.y;
	      // 	      cout << " should be: " << sH1[0] << " " << sH1[1] << " ---> " << tmpx << " " << tmpy << endl;
	      // 	      Double_t rotA = TMath::Pi()-CalcPhi(tp0.x,tp0.y);
	      // 	      cout << "rotate by " << rotA*TMath::RadToDeg() << " deg" << endl;
	      // 	      Double_t tmpx2 = tmpx*TMath::Cos(rotA)-tmpy*TMath::Sin(rotA);
	      // 	      Double_t tmpy2 = tmpx*TMath::Sin(rotA)+tmpy*TMath::Cos(rotA);
	      // 	      cout << " and then : " << tmpx   << " " << tmpy   << " ---> " << tmpx2 << " " << tmpy2 << endl;
	      // 	      cout << " and finally: " << CalcPhi(tmpx2,tmpy2) << endl;
	    }
	  }
	  else {
	    fTracksVector[fTN].meanR = -666.;
	    fTracksVector[fTN].trackPars.push_back(tp0);
	    if ( fVerbose > 3 ) {//ihit == 54 ) {
	      cout << "   >> with " << CalcPhi(tp0.x,tp0.y) << " (" << tp0.x << "," << tp0.y << ") " << tp0.r << " " << tp0.p_z << "(" << CalcPhi(tp0.x-sH1[0],tp0.y-sH1[1]) << " on " << detHit->GetZ() << ")" << endl;
// 	      Double_t tmpx = sH1[0]-tp0.x;
// 	      Double_t tmpy = sH1[1]-tp0.y;
// 	      cout << " should be: " << sH1[0] << " " << sH1[1] << " ---> " << tmpx << " " << tmpy << endl;
// 	      Double_t rotA = TMath::Pi()-CalcPhi(tp0.x,tp0.y);
// 	      cout << "rotate by " << rotA*TMath::RadToDeg() << " deg" << endl;
// 	      Double_t tmpx2 = tmpx*TMath::Cos(rotA)-tmpy*TMath::Sin(rotA);
// 	      Double_t tmpy2 = tmpx*TMath::Sin(rotA)+tmpy*TMath::Cos(rotA);
// 	      cout << " and then : " << tmpx   << " " << tmpy   << " ---> " << tmpx2 << " " << tmpy2 << endl;
// 	      cout << " and finally: " << CalcPhi(tmpx2,tmpy2) << endl;
	    }
	    //	    if ( ihit == 54 ) cout << "   >> with " << CalcPhi(tp0.x,tp0.y) << " (" << tp0.x << "," << tp0.y << ") " << tp0.r << " " << tp0.p_z << "(" << CalcPhi(tp0.x-sH1[0],tp0.y-sH1[1]) << " on " << detHit->GetZ() << ")" << endl;
	  }
	}
      }
      
    }
  

    //    cout << endl << "There are " << fTracksVector.size() << " segments in the event" << endl;
    for ( Int_t irh = removeHitArray.size()-1 ; irh >= 0 ; irh-- ) {
      for ( Int_t ilhit = removeHitArray[irh] ; ilhit < hitVector.size()-1 ; ilhit++ ) {
	hitVector[ilhit] = hitVector[ilhit+1];
	hitVectDI[ilhit] = hitVectDI[ilhit+1];
	hitVectHN[ilhit] = hitVectHN[ilhit+1];
      }
      hitVector.pop_back();
      hitVectDI.pop_back();
      hitVectHN.pop_back();
    }

    if ( removeHitArray.size() > 0 ) continue;

    hitVector.push_back(detHit);
    hitVectDI.push_back(hitDetId[hitN]);
    hitVectHN.push_back(hitDetNo[hitN]);
  }
  
  //  cout << endl << "There are " << fTracksVector.size() << " segments in the event" << endl;
  for ( Int_t itr = 0 ; itr < fTracksVector.size() ; itr++ ) {
    if ( fTracksVector[itr].trackHitD.size() < 4 ) continue;
    Double_t meanPhi = 0.;
    Double_t meanRad = 0.;
    Double_t goodPNR = 0.;
    Double_t meanP_Z = 0.;
    Double_t goodP_Z = 0.;
    std::vector<Bool_t> parGood(fTracksVector[itr].trackPars.size(),kFALSE);
    for ( Int_t ipar = 0 ; ipar < fTracksVector[itr].trackPars.size() ; ipar++ ) {
      Bool_t reasPar = kFALSE;
      Double_t tempPhi = CalcPhi(fTracksVector[itr].trackPars[ipar].x,fTracksVector[itr].trackPars[ipar].y);
      if ( fTracksVector[itr].trackPars[ipar].n >= 0.9*fTracksVector[itr].trackHitD.size() ) reasPar = kTRUE;
      //      if ( fTracksVector[itr].trackPars[ipar].n >= 0.6*fTracksVector[itr].trackHitD.size() &&
      if ( ( TMath::Abs(meanPhi/goodPNR-tempPhi) < 1./10. || TMath::Abs(meanPhi/goodPNR-tempPhi) > (2.*TMath::Pi()-(1./10.)) ) &&
	   ( TMath::Abs(meanRad/goodPNR-fTracksVector[itr].trackPars[ipar].r) < 0.1*meanRad/goodPNR ) &&
	   ( TMath::Abs(fTracksVector[itr].trackPars[ipar].p_z) > 1.e-6 && goodP_Z > 0.5 && TMath::Abs(fTracksVector[itr].trackPars[ipar].p_z-meanP_Z/goodP_Z) < 0.005 ) ) reasPar = kTRUE;
      //      cout << tempPhi << " " << fTracksVector[itr].trackPars[ipar].r << " " << fTracksVector[itr].trackPars[ipar].p_z << " VS " << endl;
      parGood[ipar] = reasPar;
      if ( reasPar == kFALSE ) continue;
      meanPhi += CalcPhi(fTracksVector[itr].trackPars[ipar].x,fTracksVector[itr].trackPars[ipar].y);
      meanRad += fTracksVector[itr].trackPars[ipar].r;
      goodPNR += 1.;
      if ( TMath::Abs(fTracksVector[itr].trackPars[ipar].p_z) > 1.e-6 ) {
	meanP_Z += fTracksVector[itr].trackPars[ipar].p_z;
	goodP_Z += 1.;
      }
    }
    //    if ( goodPNR < fTracksVector[itr].trackHits.size() ) continue;
    meanPhi /= goodPNR;
    meanRad /= goodPNR;
    if ( goodP_Z > 0.5 )
      meanP_Z /= goodP_Z;

    //    cout << "TRACK " << itr << " >>> " << flush;
    for ( Int_t ihit = fTracksVector[itr].trackHitD.size()-1 ; ihit >= 0 ; ihit-- ) {
      //cout << fTracksVector[itr].trackHitD[ihit] << "." << fTracksVector[itr].trackHitN[ihit] << " " << flush;
      //      if ( ihit == 0 ) cout << endl;
      if ( fTracksVector[itr].trackHitD[ihit] == 2 ) {
	Int_t tubeId = ((PndSttHit*)fTracksVector[itr].trackHits[ihit])->GetTubeID();
	PndSttTube* sttTube = (PndSttTube*) fTubeArray->At(tubeId);
	if ( sttTube->GetWireDirection().Z() < 1. ) {
	  continue; // leave the hit in the track
	}
      }
      
      sH2[0] = fTracksVector[itr].trackHits[ihit]->GetX();
      sH2[1] = fTracksVector[itr].trackHits[ihit]->GetY();
      sH2[2] = 0.;
      if ( fTracksVector[itr].trackHitD[ihit] == 2 )
	sH2[2] = ((PndSttHit*)fTracksVector[itr].trackHits[ihit])->GetIsochrone();
    
      sH1[0] = meanRad*TMath::Cos(meanPhi);
      sH1[1] = meanRad*TMath::Sin(meanPhi);
      sH1[2] = meanRad;

      if ( fVerbose > 4 )
	cout << "hit " << ihit << " distance to mean is: " << FindCircDist(sH1,sH2) << endl;
      
      if ( FindCircDist(sH1,sH2) > .1 ) {
	for ( Int_t ihit2 = ihit ; ihit2 < fTracksVector[itr].trackHitD.size()-1 ; ihit2++ ) {
	  fTracksVector[itr].trackHits[ihit2] = fTracksVector[itr].trackHits[ihit2+1];
	  fTracksVector[itr].trackHitD[ihit2] = fTracksVector[itr].trackHitD[ihit2+1];
	  fTracksVector[itr].trackHitN[ihit2] = fTracksVector[itr].trackHitN[ihit2+1];
	} 
	fTracksVector[itr].trackHits.pop_back();
	fTracksVector[itr].trackHitD.pop_back();
	fTracksVector[itr].trackHitN.pop_back();
      }
    }
    if ( fTracksVector[itr].trackHitD.size() < 4 ) continue;

    for ( Int_t iptr = 0 ; iptr < itr ; iptr++ ) {
      if ( fTracksVector[iptr].trackPars[fTracksVector[iptr].trackPars.size()-1].n < 666 ) continue;
      Double_t prevPhi = CalcPhi(fTracksVector[iptr].trackPars[fTracksVector[iptr].trackPars.size()-1].x,
				 fTracksVector[iptr].trackPars[fTracksVector[iptr].trackPars.size()-1].y);
      Double_t prevRad =         fTracksVector[iptr].trackPars[fTracksVector[iptr].trackPars.size()-1].r;
      Double_t prevP_Z =         fTracksVector[iptr].trackPars[fTracksVector[iptr].trackPars.size()-1].p_z;

      Bool_t sameTrack = kFALSE;
      if ( TMath::Abs(prevPhi-meanPhi) < 0.1 || TMath::Abs(prevPhi-meanPhi) > TMath::Pi()-0.1 ) {
	if ( TMath::Abs(meanRad-prevRad) < 0.1*meanRad ) {
	  if ( prevP_Z < 1.e-6 || meanP_Z < 1.e-6 ) sameTrack = kTRUE;
	  else if ( TMath::Abs(prevP_Z-meanP_Z) < 0.001 ) sameTrack = kTRUE;
	}
      }
      if ( !sameTrack ) continue;
      Double_t goodPRV = (Double_t)fTracksVector[iptr].trackPars[fTracksVector[iptr].trackPars.size()-1].n-666;

      if ( fVerbose > 3 ) 
	cout << "hm, seems that this track (" << meanPhi << " " << meanRad << " " << meanP_Z << " + " << goodPNR << ")" << endl
	     << "           is the same as (" << prevPhi << " " << prevRad << " " << prevP_Z << " + " << goodPRV << ")" << endl;

      for ( Int_t ipthit = 0 ; ipthit < fTracksVector[iptr].trackHits.size() ; ipthit++ ) {
	Bool_t newHit = kTRUE;
	for ( Int_t ithit = 0 ; ithit < fTracksVector[itr].trackHits.size() ; ithit++ ) {
	  if ( fTracksVector[iptr].trackHitD[ipthit] == fTracksVector[itr].trackHitD[ithit] &&
	       fTracksVector[iptr].trackHitN[ipthit] == fTracksVector[itr].trackHitN[ithit] ) {
	    newHit = kFALSE;
	    //  cout << fTracksVector[itr].trackHits[ithit] << " --> " << flush;
	    break;
	  }
	}
	if ( !newHit ) { 
	  if ( fVerbose > 4 )  cout << "hit " << ipthit << " already exists (" << fTracksVector[iptr].trackHits[ipthit] << ")" << endl; 
	  continue; 
	}
	if ( fVerbose > 4 )
	  cout << "             hit " << ipthit << " is a new hit " 
	       << fTracksVector[iptr].trackHitD[ipthit] << "." 
	       << fTracksVector[iptr].trackHitN[ipthit] << endl;
	fTracksVector[itr].trackHits.push_back(fTracksVector[iptr].trackHits[ipthit]);
	fTracksVector[itr].trackHitD.push_back(fTracksVector[iptr].trackHitD[ipthit]);
	fTracksVector[itr].trackHitN.push_back(fTracksVector[iptr].trackHitN[ipthit]);
      }
      
      meanPhi = (meanPhi*goodPNR+prevPhi*goodPRV)/(goodPNR+goodPRV);
      meanRad = (meanRad*goodPNR+prevRad*goodPRV)/(goodPNR+goodPRV);
      Double_t newP_Z = 0.;
      Double_t denP_Z = 0.;
      if ( TMath::Abs(meanP_Z) > 1.e-6 ) { newP_Z += meanP_Z*goodPNR; denP_Z += goodPNR; }
      if ( TMath::Abs(prevP_Z) > 1.e-6 ) { newP_Z += prevP_Z*goodPRV; denP_Z += goodPRV; }
      //  cout << "P_Z = " << meanP_Z << " " << prevP_Z << " " << goodPNR << " " << goodPRV << " " << newP_Z << " " << denP_Z << endl;
      meanP_Z = newP_Z/denP_Z;
      
      goodPNR += goodPRV;

      fTracksVector[iptr].trackPars[fTracksVector[iptr].trackPars.size()-1].n = -666;
      //      cout << "TRACK " << itr << " AND " << iptr << " ARE JOINED" << endl;
    }
    TrackParameter tp0;
    tp0.x   = meanRad*TMath::Cos(meanPhi);
    tp0.y   = meanRad*TMath::Sin(meanPhi);
    tp0.r   = meanRad;
    tp0.p_z = meanP_Z;
    tp0.n   = (Int_t)(666+goodPNR);
    fTracksVector[itr].trackPars.push_back(tp0);
  
    if ( fVerbose > 2 ) {
      for ( Int_t ipar = 0 ; ipar < fTracksVector[itr].trackPars.size() ; ipar++ ) {
	if ( parGood[ipar] == kTRUE ) 
	  cout << "        GOOD                     " << flush;
	else
	  cout << "                     BAD         " << flush;
	cout << "                                 " << CalcPhi(fTracksVector[itr].trackPars[ipar].x,fTracksVector[itr].trackPars[ipar].y) 
	     << " " << fTracksVector[itr].trackPars[ipar].r
	     << " " << fTracksVector[itr].trackPars[ipar].p_z
	     << " " << fTracksVector[itr].trackPars[ipar].n 
	     << endl;
      }
    }
 
    if ( fVerbose > 2 ) 
      for ( Int_t ihit = 0 ; ihit < fTracksVector[itr].trackHitD.size() ; ihit++ )
	cout << " hit " << fTracksVector[itr].trackHitD[ihit] << "." << fTracksVector[itr].trackHitN[ihit] << " ---> " 
	     << fTracksVector[itr].trackHits[ihit]->GetX() << " " 
	     << fTracksVector[itr].trackHits[ihit]->GetY() << " " 
	     << ((fTracksVector[itr].trackHitD[ihit]!=2)?(fTracksVector[itr].trackHits[ihit]->GetZ()):0.) << endl;
    if ( fVerbose > 3 )
      cout << "seg " << itr << "(" << fTracksVector[itr].trackHitD.size() << " hits) pars: " << meanPhi << " " << meanRad << " (basing on " << goodPNR << " parameters) and p_z = " << meanP_Z << " (" << goodP_Z << " pars)" << endl;
    
    //    cout << "---------------------------------------------" << endl;
  }
  

  //  cout << endl << "There are " << fTracksVector.size() << " segments in the event" << endl;
  for ( Int_t itr = 0 ; itr < fTracksVector.size() ; itr++ ) {
    if ( fTracksVector[itr].trackPars[fTracksVector[itr].trackPars.size()-1].n < 676 ) continue;
      Double_t prevPhi = CalcPhi(fTracksVector[itr].trackPars[fTracksVector[itr].trackPars.size()-1].x,
				 fTracksVector[itr].trackPars[fTracksVector[itr].trackPars.size()-1].y);
      Double_t prevRad =         fTracksVector[itr].trackPars[fTracksVector[itr].trackPars.size()-1].r;
      Double_t prevP_Z =         fTracksVector[itr].trackPars[fTracksVector[itr].trackPars.size()-1].p_z;
      Double_t goodPRV = (Double_t)fTracksVector[itr].trackPars[fTracksVector[itr].trackPars.size()-1].n-666;
      //      cout << " seg " << itr << " (" << fTracksVector[itr].trackHitD.size() << " hits) pars: " << prevPhi << " " << prevRad << " " << prevP_Z << " (basing on " << goodPRV << " parameters) " << endl;
      if ( TMath::IsNaN(prevP_Z) ) prevP_Z = 0.;
      cout << "F " << prevPhi << " " << prevRad << " " << prevP_Z << endl;
//       for ( Int_t ihit = 0 ; ihit < fTracksVector[itr].trackHitD.size() ; ihit++ )
// 	cout << fTracksVector[itr].trackHitD[ihit] << "." << fTracksVector[itr].trackHitN[ihit] << " " << flush;
//       cout << endl;
  }

  fTNofEvents++;
  
  fTracksVector.clear();

}
// -------------------------------------------------------------------------

// -----   Private method FindCircPar   -------------------------------
// the method finds a circle cl tangent to the three given circles c1, c2, c3 (circle given by x,y,r)
// parameter cno (from 0 to 7) is a bit mask on whether a given circle is inside or outside circle cl
Bool_t PndBarrelTrackFinder::FindCircPar(Double_t* c1, Double_t* c2, Double_t* c3, Int_t cno, Double_t* cl) {
  if ( cno < 0 || cno > 7 ) return kFALSE;
  
  Double_t s1 = -1+2*(cno%2); cno/=2;
  Double_t s2 = -1+2*(cno%2); cno/=2;
  Double_t s3 = -1+2*(cno%2);
  
  Double_t a2 = 2.*(c1[0]-c2[0]);
  Double_t b2 = 2.*(c1[1]-c2[1]);
  Double_t e2 = 2.*(s1*c1[2]-s2*c2[2]);
  Double_t d2 = (c1[0]*c1[0]+c1[1]*c1[1]-c1[2]*c1[2]) - (c2[0]*c2[0]+c2[1]*c2[1]-c2[2]*c2[2]);

  Double_t a3 = 2.*(c1[0]-c3[0]);
  Double_t b3 = 2.*(c1[1]-c3[1]);
  Double_t e3 = 2.*(s1*c1[2]-s3*c3[2]);
  Double_t d3 = (c1[0]*c1[0]+c1[1]*c1[1]-c1[2]*c1[2]) - (c3[0]*c3[0]+c3[1]*c3[1]-c3[2]*c3[2]);

  Double_t ax = (d2*b3-d3*b2)/(a2*b3-a3*b2);
  Double_t bx = (e2*b3-e3*b2)/(a2*b3-a3*b2);
  Double_t ay = (a2*d3-a3*d2)/(a2*b3-a3*b2);
  Double_t by = (a2*e3-a3*e2)/(a2*b3-a3*b2);

  Double_t sa = bx*bx+by*by-1.;
  Double_t sb = 2.*bx*(ax-c1[0])+2.*by*(ay-c1[1])+2.*s1*c1[2];
  Double_t sc = (ax-c1[0])*(ax-c1[0])+(ay-c1[1])*(ay-c1[1])-c1[2]*c1[2];
  if ( sa == 0 ) return kFALSE;
  Double_t dt = sb*sb-4.*sa*sc;
  if ( dt < 0 ) return kFALSE;

  cl[2] = (-sb-TMath::Sqrt(dt))/(2.*sa);
  cl[0] = ax+bx*cl[2];
  cl[1] = ay+by*cl[2];
  cl[2] = TMath::Abs(cl[2]);
  if ( TMath::IsNaN(cl[2]) ) {
    return kFALSE;
  }
  return kTRUE;
}
// -------------------------------------------------------------------------

// -----   Private method FindCircDist   -------------------------------
 Double_t PndBarrelTrackFinder::FindCircDist(Double_t* c1, Double_t* c2) {
  Double_t circDist = TMath::Sqrt((c1[0]-c2[0])*(c1[0]-c2[0])+
				  (c1[1]-c2[1])*(c1[1]-c2[1]));
  if ( circDist >=            c1[2]+c2[2]  ) { return circDist-c1[2]-c2[2];}
  if ( circDist <= TMath::Abs(c1[2]-c2[2]) ) { return TMath::Abs(c1[2]-c2[2])-circDist;}
  if ( c1[2] > c2[2] ) { return c2[2]-TMath::Abs(circDist-c1[2]);}
  return c1[2]-TMath::Abs(circDist-c2[2]);
}
// -------------------------------------------------------------------------

// -----   Private method CalcPhi   ----------------------------------------
Int_t PndBarrelTrackFinder::FindInterestingRegions(Double_t* circ, Double_t* tube, Double_t* reg) {
  Double_t ax = tube[4]/tube[6];
  Double_t ay = tube[5]/tube[6];
  Double_t a  = ax*ax+ay*ay;
  Double_t bx = (tube[0]-ax*tube[2]-circ[0]);
  Double_t by = (tube[1]-ay*tube[2]-circ[1]);
  Double_t b  = 2.*ax*bx+2.*ay*by;
  Double_t c  = bx*bx+by*by;

  Double_t delta[2];
  delta[0] = b*b-4.*a*(c-(circ[2]+tube[7])*(circ[2]+tube[7]));
  delta[1] = b*b-4.*a*(c-(circ[2]-tube[7])*(circ[2]-tube[7]));

  Int_t nofReg = 0;

  for ( Int_t idel = 0 ; idel < 2 ; idel++ ) {
    if ( delta[idel] < 0 ) continue;
    Double_t z[2];
    z[0] = (-b-TMath::Sqrt(delta[idel]))/2./a;
    z[1] = (-b+TMath::Sqrt(delta[idel]))/2./a;
    //    cout << " tube crosses circle in " << z[0] << " or " << z[1] << endl;
    
    for ( Int_t iz = 0 ; iz < 2 ; iz++ ) {
      Double_t p_z = 0.;
      if ( TMath::Abs(z[iz]-tube[2]) > tube[3] ) continue;
      //      cout << "checking z = " << z[iz] << endl;
      Double_t x  = tube[0] + ax * ( z[iz] - tube[2] );
      Double_t y  = tube[1] + ay * ( z[iz] - tube[2] );
      Double_t xd = x - circ[0];
      Double_t yd = y - circ[1];
      Double_t rd = TMath::Sqrt(xd*xd+yd*yd);
      Double_t pointPhi = TMath::ACos(xd/rd);
      if ( yd < 0 )
	pointPhi = 2.*TMath::Pi() - pointPhi;
      pointPhi += TMath::Pi();
      if ( pointPhi > 2.*TMath::Pi() ) pointPhi -= 2.*TMath::Pi();
      Double_t phiDiffr = (pointPhi-circ[3]);
      if ( phiDiffr >  TMath::Pi() ) phiDiffr -= 2.*TMath::Pi();
      if ( phiDiffr < -TMath::Pi() ) phiDiffr += 2.*TMath::Pi();
      p_z = phiDiffr/z[iz];
      
      //      cout << "got " << p_z << " while from other calc. it is: " << CalcP_Z(circ,x,y,z[iz]) << endl;
      reg[nofReg] = p_z;
      nofReg++;
    }
  }
  return nofReg;
}
// -------------------------------------------------------------------------

// -----   Private method CalcPhi   ----------------------------------------
Double_t PndBarrelTrackFinder::CalcPhi(Double_t x, Double_t y) {
  Double_t r = TMath::Sqrt(x*x+y*y);
  Double_t p = TMath::ACos(x/r);
  if ( y < 0 )
    p = 2.*TMath::Pi() - p;
  return p;
}
// -------------------------------------------------------------------------

// -----   Private method CalcP_Z   ----------------------------------------
Double_t PndBarrelTrackFinder::CalcP_Z(Double_t* circ, Double_t hx, Double_t hy, Double_t hz) {
  Double_t tmpx = hx-circ[0];
  Double_t tmpy = hy-circ[1];
  Double_t rotA = TMath::Pi()-CalcPhi(circ[0],circ[1]);
  Double_t tmpx2 = tmpx*TMath::Cos(rotA)-tmpy*TMath::Sin(rotA);
  Double_t tmpy2 = tmpx*TMath::Sin(rotA)+tmpy*TMath::Cos(rotA);
  Double_t phiT = CalcPhi(tmpx2,tmpy2);
  if ( phiT > TMath::Pi() ) phiT = -(2.*TMath::Pi()-phiT);
  return phiT/hz;
  //Double_t phiT = CalcPhi(hx-circ[0],hy-circ[1]);
  //  return phiT/hz;
}
// -------------------------------------------------------------------------

// -----   Private method SetParContainers   -------------------------------
void PndBarrelTrackFinder::SetParContainers() {
  
  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  fSttParameters = (PndGeoSttPar*) db->getContainer("PndGeoSttPar");

  cout << "-I- PndBarrelTrackFinder::SetParContainers()." << endl;
}
// -------------------------------------------------------------------------



// -----   Private method Init   -------------------------------------------
InitStatus PndBarrelTrackFinder::Init() {
  //  fTrackSegments.clear();
  
  // Get and check FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if( !ioman ) {
    cout << "-E- "<< GetName() <<"::Init: "
	 << "RootManager not instantised!" << endl;
    return kERROR;
  }
  
  // Get the pointer to the singleton FairRunAna object
  FairRunAna* ana = FairRunAna::Instance();
  if(NULL == ana) {
    cout << "-E- "<< GetName() <<"::Init :"
	 <<" no FairRunAna object!" << endl;
    return kERROR;
  }
  // Get the pointer to run-time data base
  FairRuntimeDb* rtdb = ana->GetRuntimeDb();
  if(NULL == rtdb) {
    cout << "-E- "<< GetName() <<"::Init :"
	 <<" no runtime database!" << endl;
    return kERROR;
  }
  
  fDetName[0] = "MVD";
  fDetName[1] = "MVD";
  fDetName[2] = "STT";
  fDetName[3] = "TPC";
  fDetName[4] = "GEM";

  TString hitArrayName[5] = {"MVDHitsPixel",
			     "MVDHitsStrip",
			     "STTHit",
			     "TPCHit",
			     "GEMHit"};

  for ( Int_t idet = 0 ; idet < 5 ; idet++ ) {
    if ( fIncludeDet[idet] == kTRUE ) {
      fHitArray[idet] = (TClonesArray*) ioman->GetObject(hitArrayName[idet].Data());
      
      if ( !fHitArray[idet] ) {
	cout << "-E- " << GetName() << "::Init: No " << hitArrayName[idet].Data() << " array!" << endl;
	return kERROR;
      }
      std::cout << "-I- " << GetName() << ": " << hitArrayName[idet].Data() << " array found" << std::endl;
    }
  }
  
  std::cout << "-I- " << GetName() << ": Initialization successfull" << std::endl;
  std::cout << "-I- " << GetName() << ": Looking for tracks in " << flush;
  for ( Int_t idet = 0 ; idet < 5 ; idet++ ) {
    if ( fIncludeDet[idet] == kFALSE ) continue;
    cout << fDetName[idet].Data() << ", ";
  }
  cout << "\b\b. " << endl;
  cout << "================================================================================" << endl;

  PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
  fTubeArray = mapper->FillTubeArray();
  cout << "****************************************************" << endl;
  cout << "**  " << fTubeArray->GetEntriesFast() << "  TUBES  ***********************************" << endl;
  cout << "****************************************************" << endl;
  
  return kSUCCESS;
}
// -------------------------------------------------------------------------



// -----   Private method ReInit   -----------------------------------------
InitStatus PndBarrelTrackFinder::ReInit() {

  return kERROR;

}
// -------------------------------------------------------------------------



// -----   Private method Reset   ------------------------------------------
void PndBarrelTrackFinder::Reset() {
}
// -------------------------------------------------------------------------

// -----   Public method Finish   ------------------------------------------
void PndBarrelTrackFinder::Finish() {
}
// -------------------------------------------------------------------------



ClassImp(PndBarrelTrackFinder)

