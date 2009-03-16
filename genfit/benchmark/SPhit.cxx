// This Class' Header ------------------
#include "SPhit.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------
#include "GeaneTrackRep.h"
#include "DetPlane.h"
#include "TRandom.h"

// Class Member definitions -----------

ClassImp(SPhit)


SPhit::~SPhit()
{}

SPhit::SPhit()
  : SpacepointRecoHit(NparHitRep)
{}

SPhit::SPhit(double x, double y, double z,
			   double sigx, double sigy,double sigz)
  : SpacepointRecoHit(NparHitRep)
{
  _hitCoord[0][0] = gRandom->Gaus(x,sigx);
  _hitCoord[1][0] = gRandom->Gaus(y,sigy);
  _hitCoord[2][0] = gRandom->Gaus(z,sigz);
  _hitCov[0][0] = sigx*sigx;
  _hitCov[1][1] = sigy*sigy;
  _hitCov[2][2] = sigz*sigz;
}

SPhit::SPhit(const TVector3& pos,
			 const TVector3& err)
  : SpacepointRecoHit(NparHitRep)
{
  _hitCoord[0][0] = gRandom->Gaus(pos.X(),err.X());
  _hitCoord[1][0] = gRandom->Gaus(pos.Y(),err.Y());
  _hitCoord[2][0] = gRandom->Gaus(pos.Z(),err.Z());
  _hitCov[0][0] = err.X()*err.X();
  _hitCov[1][1] = err.Y()*err.Y();
  _hitCov[2][2] = err.Z()*err.Z();
}

AbsRecoHit* 
SPhit::clone(){
  return new SPhit(*this);
}


void 
SPhit::setHMatrix(const AbsTrackRep* stateVector,
			 const TMatrixT<double>& state)
{
 if (dynamic_cast<const GeaneTrackRep*>(stateVector) != NULL) {
    //I know, since this is the same everytime, it could be done in the
    //the constructor, but I do it here anyway, to make clear that in the
    //case of several track-reps per hit, it would have to be done here
   //    _HMatrix.ResizeTo(NparHitRep,5);
    _HMatrix.ResizeTo(2,5);

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
    std::cerr << "SPhit can only handle state"
              << " vectors of type GeaneTrackRep -> abort" << std::endl;
    throw;
  }
 
}


