//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcPlanarRecoHit
//      see PndTpcPlanarRecoHit.hh for details
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
#include "PndTpcPlanarRecoHit.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------
#include "PndTpcCluster.h"
#include "LSLTrackRep.h"
#include "GFDetPlane.h"

// Class Member definitions -----------

ClassImp(PndTpcPlanarRecoHit)


PndTpcPlanarRecoHit::~PndTpcPlanarRecoHit()
{}

PndTpcPlanarRecoHit::PndTpcPlanarRecoHit()
  : PlanarRecoHit(NparHitRep),_cluster_size(0),_cluster_amp(0)
{}

PndTpcPlanarRecoHit::PndTpcPlanarRecoHit(double x, double y, double z,
			   double sigx, double sigy)
  : PlanarRecoHit(NparHitRep),_cluster_size(0),_cluster_amp(0)
{
  fHitCoord[0][0] = x;
  fHitCoord[1][0] = y;
  fPolicy.setDetPlane(GFDetPlane(TVector3(0,0,z),
		       TVector3(1,0,0),
		       TVector3(0,1,0)));
  fHitCov[0][0] = sigx*sigx;
  fHitCov[1][1] = sigy*sigy;
}

PndTpcPlanarRecoHit::PndTpcPlanarRecoHit(const TVector3& pos,
			   double sigx, double sigy)
  : PlanarRecoHit(NparHitRep),_cluster_size(0),_cluster_amp(0)
{
  fHitCoord[0][0] = pos.X();
  fHitCoord[1][0] = pos.Y();
  fPolicy.setDetPlane(GFDetPlane(TVector3(0,0,pos.Z()),
		       TVector3(1,0,0),
		       TVector3(0,1,0)));
  fHitCov[0][0] = sigx*sigx;
  fHitCov[1][1] = sigy*sigy;
}

PndTpcPlanarRecoHit::PndTpcPlanarRecoHit(PndTpcCluster* cl)
  : PlanarRecoHit(NparHitRep)
{
  fHitCoord[0][0] = cl->pos().X();
  fHitCoord[1][0] = cl->pos().Y();
  fPolicy.setDetPlane(GFDetPlane(TVector3(0,0,cl->pos().Z()),
		       TVector3(1,0,0),
		       TVector3(0,1,0)));
  double sigx=cl->sig().X();
  double sigy=cl->sig().Y();
  fHitCov[0][0] = sigx*sigx;
  fHitCov[1][1] = sigy*sigy;
  _cluster_size = cl->size();
  _cluster_amp  = cl->amp();
}


GFAbsRecoHit* 
PndTpcPlanarRecoHit::clone(){
  return new PndTpcPlanarRecoHit(*this);
}


TMatrixT<double>
PndTpcPlanarRecoHit::getHMatrix(const GFAbsTrackRep* stateVector)
{
 if (dynamic_cast<const LSLTrackRep*>(stateVector) != NULL) {
   TMatrixT<double> HMatrix(NparHitRep,5);

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
    return HMatrix;
  }
  else {
    std::cerr << "PndTpcPlanarRecoHit can only handle state"
              << " vectors of type LSLTrackRep -> abort" << std::endl;
    throw;
  }
 
}



