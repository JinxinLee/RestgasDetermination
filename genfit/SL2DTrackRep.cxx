#include "SL2DTrackRep.h"
#include "AbsRecoHit.h"

SL2DTrackRep::SL2DTrackRep() : AbsTrackRep(NparTrackRep) {
}

SL2DTrackRep::SL2DTrackRep(double ax,double bx, double ay, double by, double sigax, double sigbx, double sigay, double sigby) : AbsTrackRep(NparTrackRep)
{
  state[0][0]=ax;
  state[1][0]=bx;
  state[2][0]=ay;
  state[3][0]=by;
  cov[0][0]=sigax*sigax;
  cov[1][1]=sigbx*sigbx;
  cov[2][2]=sigby*sigby;
  cov[3][3]=sigby*sigby;

  startState = state;
  startCov = cov;

}

SL2DTrackRep::~SL2DTrackRep() {
}

void SL2DTrackRep::predict(AbsRecoHit* hit, 
						   TMatrixT<double>& statePred,
						   TMatrixT<double>& covPred, 
						   TMatrixT<double>& jacobian, 
						   double& sPred) {
  jacobian.ResizeTo(0,0);
  jacobian.ResizeTo(4,4);
  jacobian[0][0] = 1.;
  jacobian[1][1] = 1.;
  jacobian[2][2] = 1.;
  jacobian[3][3] = 1.;

  statePred = state;
  covPred = cov;
  sPred = hit->getS();
}

void SL2DTrackRep::extrapolate(double sExtrapolateTo, TMatrixT<double>& statePred) {
    statePred = state;
}

void SL2DTrackRep::extrapolate(double sExtrapolateFrom, 
							 double sExtrapolateTo, 
							 const TMatrixT<double>& stateFrom, 
							 TMatrixT<double>& stateResult) {
  stateResult = stateFrom;
}


void SL2DTrackRep::extrapolate(double sExtrapolateTo,
							   TMatrixT<double>& statePred,
							   TMatrixT<double>& covPred,
							   TMatrixT<double>& jacobian) {
  jacobian.ResizeTo(0,0);
  jacobian.ResizeTo(4,4);
  jacobian[0][0] = 1.;
  jacobian[1][1] = 1.;
  jacobian[2][2] = 1.;
  jacobian[3][3] = 1.;

  statePred = state;
  covPred = cov;
}
