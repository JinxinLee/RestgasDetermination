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
#include "RhoBase/TRho.h"
//#include "FastSimApp/FsmHitMap.hh"
//#include "FsmDetTypes.hh"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "TMatrixD.h"
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
  for (char i=0;i<15;i++)
    fCov5[i]=0;
  for (char i=0;i<5;i++)
    fPar5[i]=0;
  for (char i=0;i<28;i++)
    fCov7[i]=0;
}

PndFsmTrack::PndFsmTrack(TLorentzVector const p4, TVector3 start, TVector3 stop, double charge, int pdt, signed long trackId) 
: fCov5(5,5), fCov7(7,7) {
  setP4(p4);
  setStartVtx(start);
  setStopVtx(stop);
  setCharge(charge);
  setPdt(pdt);
  setGTrackId(trackId);
  setDetResponse(0);
  setMass2( 0.0);
  setMvddEdX(0.0);
  setTpcdEdX(0.0);
  setSttdEdX(0.0);

  if (fabs(charge)>1e-6) {
    // calculate helix track representation (as in TFitParams.h)
    double tandip=p4.Pz()/p4.Perp();
    double a=-2.99792458e-3*TRho::Instance()->GetMagnetField()*charge;
    double omega=a/p4.Perp();
    // construct helix center (1/omega=R)
    TVector3 center = start + (1/omega)*(TVector3( -p4.Y(), p4.X(), 0).Unit());
    // construct propagation length
    delta = (center-start).Phi() - center.Phi();
    double cosrs=cos(delta);
    double sinrs=sin(delta);

    TVector3 p(p4.Vect());
    
    p.SetZ( p4.Pz() );
    p.SetPhi( p4.Phi() - delta );
    
    start.SetX( start.X() - p.X()/a*sinrs + p.Y()/a*(1-cosrs) );
    start.SetY( start.Y() - p.Y()/a*sinrs - p.X()/a*(1-cosrs) );
    start.SetZ( start.Z() - tandip*delta/omega );
    
    fPar5[0]=start.Cross(p).Z()<0 ? start.Perp() : -start.Perp();
    fPar5[1]=p.Phi();
    fPar5[2]=omega;
    fPar5[3]=start.Z();
    fPar5[4]=tandip;
  } else {
    fPar5[0]=p4.X();
    fPar5[1]=p4.Y();
    fPar5[2]=p4.Z();
    fPar5[3]=p4.T();
  }
}

void PndFsmTrack::Propagate() {

  // for now, calculate p4 and start vertex
  // at (approximately) the point of creation
  double a=2.99792458e-3*TRho::Instance()->GetMagnetField();
  double R=1/GetHelixOmega();
  double pt=-a*R*charge();
  double s0=sin(GetHelixPhi0());
  double c0=cos(GetHelixPhi0());
  double s1=sin(GetHelixPhi0()+delta);
  double c1=cos(GetHelixPhi0()+delta);
	// momentum setup
  _p4.SetX( pt*c1 );
  _p4.SetY( pt*s1 );
  _p4.SetZ( pt*GetHelixTanDip() );
  _p4.SetVectMag(_p4.Vect(), TDatabasePDG::Instance()->GetParticle("pi-")->Mass());
  // vertex setup
  _startVtx.SetX(-s0*(GetHelixD0()+R)+s1*R );
  _startVtx.SetY( c0*(GetHelixD0()+R)-c1*R );
  _startVtx.SetZ( GetHelixZ0()+GetHelixTanDip()*R*delta );

  // calculate jacobian
  TMatrixD J(7,5);
  J(0,0)=-s0;
  J(0,1)=-_startVtx.Y();
  J(0,2)=-R*R*(s1-s0);

  J(1,0)=+c0;
  J(1,1)=+_startVtx.X();
  J(1,2)=+R*R*(c1-c0);

  J(2,2)=-delta*GetHelixTanDip()*R*R;
  J(2,3)=+1;
  J(2,4)=+delta*R;

  J(3,1)=-_p4.Y();
  J(3,2)=-_p4.X()*R;

  J(4,1)=+_p4.X();
  J(4,2)=-_p4.Y()*R;

  J(5,2)=-_p4.Z()*R;
  J(5,4)=+pt;

  J(6,2)=-_p4.Vect().Mag2()*R/_p4.T();
  J(6,4)=+pt*pt*GetHelixTanDip()/_p4.T();
  // calculate fCov7 = J * fCov5 * J.T
  TMatrixD tmp(J, TMatrixD::kMult, fCov5);
  fCov7.MultT(tmp, J);
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
PndFsmTrack::setPdt(int pdt)
{
  _pdt=pdt;
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
  o<<"  D0: "<<GetHelixD0()
   <<"  Phi0: "<<GetHelixPhi0()
   <<"  Omega: "<<GetHelixOmega()
   <<"  Z0: "<<GetHelixZ0()
   <<"  TanDip: "<<GetHelixTanDip()<<endl;
    //  if (_detResponse) _detResponse->print(o);
}

