//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: PndFsmIdealPid.h $
//
// Description:
//      Class PndFsmIdealPid
//
//  Implementation of the Montecarlo true PID for the Fast Sim Detectors
//
//  This software was developed for the PANDA collaboration.  If you
//  use all or part of it, please give an appropriate acknowledgement.
//
// Author List:
//      Ralf Kliemt                     Original Author
//
// Copyright Information:
//      Copyright (C) 2014              GSI
//
//------------------------------------------------------------------------

#ifndef PNDFSMIDEALPID_H
#define PNDFSMIDEALPID_H

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

class PndFsmIdealPid: public PndFsmAbsDet
{
public:

  //--------------------
  // Public interface --
  //--------------------

  //
  // Constructors
  //

  /** Default ctor. */
  PndFsmIdealPid();
  PndFsmIdealPid(ArgList &par);
  /** Destructor
   */
  virtual ~PndFsmIdealPid();

  /**
      Accessors to contained information
  */

  virtual PndFsmResponse* respond(PndFsmTrack *t);

private:
  bool   detected(PndFsmTrack *t) const;
  void   initParameters();
  void   print(std::ostream &o);
  bool   setParameter(std::string &name, double value);
  bool   readParameters();
};

#endif
