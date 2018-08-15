//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: PndFsmSttPid.hh,v 1.6 2006/10/05 14:05:53 aida Exp $
//
// Description:
//      Class PndFsmSttPid
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

#ifndef PNDFSMSTTPID_H
#define PNDFSMSTTPID_H

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

class PndFsmSttPid: public PndFsmAbsDet
{
public:

  //--------------------
  // Public interface --
  //--------------------

  //
  // Constructors
  //

  /** Default ctor. */
  PndFsmSttPid();
  PndFsmSttPid(ArgList &par);
  /** Destructor
   */
  virtual ~PndFsmSttPid();

  /**
      Accessors to contained information
  */

  virtual PndFsmResponse* respond(PndFsmTrack *t);

private:
  bool   detected(PndFsmTrack *t) const;

  double compdEdx(double p, double M);
  double gauss(double x, double x0, double s);

  void   initParameters();
  void   print(std::ostream &o);
  bool   setParameter(std::string &name, double value);

  double _thtMin;
  double _thtMax;
  double _ptmin;
  double _dEdxRes;

};

#endif
