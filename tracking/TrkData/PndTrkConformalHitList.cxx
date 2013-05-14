//
// PndTrkConformalHitList.cxx
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//

#include "PndTrkConformalHitList.h"

#include <iostream>
#include "PndTrkConformalHit.h"

using namespace std;


PndTrkConformalHitList::PndTrkConformalHitList() : fConformal(new PndTrkConformalTransform()) {
  hitlist.resize(0);
}
PndTrkConformalHitList::PndTrkConformalHitList(PndTrkConformalTransform *conformal) : fConformal(conformal) {
  hitlist.resize(0);
}
 
PndTrkConformalHitList::~PndTrkConformalHitList() {}

// ----------------------------------------------------

void PndTrkConformalHitList::Reset() {
  fConformal = NULL;
  hitlist.resize(0);
}

void PndTrkConformalHitList::AddHit(PndTrkConformalHit *chit) {
  hitlist.push_back(chit);
}

PndTrkConformalHit *PndTrkConformalHitList::GetHit(int index) {
  return hitlist[index];
}


void PndTrkConformalHitList::Print() {

  cout << "###############################" << endl;
  std::vector< PndTrkConformalHit * >::iterator itr = hitlist.begin();
  while(itr != hitlist.end()) {
    (*itr)->Print();
    itr++;
  }
}

void PndTrkConformalHitList::Draw(Color_t color) { 
  std::vector< PndTrkConformalHit * >::iterator itr = hitlist.begin();
  while(itr != hitlist.end()) {
    (*itr)->Draw(color);
    itr++;
  }
}



ClassImp(PndTrkHit)
 
