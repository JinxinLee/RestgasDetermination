//****************************************************************************
//*                   This file is part of PandaRoot.                        *
//*                                                                          *
//*            PandaRoot is distributed under the terms of the               *
//*              GNU General Public License (GPL) version 3,                 *
//*                 copied verbatim in the file "LICENSE".                   *
//*                                                                          *
//*  Copyright (C) 2006 - 2024 FAIR GmbH and copyright holders of PandaRoot  *
//*     The copyright holders are listed in the file "COPYRIGHTHOLDERS".     *
//*               The authors are listed in the file "AUTHORS".              *
//****************************************************************************

// -------------------------------------------------------------------------
// -----                PndTargetGenerator header file                 -----
// -----               Created 13/02/2020  by R.Kliemt                 -----
// -------------------------------------------------------------------------

/**  PndTargetGenerator.h
 *@author Ralf Kliemt <r.kliemt@gsi.de>*
 * The PndTargetGenerator is an intermediate step in the inheritance chain to provide a 3d profile of
 * interaction vertices to all Panda generators. In the x-y plane the beam profile is a Gaussian and in z
 * an arbitrary profile represented by a TF1
 **/

#ifndef PND_TARGETGENERATOR_H
#define PND_TARGETGENERATOR_H

#include "FairGenerator.h"
#include <iostream>

class FairPrimaryGenerator;
class TF1;
class TVector3;
class TGraph;

class PndTargetGenerator : public FairGenerator {
 public:
  /** Default constructor. **/
  PndTargetGenerator();

  /** Constructor with an ascii file name for the density profile and a beam width **/
  PndTargetGenerator(TString densityFile, double radius = 0., double drdz = 0., double rlimit = 2.);

  /** Destructor **/
  virtual ~PndTargetGenerator(){};

  /** Modifiers **/
  void SetDensityProfile(TString density) { fDensityFile = density; }
  void SetBeamRadius(double radius = 0.1) { fRsigma = radius; }    // default beam spot sigma 1mm^2 by "Fair Operation Modes" document v.6 (2020)
  void SetBeamPipeRadius(double radius = 2.) { fRlimit = radius; } // Smallest radius around interaction region
  void SetConstantBeamRegion(double zmin = -140., double zmax = 223)
  {
    fConstantZmin = zmin;
    fConstantZmax = zmax;
  }                                                     //
  void SetBeamDrDz(double drdz = 0.1) { fDrDz = drdz; } // default maximum beam divergence from 4-sigma emittance of 1-2 mm mrad by "Fair Operation Modes" document v.6 (2020)
  void SetVerbose(int verb = 1) { fVerbose = verb; }
  TGraph *GetDensityGraph() { return fDensityGraph; }
  /** Initializer **/
  virtual Bool_t Init();
  TVector3 SampleInteractionVertex();
  void ReadDensityFile();
  virtual Bool_t ReadEvent(FairPrimaryGenerator *) { return true; }; // FIXME Delete!
 protected:
 private:
  double fRlimit;        // Radius of beam pipe (hard limit)
  double fRsigma;        // Radius of beam profile (Gaussian sigma)
  double fConstantZmin;  // Start of constant beam width region
  double fConstantZmax;  // End of constant beam width region
  double fDrDz;          // Radius increase slope from (0,0,0)
  double fDmin;          // Minimum density
  double fDmax;          // Maximum density
  int fVerbose;          // Switch debug output
  bool fDoExtended;      // switch if not using distributed targets
  TString fDensityFile;  // ascii file to load density profile
  TGraph *fDensityGraph; // storage of measured points
  TGraph *fInvertedCDF;  // storage of measured points as inverted "CDF"

  ClassDef(PndTargetGenerator, 1);
};

#endif
