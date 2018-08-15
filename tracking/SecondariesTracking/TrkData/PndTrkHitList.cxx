//
// PndTrkHitList.cxx
// 
// 
// 
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//
////////////////////////////////////////////////////////////

#include "PndTrkHitList.h"

#include "PndDetectorList.h"

#include <iostream>
#include "TMath.h"
#include "PndSttHit.h"
#include "PndSdsHit.h"

#include "TClonesArray.h"

using namespace std;


PndTrkHitList::PndTrkHitList() {
  hitlist.resize(0);
}
 
PndTrkHitList::~PndTrkHitList() {}

// ----------------------------------------------------
void PndTrkHitList::AddHit(Int_t hitID, Int_t detID, Bool_t used, Int_t iregion, TVector3& pos) 
{
  PndTrkHit hit(hitID, detID, used, iregion, -1, pos, 0., -1);
  hitlist.push_back(hit);
}

void PndTrkHitList::AddHit(Int_t hitID, Int_t detID, Bool_t used, Int_t iregion, Int_t tubeID, TVector3& pos, Double_t isochrone)
{
  PndTrkHit hit(hitID, detID, used, iregion, tubeID, pos, isochrone, -1);
  hitlist.push_back(hit);
}

void PndTrkHitList::AddHit(Int_t hitID, Int_t detID, Bool_t used, Int_t iregion, Int_t sensorID, TVector3& pos)
{
  PndTrkHit hit(hitID, detID, used, iregion, sensorID, pos, 0.0, -1);
  hitlist.push_back(hit);
}


void PndTrkHitList::AddHit(PndTrkHit *hit)
{
  hitlist.push_back(*hit);
}
// ----------------------------------------------------

void PndTrkHitList::AddHit(Int_t hitid, Int_t detid, FairHit *hit) {
  TVector3 position;
  hit->Position(position);
  AddHit(hitid, detid, 0, -1, position); // CHECK iregion
}

void PndTrkHitList::AddTCA(Int_t detID, TClonesArray *array) {
 for(int ihit = 0; ihit < array->GetEntriesFast(); ihit++) {
    FairHit *hit = (FairHit*) array->At(ihit);
    AddHit(ihit, detID, hit);
 }
}

PndTrkHit *PndTrkHitList::GetHit(int index) {
  return &hitlist[index];

}

PndTrkHit *PndTrkHitList::GetHitByID(int id) {
  std::vector< PndTrkHit >::iterator itr = hitlist.begin();
  while(itr != hitlist.end()) {
    if(id == (*itr).GetHitID()) return &(*itr);
    itr++;
  }
  return NULL;

}

void PndTrkHitList::Print() {

  cout << "###############################" << endl;
  std::vector< PndTrkHit >::iterator itr;
  itr = hitlist.begin();
  while(itr != hitlist.end()) {
    cout << " " <<  (*itr).GetHitID() << " " << (*itr).GetDetectorID() << endl;
    itr++;
  }
}

void PndTrkHitList::Draw(Color_t color) { 
  std::vector< PndTrkHit >::iterator itr = hitlist.begin();
  while(itr != hitlist.end()) {
    (*itr).Draw(color);
    itr++;
  }
}



ClassImp(PndTrkHit)
 
