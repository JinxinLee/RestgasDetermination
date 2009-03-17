#ifndef TCEVENT_H
#define TCEVENT_H
#include"TObject.h"
#include<iostream>
#include"TCtrack.h"
#include"TCcluster.h"
class TCevent : public TObject{
 public:
  ~TCevent();

  TCevent(const TCevent&);
  TCevent(){};

  unsigned int nClusters()const {return clusters.size();};
  unsigned int nTracks() const {return tracks.size();};

  void clear(){
    clusters.clear();
    for(unsigned int i=0;i<nTracks();++i){delete tracks.at(i);}
    tracks.clear();
  };

  void addClusters(std::vector<TCcluster>& c){clusters=c;};
  void addCluster(TCcluster c){clusters.push_back(c);};
  void addTrack(TCtrack* t){tracks.push_back(t);};

  TCtrack* getTrack(int i) {return tracks.at(i);};
  TCcluster getCluster(int i) {return clusters.at(i);};

 private:
  TCevent& operator=(const TCevent&){throw;};//dont need this!
  std::vector<TCtrack*> tracks;
  std::vector<TCcluster> clusters;

 public:
  ClassDef(TCevent,1);
};


#endif
