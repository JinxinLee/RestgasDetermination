#ifndef TPCCONTFACT_H
#define TPCCONTFACT_H

#include "CbmContFact.h"

class CbmContainer;
//class CbmParIo;

class PndTpcContFact : public CbmContFact {
private:
  void setAllContainers();
public:
  PndTpcContFact();
  ~PndTpcContFact() {}
  CbmParSet* createContainer(CbmContainer*);
  //void  activateParIo(CbmParIo* io); 
  ClassDef( PndTpcContFact,1) // Factory for all TPC parameter containers
};

#endif  
