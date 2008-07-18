/////////////////////////////////////////////////////////////
//
//  PndMuoContFact
//
//  Factory for the parameter containers in libMuo
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "PndMuoContFact.h"
#include "CbmRuntimeDb.h"
#include "PndGeoMuoPar.h"
//#include "CbmParRootFileIo.h"
//#include "CbmParAsciiFileIo.h"
#include <iostream>
#include <iomanip>

ClassImp(PndMuoContFact)

static PndMuoContFact gPndMuoContFact;

PndMuoContFact::PndMuoContFact() {
  // Constructor (called when the library is loaded)
  fName="PndMuoContFact";
  fTitle="Factory for parameter containers in libMuo";
  setAllContainers();
  CbmRuntimeDb::instance()->addContFactory(this);
}

void PndMuoContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the Muo library.*/

    CbmContainer* p= new CbmContainer("PndGeoMuoPar",
                                          "Muo Geometry Parameters",
                                          "TestDefaultContext");
    p->addContext("TestNonDefaultContext");

    containers->Add(p);
}

CbmParSet* PndMuoContFact::createContainer(CbmContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  CbmParSet* p=NULL;
  if (strcmp(name,"PndGeoMuoPar")==0) {
    p=new PndGeoMuoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}

// void  PndMuoContFact::activateParIo(CbmParIo* io) {
//   // activates the input/output class for the parameters
//   // needed by the Muo
// return;
// }
