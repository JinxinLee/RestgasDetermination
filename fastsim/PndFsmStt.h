//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: FsmStt.hh,v 1.6 2006/10/05 14:05:53 aida Exp $
//
// Description:
//      Class FsmStt
//      
//  Implementation of the STT for the Fast Sim Detectors 
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

#ifndef PNDFSMSTT_H
#define PNDFSMSTT_H

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

class PndFsmStt: public PndFsmAbsDet
{
public:

  //--------------------
  // Public interface --
  //--------------------

  //
  // Constructors
  //

  /** Default ctor. */
  PndFsmStt();
  PndFsmStt(ArgList &par);
  /** Destructor
   */
  virtual ~PndFsmStt();

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
   
  //
  //  Modifiers
  //
     
  double _n;
  double _sigXY;
  double _Bfield;
  double _Lpath;
  double _X0;
  double _sigTht;	  
  double _thtMin;
  double _thtMax;
  double _pmin;
  double _rmin;
  double _radiationLength;      //material budget
  double _dEdxRes;
  
};

#endif
