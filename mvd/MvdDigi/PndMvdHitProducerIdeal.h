// -------------------------------------------------------------------------
// -----                 CbmMvdHitProducerIdel header file             -----
// -------------------------------------------------------------------------


/** CbmMvdHitProducerIdeal.h
 *@author R.Kliemt
 ** Inherit from Sds Ideal hitproducer
 **/


#ifndef PNDMVDHITPRODUCERIDEAL_H
#define PNDMVDHITPRODUCERIDEAL_H 1

#include "PndSdsHitProducerIdeal.h"

class PndMvdHitProducerIdeal : public PndSdsHitProducerIdeal
{
 public:

  /** Default constructor **/  
  PndMvdHitProducerIdeal();


  /** Destructor **/
  virtual ~PndMvdHitProducerIdeal();

  void SetBranchNames(TString inBranchname, TString outBranchname, TString folderName);
  virtual void SetBranchNames();

  ClassDef(PndMvdHitProducerIdeal,4);

};

#endif
