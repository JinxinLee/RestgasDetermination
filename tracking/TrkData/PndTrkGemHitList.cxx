//
// PndTrkGemHitList.cxx
// 
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//

#include "PndTrkGemHitList.h"

#include "PndDetectorList.h"

#include <iostream>


using namespace std;

// ------------------ instance --------------------------
PndTrkGemHitList* PndTrkGemHitList::fgemInstance = 0;

PndTrkGemHitList* PndTrkGemHitList::Instance()
{
  if(!fgemInstance) cout << "NO PndTrkGemHitList: you must fill gem hit list BEFORE doing what you are doing!" << endl;
  return fgemInstance;
}

PndTrkGemHitList* PndTrkGemHitList::Instanciate() {
  fgemInstance = this;
}

// ------------------ instance ---------------------------

PndTrkGemHitList::PndTrkGemHitList() : PndTrkHitList(), fIRegion(-1) {}

PndTrkGemHitList::PndTrkGemHitList(Int_t iregion) : PndTrkHitList(), fIRegion(iregion) {}

PndTrkGemHitList::~PndTrkGemHitList() {
  fgemInstance = 0;
}

void PndTrkGemHitList::AddHit(Int_t hitid, Int_t detid, FairHit *hit)
{
  TVector3 position;
  hit->Position(position);
  int station = ((PndGemHit*) hit)->GetStationNr();
  // int sensor = hit->GetSensorNr();

  PndTrkHitList::AddHit(hitid, detid, 0, GEM, station, position); // CHECK iregion
}



ClassImp(PndTrkGemHitList)
 
