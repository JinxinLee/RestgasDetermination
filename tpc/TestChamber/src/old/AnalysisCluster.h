#ifndef ANALYSISCLUSTER_H
#define ANALYSISCLUSTER_H

#include<iostream>

#include "TObject.h"
#include "TVector3.h"

class AnalysisCluster : public TObject {
 public:
  AnalysisCluster();
  virtual ~AnalysisCluster();

  void pos(const TVector3& p) {_pos=p;}
  void pos(double x,double y,double z) {_pos.SetXYZ(x,y,z);}
  TVector3& pos() {return _pos;}
  void err(const TVector3& e) {_err=e;}
  void err(double x,double y,double z) {_err.SetXYZ(x,y,z);}
  TVector3& err() {return _err;}
  void resid(const TVector3& r) {_resid=r;}
  void resid(double x,double y,double z) {_resid.SetXYZ(x,y,z);}
  TVector3& resid() {return _resid;}

  void setClustersizes(const int& size,const int& npads, const int& npadsX, const int& npadsY) {
	_clustersize=size;_nPadCluster=npads,_nPadClusterX=npadsX,_nPadClusterY=npadsY;
  }
  
  void getClustersizes(int& size,int& npads,int& npadsX,int& npadsY) {
	size=_clustersize;npads=_nPadCluster;npadsX=_nPadClusterX;npadsY=_nPadClusterY;
  }

  int clustersize() {return _clustersize;}
  int nPadCluster() {return _nPadCluster;}
  int nPadClusterX() {return _nPadClusterX;}
  int nPadClusterY() {return _nPadClusterY;}
  double amp() {return _amp;}
  void amp(double a) {_amp=a;}

  void print() {
	std::cout << "Position:" << std::endl; _pos.Print();
	std::cout << "Errors:" << std::endl; _err.Print();
	std::cout << "Residual:" << std::endl; _resid.Print();
	std::cout << "Clustersizes: all pulses|nPads|nPadsX|nPadsY" << std::endl;
	std::cout << _clustersize << "|" << _nPadCluster << "|" << _nPadClusterX << "|" << _nPadClusterY << std::endl;
	std::cout << "Amplitude: " << _amp << std::endl;
  }

  void clear() {
	_clustersize=0;_nPadCluster=0;_nPadClusterX=0;_nPadClusterY=0;
	_pos.SetXYZ(0.,0.,0.);
	_err.SetXYZ(0.,0.,0.);
	_resid.SetXYZ(0.,0.,0.);
  }

 private:
  TVector3 _pos,_err,_resid;
  int _clustersize,_nPadCluster,_nPadClusterX,_nPadClusterY;
  double _amp;

 public:
  ClassDef(AnalysisCluster,2)
};


#endif
 
 
