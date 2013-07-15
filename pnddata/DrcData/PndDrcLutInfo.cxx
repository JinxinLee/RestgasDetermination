// -----------------------------------------
// PndDrcLutInfo.h
//
// Created on: 13.07.2013
// Author: R.Dzhygadlo at gsi.de
// -----------------------------------------

#include "PndDrcLutInfo.h"

ClassImp(PndDrcLutInfo)

// -----   Default constructor   -------------------------------------------
PndDrcLutInfo::PndDrcLutInfo()
{ 
  fSize = 0;
}


void PndDrcLutInfo::AddEntrie(Double_t val) {

  fAnglesArray.push_back(val);
  fSize++;
}


TVector3 PndDrcLutInfo::GetEntrie(Int_t entrie) {
  return fAnglesArray[entrie];
}

