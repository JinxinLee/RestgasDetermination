//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: FsmEmcBarrel.cc,v 1.11 2007/05/24 08:07:40 klausg Exp $
//
// Description:
//      Class FsmEmcPid
//      
//  Implementation of the PID info for EMCs
//
//  This software was developed for the PANDA collaboration.  If you
//  use all or part of it, please give an appropriate acknowledgement.
//
// Author List:
//      Klaus Goetzen                    Original Author
//
// Copyright Information:
//      Copyright (C) 2014              GSI
//
//------------------------------------------------------------------------

#ifndef PNDFSMEMCPID_H
#define PNDFSMEMCPID_H

//----------------------
// Base Class Headers --
//----------------------

//-------------------------------
// Collaborating class Headers --
//-------------------------------
#include <iosfwd>
#include "ArgList.h"
#include "PndFsmAbsDet.h"
#include "TH2F.h"

//--------------------------------------------
// Collaborating class forward declarations --
// -------------------------------------------
class PndFsmTrack;
class PndFsmResponse;

class PndFsmEmcPid: public PndFsmAbsDet
{
public:

  //--------------------
  // Public interface --
  //--------------------

  //
  // Constructors
  //

  /** Default ctor. */
  PndFsmEmcPid();
  PndFsmEmcPid(ArgList &par);
  /** Destructor
   */
  virtual ~PndFsmEmcPid();

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
   
  //
  //  Parameters
  //
  TH2F   *_emcPidPdf[6];
     
  double _pmin;			//momentum threshold
  double _ptmin;		//transverse momentum threshold
  double _thtMin;		//polar angle coverage min angle
  double _thtMax;	  	//polar angle coverage max angle
  
  std::string _parFileName;     //this file contains root histograms with the EMC pdfs
};

#endif
