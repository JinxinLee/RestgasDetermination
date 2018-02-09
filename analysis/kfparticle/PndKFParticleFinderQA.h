//-----------------------------------------------------------
//-----------------------------------------------------------

#ifndef PndKFParticleFinderQA_HH
#define PndKFParticleFinderQA_HH

#include "FairTask.h"

#include "TString.h"

#include <vector>

class KFParticleTopoReconstructor;
class KFTopoPerformance;
class TClonesArray;
class TFile;
class TObject;

class PndKFParticleFinderQA : public FairTask {
 public:

  // Constructors/Destructors ---------
  PndKFParticleFinderQA(const char* name = "PndKFParticleFinderQA", Int_t iVerbose = 0, KFParticleTopoReconstructor* tr=0,
                        TString outFileName="PndKFParticleFinderQA.root");
  ~PndKFParticleFinderQA();

  void SetEffFileName(const TString& name) { fEfffileName = name; }
  void SetMCTrackBranchName(const TString& name)   { fMCTracksBranchName = name;  }
  void SetChargedTrackBranchName(const TString& name)   { fChargedTrackBranchName = name;  }
  void SetNeutralTrackBranchName(const TString& name)   { fNeutralTrackBranchName = name;  }
  
  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);
  virtual void Finish();
  
  void SetPrintEffFrequency(Int_t n);

  void SaveParticles(Bool_t b = 1) { fSaveParticles = b; }
  void SaveMCParticles(Bool_t b = 1) { fSaveMCParticles = b; }
  
 private:
  
  const PndKFParticleFinderQA& operator = (const PndKFParticleFinderQA&);
  PndKFParticleFinderQA(const PndKFParticleFinderQA&);
  
  void FindClosestMCTrackToBump(const int trackId, int& closestTrack, float& drMin, const float* rReco, 
                                const std::vector< std::vector<int> >& mcDaughters);
  void FindEmcClusterMother(const int iDaughter, int& iMother);
  
  void WriteHistosCurFile( TObject *obj );
  
  //names of input branches
  TString fMCTracksBranchName;      //! Name of the input TCA with MC tracks
  TString fChargedTrackBranchName;      //! Name of the input TCA with charged tracks
  TString fNeutralTrackBranchName;      //! Name of the input TCA with neutral tracks
  
  //input branches
  TClonesArray *fMCTrackArray; //mc tracks
  TClonesArray *fChargedTrackArray;
  TClonesArray *fNeutralTrackArray;
  TClonesArray *fEmcBumps;
    // output arrays of particles
  TClonesArray* fRecParticles;    // output array of KF Particles
  TClonesArray* fMCParticles;     // output array of MC Particles
  TClonesArray* fMatchParticles;  // output array of match objects
  
  Bool_t fSaveParticles;
  Bool_t fSaveMCParticles;
  
  //output file with histograms
  TString fOutFileName;
  TFile* fOutFile;
  TString fEfffileName;
  //KF Particle QA
  KFTopoPerformance* fTopoPerformance;
  
  Int_t fPrintFrequency;
  Int_t fNEvents;
  Double_t fTime[5];
  
  ClassDef(PndKFParticleFinderQA,1);
};

#endif
