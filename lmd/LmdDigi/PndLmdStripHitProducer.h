#ifndef PNDLMDSTRIPHITPRODUCER_H
#define PNDLMDSTRIPHITPRODUCER_H

#include "PndSdsStripHitProducer.h"

class PndLmdStripHitProducer : public PndSdsStripHitProducer
{
 public:

  /** Default constructor \n
   * creates object with parameters taken implicitly from DigiPar-File
   **/
  PndLmdStripHitProducer();

  /** Destructor **/
  virtual ~PndLmdStripHitProducer();

  virtual void SetBranchNames(TString inBranchname, TString outBranchname, TString folderName);
  virtual void SetBranchNames();

  virtual void SetParContainers();
  virtual void SetCalculators();

  ClassDef(PndLmdStripHitProducer,65);

};

#endif
