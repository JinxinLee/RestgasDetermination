//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Kalman Filter for single tracks, deterministic annealing
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//
//      Stefano Spataro, UNI Torino      //
//      modified by Elisabetta Prencipe, 19/05/2014
//-----------------------------------------------------------

#ifndef PNDRECODAFFIT2_HH
#define PNDRECODAFFIT2_HH

// Base Class Headers ----------------
#include "TNamed.h"
#include "TString.h"

// Collaborating Class Headers ------
#include "FairGeanePro.h"
#include "PndTrack.h"
#include "DAF.h"
#include "MeasurementFactory.h"


class PndRecoDafFit2 : public TNamed 
{
 public:
  
  // Constructors/Destructors ---------
  PndRecoDafFit2();
  ~PndRecoDafFit2();
  
  // Modifiers -----------------------
  void SetGeane(Bool_t opt = kTRUE)              { fUseGeane = opt;     }
  void SetPropagateToIP(Bool_t opt = kTRUE)      { fPropagateToIP = opt;}
  void SetPerpPlane(Bool_t opt = kTRUE)          { fPerpPlane = opt;    }
  void SetNumIterations(Int_t num)               { fNumIt    = num;     } 
  void SetVerbose(Int_t verb)                    { fVerbose  = verb;    }
  void SetMvdBranchName(const TString& name)             { fMvdBranchName = name; }
  void SetCentralTrackerBranchName(const TString& name)  { fCentralTrackerBranchName = name; }
  // Operations ---------------------- 
  Bool_t Init();
  PndTrack*  Fit(PndTrack *tBefore, Int_t PDG);
  
private:
  
  // Private Data Members ------------
  genfit::MeasurementFactory<genfit::AbsMeasurement>* fTheRecoHitFactory;
  genfit::DAF fGenFitter;
  
  FairGeanePro* fPro;   //! Geane Propagator
 
  TString fMvdBranchName;           //! Name of the TCA for MVD
  TString fCentralTrackerBranchName;//! Name of the TCA for central tracker
  
  Bool_t fUseGeane;     //! Flag to use Geane 
  Bool_t fPropagateToIP;//! Flag to propagate to the interaction point
  Bool_t fPerpPlane;    //! Flag to use as initial plane the one perpendicular to the track 
  Int_t fNumIt;         //! Number of iterations
  Int_t fVerbose;       //! Verbose level
  
  ClassDef(PndRecoDafFit2,0);

};

#endif
