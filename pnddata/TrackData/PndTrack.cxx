/*
 * PndTrack.cpp
 *
 *  Created on: 05.03.2009
 *      Author: stockman
 */

#include "PndTrack.h"

PndTrack::PndTrack() {
}


PndTrack::PndTrack(FairTrackPar& first, FairTrackPar& last, PndTrackCand& cand):
	fTrackParamFirst(first), fTrackParamLast(last), fTrackCand(cand)
{
}

PndTrack::~PndTrack() {
}
