//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndLheTpcPlanarRecoHit
//      see PndLheTpcPlanarRecoHit.hh for details
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
#include "PndLheTpcPlanarRecoHit.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------
#include "GeaneTrackRep.h"
#include "PndTpcCluster.h"
#include "LSLTrackRep.h"
#include "DetPlane.h"

// Class Member definitions -----------

ClassImp(PndLheTpcPlanarRecoHit)


PndLheTpcPlanarRecoHit::~PndLheTpcPlanarRecoHit()
{}

PndLheTpcPlanarRecoHit::PndLheTpcPlanarRecoHit()
  : PlanarRecoHit(NparHitRep),_cluster_size(0),_cluster_amp(0)
{}

PndLheTpcPlanarRecoHit::PndLheTpcPlanarRecoHit(double x, double y, double z,
			   double sigx, double sigy)
  : PlanarRecoHit(NparHitRep),_cluster_size(0),_cluster_amp(0)
{
  _hitCoord[0][0] = x;
  _hitCoord[1][0] = y;
  setDetPlane(DetPlane(TVector3(0,0,z),
		       TVector3(1,0,0),
		       TVector3(0,1,0)));
  _hitCov[0][0] = sigx*sigx;
  _hitCov[1][1] = sigy*sigy;
}

PndLheTpcPlanarRecoHit::PndLheTpcPlanarRecoHit(const TVector3& pos,
			   double sigx, double sigy)
  : PlanarRecoHit(NparHitRep),_cluster_size(0),_cluster_amp(0)
{
  _hitCoord[0][0] = pos.X();
  _hitCoord[1][0] = pos.Y();
  setDetPlane(DetPlane(TVector3(0,0,pos.Z()),
		       TVector3(1,0,0),
		       TVector3(0,1,0)));
  _hitCov[0][0] = sigx*sigx;
  _hitCov[1][1] = sigy*sigy;
}

PndLheTpcPlanarRecoHit::PndLheTpcPlanarRecoHit(PndTpcCluster* cl)
  : PlanarRecoHit(NparHitRep)
{
  _hitCoord[0][0] = cl->pos().X();
  _hitCoord[1][0] = cl->pos().Y();
  setDetPlane(DetPlane(TVector3(0,0,cl->pos().Z()),
		       TVector3(1,0,0),
		       TVector3(0,1,0)));
  double sigx=cl->sig().X();
  double sigy=cl->sig().Y();
  _hitCov[0][0] = sigx*sigx;
  _hitCov[1][1] = sigy*sigy;
  _cluster_size = cl->size();
  _cluster_amp  = cl->amp();
}


AbsRecoHit* 
PndLheTpcPlanarRecoHit::clone(){
  return new PndLheTpcPlanarRecoHit(*this);
}


void 
PndLheTpcPlanarRecoHit::setHMatrix(const AbsTrackRep* stateVector,
			 const TMatrixT<double>& state)
{
 if (dynamic_cast<const LSLTrackRep*>(stateVector) != NULL) {
    //I know, since this is the same everytime, it could be done in the
    //the constructor, but I do it here anyway, to make clear that in the
    //case of several track-reps per hit, it would have to be done here
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
 }
 else if (dynamic_cast<const GeaneTrackRep*>(stateVector) != NULL) {
    // Uses TrackParP (q/p,v',w',v,w)
    // coordinates are defined by detplane!
    _HMatrix.ResizeTo(NparHitRep,5);

    _HMatrix[0][0] = 0.;
    _HMatrix[0][1] = 0.;
    _HMatrix[0][2] = 0.;
    _HMatrix[0][3] = 1.;
    _HMatrix[0][4] = 0.;

    _HMatrix[1][0] = 0.;
    _HMatrix[1][1] = 0.;
    _HMatrix[1][2] = 0.;
    _HMatrix[1][3] = 0.;
    _HMatrix[1][4] = 1.;
  }

  else {
    std::cerr << "PndLheTpcPlanarRecoHit can only handle state"
              << " vectors of type LSLTrackRep or GeaneTrackRep -> abort" << std::endl;
    throw;
  }
 
}

double 
PndLheTpcPlanarRecoHit::residualScalar(AbsTrackRep* stateVector,
				 const TMatrixT<double>& state)
{
  throw;
}

