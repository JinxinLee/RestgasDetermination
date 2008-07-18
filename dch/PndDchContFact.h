#ifndef PNDDCHCONTFACT_H
#define PNDDCHCONTFACT_H

#include "CbmContFact.h"

class CbmContainer;

class PndDchContFact: public CbmContFact {

public:
	PndDchContFact();
	~PndDchContFact() {
	}
	CbmParSet* createContainer(CbmContainer*);

private:
	void setAllContainers();

	ClassDef( PndDchContFact,0) // Factory for all DCH parameter containers
};

#endif  /* !PNDDCHCONTFACT_H */
