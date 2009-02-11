/////////////////////////////////////////////////////////////
//
//  CbmplaneContFact
//
//  Factory for the parameter containers in libPlane
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "CbmPlaneContFact.h"
#include "FairRuntimeDb.h"
#include "CbmGeoPlanePar.h"
//#include "FairParRootFileIo.h"
//#include "FairParAsciiFileIo.h"
#include <iostream>
#include <iomanip>

ClassImp(CbmPlaneContFact)

static CbmPlaneContFact gCbmPlaneContFact;

CbmPlaneContFact::CbmPlaneContFact() {
  // Constructor (called when the library is loaded)
  fName="CbmPlaneContFact";
  fTitle="Factory for parameter containers in libPlane";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void CbmPlaneContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the STS library.*/

    FairContainer* p= new FairContainer("CbmGeoPlanePar",
                                          "Plane Geometry Parameters",
                                          "TestDefaultContext");
    p->addContext("TestNonDefaultContext");

    containers->Add(p);
}

FairParSet* CbmPlaneContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  FairParSet* p=NULL;
  if (strcmp(name,"CbmGeoPlanePar")==0) {
    p=new CbmGeoPlanePar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}

// void  CbmplaneContFact::activateParIo(FairParIo* io) {
//   // activates the input/output class for the parameters
//   // needed by the plane
// return;
// }
