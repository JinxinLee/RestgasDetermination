//
// PndTrkConformalHitList.cxx
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//

#include "PndTrkConformalHitList.h"

#include <iostream>
#include "PndTrkConformalHit.h"

using namespace std;


PndTrkConformalHitList::PndTrkConformalHitList() : fTrasl(0., 0.), fAngle(0) {
  hitlist.resize(0);
}
PndTrkConformalHitList::PndTrkConformalHitList(double x, double y, double delta) : fTrasl(x, y), fAngle(delta) {
  hitlist.resize(0);
}
 
PndTrkConformalHitList::~PndTrkConformalHitList() {}

// ----------------------------------------------------

void PndTrkConformalHitList::ResetTo(double x, double y, double delta) {
  fTrasl.Set(x, y);
  fAngle = delta;
  hitlist.resize(0);
}

void PndTrkConformalHitList::AddHit(PndTrkConformalHit *chit) {
  hitlist.push_back(chit);
}

PndTrkConformalHit *PndTrkConformalHitList::GetHit(int index) {
  return hitlist[index];
}


void PndTrkConformalHitList::SetTranslation(double x, double y) {
  fTrasl.Set(x, y);
}

void PndTrkConformalHitList::SetRotation(double delta){
  fAngle = delta;
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
 
