/*
 * Updated by h.xu@fz-juelich.de on Jun30.2010
 * To match the base class SdsStripHitProducer,the virtual functions SetParContainers() and
 * SetCalculators() were reloaded.
 */



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
  virtual void SetCalculators();
  virtual void SetParContainers();


  ClassDef(PndLmdStripHitProducer,65);

};

#endif
