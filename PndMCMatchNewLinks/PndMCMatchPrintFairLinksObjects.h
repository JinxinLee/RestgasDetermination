// -------------------------------------------------------------------------
// -----                 PndMCMatchPrintFairLinksObjects header file             -----
// -----                  Created 20/03/07  by R.Kliemt               -----
// -------------------------------------------------------------------------


/** PndMCMatchPrintFairLinksObjects.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Displays all available informations for a given event
 **/


#ifndef PndMCMatchPrintFairLinksObjects_H
#define PndMCMatchPrintFairLinksObjects_H


// framework includes
#include "FairTask.h"


#include <vector>
#include <map>

class TClonesArray;

class PndMCMatchPrintFairLinksObjects : public FairTask
{
 public:

  /** Default constructor **/
	PndMCMatchPrintFairLinksObjects();

  /** Destructor **/
  virtual ~PndMCMatchPrintFairLinksObjects();


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


  ClassDef(PndMCMatchPrintFairLinksObjects,1);

};

#endif
