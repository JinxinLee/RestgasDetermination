/////////////////////////////////////////////////////////////
//
// PndRpcContFact
//
// Factory for the parameter containers in libRpc
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "PndRpcContFact.h"
#include "CbmRuntimeDb.h"
#include "PndRpcGeoPar.h"
#include <iostream>
#include <iomanip>

ClassImp(PndRpcContFact)

static 	PndRpcContFact gRpcContFact;

PndRpcContFact::PndRpcContFact() {
  // Constructor (called when the library is loaded)
  fName="PndRpcContFact";
  fTitle="Factory for parameter containers in libRpc";
  setAllContainers();
  CbmRuntimeDb::instance()->addContFactory(this);
}

void PndRpcContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the Rpc library.*/

    CbmContainer* p= new CbmContainer("PndRpcGeoPar",
                                          "Rpc Geometry Parameters",
                                          "TestDefaultContext");
    p->addContext("TestNonDefaultContext");

    containers->Add(p);
}

CbmParSet* PndRpcContFact::createContainer(CbmContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  CbmParSet* p=NULL;
  if (strcmp(name,"PndRpcGeoPar")==0) {
    p=new PndRpcGeoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}

