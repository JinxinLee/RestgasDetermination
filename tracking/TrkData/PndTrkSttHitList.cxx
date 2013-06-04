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

PndTrkSttHitList::PndTrkSttHitList() : PndTrkHitList(), fTubeArray(NULL) {
  hitmap.clear();
}

PndTrkSttHitList::PndTrkSttHitList(TClonesArray *tubearray) : PndTrkHitList(), fTubeArray(tubearray) { 
   hitmap.clear();
}
 
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
  
  
  std::map< int , int > ::iterator it;
  int isec = tube->GetSectorID();
  hitmap.insert( std::pair<int , int > (isec, hitlist.size() - 1));

}



void PndTrkSttHitList::PrintSectors() {

  for(int isec = 0; isec  < 6; isec++) {
    cout << "SECTOR " << isec << ": ";


    std::pair < std::multimap< int, int >::iterator, std::multimap < int, int >::iterator> ret;
    ret = hitmap.equal_range(isec);
    for (std::multimap< int, int >::iterator it = ret.first; it != ret.second; ++it)
      std::cout << ' ' << it->second;
    cout << endl;
  }
  
  //   for(int isec = 0; isec  < 6; isec++) {
  //     cout << "SECTOR " << isec << ": ";
  //     it =  hitmap.find(isec);
  //     if(it == hitmap.end()) continue;
  //     std::vector< int > listofhits = it->second;
  //     for(int ihit = 0; ihit < listofhits.size() ;  ihit++) {
  //       cout << " " << hitlist.at(listofhits[ihit]).GetHitID();
  //     }
  //     cout << endl;
  //  }
}


int PndTrkSttHitList::GetNofHitsInSector(int isec) {
  std::pair < std::multimap< int, int >::iterator, std::multimap < int, int >::iterator> ret;
  ret = hitmap.equal_range(isec);
  int counter = 0;
  for (std::multimap< int, int >::iterator it = ret.first; it != ret.second; ++it) counter++; // CHECK
  return counter; 


}

std::vector< PndTrkHit* > PndTrkSttHitList::GetHitListFromSector(int isec) { 

  std::vector< PndTrkHit* > thissector;

  std::pair < std::multimap< int, int >::iterator, std::multimap < int, int >::iterator> ret;
  ret = hitmap.equal_range(isec);
  for (std::multimap< int, int >::iterator it = ret.first; it != ret.second; ++it) {
    PndTrkHit *hit = &hitlist[it->second];
    //   cout << "hit " << hit->GetHitID() << endl;
    thissector.push_back(hit);
  }
  return thissector;
}

PndTrkHit *PndTrkSttHitList::GetHitFromSector(int ihit, int isec) {
  std::vector< PndTrkHit* > listofhits = GetHitListFromSector(isec);
  return listofhits.at(ihit);
}

void PndTrkSttHitList::PrintSector(int isec) { 
 std::vector< PndTrkHit* > listofhits = GetHitListFromSector(isec);
   cout << "Sector " << isec << ": ";
   for (int ihit = 0; ihit < listofhits.size(); ihit++) cout << " " << (listofhits.at(ihit))->GetHitID();
   cout << endl;
}

void PndTrkSttHitList::DrawSector(int isec, Color_t color) {
  
   std::vector< PndTrkHit* > listofhits = GetHitListFromSector(isec);
   for (int ihit = 0; ihit < listofhits.size(); ihit++) (listofhits.at(ihit))->Draw(color);
}
ClassImp(PndTrkSttHitList)
 
