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

PndTrkTrackList::PndTrkTrackList() : tracklist(TObjArray()) {}

PndTrkTrackList::PndTrkTrackList(const PndTrkTrackList& tlist) {
  *this = tlist;
}

PndTrkTrackList::~PndTrkTrackList() {
tracklist.Clear();
}
  
PndTrkTrackList& PndTrkTrackList::operator=(const PndTrkTrackList &tlist){ 
  tracklist = TObjArray(tlist.tracklist);
  return *this;
}


void PndTrkTrackList::AddTrack(PndTrkTrack *track) {
  tracklist.Add(track);
}

void PndTrkTrackList::DeleteTrack(Int_t index) {
  tracklist.RemoveAt(index);
}


// CHECK test this
void PndTrkTrackList::ReplaceTrack(Int_t index, PndTrkTrack *track) { 
  tracklist.RemoveAt(index);
  tracklist.AddAt(track, index);
}
 
void PndTrkTrackList::Reset() {
tracklist.Clear();
}
// merge tracks

ClassImp(PndTrkTrackList)
