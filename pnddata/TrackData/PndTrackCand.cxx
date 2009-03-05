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
//      Tobias Stockmanns (IKP - Jülich) during the Panda Meeting 03/09
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTrackCand.h"

#include <algorithm>


ClassImp(PndTrackCand);

PndTrackCand::PndTrackCand():sorted(false){}

PndTrackCand::~PndTrackCand(){}

void
PndTrackCand::AddHit(unsigned int detId, unsigned int hitId, Double_t rho)
{
	fHitId.push_back(PndTrackCandHit(detId, hitId, rho));
	sorted = false;
}


void PndTrackCand::Reset()
{
  fHitId.clear();
}

int PndTrackCand::HitInTrack(unsigned int detId, unsigned int hitId)
{
	PndTrackCandHit test(detId, hitId, 0.);
	for (int i = 0; i < fHitId.size(); i++){
		if(fHitId[i] == test)
			return i;
	}
	return -1;
}

void PndTrackCand::DeleteHit(unsigned int detId, unsigned int hitId)
{
	int ind = HitInTrack(detId, hitId);
	fHitId.erase(fHitId.begin()+ind);
}

void PndTrackCand::Sort()
{
	std::sort(fHitId.begin(), fHitId.end());
	sorted = true;
}

std::vector<PndTrackCandHit> PndTrackCand::GetSortedHits()
{
	if (sorted == false)
		Sort();
	return fHitId;
}

bool operator== (const PndTrackCand& lhs, const PndTrackCand& rhs){
	if(lhs == rhs)
		return true;
	return false;
}
