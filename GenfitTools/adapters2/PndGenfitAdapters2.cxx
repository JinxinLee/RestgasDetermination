//modified by Elisabetta Prencipe, 19/05/2014

#include"PndGenfitAdapters2.h"

#include <iostream>

#include"PndTrack.h"
#include"PndTrackCand.h"
#include"FairTrackParP.h"

#include"AbsTrackRep.h"
#include"DetPlane.h"
#include"Exception.h"
#include"RKTrackRep.h"
#include"SharedPlanePtr.h"
#include"Track.h"
#include"TrackCand.h"

#include <TMatrixD.h>
#include <TMatrixDSym.h>

#include <cmath>

PndTrackCand* Genfit2TrackCand2PndTrackCand(const genfit::TrackCand* cand){
  PndTrackCand* retVal = new PndTrackCand();
  const unsigned nhits = cand->getNHits();
  int detId,hitId;
  double rho;
  for(unsigned int i=0;i<nhits;++i){
    cand->getHit(i,detId,hitId,rho);//
    retVal->AddHit(detId,hitId,rho);//
  }
  
  retVal->setMcTrackId(cand->getMcTrackId());
  return retVal;
}

genfit::TrackCand* PndTrackCand2Genfit2TrackCand(PndTrackCand* cand) {
  genfit::TrackCand* retVal = new genfit::TrackCand();
  unsigned int nhits = cand->GetNHits();
  for(unsigned int i=0;i<nhits;++i){
    PndTrackCandHit candHit = cand->GetSortedHit(i);
    retVal->addHit(candHit.GetDetId(),candHit.GetHitId(),i,candHit.GetRho());
  }
  
  retVal->setMcTrackId(cand->getMcTrackId());
  //double q = cand->getQoverPseed() > 0 ? 1 : -1; // assume single charged particle
  //double p = fabs(cand->getQoverPseed()) > 1E-10 ? q/cand->getQoverPseed() : q*1E10;
  //retVal->setPosMomSeed(cand->getPosSeed(),
  //			cand->getDirSeed()*p,q);
  std::cerr << "*** PndGenfitAdapters::PndTrackCand2Genfit2TrackCand" << "\t" << "PndTrackCand does not store any Seed!!! Faulty COnversion***" << std::endl;			
  return retVal;
}

PndTrack* Genfit2Track2PndTrack(const genfit::Track* tr){

  if (dynamic_cast<genfit::RKTrackRep*>(tr->getCardinalRep())==NULL) {
    std::cerr << " GenfitAbsTrackRep2PndTrack() can currently only handle RKTrackRep" << std::endl;
    throw;
  }

  const TVectorD& firstState = tr->getFittedState().getState();
  const TVectorD& lastState = tr->getFittedState(-1).getState();
  const TMatrixDSym& firstCov = tr->getFittedState().getCov();
  const TMatrixDSym& lastCov = tr->getFittedState(-1).getCov();
  genfit::SharedPlanePtr firstPlane = tr->getFittedState().getPlane();
  genfit::SharedPlanePtr lastPlane = tr->getFittedState(-1).getPlane();

  //make the FairTrackParP for first and last hit
  double firstCova[15];
  int count=0;
  for(int i=0; i<5;++i){
     for(int j=i;j<5;++j){
	   firstCova[count++]=firstCov(i,j);
     }
  }
  double lastCova[15];
  count=0;
  for(int i=0; i<5;++i){
    for(int j=i;j<5;++j){
	  lastCova[count++]=lastCov(i,j);
    }
  }

  double first_spu = (tr->getFittedState().getAuxInfo())(0);
  double last_spu = (tr->getFittedState(-1).getAuxInfo())(0);
    
  FairTrackParP first(firstState[3],firstState[4],firstState[1],firstState[2],firstState[0],firstCova,firstPlane->getO(),firstPlane->getU(),firstPlane->getV(),first_spu);
  FairTrackParP last(lastState[3],lastState[4],lastState[1],lastState[2],lastState[0],lastCova,lastPlane->getO(),lastPlane->getU(),lastPlane->getV(),last_spu);
    
  //copy the trackCand
  genfit::TrackCand* genfitCand = tr->constructTrackCand();
  PndTrackCand* pndCand = Genfit2TrackCand2PndTrackCand(genfitCand);
  PndTrack* retVal =  new PndTrack(first,last,*pndCand);
  retVal->SetChi2(tr->getFitStatus()->getChi2());
  retVal->SetNDF(tr->getFitStatus()->getNdf());
  if (tr->getFitStatus()->isFitConverged()) {
    retVal->SetFlag(1);
  }
  else {
    retVal->SetFlag(-1);
  }
  delete genfitCand;
  delete pndCand;
  return retVal;
  
}
