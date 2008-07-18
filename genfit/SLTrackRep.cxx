#include "SLTrackRep.h"
#include "AbsRecoHit.h"

SLTrackRep::SLTrackRep() : AbsTrackRep(NparTrackRep) {
}

SLTrackRep::SLTrackRep(double a, double b, 
			       double siga, double sigb) 
  : AbsTrackRep(NparTrackRep) 
{
  state[0][0]=a;
  state[1][0]=b;
  cov[0][0]=siga*siga;
  cov[1][1]=sigb*sigb;

  startState[0][0]=a;
  startState[1][0]=b;
  startCov[0][0]=siga*siga;
  startCov[1][1]=sigb*sigb;


}


SLTrackRep::~SLTrackRep() {
}

void SLTrackRep::predict(AbsRecoHit* hit, 
						 TMatrixT<double>& statePred,
						 TMatrixT<double>& covPred, 
						 TMatrixT<double>& jacobian, 
						 double& sPred) {
  jacobian.ResizeTo(0,0);
  jacobian.ResizeTo(2,2);
  jacobian[0][0] = 1.;
  jacobian[1][1] = 1.;
  statePred = state;
  covPred = cov;
  sPred = hit->getS();
}

void SLTrackRep::extrapolate(double sExtrapolateTo, TMatrixT<double>& statePred) {
    statePred = state;
}

void SLTrackRep::extrapolate(double sExtrapolateFrom, 
							 double sExtrapolateTo, 
							 const TMatrixT<double>& stateFrom, 
							 TMatrixT<double>& stateResult) {
  stateResult = stateFrom;
}

void SLTrackRep::extrapolate(double sExtrapolateTo, 
							 TMatrixT<double>& statePred,
							 TMatrixT<double>& covPred,
							 TMatrixT<double>& jacobian){
  jacobian.ResizeTo(0,0);
  jacobian.ResizeTo(2,2);
  jacobian[0][0] = 1.;
  jacobian[1][1] = 1.;
  statePred = state;
  covPred = cov;
}


ClassImp(SLTrackRep)
