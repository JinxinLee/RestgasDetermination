#ifndef PID_EMCBAYESASSOCIATOR_H
#define PID_EMCBAYESASSOCIATOR_H

// **************************************************************************
//  Author: Ronald Kunne e-mail: kunne@ipno.in2p3.fr
//   
//  Probability density functions from E/p distributions
//
// Created: 01-04-11
// Modified: 16-01-12 New formalism including moments
//
// **************************************************************************

#include "TClonesArray.h"
#include "FairTask.h"
#include "TF1.h"
#include "TString.h"

class PndPidCandidate;
class PndPidProbability;
class TH2D;

class PndPidEmcBayesAssociatorTask : public FairTask {
  
 protected:
  
  TClonesArray* fPidChargedCand;    //! PndPidCandidate TCA for charged particles
  TClonesArray* fPidChargedProb;    //! PndPidProbability TCA for charged particles


  void DoPidMatch(PndPidCandidate* pidcand, PndPidProbability* prob);

//  Double_t GetPdf(Double_t thetaC, Double_t mom, Double_t emc, Int_t charge);
  void GetPdf(Double_t pp1, Double_t th1, Double_t ph1, 
              Double_t z20, Double_t z53, Double_t LAT, Double_t EP1, 
              Int_t charge, Double_t *proba);

// variables used
  Int_t                 fNvar[6];      // nr of bins in p, th, ph, varx, vary, EP
  Int_t                 fNbins;        // total number of bins 
  TH2D                  *fBayesEP[10];
  TH2D                  *fBayesTH[10];
  TH2D                  *fBayesZZ[10];
  TString               fDirectory[10];

  Int_t                 fevcounter;

 public:
  
  virtual void Exec(Option_t * option);
  virtual InitStatus Init();                        //
  
  void SetupEnvironment(char *filename);
  
  void Register();
  void Reset();
  
  PndPidEmcBayesAssociatorTask(const char *name, const char *title="Pnd Task");
  PndPidEmcBayesAssociatorTask();
  virtual ~PndPidEmcBayesAssociatorTask();
  
  void SetVerbose(Bool_t verb)            { fVerbose = verb  ;};
	
  /** Get parameter containers **/
  virtual void SetParContainers();
  virtual void Finish();

  ClassDef(PndPidEmcBayesAssociatorTask,1)   // PndPidDrcTask
  
    };

#endif
