/////////////////////////////////////////////////////////////
//
//  CbmplaneContFact
//
//  Factory for the parameter containers in libPlane
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "CbmPlaneContFact.h"
#include "CbmRuntimeDb.h"
#include "CbmGeoPlanePar.h"
//#include "CbmParRootFileIo.h"
//#include "CbmParAsciiFileIo.h"
#include <iostream>
#include <iomanip>

ClassImp(CbmPlaneContFact)

static CbmPlaneContFact gCbmPlaneContFact;

CbmPlaneContFact::CbmPlaneContFact() {
  // Constructor (called when the library is loaded)
  fName="CbmPlaneContFact";
  fTitle="Factory for parameter containers in libPlane";
  setAllContainers();
  CbmRuntimeDb::instance()->addContFactory(this);
}

void CbmPlaneContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the STS library.*/

    CbmContainer* p= new CbmContainer("CbmGeoPlanePar",
                                          "Plane Geometry Parameters",
                                          "TestDefaultContext");
    p->addContext("TestNonDefaultContext");

    containers->Add(p);
}

CbmParSet* CbmPlaneContFact::createContainer(CbmContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  CbmParSet* p=NULL;
  if (strcmp(name,"CbmGeoPlanePar")==0) {
    p=new CbmGeoPlanePar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}

// void  CbmplaneContFact::activateParIo(CbmParIo* io) {
//   // activates the input/output class for the parameters
//   // needed by the plane
// return;
// }
