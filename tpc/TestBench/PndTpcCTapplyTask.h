//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//     Applied the crosstalk  taged by PndTpcTCcrossTalk
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Maxence Vandenbroucke    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCCTAPPLY_HH
#define TPCCTAPPLY_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------

// Collaborating Class Declarations --
class TClonesArray;
class PndTpcFrontend;
class FairGeanePro;
class PndTpcCluster;



class PndTpcCTapplyTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcCTapplyTask();
  ~PndTpcCTapplyTask();
  // Operators
  
  // Accessors -----------------------


  // Modifiers -----------------------
  void SetClusterBranchName(const TString& name) {_clusterBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}
  void SetMaxVote(const double m) {maxVote=m;}
  void SetLengthMin(const int t) {tMin = t;}  
  void SetzMax(const double z) {zMax = z;}

  // Operations ----------------------
  virtual InitStatus Init();
  
  virtual void Exec(Option_t* opt);
  
 private:

  // Private Data Members ------------
  TString _clusterBranchName;
  TClonesArray* _clusterArray;
  std::vector<PndTpcCluster*>* _ca; //!
  TClonesArray* _clusterOutArray;
  TClonesArray* _clusterOutArray_unchanged;  


  Bool_t _persistence;
  
  int tMin; //min lentgh of cluster in time
  double maxVote; //proportion of crosstalk tagged in a cluster max
  double zMax; //zMax of cluster

  // Private Methods -----------------

public:
  ClassDef(PndTpcCTapplyTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
