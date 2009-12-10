#ifndef PNDMVDCONTFACT_H
#define PNDMVDCONTFACT_H

#include "FairContFact.h"

class FairContainer;
//class FairParIo;

class PndMvdContFact : public FairContFact {
public:
  PndMvdContFact();
  ~PndMvdContFact();
  FairParSet* createContainer(FairContainer* c);
  TList* GetDigiParNames() {return fDigiParNames;};
private:
  void setAllContainers();
  TList* fDigiParNames; //!
  ClassDef( PndMvdContFact,2); // Factory for all MVD parameter containers
};

#endif  /* !MVDCONTFACT_H */
