#ifndef PNDDRPCANALYSISMATERIAL_H
#define PNDDRPCANALYSISMATERIAL_H

#include "FairTask.h"

#include "TString.h"

#include <list>

class TTree;
class TClonesArray;


class  PndDrpcAnalysisMaterial : public FairTask
{
public:
   PndDrpcAnalysisMaterial(const char* name, const Int_t iVerbose=0);
public:
  /** Default constructor **/
   PndDrpcAnalysisMaterial() {};

  /** Initing routine **/
  virtual InitStatus Init();

  /** Loop procedure **/
  virtual void Exec(Option_t* option);

  /** Finishing routine **/
  virtual void Finish();

  /** Destructor **/
  virtual ~ PndDrpcAnalysisMaterial() {};
private:
  /** Initialize a tree **/
  void InitTree();
  /** Out tree **/
  TTree* fTree;			//!

  /** Event number **/
  Int_t fEvent;
  /** Information particle **/
  Double_t fX;
  Double_t fY;
  Double_t fZ;
  Double_t fX0;
  Double_t fIntL;


  /** MC tracks array **/
  TClonesArray* fMC;		//!
  /** Array of radlen points**/
  TClonesArray* fRadLen;	//!

  ClassDef( PndDrpcAnalysisMaterial,1)
};

#endif 

