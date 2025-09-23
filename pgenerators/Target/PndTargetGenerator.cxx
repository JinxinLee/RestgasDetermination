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
// -----                PndTargetGenerator source file                 -----
// -----               Created 13/02/2020  by R.Kliemt                 -----
// -------------------------------------------------------------------------

#include "PndTargetGenerator.h"

#include "FairPrimaryGenerator.h"

#include "TRandom.h"
#include "TMath.h"
#include "TF1.h"
#include "TVector3.h"
#include "TGraph.h"

// ------------------------------------------------------------------------
PndTargetGenerator::PndTargetGenerator()
  : fRlimit(2.), fRsigma(0.), fDrDz(0.), fDmin(0.), fDmax(0.), fVerbose(0), fDoExtended(false), fDensityFile(""), fDensityGraph(nullptr), fInvertedCDF(nullptr)
{
  // Default constructor
}

// ------------------------------------------------------------------------
PndTargetGenerator::PndTargetGenerator(TString densityFile, double rsigma, double drdz, double rlimit)
  : fRlimit(rlimit), fRsigma(rsigma), fDrDz(drdz), fDmin(0.), fDmax(0.), fVerbose(0), fDoExtended(false), fDensityFile(densityFile), fDensityGraph(nullptr), fInvertedCDF(nullptr)
{
  // Constructor.
}

// ------------------------------------------------------------------------
Bool_t PndTargetGenerator::Init()
{
  // Initialize generator
  FairGenerator::Init();
  ReadDensityFile();
  std::cout << "PndTargetGenerator::Init():" << std::endl;
  return kTRUE;
}

// ------------------------------------------------------------------------
TVector3 PndTargetGenerator::SampleInteractionVertex()
{
  if (fDoExtended) {
    // Actual calculation of a random interaction vertex
    double x(0), y(0), z(0), zdiv(0), rsigma(0);
    double r = 2 * fRlimit;
    double drand = gRandom->Uniform(fDmin, fDmax); // random cumulated density
    z = fInvertedCDF->Eval(drand);                 // --> random from z profile
    if (z < fConstantZmin)
      zdiv = z - fConstantZmin; // only set zdiv not to zero if outside the solenoid
    else if (z > fConstantZmax)
      zdiv = z - fConstantZmax;
    rsigma = fRsigma + fabs(zdiv) * fDrDz; // increase beam spread by opening slope outside the solenoid field region
    while (r > fRlimit)
      r = gRandom->Gaus(0, rsigma); // sample a valid radius
    gRandom->Circle(x, y, r);       // random phi angle, directly converted to (x,y)
    TVector3 result(x, y, z);
    return result;
  } else {
    TVector3 zero(0., 0., 0.);
    return zero;
  }
}

// ------------------------------------------------------------------------
// read DD from file DDfname
// get number of points npoints, zmin, and zmax
void PndTargetGenerator::ReadDensityFile()
{
  std::cout << "PndTargetGenerator::ReadDensityFile():" << std::endl;
  if (fDensityFile == "")
    return;

  fDensityGraph = new TGraph(fDensityFile);
  fDensityGraph->SetTitle("Measured target & rest gas density profile;z/cm;#rho/?");
  int npoints = fDensityGraph->GetN();
  // Make cmmulative graph
  fInvertedCDF = new TGraph(npoints);
  fInvertedCDF->SetTitle("Measured target & rest gas density profile cumulated and inverted;#Sigma#rho/?;z/cm");
  double x, y;     // temporary
  double ysum = 0; // running sum
  for (int i = 0; i < npoints; i++) {
    // create a cumulative distribution, but switching the axis
    // this will allow easy random number generation
    fDensityGraph->GetPoint(i, x, y);
    ysum += y;
    fInvertedCDF->SetPoint(i, ysum, x); // inverted graph to acces x by random y
  }
  double zmin, zmax;                                    // temporary
  fInvertedCDF->ComputeRange(fDmin, zmin, fDmax, zmax); // x: density  y: z /cm

  if (fDensityGraph && fInvertedCDF) {
    fDoExtended = true;
    std::cout << "PndTargetGenerator::ReadDensityFile(): created new profile from file " << fDensityFile.Data() << std::endl;
  } else {
    std::cout << "PndTargetGenerator::ReadDensityFile(): ERROR: Failed to read density file " << fDensityFile.Data() << std::endl;
  }
  return;
}

// ------------------------------------------------------------------------

ClassImp(PndTargetGenerator)
