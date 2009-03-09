#include "SpacepointHitPolicy.h"

#include "assert.h"

#include "TMath.h"

#include "AbsRecoHit.h"

TMatrixT<double> 
SpacepointHitPolicy::hitCoord(AbsRecoHit* hit,const DetPlane& plane)
{
  TMatrixT<double> returnMat(2,1);

  TMatrixT<double> _D(3,1);
  TVector3 _U;
  TVector3 _V;

  _D[0][0] = (plane.getO())[0];
  _D[1][0] = (plane.getO())[1];
  _D[2][0] = (plane.getO())[2];

  _D *= -1.; 
  _D += hit->getRawHitCoord();

  _U = plane.getU();
  _V = plane.getV();


  returnMat[0][0] = _D[0][0] * _U[0] + _D[1][0] * _U[1] + _D[2][0] * _U[2];
  returnMat[1][0] = _D[0][0] * _V[0] + _D[1][0] * _V[1] + _D[2][0] * _V[2];
  //std::cout << "hitCoord="<<std::endl;
  //returnMat.Print();
  return returnMat;
}

TMatrixT<double> 
SpacepointHitPolicy::hitCov(AbsRecoHit* hit,const DetPlane& plane)
{
  TVector3 _U;
  TVector3 _V;

  _U = plane.getU();
  _V = plane.getV();

  TMatrixT<double> rawCov = hit->getRawHitCov();

  TMatrixT<double> jac(3,2);
  
  // jac = dF_i/dx_j = s_unitvec * t_untivec, with s=u,v and t=x,y,z
  jac[0][0] = _U[0];
  jac[1][0] = _U[1];
  jac[2][0] = _U[2];
  jac[0][1] = _V[0];
  jac[1][1] = _V[1];
  jac[2][1] = _V[2];

  TMatrixT<double> jac_orig = jac;
  TMatrixT<double> jac_t = jac.T();

  TMatrixT<double> result=jac_t * (rawCov * jac_orig);
  //std::cout << "hitCov="<<std::endl;
  //result.Print();
  return  result;
}

const DetPlane&
SpacepointHitPolicy::detPlane(AbsRecoHit* hit, AbsTrackRep* rep)
{
  _plane=rep->getVirtualDetPlane(_pos);
  return _plane;
}
