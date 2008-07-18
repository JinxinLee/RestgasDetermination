/**
 ** Parameter container factory for field parameter container
 **/

#ifndef PNDFIELDCONTFACT_H
#define PNDFIELDCONTFACT_H 1

#include "CbmContFact.h"

class CbmContainer;
class CbmParSet;

class PndFieldContFact : public CbmContFact
{

 public:

  /** Constructor **/
  PndFieldContFact();


  /** Destructor **/
  ~PndFieldContFact();


  /** Create containers
   ** Creates the requested parameter sets (PndFieldPar) 
   **/
  CbmParSet* createContainer(CbmContainer* container);

 private:

  /** Set all containers  
   ** Creates container objects with all accepted contexts and adds them
   ** to the list of containers for the field library. 
   **/
  void SetAllContainers();


  ClassDef(PndFieldContFact,1);

};

#endif
