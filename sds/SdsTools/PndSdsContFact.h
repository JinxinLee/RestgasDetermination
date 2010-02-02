#ifndef PNDSDSCONTFACT_H
#define PNDSDSCONTFACT_H

#include "FairContFact.h"

class FairContainer;
//class FairParIo;

class PndSdsContFact : public FairContFact {
public:
  PndSdsContFact();
  ~PndSdsContFact();
  FairParSet* createContainer(FairContainer* c);
  TList* GetDigiParNames() {return fDigiParNames;};
private:
  void setAllContainers();
  TList* fDigiParNames; //!
  ClassDef( PndSdsContFact,2); // Factory for all SDS parameter containers
};

#endif  /* !SDSCONTFACT_H */
