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
//      Ralf Kliemt,     UNI Bonn        //
//-----------------------------------------------------------

#ifndef PNDFTSKALMANFIT_HH
#define PNDFTSKALMANFIT_HH

// Base Class Headers ----------------
#include "TNamed.h"

// Collaborating Class Headers ------
#include "FairGeanePro.h"
#include "PndTrack.h"
#include "GFKalman.h"

// Collaborating Class Declarations --
class GFRecoHitFactory;
class GFKalman;

class PndFtsKalmanFit : public TNamed 
{
 public:
  
  // Constructors/Destructors ---------
  PndFtsKalmanFit();
  ~PndFtsKalmanFit();
  
  // Modifiers -----------------------
  void SetGeane(Bool_t opt = kTRUE)              { fUseGeane = opt;     }
  void SetNumIterations(Int_t num)               { fNumIt    = num;     } 
  void SetVerbose(Int_t verb)                    { fVerbose  = verb;    }
 
  // Operations ---------------------- 
  Bool_t Init();
  PndTrack*  Fit(PndTrack *tBefore, Int_t PDG);
  
private:
  
  // Private Data Members ------------
  GFRecoHitFactory* fTheRecoHitFactory;
  GFKalman fGenFitter;
  
  FairGeanePro* fPro;   //! Geane Propagator
  
 
  Bool_t fUseGeane;     //! Flag to use Geane 
  Int_t fNumIt;         //! Number of iterations
  Int_t fVerbose;       //! Verbose level
  
  ClassDef(PndFtsKalmanFit,0);

};

#endif
