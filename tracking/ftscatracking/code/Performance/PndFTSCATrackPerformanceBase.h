//-*- Mode: C++ -*-
// $Id: PndFTSCATrackPerformanceBase.h,v 1.10 2010/09/01 10:38:27 ikulakov Exp $
// ************************************************************************
// This file is property of and copyright by the ALICE HLT Project        *
// ALICE Experiment at CERN, All rights reserved.                         *
// See cxx source for full Copyright notice                               *
//                                                                        *
//*************************************************************************
#ifdef DO_TPCCATRACKER_EFF_PERFORMANCE

#ifndef PNDFTSCATrackPERFORMANCEBASE_H
#define PNDFTSCATrackPERFORMANCEBASE_H

#include "PndFTSCounters.h"

#include "PndFTSPerformanceBase.h"

#include "PndFTSCADef.h"
#include "PndFTSArray.h"
#include "PndFTSCAMCTrack.h"
#include "PndFTSCAMCPoint.h"
#include <fstream>
#include <cstdio>
#include <map>

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

class TFile;

/**
 * @class PndFTSCATrackPerformanceBase
 */
class PndFTSCATrackPerformanceBase: public PndFTSPerformanceBase
{
 public:

  PndFTSCATrackPerformanceBase();
  virtual ~PndFTSCATrackPerformanceBase() { };
    
    /// Histograms
  virtual void CreateHistos(string histoDir = "", TFile* outFile = 0);

 protected:

  virtual void FillHistos();


    /// Histos
  enum{ 
    NTracksPulls = 10,
#if defined(DRIFT_TUBES)
    NHitsPulls = 5+2+2,
#else
    NHitsPulls = 5+2,
#endif
    NHits2DPulls = 2+3+3+3,

    NAllHisto = 4,
    NAllProfiles = 4,
    NAll2DHisto = 1,
    
    NGhostsHisto = 7,
    NGhostsProfiles = 0,
    NGhosts2DHisto = 3,
        
    NRecoTracksHisto = 9,
    NRecoTracksProfiles = 14,
    NRecoTracks2DHisto = 3
  };

};

#endif
#endif //DO_TPCCATRACKER_EFF_PERFORMANCE
