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

#include "FairTask.h"

#include "PndLheHit.h"
#include "PndLheCMPoint.h"
#include "PndLheCMCandidate.h"
#include "PndLheTrackCuts.h"
#include "PndLheSegments.h"
#include "PndLheTrackFitter.h"

class PndLheTrackFinder : public FairTask {

protected:

  enum Momentum {HIGH, INTERMED, LOW};

  PndLhePoint  *fVertex;       // pointer to the vertex

  TClonesArray  *fCMHits;       // Array of hits transformed with conformal mapping
  TObjArray     *fCMTracks;     // Array of tracks

  PndLheSegments *fSegment;      //
  TObjArray      *fSegments;     // segments with hits

  TClonesArray  *fLheHits;         // Array of event's hits
  TClonesArray  *fGeantTracks;  // Array of geant tracks
  TClonesArray  *fFoundTracks;  // Array of found tracks

  TString fOption;              //  options for operation
  Bool_t fVerbose;              // Switch ON/OFF debug messages (deafault OFF)
  Bool_t fCanPersistence;       // Persistence of PndLheCandidate
  Bool_t fCMPersistence;        // Persistence of PndCMPoint
  TBenchmark   *fBench;         // benchmark object (just for run-time measurements)
  Float_t    mTime;          // total time consumption

  // Cuts
  PndLheTrackCuts*     fTrackCuts;       // cuts for tracks

public:

  PndLheTrackFinder();            //
  PndLheTrackFinder(const char *name, const char *title="CBM Task");
  virtual  ~PndLheTrackFinder();  //

  virtual void Exec(Option_t * option);
  virtual InitStatus Init();              // 
  virtual void Finish();        // 

  void  Reset();                    //
  void  Register();                 //
  void SetOption(Option_t *option=" ") {fOption = option;  fOption.ToLower();}
  void SetVerbose(Bool_t verb)   { fVerbose = verb; };
  void SetCanPersistence(Bool_t pers)  { fCanPersistence = pers; };
  void SetCMPersistence(Bool_t pers)   { fCMPersistence = pers;  };

  // getters
  void GetPhiRange(TArrayI* c, PndLheCMCandidate *cmtrack);
  void GetThetaRange(TArrayI* c, PndLheCMCandidate *cmtrack);
  void CopyClones(TClonesArray* cl1, TClonesArray* cl2);

  PndLhePoint  *GetVertex()     { return fVertex;}
  Int_t   GetNumberOfHits()     { return fCMHits->GetEntriesFast();}
  TObjArray  *GetTracks()       { return fCMTracks;}

  // setters:

  // Tracking procedures:
  void  DoTracking();              // tracking of main vertex tracks
  void  LoopOverHits();                        // loops over hits
  void  CreateTrack(PndLheCMPoint *hit);   // create track with start at hit
  void  RemoveTrack(PndLheCMCandidate *track);
  void  GetClosestHit(PndLheCMCandidate *tr, Int_t st, Bool_t back);
  Bool_t TrackExtension(PndLheCMCandidate *track);  //
  void AddTrackForFit(PndLheCMCandidate *track);     //
  void  TrackingInfo(char* info);           // information about the tracking 
  void CheckClones();

  ClassDef(PndLheTrackFinder, 1)          //  
    
    };

//________________________________________________________________
inline void PndLheTrackFinder::RemoveTrack(PndLheCMCandidate *track) {
  //--- Removes track from TObjArray and release the points.

  //  track->ClearPoints();        // release points
  fCMTracks->Remove(track);        // remove from TObjArray
  delete track;                    // delete track
  track = NULL;

}

#endif
