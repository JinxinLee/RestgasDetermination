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

PndTrkClusterList::PndTrkClusterList() : clusterlist(TObjArray()) {}
PndTrkClusterList::PndTrkClusterList(const PndTrkClusterList& clist) {
  *this = clist;
}

PndTrkClusterList::~PndTrkClusterList() {
clusterlist.Clear();
}
  
PndTrkClusterList& PndTrkClusterList::operator=(const PndTrkClusterList &clist){ 
  clusterlist = TObjArray(clist.clusterlist);
  return *this;
}


void PndTrkClusterList::AddCluster(PndTrkCluster *cluster) {
  clusterlist.Add(cluster);
}

void PndTrkClusterList::DeleteCluster(Int_t index) {
  clusterlist.RemoveAt(index);
}


// CHECK test this
void PndTrkClusterList::ReplaceCluster(Int_t index, PndTrkCluster *cluster) { 
  clusterlist.RemoveAt(index);
  clusterlist.AddAt(cluster, index);
}
 
void PndTrkClusterList::Reset() {
clusterlist.Clear();
}
// merge clusters

ClassImp(PndTrkClusterList)
