#include "Kalman.h"

#include "assert.h"
#include <iostream>
#include <sstream>

#include "TMath.h"

#include "Track.h"
#include "AbsRecoHit.h"
#include "AbsTrackRep.h"
#include "FitterExceptions.h"
  
#define COVEXC "cov_is_zero"


Kalman::Kalman():_lazy(0),_initialDirection(1),_numIt(3),_blowUpFactor(20.),_outlierCut(-1.){;}

Kalman::~Kalman(){;}

void Kalman::processTrack(Track* trk){
  int direction=_initialDirection;
  assert(direction==1 || direction==-1);
  trk->clearBookkeeping();
  trk->clearRepAtHit();
  /*why is there a factor of two here (in the for statement)?
    Because we consider one full iteration to be one back and
    one forth fitting pass */
  for(int i=0; i<2*_numIt; i++){
    _fitPassCounter=i;
    if(i>0) blowUpCovs(trk);
    if(direction==1){
      trk->setNextHitToFit(0);
    }
    else {
      trk->setNextHitToFit(trk->getNumHits()-1);
    }
    fittingPass(trk,direction);
    
    //save first and last plane,state&cov after the fitting pass
    if(direction==1){//forward at last hit
      int nreps=trk->getNumReps();
      for(int i=0; i<nreps; ++i){
	trk->getTrackRep(i)->
	  setLastPlane( trk->getTrackRep(i)->getReferencePlane() );
	trk->getTrackRep(i)->
	  setLastState( trk->getTrackRep(i)->getState() );
	trk->getTrackRep(i)->
	  setLastCov( trk->getTrackRep(i)->getCov() );
      }
    }
    else{//backward at first hit
      int nreps=trk->getNumReps();
      for(int i=0; i<nreps; ++i){
	trk->getTrackRep(i)->
	  setFirstPlane( trk->getTrackRep(i)->getReferencePlane() );
	trk->getTrackRep(i)->
	  setFirstState( trk->getTrackRep(i)->getState() );
	trk->getTrackRep(i)->
	  setFirstCov( trk->getTrackRep(i)->getCov() );
      }
    }

    //switch direction of fitting and also inside all the reps
    if(direction==1) direction=-1;
    else direction=1;
    switchDirection(trk);
  }
  
  return;
}

void
Kalman::switchDirection(Track* trk){
  int nreps=trk->getNumReps();
  for(int i=0; i<nreps; ++i){
    trk->getTrackRep(i)->switchDirection();
  }
}

void Kalman::blowUpCovs(Track* trk){
  int nreps=trk->getNumReps();
  for(int i=0; i<nreps; ++i){
    AbsTrackRep* arep=trk->getTrackRep(i);
    //dont do it for already compromsied reps, since they wont be fitted anyway
    if(arep->getStatusFlag()==0) { 
      TMatrixT<double> cov = arep->getCov();
      for(int i=0;i<cov.GetNrows();++i){
	for(int j=0;j<cov.GetNcols();++j){
	  if(i!=j){//off diagonal
	    cov[i][j]=0.;
	  }
	  else{//diagonal
	    cov[i][j] = cov[i][j] * _blowUpFactor;
	  }
	}
      }
      arep->setCov(cov);
    }
  }  
}

void
Kalman::fittingPass(Track* trk, int direction){
  //loop over hits
  unsigned int nhits=trk->getNumHits();
  unsigned int starthit=trk->getNextHitToFit();
  if(starthit==nhits && direction >0) {
    std::cout<<"Kalman::processTrack::Already at end of Track!"<<std::endl;
    return;
  }  if(starthit==-1 && direction <0) {
    std::cout<<"Kalman::processTrack(backward)::Already at start of Track!"<<std::endl;
    return;
  }

  int nreps=trk->getNumReps();
  int ihit=(int)starthit;
  
  //clear chi2 sum and ndf sum in track reps
  for(int i=0;i<nreps;++i){
    AbsTrackRep* arep=trk->getTrackRep(i);
    arep->setChiSqu(0.);
    arep->setNDF(0);
  }

  //clear failedHits and outliers
  trk->clearBookkeeping();

  while((ihit<nhits && direction==1) || (ihit>-1 && direction==-1)){
    AbsRecoHit* ahit=trk->getHit(ihit);
    // loop over reps
    for(int irep=0; irep<nreps; ++irep){
	  AbsTrackRep* arep=trk->getTrackRep(irep);
	  if(arep->getStatusFlag()==0) { 
	    try {
	      bool rejOutliers=false;
	      if(_outlierCut>0. && _fitPassCounter>1){
		rejOutliers=true;
	      }
	      bool passedOutlier = processHit(trk,ihit,irep,rejOutliers);
	      if(!passedOutlier){
		/* trackrep state,cov,refPlane have not been
		 * updated, do bookkeeping
		 */
		trk->addOutlier(irep,ihit);
	      }
	    }
	    catch(FitterException& e) {
	      trk->addFailedHit(irep,ihit);
	      std::cerr << e.what() << std::endl;
	      e.info();
	      
	      if(e.isFatal()) {
		arep->setStatusFlag(1);
		continue; // go to next rep immediately
	      }
	    }	
	  }
    }// end loop over reps
    ihit+=direction;
  }// end loop over hits
  trk->setNextHitToFit(ihit-2*direction);
  //trk->printBookkeeping();
}

