// DataClass to store monte carlo tracklet infos
#ifndef PNDTPCMCTRACKLET_HH
#define PNDTPCMCTRACKLET_HH


#include "TObject.h"
#include "McId.h"
#include "TVector3.h"
#include <vector>

class PndTpcMCTracklet : public TObject {
 public:
  PndTpcMCTracklet():fq(0),fpdg(0),ft0(-9999),fID(19999,0),fV0res(9999),fstatus(0),fzpoca(999,999,999){};
  ~PndTpcMCTracklet(){};
  PndTpcMCTracklet(const TVector3& pos,
		   const TVector3& mom,
		   const double& q,
		   const int& pdg,
		   const Int_t trackid,
		   const Int_t evtid) : fpos(pos),fmom(mom),
                                        fq(q),fpdg(pdg),
    fID(evtid,trackid),ft0(-9999),fV0res(9999),fstatus(0),fzpoca(999,999,999)
    {}
  
  const McId& mcid() const {return fID;}
  const TVector3& mom() const {return fmom;}
  const TVector3& pos() const {return fpos;}
  const Double_t& q() const {return fq;}
  const Int_t& pdg() const {return fpdg;}
  Int_t nmvd() const {return fmvdhits.size();}
  Int_t nmvdAcc(Double_t cut);
  Int_t nemc() const {return femchits.size();}
  Double_t V0res() const {return fV0res;}
  Int_t status() const {return fstatus;}
  const Double_t& t0() const {return ft0;}
  const TVector3& zpoca() const {return fzpoca;}

  void setMvdHits(const std::vector<double>& v){fmvdhits=v;}
  void setEmcHits(const std::vector<double>& v){femchits=v;}
  void setT0(Double_t t){ft0=t;}
  void setV0Res(Double_t r){fV0res=r;}
  void setStatus(Int_t s){fstatus=s;}
  void setZPoca(const TVector3& x){fzpoca=x;}

 private:
  McId fID;
  TVector3 fmom;
  TVector3 fpos;
  Double_t fq;
  Int_t fpdg;
  std::vector<double> fmvdhits; // residuals to mvd hits
  std::vector<double> femchits; // residuals to emc hits
  Double_t ft0;    // reconstructed t0 for this event
  Double_t fV0res; // residual for V0 vertex
  Int_t fstatus;
  TVector3 fzpoca; // poca to z-axis

 public:
  ClassDef(PndTpcMCTracklet,5)
};


// sorting functor -> sort Tracklets into voxels
class PndTpcMCTrackletSort {
 public:
  PndTpcMCTrackletSort(){}
  ~PndTpcMCTrackletSort(){}

  bool operator() (PndTpcMCTracklet* t1, PndTpcMCTracklet* t2);

};





#endif
