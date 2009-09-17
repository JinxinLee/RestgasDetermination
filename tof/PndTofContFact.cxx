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
#include "FairRuntimeDb.h"
//#include "iostream"
//#include "FairParRootFileIo.h"
//#include "FairParAsciiFileIo.h"
#include "PndGeoTofPar.h"
#include <iostream>
//#include <iomanip>

ClassImp(PndTofContFact)

static PndTofContFact gPndTofContFact;

PndTofContFact::PndTofContFact() {
  // Constructor (called when the library is loaded)
  fName="PndTofContFact";
  fTitle="Factory for parameter containers in libTof";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void PndTofContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the Hyp library.*/

    FairContainer* p= new FairContainer("PndGeoTofPar",
                                          "tof Geometry Parameters",
                                          "TestDefaultContext");
    p->addContext("TestNonDefaultContext");

    containers->Add(p);
}

FairParSet* PndTofContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  FairParSet* p=NULL;
  if (strcmp(name,"PndGeoTofPar")==0) {
    p=new PndGeoTofPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}
