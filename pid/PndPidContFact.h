// -------------------------------------------------------------------------
// -----                     PndPidContFact header file                -----
// -----               Created 03/07/09  by S.Spataro                  -----
// -------------------------------------------------------------------------


#ifndef PNDPIDCONTFACT_H
#define PNDPIDCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class PndPidContFact : public FairContFact {
private:
  void setAllContainers();
public:
	PndPidContFact();
	~PndPidContFact() {}
	FairParSet* createContainer(FairContainer*);
	//void  activateParIo(FairParIo* io);

  ClassDef( PndPidContFact,0) // Factory for PID Correlation parameter containers
};

#endif  /* !PNDPIDCONTFACT_H */
