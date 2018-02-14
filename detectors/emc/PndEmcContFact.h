// -------------------------------------------------------------------------
// -----                     PndEmcContFact header file                -----
// -----               Created 14/08/06  by S.Spataro                  -----
// -------------------------------------------------------------------------


#ifndef PNDEMCCONTFACT_H
#define PNDEMCCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class PndEmcContFact : public FairContFact {
private:
  void setAllContainers();
public:
	PndEmcContFact();
	~PndEmcContFact() {}
	FairParSet* createContainer(FairContainer*);
	//void  activateParIo(FairParIo* io);

  ClassDef( PndEmcContFact,0) // Factory for all EMC parameter containers
};

#endif  /* !PNDEMCCONTFACT_H */
