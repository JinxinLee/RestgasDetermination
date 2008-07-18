//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class LSLTrackRep
//      see LSLTrackRep.hh for details
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
#include "LSLTrackRep.h"

// C/C++ Headers ----------------------
#include <iostream>
#include "TMath.h"

// Collaborating Class Headers --------
#include "Nystrom.h"
#include "AbsRecoHit.h"
#include "AbsBFieldIfc.h"
#include "LSLEQM.h"
#include "AbsNystromEQM.h"
#include "TMatrixD.h"

// Class Member definitions -----------

LSLTrackRep::LSLTrackRep()
  : AbsTrackRep(5), s(0) ,_acc(1E-2), _adaptive(false)
{
  
  _eqm=new LSLEQM(0); // this will leak memory in ROOT!
  _refPlane=DetPlane(TVector3(0,0,0),TVector3(1,0,0),TVector3(0,1,0));
  
}

LSLTrackRep::LSLTrackRep(double z, double x, double y, 
			 double dxdz, double dydz, double invp,
			 double sigx, double sigy, 
			 double sigdxdz, double sigdydz, 
			 double siginvp,
			 AbsBFieldIfc* field) 
  : AbsTrackRep(5), s(0), _acc(1E-2), _adaptive(false)
{
  s=z;
  state[0][0]=x;
  state[1][0]=y;
  state[2][0]=dxdz;
  state[3][0]=dydz;
  state[4][0]=invp;

  cov[0][0]=sigx;
  cov[1][1]=sigy;
  cov[2][2]=sigdxdz;
  cov[3][3]=sigdydz;
  cov[4][4]=siginvp;
  
  _eqm=new LSLEQM(field);

  _refPlane=DetPlane(TVector3(x,y,z),TVector3(1,0,0),TVector3(0,1,0));
}

LSLTrackRep::LSLTrackRep(const LSLTrackRep& rep) 
  : AbsTrackRep(rep)
{
  _eqm=new LSLEQM(0);
  _acc=rep._acc;
  _adaptive=rep._adaptive;
  s=rep.s;
}


LSLTrackRep::~LSLTrackRep()
{
  if(_eqm!=NULL)delete _eqm;
}

void
LSLTrackRep::init(const TVector3& pos,
		  double dxdz, double dydz, double invp,
		  double sigx, double sigy, 
		  double sigdxdz, double sigdydz, 
		  double siginvp, 
		  AbsBFieldIfc* field) 
{
  s=pos.Z();
  state[0][0]=pos.X();
  state[1][0]=pos.Y();
  state[2][0]=dxdz;
  state[3][0]=dydz;
  state[4][0]=invp;

  cov[0][0]=sigx;
  cov[1][1]=sigy;
  cov[2][2]=sigdxdz;
  cov[3][3]=sigdydz;
  cov[4][4]=siginvp;

  if(_eqm!=NULL)delete _eqm;
  _eqm=new LSLEQM(field);

  _refPlane=DetPlane(pos,TVector3(1,0,0),TVector3(0,1,0));
}


DetPlane 
LSLTrackRep::getVirtualDetPlane(const TVector3& hit){
  //TMatrixT<double> statePred(5,1);
  //TMatrixT<double> covPred(5,5);
  DetPlane plane;
  //TVector3 o=extrapolateToPoca(hit,statePred,covPred,plane);
  return DetPlane(TVector3(0,0,hit.Z()),TVector3(1,0,0),TVector3(0,1,0));
}

void 
LSLTrackRep::SetBField(AbsBFieldIfc* b)
{
  if(_eqm!=NULL)delete _eqm;
  _eqm=new LSLEQM(b);
}

