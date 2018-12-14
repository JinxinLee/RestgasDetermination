#ifndef PndMcCloner2_H
#define PndMcCloner2_H 1


#include "FairTask.h"
#include "PndMCTrack.h"
#include <map>

using namespace std;

class TClonesArray;

class PndMcCloner2 : public FairTask
{

 public:

  /** Default constructor **/
  PndMcCloner2();


  /** Destructor **/
  ~PndMcCloner2();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t*);

  void SetCleanMc(Bool_t opt = kTRUE) { fCleanMC = opt; };
  void SetOutputBranch(TString branch)    { fTrackBranchNamePidHypo = branch; };

 protected:

  void FindUsedMCIndices();
  void CloneMCTrack();
  void CloneAndCleanMCTrack();
  void CorrectMotherIndices();
  void CorrectPidIndices();

  /** Input array of PndMCTrack **/
  TClonesArray* fInputArray;

  /** Input array of PidChargedCand **/
  TClonesArray* fPidChargedArray[5];

  /** Input array of PidNeutralCand **/
  TClonesArray* fPidNeutralArray[5];


  /** Output array of PndMCTrack **/
  TClonesArray* fOutputArray;

  map<Int_t, Int_t> mapMCIndex; // Map <old mc index, new mc index>

  Bool_t fCleanMC; // Flag to clean the MCTrack from unused indices

  TString fPidHypoStr[5];
  Bool_t fHypoFlagCharged[5];				//! Flag to check which hypotheses are present
  Bool_t fHypoFlagNeutral[5];				//! Flag to check which hypotheses are present

  TString fTrackBranchNamePidHypo;

  ClassDef(PndMcCloner2,2);

};

#endif
