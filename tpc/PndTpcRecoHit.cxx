#include "PndTpcRecoHit.h"
#include "AbsTrackRep.h"
#include "AliceHelixTrackRep.h"
#include "LSLTrackRep.h"

#include "TVector3.h"

/*
double PndTpcRecoHit::vecToScalar(const TMatrixT<double>& vec, double s) {
  double x_meas, y_meas, z_meas;
  double x_state, y_state, z_state;

  return sqrt(1.);
  //  return vec[0][0]*vec[0][0]+vec[1][0]*vec[1][0];
}
*/

double PndTpcRecoHit::residualScalar(double s, 
							  const AbsTrackRep* stateVector,
							  const TMatrixT<double>& state){
  
  double x_meas, y_meas, z_meas;
  double x_state, y_state, z_state;
  x_meas = sVal * cos(hitCoord[0][0]/sVal);
  y_meas = sVal * sin(hitCoord[0][0]/sVal);
  z_meas = hitCoord[1][0];
  x_state = s * cos( state[0][0] );
  y_state = s * sin( state[0][0] );
  z_state = state[1][0];
  return sqrt( (x_meas-x_state)*(x_meas-x_state) +
			   (y_meas-y_state)*(y_meas-y_state) +
			   (z_meas-z_state)*(z_meas-z_state) );
}


PndTpcRecoHit::~PndTpcRecoHit() {}

PndTpcRecoHit::PndTpcRecoHit() : AbsRecoHit(NparHitRep) {}

PndTpcRecoHit::PndTpcRecoHit(PndTpcPoint* thePoint) : AbsRecoHit(NparHitRep) {
  TVector3 pos;
  thePoint->Position(pos);
  setHit(pos.Pt(),//r
		 pos.Phi(),//phi
		 pos.Z(),//phi
		 0.1,//sig_rphi
		 0.1,//sig_z
		 0.);//amplitude
}

PndTpcRecoHit::PndTpcRecoHit(PndTpcDigi* theDigi) : AbsRecoHit(NparHitRep) {
  TVector3 pos;
  PndTpcDigiMapper* mapper = PndTpcDigiMapper::getInstance();
  mapper->map(theDigi,pos);
  setHit(pos.Pt(),//r
		 pos.Phi(),//phi
		 pos.Z(),//phi
		 0.1,//sig_rphi
		 0.5,//sig_z
		 0.);//amplitude
}

PndTpcRecoHit::PndTpcRecoHit(CbmTstPoint* thePoint) : AbsRecoHit(NparHitRep) {
  TVector3 pos;
  thePoint->Position(pos);
  setHit(pos.Pt(),//r
		 pos.Phi(),//phi
		 pos.Z(),//phi
		 0.1,//sig_rphi
		 0.1,//sig_z
		 0.);//amplitude
}


PndTpcRecoHit::PndTpcRecoHit(double r, double phi, double z, double sig_rphi, double sig_z, double amplitude) : AbsRecoHit(NparHitRep){
  setHit(r,phi,z,sig_rphi,sig_z,amplitude);
}

void PndTpcRecoHit::setHit(double r, double phi, double z, double sig_rphi, double sig_z, double amplitude) {
  hitCoord[0][0] = r*phi;
  hitCoord[1][0] = z;
  hitCov[0][0] = sig_rphi*sig_rphi;
  hitCov[1][1] = sig_z*sig_z;
  sVal = r;
  amp = amplitude;
}


void PndTpcRecoHit::setHMatrix(double s,  
			    const AbsTrackRep* stateVector,
			    const TMatrixT<double>& state) 
{
  if(dynamic_cast<const AliceHelixTrackRep*>(stateVector) != NULL) {    
    //I know, since this is the same everytime, it could be done in the
    //the constructor, but I do it here anyway, to make clear that in the
    //case of several track-reps per hit, it would have to be done here
    HMatrix.ResizeTo(NparHitRep,5);
	
    HMatrix[0][0] = s;
    HMatrix[0][1] = 0.;
    HMatrix[0][2] = 0.;
    HMatrix[0][3] = 0.;
    HMatrix[0][4] = 0.;
    
    HMatrix[1][0] = 0.;
    HMatrix[1][1] = 1.;
    HMatrix[1][2] = 0.;
    HMatrix[1][3] = 0.;
    HMatrix[1][4] = 0.;
  }
  else if (dynamic_cast<const LSLTrackRep*>(stateVector) != NULL) {    
    //I know, since this is the same everytime, it could be done in the
    //the constructor, but I do it here anyway, to make clear that in the
    //case of several track-reps per hit, it would have to be done here
    HMatrix.ResizeTo(NparHitRep,5);
	
    HMatrix[0][0] = s;
    HMatrix[0][1] = 0.;
    HMatrix[0][2] = 0.;
    HMatrix[0][3] = 0.;
    HMatrix[0][4] = 0.;
    
    HMatrix[1][0] = 0.;
    HMatrix[1][1] = 1.;
    HMatrix[1][2] = 0.;
    HMatrix[1][3] = 0.;
    HMatrix[1][4] = 0.;
  }
  else {
    std::cerr << "PndTpcRecoHit can only handle state"
	      << " vectors of type AliceHelixTrackRep -> abort" << std::endl;
    throw;
  }
}

TMatrixT<double> 
PndTpcRecoHit::getHitCov(double s,AbsTrackRep* const trkRep) const
{
  if(dynamic_cast<const LSLTrackRep*>(trkRep) != NULL){
    // LSLTrackRep has a local coordinate system => we have to transform there!
    return TMatrixT<double>(hitCov);
  }
  else return hitCov;
}
  
TMatrixT<double> 
PndTpcRecoHit::getHitCoord(double s,AbsTrackRep* const trkRep) const
{
  if(dynamic_cast<const LSLTrackRep*>(trkRep) != NULL){
    // LSLTrackRep has a local coordinate system => we have to transform there!
    TVector3 mom=trkRep->getMom(s);
    mom.Print();
    return TMatrixT<double>(hitCoord);
  }
  else return hitCoord;
}


