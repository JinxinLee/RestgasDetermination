// -------------------------------------------------------------------------
// -----                     CbmHypGeContFact header file                -----

// -------------------------------------------------------------------------


#ifndef PNDHYPGECONTFACT_H
#define PNDHYPGECONTFACT_H

#include "CbmContFact.h"

class CbmContainer;

class PndHypGeContFact : public CbmContFact {
private:
  void setAllContainers();
public:
  PndHypGeContFact();
  ~PndHypGeContFact() {}
  CbmParSet* createContainer(CbmContainer*);

  ClassDef( PndHypGeContFact,0) // Factory for all HYPGE parameter containers
};

#endif  /* !PNDHYPGECONTFACT_H */
