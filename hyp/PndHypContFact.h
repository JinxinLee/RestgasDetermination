// -------------------------------------------------------------------------
// -----                     CbmHypContFact header file                -----
// -----               Created A.Sanchez                  -----
// -------------------------------------------------------------------------


#ifndef PNDHYPCONTFACT_H
#define PNDHYPCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class PndHypContFact : public FairContFact {
private:
  void setAllContainers();
public:
  PndHypContFact();
  ~PndHypContFact() {}
  FairParSet* createContainer(FairContainer*);

  ClassDef( PndHypContFact,2) // Factory for all HYP parameter containers
};

#endif  /* !PNDHYPCONTFACT_H */
