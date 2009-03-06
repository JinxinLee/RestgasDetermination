/*  Copyright © 2008, Christian Hoeppner & Sebastian Neubert
    All rights reserved.

    This file is part of Genfit.

    Genfit is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Genfit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser Public License for more details.

    You should have received a copy of the GNU Lesser Public License
    along with Genfit.  If not, see <http://www.gnu.org/licenses/>. */

//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class TrackCand
//      see TrackCand.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "TrackCand.h"

// C/C++ Headers ----------------------
#include <algorithm>

// Collaborating Class Headers --------


// Class Member definitions -----------

ClassImp(TrackCand)

TrackCand::TrackCand():_curv(0),_dip(0),_inv(false){}

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
