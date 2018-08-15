//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: PndFsmSimpleVtx.hh,v 1.4 2006/08/10 09:58:41 klausg Exp $
//
// Description:
//      Class PndFsmSimpleVtx
//
//  Implementation of the MVD for the Fast Sim Detectors
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

#ifndef PNDFSMSIMPLEVTX_H
#define PNDFSMSIMPLEVTX_H

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

class PndFsmSimpleVtx: public PndFsmAbsDet
{
public:

  //--------------------
  // Public interface --
  //--------------------

  //
  // Constructors
  //

  /** Default ctor. */
  PndFsmSimpleVtx();
  PndFsmSimpleVtx(ArgList &par);
  /** Destructor
   */
  virtual ~PndFsmSimpleVtx();

  /**
      Accessors to contained information
  */

  virtual PndFsmResponse* respond(PndFsmTrack *t);

private:
  bool   detected(PndFsmTrack *t) const;

  void   initParameters();
  void   print(std::ostream &o);
  bool   setParameter(std::string &name, double value);

  //
  //  Modifiers
  //

  double _thtMin;
  double _thtMax;
  double _ptmin;
  double _vtxRes;               //vertex resolution dx,dy,dz
};

#endif
