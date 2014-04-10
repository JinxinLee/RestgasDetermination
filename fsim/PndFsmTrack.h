//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: PndFsmTrack.hh,v 1.13 2006/10/06 15:19:11 aida Exp $
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

#ifndef PNDFSMTRACK_H
#define PNDFSMTRACK_H

//----------------------
// Base Class Headers --
//----------------------

//-------------------------------
// Collaborating class Headers --
//-------------------------------
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TMatrixD.h"

//#include "CLHEP/Vector/LorentzVector.h"
//#include "CLHEP/Geometry/HepPoint.h"
//#include "PDT/PdtEntry.hh"
//#include "G4Data/GVertex.hh"

//--------------------------------------------
// Collaborating class forward declarations --
// -------------------------------------------
#include <iosfwd>
#include <vector>

class PndFsmResponse;
//class FsmHitMap;

class PndFsmTrack
{
public:

  //--------------------
  // Public interface --
  //--------------------

  //
  // Constructors
  //

  PndFsmTrack();
  PndFsmTrack(TLorentzVector p4, TVector3 start, TVector3 stop, double charge, int pdt, signed long trackId);

  //
  // Destructor
  //
  virtual ~PndFsmTrack();

  //
  //    Accessors to contained information
  //
  TLorentzVector   p4() {return _p4;}   // 4-momentum
  TVector3         startVtx() {return _startVtx;}
  TVector3         stopVtx() {return _stopVtx;}
  double	   charge() {return _charge;}
  int              pdt() {return _pdt;}
  signed long	   gTrackId() {return _gTrackId;}
  PndFsmResponse*     detResponse() {return _detResponse;}
  double  Mass2()    {return _Mass2;}
  double  MvddEdX() {return _MvddEdX;}
  double  TpcdEdX() {return _TpcdEdX;}
  double  SttdEdX() {return _SttdEdX;}

  // for the time being partial interface conservation for migration purpose
  bool hitMapValid() {return false;}
  bool hitMapResponse(unsigned int index) {return false;}



  //
  //  Modifiers
  //
  void setP4(TLorentzVector l);
  void setStartVtx(TVector3 v);
  void setStopVtx(TVector3 v);
  void setCharge(double c);
  void setGTrackId(signed long id);
  void setPdt(int pdt);
  void setDetResponse(PndFsmResponse* resp);

  void setMass2(double c);
  void setMvddEdX(double c);
  void setTpcdEdX(double c);
  void setSttdEdX(double c);


  // Operations

  void print(std::ostream &o);

private:
  TLorentzVector   _p4;
  TVector3         _startVtx;
  TVector3         _stopVtx;
  double	   _charge;
  int              _pdt;
  signed long	   _gTrackId;
  double           _Mass2;
  double           _MvddEdX;
  double           _TpcdEdX;
  double           _SttdEdX;
  PndFsmResponse*     _detResponse;
  // adding helix representation for some realism
  double fPar5[5]; // helix fit parameters: d0, phi0, omega, z0, tandip
  TVector3 fReference; // reference point to helix rep
  TMatrixD fCov5;
  TMatrixD fCov7;

public:
  double *GetHelixParams() { return fPar5; }
  TMatrixD& GetHelixCov() {return fCov5; }
  TMatrixD& Cov7() {return fCov7;}
  void SetP7Cov(TMatrixD &p7cov);
  void SetP4Cov(TMatrixD &p4cov);
  void SetVCov(TMatrixD &vcov);

  Double_t GetHelixD0()     const {return fPar5[0];}
  Double_t GetHelixPhi0()   const {return fPar5[1];}
  Double_t GetHelixOmega()  const {return fPar5[2];}
  Double_t GetHelixZ0()     const {return fPar5[3];}
  Double_t GetHelixTanDip() const {return fPar5[4];}

  void HelixRep(TVector3 reference);
  void Propagate(TVector3 origin, double deltaError=2.5);

};

#endif
