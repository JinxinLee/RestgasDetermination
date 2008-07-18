// -------------------------------------------------------------------------
// -----                     CbmHypContFact header file                -----
// -----               Created A.Sanchez                  -----
// -------------------------------------------------------------------------


#ifndef PNDHYPCONTFACT_H
#define PNDHYPCONTFACT_H

#include "CbmContFact.h"

class CbmContainer;

class PndHypContFact : public CbmContFact {
private:
  void setAllContainers();
public:
  PndHypContFact();
  ~PndHypContFact() {}
  CbmParSet* createContainer(CbmContainer*);

  ClassDef( PndHypContFact,2) // Factory for all HYP parameter containers
};

#endif  /* !PNDHYPCONTFACT_H */
