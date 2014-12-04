//
// PndTrkSciTHitList.cxx
// 
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//

#include "PndTrkSciTHitList.h"

#include "PndDetectorList.h"

#include <iostream>


using namespace std;

// ------------------ instance --------------------------
PndTrkSciTHitList* PndTrkSciTHitList::fscitInstance = 0;

PndTrkSciTHitList* PndTrkSciTHitList::Instance()
{
  if(!fscitInstance) cout << "NO PndTrkSciTHitList: you must fill sciT hit list BEFORE doing what you are doing!" << endl;
  return fscitInstance;
}

PndTrkSciTHitList* PndTrkSciTHitList::Instanciate() {
  fscitInstance = this;
}

// ------------------ instance ---------------------------

PndTrkSciTHitList::PndTrkSciTHitList() : PndTrkHitList(), fIRegion(-1) {}

PndTrkSciTHitList::PndTrkSciTHitList(Int_t iregion) : PndTrkHitList(), fIRegion(iregion) {}

PndTrkSciTHitList::~PndTrkSciTHitList() {
  fscitInstance = 0;
}

void PndTrkSciTHitList::AddHit(Int_t hitid, Int_t detid, FairHit *hit)
{
  TVector3 position;
  hit->Position(position);

  PndTrkHitList::AddHit(hitid, detid, 0, SCIT, position); // CHECK iregion
}



ClassImp(PndTrkSciTHitList)
 
