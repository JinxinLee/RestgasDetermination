// -------------------------------------------------------------------------
// -----                 PNDMCMATCHSELECTORTASK header file             -----
// -----                  Created 18/01/10  by T.Stockmanns             -----
// -------------------------------------------------------------------------


/** PNDMCMATCHSELECTORTASK.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Displays all available informations for a given event
 **/


#ifndef PNDMCTESTMOMENTUMCOMPARE_H
#define PNDMCTESTMOMENTUMCOMPARE_H


// framework includes
#include "FairTask.h"
#include "PndMCMatch.h"
#include "PndDetectorList.h"
#include "TH1.h"


#include <vector>
#include <map>

class TClonesArray;

class PndMCTestMomentumCompare : public FairTask
{
 public:

  /** Default constructor **/
	PndMCTestMomentumCompare();

  /** Destructor **/
  virtual ~PndMCTestMomentumCompare();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();



 private:
  PndMCMatch* fMCMatch;
  TClonesArray* fTrack;
  TClonesArray* fMCTrack;
  TClonesArray* fTrackCand;
  TH1* fPHisto;
  TH1* fPtHisto;
  TH1* fQualyHisto;

  Int_t fEventNr;

  void Register();

  void Reset();

  void ProduceHits();


  ClassDef(PndMCTestMomentumCompare,1);

};

#endif
