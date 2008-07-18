/////////////////////////////////////////////////////////////
//
//  CbmHypContFact
//
//  Factory for the parameter containers in libHyp
//
//  
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "PndTofContFact.h"
#include "CbmRuntimeDb.h"
#include "iostream"
#include "CbmParRootFileIo.h"
#include "CbmParAsciiFileIo.h"
#include "PndGeoTofPar.h"
#include <iostream>
#include <iomanip>

ClassImp(PndTofContFact)

static PndTofContFact gPndTofContFact;

PndTofContFact::PndTofContFact() {
  // Constructor (called when the library is loaded)
  fName="PndTofContFact";
  fTitle="Factory for parameter containers in libTof";
  setAllContainers();
  CbmRuntimeDb::instance()->addContFactory(this);
}

void PndTofContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the Hyp library.*/

    CbmContainer* p= new CbmContainer("PndGeoTofPar",
                                          "tof Geometry Parameters",
                                          "TestDefaultContext");
    p->addContext("TestNonDefaultContext");

    containers->Add(p);
}

CbmParSet* PndTofContFact::createContainer(CbmContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  CbmParSet* p=NULL;
  if (strcmp(name,"PndGeoTofPar")==0) {
    p=new PndGeoTofPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}
