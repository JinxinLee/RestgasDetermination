//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Pattern recognition = track finding in the TPC
//      Using a riemann track fit
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCTRACKINITTASK_HH
#define TPCTRACKINITTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
#include "PndTpcRiemannTrack.h"
#include "PndTpcCluster.h"
#include "FairGeanePro.h"
class TClonesArray;
class PndTpcFrontend;
class GFRecoHitFactory;
class TH1I;
class TH1D;
class PndTrackCand;
class PndTrack;
class PndTpcDigiPar;

class PndTpcTrackInitTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcTrackInitTask();
  ~PndTpcTrackInitTask();

  // Operators
  
  // Accessors -----------------------


  // Modifiers -----------------------
  void SetClusterBranchName(const TString& name) {_clusterBranchName=name;}
  void SetRiemannBranchName(const TString& name) {_riemannBranchName=name;}
  void SetOutBranchNames(const TString& tracks,
			 const TString& cands,
			 const TString& pndcands)
  {_trackBranchName=tracks;
    _trackCandBranchName=cands;
    _pndTrackBranchName=pndcands;}

  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;} // store GFTracks with GFTrackCands
  void SetPersistencePnd(Bool_t opt=kTRUE) {_persistencePnd=opt;} // store PndTracks and PndTrackCands
 
  // Operations ----------------------
  virtual InitStatus Init();

  virtual void SetParContainers();

  virtual void Exec(Option_t* opt);
  
  void SetMCPid(Bool_t opt=kTRUE) {_mcPid = opt;} // use MC information for particle identification
  void SetPDG(int pdg) {_pdg=pdg;} // use hypothesis

  void SetSmoothing(bool s=true) {_smoothing=s;} // use smoothing for the Kalman
  void useGeane(Bool_t geane=kTRUE) {_geane=geane;}

  void WriteHistograms(const TString& filename);

private:

  // Private Data Members ------------
  TString _clusterBranchName;
  TString _riemannBranchName;
  TString _trackBranchName;
  TString _trackCandBranchName;
  TString _pndTrackBranchName;
 

  TClonesArray* _mcTrackArray;
  TClonesArray* _clusterArray;
  TClonesArray* _trackArray;
  TClonesArray* _riemannTrackArray;
  TClonesArray* _riemannHitArray;
  TClonesArray* _trackCandArray;
  TClonesArray* _pndTrackArray;

  PndTpcDigiPar* fpar;

  Bool_t _persistence;
  Bool_t _persistencePnd;

  double Bz; //mag field

  double fmaxLenZbackw; // max length in z of backward target track
  double fRMin;
  double fRMax;

  bool _mcPid;
  int _pdg;


  GFRecoHitFactory* _theRecoHitFactory;

  bool _smoothing;
  Bool_t _geane;
  FairGeanePro* gPro;

  int counter;

  // Private Methods -----------------

public:
  ClassDef(PndTpcTrackInitTask,2)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
