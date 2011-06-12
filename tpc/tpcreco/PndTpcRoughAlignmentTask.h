//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Rough Alignment of TPC clusters
//      
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCROUGHALIGNMENTTASK_HH
#define TPCROUGHALIGNMENTTASK_HH

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


class PndTpcRoughAlignmentTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcRoughAlignmentTask();
  virtual ~PndTpcRoughAlignmentTask();

  // Operators
  
  // Accessors -----------------------
  
  
  // Modifiers -----------------------
  void SetClusterBranchName(const TString& name) {fClusterBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {fPersistence=opt;}
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  void SetRotation(const TVector3& r) {fRot = r;}
  void SetShift(const TVector3& s) {fShift = s;}
  
 private:

  TString fClusterBranchName;
  
  bool fPersistence;
  
  TClonesArray* fClusterArray;

  TVector3 fRot;
  TVector3 fShift;

  ClassDef(PndTpcRoughAlignmentTask,1);
  
};

#endif
