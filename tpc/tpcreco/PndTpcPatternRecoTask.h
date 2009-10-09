//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Pattern recognition = track finding in the TPC
//      Using a conformal track follower
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCPATTERNRECOTASK_HH
#define TPCPATTERNRECOTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
#include "PndTpcCluster.h"
class TClonesArray;
class PndTpcFrontend;
class GFRecoHitFactory;
class PndTpcConfTrackFinder;
class TH1I;
class TH1D;

class PndTpcPatternRecoTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcPatternRecoTask();
  ~PndTpcPatternRecoTask();

  // Operators
  
  // Accessors -----------------------


  // Modifiers -----------------------
  void SetClusterBranchName(const TString& name) {_clusterBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}
  void SetTrkFinderParameters(double xcut, double ycut, double zcut,
			      double chi2cut,
			      unsigned int minpointsforfit);


  // Operations ----------------------
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  void WriteHistograms(const TString& filename);

private:

  // Private Data Members ------------
  TString _clusterBranchName;
  TClonesArray* _clusterArray;
  TClonesArray* _trackArray;

  Bool_t _persistence;

  // tuning parameters for Conformal Map TrackFinder
  double _xcut;
  double _ycut;
  double _zcut; 
  double _chi2cut;     
  unsigned int _minpoints;

  TH1I* _multiplicityHisto;
  TH1I* _trackSizeH;
  TH1D* _trackPurityH;

  PndTpcConfTrackFinder* _trackfinder;

  GFRecoHitFactory* _theRecoHitFactory;


  // Private Methods -----------------

public:
  ClassDef(PndTpcPatternRecoTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
