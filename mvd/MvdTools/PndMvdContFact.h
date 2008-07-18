#ifndef PNDMVDCONTFACT_H
#define PNDMVDCONTFACT_H

#include "CbmContFact.h"

class CbmContainer;
//class CbmParIo;

class PndMvdContFact : public CbmContFact {
public:
  PndMvdContFact();
  ~PndMvdContFact() {}
  CbmParSet* createContainer(CbmContainer* c);
private:
  void setAllContainers();
  
  ClassDef( PndMvdContFact,2); // Factory for all MVD parameter containers
};

#endif  /* !MVDCONTFACT_H */
