// -----------------------------------------
// PndDrcTrackInfo.h
//
// Created on: 18.10.2013
// Author  : R.Dzhygadlo at gsi.de
// -----------------------------------------

#include "PndDrcTrackInfo.h"

ClassImp(PndDrcTrackInfo)

// -----   Default constructor   -------------------------------------------
PndDrcTrackInfo::PndDrcTrackInfo(): TObject(),
  fPhotonArray(),
  fPhotonSize(0), 
  fMcPdg(0),
  fMcMomentum(TVector3()),
  fMcMomentumInBar(TVector3()),
  fMcPositionInBar(TVector3()),
  fMcCherenkov(0),
  fPdg(0),
  fMomentum(TVector3()),
  fCherenkov(0)
{ 
}


PndDrcTrackInfo::~PndDrcTrackInfo()
{ 
  // for(std::vector<PndDrcPhotonInfo*>::iterator it = fPhotonArray.begin(); it != fPhotonArray.end(); it++){
  //   (*it)->Delete();
  // } 
  // fPhotonArray.clear();
}

void PndDrcTrackInfo::AddPhoton(PndDrcPhotonInfo photon){
  fPhotonArray.push_back(photon);
  fPhotonSize++;
}
