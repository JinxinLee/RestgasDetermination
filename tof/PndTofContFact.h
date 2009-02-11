// -------------------------------------------------------------------------
// -----                     CbmHypContFact header file                -----
// -----               Created 14/08/06  by S.Spataro                  -----
// -------------------------------------------------------------------------


#ifndef PNDTOFCONTFACT_H
#define PNDTOFCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class PndTofContFact : public FairContFact {
private:
  void setAllContainers();
public:
  PndTofContFact();
  ~PndTofContFact() {}
  FairParSet* createContainer(FairContainer*);

  ClassDef( PndTofContFact,0) // Factory for all HYP parameter containers
};

#endif  /* !PNDTOFCONTFACT_H */
