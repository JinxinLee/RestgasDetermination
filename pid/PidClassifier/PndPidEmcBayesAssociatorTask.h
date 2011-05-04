#ifndef PID_EMCBAYESASSOCIATOR_H
#define PID_EMCBAYESASSOCIATOR_H

// **************************************************************************
//  Author: Ronald Kunne e-mail: kunne@ipno.in2p3.fr
//   
//  Probability density functions from E/p distributions
//
// Created: 01-04-11
// Modified:
//
// **************************************************************************

#include "TClonesArray.h"
#include "FairTask.h"
#include "TF1.h"
#include "TString.h"

class PndPidCandidate;
class PndPidProbability;

class PndPidEmcBayesAssociatorTask : public FairTask {
  
 protected:
  
  TClonesArray* fPidChargedCand;    //! PndPidCandidate TCA for charged particles
  TClonesArray* fPidChargedProb;    //! PndPidProbability TCA for charged particles


  void DoPidMatch(PndPidCandidate* pidcand, PndPidProbability* prob);

//  Float_t GetPdf(Float_t thetaC, Float_t mom, Float_t emc, Int_t charge);
  void GetPdf(Float_t pp1, Float_t th1, Float_t ph1, 
              Float_t z20, Float_t z53, Float_t EP1, 
              Int_t charge, Float_t *proba);

// variables used
  Int_t                 fNvar[6];      // nr of bins in p, th, ph, varx, vary, EP
  Int_t                 fNbins;        // total number of bins 
  TF1                   *fPDF[11][100];
  TString               fDirectory[10];

  Double_t              fitParam[11][20][100]; 
  Double_t              fitChi2[11][100];
  Double_t              fitScale[11][100];
  Double_t              fitRange[11][2][100];
  Double_t              fitCenter[11][5][100];   // p,th,ph, varx, vary

  Int_t                 fevcounter;
/*
  Int_t                 fImod;         // transform Z20 and Z53 if larger than 0 
  Double_t              fRange[6][2];  // variable ranges
  Double_t              fdRange[6];    // variable ranges

  TH1D                  *fitelec[11][100];
  TF1                   *funelec[11][100];
  TF1                   *ffit;
*/
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
