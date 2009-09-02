#include"PndGenfitAdapters.h"

#include"Track.h"
#include"AbsTrackRep.h"
#include"TrackCand.h"
#include"PndTrack.h"
#include"PndTrackCand.h"
#include"DetPlane.h"
#include"TMatrixT.h"
#include"FairTrackParP.h"

#include"GeaneTrackRep.h"

PndTrackCand* GenfitTrackCand2PndTrackCand(const TrackCand* cand){
  PndTrackCand* retVal = new PndTrackCand();
  unsigned int nhits = cand->getNHits();
  unsigned detId,hitId;
  double rho;
  for(unsigned int i=0;i<nhits;++i){
    cand->getHit(i,detId,hitId,rho);
    retVal->AddHit(detId,hitId,rho);
  }
  retVal->setMcTrackId(cand->getMcTrackId());
  retVal->setTrackSeed(cand->getPosSeed(),
		       cand->getDirSeed(),
		       cand->getQoverPseed());
  return retVal;
}

TrackCand* PndTrackCand2GenfitTrackCand(PndTrackCand* cand){
  TrackCand* retVal = new TrackCand();
  unsigned int nhits = cand->GetNHits();
  for(unsigned int i=0;i<nhits;++i){
    PndTrackCandHit candHit = cand->GetSortedHit(i);
    retVal->addHit(candHit.GetDetId(),candHit.GetHitId(),candHit.GetRho());
  }
  retVal->setMcTrackId(cand->getMcTrackId());
  retVal->setTrackSeed(cand->getPosSeed(),
		       cand->getDirSeed(),
		       cand->getQoverPseed());
  return retVal;
}

PndTrack* GenfitTrack2PndTrack(const Track* tr){
  AbsTrackRep* clone = tr->getCardinalRep()->clone();
  TMatrixT<double> firstState = clone->getFirstState();
  TMatrixT<double> lastState = clone->getLastState();
  TMatrixT<double> firstCov = clone->getFirstCov();
  TMatrixT<double> lastCov = clone->getLastCov();
  DetPlane firstPlane = clone->getFirstPlane();
  DetPlane lastPlane = clone->getLastPlane();
  GeaneTrackRep* gtr = dynamic_cast<GeaneTrackRep*>(clone);
  if (gtr != NULL) {//is GeaneTrackRep
    //make the FairTrackParP for first and last hit
    double firstCova[15];
    int count=0;;
    for(int i=0; i<5;++i){
      for(int j=i;j<5;++j){
	firstCova[count++]=firstCov[i][j];
      }
    }
    double lastCova[15];
    count=0;;
    for(int i=0; i<5;++i){
      for(int j=i;j<5;++j){
	lastCova[count++]=lastCov[i][j];
      }
    }
    double spu = gtr->getSPU();
    FairTrackParP first(firstState[3][0],firstState[4][0],firstState[1][0],firstState[2][0],firstState[0][0],firstCova,firstPlane.getO(),firstPlane.getU(),firstPlane.getV(),spu);
    FairTrackParP last(lastState[3][0],lastState[4][0],lastState[1][0],lastState[2][0],lastState[0][0],lastCova,lastPlane.getO(),lastPlane.getU(),lastPlane.getV(),spu);
    
    //copy the trackCand
    TrackCand genfitCand = tr->getCand();
    PndTrackCand* pndCand = GenfitTrackCand2PndTrackCand(&genfitCand);
    PndTrack* retVal =  new PndTrack(first,last,*pndCand);
    retVal->SetChi2(tr->getChiSqu());
    retVal->SetNDF(tr->getNDF());
    delete pndCand;
    return retVal;
  }
  else {
    std::cerr << " GenfitAbsTrackRep2PndTrack() can currently only handle GeaneTrackRep" << std::endl;
    throw;
  }
}
