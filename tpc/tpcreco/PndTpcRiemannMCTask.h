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

#ifndef TPCRIEMANNMCTASK_HH
#define TPCRIEMANNMCTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
#include "PndTpcCluster.h"
class TClonesArray;
class PndTpcFrontend;
class PndTpcRiemannTrack;
class PndMCTrack;
class GFRecoHitFactory;
class TH1I;
class TH1D;
class TH2D;

class TTree;


class PndTpcRiemannMCTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcRiemannMCTask();
  ~PndTpcRiemannMCTask();

  // Operators
  
  // Accessors -----------------------


  // Modifiers -----------------------
  void SetClusterBranchName(const TString& name) {_clusterBranchName=name;}
  void SetBkgFileName(const TString& name){_bkgFileName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}
  void SetTrkFinderParameters(double proxcut,
			      double riproxcut,
			      double planecut,
			      double szcut,
			      unsigned int minpointsforfit);


  // Operations ----------------------
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  void WriteHistograms();

private:

  // Private Data Members ------------
  TString _clusterBranchName;
  TString _mctrackBranchName;
  TString _bkgBranchName;

  TClonesArray* _mctrackArray;
  TClonesArray* _bkgArray;
  TClonesArray* _clusterArray;
  TClonesArray* _riemannTrackArray;
  TClonesArray* _riemannHitArray;
  TClonesArray* _riemannResArray;

  Bool_t _persistence;

  TString _bkgFileName;
  TTree* _bkgTree;

  // tuning parameters for Conformal Map TrackFinder
  double _proxcut;
  double _riproxcut;
  double _planecut;      
  unsigned int _minpoints;
  double _szcut;

  TH1D* _distH;
  TH1D* _distTrueH;
  TH2D* _distTrueRH;
  TH2D* _distTrueIDH;
  TH1D* _sdistH;
  TH1D* _sdistTrueH;
  TH2D* _sdistTrueZH;
  TH2D* _2dDist;
  TH2D* _2dTrueDist;

  // Private Methods -----------------
  PndTpcRiemannTrack* McToRiemann(PndMCTrack* mctrk);

public:
  ClassDef(PndTpcRiemannMCTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
