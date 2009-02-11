#ifndef PNDRPCCONTFACT_H
#define PNDRPCCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class PndRpcContFact : public FairContFact {
private:
  void setAllContainers();
public:
  PndRpcContFact();
  ~PndRpcContFact() {}
  FairParSet* createContainer(FairContainer*);
  ClassDef( PndRpcContFact,0) // Factory for all RPC parameter containers
};

#endif  /* !PNDRPCCONTFACT_H */
