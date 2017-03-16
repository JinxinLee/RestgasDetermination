//
// PndTrkSkewHitList.cxx
//
// authors: Lia Lavezzi - University of Torino (2014)
//

#include "PndTrkSkewHitList.h"

#include <iostream>
#include "PndTrkConformalHit.h"

using namespace std;


PndTrkSkewHitList::PndTrkSkewHitList() : fHitList(TClonesArray("PndTrkSkewHit", 10000)) {}

PndTrkSkewHitList::PndTrkSkewHitList(const PndTrkSkewHitList& hlist) : TObject(hlist) {
  *this = hlist;
}

PndTrkSkewHitList::~PndTrkSkewHitList() {
  fHitList.Delete();
}

PndTrkSkewHitList& PndTrkSkewHitList::operator=(const PndTrkSkewHitList& hlist) {
  fHitList = TClonesArray(hlist.fHitList);
  return *this;
}
 

// ----------------------------------------------------

void PndTrkSkewHitList::Clear(Option_t* opt) {
  fHitList.Clear(opt);
}

void PndTrkSkewHitList::AddHit(PndTrkSkewHit *shit) {
  int size = fHitList.GetEntriesFast();
  new(fHitList[size]) PndTrkSkewHit(*shit);
}

void PndTrkSkewHitList::AddHit(PndTrkSkewHit hit) {
  int size = fHitList.GetEntriesFast();
  new(fHitList[size]) PndTrkSkewHit(hit);
}

void PndTrkSkewHitList::AddHit(PndTrkHit *hit) {
  int size = fHitList.GetEntriesFast();
  new(fHitList[size]) PndTrkSkewHit(*hit);
} 

PndTrkSkewHit *PndTrkSkewHitList::GetHit(int index) {
  return (PndTrkSkewHit*) fHitList.At(index);
}


void PndTrkSkewHitList::Print() {

  cout << "###############################" << endl;
  for(int ihit = 0; ihit < GetNofHits(); ihit++) {
    PndTrkSkewHit* shit = (PndTrkSkewHit*) fHitList.At(ihit);
    shit->Print();
  }
}


void PndTrkSkewHitList::Draw(Color_t color) { 
   for(int ihit = 0; ihit < GetNofHits(); ihit++) {
     PndTrkSkewHit* shit = (PndTrkSkewHit*) fHitList.At(ihit);
     shit->Draw(color);
   }
}



ClassImp(PndTrkSkewHitList)
 
