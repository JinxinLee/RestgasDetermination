/*  Copyright © 2008, Christian Hoeppner & Sebastian Neubert
    All rights reserved.

    This file is part of Genfit.

    Genfit is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Genfit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser Public License for more details.

    You should have received a copy of the GNU Lesser Public License
    along with Genfit.  If not, see <http://www.gnu.org/licenses/>. */

#include "Kalman.h"

#include "assert.h"
#include <iostream>
#include <sstream>

#include "TMath.h"

#include "Track.h"
#include "AbsRecoHit.h"
#include "AbsTrackRep.h"
#include "FitParams.h"
#include "FitterExceptions.h"
  
 Kalman::Kalman():_lazy(0),_numIt(3){;}
  
Kalman::~Kalman(){;}

void
Kalman::processTrack(Track* trk){
 
  for(int i=0; i<_numIt; i++){
    // first we do the "normal" propagation
    if(i == 0) trk->setNextHitToFit(0);
    else trk->setNextHitToFit(1);
    continueTrack(trk,1);
 //    //the we do backtracking
    switchDirection(trk);
    trk->setNextHitToFit(trk->getNumHits()-2);
    continueTrack(trk,-1);
    switchDirection(trk);
  }
}

void
Kalman::switchDirection(Track* trk){
  int nreps=trk->getNumReps();
  for(int i=0; i<nreps; ++i){
    trk->getTrackRep(i)->switchDirection();
  }
}

void
Kalman::continueTrack(Track* trk, int direction){
  //loop over hits
  //std::cout<<"Kalman::processTrack::Starting track"<<std::endl;
  unsigned int nhits=trk->getNumHits();
  unsigned int starthit=trk->getNextHitToFit();
  if(starthit==nhits && direction >0) {
    std::cout<<"Kalman::processTrack::Already at end of Track!"<<std::endl;
    return;
  }
  if(starthit==-1 && direction <0) {
    std::cout<<"Kalman::processTrack(backward)::Already at start of Track!"<<std::endl;
    return;
  }

  int nreps=trk->getNumReps();
  int ihit=(int)starthit;
  
  while((ihit<nhits && direction==1) || (ihit>-1 && direction==-1)){
    //std::cout<<"Hit#"<<ihit<<std::endl;
    AbsRecoHit* ahit=trk->getHit(ihit);
    // loop over reps
    for(int irep=0; irep<nreps; ++irep){
	  AbsTrackRep* arep=trk->getTrackRep(irep);
	  if(arep->getStatusFlag()==0) { 
		try {
		  //std::cout<<".";
		  processHit(ahit,arep,ihit);
		}
		catch(FitterException& e) {
		  std::cout << e.what() << std::endl;
		  if(!_lazy){
		    e.info();
		    arep->setStatusFlag(1);
		    continue; // go to next rep immediately
		  }
		}	
	  }
    }// end loop over reps
    ihit+=direction;
  }// end loop over reps;
  trk->setNextHitToFit(ihit-2*direction);
}


