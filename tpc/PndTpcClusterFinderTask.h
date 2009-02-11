//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      PndTpc PSA: Pulse shape analysis
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCCLUSTERFINDERTASK_HH
#define TPCCLUSTERFINDERTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include "PndTpcCluster.h"

// Collaborating Class Declarations --
class TClonesArray;
class PndTpcFrontend;
class PndTpcAbsClusterFinder;
class PndTpcDigiPar;
class PndTpcPadPlane;
class PndTpcGem;
class PndTpcFrontend;
class PndTpcGas;

class PndTpcClusterFinderTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcClusterFinderTask();
  ~PndTpcClusterFinderTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetDigiBranchName(const TString& name) {_digiBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}
  void SetTrivialClustering(Bool_t opt=kTRUE) {_trivial=opt;}
  void SetMode(int mode){_mode=mode;}
  
  void timeslice(unsigned int t){_timeslice=t;}

  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  virtual void SetParContainers();

private:

  // Private Data Members ------------
  TString _digiBranchName;
  TClonesArray* _clusterArray;
  TClonesArray* _digiArray;
  
  Bool_t _persistence;
  Bool_t _trivial;
  int _mode;
  unsigned int _timeslice;

  const PndTpcFrontend* _frontend;
  PndTpcPadPlane* _padplane;
  const PndTpcGem* _gem;
  const PndTpcGas* _gas;
  double _zGem;
  PndTpcAbsClusterFinder* _finder;

  PndTpcDigiPar* _par;
 
  std::vector<PndTpcCluster*>* _cluster_buffer;
  // Private Methods -----------------

public:
  ClassDef(PndTpcClusterFinderTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
