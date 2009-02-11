/////////////////////////////////////////////////////////////
//
//  PndTpcContFact
//
//  Factory for the parameter containers in libTst
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "PndTpcContFact.h"
#include "FairRuntimeDb.h"
#include "PndTpcGeoPar.h"
#include "PndTpcDigiPar.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
//#include "FairParIo.h"
//#include "PndTpcParRootFileIo.h"
//#include "PndTpcParAsciiFileIo.h"
#include <iostream>
#include <iomanip>

ClassImp(PndTpcContFact)

static PndTpcContFact gPndTpcContFact;

PndTpcContFact::PndTpcContFact() {
  // Constructor (called when the library is loaded)
  fName="PndTpcContFact";
  fTitle="Factory for parameter containers in libPndTpc";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void PndTpcContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the TST library.*/

    FairContainer* p= new FairContainer("PndTpcGeoPar",
                                          "PndTpc Geometry Parameters",
                                          "PndTpcDefaultContext");
    p->addContext("PndTpcNonDefaultContext");

    FairContainer* p2= new FairContainer("PndTpcDigiPar",
				      "PndTpc Digitization Parameters",
				      "PndTpcDefaultContext");

    p2->addContext("PndTpcNonDefaultContext");

    containers->Add(p);
    containers->Add(p2);
}

FairParSet* PndTpcContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  FairParSet* p=NULL;
  if (strcmp(name,"PndTpcGeoPar")==0) {
    p=new PndTpcGeoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  if (strcmp(name, "PndTpcDigiPar")==0){
    p=new PndTpcDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}

//void  PndTpcContFact::activateParIo(FairParIo* io) {
//  if (strcmp(io->IsA()->GetName(),"FairParAsciiFileIo")==0) {
//    PndTpcParAsciiFileIo* p=new PndTpcParAsciiFileIo(((FairParAsciiFileIo*)io)->getFile());
//    io->setDetParIo(p);
//  }
//}
