// -------------------------------------------------------------------------
// -----                     PndLheContFact header file                -----
// -----               Created 03/06/08  by S.Spataro                  -----
// -------------------------------------------------------------------------


#ifndef PNDLHECONTFACT_H
#define PNDLHECONTFACT_H

#include "CbmContFact.h"

class CbmContainer;

class PndLheContFact : public CbmContFact {
private:
  void setAllContainers();
public:
	PndLheContFact();
	~PndLheContFact() {}
	CbmParSet* createContainer(CbmContainer*);
	//void  activateParIo(CbmParIo* io);

  ClassDef( PndLheContFact,0) // Factory for LHE Correlation parameter containers
};

#endif  /* !PNDLHECONTFACT_H */
