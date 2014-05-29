//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: PndFsmResponse.cc,v 1.8 2007/05/24 08:07:40 klausg Exp $
//
// Description:
//      Class PndFsmResponse
//      
//  Response of FsmDet for a FsmTrack 
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
#include "PndFsmResponse.h"
#include "PndFsmAbsDet.h"

//-------------
// C Headers --
//-------------

//---------------
// C++ Headers --
//---------------
#include <iostream>
using std::cout;
using std::endl;
using std::ostream;

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

//----------------
// Constructors --
//----------------

PndFsmResponse::PndFsmResponse()
{  
  setDetector(0);
  setDetected(true);
  
  setdE(0.0);
  setdp(0.0);
  setdtheta(0.0);
  setdphi(0.0);
  setdt(0.0);
  setdm(0.0);
  setm2(0.0);              // m2 from Tof
  setTpcdEdx(0.0);         // dEdx from Tpc
  setMvddEdx(0.0);         // dEdx fro Mvd
  setSttdEdx(0.0);         // dEdx fro Stt
  
  setDrcDiscThtc(0.0);         // thetac from Drc disc
  setDrcBarrelThtc(0.0);         // thetac from Drc barrel
  setRichThtc(0.0);         // thetac rich
  setEmcEcal(0.0);			// emc E_calib
  setMuoIron(0.0);			// MDT length in iron
  
  setdV(0.0, 0.0, 0.0);

  setLHElectron(0.0);
  setLHMuon(0.0);
  setLHPion(0.0);
  setLHKaon(0.0);
  setLHProton(0.0);
}

//--------------
// Destructor --
//--------------

PndFsmResponse::~PndFsmResponse()
{
}

//--------------
// Operations --
//--------------

void
PndFsmResponse::setDetector(PndFsmAbsDet *det)
{
  _detector=det;
}

void
PndFsmResponse::setDetected(bool isdet)
{
  _detected=isdet;
}

void
PndFsmResponse::setdE(double val)
{
  _dE=val;
}

void
PndFsmResponse::setdp(double val)
{
  _dp=val;
}

void
PndFsmResponse::setdm(double val)
{
  _dm=val;
}

void
PndFsmResponse::setm2(double val, double err)
{
 _m2=val;
 _m2Err=err;
 
}

void
PndFsmResponse::setTpcdEdx(double val, double err)
{
  _TpcdEdx=val;
  _TpcdEdxErr=err;
}
void
PndFsmResponse::setSttdEdx(double val, double err)
{
  _SttdEdx=val;
  _SttdEdxErr=err;
}

void
PndFsmResponse::setMvddEdx(double val, double err)
{
  _MvddEdx=val;
  _MvddEdxErr=err;
}

void
PndFsmResponse::setDrcDiscThtc(double val, double err)
{
  _DrcDiscThtc=val;
  _DrcDiscThtcErr=err;
}
void
PndFsmResponse::setDrcBarrelThtc(double val, double err)
{
  _DrcBarrelThtc=val;
  _DrcBarrelThtcErr=err;
}

void
PndFsmResponse::setRichThtc(double val, double err)
{
  _RichThtc=val;
  _RichThtcErr=err;
}

void
PndFsmResponse::setdtheta(double val)
{
  _dtheta=val;
}

void
PndFsmResponse::setdphi(double val)
{
  _dphi=val;
}

void
PndFsmResponse::setdt(double val)
{
  _dt=val;
}


void
PndFsmResponse::setLHElectron(double val)
{
  _LHElectron=val;
}

void
PndFsmResponse::setLHMuon(double val)
{
  _LHMuon=val;
}

void
PndFsmResponse::setLHPion(double val)
{
  _LHPion=val;
}

void
PndFsmResponse::setLHKaon(double val)
{
  _LHKaon=val;
}

void
PndFsmResponse::setLHProton(double val)
{
  _LHProton=val;
}

void 
PndFsmResponse::setdV(TVector3 v)
{
  _dV=v;
}

void 
PndFsmResponse::setdV(double dx, double dy, double dz) 
{
  _dV.SetXYZ(dx,dy,dz);
}

void PndFsmResponse::print(ostream &o)
{
  o<<"PndFsmResponse for ";
  o<<"Detector = ";
  if (_detector) o<<_detector->detName();
  else o<<"<UNSPECIFIED>";
  o<<endl;
  o<<"  detected = "<<_detected<<" / ";
  o<<"dE = "<<_dE<<" / ";
  o<<"dp = "<<_dp<<" / ";
  o<<"dm = " << _dm << " / ";
  o<<"m2_Tof = " << _m2 << " / ";
  o<<"dEdx_Tpc = " << _TpcdEdx << " / ";
  o<<"dEdx_Mvd = " << _MvddEdx << " / ";
  o<<"dEdx_Stt = " << _SttdEdx << " / ";
  
  o<<"thtc_disc = " << _DrcDiscThtc << " / ";
  o<<"thtc_barrel = " << _DrcBarrelThtc << " / ";
  o<<"thtc_rich = " << _RichThtc << " / ";
  o<<"ecal_emc = " << _EmcEcal << " / ";
  o<<"iron_mdt = " << _MuoIron << " / ";
  
  o<<"dtheta = "<<_dtheta<<" / ";
  o<<"dphi = "<<_dphi<<" / ";
  o<<"dt = "<<_dt<<endl;

  o<<"dVx = "<<_dV.X()<<" / ";
  o<<"dVy = "<<_dV.Y()<<" / ";
  o<<"dVz = "<<_dV.Z()<<endl;
  
  o<<"  LH_e = "<<_LHElectron <<" / ";
  o<<"LH_mu = "<< _LHMuon<<" / ";
  o<<"LH_pi = "<< _LHPion<<" / ";
  o<<"LH_K = "<< _LHKaon<<" / ";
  o<<"LH_p = "<<_LHProton <<endl; 
}
