
#ifndef PNDMDTCONTFACT_H
#define PNDMDTCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class PndMdtContFact : public FairContFact {
private:
  void setAllContainers();
public:
  PndMdtContFact();
  ~PndMdtContFact() {}
  FairParSet* createContainer(FairContainer*);
 /*  void  activateParIo(FairParIo* io); */
  ClassDef( PndMdtContFact,0) // Factory for all MDT parameter containers
};

#endif  /* !PNDMDTCONTFACT_H */
