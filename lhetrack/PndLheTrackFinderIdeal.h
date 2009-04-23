#ifndef LHE_TRACK_FINDERIDEAL_H
#define LHE_TRACK_FINDERIDEAL_H

// *************************************************************************
//  Author: Stefano Spataro e-mail: stefano.spataro@exp2.physik.uni-giessen.de
//   
//  ideal track finder using MC id
//
// Created: 14-04-09
// Modified:
//
// *************************************************************************

#include "TObject.h"
#include "TObjArray.h"
#include "TArrayI.h"
#include "TBenchmark.h"
#include "Riostream.h"

#include "FairTask.h"

#include "PndTpcLheCMPoint.h"
#include "PndTpcLheCMTrack.h"
#include "PndTpcLheTrack.h"
#include "PndTpcLheSegments.h"
#include "PndTpcLheTrackFitter.h"

class PndLheTrackFinderIdeal : public FairTask {

protected:

  PndTpcLhePoint  *fVertex;       // pointer to the vertex

  TClonesArray  *fCMHits;       // Array of hits transformed with conformal mapping
  TObjArray     *fCMTracks;     // Array of tracks

  TClonesArray  *fLheHits;         // Array of event's hits
  TClonesArray  *fFoundTracks;  // Array of found tracks

  TString fOption;              //  options for operation
  Bool_t fVerbose;              // Switch ON/OFF debug messages (deafault OFF)
  TBenchmark   *fBench;         // benchmark object (just for run-time measurements)
  Float_t    mTime;          // total time consumption
  
  PndTpcLheTrack* AddTrack(PndTpcLheTrack* track);
  
public:

  PndLheTrackFinderIdeal();            //
  PndLheTrackFinderIdeal(const char *name, const char *title="CBM Task");
  virtual  ~PndLheTrackFinderIdeal();  //

  virtual void Exec(Option_t * option);
  virtual InitStatus Init();              // 
  virtual void Finish();        // 

  void  Reset();                    //
  void  Register();                 //
  void SetOption(Option_t *option=" ") {fOption = option;  fOption.ToLower();}
  void SetVerbose(Bool_t verb)   { fVerbose = verb; };
  // getters

 
  ClassDef(PndLheTrackFinderIdeal, 1)          //  
    
    };
#endif
