//* $Id: 

// -------------------------------------------------------------------------
// -----                      PndGemDigi source file                   -----
// -------------------------------------------------------------------------
#include "PndDrcDigi.h"

#include <iostream>

using std::cout;
using std::endl;


// copied from GEM C.S. 5.9.2011

// -----   Default constructor   -------------------------------------------
PndDrcDigi::PndDrcDigi() {
  fDetectorId   =  0;
  fPixelNr    =  0;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndDrcDigi::PndDrcDigi(Int_t iDetectorId, Int_t iPixel, Int_t index) {

  AddIndex(index);

  fDetectorId   = iDetectorId;
  fPixelNr      = iPixel;
  
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndDrcDigi::~PndDrcDigi() { }
// -------------------------------------------------------------------------



ClassImp(PndDrcDigi)
