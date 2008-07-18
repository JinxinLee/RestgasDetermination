#ifndef ANALYSISEVENT_H
#define ANALYSISEVENT_H

#include<iostream>
#include<cmath>

#include "TObject.h"
#include "TMath.h"
#include "TClonesArray.h"

#include "AnalysisCluster.h"


class AnalysisEvent : public TObject {
 public:
  
  AnalysisEvent();
  virtual ~AnalysisEvent();

  void addCluster(const AnalysisCluster& c) {
	int size = nClusters();
	new((*_clusters)[size]) AnalysisCluster(c);
	//_clusters.push_back(c);
  }

  void setTrackParams(const double& ax,const double& bx,const double& ay,const double& by) {
	_ax=ax;	_bx=bx; _ay=ay; _by=by;
  }

  unsigned int nClusters(){
	return _clusters->GetEntriesFast();
	//return _clusters.size();
  }
  
  AnalysisCluster* getCluster(unsigned int i) {
	if(i>=nClusters()) {
	  std::cerr << "cluster index out of range -> abort" << std::endl;
	  throw;
	}
	return (AnalysisCluster*)_clusters->At(i);
  }

  void clear() {
	_ax=0.;_bx=0.;_ay=0.;_by=0.;
	_th=0.;_thX=0.;_thY=0.;
	_trigger=0;
	//_clusters.clear();
	_clusters->Delete();
  }

  void setTrigger(int t) {_trigger=t;}

  void setNhits(int i) {_nhits=i;}
  void setNselhits(int i) {_nselhits=i;}
  int getNhits(){return _nhits;}
  int getNselhits(){return _nselhits;}

  void setAngles() {
	_thX=180.*atan(_ax)/TMath::Pi();
	_thY=180.*atan(_ay)/TMath::Pi();
	_th= 180.*atan(sqrt(_ay*_ay+_ax*_ax))/TMath::Pi();
  }
  
  void print() {
	std::cout << "+++++++++++++++++++++++++++++++++" << std::endl;
	std::cout << "Printing AnalysisEvent:" << std::endl;
	std::cout << "TrackParameters (ax,bx,ay,by): (" << _ax
			  << "," << _bx
			  << "," << _ay
			  << "," << _by << ")" << std::endl;
	std::cout << "Angles (th,thX,thY): (" << _th
			  << "," << _thX
			  << "," << _thY << ")" << std::endl;
	std::cout << "Trigger, nHits, nSelHits : " << _trigger 
			  << ", " << _nhits 
			  << ", " << _nselhits << std::endl;
	std::cout << "Printing all " << nClusters() << " clusters:" << std::endl;
	for(unsigned int i=0;i<nClusters();++i) getCluster(i)->print();
	std::cout << "---------------------------------" << std::endl << std::endl;
  }

  int trigger() {return _trigger;}
  int nhits() {return _nhits;}
  int nselhits() {return _nselhits;}


  double ax(){return _ax;}
  double bx(){return _bx;}
  double ay(){return _ay;}
  double by(){return _by;}
  double th(){return _th;}
  double thX(){return _thX;}
  double thY(){return _thY;}


  double getMinYresid();

 private:
  double _ax,_bx,_ay,_by;
  double _th,_thX,_thY;
  //  std::vector<AnalysisCluster> _clusters;
  TClonesArray* _clusters;
  int _trigger;
  int _nhits;
  int _nselhits;

 public:
  ClassDef(AnalysisEvent,2)

};

#endif
 
 
