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
#include "PndMCMatch.h"


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
  void SetPersistance(Bool_t pers){fPersistance = pers;}
  Bool_t GetPersistance(){return fPersistance;}

  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();



 private:
  InitStatus InitBranches();
  std::map<Int_t, TClonesArray*> fBranches;
  Bool_t fPersistance;
  Int_t fEvent;
  Bool_t fPrintOnce;

  TClonesArray* fMCLink;//->

  int fEventNr;
  PndMCMatch* fMCMatch;

  void Register();

  void Reset();

  void ProduceHits();


  ClassDef(PndMCMatchPrintFairLinksObjects,1);

};

#endif
