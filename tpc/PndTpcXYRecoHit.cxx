//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcXYRecoHit
//      see PndTpcXYRecoHit.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcXYRecoHit.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------
#include "PndTpcCluster.h"
#include "LSLTrackRep.h"

// Class Member definitions -----------

ClassImp(PndTpcXYRecoHit)


PndTpcXYRecoHit::~PndTpcXYRecoHit()
{}

PndTpcXYRecoHit::PndTpcXYRecoHit()
  : AbsRecoHit(NparHitRep)
{}

PndTpcXYRecoHit::PndTpcXYRecoHit(double x, double y, double z,
			   double sigx, double sigy)
  : AbsRecoHit(NparHitRep)
{
  hitCoord[0][0] = x;
  hitCoord[1][0] = y;
  sVal=z;
  hitCov[0][0] = sigx*sigx;
  hitCov[1][1] = sigy*sigy;
}

PndTpcXYRecoHit::PndTpcXYRecoHit(const TVector3& pos,
			   double sigx, double sigy)
  : AbsRecoHit(NparHitRep)
{
  hitCoord[0][0] = pos.X();
  hitCoord[1][0] = pos.Y();
  sVal=pos.Z();
  hitCov[0][0] = sigx*sigx;
  hitCov[1][1] = sigy*sigy;
}

PndTpcXYRecoHit::PndTpcXYRecoHit(PndTpcCluster* cl)
  : AbsRecoHit(NparHitRep)
{
  hitCoord[0][0] = cl->pos().X();
  hitCoord[1][0] = cl->pos().Y();
  sVal=cl->pos().Z();
  double sigx=cl->sig().X();
  double sigy=cl->sig().Y();
  hitCov[0][0] = sigx*sigx;
  hitCov[1][1] = sigy*sigy;
}


AbsRecoHit* 
PndTpcXYRecoHit::clone(){
  return new PndTpcXYRecoHit(*this);
}


void 
PndTpcXYRecoHit::setHMatrix(double s, const AbsTrackRep* stateVector,
			 const TMatrixT<double>& state)
{
 if (dynamic_cast<const LSLTrackRep*>(stateVector) != NULL) {
    //I know, since this is the same everytime, it could be done in the
    //the constructor, but I do it here anyway, to make clear that in the
    //case of several track-reps per hit, it would have to be done here
    HMatrix.ResizeTo(NparHitRep,5);

    HMatrix[0][0] = 1.;
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
              << " vectors of type LSLTrackRep -> abort" << std::endl;
    throw;
  }
 
}

double 
PndTpcXYRecoHit::residualScalar(double s,
			     const AbsTrackRep* stateVector,
			     const TMatrixT<double>& state)
{}

