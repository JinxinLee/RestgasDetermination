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

#include "PndLheCandidate.h"
#include "PndTpcLheTrackCuts.h"

void fitCircle(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag); // Minimization function for Minuit fit

class PndTpcLheTrackFitter : public FairTask {

protected:

  FairField*     fMagField;       //
  TClonesArray* fPndTracks;      //!
  TClonesArray* fTpcTracks;      //!
  TClonesArray* fTpcHits;        //!
  TClonesArray* fTpcPoints;      //!
  
  TList *fHistList;
  TString fOption;          //  options to choose branches
  Bool_t fVerbose;          // Switch ON/OFF debug messages 
  Bool_t fSimulation;       // Switch simulation diagnostic
  Short_t fCircleFit;       // Circular fit (0 Oleg, 1 TMinuit)
 
  PndTpcLheTrackCuts   *fTrackCuts;       // RO cuts for tracks
  Int_t FastCircleFit(PndLheCandidate *track, Double_t prefit[]);
    
  static PndTpcLheTrackFitter* ftInstance;

public:

  virtual void Exec(Option_t * option);
  virtual InitStatus Init();                        //
  
  PndTpcLheTrackFitter(const char *name, const char *title="Panda Task");
  PndTpcLheTrackFitter();
  virtual ~PndTpcLheTrackFitter();

  void  Register();                 //
  void  Reset();

  Int_t DeepFit(PndLheCandidate * tr);           // Fit of DIP angle (Stefano: fit of r vs z)
  Int_t DeepFitOleg(PndLheCandidate * tr);       // Fit of DIP angle (Oleg: s vs z)
  Int_t CircleFit(PndLheCandidate * tr);         // Circular fit (Oleg)
  Int_t CircleFitMinuit(PndLheCandidate * tr);   // Circular fit (Stefano -> TMinuit)
  Int_t SetUpFitVector(PndLheCandidate* pTrack,
	        TMatrixT<Double32_t> &fitvect); // Setup of values for minuit fit
  Int_t HelixFit(PndLheCandidate * tr);          // Separation of circular and dip fit
  void  Info4Fit(PndLheCandidate *track);        // Folling final values of Track
  void SetOption(Option_t *option=" ") { fOption = option;  fOption.ToLower();}
  void SetVerbose(Bool_t verb)         { fVerbose = verb  ;};
  void SetSimulation(Bool_t sim)       { fSimulation = sim; };
  void SetCircleFit(Short_t fit)       { fCircleFit = fit; };
 
  static PndTpcLheTrackFitter* Instance();

ClassDef(PndTpcLheTrackFitter,2)   // PndTpcLheTrackFitter

};

#endif
