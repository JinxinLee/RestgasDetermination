//-*- Mode: C++ -*-
// ************************************************************************
// This file is property of and copyright by the ALICE HLT Project        *
// ALICE Experiment at CERN, All rights reserved.                         *
// See cxx source for full Copyright notice                               *
//                                                                        *
//*************************************************************************
#ifdef DO_TPCCATRACKER_EFF_PERFORMANCE

#ifndef PNDCAPERFORMANCEBASE_H
#define PNDCAPERFORMANCEBASE_H

#include "PndCACounters.h"
#include "PndCAMCTrack.h"
#include "PndCAMCPoint.h"

//#include <fstream>
//#include <cstdio>

#include <vector>

#include <string>
using std::string;

#include "TString.h"

#include <iostream>
using std::ostream;
using std::istream;

//class TObject;
//class TParticle;
//class PndCAMCPoint;
class PndCAGBTracker;
class TDirectory;
class TH1;

class TFile;

/**
 * @class PndCAPerformanceBase
 */
class PndCAPerformanceBase
{
 public:

  struct PndCAHitLabel {
    int fLab[3]; //* array of 3 MC labels
    friend ostream& operator<<(ostream& out, const PndCAHitLabel& hl)
    { return out << hl.fLab[0] << " " << hl.fLab[1] << " "<< hl.fLab[2] << " " << std::endl;}
    friend istream& operator>>(istream& in, PndCAHitLabel& hl)
    { return in >> hl.fLab[0] >> hl.fLab[1] >> hl.fLab[2];}
  };

  PndCAPerformanceBase();
  virtual ~PndCAPerformanceBase();

  virtual void SetNewEvent( const PndCAGBTracker * const Tracker,
  vector<PndCAHitLabel> *hitLabels,
  vector<PndCAMCTrack> *mcTracks,
  vector<PndCALocalMCPoint> *localMCPoints);
    
    /// Histograms
  virtual void CreateHistos(string histoDir = "", TFile* outFile = 0){ UNUSED_PARAM2(histoDir, outFile); };

    /// Execute standart chain of procedures
  virtual void Exec(bool print = false);

  virtual void PrintEfficiencyStatistic(){ fEffStat.CalcEff(); fEffStat.Print(); };
  virtual void PrintEfficiency()         { fEff.CalcEff();     fEff.Print();     };

  virtual void Draw(){}; // draw diff things after performance
  
    /// Accessors
  PndCAEfficiencies &GetEff()    { return fEff;     };
  PndCAEfficiencies &GetEffStat(){ return fEffStat; };
    
  bool IsHistoCreated() { return fIsHistoCreated; }
  void SetHistoCreated(bool v = 1) { fIsHistoCreated = v; }

  vector<PndCAPerformanceMCTrackData>   &GetMCData()  { return mcData;   }; 
  vector<PndCAPerformanceRecoTrackData> &GetRecoData(){ return recoData; };
  
 protected:

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
  PndCAEfficiencies fEff;
  PndCAEfficiencies fEffStat;


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
  vector<PndCAPerformanceMCTrackData>   mcData;   // iMCTrack to trackInfo map.
  vector<PndCAPerformanceRecoTrackData> recoData; // iRecoTrack to trackInfo map.

    /// Reco information
  const PndCAGBTracker *fTracker; //* pointer to the tracker

    /// MC information
  vector<PndCAHitLabel> *fHitLabels; //* array of hit MC labels
  vector<PndCAMCTrack> *fMCTracks;   //* array of MC tracks
  vector<PndCALocalMCPoint> *fLocalMCPoints;   //* array of MC points in slices CS

  int nRecoTracks, nMCTracks;
    
  TDirectory *fHistoDir; //* ROOT directory with histogramm
  bool fIsHistoCreated;
};

#endif
#endif //DO_TPCCATRACKER_EFF_PERFORMANCE
