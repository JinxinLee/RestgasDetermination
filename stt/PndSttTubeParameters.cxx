/////////////////////////////////////////////////////////////
// PndSttTubeParameters
//
// Class for tube parameters (object)
//
// authors: Lia Lavezzi - INFN Pavia (2013)
//                                   
/////////////////////////////////////////////////////////////


#include "PndSttTubeParameters.h"
#include "TGeoManager.h"
#include <iostream>
using std::cout;
using std::endl;

PndSttTubeParameters::PndSttTubeParameters()
  :fTubeID(-1),
   fHalfLength(0)
{
}

PndSttTubeParameters::PndSttTubeParameters(PndSttTubeParameters &tubs)
  :fTubeID(tubs.GetTubeID()),
   fHalfLength(tubs.GetHalfLength())
{
}

PndSttTubeParameters::PndSttTubeParameters(Int_t tubeID, Double_t hl) 
  : fTubeID(tubeID),
    fHalfLength(hl) 
{
}

PndSttTubeParameters::~PndSttTubeParameters(){
}

Double_t PndSttTubeParameters::GetHalfLength() { 
  return fHalfLength; 
}

Int_t PndSttTubeParameters::GetTubeID() {
  return fTubeID;
}

ClassImp(PndSttTubeParameters)
    
