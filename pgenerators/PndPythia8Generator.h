#ifndef PNDP8GENERATOR_H
#define PNDP8GENERATOR_H 1

#include "TROOT.h"
#include "FairGenerator.h"

#ifdef __CINT__
#define _DLFCN_H_
#define _DLFCN_H
#endif

#include "Pythia.h"
#include "TRandom1.h"
#include "TRandom3.h"

class FairPrimaryGenerator;
using namespace Pythia8;

class PyTr1Rng : public RndmEngine
{
 public:
  PyTr1Rng() {  rng = new TRandom1(gRandom->GetSeed()); };
  virtual ~PyTr1Rng() {};
  
  Double_t flat() { return rng->Rndm(); };
  
 private:
  TRandom1 *rng; //!
};

class PyTr3Rng : public RndmEngine
{
 public:
  PyTr3Rng() {  rng = new TRandom3(gRandom->GetSeed()); };
  virtual ~PyTr3Rng() {};
  
  Double_t flat() { return rng->Rndm(); };
  
 private:
  TRandom3 *rng; //!
};




class PndPythia8Generator : public FairGenerator
{
 public:
  
  /** default constructor **/
  PndPythia8Generator();
  
  /** destructor **/
  virtual ~PndPythia8Generator();
  
  /** public method ReadEvent **/
  Bool_t ReadEvent(FairPrimaryGenerator*);
  void SetParameters(char*);
  void Print(); //!
  
  virtual Bool_t Init(); //!
  
  void SetMom(Double_t mom) { fMom = mom; };
  void UseRandom1() { fUseRandom1 = kTRUE; fUseRandom3 = kFALSE; };
  void UseRandom3() { fUseRandom1 = kFALSE; fUseRandom3 = kTRUE; };
  
 private:
  
  Pythia fPythia;             //!
  RndmEngine* fRandomEngine;  //!
  char* fParams[100];         //!
  
 protected:
  
  Double_t fMom;       // pbar momentum
  Int_t fPar_it;       // number of parameter lines
  Bool_t fUseRandom1;  // flag to use TRandom1
  Bool_t fUseRandom3;  // flag to use TRandom3 (default)

  ClassDef(PndPythia8Generator,1);
};

#endif /* !PNDP8GENERATOR_H */