double
Kalman::getChi2Hit(AbsRecoHit* hit, AbsTrackRep* rep)
{
  // get prototypes for matrices
  int repDim=rep->getDim();
  TMatrixT<double> state(repDim,1);
  TMatrixT<double> cov(repDim,repDim);;
  TMatrixT<double> jacobian(repDim,repDim);
  DetPlane pl=hit->getDetPlane(rep);
  rep->predict(pl,state,cov,jacobian);
  hit->setHMatrix(rep,state);
  //hit->setHMatrix(s,pred,);
  TMatrixT<double> H=hit->getHMatrix();
  // get hit covariances  
  TMatrixT<double> V=hit->getHitCov(pl);
  TMatrixT<double> r=hit->residualVector(rep,state);
  // residuals covariances:R=(V - HCH^T)
  TMatrixT<double> R(V);
  TMatrixT<double> covsum1(cov,TMatrixT<double>::kMultTranspose,H);
  TMatrixT<double> covsum(H,TMatrixT<double>::kMult,covsum1);

  R+=covsum; // note minus sign!

  // chisq= r^TR^(-1)r
  double det=0;
  TMatrixT<double> Rsave(R);
  R.Invert(&det);
  if(TMath::IsNaN(det))std::cout<<"predicted residual: det nan!"<<std::endl;
  TMatrixT<double> chisq=r.T()*(R*r); // note: .T() will change r!
  assert(chisq.GetNoElements()==1);
  return chisq[0][0];
}


  
void
Kalman::processHit(AbsRecoHit* hit, AbsTrackRep* rep,int hitIndex){

  //get fitParamsObject from trackRep in order to fill statePre,stateFilt,
  //covPred,covFilt for this hit
  FitParams* params =  rep->getFitParams();

  // make prediction ------------------------------------
  // get prototypes for matrices
  int repDim=rep->getDim();
  TMatrixT<double> state(repDim,1);
  TMatrixT<double> cov(repDim,repDim);;
  TMatrixT<double> jacobian(repDim,repDim);
  //double s=0;

  //   rep->getState().Print();
  //rep->getCov().Print();
  //hit->getHitCoord(s,rep).Print();
  //hit->getHitCov(s,rep).Print();
  //std::cout << hit->getS() << std::endl;  
 
  // get the virtual detector plane
  DetPlane pl=hit->getDetPlane(rep);
  
  //pl.Print();
  
  // let the rep do the prediction
  //std::cout<<"++++++++++++++ do prediction: ++++++++++++++++"<<std::endl;
  rep->predict(pl,state,cov,jacobian);
  //state.Print();
  //cov.Print();
  
  if(cov[0][0]<1.E-50){
    FitterException exc("cov[0][0]<1.-50",__LINE__,__FILE__);
    throw exc;
  }
  
  TMatrixT<double> origcov=rep->getCov();
  
  for(int i=0; i<5; ++i){
    for(int j=0;j<5; ++j){
      if(cov[i][j]*origcov[i][j]<0){
	//std::cout<<"AT HIT#"<<hitIndex<<" COV ELEMENT "<<i<<","
	//	 <<j<<" CHANGED ITS SIGN!"<<std::endl;
	//cov[i][j]=-cov[i][j];
      }
    }
  }
  
  
  //std::cout<<"++++++++++++++++++++++++++++++++++++++++++++++"<<std::endl;
  
  params->addfStatePred(hitIndex,state);
  params->addfCovPred(hitIndex,cov);
  params->addfJacobian(hitIndex,jacobian);
  // create a predicted trackrep
  //  AbsTrackRep* pred=rep->prototype();
  //  pred->setState(state); 
  // pred->setCov(cov);
  //pred->setS(s);
  
  // get H Matrix at prediction
  hit->setHMatrix(rep,state);
  //hit->setHMatrix(s,pred,);
  TMatrixT<double> H=hit->getHMatrix();
  // get hit covariances  
  TMatrixT<double> V=hit->getHitCov(pl);
  
  // calculate kalman gain ------------------------------
  TMatrixT<double> Gain(gain(cov,V,H));
  TMatrixT<double> res=hit->residualVector(rep,state);
  // calculate update -----------------------------------
  TMatrixT<double> update=Gain*res;
  
  state+=update; // prediction overwritten!
  cov-=Gain*(H*cov);
  
  params->addfStateFilt(hitIndex,state);
  params->addfCovFilt(hitIndex,cov);

  // calculate filtered chisq
  // filtered residual
  TMatrixT<double> r=hit->residualVector(rep,state);
  // residuals covariances:R=(V - HCH^T)
  TMatrixT<double> R(V);

  TMatrixT<double> covsum1(cov,TMatrixT<double>::kMultTranspose,H);
  TMatrixT<double> covsum(H,TMatrixT<double>::kMult,covsum1);

  R+=covsum; // note minus sign!

  // chisq= r^TR^(-1)r
  double det=0;
  TMatrixT<double> Rsave(R);
  R.Invert(&det);
  if(TMath::IsNaN(det))std::cout<<"filtered residual: det nan!"<<std::endl;
  TMatrixT<double> chisq=r.T()*(R*r); // note: .T() will change r!
  assert(chisq.GetNoElements()==1);
  //  cov.Print();
  //std::cout << "chi2 incr: " << chisq[0][0] << std::endl;
  rep->addChiSqu(chisq[0][0]);
  if(TMath::IsNaN(chisq[0][0])){
	FitterException exc("chi2 is nan",__LINE__,__FILE__);
	std::vector<double> numbers;
	numbers.push_back(det);
	exc.setNumbers("det",numbers);
	std::vector< TMatrixT<double> > matrices;
	matrices.push_back(r);
	matrices.push_back(V);
	matrices.push_back(Rsave);
	matrices.push_back(R);
	matrices.push_back(state);
	matrices.push_back(cov);
	matrices.push_back(Gain);
	exc.setMatrices("r, V, Rsave, R, state, cov and Gain",matrices);
    throw exc;
  }

  // if we survive until here: update TrackRep
  rep->setState(state);
  rep->setCov(cov);
  rep->setReferencePlane(pl);
  // No throwing beyond this point!!!!!!!!!!!!!!!!!!!!
}


