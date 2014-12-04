//
// PndTrkTrackList.cxx
// 
// Class for pattern recognition track list
// (taken from PndTrkClusterList)
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//

// #include "PndTrkTrack.h"
#include "PndTrkTrackList.h"

// #include "PndDetectorList.h"

#include <iostream>


using namespace std;

PndTrkTrackList::PndTrkTrackList() : fTrackList(TClonesArray("PndTrkTrack", 10000)) {}

PndTrkTrackList::PndTrkTrackList(const PndTrkTrackList& tlist) {
  *this = tlist;
}

PndTrkTrackList::~PndTrkTrackList() {
  fTrackList.Clear();
}
  
PndTrkTrackList& PndTrkTrackList::operator=(const PndTrkTrackList &tlist){ 
  fTrackList = TClonesArray(tlist.fTrackList);
  return *this;
}


void PndTrkTrackList::AddTrack(PndTrkTrack *track) {
  int size = fTrackList.GetEntriesFast();
  new(fTrackList[size]) PndTrkTrack(*track);
}

void PndTrkTrackList::DeleteTrack(Int_t index) {
  fTrackList.RemoveAt(index);
}


// // CHECK test this
// void PndTrkTrackList::ReplaceTrack(Int_t index, PndTrkTrack *track) { 
//   fTrackList.RemoveAt(index);
//   fTrackList.AddAt(track, index);
// }
 
void PndTrkTrackList::Reset() {
  Clear();
}

void PndTrkTrackList::Clear(Option_t* opt) {
fTrackList.Clear(opt);
}
// merge tracks

ClassImp(PndTrkTrackList)
