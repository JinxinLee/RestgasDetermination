#ifndef PNDSTTCONTFACT_H
#define PNDSTTCONTFACT_H

#include "CbmContFact.h"

class CbmContainer;

class PndSttContFact : public CbmContFact 
{
private:
  void setAllContainers();
public:
  PndSttContFact();
  ~PndSttContFact() {}
  CbmParSet* createContainer(CbmContainer*);
//  void  activateParIo(CbmParIo* io);
  ClassDef( PndSttContFact,0) // Factory for all STT parameter containers
};

#endif  /* !PNDSTTCONTFACT_H */
