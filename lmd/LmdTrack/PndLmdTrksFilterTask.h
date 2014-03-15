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
//#include "TNtuple.h"
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
  void SetSkipKinFilt(bool fl){ flSkipKinFilt = fl; };
  void SetBOXFilt(bool fl){ flBOXKinFilt = fl; };
  void SetXThFilt(bool fl){ flXThKinFilt = fl; };
  void SetYPhFilt(bool fl){ flYPhKinFilt = fl; };
  void SetDX(double dx){ fdX = dx;}
  void SetDY(double dy){ fdY = dy;}

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
  bool flSkipKinFilt;
  bool flBOXKinFilt;
  bool flXThKinFilt;
  bool flYPhKinFilt;

  double fdX; //beam shift in X
  double fdY; //beam shift in Y
  //  TNtuple *htthetatphiTrkFit;
  ClassDef(PndLmdTrksFilterTask,2);
};

#endif

