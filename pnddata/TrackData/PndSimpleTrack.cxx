/*
 * PndSimpleTrack.cpp
 *
 *  Created on: 05.03.2009
 *      Author: stockman
 */

#include "PndSimpleTrack.h"

ClassImp(PndSimpleTrack);

PndSimpleTrack::PndSimpleTrack(): fChi2(0)
{
}


PndSimpleTrack::PndSimpleTrack(const FairTrackPar& param, const PndTrackCand& cand, const double& chi2):
  fTrackParam(param), fTrackCand(cand), fChi2(chi2)
{

	SetTimeStamp(cand.GetTimeStamp());
	SetTimeStampError(cand.GetTimeStampError());
}



void PndSimpleTrack::Print(){
  std::cout << "TrackParam" << std::endl;
  fTrackParam.Print();
}
