#ifndef LHE_TRACK_FITTER_H
#define LHE_TRACK_FITTER_H

// *************************************************************************
//  Author: Oleg Rogachevsky             e-mail: rogach@sunhe.jinr.ru
//   
//  track fitter 
//
// Created: 1-07-07
// Modified:
//
// *************************************************************************

#include "CbmTask.h"
#include "CbmField.h"
#include "CbmTrackParH.h"


#include "PndTpcLheTrack.h"
#include "PndTpcLheTrackCuts.h"
#include "PndTpcLheCMTrack.h"
#include "PndTpcLheCMPoint.h"

class TH2F;
class TGeoTrack;

class PndTpcLheTrackFitter : public CbmTask {

protected:

  CbmField*     fMagField;       //  
  TClonesArray* fTpcTracks;      //
  TClonesArray* fTpcHits;        //!

  TClonesArray* fTpcTrCand;
  TClonesArray* fTpcTrFit;

  TH2F *fXYG, *fXYF,*fYZG, *fYZF,*fXZG, *fXZF,*fPXG,
    *fPXF,*fPYG, *fPYF,*fPZG, *fPZF;

  TClonesArray* fTpcPoints;      //!
  TGeoTrack *fTrCan, *fTrFit;

  TList *fHistList;
  TString fOption;          //  options to choose branches
  Bool_t fVerbose;          // Switch ON/OFF debug messages 
  Bool_t fSimulation;       // Switch simulation diagnostic

  PndTpcLheTrackCuts   *fTrackCuts;       // RO cuts for tracks

  static PndTpcLheTrackFitter* ftInstance;

public:

  virtual void Exec(Option_t * option);
  virtual InitStatus Init();                        //
  virtual void Finish();

  PndTpcLheTrackFitter(const char *name, const char *title="Cbm Task");
  PndTpcLheTrackFitter();
  virtual ~PndTpcLheTrackFitter();


  Int_t DeepFit(PndTpcLheTrack * tr);
  Int_t DeepFitOleg(PndTpcLheTrack * tr);
  Int_t CircleFit(PndTpcLheTrack * tr);
  Int_t HelixFit(PndTpcLheTrack * tr);
  //  void Yvone(PndTpcLheTrack *track, Double_t psib);
  void  Info4Fit(PndTpcLheTrack *track);
  void SetOption(Option_t *option=" ") {fOption = option;  fOption.ToLower();}
  void SaveHistograms();
  void SetVerbose(Bool_t verb)   { fVerbose = verb  ;};
  void SetSimulation(Bool_t sim) { fSimulation = sim; };

  static PndTpcLheTrackFitter* Instance();

ClassDef(PndTpcLheTrackFitter,1)   // PndTpcLheTrackFitter

};

#endif
