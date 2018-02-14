#ifndef PNDSTTCONTFACT_H
#define PNDSTTCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class PndSttContFact : public FairContFact 
{
private:
  void setAllContainers();
public:
  PndSttContFact();
  ~PndSttContFact() {}
  FairParSet* createContainer(FairContainer*);
//  void  activateParIo(FairParIo* io);
  ClassDef( PndSttContFact,0) // Factory for all STT parameter containers
};

#endif  /* !PNDSTTCONTFACT_H */
