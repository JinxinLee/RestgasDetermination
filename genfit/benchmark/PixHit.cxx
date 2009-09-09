// This Class' Header ------------------
#include "PixHit.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------
#include "RKtrackRep.h"
#include "DetPlane.h"
#include "TRandom.h"

// Class Member definitions -----------

ClassImp(PixHit)


PixHit::~PixHit()
{}

PixHit::PixHit()
  : PlanarRecoHit(NparHitRep)
{}

PixHit::PixHit(TVector3 point,double res)
  : PlanarRecoHit(NparHitRep){

  _hitCov[0][0] = res*res;
  _hitCov[1][1] = res*res;
  DetPlane d;
  static TVector3 xUnit(1.,0.,0.);
  static TVector3 yUnit(0.,1.,0.);
  d.setO(TVector3(0.,0.,point.Z()));
  d.setU(xUnit);
  d.setV(yUnit);
  _hitCoord[0][0] = gRandom->Gaus(point.X(),res);
  _hitCoord[1][0] = gRandom->Gaus(point.Y(),res);
  setDetPlane(d);

}
AbsRecoHit* 
PixHit::clone(){
  return new PixHit(*this);
}


void 
PixHit::setHMatrix(const AbsTrackRep* stateVector,
			 const TMatrixT<double>& state)
{
 if (dynamic_cast<const RKtrackRep*>(stateVector) != NULL) {
    //I know, since this is the same everytime, it could be done in the
    //the constructor, but I do it here anyway, to make clear that in the
    //case of several track-reps per hit, it would have to be done here
   //    _HMatrix.ResizeTo(NparHitRep,5);
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
    std::cerr << "PixHit can only handle state"
              << " vectors of type GeaneTrackRep -> abort" << std::endl;
    throw;
  }
 
}


