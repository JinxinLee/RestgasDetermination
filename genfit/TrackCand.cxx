#include "TrackCand.h"

#include <algorithm>
#include <iostream>

ClassImp(TrackCand)

TrackCand::TrackCand():_curv(0),_dip(0),_inv(false), _mcTrackId(-1),_qoverpSeed(0.){}

TrackCand::~TrackCand(){}

TrackCand::TrackCand(double curv, double dip, double inv, std::vector<unsigned int> detIDs, std::vector<unsigned int> hitIDs)
  : _detId(detIDs),_hitId(hitIDs),_curv(curv), _dip(dip), _inv(inv), _mcTrackId(-1),_qoverpSeed(0.)
{
  assert(_detId.size()==_hitId.size());
  _rho.resize(_detId.size(),0.);
}
TrackCand::TrackCand(double curv, double dip, double inv, std::vector<unsigned int> detIDs, std::vector<unsigned int> hitIDs,std::vector<double> rhos)
  : _detId(detIDs),_hitId(hitIDs),_curv(curv), _dip(dip), _inv(inv), _mcTrackId(-1),_qoverpSeed(0.),_rho(rhos)
{
  assert(_detId.size()==_hitId.size());
  assert(_detId.size()==_rho.size());
}

void 
TrackCand::addHit(unsigned int detId, unsigned int hitId, double rho)
{
  _detId.push_back(detId);
  _hitId.push_back(hitId);
  _rho.push_back(rho);
}

std::vector<unsigned int> 
TrackCand::GetHitIDs(int detId){
  if(detId<0){ // return hits from all detectors
    return _hitId;
  }
  else {
    std::vector<unsigned int> result;
    unsigned int n=_hitId.size();
    for(int i=0;i<n;++i){
      if(_detId[i]==detId)result.push_back(_hitId[i]);
    }
    return result;
  }
}

void
TrackCand::reset()
{
  _detId.clear();_hitId.clear();
}

bool TrackCand::HitInTrack(unsigned int detId, unsigned int hitId)
{
	for (int i = 0; i < _detId.size(); i++){
		if (detId == _detId[i])
			if (hitId == _hitId[i])
				return true;
	}
	return false;	
}

bool operator== (const TrackCand& lhs, const TrackCand& rhs){
  if(lhs.getNHits()!=rhs.getNHits()) return false;
  bool result=std::equal(lhs._detId.begin(),lhs._detId.end(),rhs._detId.begin());
  result &=std::equal(lhs._hitId.begin(),lhs._hitId.end(),rhs._hitId.begin());
  return result;
}

void TrackCand::print() const {
  std::cout << "======== TrackCand::print ========" << std::endl;
  if(_mcTrackId>=0) std::cout << "mcTrackId=" << _mcTrackId << std::endl;
  std::cout << "seed values for pos,direction, and q/p: " << std::endl;
  _posSeed.Print();
  _dirSeed.Print();
  std::cout << "q/p=" << _qoverpSeed << std::endl;
  assert(_detId.size()==_hitId.size());
  std::cout << "detId|hitId|rho ";
  for(unsigned int i=0;i<_detId.size();++i){
    std::cout << _detId.at(i) << "|" << _hitId.at(i) 
	      << "|" << _rho.at(i) << " ";
  }
  std::cout << std::endl;
}

void TrackCand::append(const TrackCand& rhs){
  unsigned int detId,hitId;
  double rho;
  for(unsigned int i=0;i<rhs.getNHits();++i){
    rhs.getHit(i,detId,hitId,rho);
    addHit(detId,hitId,rho);
  }


}
