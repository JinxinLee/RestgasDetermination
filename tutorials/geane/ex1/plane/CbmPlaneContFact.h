#ifndef CBMPlaneCONTFACT_H
#define CBMPlaneCONTFACT_H

#include "CbmContFact.h"

class CbmContainer;

class CbmPlaneContFact : public CbmContFact {
private:
  void setAllContainers();
public:
  CbmPlaneContFact();
  ~CbmPlaneContFact() {}
  CbmParSet* createContainer(CbmContainer*);
 /*  void  activateParIo(CbmParIo* io); */
  ClassDef( CbmPlaneContFact,0) // Factory for all Plane parameter containers
};

#endif  /* !CBMPlaneCONTFACT_H */
