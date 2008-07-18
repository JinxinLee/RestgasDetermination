// -------------------------------------------------------------------------
// -----                     CbmHypContFact header file                -----
// -----               Created 14/08/06  by S.Spataro                  -----
// -------------------------------------------------------------------------


#ifndef PNDTOFCONTFACT_H
#define PNDTOFCONTFACT_H

#include "CbmContFact.h"

class CbmContainer;

class PndTofContFact : public CbmContFact {
private:
  void setAllContainers();
public:
  PndTofContFact();
  ~PndTofContFact() {}
  CbmParSet* createContainer(CbmContainer*);

  ClassDef( PndTofContFact,0) // Factory for all HYP parameter containers
};

#endif  /* !PNDTOFCONTFACT_H */
