#ifndef CBMPlaneCONTFACT_H
#define CBMPlaneCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class CbmPlaneContFact : public FairContFact {
private:
  void setAllContainers();
public:
  CbmPlaneContFact();
  ~CbmPlaneContFact() {}
  FairParSet* createContainer(FairContainer*);
 /*  void  activateParIo(FairParIo* io); */
  ClassDef( CbmPlaneContFact,0) // Factory for all Plane parameter containers
};

#endif  /* !CBMPlaneCONTFACT_H */
