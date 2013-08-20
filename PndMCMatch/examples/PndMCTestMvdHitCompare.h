// -------------------------------------------------------------------------
// -----                 PNDMCMATCHSELECTORTASK header file             -----
// -----                  Created 18/01/10  by T.Stockmanns             -----
// -------------------------------------------------------------------------


/** PNDMCMATCHSELECTORTASK.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Displays all available informations for a given event
 **/


#ifndef PNDMCTESTHITCOMPARE_H
#define PNDMCTESTHITCOMPARE_H


// framework includes
#include "FairTask.h"
#include "PndMCMatch.h"
#include "PndDetectorList.h"


#include <vector>
#include <map>

class TClonesArray;

class PndMCTestHitCompare : public FairTask
{
 public:

  /** Default constructor **/
	PndMCTestHitCompare();

  /** Destructor **/
  virtual ~PndMCTestHitCompare();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();



 private:
  PndMCMatch* fMCMatch;
  TClonesArray* fStripHit;
  TClonesArray* fPixelHit;
  TClonesArray* fMCPoint;

  void Register();

  void Reset();

  void ProduceHits();


  ClassDef(PndMCTestHitCompare,1);

};

#endif
