//-*- Mode: C++ -*-
// $Id: PndFTSCAGlobalPerformance.h,v 1.1.1.1 2010/07/26 20:55:38 ikulakov Exp $
// ************************************************************************
// This file is property of and copyright by the ALICE HLT Project        *
// ALICE Experiment at CERN, All rights reserved.                         *
// See cxx source for full Copyright notice                               *
//                                                                        *
//*************************************************************************

#ifndef PNDFTSCAGLOBALPERFORMANCE_H
#define PNDFTSCAGLOBALPERFORMANCE_H


#include "PndFTSCATrackPerformanceBase.h"

#include "PndFTSCADef.h"
#include "PndFTSArray.h"
#include "PndFTSCAMCTrack.h"
#include "PndFTSCAMCPoint.h"
#include <fstream>
#include <cstdio>
#include <map>

class TObject;
class TParticle;
class PndFTSCAMCPoint;
class PndFTSCAGBTracker;
class TDirectory;
class TH1D;
class TH2D;
class TProfile;

class TFile;
class PndFTSCATracker;

#define IsOutTrack1

/**
 * @class PndFTSCAGlobalPerformance
 */
class PndFTSCAGlobalPerformance: public PndFTSCATrackPerformanceBase
{
  public:
  
    PndFTSCAGlobalPerformance(){ };
    virtual ~PndFTSCAGlobalPerformance(){};

    virtual void SetNewEvent(const PndFTSCAGBTracker * const Tracker,
                             PndFTSResizableArray<PndFTSCAHitLabel> *hitLabels,
                             PndFTSResizableArray<PndFTSCAMCTrack> *mcTracks,
                             PndFTSResizableArray<PndFTSCALocalMCPoint> *localMCPoints);
    
      /// Efficiency
      // Check if MC track is reconstructable. Calculate set of MC track. Etc.
    virtual void CheckMCTracks(); // fill mcData.
      // Find reco-MCTracks correspondence
    virtual void MatchTracks();   // fill recoData.
      // Calculate efficiencies
    virtual void EfficiencyPerformance();

    virtual void Draw();
  
      /// Histograms
//     virtual void CreateHistos(string histoDir);
    virtual void FillHistos();
};

#endif
