//-*- Mode: C++ -*-
// $Id: PndCAPerformance.h,v 1.9 2010/09/01 10:38:27 ikulakov Exp $
// ************************************************************************
// This file is property of and copyright by the ALICE HLT Project        *
// ALICE Experiment at CERN, All rights reserved.                         *
// See cxx source for full Copyright notice                               *
//                                                                        *
//*************************************************************************
#ifdef DO_TPCCATRACKER_EFF_PERFORMANCE

#ifndef PNDCAPERFORMANCE_H
#define PNDCAPERFORMANCE_H

#include "PndCAPerformanceBase.h"

#include "PndCADef.h"
#include "PndCAMCTrack.h"
#include "PndCAMCPoint.h"

#include <fstream>
#include <cstdio>
#include <map>
#include <string>
using std::string;

class TObject;
class TParticle;
class PndCAHit;
class PndCAMCPoint;
class PndCAGBTracker;
class PndCATopoReconstructor;
class TDirectory;
class TH1D;
class TH2D;
class TProfile;

/**
 * @class PndCAPerformance
 *
 * Does performance evaluation of the HLT Cellular Automaton-based tracker
 * It checks performance for PndCATracker slice tracker
 * and for PndCAGBTracker global tracker
 *
 */

class PndCAPerformance
{
  public:

    typedef PndCAPerformanceBase::PndCAHitLabel PndCAHitLabel;

    PndCAPerformance();
    virtual ~PndCAPerformance();

      /// initialization before the new event
    bool SetNewEvent(PndCAGBTracker* const Tracker, string mcTracksFile, string mcPointsFile); // set info for new event
    void InitSubPerformances();

      /// Instance
    static PndCAPerformance &Instance();
    
      /// Efficiencies
    void ExecPerformance();

    /// functional is needed by DRAW option. TODO: clean up
  const PndCAMCTrack &MCTrack(int i) const { return fMCTracks[i]; }
  const PndCAHitLabel &HitLabel(int i) const { return fHitLabels[i]; }
  const PndCAGBTracker *GetTracker(){ return fTracker; };
    

  void SetTracker( PndCAGBTracker* const tracker ){ fTracker = tracker; };
  void SetTopoReconstructor( PndCATopoReconstructor* const tr ){ fTopoReconstructor = tr; };
  void SetMCTracks(vector<PndCAMCTrack>& mcTracks);
  void SetMCPoints(vector<PndCALocalMCPoint>& mcPoints);
  void SetHitLabels(vector<PndCAHitLabel>& hitLabels);

  vector<PndCAHitLabel>     * GetHitLabels() { return &fHitLabels; }      // array of hit MC labels
  vector<PndCAMCTrack>      * GetMCTracks()  { return &fMCTracks;  }      // array of MC tracks
  vector<PndCALocalMCPoint> * GetMCPoints()  { return &fLocalMCPoints;}   // array of MC points in slices CS

  PndCAPerformanceBase* GetSubPerformance(string name);

  const double* PV() const { return fPV; }

  
  bool CreateHistos(string name);
  void WriteHistos();
  
  void SetOutputFile(TFile *oF) { fOutputFile = oF; }

  void SaveDataInFiles( string prefix ) const; // Save all MC Data in txt files. @prefix - prefix for file name. Ex: "./data/ev1"
  bool ReadDataFromFiles( string prefix ); // @prefix - prefix for file name. Ex: "./data/ev1"


  void CombineHits(); // Rid of hits with same wires positions
  
  protected:

          /// Histograms
    void CreateHistos();
    
          /// Read\write MC information
    void ReadMCEvent( FILE *in );
    void ReadLocalMCPoints( FILE *in );

    void WriteMCEvent( FILE *out ) const;

      /// Sub-pefromances
    struct TSubPerformance{
      PndCAPerformanceBase* perf;
      string name;
      bool IsGlobalPerf;

      TSubPerformance(){};
      TSubPerformance(PndCAPerformanceBase* perf_, string name_, bool IsGlobalPerf_ = 1){
        perf = perf_;
        name = name_;
	IsGlobalPerf = IsGlobalPerf_;
	perf->SetHistoCreated(0);
      };
//       ~TSubPerformance(){if (perf) delete perf;};
      
      PndCAPerformanceBase& operator*(){return *perf;}
      PndCAPerformanceBase* operator->(){return perf;}
    };
    vector<TSubPerformance> subPerformances;
//     vector<PndCAPerformanceBase*> subPerformances;
    
    const PndCAGBTracker *fTracker; // pointer to the tracker
    const PndCATopoReconstructor* fTopoReconstructor;
       /// MC information
    vector<PndCAHitLabel> fHitLabels; // array of hit MC labels
    vector<PndCAMCTrack> fMCTracks;   // array of MC tracks
    vector<PndCALocalMCPoint> fLocalMCPoints;   // array of MC points in slices CS
  
    double fPV[3]; // primary vertex position (x,y,z)


    int fStatNEvents; // n of events proceed

    TFile *fOutputFile;
    TDirectory *fHistoDir; // ROOT directory with histogramms
  
  private:
    void WriteDir2Current( TObject *obj );
  
    PndCAPerformance( const PndCAPerformance& );
    PndCAPerformance &operator=( const PndCAPerformance& );

};

#endif
#endif //DO_TPCCATRACKER_EFF_PERFORMANCE
