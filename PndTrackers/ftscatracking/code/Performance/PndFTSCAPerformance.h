//-*- Mode: C++ -*-
// $Id: PndFTSCAPerformance.h,v 1.9 2010/09/01 10:38:27 ikulakov Exp $
// ************************************************************************
// This file is property of and copyright by the ALICE HLT Project        *
// ALICE Experiment at CERN, All rights reserved.                         *
// See cxx source for full Copyright notice                               *
//                                                                        *
//*************************************************************************
#ifdef DO_TPCCATRACKER_EFF_PERFORMANCE

#ifndef PNDFTSCAPERFORMANCE_H
#define PNDFTSCAPERFORMANCE_H

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

class TObject;
class TParticle;
class PndFTSCAGBHit;
class PndFTSCAMCPoint;
class PndFTSCAGBTracker;
class PndFTSTopoReconstructor;
class TDirectory;
class TH1D;
class TH2D;
class TProfile;

/**
 * @class PndFTSCAPerformance
 *
 * Does performance evaluation of the HLT Cellular Automaton-based tracker
 * It checks performance for PndFTSCATracker slice tracker
 * and for PndFTSCAGBTracker global tracker
 *
 */

class PndFTSCAPerformance
{
 public:

  typedef PndFTSPerformanceBase::PndFTSCAHitLabel PndFTSCAHitLabel;

  PndFTSCAPerformance();
  virtual ~PndFTSCAPerformance();

    /// initialization before the new event
  bool SetNewEvent(PndFTSCAGBTracker* const Tracker, string mcTracksFile, string mcPointsFile); // set info for new event
  void InitSubPerformances();

    /// Instance
  static PndFTSCAPerformance &Instance();
  
    /// Efficiencies
  void ExecPerformance();

    /// functional is needed by DRAW option. TODO: clean up
  const PndFTSCAMCTrack &MCTrack(int i) const { return fMCTracks[i]; }
  const PndFTSCAHitLabel &HitLabel(int i) const { return fHitLabels[i]; }
  const PndFTSCAGBTracker *GetTracker(){ return fTracker; };
    

  void SetTracker( PndFTSCAGBTracker* const tracker ){ fTracker = tracker; };
  void SetTopoReconstructor( PndFTSTopoReconstructor* const tr ){ fTopoReconstructor = tr; };
  void SetMCTracks(vector<PndFTSCAMCTrack>& mcTracks);
  void SetMCPoints(vector<PndFTSCALocalMCPoint>& mcPoints);
  void SetHitLabels(vector<PndFTSCAHitLabel>& hitLabels);

  PndFTSResizableArray<PndFTSCAHitLabel>     * GetHitLabels() { return &fHitLabels; }      // array of hit MC labels
  PndFTSResizableArray<PndFTSCAMCTrack>      * GetMCTracks()  { return &fMCTracks;  }      // array of MC tracks
  PndFTSResizableArray<PndFTSCALocalMCPoint> * GetMCPoints()  { return &fLocalMCPoints;}   // array of MC points in slices CS

  PndFTSPerformanceBase* GetSubPerformance(string name);

  const double* PV() const { return fPV; }

  
  bool CreateHistos(string name);
  void WriteHistos();
  
  void SetOutputFile(TFile *oF) { fOutputFile = oF; }

  void SaveDataInFiles( string prefix ) const; // Save all MC Data in txt files. @prefix - prefix for file name. Ex: "./data/ev1"
  bool ReadData( vector <int>& labels, vector <PndFTSCALocalMCPoint>& mcpoints, vector<PndFTSCAMCTrack>& mctracks );

  int GetMCPoint( const PndFTSCAGBHit& hit ) const;

  void ShiftHitsToMC( float errX = -1, float errY = -1 ); // Use spreaded MCposition instead of hits
  void ResimulateHits( float errX = -1, float errY = -1 ); // Create new hits spreading MCPositions
  void RematchHits();// Match hits with closest MCPoint
#ifdef DRIFT_TUBES
  void CombineHits(); // Rid of hits with same wires positions
  void DivideHitsOnLR();
#endif
  
  /// Histograms
  void CreateHistos();

 protected:    
        /// Read\write MC information
  void ReadMCEvent( FILE *in );
  void ReadLocalMCPoints( FILE *in );

  void WriteMCEvent( FILE *out ) const;

    /// Sub-pefromances
  struct TSubPerformance{
    PndFTSPerformanceBase* perf;
    string name;
    bool IsGlobalPerf;

    TSubPerformance(){};
    TSubPerformance(PndFTSPerformanceBase* perf_, string name_, bool IsGlobalPerf_ = 1){
      perf = perf_;
      name = name_;
      IsGlobalPerf = IsGlobalPerf_;
      perf->SetHistoCreated(0);
    };
//       ~TSubPerformance(){if (perf) delete perf;};
    
    PndFTSPerformanceBase& operator*(){return *perf;}
    PndFTSPerformanceBase* operator->(){return perf;}
  };
  vector<TSubPerformance> subPerformances;
//     vector<PndFTSCAPerformanceBase*> subPerformances;
  
  const PndFTSCAGBTracker *fTracker; // pointer to the tracker
  const PndFTSTopoReconstructor* fTopoReconstructor;
      /// MC information
  PndFTSResizableArray<PndFTSCAHitLabel> fHitLabels; // array of hit MC labels
  PndFTSResizableArray<PndFTSCAMCTrack> fMCTracks;   // array of MC tracks
  PndFTSResizableArray<PndFTSCALocalMCPoint> fLocalMCPoints;   // array of MC points in slices CS

  double fPV[3]; // primary vertex position (x,y,z)


  int fStatNEvents; // n of events proceed

  TFile *fOutputFile;
  TDirectory *fHistoDir; // ROOT directory with histogramms

 private:
  void WriteDir2Current( TObject *obj );

  PndFTSCAPerformance( const PndFTSCAPerformance& );
  PndFTSCAPerformance &operator=( const PndFTSCAPerformance& );

};

#endif
#endif //DO_TPCCATRACKER_EFF_PERFORMANCE
