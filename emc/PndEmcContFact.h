// -------------------------------------------------------------------------
// -----                     PndEmcContFact header file                -----
// -----               Created 14/08/06  by S.Spataro                  -----
// -------------------------------------------------------------------------


#ifndef PNDEMCCONTFACT_H
#define PNDEMCCONTFACT_H

#include "CbmContFact.h"

class CbmContainer;

class PndEmcContFact : public CbmContFact {
private:
  void setAllContainers();
public:
	PndEmcContFact();
	~PndEmcContFact() {}
	CbmParSet* createContainer(CbmContainer*);
	//void  activateParIo(CbmParIo* io);

  ClassDef( PndEmcContFact,0) // Factory for all EMC parameter containers
};

#endif  /* !PNDEMCCONTFACT_H */
