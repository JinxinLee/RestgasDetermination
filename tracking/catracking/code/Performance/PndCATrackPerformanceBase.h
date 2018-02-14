//-*- Mode: C++ -*-
// $Id: PndCATrackPerformanceBase.h,v 1.10 2010/09/01 10:38:27 ikulakov Exp $
// ************************************************************************
// This file is property of and copyright by the ALICE HLT Project        *
// ALICE Experiment at CERN, All rights reserved.                         *
// See cxx source for full Copyright notice                               *
//                                                                        *
//*************************************************************************
#ifdef DO_TPCCATRACKER_EFF_PERFORMANCE

#ifndef PNDCATRACKPERFORMANCEBASE_H
#define PNDCATRACKPERFORMANCEBASE_H

#include "PndCACounters.h"

#include "PndCAPerformanceBase.h"

#include "PndCADef.h"
#include "PndCAMCTrack.h"
#include "PndCAMCPoint.h"
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
class PndCAMCPoint;
class TDirectory;
class TH1;

class TFile;

/**
 * @class PndCATrackPerformanceBase
 */
class PndCATrackPerformanceBase: public PndCAPerformanceBase
{
 public:

  PndCATrackPerformanceBase();
  virtual ~PndCATrackPerformanceBase(){};
    
    /// Histograms
  virtual void CreateHistos(string histoDir = "", TFile* outFile = 0);

 protected:

  virtual void FillHistos();


    /// Histos
  enum{ 
    NTracksPulls = 10,
    NHitsPulls = 5+2,
    NHits2DPulls = 3+3+3,

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
