// -------------------------------------------------------------------------
// -----                 PndDchCylinderHit source file                 -----
// -----                Created 20/03/08  by A. Wronska                -----
// -------------------------------------------------------------------------

// Pnd includes
#include "PndDchCylinderHit.h"
#include "PndDchDigi.h"

// ROOT includes
#include "TMath.h"

// C++ includes
#include <iostream>
using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndDchCylinderHit::PndDchCylinderHit() {
	fDigiIndex        = 0;
	fWireXcoordLocal  = 0.;
	fWireZcoordGlobal = 0;
	fDistance         = 0.;
	fAlpha            = 0;
}
// -------------------------------------------------------------------------

// -----   Constructor   ---------------------------------------------------
PndDchCylinderHit::PndDchCylinderHit(Int_t digiidx, Double_t xLoc, Double_t zGlo,
				     Double_t dist, Double_t distErr, 
				     Double_t alpha){
  fDigiIndex        = digiidx;
  fWireXcoordLocal  = xLoc;
  fWireZcoordGlobal = zGlo;
  fDistance         = dist; 
  fDistanceError    = distErr;  
  fAlpha            = alpha;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndDchCylinderHit::~PndDchCylinderHit() {}
// -------------------------------------------------------------------------


// -----   Public method Print   -------------------------------------------
void PndDchCylinderHit::Print() const {
	cout << " Hit at  zGlobal =  "<< fWireZcoordGlobal <<
	  " cm, wire's xLocal = "<<fWireXcoordLocal<<
	  " cm, at a distance of "<<fDistance<<
	  " plus/minus "<<fDistanceError<<
	  " cm, wire inclination angle = "<<
	  fAlpha*TMath::RadToDeg()<<" deg" <<endl;
}
// -------------------------------------------------------------------------


ClassImp(PndDchCylinderHit)
