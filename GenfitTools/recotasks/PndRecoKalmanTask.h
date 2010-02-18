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
//      Stefano Spatarot, UNI Torino
//
//-----------------------------------------------------------

#ifndef PNDRECOKALMANTASK_HH
#define PNDRECOKALMANTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include "TString.h"
#include "PndRecoKalmanFit.h"

// Collaborating Class Declarations --
class TClonesArray;
class GFRecoHitFactory;
 

class PndRecoKalmanTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndRecoKalmanTask(const char* name = "Genfit", Int_t iVerbose = 0);
    ~PndRecoKalmanTask();

  // Operators
  

  // Accessors -----------------------
  
  // Modifiers -----------------------
  void SetTrackInBranchName(const TString& name)   { fTrackInBranchName = name; } 
  void SetTrackOutBranchName(const TString& name)  { fTrackOutBranchName = name; }
  void SetPersistence(Bool_t opt = kTRUE)        { fPersistence = opt;      }
  void SetGeane(Bool_t opt = kTRUE)              { fUseGeane = opt;         }
  void SetNumIterations(Int_t num)               { fNumIt = num;        }
 
  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);
  
  
private:

  // Private Data Members ------------
  TClonesArray* fTrackArray; 
  TClonesArray* fFitTrackArray;    //! Output TCA for track
  
  TString fTrackInBranchName;      //! Name of the input TCA
  TString fTrackOutBranchName;     //! Name of the output TCA
   
  PndRecoKalmanFit *fFitter;
  
  Bool_t fPersistence;

  Bool_t fUseGeane;     //! Flag to use Geane 
  Bool_t fSmoothing;    //! Flag to set on smoothing
  Int_t fNumIt;         //! Number of iterations

  ClassDef(PndRecoKalmanTask,1);

};

#endif
