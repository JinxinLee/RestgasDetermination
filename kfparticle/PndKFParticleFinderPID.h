//-----------------------------------------------------------
//-----------------------------------------------------------

#ifndef PndKFParticleFinderPID_HH
#define PndKFParticleFinderPID_HH

#include "FairTask.h"

#include "TString.h"

#include <vector>

class TClonesArray;
class TFile;
class TObject;

class PndKFParticleFinderPID : public FairTask {
 public:

  // Constructors/Destructors ---------
  PndKFParticleFinderPID(const char* name = "PndKFParticleFinderPID", Int_t iVerbose = 0);
  ~PndKFParticleFinderPID();

  void SetMCTrackBranchName(const TString& name)   { fMCTracksBranchName = name;  }
  void SetChargedTrackBranchName(const TString& name)   { fChargedTrackBranchName = name;  }
  void SetNeutralTrackBranchName(const TString& name)   { fNeutralTrackBranchName = name;  }
  
  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);
  virtual void Finish();
  
  void SetPIDMode(int mode) { fPIDMode = mode; }
  void UseNoPID() { fPIDMode = 0; }
  void UseMCPID() { fPIDMode = 1; }
  void UseRecoPID() { fPIDMode = 2; }
  const std::vector<int>& GetPID() const { return fPID; }
  
 private:
  
  void SetMCPID();
  void SetRecoPID();
  
  //names of input branches
  //names of input branches
  TString fChargedTrackBranchName;      //! Name of the input TCA 
  TString fNeutralTrackBranchName;      //! Name of the input TCA 
  TString fMCTracksBranchName;      //! Name of the input TCA with MC tracks

  //input branches
  TClonesArray *fChargedTrackArray;
  TClonesArray *fNeutralTrackArray;
  TClonesArray *fMCTrackArray; //mc tracks
  
  //PID variables
  Int_t fPIDMode;
  std::vector<int> fPID;
  
  ClassDef(PndKFParticleFinderPID,1);
};

#endif
