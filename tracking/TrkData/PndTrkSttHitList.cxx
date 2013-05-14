//
// PndTrkSttHitList.cxx
// 
// 
// authors: Lia Lavezzi - INFN Pavia (2012)
//

#include "PndTrkSttHitList.h"

#include "PndDetectorList.h"
#include "PndTrkParameters.h"
#include "PndSttTube.h"

#include <iostream>


using namespace std;

// ------------------ instance ----------------------------------
PndTrkSttHitList* PndTrkSttHitList::fInstance = 0;

PndTrkSttHitList* PndTrkSttHitList::Instance()
{
  if(!fInstance) cout << "NO PndTrkSttHitList: you must fill it BEFORE doing what you are doing!" << endl;
  return fInstance;
}

PndTrkSttHitList* PndTrkSttHitList::Instanciate() {
  if(fInstance) cout << "you are instanciating a PndTrkSttHitList, but you already did it, what are you doing?" << endl;
  fInstance = this;
}

PndTrkSttHitList::PndTrkSttHitList() : PndTrkHitList(), fTubeArray(NULL) {}

PndTrkSttHitList::PndTrkSttHitList(TClonesArray *tubearray) : PndTrkHitList(), fTubeArray(tubearray) {}
 
PndTrkSttHitList::~PndTrkSttHitList() { 
fInstance = 0;
}

// void PndTrkSttHitList::AddHit(Int_t hitid, Int_t detid, PndSttHit *hit) {
void PndTrkSttHitList::AddHit(Int_t hitid, Int_t detid, FairHit *hit) {
  TVector3 position;
//   hit->Position(position);

  int tubeID = ((PndSttHit*) hit)->GetTubeID();
  PndSttTube *tube = (PndSttTube* ) fTubeArray->At(tubeID);
  Int_t iregion = -1;
  position = tube->GetPosition();
  // parallel
  if(tube->GetWireDirection() == TVector3(0., 0., 1.)) {
    // x < 0
    if(tube->GetPosition().X() < 0) {
      if(tube->GetPosition().Perp() < SKEWLIMIT) iregion = INNER_LEFT; // 3
      else iregion = OUTER_LEFT; // 7
    }
    // x > 0
    else {
      if(tube->GetPosition().Perp() < SKEWLIMIT) iregion = INNER_RIGHT; // 2
      else iregion = OUTER_RIGHT; // 6
    }
  }
  // skewed tubes
  else {
    // x < 0
    if(tube->GetPosition().X() < 0) iregion = SKEW_LEFT; // 5
    // x > 0
    else iregion = SKEW_RIGHT; // 4

    //    cout << "tubeID " << tubeID << " " << iregion << endl;
    //    position.Print();
    //    tube->GetWireDirection().Print();
  }

  PndTrkHitList::AddHit(hitid, detid, 0, iregion, tubeID, position, ((PndSttHit *) hit)->GetIsochrone()); // CHECK iregion
}



ClassImp(PndTrkSttHitList)
 
