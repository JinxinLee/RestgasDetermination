#ifndef PNDGEMCONTFACT_H
#define PNDGEMCONTFACT_H

#include "FairContFact.h"

class FairContainer;
//class FairParIo;

class PndGemContFact : public FairContFact {
public:
  PndGemContFact();
  ~PndGemContFact() {}
  FairParSet* createContainer(FairContainer* c);
private:
  void setAllContainers();
  
  ClassDef( PndGemContFact,2); // Factory for all GEM parameter containers
};

#endif  /* !GEMCONTFACT_H */
