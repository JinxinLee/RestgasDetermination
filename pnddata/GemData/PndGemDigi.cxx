//* $Id: 

// -------------------------------------------------------------------------
// -----                      PndGemDigi source file                   -----
// -----                  Created 28/08/06  by V. Friese               -----
// -------------------------------------------------------------------------
#include "PndGemDigi.h"

#include <iostream>

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
PndGemDigi::PndGemDigi() {
  fDetectorId   =  0;
  fChannelNr    =  0;

  fDigiADC      =  0.;
  fDigiTDC      =  0.;  
  fDigiCor      =  0.5; 
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndGemDigi::PndGemDigi(Int_t iDetectorId, Int_t iChannel, Int_t index) {

  fDetectorId   = iDetectorId;
  fChannelNr    = iChannel;
  
  fIndex.push_back(index);

  fDigiADC      =  0.;
  fDigiTDC      =  0.;  
  fDigiCor      =  0.5;  
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndGemDigi::~PndGemDigi() { }
// -------------------------------------------------------------------------



ClassImp(PndGemDigi)
