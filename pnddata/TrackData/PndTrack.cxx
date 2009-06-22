/*
 * PndTrack.cpp
 *
 *  Created on: 05.03.2009
 *      Author: stockman
 */

#include "PndTrack.h"

ClassImp(PndTrack);

PndTrack::PndTrack()
{
}


PndTrack::PndTrack(const FairTrackParP& first, const FairTrackParP& last, const PndTrackCand& cand):
	fTrackParamFirst(first), fTrackParamLast(last), fTrackCand(cand)
{
}



void PndTrack::Print(){
  std::cout << "FirstTrackPar" << std::endl;
  fTrackParamFirst.Print();
  std::cout << "LastTrackPar" << std::endl;
  fTrackParamLast.Print();
  std::cout << "chi2 " << fChi2 << std::endl;
  std::cout << "ndf " << fNDF << std::endl;
}
