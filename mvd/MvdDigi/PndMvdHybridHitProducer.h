
/** PndMvdHybridHitProducer.h
 **
 **/

#ifndef PNDMVDHYBRIDHITPRODUCER_H
#define PNDMVDHYBRIDHITPRODUCER_H 1

#include "PndSdsHybridHitProducer.h"
#include "PndSdsTotDigiPar.h"
#include "TString.h"

class PndMvdHybridHitProducer : public PndSdsHybridHitProducer
{
 public:

  /** Default constructor **/
 PndMvdHybridHitProducer();

  /** Destructor **/
  virtual ~PndMvdHybridHitProducer();

  /** Specific Parameter set loading **/
  virtual void SetParContainers();
  
  virtual InitStatus Init();

  /** Specify the branch & folder names for the I/O **/
  void SetBranchNames(TString inBranchname, TString outBranchname, TString folderName);
  
  /** Take default naming **/
  virtual void SetBranchNames();
    
  ClassDef(PndMvdHybridHitProducer,9);

};

#endif
