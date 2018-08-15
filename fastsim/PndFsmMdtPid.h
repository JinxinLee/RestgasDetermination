//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: PndFsmMdtPid.hs $
//
// Description:
//      Class PndFsmMdtPid
//
//  Implementation of the MDT for the Fast Sim Detectors
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

#ifndef PNDFSMMDTPID_H
#define PNDFSMMDTPID_H

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
class TH3F;

class PndFsmMdtPid: public PndFsmAbsDet
{
public:

  //--------------------
  // Public interface --
  //--------------------

  //
  // Constructors
  //

  /** Default ctor. */
  PndFsmMdtPid();
  PndFsmMdtPid(ArgList &par);
  /** Destructor
   */
  virtual ~PndFsmMdtPid();

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

  TH3F   *_mdtPidPdf[6];
  
  double _thtMin;
  double _thtMax;
  double _pmin;
  double _misId;
  
  bool _useFlat;

  std::string _parFileName;     //this file contains root histograms with the MDT pdfs
};

#endif
