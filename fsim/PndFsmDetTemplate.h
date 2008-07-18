//--------------------------------------------------------------------------
// Description:
//      Class PndFsmDetTemplate
//      
//  Template class for a Fast Sim Detectors 
//
//  This software was developed for the PANDA collaboration.  If you
//  use all or part of it, please give an appropriate acknowledgement.
//
// Author List:
//      Klaus Goetzen                    Original Author
//
// Copyright Information:
//      Copyright (C) 2008              GSI
//
//------------------------------------------------------------------------

#ifndef PNDFSMDETTEMPLATE_H
#define PNDFSMDETTEMPLATE_H

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

class PndFsmDetTemplate: public PndFsmAbsDet
{
public:

  //--------------------
  // Public interface --
  //--------------------

  //
  // Constructors
  //

  /** Default ctor. */
  PndFsmDetTemplate();
  PndFsmDetTemplate(ArgList &par);
  /** Destructor
   */
  virtual ~PndFsmDetTemplate();

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
  double _ptmin;

  double _pRes;
  double _thtRes;
  double _phiRes;
  
};

#endif
