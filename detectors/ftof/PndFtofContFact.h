// -------------------------------------------------------------------------
// -----                     CbmHypContFact header file                -----
// -----               Created 14/08/06  by S.Spataro                  -----
// -------------------------------------------------------------------------


#ifndef PNDFTOFCONTFACT_H
#define PNDFTOFCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class PndFtofContFact : public FairContFact {
private:
  void setAllContainers();
public:
  PndFtofContFact();
  ~PndFtofContFact() {}
  FairParSet* createContainer(FairContainer*);

  ClassDef( PndFtofContFact,0) // Factory for all HYP parameter containers
};

#endif  /* !PNDFTOFCONTFACT_H */
