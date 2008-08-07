#ifndef TCCLUSTER_H
#define TCCLUSTER_H

#include"TVector3.h"
#include"TObject.h"

#include "TCalign.h"
#include "../../PndTpcCluster.h"


class TCcluster : public TObject{

 public:
  TCcluster(TVector3 p,TVector3 e,double a,int id);
  TCcluster(const PndTpcCluster& _c,int id);
  TCcluster(const PndTpcDigi& _d,int id);
  TCcluster();


  unsigned int nPadX();
  unsigned int nPadY();

  void setResid(double *par);
  TVector3 getResid(double *par);
  double getChi2(double *par);

  int getNDF();

  int getId(){return detId;}
  void setId(int _i){detId=_i;}

  void posUVW(TVector3& v){
    pos=v;
  }
  void posUVW(double u,double v,double w){
    pos.SetXYZ(u,v,w);
  }
  TVector3 posUVW(){return pos;}
  TVector3 posXYZ(){return TCalign::getInstance()->UVWtoXYZ(detId,pos);}

  void print();
  void setFit(bool _b=true){fit=_b;}
  bool getFit(){return fit;}
  TVector3 getRes(){return res;}
  TVector3 getErr(){return err;}
  void setErr(TVector3 v){
    err=v;
  }
  void setErr(double ue,double ve,double we){
    err.SetXYZ(ue,ve,we);
  }
  unsigned int nRaw(){
    return raw.size();
  }

  void addRaw(const TCcluster& _c){
    raw.push_back (_c);
  }

  TCcluster getRaw(int i){
    assert(i<nRaw());
    return raw.at(i);
  }

  void setAmp(double _d){
    amp=_d;
  }
  double getAmp(){
    return amp;
  }
  double getPedestalRMS(){
    return pedestalRMS;
  }

  void setPedestalRMS(double d){
    pedestalRMS=d;
  }

 private:
  int detId;
  TVector3 pos,err,res;
  double amp;
  bool fit;
  double pedestalRMS;//for trivial clusters
  std::vector<TCcluster> raw;

  TVector3 residStrip(const TVector3& p, const TVector3& d);
  TVector3 residPixel(const TVector3& p, const TVector3& d);
  TVector3 residPoint(const TVector3& p, const TVector3& d);

  void convertPar(double* par,TVector3& p, TVector3& d);//return by reference
 public:
  ClassDef(TCcluster,1)
};

//I had this as a normal constructor, but then rootcint segmented :-(
TCcluster cogTCcluster(std::vector<TCcluster>& _raw);


#endif
