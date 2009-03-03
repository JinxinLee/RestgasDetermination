//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class TestRecoHit
//      see TestRecoHit.hh for details
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
#include "TestRecoHit.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------
#include "DetPlane.h"
#include "LSLTrackRep.h"

// Class Member definitions -----------

TestRecoHit::TestRecoHit(double u, double v, double z)
  : PlanarRecoHit(2)
{
  _hitCoord[0][0]=u;
  _hitCoord[1][0]=v;
  _hitCov[0][0]=0.1;
  _hitCov[1][1]=0.1;
  setDetPlane(DetPlane(TVector3(0,0,z),TVector3(1,0,0),TVector3(0,1,1)));
}

void 
TestRecoHit::setHMatrix(const AbsTrackRep* stateVector,
			const TMatrixT<double>& state)
{
  if (dynamic_cast<const LSLTrackRep*>(stateVector) != NULL) {
    //I know, since this is the same everytime, it could be done in the
    //the constructor, but I do it here anyway, to make clear that in the
    //case of several track-reps per hit, it would have to be done here
    _HMatrix.ResizeTo(2,5);

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
  }
  else {
    std::cerr << "TpcRecoHit can only handle state"
              << " vectors of type LSLTrackRep -> abort" << std::endl;
    throw;
  }

}




AbsRecoHit* 
TestRecoHit::clone()
{
  return new TestRecoHit(*this);
}
