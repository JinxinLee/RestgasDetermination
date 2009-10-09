//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcConfMapRecoHit
//      see PndTpcConfMapRecoHit.hh for details
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
#include "PndTpcConfMapRecoHit.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------
#include "GFDetPlane.h"
#include "PndTpcConfMapFit.h"
#include "PndTpcZSFit.h"

// Class Member definitions -----------


PndTpcConfMapRecoHit::PndTpcConfMapRecoHit()
  : GFRecoHitIfc<GFPlanarHitPolicy>(2), _refx(0), _refy(0), 
    _s(1E-4), _isRotated(false), _cl(NULL)
{}
  
PndTpcConfMapRecoHit::PndTpcConfMapRecoHit(double xp, double yp)
  : GFRecoHitIfc<GFPlanarHitPolicy>(2), _refx(0), _refy(0),
    _s(1E-4), _isRotated(false), _cl(NULL)
{
  fHitCoord[0][0]=yp;
  fHitCoord[1][0]=0;
  _xcf=xp;
  fHitCov[0][0]=0.1;
}

PndTpcConfMapRecoHit::PndTpcConfMapRecoHit(PndTpcCluster* cl)
  : GFRecoHitIfc<GFPlanarHitPolicy>(2), _refx(0), _refy(0)
    , _s(1E-4), _isRotated(false), _cl(cl)
{
  _clusterx=cl->pos().X();
  _clustery=cl->pos().Y();
  _clusterz=cl->pos().Z();
  _clusterindex=cl->index();
  fHitCoord[1][0]=_clusterz;
  fHitCov[0][0]=0.1;
  fHitCov[1][1]=_cl->sig().Z();
  reMap();
}

GFAbsRecoHit* 
PndTpcConfMapRecoHit::clone()
{
  return new PndTpcConfMapRecoHit(*this);
}

double 
PndTpcConfMapRecoHit::calc_s(double R) {
  // R is radius of track from 
  double dx=_clusterx-_refx;
  double dy=_clustery-_refy;
  double L=sqrt(dx*dx+dy*dy);
  double _2R=2.*R;
  _s=_2R*TMath::ASin(L/_2R);
  return _s;
}


TMatrixT<double> 
PndTpcConfMapRecoHit::residualVector(const GFAbsTrackRep* stateVector,
				     const TMatrixT<double>& state)
{
  TMatrixT<double> H = getHMatrix(stateVector);
  TMatrixT<double> res=( fHitCoord - (H*state ));
  if (dynamic_cast<const PndTpcConfMapFit*>(stateVector) != NULL) {
    // clear lower entry
    res[1][0]=0;
  }
  else if(dynamic_cast<const PndTpcZSFit*>(stateVector) !=NULL) {
    // clear upper entry
    res[0][0]=0;
  }
  return res;
}




TMatrixT<double>
PndTpcConfMapRecoHit::getHMatrix(const GFAbsTrackRep* stateVector)
{
  if (dynamic_cast<const PndTpcConfMapFit*>(stateVector) != NULL) {
    TMatrixT<double> HMatrix(2,2);
    HMatrix[0][0]=_xcf;
    HMatrix[0][1]=1.;
    HMatrix[1][0]=0.;
    HMatrix[1][1]=0.;
    return HMatrix;
  }
  else if(dynamic_cast<const PndTpcZSFit*>(stateVector) !=NULL) {
    TMatrixT<double> HMatrix(2,2);
    HMatrix[0][0]=0;
    HMatrix[0][1]=0;
    HMatrix[1][0]=_s;
    HMatrix[1][1]=1.;   
    return HMatrix;
  }

  else {
    std::cerr << "PndTpcConfMapRecoHit can only handle state"
              << " vectors of type PndTpcConfMapFit -> abort" << std::endl;
    throw;
  }

}


void
PndTpcConfMapRecoHit::setReferencePoint(double xref, double yref)
{
  _refx=xref;
  _refy=yref;
  reMap();
}

void
PndTpcConfMapRecoHit::reMap()
{
  double x=_clusterx;
  double x0=_refx;
  double y=_clustery;
  double y0=_refy;
  // see if it is rotated
  if(_isRotated){
    double xstored=x;
    x=y;
    y=-xstored;
    double x0stored=x0;
    x0=y0;
    y0=-x0stored;
  }


  // conformal mapping part
  double dx=x-x0;
  double dy=y-y0;
  double r2=dx*dx+dy*dy;
  if(r2!=0){
    fHitCoord[0][0]=dy/r2;
    _xcf=dx/r2;
  }
  else{
    fHitCoord[0][0]=1E-12;
    _xcf=1E-12;
  }
  // set covariances;
  double r4=r2*r2;
  
  if(r4==0){
    //std::cout<<"r4==0!"<<std::endl;
    r4=1;
  }

  double dyp_dy=(r2-2*dy*dy)/r4;
  double dyp_dx=(-dy*4*dx)/r4;

  double sigx2=0.01;
  double sigy2=0.01;
  if(_cl!=NULL){
    sigx2=_cl->sig().X();sigx2*=sigx2;
    sigy2=_cl->sig().Y();sigy2*=sigy2;
  }


  fHitCov[0][0]=dyp_dy*dyp_dy*sigy2 + dyp_dx*dyp_dx*sigx2;
  if(TMath::IsNaN(fHitCov[0][0])){
    std::cout<<"hitcov nan! &&&&&&&&&&&&&&&&&&&"<<std::endl;
    std::cout<<r2<<"  "<<dx<<"  "<<dy<<"  "<<dyp_dy<<"  "<<dyp_dx<<std::endl;
  }

 
}

