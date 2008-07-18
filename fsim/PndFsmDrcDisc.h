//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: FsmDrcDisc.hh,v 1.2 2006/07/13 10:43:45 klausg Exp $
//
// Description:
//      Class FsmDrcDisc
//      
//  Implementation of the Disc DIRC for the Fast Sim Detectors 
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

#ifndef PNDFSMDRCDISC_H
#define PNDFSMDRCDISC_H

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
class TH2F;

class PndFsmDrcDisc: public PndFsmAbsDet
{
public:

  //--------------------
  // Public interface --
  //--------------------

  //
  // Constructors
  //

  /** Default ctor. */
  PndFsmDrcDisc();
  PndFsmDrcDisc(ArgList &par);
  /** Destructor
   */
  virtual ~PndFsmDrcDisc();

  /**
      Accessors to contained information
  */
  
  virtual PndFsmResponse* respond(PndFsmTrack *t);
  
private:
  bool   detected(PndFsmTrack *t) const;
  double compThetaC(double p, double m);
  double gauss(double x, double x0, double s);
   
   
  void   initParameters();
  void   print(std::ostream &o);
  bool   setParameter(std::string &name, double value);
  bool   setParameter(std::string &name, std::string &value);
  
  bool   readParameters();
  
  //
  //  Modifiers
  //
     
  
  TH2F   *trapfrac[5];
  
  double _thtMin;               // spatial coverage
  double _thtMax;               //    "
  double _radiationLength;      // material budget
  double _pmin;                 // minimum p_t to reach DIRC
  double _dthtc;                // resolution for tht_C for single photon (-> res=dthtc/sqrt(nPhot) ) 
  double _nPhotMin;             // minimum number of photons cutoff
  double _nRefrac;              // refraction index of radiator
  double _Bfield;               // B field strength [T]
  double _effNPhotons;          // overall efficiency for photon detection
  double _dDisc;                // thickness of radiator disc
  double _dp;                   // accuracy of momentum measurement
  double _trap;                 // avg trapping fraction (only when file below doesn't exist)
  std::string _parFileName;     //this file contains root histograms with the trapping fractions
  
};

#endif
