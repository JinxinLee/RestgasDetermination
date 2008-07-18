/////////////////////////////////////////////////////////////
//
//  PndDrcContFact
//
//  Factory for the parameter containers in libDrc
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "PndDrcContFact.h"
#include "CbmRuntimeDb.h"
#include "PndGeoDrcPar.h"
#include "CbmParRootFileIo.h"
#include "CbmParAsciiFileIo.h"
#include <iostream>
#include <iomanip>

ClassImp(PndDrcContFact)

static PndDrcContFact gPndDrcContFact;

PndDrcContFact::PndDrcContFact() {
  // Constructor (called when the library is loaded)
  fName="PndDrcContFact";
  fTitle="Factory for parameter containers in libDrc";
  setAllContainers();
  CbmRuntimeDb::instance()->addContFactory(this);
}

void PndDrcContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the STS library.*/

    CbmContainer* p= new CbmContainer("PndGeoDrcPar",
                                          "Drc Geometry Parameters",
                                          "TestDefaultContext");
    p->addContext("TestNonDefaultContext");

    containers->Add(p);
}

CbmParSet* PndDrcContFact::createContainer(CbmContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  CbmParSet* p=NULL;
  if (strcmp(name,"PndGeoDrcPar")==0) {
    p=new PndGeoDrcPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}

// void  PndDrcContFact::activateParIo(CbmParIo* io) {
//   // activates the input/output class for the parameters
//   // needed by the Drc
// return;
// }
