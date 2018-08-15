//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: FsmTpc.hh,v 1.4 2006/08/10 09:58:41 klausg Exp $
//
// Description:
//      Class FsmEffTracker
//      
//  Implementation of an effective Tracker for the FastSim
//  which provides resolution for mom, tht, phi from full sim
//  NOTE: should be used INSTEAD Tpc/Stt, Mvd and the Mdc's
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

#ifndef PNDFSMEFFTRACKER_H
#define PNDFSMEFFTRACKER_H

//----------------------
// Base Class Headers --
//----------------------

//-------------------------------
// Collaborating class Headers --
//-------------------------------
#include <iosfwd>
#include "ArgList.h"
#include "PndFsmAbsDet.h"

//--------------------------------------------
// Collaborating class forward declarations --
// -------------------------------------------
class PndFsmTrack;
class PndFsmResponse;

class PndFsmEffTracker: public PndFsmAbsDet
{
public:

  //--------------------
  // Public interface --
  //--------------------

  //
  // Constructors
  //

  /** Default ctor. */
  PndFsmEffTracker();
  PndFsmEffTracker(ArgList &par);
  /** Destructor
   */
  virtual ~PndFsmEffTracker();

  /**
      Accessors to contained information
  */
  
  virtual PndFsmResponse* respond(PndFsmTrack *t);
  
private:
  bool   detected(PndFsmTrack *t) const;
  double dp(PndFsmTrack *t) const;
  double dphi(PndFsmTrack *t) const;
  double dtheta(PndFsmTrack *t) const;
   
  double compdEdx(double p, double M);
  double gauss(double x, double x0, double s);

  void   initParameters();
  void   print(std::ostream &o);
  bool   setParameter(std::string &name, double value);

  bool   readParameters();
   
  //
  //  Modifiers
  //
     
  double _thtMin;
  double _thtMax;
  double _radiationLength;      //material budget
  double _pmin;
  double _pRes;
  double _phiRes;
  double _thetaRes;
  double _dEdxRes;

  std::string _parFileName;

  int _rows;
  int _cols;
  
  double _sigPhi[6][12];
  double _sigTht[6][12];
  double _sigMom[6][12];

};

#endif
