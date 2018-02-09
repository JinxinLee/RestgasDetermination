//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: PndFsmTrack.cc,v 1.16 2006/10/06 15:19:11 aida Exp $
//
// Description:
//      Class PndFsmTrack
//
//  Candidate "Tracks" or "Particles" for the Fast Simulation
//
//  This software was developed for the PANDA collaboration.  If you
//  use all or part of it, please give an appropriate acknowledgement.
//
// Author List:
//      Klaus Goetzen                    Original Author
//
// Copyright Information:
//      Copyright (C) 2006              GSI
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "PndFsmTrack.h"
#include "PndFsmResponse.h"
//#include "RhoBase/TRho.h"
//#include "FastSimApp/FsmHitMap.hh"
//#include "FsmDetTypes.hh"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "TMatrixD.h"
#include "FairRunAna.h"
#include "FairRunSim.h"
#include "FairField.h"
//-------------
// C Headers --
//-------------
#include <iostream>
#include <cmath>
using std::cout;
using std::endl;
using std::ostream;


//---------------
// C++ Headers --
//---------------

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
//#include "PDT/PdtEntry.hh"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

//----------------
// Constructors --
//----------------


PndFsmTrack::PndFsmTrack() {
  setP4(TLorentzVector(0.,0.,0.,0.));
  setStartVtx(TVector3(0.,0.,0.));
  setStopVtx(TVector3(0.,0.,0.));
  setPdt(0);
  setMass2(0.);
  setMvddEdX(0.);
  setTpcdEdX(0.);
  setSttdEdX(0.);
  setCharge(0);
  setGTrackId(0);
  setDetResponse(0);
  for (int i=0;i<15;i++)
    fCov5[i][0]=0;
  for (int i=0;i<5;i++)
    fPar5[i]=0;
  for (int i=0;i<28;i++)
    fCov7[i][0]=0;
}

PndFsmTrack::PndFsmTrack(TLorentzVector const inP4, TVector3 start, TVector3 stop, double inCharge, int inPdt, signed long trackId)
: fCov5(5,5), fCov7(7,7) {
  setP4(inP4);
  setStartVtx(start);
  setStopVtx(stop);
  setCharge(inCharge);
  setPdt(inPdt);
  setGTrackId(trackId);
  setDetResponse(0);
  setMass2( 0.0);
  setMvddEdX(0.0);
  setTpcdEdX(0.0);
  setSttdEdX(0.0);
}

void PndFsmTrack::HelixRep(TVector3 reference) {
  fReference=reference;
  if (fabs(charge())>1e-6) {
    // calculate helix track representation (as in TFitParams.h)
    reference=_startVtx-fReference;
    double tandip=p4().Pz()/p4().Perp();
    double pnt[3], Bf[3];
    pnt[0]=fReference.X(); pnt[1]=fReference.Y(); pnt[2]=fReference.Z();
  FairField* theField=0;
  if(FairRun::Instance()->IsAna()){
    theField=FairRunAna::Instance()->GetField();
//    FairRunAna::Instance()->GetField()->GetFieldValue(pnt, Bf); //[kGs]
  }else{
    theField=FairRunSim::Instance()->GetField();
//    FairRunSim::Instance()->GetField()->GetFieldValue(pnt, Bf); //[kGs]
  }
  if(theField==0) Fatal("PndFsmTrack::HelixRep()","Magnetic Field pointer missing. Set your field!");
    theField->GetFieldValue(pnt, Bf); //[kGs]


    double a=-2.99792458e-3*Bf[2]*charge();
    double omega=a/p4().Perp();
    // construct helix center (1/omega=R)
    TVector3 center = reference + (1/omega)*(TVector3( -p4().Y(), p4().X(), 0).Unit());
    // construct propagation length
    double delta = (center-reference).Phi() - center.Phi();
    double cosrs=cos(delta);
    double sinrs=sin(delta);

    TVector3 p(p4().Vect());

    p.SetZ( p4().Pz() );
    p.SetPhi( p4().Phi() - delta );

    reference.SetX( reference.X() - p.X()/a*sinrs + p.Y()/a*(1-cosrs) );
    reference.SetY( reference.Y() - p.Y()/a*sinrs - p.X()/a*(1-cosrs) );
    reference.SetZ( reference.Z() - tandip*delta/omega );

    fPar5[0]=reference.Cross(p).Z()<0 ? reference.Perp() : -reference.Perp();
    fPar5[1]=p.Phi();
    fPar5[2]=omega;
    fPar5[3]=reference.Z();
    fPar5[4]=tandip;
  } else {
    fPar5[0]=p4().X();
    fPar5[1]=p4().Y();
    fPar5[2]=p4().Z();
    fPar5[3]=p4().T();
  }
}

