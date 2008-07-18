#ifndef PNDTUTCONTFACT_H
#define PNDTUTCONTFACT_H

#include "CbmContFact.h"

class CbmContainer;
//class CbmParIo;

class PndTutContFact : public CbmContFact {
public:
  PndTutContFact();
  ~PndTutContFact() {}
  CbmParSet* createContainer(CbmContainer* c);
private:
  void setAllContainers();
  
  ClassDef( PndTutContFact,0); // Factory for all MVD parameter containers
};

#endif  /* !TUTCONTFACT_H */
