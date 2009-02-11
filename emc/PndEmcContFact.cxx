/////////////////////////////////////////////////////////////
//
//  PndEmcContFact
//
//  Factory for the parameter containers in libEmc
//
//  Created 14/08/06  by S.Spataro
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "PndEmcContFact.h"
#include "FairRuntimeDb.h"
#include "PndEmcDigiPar.h"
#include "PndEmcRecoPar.h"					
#include "FairParAsciiFileIo.h"
#include "TClass.h"										
#include <iostream>
#include <iomanip>

ClassImp(PndEmcContFact)

static PndEmcContFact gPndEmcContFact;

PndEmcContFact::PndEmcContFact() {
  // Constructor (called when the library is loaded)
  fName="PndEmcContFact";
  fTitle="Factory for parameter containers in libEmc";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void PndEmcContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the Emc library.*/
	
	FairContainer* p1= new FairContainer("PndEmcDigiPar",
														"Emc Digitalization Parameters",
														"TestDefaultContext");
	p1->addContext("TestNonDefaultContext");
	
	FairContainer* p2= new FairContainer("PndEmcRecoPar",
														"Emc Reconstruction Parameters",
														"TestDefaultContext");
	p2->addContext("TestNonDefaultContext");

	containers->Add(p1);
	containers->Add(p2);
}

FairParSet* PndEmcContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  cout << " -I container name " << name << endl;
  FairParSet* p=NULL;
  
  if (strcmp(name,"PndEmcDigiPar")==0) {
    p=new PndEmcDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  
  if (strcmp(name,"PndEmcRecoPar")==0) {
    p=new PndEmcRecoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  
  return p;
}

