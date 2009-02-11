#ifndef PNDDRCCONTFACT_H
#define PNDDRCCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class PndDrcContFact : public FairContFact {
private:
  void setAllContainers();
public:
  PndDrcContFact();
  ~PndDrcContFact() {}
  FairParSet* createContainer(FairContainer*);
 /*  void  activateParIo(FairParIo* io); */
  ClassDef( PndDrcContFact,0) // Factory for all DRC parameter containers
};

#endif  /* !PNDDRCCONTFACT_H */
