//
// PndTrkClusterList.cxx
// 
// Class for pattern recognition cluster list
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//

// #include "PndTrkCluster.h"
#include "PndTrkClusterList.h"

// #include "PndDetectorList.h"

#include <iostream>


using namespace std;

PndTrkClusterList::PndTrkClusterList() : fClusterList(TClonesArray("PndTrkCluster", 10000)) {}

PndTrkClusterList::PndTrkClusterList(const PndTrkClusterList& clist) {
  *this = clist;
}

PndTrkClusterList::~PndTrkClusterList() {
fClusterList.Clear();
}
  
PndTrkClusterList& PndTrkClusterList::operator=(const PndTrkClusterList &clist){ 
  fClusterList = TClonesArray(clist.fClusterList);
  return *this;
}


void PndTrkClusterList::AddCluster(PndTrkCluster *cluster) {
  int size = fClusterList.GetEntriesFast();
  new(fClusterList[size]) PndTrkCluster(*cluster);
}

void PndTrkClusterList::DeleteCluster(Int_t index) {
  fClusterList.RemoveAt(index);
}


// // CHECK test this
// void PndTrkClusterList::ReplaceCluster(Int_t index, PndTrkCluster *cluster) { 
//   fClusterList.RemoveAt(index);
//   fClusterList.AddAt(cluster, index);
// }
 
void PndTrkClusterList::Reset() {
  Clear();
}

void PndTrkClusterList::Clear(Option_t* opt) {
fClusterList.Clear(opt);
}

// merge clusters

ClassImp(PndTrkClusterList)
