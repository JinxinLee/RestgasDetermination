//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Kalman Filter Task
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Ralf Kliemt, TU Dresden             (Copied for MVD use)
//      Radoslaw Karabowicz, GSI            (converted for FWDs)
//
//-----------------------------------------------------------

#ifndef PNDFWDKALMANTASK_H
#define PNDFWDKALMANTASK_H

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include <map>
#include "TString.h"
#include "FairGeanePro.h"

// Collaborating Class Declarations --
class TClonesArray;
class RecoHitFactory;
class TH1D;
class TH2D;
class TGeoManager;

class PndFwdKalmanTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndFwdKalmanTask();
  virtual ~PndFwdKalmanTask();

  // Operators


  // Accessors -----------------------


  // Modifiers -----------------------
  void SetTrackBranchName(const TString& name) {fTrackBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {fPersistence=opt;}

  /** public method SetNumIterations(Int_t i) **/
  void SetNumIterations(Int_t i){fNumIt=i;}

  // Operations ----------------------

  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  virtual void Finish();

  void WriteHistograms();

private:

  // Private Data Members ------------
  TClonesArray* fTrackArray;
  TString fTrackBranchName;

  Bool_t fPersistence;
  Int_t fNumIt;          ///< number of iterations

  RecoHitFactory* fTheRecoHitFactory;


  TH1D* fPH; // momentum histo;
  TH1D* fChi2H; // chi2 histo;
  TH1D* fMassV0; // chi2 histo;
  TH1D* fMassETAC; // chi2 histo;
  TH1D* fMasses; // all particle histo;

  TH2D* fhMomentumX2D;
  TH2D* fhMomentumY2D;
  TH2D* fhMomentumZ2D;
  TH2D* fhMomentumM2D;
  TH1D* fhMomentumX1D;
  TH1D* fhMomentumY1D;
  TH1D* fhMomentumZ1D;
  TH1D* fhMomentumM1D;
  TH2D* fhMomentumRes;

  TH2D* fhPositionX2D;
  TH2D* fhPositionY2D;
  TH1D* fhPositionX1D;
  TH1D* fhPositionY1D;

  Int_t fTrackcount;

  Int_t fEvent;
  TGeant3 *gMC3;
  FairGeanePro *fPro;

  // Private Methods -----------------

//public:
  ClassDef(PndFwdKalmanTask,1);

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
