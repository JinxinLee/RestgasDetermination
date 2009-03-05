//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTrackCand
//      see PndTrackCand.hh for details
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
#include "PndTrackCand.h"

// C/C++ Headers ----------------------
#include <algorithm>

// Collaborating Class Headers --------


// Class Member definitions -----------

ClassImp(PndTrackCand);

PndTrackCand::PndTrackCand(){}

PndTrackCand::~PndTrackCand(){}

void
PndTrackCand::AddHit(unsigned int detId, unsigned int hitId)
{
  fDetId.AddAt(detId, fDetId.GetSize());
  fHitId.AddAt(hitId, fHitId.GetSize());
}

TArrayI
PndTrackCand::GetHitIDs(int detId){
  if(detId<0){ // return hits from all detectors
    return fHitId;
  }
  else {
    TArrayI result;
    for(int i=0;i<fHitId.GetSize();++i){
      if(fDetId[i]==detId)result.AddAt(fHitId[i], result.GetSize());
    }
    return result;
  }
}

void
PndTrackCand::Reset()
{
  fDetId.Reset();
  fHitId.Reset();
}

bool PndTrackCand::HitInTrack(unsigned int detId, unsigned int hitId)
{
	for (int i = 0; i < fDetId.GetSize(); ++i){
		if (detId == fDetId[i])
			if (hitId == fHitId[i])
				return true;
	}
	return false;
}

bool operator== (const PndTrackCand& lhs, const PndTrackCand& rhs){
  if(lhs.GetNHits()!=rhs.GetNHits()) return false;
  for (int i = 0; i < lhs.GetNHits(); ++i){
	  if (lhs.GetHitId(i) != rhs.GetHitId(i) ||
		  lhs.GetDetId(i) != rhs.GetDetId(i))
		  return false;
  }
  return true;
}
