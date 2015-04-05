//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Kalman Filter for single tracks
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Stefano Spataro, UNI Torino      //
//      modified by Elisabetta Prencipe 19/05/2014
//-----------------------------------------------------------

#ifndef PNDRECOKALMANFIT2_HH
#define PNDRECOKALMANFIT2_HH

// Base Class Headers ----------------
#include "TNamed.h"
#include "TString.h"

// Collaborating Class Headers ------
#include "FairGeanePro.h"
#include "PndTrack.h"
#include "KalmanFitter.h"
#include "AbsMeasurement.h"
#include "MeasurementFactory.h"
// Collaborating Class Declarations --

class PndRecoKalmanFit2 : public TNamed 
{
 public:
  
  // Constructors/Destructors ---------
  PndRecoKalmanFit2();
  ~PndRecoKalmanFit2();
  
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

  genfit::MeasurementFactory<genfit::AbsMeasurement>* GetRecoHitFactory() { return fTheRecoHitFactory;};
  
private:
  
  // Private Data Members ------------


  genfit::MeasurementFactory<genfit::AbsMeasurement>* fTheRecoHitFactory;
  genfit::KalmanFitter fGenFitter;
  
  FairGeanePro* fPro;   //! Geane Propagator
 
  TString fMvdBranchName;           //! Name of the TCA for MVD
  TString fCentralTrackerBranchName;//! Name of the TCA for central tracker
  
  Bool_t fUseGeane;     //! Flag to use Geane
  Bool_t fPropagateToIP;//! Flag to propagate to the interaction point
  Bool_t fPerpPlane;    //! Flag to use as initial plane the one perpendicular to the track 
  Int_t fNumIt;         //! Number of iterations
  Int_t fVerbose;       //! Verbose level
  
  ClassDef(PndRecoKalmanFit2,0);

};

#endif
