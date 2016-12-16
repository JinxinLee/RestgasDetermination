#ifndef PNDMVDCONTFACT_H
#define PNDMVDCONTFACT_H 1

#include "FairContFact.h"

class FairContainer;
//class FairParIo;

class PndMvdContFact : public FairContFact {
public:
  PndMvdContFact();
  ~PndMvdContFact();
  PndMvdContFact(const PndMvdContFact& o) :
    fDigiParNames(o.fDigiParNames)
  {};
  PndMvdContFact& operator=(const PndMvdContFact& o)
  {
    fDigiParNames=o.fDigiParNames;
    return *this;
  };
  FairParSet* createContainer(FairContainer* c);
  TList* GetDigiParNames() {return fDigiParNames;};
private:
  void setAllContainers();
  TList* fDigiParNames; //!
  ClassDef( PndMvdContFact,3); // Factory for all MVD parameter containers
};

#endif  /* !MVDCONTFACT_H */
