/////////////////////////////////////////////////////////////
//
//  PndMuoContFact
//
//  Factory for the parameter containers in libMuo
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "PndMuoContFact.h"
#include "FairRuntimeDb.h"
#include "PndGeoMuoPar.h"
//#include "FairParRootFileIo.h"
//#include "FairParAsciiFileIo.h"
#include <iostream>
#include <iomanip>

ClassImp(PndMuoContFact)

static PndMuoContFact gPndMuoContFact;

PndMuoContFact::PndMuoContFact() {
  // Constructor (called when the library is loaded)
  fName="PndMuoContFact";
  fTitle="Factory for parameter containers in libMuo";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void PndMuoContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the Muo library.*/

    FairContainer* p= new FairContainer("PndGeoMuoPar",
                                          "Muo Geometry Parameters",
                                          "TestDefaultContext");
    p->addContext("TestNonDefaultContext");

    containers->Add(p);
}

FairParSet* PndMuoContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  FairParSet* p=NULL;
  if (strcmp(name,"PndGeoMuoPar")==0) {
    p=new PndGeoMuoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}

// void  PndMuoContFact::activateParIo(FairParIo* io) {
//   // activates the input/output class for the parameters
//   // needed by the Muo
// return;
// }
