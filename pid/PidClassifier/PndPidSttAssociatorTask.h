// 
// PndPidSttAssociatorTask.h
//
// STANDARD PROCEDURE:
//
// @ fixed momentum, the de/dx_mean values distribute as a gaussian:
// its mean and sigma is evaluated from the distribution of de/dx 
// from truncated mean vs momentum from global tracking for the 
// different particles.
//
// The candidate pid is evaluated as the probability density function 
// value of the gaussian @ the reconstructed momentum, evaluated for 
// the measured dedx_mean of the track.
//
// ============================================

#ifndef PID_STT_ASSOCIATOR_H
#define PID_STT_ASSOCIATOR_H

#include "FairTask.h"

#include "TF1.h"
class PndPidCandidate;
class PndPidProbability;

class PndPidSttAssociatorTask : public FairTask {
  
 protected:
  
  TClonesArray* fPidChargedCand;    //! charged candidates
  TClonesArray* fPidChargedProb;    //! 

  // assign probabilities
  void DoPidMatch(PndPidCandidate* pidcand, PndPidProbability* prob);

  // calculate probability
  Double_t GetPdf(Double_t dedx, Double_t mean, Double_t sigma);

  void ElectronDEDXMeanFunction();
  void MuonDEDXMeanFunction();
  void PionDEDXMeanFunction();
  void KaonDEDXMeanFunction();
  void ProtonDEDXMeanFunctionL();
  void ProtonDEDXMeanFunctionH();

  void ElectronDEDXSigmaFunction();
  void MuonDEDXSigmaFunction();
  void PionDEDXSigmaFunction();
  void KaonDEDXSigmaFunction();
  void ProtonDEDXSigmaFunctionL();
  void ProtonDEDXSigmaFunctionH();
 
  TF1 *fElectronDEDXMean;
  TF1 *fMuonDEDXMean;
  TF1 *fPionDEDXMean;
  TF1 *fKaonDEDXMean;
  TF1 *fProtonDEDXMeanL;
  TF1 *fProtonDEDXMeanH;

  TF1 *fElectronDEDXSigma;
  TF1 *fMuonDEDXSigma;
  TF1 *fPionDEDXSigma;
  TF1 *fKaonDEDXSigma;
  TF1 *fProtonDEDXSigmaL;
  TF1 *fProtonDEDXSigmaH;



 public:
  
 
  /** Default constructor **/  
  PndPidSttAssociatorTask();
  
  PndPidSttAssociatorTask(const char *name, const char *title="Pnd Task");

  /** Destructor **/
   virtual ~PndPidSttAssociatorTask();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  void SetVerbose(Bool_t verb)            { fVerbose = verb  ;};
	
  void SetDefaultMassHypo(Bool_t hypo ) { fDefaultHypo = hypo; }
  Bool_t IsDefaultMassHypo() { return fDefaultHypo; }

  // when using the Kalman performed only with the muon hypothesis
  // and the pid with the default hypothesis
  Int_t fDefaultHypo;
  ClassDef(PndPidSttAssociatorTask,1);
    
};

#endif
