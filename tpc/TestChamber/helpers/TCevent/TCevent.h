#ifndef TCEVENT_H
#define TCEVENT_H
#include"TObject.h"
#include<iostream>
#include"TCtrack.h"
#include"TCcluster.h"
class TCevent : public TObject{
 public:
  std::vector<TCtrack*> tracks;
  std::vector<TCcluster*> clusters;

  unsigned int nClusters(){return clusters.size();}
  unsigned int nTracks(){return tracks.size();}

  void clear(){
    for(int i=0;i<nClusters();++i){delete clusters.at(i);}
    clusters.clear();
    for(int i=0;i<nTracks();++i){delete tracks.at(i);}
    tracks.clear();
  }

  void addClusters(std::vector<TCcluster*>& c){clusters=c;}
  void addTrack(TCtrack* t){tracks.push_back(t);}

  double calc(){
    std::cout << "TCevent::calc()" << std::endl;
    for(int i=0;i<nTracks();++i){
      tracks.at(i)->print();
    }
    std::cout << std::endl;
    return 1.;
  }

  ClassDef(TCevent,1);
};


#endif
