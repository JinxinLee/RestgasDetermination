// -------------------------------------------------------------------------
// -----                    PndLinTrack source file                -----
// -----                                 -----
// -------------------------------------------------------------------------

#include "stdlib.h"
#include <iostream>
#include "PndLinTrack.h"
#include "TGeoManager.h"
#include "TGeoNode.h"
#include "TGeoVolume.h"
#include "TGeoMatrix.h"


// -----   Default constructor   -------------------------------------------
PndLinTrack::PndLinTrack() {
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
PndLinTrack::PndLinTrack(TString detName, Double_t p0, Double_t p1, Double_t p2, Double_t p3,
		 Double_t e0, Double_t e1, Double_t e2, Double_t e3,
  	    Double_t chi, Int_t first, Int_t last, Int_t cand){
  fDetName = detName;
  fP0 = p0; fP1 = p1; fP2 = p2; fP3 = p3;
  fE0 = e0; fE1 = e1; fE2 = e2; fE3 = e3;
  fChiSq = chi;
  fFirst = first; fLast = last;
  fCandId = cand;

}


// -----   Destructor   ----------------------------------------------------
PndLinTrack::~PndLinTrack() {}

// -------------------------------------------------------------------------
ClassImp(PndLinTrack);
