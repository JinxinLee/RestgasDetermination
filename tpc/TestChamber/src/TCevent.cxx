#include"TCevent.h"
#include<algorithm>//for swap

TCevent::TCevent(const TCevent& rhs){
  for(int i=0;i<rhs.nClusters();++i){
    TCcluster *c = new TCcluster(*(rhs.clusters.at(i)));
    clusters.push_back(c);
  }
  
  for(int i=0;i<rhs.nTracks();++i){
    std::vector<int> index;
    for(int j=0;j<rhs.tracks.at(i)->nCl();++j){
      TCcluster* rhsCluster = rhs.tracks.at(i)->getCl(j);
	  for(int k=0;k<rhs.nClusters();++k){
		if(rhs.clusters.at(k)==rhsCluster) {
		  index.push_back(k);
		  break;
		}
	  }
    }
    TCtrack* t = new TCtrack;
    std::vector<TCcluster*> lhsClusters;
    for(int j=0;j<index.size();++j){
      lhsClusters.push_back(rhs.clusters.at(j));
    }
    t->addClusters(lhsClusters);
    tracks.push_back(t);
  }
}

TCevent::~TCevent(){clear();}

ClassImp(TCevent);
