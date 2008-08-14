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
//      Stefano Spatarot, JLU Gießen        (Copied for LHE use)
//
//-----------------------------------------------------------

#ifndef PNDLHEKALMANTASK_HH
#define PNDLHEKALMANTASK_HH

// Base Class Headers ----------------
#include "CbmTask.h"

// Collaborating Class Headers -------
#include <map>
#include "TString.h"
#include "CbmGeanePro.h"

// Collaborating Class Declarations --
class TClonesArray;
class PndTpcLheTrack;
class RecoHitFactory;
class TrackCand;
class TH1D;
class Track;
class TGeoManager; 

class PndLheKalmanTask : public CbmTask {
public:

  // Constructors/Destructors ---------
  PndLheKalmanTask(const char* name = "LheGenfit", Int_t iVerbose = 0);
    ~PndLheKalmanTask();

  // Operators
  

  // Accessors -----------------------
  
  // Modifiers -----------------------
  void SetTrackBranchName(const TString& name)   { fTrackBranchName = name; }
  void SetPersistence(Bool_t opt = kTRUE)        { fPersistence = opt;      }
  void SetGeane(Bool_t opt = kTRUE)              { fUseGeane = opt;         }
  void SetSmooth(Bool_t opt = kTRUE)             { fSmoothing = opt;        }
  void SetNumIterations(Int_t num)               { fNumIt = num;        }
 
  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);
  
  void WriteHistograms(const TString& filename);

 
  
private:

  // Private Data Members ------------
  TClonesArray* fTrackArray; 
  TClonesArray* fFitTrackArray;    //! Output TCA for track
  
  TString fTrackBranchName;

  Bool_t fPersistence;

  RecoHitFactory* fTheRecoHitFactory;
  

  TH1D* fPH; // momentum histo;
  TH1D* fChi2H; // chi2 histo;
  TH1D* fMassV0; // chi2 histo;
  TH1D* fMassETAC; // chi2 histo;
  TH1D* fMasses; // all particle histo;

  Int_t fTrackcount;
  Int_t fEvent;
  TGeant3 *gMC3;
  CbmGeanePro *fPro;

  Bool_t fUseGeane;     //! Flag to use Geane 
  Bool_t fSmoothing;    //! Flag to set on smoothing
  Int_t fNumIt;         //! Number of iterations

  Track* AddTrack(Track* track);

  ClassDef(PndLheKalmanTask,1);

};

#endif
