#ifndef LHE_TRACK_FITTER_H
#define LHE_TRACK_FITTER_H

// *************************************************************************
//  Author: Oleg Rogachevsky             e-mail: rogach@sunhe.jinr.ru
//   
//  track fitter 
//
// Created: 1-07-07
// Modified: 23-04-09 Stefano Spataro (stefano.spataro@to.infn.it)
//
// *************************************************************************

#include "FairTask.h"
#include "FairField.h"
#include "FairTrackParH.h"


#include "PndTpcLheTrack.h"
#include "PndTpcLheTrackCuts.h"
#include "PndTpcLheCMTrack.h"
#include "PndTpcLheCMPoint.h"

class PndTpcLheTrackFitter : public FairTask {

protected:

  FairField*     fMagField;       //  
  TClonesArray* fTpcTracks;      //
  TClonesArray* fTpcHits;        //!
  TClonesArray* fTpcPoints;      //!
  
  TList *fHistList;
  TString fOption;          //  options to choose branches
  Bool_t fVerbose;          // Switch ON/OFF debug messages 
  Bool_t fSimulation;       // Switch simulation diagnostic

  PndTpcLheTrackCuts   *fTrackCuts;       // RO cuts for tracks

  static PndTpcLheTrackFitter* ftInstance;

public:

  virtual void Exec(Option_t * option);
  virtual InitStatus Init();                        //
  
  PndTpcLheTrackFitter(const char *name, const char *title="Panda Task");
  PndTpcLheTrackFitter();
  virtual ~PndTpcLheTrackFitter();


  Int_t DeepFit(PndTpcLheTrack * tr);
  Int_t DeepFitOleg(PndTpcLheTrack * tr);
  Int_t CircleFit(PndTpcLheTrack * tr);
  Int_t HelixFit(PndTpcLheTrack * tr);
  void  Info4Fit(PndTpcLheTrack *track);
  void SetOption(Option_t *option=" ") {fOption = option;  fOption.ToLower();}
  void SetVerbose(Bool_t verb)   { fVerbose = verb  ;};
  void SetSimulation(Bool_t sim) { fSimulation = sim; };

  static PndTpcLheTrackFitter* Instance();

ClassDef(PndTpcLheTrackFitter,2)   // PndTpcLheTrackFitter

};

#endif
