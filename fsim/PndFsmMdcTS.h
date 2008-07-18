//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: FsmMdcTS.hh,v 1.3 2006/07/13 10:43:45 klausg Exp $
//
// Description:
//      Class FsmMdcTS
//      
//  Implementation of the MDCs system in the Traget Spectrometer (DC1-DC2) for the Fast Sim Detectors 
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

#ifndef PNDFSMMDCTS_H
#define PNDFSMMDCTS_H

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

class PndFsmMdcTS: public PndFsmAbsDet
{
public:

  //--------------------
  // Public interface --
  //--------------------

  //
  // Constructors
  //

  /** Default ctor. */
  PndFsmMdcTS();
  PndFsmMdcTS(ArgList &par);
  /** Destructor
   */
  virtual ~PndFsmMdcTS();

  /**
      Accessors to contained information
  */
  
  virtual PndFsmResponse* respond(PndFsmTrack *t);
  
private:
  bool   detected(PndFsmTrack *t) const;
  double dp(PndFsmTrack *t) const;
  double dphi(PndFsmTrack *t) const;
  double dtheta(PndFsmTrack *t) const;
   
  void   initParameters();
  void   print(std::ostream &o);
  bool   setParameter(std::string &name, double value);

  //
  //  Modifiers
  //
     
  double _thtMin;
  double _thtMax;
  double _radiationLength;      //material budget
  double _pmin;
  
  double _a3Par;
  double _a4Par;
};

#endif
