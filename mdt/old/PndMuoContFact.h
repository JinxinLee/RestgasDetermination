#ifndef PNDMUOCONTFACT_H
#define PNDMUOCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class PndMuoContFact : public FairContFact {
private:
  void setAllContainers();
public:
  PndMuoContFact();
  ~PndMuoContFact() {}
  FairParSet* createContainer(FairContainer*);
 /*  void  activateParIo(FairParIo* io); */
  ClassDef( PndMuoContFact,0) // Factory for all MUO parameter containers
};

#endif  /* !PNDMUOCONTFACT_H */
