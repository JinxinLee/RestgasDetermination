//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: FsmEmcBwCap.hh,v 1.3 2006/07/13 10:43:45 klausg Exp $
//
// Description:
//      Class FsmEmcBwCap
//      
//  Implementation of the EMC barrel for the Fast Sim Detectors 
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

#ifndef PNDFSMEMCBWCAP_H
#define PNDFSMEMCBWCAP_H

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

class PndFsmEmcBwCap: public PndFsmAbsDet
{
public:

  //--------------------
  // Public interface --
  //--------------------

  //
  // Constructors
  //

  /** Default ctor. */
  PndFsmEmcBwCap();
  PndFsmEmcBwCap(ArgList &par);
  /** Destructor
   */
  virtual ~PndFsmEmcBwCap();

  /**
      Accessors to contained information
  */
  
  virtual PndFsmResponse* respond(PndFsmTrack *t);
  
private:
  bool   detected(PndFsmTrack *t) const;
  double dE(PndFsmTrack *t) const;
  double dphi(PndFsmTrack *t) const;
  double dtheta(PndFsmTrack *t) const;
   
  void   initParameters();
  void   print(std::ostream &o);
  bool   setParameter(std::string &name, double value);
  //
  //  Modifiers
  //
     
  double _aPar;
  double _bPar;
  double _cPar;
  double _xtalDim;
  double _Emin;
  double _dist;
  double _resFactor;
  double _thtMin;
  double _thtMax;	  
  double _radiationLength;      //material budget
  		  
};

#endif
