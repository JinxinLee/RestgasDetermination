// -------------------------------------------------------------------------
// -----                     PndLheContFact header file                -----
// -----               Created 03/06/08  by S.Spataro                  -----
// -------------------------------------------------------------------------


#ifndef PNDLHECONTFACT_H
#define PNDLHECONTFACT_H

#include "FairContFact.h"

class FairContainer;

class PndLheContFact : public FairContFact {
private:
  void setAllContainers();
public:
	PndLheContFact();
	~PndLheContFact() {}
	FairParSet* createContainer(FairContainer*);
	//void  activateParIo(FairParIo* io);

  ClassDef( PndLheContFact,0) // Factory for LHE Correlation parameter containers
};

#endif  /* !PNDLHECONTFACT_H */
