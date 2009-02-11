#ifndef PNDTUTCONTFACT_H
#define PNDTUTCONTFACT_H

#include "FairContFact.h"

class FairContainer;
//class FairParIo;

class PndTutContFact : public FairContFact {
public:
  PndTutContFact();
  ~PndTutContFact() {}
  FairParSet* createContainer(FairContainer* c);
private:
  void setAllContainers();
  
  ClassDef( PndTutContFact,0); // Factory for all MVD parameter containers
};

#endif  /* !TUTCONTFACT_H */
