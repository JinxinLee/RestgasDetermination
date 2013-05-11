//-----------------------------------------------------------
//
// Description:
//      Task to filter fitted tracks based on chi2 and hits comparasion
//
// Author:
//	Anastasia Karavdina
//-----------------------------------------------------------

#ifndef PNDLMDTRKSFILTERTASK_H
#define PNDLMDTRKSFILTERTASK_H

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include <map>
#include "TString.h"
#include "TH2.h"
#include "TH1.h"
#include "PndLmdDim.h"

// Collaborating Class Declarations --
class TClonesArray;
class TGeoManager;

class PndLmdTrksFilterTask :public FairTask{

public:

  // Constructors/Destructors ---------
  PndLmdTrksFilterTask();
  PndLmdTrksFilterTask& operator=(const PndLmdTrksFilterTask& o) { return *this;};
  virtual ~PndLmdTrksFilterTask();
  void SetVerboseLevel(int verbose){verboseLevel = verbose;};
  virtual InitStatus Init();
  virtual void FinishTask();

  virtual void Exec(Option_t* opt);

protected:
  int verboseLevel;
  // Input Data------------
  TClonesArray* fHitArray;
  TClonesArray* fTrkCandArray;
  TClonesArray* fTrkArray;
  TClonesArray* fTrkOutArray;
  TClonesArray* fMCHitArray;

  TString fHitName;
  TString fMCHitName;
  TString fTrkCandName;
  TString fTrkName;
  TString fTrkOutName;
int fEventNr;
  PndLmdDim* lmddim;
  ClassDef(PndLmdTrksFilterTask,1);
};

#endif

