//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class GeaneTrackRep
//      see GeaneTrackRep.hh for details
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
#include "GeaneTrackRep.h"
#include "FairGeaneUtil.h"
#include "FairTrackParH.h"

// C/C++ Headers ----------------------
#include <iostream>
#include <cmath>

// Collaborating Class Headers --------
#include "AbsRecoHit.h"
#include "FitterExceptions.h"
#include "FairGeanePro.h"

// Class Member definitions -----------

#define THETACUT 0.4

GeaneTrackRep::GeaneTrackRep()
  : AbsTrackRep(5), _pdg(211),_backw(0)
{

}

GeaneTrackRep::GeaneTrackRep(FairGeanePro* geane, 
			     const DetPlane& plane,
			     const TVector3& mom,
			     const TVector3& poserr,
			     const TVector3& momerr,
			     double q,
			     int PDGCode) 
  : AbsTrackRep(5), _geane(geane), _pdg(PDGCode), _backw(0)
{
  printf("$#$#$##$#$#$#$$#$#$#$# %10.10f\n",mom.Theta()/TMath::Pi()*180.);
  FairTrackParP par(plane.getO(),mom,poserr,momerr,q,plane.getO(),plane.getU(),plane.getV());

  _spu=par.GetSPU(); // direction of the momentum

  state[0][0]=par.GetQp();
  state[1][0]=par.GetTV();
  state[2][0]=par.GetTW();
  state[3][0]=par.GetV();
  state[4][0]=par.GetW();

  // blow up cov-array: ROOT does not support init with symmetric data
  // See ROOT docu source-file for TMatrixTSym
  // i=row, j=collumn
  double* covarray=par.GetCov();
  int count=0;
  for(int i=0;i<5;++i){
    for(int j=i;j<5;++j){
      cov[i][j]=covarray[count];
      if(i!=j)cov[j][i]=covarray[count];
      ++count;
    }
  }
  setStartState(state);
  setStartCov(cov);
  setStartPlane(plane);
  _refPlane=plane;
}

//  GeaneTrackRep::GeaneTrackRep(const GeaneTrackRep& rep) 
//   : AbsTrackRep(rep)
// {
//   _geane=rep._geane;
// }


GeaneTrackRep::~GeaneTrackRep()
{
  
}




double
GeaneTrackRep::extrapolate(const DetPlane& pl, 
			   TMatrixT<double>& statePred)
{
  TMatrixT<double> covPred(5,5);
  return  extrapolate(pl,statePred,covPred);
  //! TODO: make this faster by neglecting covariances ?
}


double
GeaneTrackRep::extrapolate(const DetPlane& pl, 
			   TMatrixT<double>& statePred,
			   TMatrixT<double>& covPred)
{
  if(fabs(getMom(_refPlane).Theta()/TMath::Pi()*180.) < THETACUT){
	FitterException exc("GEANE propagation not possible for p.theta<THETACUT",__LINE__,__FILE__);
    throw exc;
  }

  TVector3 o=pl.getO();
  TVector3 u=pl.getU();
  TVector3 v=pl.getV();

  TVector3 ofrom=_refPlane.getO();
  TVector3 ufrom=_refPlane.getU();
  TVector3 vfrom=_refPlane.getV();

  _geane->PropagateFromPlane(ufrom,vfrom);
  _geane->PropagateToPlane(o,u,v);

  FairTrackParP result;
  FairTrackParH result2;
  
  //std::cout<<"Before prop:"<<std::endl;
  //Print();

  double cova[15];
  int count=0;;
  for(int i=0; i<5;++i){
    for(int j=i;j<5;++j){
      cova[count++]=cov[i][j];
    }
  }
  // protect against low momentum:
  if(fabs(state[0][0])>10){
    FitterException exc("GeaneTrackRep: PROTECT AGAINST LOW MOMENTA",__LINE__,__FILE__);
	throw exc;
  }

  // protect against (x,y)=(0,0)
  if(state[3][0]==0)state[3][0]=1E-4;
  if(state[4][0]==0)state[4][0]=1E-4;
  
  
  FairTrackParP par(state[3][0],state[4][0],state[1][0],state[2][0],state[0][0],cova,ofrom,ufrom,vfrom,_spu);

  bool backprop=_backw<0;
  if(_backw==0){
    //Try to guess if we are doing a forward or backward step:
    TVector3 pos(par.GetX(),par.GetY(),par.GetZ());
    TVector3 dir=pl.dist(pos); // direction from pos to plane;
    //Assume B=(0,0,BZ) -> compare signs of dir.Z and mom.Z:
    backprop= (dir.Z()*par.GetPz())<0 ? true : false;
  }
  if(backprop){
    _geane->setBackProp();
    std::cout<<"GEANETRACKREP: USING BACKPROPAGATION!" << std::endl;
  }

  Bool_t prop = kTRUE;
  prop = _geane->Propagate(&par,&result,_pdg);   //211
  if (prop==kFALSE){
    FitterException exc("GEANE propagation failed",__LINE__,__FILE__);
    throw exc;
  }
  

  double l=_geane->GetLengthAtPCA();
 
  statePred[0][0]=result.GetQp();
  statePred[1][0]=result.GetTV();
  statePred[2][0]=result.GetTW();
  statePred[3][0]=result.GetV();
  statePred[4][0]=result.GetW();

  

  double* rescov=result.GetCov();
  count=0;
  for(int i=0;i<5;++i){
    for(int j=i;j<5;++j){
      covPred[i][j]=rescov[count];
      if(i!=j)covPred[j][i]=rescov[count];
      ++count;
    }
  }
  
  //   if(result.GetSPU()!=_spu)std::cout<<"SPU HAS CHANGED! "<<_spu<<" --> "<<result.GetSPU()<<std::endl;
  _spu=result.GetSPU();

  //std::cout<<"AFTER EXTRAPOLATE:"<<std::endl;
  //result.Print();
  //pl.Print();
  //statePred.Print();
  //covPred.Print();
  


  return l;
}



