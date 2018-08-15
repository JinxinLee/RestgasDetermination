#ifndef PNDFTSCONTFACT_H
#define PNDFTSCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class PndFtsContFact : public FairContFact 
{
private:
  void setAllContainers();
public:
  PndFtsContFact();
  ~PndFtsContFact() {}
  FairParSet* createContainer(FairContainer*);
//  void  activateParIo(FairParIo* io);
  ClassDef( PndFtsContFact,0) // Factory for all FTS parameter containers
};

#endif  /* !PNDFTSCONTFACT_H */
