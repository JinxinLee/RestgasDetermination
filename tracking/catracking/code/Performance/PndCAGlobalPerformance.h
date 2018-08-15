//-*- Mode: C++ -*-
// $Id: PndCAGlobalPerformance.h,v 1.1.1.1 2010/07/26 20:55:38 ikulakov Exp $
// ************************************************************************
// This file is property of and copyright by the ALICE HLT Project        *
// ALICE Experiment at CERN, All rights reserved.                         *
// See cxx source for full Copyright notice                               *
//                                                                        *
//*************************************************************************

#ifndef PNDCAGLOBALPERFORMANCE_H
#define PNDCAGLOBALPERFORMANCE_H


#include "PndCATrackPerformanceBase.h"

#include "PndCADef.h"
#include "PndCAMCTrack.h"
#include "PndCAMCPoint.h"
#include <fstream>
#include <cstdio>
#include <map>

class TObject;
class TParticle;
class PndCAMCPoint;
class PndCAGBTracker;
class TDirectory;
class TH1D;
class TH2D;
class TProfile;

class TFile;
class PndCATracker;

#define IsOutTrack1

/**
 * @class PndCAGlobalPerformance
 */
class PndCAGlobalPerformance: public PndCATrackPerformanceBase
{
  public:
  
    PndCAGlobalPerformance(){ };
    virtual ~PndCAGlobalPerformance(){};

    virtual void SetNewEvent(const PndCAGBTracker * const Tracker,
                             vector<PndCAHitLabel> *hitLabels,
                             vector<PndCAMCTrack> *mcTracks,
                             vector<PndCALocalMCPoint> *localMCPoints);
    
      /// Efficiency
      // Check if MC track is reconstructable. Calculate set of MC track. Etc.
    virtual void CheckMCTracks(); // fill mcData.
      // Find reco-MCTracks correspondence
    virtual void MatchTracks();   // fill recoData.
      // Calculate efficiencies
    virtual void EfficiencyPerformance();
  
      /// Histograms
//     virtual void CreateHistos(string histoDir);
    virtual void FillHistos();
};

#endif
