//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: FsmDrcBarrel.hh,v 1.3 2006/07/13 10:43:45 klausg Exp $
//
// Description:
//      Class FsmDrc
//      
//  Implementation of the barrel DIRC for the Fast Sim Detectors 
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

#ifndef PNDFSMDRCBARREL_H
#define PNDFSMDRCBARREL_H

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

class PndFsmDrcBarrel: public PndFsmAbsDet
{
public:

  //--------------------
  // Public interface --
  //--------------------

  //
  // Constructors
  //

  /** Default ctor. */
  PndFsmDrcBarrel();
  PndFsmDrcBarrel(ArgList &par);
  /** Destructor
   */
  virtual ~PndFsmDrcBarrel();

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
  double _rBarrel;              // radius of DIRC barrel
  double _dSlab;                // thickness of radiator bars/slabs
  double _dp;                   // momentum resolution 
  double _trap;                 // avg trapping fraction (only when file below doesn't exist)
  std::string _parFileName;     //this file contains root histograms with the trapping fractions
  
};

#endif
