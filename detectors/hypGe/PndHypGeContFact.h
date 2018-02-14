// -------------------------------------------------------------------------
// -----                     CbmHypGeContFact header file                -----

// -------------------------------------------------------------------------


#ifndef PNDHYPGECONTFACT_H
#define PNDHYPGECONTFACT_H

#include "FairContFact.h"

class FairContainer;

class PndHypGeContFact : public FairContFact {
private:
  void setAllContainers();
public:
  PndHypGeContFact();
  ~PndHypGeContFact() {}
  FairParSet* createContainer(FairContainer*);

  ClassDef( PndHypGeContFact,0) // Factory for all HYPGE parameter containers
};

#endif  /* !PNDHYPGECONTFACT_H */
