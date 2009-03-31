#include "TrackCand.h"

#include <algorithm>

ClassImp(TrackCand)

TrackCand::TrackCand():_curv(0),_dip(0),_inv(false), _dDip(0.), _dCurv(0.){}

TrackCand::~TrackCand(){}

TrackCand::TrackCand(double curv, double dip, double inv, std::vector<unsigned int> detIDs, std::vector<unsigned int> hitIDs)
  : _detId(detIDs),_hitId(hitIDs),_curv(curv), _dip(dip), _inv(inv)
{}

void 
TrackCand::addHit(unsigned int detId, unsigned int hitId)
{
  _detId.push_back(detId);
  _hitId.push_back(hitId);
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