void
GeaneTrackRep::extrapolateToPoca(const TVector3& pos,
				 TVector3& poca,
				 TVector3& dirInPoca){
  if(fabs(getMom(_refPlane).Theta()/TMath::Pi()*180.) < THETACUT){
	FitterException exc("GEANE propagation not possible for p.theta<THETACUT",__LINE__,__FILE__);
    throw exc;
  }
  int dim = getDim();
  TMatrixT<double> statePred(dim,1);
  TMatrixT<double> covPred(dim,dim);
  //std::cout<<"GeaneTrackRep::extrapolateToPoca"<<std::endl;
  //_refPlane.Print();

  TVector3 ofrom=_refPlane.getO();
  TVector3 ufrom=_refPlane.getU();
  TVector3 vfrom=_refPlane.getV();

  _geane->SetPoint(pos);
  _geane->PropagateFromPlane(ufrom,vfrom);

  double cova[15];
  int count=0;;
  for(int i=0; i<5;++i){
    for(int j=i;j<5;++j){
      cova[count++]=cov[i][j];
    }
  }
  // protect against low momentum:
  if(fabs(state[0][0])>10){
    FitterException exc("GeaneTrackRep: PROTECT AGAINST LOW MOMENTA",__LINE__,__FILE__);
	throw exc;
  }

  // protect against (x,y)=(0,0)
  if(state[3][0]==0)state[3][0]=1E-4;
  if(state[4][0]==0)state[4][0]=1E-4;
  
  FairTrackParP par(state[3][0],state[4][0],state[1][0],state[2][0],state[0][0],cova,ofrom,ufrom,vfrom,_spu);
  par.Print();
  
  bool backprop=_backw<0;
  if(_backw==0){
    // check if new point is after or before my position
    double myz=par.GetZ();
    double dir=pos.Z()-myz;
    double mypz=par.GetPz();
    dir*=mypz;
    backprop=dir<0;
  }
  if(!backprop){ // point lies in same direction of flight as momentum
    std::cout<<" Propagate in flight direction"<<std::endl;
    _geane->PropagateToVirtualPlaneAtPCA(1);
  }
  else{
    std::cout<<" backPropagate"<<std::endl;
    _geane->BackTrackToVirtualPlaneAtPCA(1);
  }

  FairTrackParP result;
  Bool_t prop = kTRUE;

  prop = _geane->Propagate(&par,&result,_pdg);   //211
  if (prop==kFALSE) {
    FitterException exc("GEANE propagation failed",__LINE__,__FILE__);
    throw exc;
    //pl=_refPlane;
    //return pos;
  }

  statePred[0][0]=result.GetQp();
  statePred[1][0]=result.GetTV();
  statePred[2][0]=result.GetTW();
  statePred[3][0]=result.GetV();
  statePred[4][0]=result.GetW();

  double* rescov=result.GetCov();
  count=0;
  for(int i=0;i<5;++i){
    for(int j=i;j<5;++j){
      covPred[i][j]=rescov[count];
      if(i!=j)covPred[j][i]=rescov[count];
      ++count;
    }
  }

  poca.SetXYZ(result.GetX(),result.GetY(),result.GetZ());
  dirInPoca = result.GetJVer().Cross( result.GetKVer() );
}


