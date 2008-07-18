#ifndef PNDMUOCONTFACT_H
#define PNDMUOCONTFACT_H

#include "CbmContFact.h"

class CbmContainer;

class PndMuoContFact : public CbmContFact {
private:
  void setAllContainers();
public:
  PndMuoContFact();
  ~PndMuoContFact() {}
  CbmParSet* createContainer(CbmContainer*);
 /*  void  activateParIo(CbmParIo* io); */
  ClassDef( PndMuoContFact,0) // Factory for all MUO parameter containers
};

#endif  /* !PNDMUOCONTFACT_H */
