//
// PndTrkConformalHitList.cxx
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//

#include "PndTrkConformalHitList.h"

#include <iostream>
#include "PndTrkConformalHit.h"

using namespace std;


PndTrkConformalHitList::PndTrkConformalHitList() : fConformal(new PndTrkConformalTransform()), hitlist(TObjArray()) {}

PndTrkConformalHitList::PndTrkConformalHitList(PndTrkConformalTransform *conformal) : fConformal(conformal), hitlist(TObjArray()) {}
 
PndTrkConformalHitList::PndTrkConformalHitList(const PndTrkConformalHitList& hlist) : fConformal(new PndTrkConformalTransform()) {
  *this = hlist;
}

PndTrkConformalHitList::~PndTrkConformalHitList() {}

PndTrkConformalHitList& PndTrkConformalHitList::operator=(const PndTrkConformalHitList& hlist) {
  hitlist = TObjArray(hlist.hitlist);
  fConformal = hlist.fConformal;
  return *this;
}
 

// ----------------------------------------------------

void PndTrkConformalHitList::Reset() {
  fConformal = NULL;
  hitlist.Clear();
}

void PndTrkConformalHitList::AddHit(PndTrkConformalHit *chit) {
  hitlist.Add(chit);
}

PndTrkConformalHit *PndTrkConformalHitList::GetHit(int index) {
  return (PndTrkConformalHit*) hitlist[index];
}


void PndTrkConformalHitList::Print() {

  cout << "###############################" << endl;
  for(int ihit = 0; ihit < GetNofHits(); ihit++) {
    PndTrkConformalHit* chit = (PndTrkConformalHit*) hitlist[ihit];
    chit->Print();
  }
}


void PndTrkConformalHitList::Draw(Color_t color) { 
   for(int ihit = 0; ihit < GetNofHits(); ihit++) {
    PndTrkConformalHit* chit = (PndTrkConformalHit*) hitlist[ihit];
    chit->Draw(color);
   }
}



ClassImp(PndTrkHit)
 
