#ifndef LHE_TRACK_FINDER_H
#define LHE_TRACK_FINDER_H

// *************************************************************************
//  Author: Oleg Rogachevsky             e-mail: rogach@sunhe.jinr.ru
//   
//  track finder with conformal mapping 
//
// Created: 1-07-07
// Modified:
//
// *************************************************************************

#include "TObject.h"
#include "TObjArray.h"
#include "TArrayI.h"
#include "TBenchmark.h"
#include "Riostream.h"

#include "CbmTask.h"

#include "PndTpcLheHit.h"
#include "PndTpcLheCMPoint.h"
#include "PndTpcLheCMTrack.h"
#include "PndTpcLheTrackCuts.h"
#include "PndTpcLheSegments.h"
#include "PndTpcLheTrackFitter.h"
#include "lhe.h"

class PndTpcLheTrackFinder : public CbmTask {

protected:

  enum Momentum {HIGH, INTERMED, LOW};

  PndTpcLheTrackFitter *fFitter;
  PndTpcLhePoint  *fVertex;       // pointer to the vertex

  TClonesArray  *fCMHits;       // Array of hits transformed with conformal mapping
  TObjArray     *fCMTracks;     // Array of tracks

  PndTpcLheSegments *fSegment;      //
  TObjArray      *fSegments;     // segments with hits

  TClonesArray  *fLheHits;         // Array of event's hits
  TClonesArray  *fGeantTracks;  // Array of geant tracks
  TClonesArray  *fFoundTracks;  // Array of found tracks

  TString fOption;              //  options for operation
  Bool_t fVerbose;              // Switch ON/OFF debug messages (deafault OFF)
  TBenchmark   *fBench;         // benchmark object (just for run-time measurements)
  Float_t    mTime;          // total time consumption

  // Cuts
  PndTpcLheTrackCuts*     fTrackCuts;       // cuts for tracks

public:

  PndTpcLheTrackFinder();            //
  PndTpcLheTrackFinder(const char *name, const char *title="CBM Task");
  virtual  ~PndTpcLheTrackFinder();  //

  virtual void Exec(Option_t * option);
  virtual InitStatus Init();              // 
  virtual void Finish();        // 

  void  Reset();                    //
  void  Register();                 //
  void SetOption(Option_t *option=" ") {fOption = option;  fOption.ToLower();}
  void SetVerbose(Bool_t verb)   { fVerbose = verb; };
  // getters


  void GetPhiRange(TArrayI* c, PndTpcLheCMTrack *cmtrack);
  void GetThetaRange(TArrayI* c, PndTpcLheCMTrack *cmtrack);
  void CopyClones(TClonesArray* cl1, TClonesArray* cl2);

  PndTpcLhePoint  *GetVertex()     { return fVertex;}
  Int_t   GetNumberOfHits()     { return fCMHits->GetEntriesFast();}
  TObjArray  *GetTracks()       { return fCMTracks;}

  // setters:

  // Tracking procedures:
  void  DoTracking();              // tracking of main vertex tracks
  void  LoopOverHits();                        // loops over hits
  void  CreateTrack(PndTpcLheCMPoint *hit);   // create track with start at hit
  void  RemoveTrack(PndTpcLheCMTrack *track);
  void  GetClosestHit(PndTpcLheCMTrack *tr, Int_t st, Bool_t back);
  Bool_t TrackExtension(PndTpcLheCMTrack *track);  //
  void AddTrackForFit(PndTpcLheCMTrack *track);     //
  void  TrackingInfo(char* info);           // information about the tracking 
  void CheckClones();

  ClassDef(PndTpcLheTrackFinder, 1)          //  
    
    };

//________________________________________________________________
inline void PndTpcLheTrackFinder::RemoveTrack(PndTpcLheCMTrack *track) {
  //--- Removes track from TObjArray and release the points.

  //  track->ClearPoints();        // release points
  fCMTracks->Remove(track);        // remove from TObjArray
  delete track;                    // delete track
  track = NULL;

}

#endif