void 
GeaneTrackRep::extrapolateToLine(const TVector3& point1,
				 const TVector3& point2,
				 TVector3& poca,
				 TVector3& dirInPoca,
				 TVector3& poca_onwire)
{
  if(fabs(getMom(_refPlane).Theta()/TMath::Pi()*180.) < THETACUT){
	FitterException exc("GEANE propagation not possible for p.theta<THETACUT",__LINE__,__FILE__);
    throw exc;
  }

  // call propagation to closest approach to a wire 
  Int_t pca = 2;

  // calculate a very large track length 
  TVector3 start = getPos(_refPlane);
  Double_t distance1, distance2;
  distance1 = (point1 - start).Mag();
  distance2 = (point2 - start).Mag();
  Double_t maxdistance;
  if(distance1 < distance2) maxdistance = distance2;
  else maxdistance = distance1;
  maxdistance *= 2.;  

  // variables for FindPCA:
  TVector3 point(0,0,0);
  Double_t Rad = 0.;
  // poca = vpf = point of closest approach on track
  // poca_onwire = vwi = point of closest approach on wire
  Double_t Di = 0.;
  Float_t trklength = 0.;
  
  // covariance matrix
  FairGeaneUtil util;
  Double_t cov55[5][5];
  for(int i = 0; i < 5; i++) for(int j = 0; j < 5; j++) cov55[i][j] = cov[i][j];
  Double_t cova[15];
  util.FromMat25ToVec15(cov55, cova);
  
  TVector3 o  = _refPlane.getO();
  TVector3 dj = _refPlane.getU();
  TVector3 dk = _refPlane.getV();
  
  FairTrackParP par(state[3][0],state[4][0],state[1][0],state[2][0],state[0][0],cova,o,dj,dk,_spu);

  // get propagation direction
  Int_t direction = getPropDir();
  
  _geane->ActualFindPCA(pca, &par, direction);
  Int_t findpca = _geane->FindPCA(pca, _pdg, point, point1, point2, maxdistance, Rad, poca, poca_onwire, Di, trklength);
  
  if(findpca != 0) {
    FitterException exc("findpca failure", __LINE__,__FILE__);	
    throw exc;    
  }

  // dir in poca not filled now
  dirInPoca.SetXYZ(0., 0., 0.);
  
}






TVector3 
GeaneTrackRep::getPocaOnLine(const TVector3& p1, const TVector3& p2, bool back){
  
  //std::cout<<"GeaneTrackRep::getPocaToWire"<<std::endl;

  TVector3 ofrom=_refPlane.getO();
  TVector3 ufrom=_refPlane.getU();
  TVector3 vfrom=_refPlane.getV();

  _geane->SetWire(p1,p2);
  _geane->PropagateFromPlane(ufrom,vfrom);
  double cova[15];
  int count=0;;
  for(int i=0; i<5;++i){
    for(int j=i;j<5;++j){
      cova[count++]=cov[i][j];
    }
  }
  // protect against low momentum:
  if(fabs(state[0][0])>10){
    //statePred=state;
    //covPred=cov;
    statusFlag=10;
    std::cout<<"*** PROTECT AGAINST LOW MOMENTA ***"<<std::endl;
    return TVector3(0,0,-9999);
  }

  // protect against (x,y)=(0,0)
  if(state[3][0]==0)state[3][0]=1E-4;
  if(state[4][0]==0)state[4][0]=1E-4;
  
  
  FairTrackParP par(state[3][0],state[4][0],state[1][0],state[2][0],state[0][0],cova,ofrom,ufrom,vfrom,_spu);

  
  if(!back){ // point lies in same direction of flight as momentum
    //std::cout<<" Propagate in flight direction"<<std::endl;
    _geane->PropagateToVirtualPlaneAtPCA(2); // option 2 means wire!
  }
  else{
    //std::cout<<" backPropagate"<<std::endl;
    _geane->BackTrackToVirtualPlaneAtPCA(2);
  }

  FairTrackParP result;
  Bool_t prop = kTRUE;

  prop = _geane->Propagate(&par,&result,_pdg);
  if (prop==kFALSE) {
    FitterException exc("GEANE propagation failed",__LINE__,__FILE__);
    throw exc;
  }

  return _geane->GetPCAOnWire();
}






TVector3 
GeaneTrackRep::getPos(const DetPlane& pl)
{
  TMatrixT<double> statePred(state);
  if(pl!=_refPlane)extrapolate(pl,statePred);
  return pl.getO()+(statePred[3][0]*pl.getU())+(statePred[4][0]*pl.getV());
}
 
TVector3 
GeaneTrackRep::getMom(const DetPlane& pl)
{
  TMatrixT<double> statePred(state);
  if(pl!=_refPlane)extrapolate(pl,statePred);
  double fSPU  = _spu;
  TVector3 mom = fSPU*pl.getNormal()+fSPU*statePred[1][0]*pl.getU()+fSPU*statePred[2][0]*pl.getV();
  mom.SetMag(1./fabs(statePred[0][0]));
  return mom;
}
void
GeaneTrackRep::getPosMom(const DetPlane& pl,TVector3& pos, TVector3& mom)
{
  TMatrixT<double> statePred(state);
  if(pl!=_refPlane)extrapolate(pl,statePred);
  mom = pl.getNormal()+statePred[1][0]*pl.getU()+statePred[2][0]*pl.getV();

  mom.SetMag(1./fabs(statePred[0][0]));
  pos = pl.getO()+(statePred[3][0]*pl.getU())+(statePred[4][0]*pl.getV());


}
 
ClassImp(GeaneTrackRep)
