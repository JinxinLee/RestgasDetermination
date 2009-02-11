#ifndef TPCCONTFACT_H
#define TPCCONTFACT_H

#include "FairContFact.h"

class FairContainer;
//class FairParIo;

class PndTpcContFact : public FairContFact {
private:
  void setAllContainers();
public:
  PndTpcContFact();
  ~PndTpcContFact() {}
  FairParSet* createContainer(FairContainer*);
  //void  activateParIo(FairParIo* io); 
  ClassDef( PndTpcContFact,1) // Factory for all TPC parameter containers
};

#endif  
