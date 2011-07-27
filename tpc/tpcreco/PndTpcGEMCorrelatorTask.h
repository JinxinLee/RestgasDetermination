//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Correlate fitted TPC track with GEM hits 
//      and append to Track 
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCGEMCORRELATORTASK_HH
#define TPCGEMCORRELATORTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
#include "PndTpcRiemannTrack.h"
#include "PndTpcCluster.h"
#include "FairGeanePro.h"

class TClonesArray;
class GFRecoHitFactory;
class TH1D;


class PndTpcGEMCorrelatorTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcGEMCorrelatorTask();
  virtual ~PndTpcGEMCorrelatorTask();

  // Operators
  
  // Accessors -----------------------
  
  
  // Modifiers -----------------------
  void SetTrackBranchName(const TString& name) {fTrackBranchName=name;}
  void SetOutTrackBranchName(const TString& name) {fOutTrackBranchName=name;}
  void SetGEMBranchName(const TString& name) {fGEMBranchName=name;}
  
  
  void SetMatchDistance(double d) {fMatchDistance=d;}
  void SetMinGEMHits(unsigned int n) {fMinGEMHits = n;}
  
  void SetPersistence(Bool_t opt=kTRUE) {fPersistence=opt;}
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  void WriteHistograms(const TString& fname) const;
  
  
 private:

  double fMatchDistance;   // lookup distance in cm
  
  // Private Data Members ------------
  TString fTrackBranchName;
  TString fOutTrackBranchName;
  TString fGEMBranchName;
  
  bool fPersistence;

  unsigned int fMinGEMHits;
  
  TClonesArray* fTrackArray;
  TClonesArray* fOutTrackArray;
  TClonesArray* fGEMArray;

  GFRecoHitFactory* fTheRecoHitFactory;

  TH1D* fResHistX;
  TH1D* fResHistY;
  TH1D* fResHistZ;

  // gem correlator stuff goes here
  std::vector<int> fOrdering;
  std::vector<int>::iterator fOrderingIterator; 
  

  ClassDef(PndTpcGEMCorrelatorTask,1);
  
};

#endif
