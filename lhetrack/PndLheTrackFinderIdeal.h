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

#include "PndLheCMPoint.h"
#include "PndLheCandidate.h"

class PndLheTrackFinderIdeal : public FairTask {

protected:

  PndLhePoint  *fVertex;       // pointer to the vertex

  TClonesArray  *fCMHits;       // Array of hits transformed with conformal mapping
  TObjArray     *fCMTracks;     // Array of tracks

  TClonesArray  *fLheHits;         // Array of event's hits
  TClonesArray  *fFoundTracks;  // Array of found tracks

  TString fOption;              //  options for operation
  Bool_t fVerbose;              // Switch ON/OFF debug messages (deafault OFF)
  Bool_t fCanPersistence;       // Persistence of PndLheCandidate
  Bool_t fCMPersistence;        // Persistence of PndCMPoint
  TBenchmark   *fBench;         // benchmark object (just for run-time measurements)
  Float_t    mTime;          // total time consumption
  
  PndLheCandidate* AddTrack(PndLheCandidate* track);
  
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
  void SetVerbose(Bool_t verb)         { fVerbose = verb; };
  void SetCanPersistence(Bool_t pers)  { fCanPersistence = pers; };
  void SetCMPersistence(Bool_t pers)   { fCMPersistence = pers;  };
 
  ClassDef(PndLheTrackFinderIdeal, 1)          //  
    
    };
#endif
