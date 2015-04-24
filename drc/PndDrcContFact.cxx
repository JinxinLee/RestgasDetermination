/////////////////////////////////////////////////////////////
//
//  PndDrcContFact
//
//  Factory for the parameter containers in libDrc
//
/////////////////////////////////////////////////////////////

#include "PndDrcContFact.h"
#include "FairRuntimeDb.h"
#include "PndGeoDrcPar.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include <iostream>
#include <iomanip>

ClassImp(PndDrcContFact)

static PndDrcContFact gPndDrcContFact;

PndDrcContFact::PndDrcContFact() {
  // Constructor (called when the library is loaded)
  fName="PndDrcContFact";
  fTitle="Factory for parameter containers in libDrc";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void PndDrcContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the STS library.*/

    FairContainer* p= new FairContainer("PndGeoDrcPar",
                                          "Drc Geometry Parameters",
                                          "TestDefaultContext");
    p->addContext("TestNonDefaultContext");

    containers->Add(p);
}

FairParSet* PndDrcContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  FairParSet* p=NULL;
  if (strcmp(name,"PndGeoDrcPar")==0) {
    p=new PndGeoDrcPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}

// void  PndDrcContFact::activateParIo(FairParIo* io) {
//   // activates the input/output class for the parameters
//   // needed by the Drc
// return;
// }
