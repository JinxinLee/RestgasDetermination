#ifndef PNDRPCCONTFACT_H
#define PNDRPCCONTFACT_H

#include "CbmContFact.h"

class CbmContainer;

class PndRpcContFact : public CbmContFact {
private:
  void setAllContainers();
public:
  PndRpcContFact();
  ~PndRpcContFact() {}
  CbmParSet* createContainer(CbmContainer*);
  ClassDef( PndRpcContFact,0) // Factory for all RPC parameter containers
};

#endif  /* !PNDRPCCONTFACT_H */
