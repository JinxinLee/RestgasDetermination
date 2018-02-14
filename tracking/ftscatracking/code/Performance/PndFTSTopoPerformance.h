//-*- Mode: C++ -*-
// ************************************************************************
// This file is property of and copyright by the ALICE HLT Project        *
// ALICE Experiment at CERN, All rights reserved.                         *
// See cxx source for full Copyright notice                               *
//                                                                        *
//*************************************************************************
#ifdef DO_TPCCATRACKER_EFF_PERFORMANCE

#ifndef PNDFTSTOPOPERFORMANCE_H
#define PNDFTSTOPOPERFORMANCE_H


#include "PndFTSParticlePerformanceBase.h"

#include "PndFTSCADef.h"
#include "PndFTSArray.h"

#include "PndFTSCAMCTrack.h"
#include "PndFTSCAMCPoint.h"
#include "PndFTSCAMCVertex.h"
#include <fstream>
#include <cstdio>
#include <map>

#include "KFPartMatch.h"
#include "KFMCParticle.h"

class TObject;
class TParticle;
class PndFTSCAMCPoint;
class PndFTSCAGBTracker;
class PndFTSTopoReconstructor;
class TDirectory;
class TH1D;
class TH2D;
class TProfile;

class TFile;
class PndFTSCATracker;

/**
 * @class PndFTSTopoPerformance. Don't use w\o GlobalPerformance
 */
class PndFTSTopoPerformance: public PndFTSParticlePerformanceBase
{
 public:
  
  PndFTSTopoPerformance();
  virtual ~PndFTSTopoPerformance(){};

  virtual void SetNewEvent(
    const PndFTSCAGBTracker * const Tracker,
    PndFTSResizableArray<PndFTSCAHitLabel> *hitLabels,
    PndFTSResizableArray<PndFTSCAMCTrack> *mcTracks,
    PndFTSResizableArray<PndFTSCALocalMCPoint> *localMCPoints);
  
  void SetNewEvent2( const PndFTSTopoReconstructor * const TopoReconstructor ); // use together with SetNewEvent !!!
    
    /// Efficiency
    // Check if MC track is reconstructable. Calculate set of MC track. Etc.
  virtual void CheckMCTracks(); // fill mcData.
    // Find reco-MCTracks correspondence
  virtual void MatchTracks();   // fill recoData.
    // Calculate efficiencies
  virtual void EfficiencyPerformance(){}; // current don't use eff

  virtual void PrintEfficiencyStatistic(){}; // current don't use eff
  virtual void PrintEfficiency()         {};
  
    /// Histograms
    //     virtual void CreateHistos(string histoDir);
  virtual void FillHistos();

 private:

  void GetMCParticles();
  void MatchParticles();
  void CalculateEfficiency();
  void FindReconstructableMCParticles();
  void CheckMCParticleIsReconstructable(KFMCParticle &part);

  const PndFTSTopoReconstructor *fTopoReconstructor;

  vector<PndFTSCAMCVertex> fPrimVertices; // primary vertex positions (currently only one vertex is implemented)

  vector<KFMCParticle> vMCParticles;  // MC particles

  vector<KFPartMatch> MCtoRParticleId; // array for match MC and reco particles
  vector<KFPartMatch> RtoMCParticleId;

  //defines the mode of an efficiency calculation
  //1 - efficiency is normalized on the all MCParticles
  //2 - efficiency is normalized on the MCParticles, which has all daughters reconstructable
  //3 - efficiency is normalized on the MCParticles, which has all daughters reconstructed by the track finder
  const int fFindParticlesMode;
};

#endif
#endif //DO_TPCCATRACKER_EFF_PERFORMANCE
