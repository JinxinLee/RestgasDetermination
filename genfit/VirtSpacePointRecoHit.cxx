//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class VirtSpacePointRecoHit
//      see VirtSpacePointRecoHit.hh for details
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
#include "VirtSpacePointRecoHit.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------
#include "LSLTrackRep.h"

// Class Member definitions -----------

ClassImp(VirtSpacePointRecoHit)

VirtSpacePointRecoHit::~VirtSpacePointRecoHit()
{}

VirtSpacePointRecoHit::VirtSpacePointRecoHit()
: SpacepointRecoHit(NparHitRep)
{}

VirtSpacePointRecoHit::VirtSpacePointRecoHit(double x, double y, double z)
  : SpacepointRecoHit(NparHitRep)
{
  setPos(TVector3(x,y,z));
  _hitCoord[0][0] = x;
  _hitCoord[1][0] = y;
  _hitCoord[2][0] = z;
  _hitCov.UnitMatrix();
}

VirtSpacePointRecoHit::VirtSpacePointRecoHit(const TVector3& pos)
  : SpacepointRecoHit(NparHitRep)
{
 setPos(pos);
  _hitCoord[0][0] = pos.X();
  _hitCoord[1][0] = pos.Y();
  _hitCoord[2][0] = pos.Z();
  _hitCov.UnitMatrix();
}

AbsRecoHit*
VirtSpacePointRecoHit::clone()
{ 
  return new VirtSpacePointRecoHit(*this);
}


void 
VirtSpacePointRecoHit::setHMatrix(const AbsTrackRep* stateVector,
	   const TMatrixT<double>& state)
{
  if (dynamic_cast<const LSLTrackRep*>(stateVector) != NULL) {
    _HMatrix.ResizeTo(NparHitRep,5);

    _HMatrix[0][0] = 1.;
    _HMatrix[0][1] = 0.;
    _HMatrix[0][2] = 0.;
    _HMatrix[0][3] = 0.;
    _HMatrix[0][4] = 0.;

    _HMatrix[1][0] = 0.;
    _HMatrix[1][1] = 1.;
    _HMatrix[1][2] = 0.;
    _HMatrix[1][3] = 0.;
    _HMatrix[1][4] = 0.;

    _HMatrix[2][0] = 0.;
    _HMatrix[2][1] = 0.;
    _HMatrix[2][2] = 0.;
    _HMatrix[2][3] = 0.;
    _HMatrix[2][4] = 0.;
  }
  else {
    std::cerr << "VirtSpacePointRecoHit can only handle state"
              << " vectors of type LSLTrackRep -> abort" << std::endl;
    throw;
  }

}


double 
VirtSpacePointRecoHit::residualScalar(AbsTrackRep* stateVector,
	       const TMatrixT<double>& state)
{
  TMatrixT<double> resv=residualVector(stateVector,state);
  return resv[0][0]*resv[0][0]+resv[1][0]*resv[1][0]+resv[2][0]*resv[2][0];
}
