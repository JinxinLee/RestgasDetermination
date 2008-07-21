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
    double delta = (center-start).Phi() - center.Phi();
    double cosrs=cos(delta);
    double sinrs=sin(delta);

    TVector3 p(p4.Vect());
    TVector3 r;
    
    p.SetZ( p4.Pz() );
    p.SetPhi( p4.Phi() - delta );
    
    r.SetX( start.X() - p.X()/a*sinrs + p.Y()/a*(1-cosrs) );
    r.SetY( start.Y() - p.Y()/a*sinrs - p.X()/a*(1-cosrs) );
    r.SetZ( start.Z() - tandip*delta/omega );
    
    fPar5[0]=r.Cross(p).Z()<0 ? r.Perp() : -r.Perp();
    fPar5[1]=p.Phi();
    fPar5[2]=omega;
    fPar5[3]=r.Z();
    fPar5[4]=tandip;
  } else {
    fPar5[0]=p4.X();
    fPar5[1]=p4.Y();
    fPar5[2]=p4.Z();
    fPar5[3]=p4.T();
  }
}

void PndFsmTrack::Propagate() {

  // for now, calculate p4 and startvertex at doca
  double a=2.99792458e-3*TRho::Instance()->GetMagnetField();
  double pt=-a*charge()/GetHelixOmega();
  double s=sin(GetHelixPhi0());
  double c=cos(GetHelixPhi0());
	const static double mass = TDatabasePDG::Instance()->GetParticle("pi-")->Mass();
	// momentum setup
  _p4.SetX( pt*c );
  _p4.SetY( pt*s );
  _p4.SetZ( pt*GetHelixTanDip() );
  _p4.SetVectMag(_p4.Vect(), mass);
  // vertex setup
  _startVtx.SetX(-s*GetHelixD0() );
  _startVtx.SetY( c*GetHelixD0() );
  _startVtx.SetZ( GetHelixZ0() );

  // calculate jacobian
  TMatrixD J(7,5);
  J(0,0)=-s;
  J(0,1)=-_startVtx.Y();

  J(1,0)=+c;
  J(1,1)=+_startVtx.X();

  J(2,3)=1;

  J(3,1)=-_p4.Y();
  J(3,2)=-_p4.X()/GetHelixOmega();

  J(4,1)=+_p4.X();
  J(4,2)=-_p4.Y()/GetHelixOmega();

  J(5,2)=-_p4.Z()/GetHelixOmega();
  J(5,4)=+pt;

  J(6,2)=-_p4.Vect().Mag2()/_p4.T()/GetHelixOmega();
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