double
LSLTrackRep::extrapolate(const DetPlane& pl, 
			 TMatrixT<double>& statePred)
{
  double sExtrapolateTo=pl.getO().Z();
  if(sExtrapolateTo<-1000 || sExtrapolateTo>5000)return 0;
  Nystrom rungeKutta(_eqm);
  rungeKutta.setAccuracy(_acc);
  rungeKutta.setAdaptive(_adaptive);
  //prepare the vectors

  //std::cout<<"s before extrapolation: "<<s<<std::endl;
  //std::cout<<"s_to: "<<sExtrapolateTo<<std::endl;

  TVectorT<double> u(3);u[0]=state[0][0];u[1]=state[1][0];u[2]=s;
  TVectorT<double> uprim(3);uprim[0]=state[2][0];uprim[1]=state[3][0];uprim[2]=1.;
  TVectorT<double> par(1);par[0]=state[4][0];

  TVectorT<double> unew(3);unew=u;
  TVectorT<double> uprimnew(3);uprimnew=uprim;
  double l=rungeKutta.propagate(s,sExtrapolateTo,
		       u, uprim, par,
		       unew, uprimnew);
  // write results into statePred
  statePred[0][0]=unew[0];
  statePred[1][0]=unew[1];
  statePred[2][0]=uprimnew[0];
  statePred[3][0]=uprimnew[1];
  statePred[4][0]=state[4][0];
  //std::cout<<"unew[2]=z="<<unew[2];
  //std::cout<<"s after extrapolation: "<<s<<std::endl;
  return l;
}

/*
void LSLTrackRep::extrapolate(const DetPlane& pl,
			      const TMatrixT<double>& stateFrom, 
			      TMatrixT<double>& stateResult) {
  stateResult.ResizeTo(5,1);
  double s=
  Nystrom rungeKutta(_eqm);
  //prepare the vectors
  TVectorT<double> u(3);
  u[0]=stateFrom[0][0];
  u[1]=stateFrom[1][0];
  u[2]=sExtrapolateFrom;
  TVectorT<double> uprim(3);uprim[0]=stateFrom[2][0];uprim[1]=stateFrom[3][0];uprim[2]=1;
  TVectorT<double> par(1);par[0]=stateFrom[4][0];

  TVectorT<double> unew(3);
  TVectorT<double> uprimnew(3);
  rungeKutta.propagate(sExtrapolateFrom,sExtrapolateTo,
					   u, uprim, par,
					   unew, uprimnew);
  // write results into statePred
  stateResult[0][0]=unew[0];
  stateResult[1][0]=unew[1];
  stateResult[2][0]=uprimnew[0];
  stateResult[3][0]=uprimnew[1];
  stateResult[4][0]=state[4][0];
  //std::cout<<"unew[2]=z="<<unew[2];

}
*/

double
LSLTrackRep::extrapolate(const DetPlane& pl, 
			 TMatrixT<double>& statePred,
			 TMatrixT<double>& covPred,
			 TMatrixT<double>& jacobian)
{
  //std::cout << "Extr from To: " << s << " " << sExtrapolateTo << std::endl;
  double l=extrapolate(pl,statePred);
  // covPred=JCovJ^T with J being Jacobian
  jacobian.ResizeTo(5,5);
  Jacobian(pl,statePred,jacobian);
  TMatrixT<double> dummy(cov,TMatrixT<double>::kMultTranspose,jacobian);
  covPred=jacobian*dummy;
  //covPred=cov;
  return l;
}

TVector3
LSLTrackRep::extrapolateToPoca(const TVector3& p, 
			       TMatrixT<double>& statePred,
			       TMatrixT<double>& covPred,
			       DetPlane& plane){
  TMatrixT<double> jacobian;
  plane.setO(p);
  plane.setU(TVector3(1,0,0));
  plane.setV(TVector3(0,1,0));
  extrapolate(plane,statePred,covPred,jacobian);
  return TVector3(statePred[0][0],statePred[1][0],plane.getO().Z());
}


void 
LSLTrackRep::predict(const DetPlane& pl, 
		     TMatrixT<double>& statePred,
		     TMatrixT<double>& covPred,
		     TMatrixT<double>& jacobian)
{
  extrapolate(pl,statePred,covPred,jacobian);
}

void 
LSLTrackRep::stepalong(double h){
  // create new detplane:
  DetPlane newp(_refPlane);
  newp.setO(newp.getO()+TVector3(0,0,h));
  AbsTrackRep::extrapolate(newp);
}