void PndFsmTrack::Propagate(TVector3 origin, double deltaError) {
  origin-=fReference;
  // calculate p4 and start vertex at point
  // on helix track closest to origin
  double pnt[3], Bf[3];
  pnt[0]=fReference.X(); pnt[1]=fReference.Y(); pnt[2]=fReference.Z();
  FairField* theField=0;
  if(FairRun::Instance()->IsAna()){
    theField=FairRunAna::Instance()->GetField();
//    FairRunAna::Instance()->GetField()->GetFieldValue(pnt, Bf); //[kGs]
  }else{
    theField=FairRunSim::Instance()->GetField();
//    FairRunSim::Instance()->GetField()->GetFieldValue(pnt, Bf); //[kGs]
  }
  if(theField==0) Fatal("PndFsmTrack::HelixRep()","Magnetic Field pointer missing. Set your field!");
  theField->GetFieldValue(pnt, Bf); //[kGs]
  double a=2.99792458e-3*Bf[2];
  double R=1/GetHelixOmega();
  double pt=-a*R*charge();
  double s0=sin(GetHelixPhi0());
  double c0=cos(GetHelixPhi0());
  // add some helix revolutions to get somewhere near origin
  if (fabs(GetHelixTanDip()*R)>1e-9) {
    double dz=2*M_PI*fabs(R*GetHelixTanDip());
    fPar5[3]-=dz*floor((fPar5[3]-origin.Z())/dz+0.5);
  }
  // use gradient method to find POCA to origin
  double s1;
  double c1;
  double ds=0;
  double delta=0;
  double alpha=1;
  double distance2=1e150;
  do {
    delta+=ds;
    s1=sin(GetHelixPhi0()+delta);
    c1=cos(GetHelixPhi0()+delta);
    // vertex setup
    _startVtx.SetXYZ(-s0*(GetHelixD0()+R)+s1*R,
                      c0*(GetHelixD0()+R)-c1*R,
                      GetHelixZ0()+GetHelixTanDip()*R*delta );
    TVector3 distance(_startVtx-origin);
    if (distance2<distance.Mag2()) {
      alpha*=0.5;
      delta-=ds;
    }
    distance2=distance.Mag2();
    // construct helix tangent at delta
    TVector3 u(c1,s1,GetHelixTanDip());
    ds=-u.Dot(distance)/(1+GetHelixTanDip()*GetHelixTanDip())*alpha*GetHelixOmega();
  } while (fabs(ds)>1e-9);

	// momentum setup
  _p4.SetXYZM( pt*c1, pt*s1, pt*GetHelixTanDip(),
               TDatabasePDG::Instance()->GetParticle("pi-")->Mass());

  // calculate jacobian wrt d0..tandip
  TMatrixD J_alpha(7,5);
  J_alpha(0,0)=-s0;
  J_alpha(0,1)=-_startVtx.Y();
  J_alpha(0,2)=-R*R*(s1-s0);

  J_alpha(1,0)=+c0;
  J_alpha(1,1)=+_startVtx.X();
  J_alpha(1,2)=+R*R*(c1-c0);

  J_alpha(2,2)=-delta*GetHelixTanDip()*R*R;
  J_alpha(2,3)=+1;
  J_alpha(2,4)=+delta*R;

  J_alpha(3,1)=-_p4.Y();
  J_alpha(3,2)=-_p4.X()*R;

  J_alpha(4,1)=+_p4.X();
  J_alpha(4,2)=-_p4.Y()*R;

  J_alpha(5,2)=-_p4.Z()*R;
  J_alpha(5,4)=+pt;

  J_alpha(6,2)=-_p4.Vect().Mag2()*R/_p4.T();
  J_alpha(6,4)=+pt*pt*GetHelixTanDip()/_p4.T();

  if (deltaError>=0.001) {
    // calculate jacobian wrt delta to allow fitter
    // to move 1deg along the linearized trajectory
    deltaError*=3.1416/180;
    double covDelta=deltaError*deltaError;
    TMatrixD J_delta(7,1);
    J_delta(0,0)=+R*c1;
    J_delta(1,0)=+R*s1;
    J_delta(2,0)=+GetHelixTanDip()*R;
    J_delta(3,0)=-pt*s1;
    J_delta(4,0)=+pt*c1;
    TMatrixD tmp2(J_delta, TMatrixD::kMultTranspose, J_delta);
    tmp2*=covDelta;
    fCov7+=tmp2;
  }

  // calculate fCov7 = J_alpha * fCov5 * J_alpha.T +
  // covDelta * J_delta * J_delta.T (covDelta is scalar)
  TMatrixD tmp1(J_alpha, TMatrixD::kMult, fCov5);
  fCov7.MultT(tmp1, J_alpha);

  _startVtx+=fReference;
}

