/*
 * PndLumiTrackFitter.h
 *
 *  Created on: Aug 14, 2010
 *      Author: tsito
 */

#ifndef PNDLUMITRACKFITTER_H_
#define PNDLUMITRACKFITTER_H_

// Base Class Headers ----------------
#include "FairTask.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

#include "PndLumiLinFitPar.h"

#include <TGraph2D.h>
#include <TGraph2DErrors.h>

// Collaborating Class Headers -------
#include <map>
#include "TString.h"
#include <TVector3.h>

// Collaborating Class Declarations --
class TClonesArray;
class TGeoManager;


void CHISQR(Int_t &npar, Double_t *gin, Double_t &f, Double_t *p, Int_t iflag);

class PndLumiTrackFitter : public FairTask {

public:

  // Constructors/Destructors ---------
  PndLumiTrackFitter();
  PndLumiTrackFitter(Double_t dx, Double_t dy, Int_t iverb);

  virtual ~PndLumiTrackFitter();

  /** Load paramaters for the linear fit */
  virtual void SetParContainers();

  virtual InitStatus ReInit();

  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

private:

  // Input Data------------

	TClonesArray* fLumiTrack;


  // Output Data----------
  TClonesArray* fTrackArray;

  PndLumiLinFitPar* fFitPar;

  Double_t fdX, fdY;

  Int_t fVerbose;

  // Fitting ------------
   double line3Dfit(Double_t* fitpar, Double_t* errors);

   bool IsGoodCandidate(std::vector<TVector3> oneCandidate);

  void AscendingOrder(std::vector<TVector3> oneCandidate, std::vector<TVector3>& orderedvec);

  ClassDef(PndLumiTrackFitter,1);

};
#endif /* PNDLUMITRACKFITTER_H_ */