void
LSLTrackRep::Jacobian(const DetPlane& pl,
		      const TMatrixT<double>& statePred,
		      TMatrixT<double>& jacResult){
  
  TMatrixT<double> difPred(statePred);
  // do column wise differntiation:
  for(int icol=0;icol<5;++icol){
    // choose step
    double h=TMath::Abs(state[icol][0])*1.e-4;
    if(h<1e-13)h=1.e-13;
    // vary the state
    state[icol][0]+=h;
    extrapolate(pl,difPred);
    // difference:
    difPred-=statePred;
    // remove variation from state
    state[icol][0]-=h;
    // fill jacobian with difference quotient
    for(int irow=0;irow<5;++irow)jacResult[irow][icol]=difPred[irow][0]/h;
  }
}
		

TVector3 
LSLTrackRep::getPos(const DetPlane& pl)
{
  double z=pl.getO().Z();
  TMatrixT<double> statePred(state);
  DetPlane p(TVector3(0,0,z),TVector3(1,0,0),TVector3(0,1,0));
  extrapolate(p,statePred);
  return TVector3(statePred[0][0],statePred[1][0],z);
}
 
TVector3 
LSLTrackRep::getMom(const DetPlane& pl)
{
  double z=pl.getO().Z();
  TMatrixT<double> statePred(state);
  //statePred.Print();
  DetPlane p(TVector3(0,0,z),TVector3(1,0,0),TVector3(0,1,0));
  extrapolate(p,statePred);
  //statePred.Print();
  TVector3 result(statePred[2][0],statePred[3][0],1);
  if(TMath::Abs(statePred[4][0])!=0){
    result.SetMag(1./TMath::Abs(statePred[4][0]));
  }
  else result.SetMag(100);
  if(inverted)result=(-1.)*result;
  return result;
}

void
LSLTrackRep::getPosMom(const DetPlane& pl,TVector3& pos,TVector3& mom)
{
  double z=pl.getO().Z();
  TMatrixT<double> statePred(state);
  DetPlane p(TVector3(0,0,z),TVector3(1,0,0),TVector3(0,1,0));
  extrapolate(p,statePred);
  pos.SetXYZ(statePred[0][0],statePred[1][0],z);
  mom.SetXYZ(statePred[2][0],statePred[3][0],1);
  if(TMath::Abs(statePred[4][0])!=0){
    mom.SetMag(1./TMath::Abs(statePred[4][0]));
  }
  else mom.SetMag(100);
  if(inverted)mom=(-1.)*mom;
}


TVectorT<double> 
LSLTrackRep::getGlobal() {// (x,y,z,px,py,pz)
  TVector3 pos=AbsTrackRep::getPos();
  TVector3 mom=AbsTrackRep::getMom();
  double par[6];
  par[0]=pos.X();par[1]=pos.Y();par[2]=pos.Z();
  par[3]=mom.X();par[4]=mom.Y();par[5]=mom.Z();
  return TVectorT<double>(6,par);
}


TMatrixT<double> 
LSLTrackRep::getGlobalCov(){ // covariances
  TMatrixT<double> L(6,5);
  double xp=state[2][0];
  double yp=state[3][0];
  double no=xp*xp+yp*yp+1;
  double sq=sqrt(no);
  double sq3inv=1/(sq*sq*sq);
  double q=getCharge();
  double p=q/state[4][0];
  L[0][0]=1;
  L[1][1]=1;
  L[3][2]=p*(1+yp*yp)*sq3inv;
  L[3][3]=-p*xp*yp*sq3inv;
  L[3][4]=-p/(2.*state[4][0])*xp/sq;
  L[4][2]=p*(1+xp*xp)*sq3inv;
  L[4][3]=-p*xp*yp*sq3inv;
  L[4][4]=-p/(2.*state[4][0])*yp/sq;
  L[5][2]=-p*xp*sq3inv;
  L[5][3]=-p*yp*sq3inv;
  L[5][4]=-p/(2.*state[4][0])/sq;
  
  // calculate new cov;
  TMatrixT<double> LT(TMatrixD::kTransposed,L);
  TMatrixT<double> dum(cov,TMatrixD::kMult,LT);
  TMatrixT<double> result(L,TMatrixD::kMult,dum);

  // set sigma_z by hand: 
  result[2][2]=0.01;

  return result;
  
}
	      
ClassImp(LSLTrackRep)
