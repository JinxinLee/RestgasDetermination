#ifndef PNDDCHCONTFACT_H
#define PNDDCHCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class PndDchContFact: public FairContFact {

public:
	PndDchContFact();
	~PndDchContFact() {
	}
	FairParSet* createContainer(FairContainer*);

private:
	void setAllContainers();

	ClassDef( PndDchContFact,0) // Factory for all DCH parameter containers
};

#endif  /* !PNDDCHCONTFACT_H */
