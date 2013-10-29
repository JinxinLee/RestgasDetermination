// -----------------------------------------
// PndDrcPhotonInfo.h
//
// Created on: 18.10.2013
// Author  : R.Dzhygadlo at gsi.de
// -----------------------------------------

#include "PndDrcPhotonInfo.h"

ClassImp(PndDrcPhotonInfo)

// -----   Default constructor   -------------------------------------------
PndDrcPhotonInfo::PndDrcPhotonInfo()
{ 
  fAmbiguitySize = 0;
  fHitTime = 0;
  fReflected = kFALSE;
  fEvReflections = 0;
}

PndDrcPhotonInfo::~PndDrcPhotonInfo()
{ 
  // for(std::vector<PndDrcAmbiguityInfo*>::iterator it = fAmbiguityArray.begin(); it != fAmbiguityArray.end(); it++){
  //   delete *it;
  // } 
  // fAmbiguityArray.clear();
}


void PndDrcPhotonInfo::AddAmbiguity(PndDrcAmbiguityInfo ambiguity){
  fAmbiguityArray.push_back(ambiguity);
  fAmbiguitySize++;
}
