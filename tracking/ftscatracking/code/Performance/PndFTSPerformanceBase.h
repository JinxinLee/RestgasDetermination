//-*- Mode: C++ -*-
// ************************************************************************
// This file is property of and copyright by the ALICE HLT Project        *
// ALICE Experiment at CERN, All rights reserved.                         *
// See cxx source for full Copyright notice                               *
//                                                                        *
//*************************************************************************
#ifdef DO_TPCCATRACKER_EFF_PERFORMANCE

#ifndef PNDFTSPERFORMANCEBASE_H
#define PNDFTSPERFORMANCEBASE_H

#include "PndFTSCounters.h"

#include "PndFTSCADef.h"
#include "PndFTSArray.h"
#include "PndFTSCAMCTrack.h"
#include "PndFTSCAMCPoint.h"
#include <fstream>
#include <cstdio>
#include <map>

#include <string>
using std::string;

#include "TString.h"

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
 * @class PndFTSPerformanceBase
 */
class PndFTSPerformanceBase
{
 public:

  struct PndFTSCAHitLabel {
    int fLab[3]; //* array of 3 MC labels
    friend ostream& operator<<(ostream& out, const PndFTSCAHitLabel& hl)
    { return out << hl.fLab[0] << " " << hl.fLab[1] << " "<< hl.fLab[2] << " " << std::endl;}
    friend istream& operator>>(istream& in, PndFTSCAHitLabel& hl)
    { return in >> hl.fLab[0] >> hl.fLab[1] >> hl.fLab[2];}
  };

  PndFTSPerformanceBase();
  virtual ~PndFTSPerformanceBase();

  virtual void SetNewEvent( const PndFTSCAGBTracker * const Tracker,
  PndFTSResizableArray<PndFTSCAHitLabel> *hitLabels,
  PndFTSResizableArray<PndFTSCAMCTrack> *mcTracks,
  PndFTSResizableArray<PndFTSCALocalMCPoint> *localMCPoints);
    
    /// Histograms
  virtual void CreateHistos(string histoDir = "", TFile* outFile = 0){ UNUSED_PARAM2(histoDir, outFile); };

    /// Execute standart chain of procedures
  virtual void Exec(bool print = false);

  virtual void PrintEfficiencyStatistic(){ fEffStat.CalcEff(); fEffStat.Print(); };
  virtual void PrintEfficiency()         { fEff.CalcEff();     fEff.Print();     };

  virtual void Draw(){}; // draw diff things after performance
  
    /// Accessors
  PndFTSEfficiencies &GetEff()    { return fEff;     };
  PndFTSEfficiencies &GetEffStat(){ return fEffStat; };
    
  bool IsHistoCreated() { return fIsHistoCreated; }
  void SetHistoCreated(bool v = 1) { fIsHistoCreated = v; }

  void SetHitLabels();
  void SetMCTracks();
  void SetMCPoints();
  
  vector<PndFTSCAPerformanceMCTrackData>   &GetMCData()  { return mcData;   }; 
  vector<PndFTSCAPerformanceRecoTrackData> &GetRecoData(){ return recoData; };
  
 public:

  virtual void FillHistos(){};
  TH1 *GetHisto(const char* name);
    
    // Check if MC track is reconstructable. Calculate set of MC track. Etc.
  virtual void CheckMCTracks(){}; // fill mcData.
    // Find reco-MCTracks correspondence
  virtual void MatchTracks(){};   // fill recoData.
    // Calculate fEfficiencies
  virtual void EfficiencyPerformance();
    // Print fEfficiencies


  int fStatNEvents; //* n of events proceed

    /// Efficiencies
  PndFTSEfficiencies fEff;
  PndFTSEfficiencies fEffStat;


    /// Histos
  int NHisto;
  struct THistoInfo {
    THistoInfo(){};
    THistoInfo( const char *name_, const char *title_, Int_t nx_, Double_t left_, Double_t right_,
                Int_t ny_ = 0, Double_t low_ = 0, Double_t up_ = 0, TString XAxisName_="", TString YAxisName_="")
      :name(name_),title(title_),nx(nx_),left(left_),right(right_),ny(ny_),low(low_),up(up_),XAxisName(XAxisName_),YAxisName(YAxisName_){};
      
    const char *name;
    const char *title;
    Int_t nx;
    Double_t left,right;
    Int_t ny;
    Double_t low,up;
    TString XAxisName, YAxisName;
  };
  TH1 **fHistos; // array of histos
  THistoInfo *fHistosInfo; // array of histos parameters

    /// Track information
  vector<PndFTSCAPerformanceMCTrackData>   mcData;   // iMCTrack to trackInfo map.
  vector<PndFTSCAPerformanceRecoTrackData> recoData; // iRecoTrack to trackInfo map.

    /// Reco information
  const PndFTSCAGBTracker *fTracker; //* pointer to the tracker

    /// MC information
  PndFTSResizableArray<PndFTSCAHitLabel> *fHitLabels; //* array of hit MC labels
  PndFTSResizableArray<PndFTSCAMCTrack> *fMCTracks;   //* array of MC tracks
  PndFTSResizableArray<PndFTSCALocalMCPoint> *fLocalMCPoints;   //* array of MC points in slices CS

  int nRecoTracks, nMCTracks;
    
  TDirectory *fHistoDir; //* ROOT directory with histogramm
  bool fIsHistoCreated;
};

#endif
#endif //DO_TPCCATRACKER_EFF_PERFORMANCE
