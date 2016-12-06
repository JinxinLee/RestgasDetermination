//* $Id: 

// -------------------------------------------------------------------------
// -----                      PndGemDigi source file                   -----
// -------------------------------------------------------------------------
#include "PndGemDigi.h"

#include <iostream>

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
PndGemDigi::PndGemDigi() :
  fDetectorId(0),
  fChannelNr(0),
  fDigiCharge(0.),
  fDigiCor(0.0)
  {}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndGemDigi::PndGemDigi(Int_t iDetectorId, Double_t iChannel, Int_t index) : 
  fDetectorId(iDetectorId),
  fChannelNr(iChannel),
  fDigiCharge(0.),
  fDigiCor(0.0)
{
  AddIndex(index);
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndGemDigi::PndGemDigi(Int_t iDetectorId, Double_t iChannel, Int_t index, Double_t signal, Double_t time):
  fDetectorId(iDetectorId),
  fChannelNr(iChannel),
  fDigiCharge(signal),
  fDigiCor(0.0)
{  
  AddIndex(index);
  SetTimeStamp(time);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndGemDigi::~PndGemDigi() { }
// -------------------------------------------------------------------------



ClassImp(PndGemDigi)
