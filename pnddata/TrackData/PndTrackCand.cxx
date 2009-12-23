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
//      Tobias Stockmanns (IKP - Juelich) during the Panda Meeting 03/09
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTrackCand.h"

#include <algorithm>


ClassImp(PndTrackCand);

PndTrackCand::PndTrackCand():sorted(false), fMcTrackId(-1),fQoverPseed(0.){}

PndTrackCand::~PndTrackCand(){}

void
PndTrackCand::AddHit(UInt_t detId, UInt_t hitId, Double_t rho)
{
	fHitId.push_back(PndTrackCandHit(detId, hitId, rho));
	sorted = false;
	//Int_t det = detId;
	//Int_t hit = hitId;
	//AddLink(det, hit);
}


void PndTrackCand::Reset()
{
  fHitId.clear();
}

int PndTrackCand::HitInTrack(UInt_t detId, UInt_t hitId)
{
	PndTrackCandHit test(detId, hitId, 0.);
	for (UInt_t i = 0; i < fHitId.size(); i++){
		if(fHitId[i] == test)
			return i;
	}
	return -1;
}

void PndTrackCand::DeleteHit(UInt_t detId, UInt_t hitId)
{
	int ind = HitInTrack(detId, hitId);
	fHitId.erase(fHitId.begin()+ind);

	//Int_t det = detId;
	//Int_t hit = hitId;
	//DeleteLink(det, hit);
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


void PndTrackCand::Print(){
  std::cout << "=========== PndTrackCand::Print() ==========" << std::endl;
  if(fMcTrackId>=0) std::cout << "McTrackId=" << fMcTrackId << std::endl;
  std::cout << "seed values for pos,direction, and q/p: " << std::endl;
  fPosSeed.Print();
  fDirSeed.Print();
  std::cout << "q/p=" << fQoverPseed << std::endl;
  for(unsigned int i=0;i<fHitId.size();++i){
    fHitId.at(i).Print();
  }
}
