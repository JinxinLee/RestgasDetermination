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

#ifndef TPCRIEMANNTRACKINGTASK_HH
#define TPCRIEMANNTRACKINGTASK_HH

// Base Class Headers ----------------
#include "CbmTask.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
#include "PndTpcCluster.h"
#include "CbmGeanePro.h"
class TClonesArray;
class PndTpcFrontend;
class PndTpcRiemannTrackFinder;
class RecoHitFactory;
class TH1I;
class TH1D;

class PndTpcRiemannTrackingTask : public CbmTask {
public:

  // Constructors/Destructors ---------
  PndTpcRiemannTrackingTask();
  ~PndTpcRiemannTrackingTask();

  // Operators
  
  // Accessors -----------------------


  // Modifiers -----------------------
  void SetClusterBranchName(const TString& name) {_clusterBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}
  void SetTrkFinderParameters(double proxcut,
			      double riproxcut,
			      double planecut,
			      double szcut,
			      unsigned int minpointsforfit);


  // Operations ----------------------
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);
  
  void useGeane(Bool_t geane=kTRUE) {_geane=geane;}

  void WriteHistograms(const TString& filename);

private:

  // Private Data Members ------------
  TString _clusterBranchName;
  TClonesArray* _clusterArray;
  TClonesArray* _mvdArray;
  TClonesArray* _trackArray;
  TClonesArray* _riemannTrackArray;
  TClonesArray* _riemannHitArray;

  Bool_t _persistence;
  Bool_t _geane;

  // tuning parameters for Conformal Map TrackFinder
  double _proxcut;
  double _riproxcut;
  double _planecut;      
  unsigned int _minpoints;
  double _szcut;

  TH1I* _multiplicityHisto;
  TH1I* _trackSizeH;
  TH1D* _trackPurityH;
  TH1D* _trackMcIdsH;

  PndTpcRiemannTrackFinder* _trackfinder;

  CbmGeanePro* _geanePro;

  RecoHitFactory* _theRecoHitFactory;


  // Private Methods -----------------

public:
  ClassDef(PndTpcRiemannTrackingTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