double Kalman::chi2Increment(const TMatrixT<double>& r,const TMatrixT<double>& H,
			     const TMatrixT<double>& cov,const TMatrixT<double>& V){

  // residuals covariances:R=(V - HCH^T)
  TMatrixT<double> R(V);
  TMatrixT<double> covsum1(cov,TMatrixT<double>::kMultTranspose,H);
  TMatrixT<double> covsum(H,TMatrixT<double>::kMult,covsum1);

  R-=covsum;

  // chisq= r^TR^(-1)r
  double det=0.;
  TMatrixT<double> Rsave(R);
  R.Invert(&det);
  TMatrixT<double> residTranspose(r);
  residTranspose.T();
  TMatrixT<double> chisq=residTranspose*(R*r);
  assert(chisq.GetNoElements()==1);

  if(TMath::IsNaN(chisq[0][0])){
	FitterException exc("chi2 is nan",__LINE__,__FILE__);
	exc.setFatal();
	std::vector<double> numbers;
	numbers.push_back(det);
	exc.setNumbers("det",numbers);
	std::vector< TMatrixT<double> > matrices;
	matrices.push_back(r);
	matrices.push_back(V);
	matrices.push_back(Rsave);
	matrices.push_back(R);
	matrices.push_back(cov);
	exc.setMatrices("r, V, Rsave, R, cov",matrices);
    throw exc;
  }

  return chisq[0][0];
}


double
Kalman::getChi2Hit(AbsRecoHit* hit, AbsTrackRep* rep)
{
  // get prototypes for matrices
  int repDim=rep->getDim();
  TMatrixT<double> state(repDim,1);
  TMatrixT<double> cov(repDim,repDim);;
  DetPlane pl=hit->getDetPlane(rep);
  rep->extrapolate(pl,state,cov);
  hit->setHMatrix(rep,state);

  TMatrixT<double> H=hit->getHMatrix();
  // get hit covariances  
  TMatrixT<double> V=hit->getHitCov(pl);

  TMatrixT<double> r=hit->residualVector(rep,state,pl);
  assert(r.GetNrows()>0);

  //this is where chi2 is calculated
  double chi2 = chi2Increment(r,H,cov,V);

  return chi2/r.GetNrows();
}

  
bool
Kalman::processHit(Track* tr, int ihit, int irep, bool rejectOutlier){
  AbsRecoHit* hit = tr->getHit(ihit);
  AbsTrackRep* rep = tr->getTrackRep(irep);

  // get prototypes for matrices
  int repDim=rep->getDim();
  TMatrixT<double> state(repDim,1);
  TMatrixT<double> cov(repDim,repDim);;
  DetPlane pl;
  /* do an extrapolation, if the trackrep irep is not given
   * at this ihit position. This will usually be the case, but
   * not is the fit turnes around
   */
  if(ihit!=tr->getRepAtHit(irep)){
    // get the (virtual) detector plane
    pl=hit->getDetPlane(rep);
    //do the extrapolation
    rep->extrapolate(pl,state,cov);
  }
  else{
    pl = rep->getReferencePlane();
    state = rep->getState();
    cov = rep->getCov();
  }
  
  if(cov[0][0]<1.E-50){
    FitterException exc(COVEXC,__LINE__,__FILE__);
    throw exc;
  }
  

  TMatrixT<double> origcov=rep->getCov();

  //set and get H matrix  
  hit->setHMatrix(rep,state);

  TMatrixT<double> H=hit->getHMatrix();

  // get hit covariances  
  TMatrixT<double> V=hit->getHitCov(pl);

  // calculate kalman gain ------------------------------
  TMatrixT<double> Gain(calcGain(cov,V,H));

  TMatrixT<double> res=hit->residualVector(rep,state,pl);

  // calculate update -----------------------------------
  TMatrixT<double> update=Gain*res;
  
  state+=update; // prediction overwritten!
  cov-=Gain*(H*cov);
  
  // calculate filtered chisq
  // filtered residual
  TMatrixT<double> r=hit->residualVector(rep,state,pl);

  double chi2 = chi2Increment(r,H,cov,V);
  int ndf = r.GetNrows();

  if(rejectOutlier){
    if(chi2/ndf > _outlierCut){
      return false;
    }
  }

  rep->addChiSqu( chi2 );
  rep->addNDF( ndf );

  // if we survive until here: update TrackRep
  rep->setState(state);
  rep->setCov(cov);
  rep->setReferencePlane(pl);
  tr->setRepAtHit(irep,ihit);

  return true;
}


TMatrixT<double>
Kalman::calcGain(const TMatrixT<double>& cov, 
					 const TMatrixT<double>& HitCov,
					 const TMatrixT<double>& H){

  // calculate covsum (V + HCH^T)
  TMatrixT<double> covsum1(cov,TMatrixT<double>::kMultTranspose,H);
  TMatrixT<double> covsum(H,TMatrixT<double>::kMult,covsum1);

  covsum+=HitCov;
  
  // invert
  double det=0;
  covsum.Invert(&det);
  if(TMath::IsNaN(det)) {
    FitterException e("Kalman Gain: det of covum is nan",__LINE__,__FILE__);
    e.setFatal();
    throw e;
  }
  if(det==0){
	FitterException exc("cannot invert covsum in Kalman Gain - det=0",
						__LINE__,__FILE__);
	exc.setFatal();
	std::vector< TMatrixT<double> > matrices;
	matrices.push_back(cov);
	matrices.push_back(HitCov);
	matrices.push_back(covsum1);
	matrices.push_back(covsum);
	exc.setMatrices("cov, HitCov, covsum1 and covsum",matrices);
    throw exc;

  }
  // calculate gain
  TMatrixT<double> gain1(H,TMatrixT<double>::kTransposeMult,covsum);
  TMatrixT<double> gain(cov,TMatrixT<double>::kMult,gain1);

  return gain;
}







