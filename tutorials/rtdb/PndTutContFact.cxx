/////////////////////////////////////////////////////////////
//
//  PndTutContFact
//
//  Factory for the parameter containers in libPndMvd
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "PndTutContFact.h"
#include "CbmRuntimeDb.h"
#include "PndTutPar.h"
#include "CbmParRootFileIo.h"
#include "CbmParAsciiFileIo.h"
#include <iostream>
#include <iomanip>

ClassImp(PndTutContFact);

static PndTutContFact gPndTutContFact;

PndTutContFact::PndTutContFact() {
  // Constructor (called when the library is loaded)
  fName="PndTutContFact";
  fTitle="Tutorial factory for parameter containers";
  setAllContainers();
  CbmRuntimeDb::instance()->addContFactory(this);
}

void PndTutContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers.*/

    CbmContainer* p1 = new CbmContainer("TUTParDefault", "Default tutorial parameters", "TestDefaultContext");
    p1->addContext("TestNonDefaultContext");
    containers->Add(p1);

    CbmContainer* p2 = new CbmContainer("TUTParAlternative", "Alternative to the default tutorial parameters", "TestDefaultContext");
    p2->addContext("TestNonDefaultContext");
    containers->Add(p2);

    
}

CbmParSet* PndTutContFact::createContainer(CbmContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  CbmParSet* p=NULL;
  if (strcmp(name,"TUTParDefault")==0) {
  p=new PndTutPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  
  if (strcmp(name,"TUTParAlternative")==0) {
  p=new PndTutPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  
  return p;
}