void PndFsmTrack::SetP7Cov(TMatrixD &p7cov)
{
  for (int i=0;i<7;i++)
    for (int j=0;j<7;j++)
    {
      fCov7[i][j]=p7cov[i][j];
    }
}

void PndFsmTrack::SetP4Cov(TMatrixD &p4cov)
{
  for (int i=0;i<4;i++)
    for (int j=0;j<4;j++)
    {
      fCov7[i+3][j+3]=p4cov[i][j];
    }
}

void PndFsmTrack::SetVCov(TMatrixD &vcov)
{
  for (int i=0;i<3;i++)
    for (int j=0;j<3;j++)
    {
      fCov7[i][j]=vcov[i][j];
    }
}

//--------------
// Destructor --
//--------------

PndFsmTrack::~PndFsmTrack()
{
  if (_detResponse)
    delete _detResponse;
}

//--------------
// Operations --
//--------------

void
PndFsmTrack::setP4(TLorentzVector l)
{
  _p4=l;
}

void
PndFsmTrack::setStartVtx(TVector3 v)
{
  _startVtx=v;
}

void
PndFsmTrack::setStopVtx(TVector3 v)
{
  _stopVtx=v;
}

void
PndFsmTrack::setCharge(double c)
{
  _charge=c;
}

void
PndFsmTrack::setMass2(double c)
{
  _Mass2=c;
}


void
PndFsmTrack::setMvddEdX(double c)
{
_MvddEdX=c;
}

void
PndFsmTrack::setTpcdEdX(double c)
{
_TpcdEdX=c;
}

void
PndFsmTrack::setSttdEdX(double c)
{
_SttdEdX=c;
}

void
PndFsmTrack::setPdt(int n)
{
  _pdt=n;
}

void
PndFsmTrack::setGTrackId(signed long id)
{
  _gTrackId=id;
}


void
PndFsmTrack::setDetResponse(PndFsmResponse *resp)
{
  _detResponse=resp;
}



void PndFsmTrack::print(ostream &o)
{
  o<<"PndFsmTrack printout"<<endl;
  o<<"  P4   : < "<< _p4.Px()     <<" / "         <<_p4.Py()     <<" / "           <<_p4.Pz()       <<" / "<<_p4.E()<<" >"<<endl;
  o<<"  Vtx1 : < "<< _startVtx.X()<<" / "         <<_startVtx.Y()<<" / "           <<_startVtx.Z()  <<" > " << endl;
  o<<"  Vtx2 : < "<< _stopVtx.X() <<" / "         <<_stopVtx.Y() <<" / "           <<_stopVtx.Z(   )<<" > "  << endl;
  o<<"  charge = "<< _charge      <<" / lundId = "<<_pdt         <<" / gTrackId = "<<_gTrackId      <<endl;
  o<<"  D0: "<<GetHelixD0();
  o<<"  Phi0: "<<GetHelixPhi0()*180/3.1416<<"deg";
  if (GetHelixOmega()) o <<"  1/Omega: "<<1/GetHelixOmega();
  else o <<"  1/Omega: " << "inf";
  o<<"  Z0: "<<GetHelixZ0();
  o<<"  TanDip: "<<GetHelixTanDip()<<endl;
    //  if (_detResponse) _detResponse->print(o);
}

