/////////////////////////////////////////////////////////////
//
//  PndAnaContFact
//
//  Factory for the parameter containers in libAnalysisTools
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "PndAnaContFact.h"
#include "PndAnaSelectorPar.h"
#include "FairRuntimeDb.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include "TList.h"
#include "TObjString.h"
#include <iostream>
#include <iomanip>

ClassImp(PndAnaContFact);

static PndAnaContFact gPndAnaContFact;

PndAnaContFact::PndAnaContFact() {
  // Constructor (called when the library is loaded)
  fName="PndAnaContFact";
  fTitle="Factory for parameter containers in analysis tools";
  fSelectParNames = new TList();
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}
PndAnaContFact::~PndAnaContFact(){
  if(0!=fSelectParNames) 
  {
    fSelectParNames->Delete(); 
    delete fSelectParNames;
  }
}


void PndAnaContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the analysis pid selction criteria library.*/
  
//  FairContainer* p= new FairContainer("ANAPidSelectionFlux","Flux weights", "TestDefaultContext");
//  p->addContext("TestNonDefaultContext");
//  fSelectParNames->Add(new TObjString(p->GetName()));
//  containers->Add(p);
  
  FairContainer* p2 = new FairContainer("ANAPidSelectionsDefault","Default PID selctrion criteria parameters", "TestDefaultContext");
  p2->addContext("TestNonDefaultContext");
  fSelectParNames->Add(new TObjString(p2->GetName()));
  containers->Add(p2);
  
  FairContainer* p3 = new FairContainer("ANAPidSelectionsUser","User PID selctrion criteria parameters", "TestDefaultContext");
  p3->addContext("TestNonDefaultContext");
  fSelectParNames->Add(new TObjString(p3->GetName()));
  containers->Add(p3);

}

FairParSet* PndAnaContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  FairParSet* p=NULL;

//  if (strcmp(name,"ANAPidSelectionFlux")==0) {
//	p=new PndAnaSelectorPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
//  }

  if (strcmp(name,"ANAPidSelectionsDefault")==0) {
  p=new PndAnaSelectorPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  
  if (strcmp(name,"ANAPidSelectionsUser")==0) {
    p=new PndAnaSelectorPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }

  return p;
}
