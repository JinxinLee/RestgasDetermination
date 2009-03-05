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


ClassImp(PndTrackCand);

PndTrackCand::PndTrackCand(){}

PndTrackCand::~PndTrackCand(){}

void
PndTrackCand::AddHit(unsigned int detId, unsigned int hitId, Double_t rho)
{
	fHitId.insert(std::pair<Double_t, std::pair<Int_t, Int_t> >(rho, std::pair<Int_t, Int_t>(detId, hitId)));
}


void PndTrackCand::Reset()
{
  fHitId.clear();
}

int PndTrackCand::HitInTrack(unsigned int detId, unsigned int hitId)
{
	mapIter iter(fHitId.begin());
	std::pair<Int_t, Int_t> hit(detId, hitId);
	int i = 0;
	while(iter != fHitId.end()){
		if (iter->second == hit)
			return i;
		iter++;
		i++;
	}
	return -1;
}

void PndTrackCand::DeleteHit(unsigned int detId, unsigned int hitId)
{
	int pos = HitInTrack(detId, hitId);
	mapIter iter(fHitId.begin());

	for (int i = 0; i < pos; i++) iter++;

	fHitId.erase(iter->first);
}

bool operator== (const PndTrackCand& lhs, const PndTrackCand& rhs){
	if(lhs == rhs)
		return true;
	return false;
}
