
#ifndef PNDMDTCONTFACT_H
#define PNDMDTCONTFACT_H

#include "CbmContFact.h"

class CbmContainer;

class PndMdtContFact : public CbmContFact {
private:
  void setAllContainers();
public:
  PndMdtContFact();
  ~PndMdtContFact() {}
  CbmParSet* createContainer(CbmContainer*);
 /*  void  activateParIo(CbmParIo* io); */
  ClassDef( PndMdtContFact,0) // Factory for all MDT parameter containers
};

#endif  /* !PNDMDTCONTFACT_H */
