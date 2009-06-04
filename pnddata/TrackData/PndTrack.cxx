/*
 * PndTrack.cpp
 *
 *  Created on: 05.03.2009
 *      Author: stockman
 */

#include "PndTrack.h"

ClassImp(PndTrack);

PndTrack::PndTrack() :
  fTrackParamFirst(NULL), fTrackParamLast(NULL), fTrackCand(NULL)
{
}


PndTrack::PndTrack(FairTrackPar* first, FairTrackPar* last, PndTrackCand* cand):
	fTrackParamFirst(first), fTrackParamLast(last), fTrackCand(cand)
{
}


PndTrack::~PndTrack() {
  if(fTrackParamFirst!=NULL) delete fTrackParamFirst;
  if(fTrackParamLast!=NULL) delete fTrackParamLast;
  if(fTrackCand!=NULL) delete fTrackCand;
}

void PndTrack::Print(){
  std::cout << "FirstTrackPar" << std::endl;
  fTrackParamFirst->Print();
  std::cout << "LastTrackPar" << std::endl;
  fTrackParamLast->Print();
  std::cout << "chi2 " << fChi2 << std::endl;
  std::cout << "ndf " << fNDF << std::endl;
}
