//-----------------------------------------------------------
//-----------------------------------------------------------

#ifndef PndKFParticleFinder_HH
#define PndKFParticleFinder_HH

#include "FairTask.h"

#include "TString.h"

#include <vector>

class PndKFParticleFinderPID;
class KFParticleTopoReconstructor;
class TClonesArray;

class PndKFParticleFinder : public FairTask {
 public:

  // Constructors/Destructors ---------
  PndKFParticleFinder(const char* name = "PndKFParticleFinder", Int_t iVerbose = 0);
  ~PndKFParticleFinder();

  void SetPVToZero() { fPVFindMode = 0; }
  void ReconstructSinglePV() { fPVFindMode = 1; }
  void RconstructMultiplePV() { fPVFindMode = 2; }
  
  void SetChargedTrackBranchName(const TString& name)   { fChargedTrackBranchName = name;  }
  void SetNeutralTrackBranchName(const TString& name)   { fNeutralTrackBranchName = name;  }

  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);

  const KFParticleTopoReconstructor * GetTopoReconstructor() const { return fTopoReconstructor; }
  
  void SetPIDInformation(PndKFParticleFinderPID* pid) { fPID = pid; }
  
  // set cuts
  void SetPrimaryProbCut(float prob);
  
 private:

  double InversedChi2Prob(double p, int ndf) const;

  const PndKFParticleFinder& operator = (const PndKFParticleFinder&);
  PndKFParticleFinder(const PndKFParticleFinder&);
  
  //names of input branches
  TString fChargedTrackBranchName;      //! Name of the input TCA 
  TString fNeutralTrackBranchName;      //! Name of the input TCA 

  //input branches
  TClonesArray *fChargedTrackArray;
  TClonesArray *fNeutralTrackArray;
  TClonesArray *fEmcBumps;
  
  //topology reconstructor
  KFParticleTopoReconstructor *fTopoReconstructor;
  int fPVFindMode;
  
  //PID information
  PndKFParticleFinderPID* fPID;
  
  ClassDef(PndKFParticleFinder,1);
};

#endif
