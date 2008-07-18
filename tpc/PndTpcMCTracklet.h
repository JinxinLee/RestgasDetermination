// DataClass to store monte carlo tracklet infos
#ifndef PNDTPCMCTRACKLET_HH
#define PNDTPCMCTRACKLET_HH


#include "TObject.h"
#include "McId.h"
#include "TVector3.h"
#include <vector>

class PndTpcMCTracklet : public TObject {
 public:
  PndTpcMCTracklet():_q(0),_pdg(0),_t0(-9999),_ID(19999,0),_V0res(9999),_status(0),_zpoca(999,999,999){};
  ~PndTpcMCTracklet(){};
  PndTpcMCTracklet(const TVector3& pos,
		   const TVector3& mom,
		   const double& q,
		   const int& pdg,
		   const Int_t trackid,
		   const Int_t evtid) : _pos(pos),_mom(mom),
                                        _q(q),_pdg(pdg),
    _ID(evtid,trackid),_t0(-9999),_V0res(9999),_status(0),_zpoca(999,999,999)
    {}
  
  const McId& mcid() const {return _ID;}
  const TVector3& mom() const {return _mom;}
  const TVector3& pos() const {return _pos;}
  const Double_t& q() const {return _q;}
  const Int_t& pdg() const {return _pdg;}
  Int_t nmvd() const {return _mvdhits.size();}
  Int_t nmvdAcc(Double_t cut);
  Int_t nemc() const {return _emchits.size();}
  Double_t V0res() const {return _V0res;}
  Int_t status() const {return _status;}
  const Double_t& t0() const {return _t0;}
  const TVector3& zpoca() const {return _zpoca;}

  void setMvdHits(const std::vector<double>& v){_mvdhits=v;}
  void setEmcHits(const std::vector<double>& v){_emchits=v;}
  void setT0(Double_t t){_t0=t;}
  void setV0Res(Double_t r){_V0res=r;}
  void setStatus(Int_t s){_status=s;}
  void setZPoca(const TVector3& x){_zpoca=x;}

 private:
  McId _ID;
  TVector3 _mom;
  TVector3 _pos;
  Double_t _q;
  Int_t _pdg;
  std::vector<double> _mvdhits; // residuals to mvd hits
  std::vector<double> _emchits; // residuals to emc hits
  Double_t _t0;    // reconstructed t0 for this event
  Double_t _V0res; // residual for V0 vertex
  Int_t _status;
  TVector3 _zpoca; // poca to z-axis

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
