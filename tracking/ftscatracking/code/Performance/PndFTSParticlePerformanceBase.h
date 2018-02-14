//-*- Mode: C++ -*-
// $Id: PndFTSParticlePerformanceBase.h,v 1.10 2010/09/01 10:38:27 ikulakov Exp $
// ************************************************************************
// This file is property of and copyright by the ALICE HLT Project        *
// ALICE Experiment at CERN, All rights reserved.                         *
// See cxx source for full Copyright notice                               *
//                                                                        *
//*************************************************************************
#ifdef DO_TPCCATRACKER_EFF_PERFORMANCE

#ifndef PNDFTSParticlePERFORMANCEBASE_H
#define PNDFTSParticlePERFORMANCEBASE_H

#include "PndFTSCounters.h"

#include "PndFTSPerformanceBase.h"

#include "PndFTSCADef.h"
#include "PndFTSArray.h"
#include "PndFTSCAMCTrack.h"
#include "PndFTSCAMCPoint.h"
#include <fstream>
#include <cstdio>
#include <map>

#include "KFPartEfficiencies.h"

#include <string>
using std::string;

#include <iostream>
using std::ostream;
using std::istream;

class TObject;
class TParticle;
class PndFTSCAMCPoint;
class PndFTSCAGBTracker;
class TDirectory;
class TH1;
class TH1F;
class TH2F;
class TFile;

class KFParticle;

/**
 * @class PndFTSParticlePerformanceBase
 */
class PndFTSParticlePerformanceBase: public PndFTSPerformanceBase
{
 public:

  PndFTSParticlePerformanceBase();
  virtual ~PndFTSParticlePerformanceBase() { };
    
    /// Histograms
  virtual void CreateHistos(string histoDir = "", TFile* outFile = 0);

 protected:

  virtual void FillHistos();


    /// Histos
  enum{ 
    NTrackPulls = 12,
    NTrackPullsAtProdVertex = 12,
    NVertexPulls = 6
  };

  //for KFParticleFinder
  
  vector<KFParticle> *fParticles;
  KFParticle *fPV;
// Arrays of points, tracks, etc

// Names of files
  TString outfileName;
  TDirectory* histodir;

// efficiencies
  KFPartEfficiencies fParteff;
  int fNEvents;

//histos
  static const int nFitQA = 16;
  TH1F *hFitDaughtersQA[KFPartEfficiencies::nParticles][nFitQA];
  TH1F *hFitQA[KFPartEfficiencies::nParticles][nFitQA];

  static const int nHistoPartParam = 12;
  TH1F *hPartParam[KFPartEfficiencies::nParticles][nHistoPartParam]; // mass, p, pt, Y, decay length, c*tau, chi/ndf, prob, theta, phi, z
  TH1F *hPartParamBG[KFPartEfficiencies::nParticles][nHistoPartParam];
  TH1F *hPartParamGhost[KFPartEfficiencies::nParticles][nHistoPartParam];
//   TH1F *hPartParamCorrBG[KFPartEfficiencies::nParticles][nHistoPartParam];
  TH1F *hPartParamSignal[KFPartEfficiencies::nParticles][nHistoPartParam];
  static const int nHistoPartParamQA = 3;
  TH1F *hPartParamQA[KFPartEfficiencies::nParticles][nHistoPartParamQA*2]; // residuals and pulls of these parameters

  static const int nHistoPartParam2D = 1;
  TH2F *hPartParam2D[KFPartEfficiencies::nParticles][nHistoPartParam2D]; // y-pt,
  TH2F *hPartParam2DBG[KFPartEfficiencies::nParticles][nHistoPartParam2D];
  TH2F *hPartParam2DGhost[KFPartEfficiencies::nParticles][nHistoPartParam2D];
//   TH2F *hPartParam2DCorrBG[KFPartEfficiencies::nParticles][nHistoPartParam2D];
  TH2F *hPartParam2DSignal[KFPartEfficiencies::nParticles][nHistoPartParam2D];

  static const int nHistosPV = 6;
  TH1F *hPVFitQa[nHistosPV];

  static const int nHistosTP = KFPartEfficiencies::nParticles + 1;
  TH1F *hTrackParameters[nHistosTP];
};

#endif
#endif //DO_TPCCATRACKER_EFF_PERFORMANCE
