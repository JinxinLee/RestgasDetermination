// -------------------------------------------------------------------------
// -----                 PNDMCTESTDATACRAWLER header file             -----
// -----                  Created 18/01/10  by T.Stockmanns             -----
// -------------------------------------------------------------------------


/** PNDMCTESTDATACRAWLER.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Displays all available informations for a given event
 **/


#ifndef PNDMCTESTDATACRAWLER_H
#define PNDMCTESTDATACRAWLER_H


// framework includes
#include "FairTask.h"
#include "PndMCDataCrawler.h"
#include "TH1.h"


#include <vector>
#include <map>

class TClonesArray;

class PndMCTestDataCrawler : public FairTask
{
 public:

  /** Default constructor **/
	PndMCTestDataCrawler();

  /** Destructor **/
  virtual ~PndMCTestDataCrawler();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();



 private:
  PndMCDataCrawler* fCrawler;
  TClonesArray* fInputData;

  TH1* fTimeResHisto;

  Int_t fEventNr;

  void Register();

  void Reset();

  void ProduceHits();


  ClassDef(PndMCTestDataCrawler,1);

};

#endif