TMatrixT<double>
Kalman::gain(const TMatrixT<double>& cov, 
					 const TMatrixT<double>& HitCov,
					 const TMatrixT<double>& H){

// calculate covsum (V + HCH^T)
  TMatrixT<double> covsum1(cov,TMatrixT<double>::kMultTranspose,H);
  TMatrixT<double> covsum(H,TMatrixT<double>::kMult,covsum1);
  //TMatrixT<double> covsum=H*(cov*H.T());
  //std::cout<<"Covsum==";
  //covsum.Print();

  covsum+=HitCov;
  
  // invert
  double det=0;
  covsum.Invert(&det);
  if(TMath::IsNaN(det)) throw FitterException("Kalman Gain: det of covum is nan",__LINE__,__FILE__);
  if(det==0){
	FitterException exc("cannot invert covsum in Kalman Gain - det=0",
						__LINE__,__FILE__);
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

void Kalman::smoothing(Track* trk) {
  std::cout<<"Kalman::smoothing"<<std::endl;
  int nhits=trk->getNumHits();
  int nreps=trk->getNumReps();

  //for the last hit set the fin state and cov to the filt values
  for(int irep=0; irep<nreps; irep++){
	AbsTrackRep* rep=trk->getTrackRep(irep);
	FitParams* par = rep->getFitParams();
	TMatrixT<double> lastStateFilt;
	TMatrixT<double> lastCovFilt;
	par->getfStateFilt(nhits-1,lastStateFilt);
	par->getfCovFilt(nhits-1,lastCovFilt);
	par->addStateFin(nhits-1,lastStateFilt);
	par->addCovFin(nhits-1,lastCovFilt);
  }


  //go backwards over all the hit indices from n-1 to 0
  for(int k=nhits-2; k>=0; k--){
    for(int irep=0; irep<nreps; irep++){
      AbsTrackRep* rep=trk->getTrackRep(irep);
	  int repDim = rep->getDim();
	  FitParams* par = rep->getFitParams();

	  TMatrixT<double> p_k_k;
	  TMatrixT<double> p_kplus1_k;
	  TMatrixT<double> p_kplus1_n;
	  TMatrixT<double> C_k_k;
	  TMatrixT<double> C_kplus1_k;
	  TMatrixT<double> C_kplus1_n;
	  TMatrixT<double> F_kplus1;	  
	  par->getfStatePred(k+1,p_kplus1_k);
	  par->getfStateFilt(k,p_k_k);
	  par->getStateFin(k+1,p_kplus1_n);
	  par->getfCovPred(k+1,C_kplus1_k);
	  par->getfCovFilt(k,C_k_k);
	  par->getCovFin(k+1,C_kplus1_n);
	  par->getfJacobian(k+1,F_kplus1);

	  TMatrixT<double> C_kplus1_k_inv(C_kplus1_k);
	  double det = 0.;
// 	  std::cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << std::endl;
// 	  C_kplus1_k_inv.Print();
	  C_kplus1_k_inv.Invert(&det);
// 	  C_kplus1_k_inv.Print();
// 	  C_kplus1_k_inv.Invert(&det);
// 	  C_kplus1_k_inv.Print();
// 	  C_kplus1_k_inv.Invert(&det);
	  TMatrixT<double> A1(F_kplus1,
						  TMatrixT<double>::kTransposeMult,
						  C_kplus1_k_inv);
	  TMatrixT<double> A(C_k_k,TMatrixT<double>::kMult,A1);

	  TMatrixT<double> stateFin(p_k_k-A*
								(p_kplus1_k-p_kplus1_n)
								);
	  TMatrixT<double> covFin2( C_kplus1_k-C_kplus1_n,
								TMatrixT<double>::kMultTranspose,
								A );
	  TMatrixT<double> covFin( C_k_k-(A*covFin2) );
	  
	  par->addStateFin(k,stateFin);
	  par->addCovFin(k,covFin);
	 
    }
  }


}






