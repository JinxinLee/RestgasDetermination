//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: FsmMdcFS.hh,v 1.3 2006/07/13 10:43:45 klausg Exp $
//
// Description:
//      Class FsmMdcFS
//      
//  Implementation of the MDCs system (DC3-DC8) in the Forward Spectormeter for the Fast Sim Detectors 
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

#ifndef PNDFSMMDCFS_H
#define PNDFSMMDCFS_H

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

class PndFsmMdcFS: public PndFsmAbsDet
{
public:

  //--------------------
  // Public interface --
  //--------------------

  //
  // Constructors
  //

  /** Default ctor. */
  PndFsmMdcFS();
  PndFsmMdcFS(ArgList &par);
  /** Destructor
   */
  virtual ~PndFsmMdcFS();

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
  double _angleXMax;
  double _angleYMax;
  double _radiationLength;      //material budget
  double _pmin;
  
  double _a1Par;
  double _a2Par;
  double _Bl;
};

#endif
