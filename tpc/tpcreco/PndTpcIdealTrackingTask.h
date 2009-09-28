//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Pattern recognition = track finding in the TPC
//      Using Monte Carlo Truth
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCIDEALTRACKINGTASK_HH
#define TPCIDEALTRACKINGTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op
#include "TRandom3.h"

// Collaborating Class Declarations --
#include "PndTpcCluster.h"
class TClonesArray;
class PndTpcFrontend;
class RecoHitFactory;
class TH1I;
class TH1D;
class FairGeanePro;

class PndTpcIdealTrackingTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcIdealTrackingTask();
  ~PndTpcIdealTrackingTask();

  // Operators
  
  // Accessors -----------------------


  // Modifiers -----------------------
  void SetClusterBranchName(const TString& name) {_clusterBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}
  void useGeane(Bool_t opt=kTRUE){_useGeane=opt;}
  void useDistSorting(Bool_t opt=kTRUE) {_useDistSorting=opt;}
  void SetSecondarySuppression(Bool_t opt=kTRUE) {_secondarySuppression=opt;}
 
  // Operations ----------------------
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  void WriteHistograms(const TString& filename);

private:

  // Private Data Members ------------
  TString _clusterBranchName;
  TClonesArray* _clusterArray;
  TClonesArray* _mcTrackArray;
  TClonesArray* _trackArray;
 
  Bool_t _persistence;
  Bool_t _useGeane;
  Bool_t _useDistSorting;
  Bool_t _secondarySuppression;  //ignore secondaries

  TH1I* _multiplicityHisto;
  TH1I* _trackSizeH;
  TH1D* _trackPurityH;
  TH1D* _trackMcIdsH;

  RecoHitFactory* _theRecoHitFactory;
  FairGeanePro* _geanePro;

  TRandom3 myrandom;
  // Private Methods -----------------

public:
  ClassDef(PndTpcIdealTrackingTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
