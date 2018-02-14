#ifndef PNDRICHCONTFACT_H
#define PNDRICHCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class PndRichContFact : public FairContFact
{
  private:
    void setAllContainers();
  public:
    PndRichContFact();
    ~PndRichContFact() {}
    FairParSet* createContainer(FairContainer*);
    ClassDef( PndRichContFact,0) // Factory for all PndRich parameter containers
};

#endif
