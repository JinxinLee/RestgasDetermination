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
  int sensor =  ((PndGemHit*) hit)->GetSensorNr();


  // layerid = 0, 1, 2, ...
  int layerid = 2 * station + sensor - 3;


  PndTrkHitList::AddHit(hitid, detid, 0, GEM, layerid, position); // CHECK iregion
}



ClassImp(PndTrkGemHitList)
 
