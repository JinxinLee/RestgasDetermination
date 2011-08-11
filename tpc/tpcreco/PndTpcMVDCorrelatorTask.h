//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Correlate fitted TPC track with MVD hits 
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

#ifndef TPCMVDCORRELATORTASK_HH
#define TPCMVDCORRELATORTASK_HH

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


class PndTpcMVDCorrelatorTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcMVDCorrelatorTask();
  virtual ~PndTpcMVDCorrelatorTask();

  // Operators
  
  // Accessors -----------------------
  
  
  // Modifiers -----------------------
  void SetTrackBranchName(const TString& name) {fTrackBranchName=name;}
  void SetOutTrackBranchName(const TString& name) {fOutTrackBranchName=name;}
  void SetPixelBranchName(const TString& name) {fPixelBranchName=name;}
  void SetStripBranchName(const TString& name) {fStripBranchName=name;}
  
  void SetMatchDistance(double d) {fMatchDistance=d;}
  void SetMinMVDHits(unsigned int n) {fMinMVDHits = n;}
  
  void SetPersistence(Bool_t opt=kTRUE) {fPersistence=opt;}
  void RequireMatch(bool opt=true) {fRequireMatch=opt;}
  void MergeHits(bool opt=true){fMergeHits=opt;}

  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  void WriteHistograms(const TString& fname) const;
  
  
 private:

  double fMatchDistance;   // lookup distance in cm
  
  // Private Data Members ------------
  TString fTrackBranchName;
  TString fOutTrackBranchName;
  TString fPixelBranchName;
  TString fStripBranchName;
  
  bool fPersistence;
  bool fRequireMatch; //true: ONLY write out tracks with a MVD match!
  bool fMergeHits; // merge found MVD hits into output track 

  unsigned int fMinMVDHits;
  
  TClonesArray* fTrackArray;
  TClonesArray* fOutTrackArray;
  TClonesArray* fPixelArray;
  TClonesArray* fStripArray;

  GFRecoHitFactory* fTheRecoHitFactory;

  TH1D* fResHistX;
  TH1D* fResHistY;
  TH1D* fResHistZ;

  ClassDef(PndTpcMVDCorrelatorTask,1);
  
};

#endif

