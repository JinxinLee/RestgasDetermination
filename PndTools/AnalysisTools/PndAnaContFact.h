#ifndef PNDANACONTFACT_H
#define PNDANACONTFACT_H

#include "FairContFact.h"

class FairContainer;
//class FairParIo;

class PndAnaContFact : public FairContFact {
public:
  PndAnaContFact();
  ~PndAnaContFact();
  FairParSet* createContainer(FairContainer* c);
  TList* GetParNames() {return fSelectParNames;};
private:
  void setAllContainers();
  TList* fSelectParNames; //!
  ClassDef(PndAnaContFact,1); // Factory for all libAnalysis parameter containers
};

#endif  /* !ANACONTFACT_H */
