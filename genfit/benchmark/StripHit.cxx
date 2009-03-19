// This Class' Header ------------------
#include "StripHit.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------
#include "GeaneTrackRep.h"
#include "DetPlane.h"
#include "TRandom.h"

// Class Member definitions -----------

ClassImp(StripHit)


StripHit::~StripHit()
{}

StripHit::StripHit()
  : PlanarRecoHit(NparHitRep)
{}

StripHit::StripHit(TVector3 point, int proj,double res)
  : PlanarRecoHit(NparHitRep){

  _hitCov[0][0] = res*res;
  DetPlane d;
  static TVector3 xUnit(1.,0.,0.);
  static TVector3 yUnit(0.,1.,0.);
  static TVector3 minxUnit(-1.,0.,0.);
  d.setO(TVector3(0.,0.,point.Z()));
  if((proj%2)==0){
    d.setU(xUnit);
    d.setV(yUnit);
    _hitCoord[0][0] = gRandom->Gaus(point.X(),res);
  }
  else{
    d.setU(yUnit);
    d.setV(minxUnit);
    _hitCoord[0][0] = gRandom->Gaus(point.Y(),res);
  }
  setDetPlane(d);

}
AbsRecoHit* 
StripHit::clone(){
  return new StripHit(*this);
}


void 
StripHit::setHMatrix(const AbsTrackRep* stateVector,
			 const TMatrixT<double>& state)
{
 if (dynamic_cast<const GeaneTrackRep*>(stateVector) != NULL) {
    //I know, since this is the same everytime, it could be done in the
    //the constructor, but I do it here anyway, to make clear that in the
    //case of several track-reps per hit, it would have to be done here
   //    _HMatrix.ResizeTo(NparHitRep,5);
    _HMatrix.ResizeTo(1,5);

    _HMatrix[0][0] = 0.;
    _HMatrix[0][1] = 0.;
    _HMatrix[0][2] = 0.;
    _HMatrix[0][3] = 1.;//only u is left
    _HMatrix[0][4] = 0.;

  }
  else {
    std::cerr << "StripHit can only handle state"
              << " vectors of type GeaneTrackRep -> abort" << std::endl;
    throw;
  }
 
}


