// -------------------------------------------------------------------------
// -----                 PndPrintFairLinks header file             -----
// -----                  Created 20/03/07  by R.Kliemt               -----
// -------------------------------------------------------------------------


/** PndPrintFairLinks.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Prints all FairLinks of a given branch to the screen
 **/


#ifndef PndPrintFairLinks_H
#define PndPrintFairLinks_H


// framework includes
#include "FairTask.h"


#include <vector>
#include <map>

class TClonesArray;

class PndPrintFairLinks : public FairTask
{
 public:

  /** Default constructor **/
	PndPrintFairLinks();

  /** Destructor **/
  virtual ~PndPrintFairLinks();


  /** Virtual method Init **/
  virtual void SetParContainers();

  virtual void AddBranchName(const TString& name){
	  fSelectedBranches->AddLast(new TObjString(name.Data()));
  }

  virtual void PrintBranchNameList(TList* branches);

  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();

 protected:
  void InitBranchList(TList* branches);



 private:
  std::map<Int_t, TClonesArray*> fBranches;
  TList* fSelectedBranches;

  void Register();

  void Reset();

  void ProduceHits();


  ClassDef(PndPrintFairLinks,1);

};

#endif
