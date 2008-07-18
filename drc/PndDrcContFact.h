#ifndef PNDDRCCONTFACT_H
#define PNDDRCCONTFACT_H

#include "CbmContFact.h"

class CbmContainer;

class PndDrcContFact : public CbmContFact {
private:
  void setAllContainers();
public:
  PndDrcContFact();
  ~PndDrcContFact() {}
  CbmParSet* createContainer(CbmContainer*);
 /*  void  activateParIo(CbmParIo* io); */
  ClassDef( PndDrcContFact,0) // Factory for all DRC parameter containers
};

#endif  /* !PNDDRCCONTFACT_H */
