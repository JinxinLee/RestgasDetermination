//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: FsmAbsDet.hh,v 1.5 2007/05/24 08:07:40 klausg Exp $
//
// Description:
//      Class FsmAbsDet
//
//  Abstract Class Interface for Fast Sim Detectors
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

#ifndef PNDFSMABSDET_H
#define PNDFSMABSDET_H

//----------------------
// Base Class Headers --
//----------------------

//-------------------------------
// Collaborating class Headers --
//-------------------------------

//---------------
// C++ Headers --
//---------------
#include <string>

#include "FsmDetTypes.h"

#include "ArgList.h"
#include "TRandom3.h"
#include "TDatabasePDG.h"

//--------------------------------------------
// Collaborating class forward declarations --
// -------------------------------------------
class PndFsmResponse;
class PndFsmTrack;

class PndFsmAbsDet
{
public:

  //--------------------
  // Public interface --
  //--------------------

  //
  // Constructors
  //

  PndFsmAbsDet();
  PndFsmAbsDet(ArgList &par);
  // Destructor
  //
  virtual ~PndFsmAbsDet();

  //
  //   Accessors to contained information
  //

  virtual PndFsmResponse* respond(PndFsmTrack *t) = 0;
  virtual void print(std::ostream &o);

  const std::string& detName() {return _detName;}
  void setName(std::string& name) {_detName = name;};
  Bool_t doesPid() const {return _doesPid;};
  void setStorePid(Bool_t doespid=kTRUE) {_doesPid=doespid;};

protected:

  virtual bool setParameter(std::string &name, double value);
  virtual bool setParameter(std::string &name, std::string &value);

  virtual void initParameters();
  void   parseParameterList(ArgList &par);


  //
  //  Modifiers
  //

  std::string   _detName;
  double        _efficiency;		//overall efficiency
  TRandom3*     _rand;
  TDatabasePDG* _fdbPDG;
  Bool_t        _doesPid;

};

#endif
