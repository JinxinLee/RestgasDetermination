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
PndLinTrack::PndLinTrack(TString detName, Double_t z0, Double_t p0, Double_t p1, Double_t p2, Double_t p3, 
			 Double_t p0err, Double_t p1err, Double_t p2err, Double_t p3err, 
			 Double_t chi, Int_t first, Int_t last, Int_t cand){
  fDetName = detName;
  fP0 = p0; fP1 = p1; fP2 = p2; fP3 = p3;
  fChiSq = chi;
  fFirst = first; fLast = last;
  fCandId = cand;
  fP0err = p0err; fP1err = p1err; fP2err = p2err; fP3err = p3err;
  fz0 = z0;
}


// -----   Destructor   ----------------------------------------------------
PndLinTrack::~PndLinTrack() {}

// -------------------------------------------------------------------------
ClassImp(PndLinTrack);
