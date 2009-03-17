#include"TCevent.h"
#include<algorithm>//for swap

TCevent::TCevent(const TCevent& rhs){
  for(int i=0;i<rhs.nClusters();++i){
    
    clusters.push_back(rhs.clusters.at(i));
  }
  
  for(int i=0;i<rhs.nTracks();++i){
    TCtrack* t = new TCtrack();
    std::vector<TCcluster> c;
    for(int j =0;j<rhs.tracks.at(i)->nCl();++j){
      c.push_back(rhs.tracks.at(i)->getCl(j));
    }
    t->addClusters(c);
    tracks.push_back(t);
  }
}

TCevent::~TCevent(){clear();}

ClassImp(TCevent);
