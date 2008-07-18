#include "GEANEhelixTrackRep.h"
#include "AbsRecoHit.h"

#include<cmath>

GEANEhelixTrackRep::GEANEhelixTrackRep() : AbsTrackRep(5) {
}

GEANEhelixTrackRep::GEANEhelixTrackRep(const CbmTrackParH& par)
  : AbsTrackRep(5)
{
  state[4][0]=par.GetQp();
  state[2][0]=par.GetLambda();
  state[3][0]=par.GetPhi();
  state[0][0]=par.GetX_sc();
  state[1][0]=par.GetY_sc();
}

GEANEhelixTrackRep::~GEANEhelixTrackRep(){}

void GEANEhelixTrackRep::init(const TVector3& startpoint, 
							  const TVector3& startmomentum, 
							  const TVector3& locBField,
							  const int charge) {
  //example of best way to intialize helix from given values
  //can use totally different things if you like
}

void GEANEhelixTrackRep::predict(const DetPlane& plane,
		       TMatrixT<double>& statePred, 
		       TMatrixT<double>& covPred,  
		       TMatrixT<double>& jacobian){
  //find value of free parameter the next hit is found at,
  //probably have to calculate minimum of disance

  //sPred = value;
  //extrapolate(sPred,statePred,covPred,jacobian);
}



void GEANEhelixTrackRep::extrapolate(const DetPlane& plane, 
				     TMatrixT<double>& statePred){
  //put code here that predicts the state vector stateResult at value of s 
  //sExtrapolateTo given the stateVector stateFrom at sExtrapolateFrom
}

void GEANEhelixTrackRep::extrapolate(const DetPlane& plane,
                           TMatrixT<double>& statePred,
                           TMatrixT<double>& covPred,
                           TMatrixT<double>& jacobian){
  //use the other extrapolate prototype for predictions of state
  //s holds the curent value of the free parameter
  //and state the current value of the state vector
  
}


 

ClassImp(GEANEhelixTrackRep)
