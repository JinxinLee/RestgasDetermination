//
// PndTrkClusterList.cxx
// 
// Class for pattern recognition hit
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//

#include "PndTrkCluster.h"
#include "PndTrkClusterList.h"

#include "PndDetectorList.h"

#include <iostream>


using namespace std;

PndTrkClusterList::PndTrkClusterList() {
clusterlist.clear();
}

PndTrkClusterList::~PndTrkClusterList() {
clusterlist.clear();
}

void PndTrkClusterList::AddCluster(PndTrkCluster cluster) {
  clusterlist.push_back(cluster);
}

void PndTrkClusterList::DeleteCluster(Int_t index) {
  clusterlist.erase(clusterlist.begin() + index);
}

void PndTrkClusterList::ReplaceCluster(Int_t index, PndTrkCluster *cluster) { 
 //  PndTrkCluster *oldcluster = GetCluster(index);
//   // CHECK qualche tipo di if
//   cout << "replace" << endl;
//   oldcluster->PrintList();
//   cout << "with" << endl;  
// cluster->PrintList();
//     std::replace(clusterlist.begin(), clusterlist.end(), *oldcluster, *cluster);
}
 

// merge clusters

ClassImp(PndTrkClusterList)
