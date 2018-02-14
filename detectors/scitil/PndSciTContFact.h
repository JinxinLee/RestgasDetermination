// -------------------------------------------------------------------------
// -----                     CbmHypContFact header file                -----
// -----               Created 14/08/06  by S.Spataro                  -----
// -------------------------------------------------------------------------


#ifndef PNDSCITCONTFACT_H
#define PNDSCITCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class PndSciTContFact : public FairContFact {
private:
  void setAllContainers();
public:
  PndSciTContFact();
  ~PndSciTContFact() {}
  FairParSet* createContainer(FairContainer*);

  ClassDef( PndSciTContFact,0) // Factory for all HYP parameter containers
};

#endif  /* !PNDSCITCONTFACT_H */
