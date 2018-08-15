//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: FsmEmcFwCap.hh,v 1.4 2006/07/13 10:43:45 klausg Exp $
//
// Description:
//      Class FsmEmcFwCap
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

#ifndef PNDFSMEMCFWCAP_H
#define PNDFSMEMCFWCAP_H

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

class PndFsmEmcFwCap: public PndFsmAbsDet
{
public:

  //--------------------
  // Public interface --
  //--------------------

  //
  // Constructors
  //

  /** Default ctor. */
  PndFsmEmcFwCap();
  PndFsmEmcFwCap(ArgList &par);
  /** Destructor
   */
  virtual ~PndFsmEmcFwCap();

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
     
  double _aPar;			//Parameter 1 for energy resolution
  double _bPar;			//Parameter 2 for energy resolution
  double _cPar;			//Parameter 3 for energy resolution
  double _xtalDim;		//single x-tal front width
  double _Emin;			//energy threshold
  double _dist;			//distance IP - forward endcap
  double _resFactor;		//multiplier for angular resolution
  double _thtMin;		//polar angle coverage min angle
  double _thtMax;	  	//polar angle coverage max angle
  double _radiationLength;      //material budget  		  
};

#endif
