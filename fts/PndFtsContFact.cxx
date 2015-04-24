/////////////////////////////////////////////////////////////
//
//  PndFtsContFact
//
//  Factory for the parameter containers in libFts
//
/////////////////////////////////////////////////////////////

#include "PndFtsContFact.h"
#include "FairRuntimeDb.h"
#include "PndGeoFtsPar.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include <iostream>
#include <iomanip>

ClassImp(PndFtsContFact)

static PndFtsContFact gPndFtsContFact;

PndFtsContFact::PndFtsContFact() 
{
  // Constructor (called when the library is loaded)
  fName="PndFtsContFact";
  fTitle="Factory for parameter containers in libFts";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void PndFtsContFact::setAllContainers() 
{
    /** Creates the Container objects with all accepted contexts and adds them to
     *  the list of containers for the FTS library.*/
    FairContainer* p2= new FairContainer("PndGeoFtsPar",
				       "Fts Geometry Parameters",
				       "TestDefaultContext");
    p2->addContext("TestNonDefaultContext");
    
    containers->Add(p2);
}

FairParSet* PndFtsContFact::createContainer(FairContainer* c) 
{
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  std::cout << " -I container name " << name << std::endl;
  FairParSet* p=0;

  if (strcmp(name,"PndGeoFtsPar")==0) {
    p=new PndGeoFtsPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}
