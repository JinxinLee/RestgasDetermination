/////////////////////////////////////////////////////////////
//
//  PndPidContFact
//
//  Factory for the parameter containers in libPid
//
//  Created 03/07/09  by S.Spataro
//
/////////////////////////////////////////////////////////////

#include "PndPidContFact.h"
#include "FairRuntimeDb.h"
#include "PndPidCorrPar.h"
#include "FairParAsciiFileIo.h"
#include "TClass.h"										
#include <iostream>
#include <iomanip>

ClassImp(PndPidContFact)

static PndPidContFact gPndPidContFact;

PndPidContFact::PndPidContFact() {
  // Constructor (called when the library is loaded)
  fName="PndPidContFact";
  fTitle="Factory for parameter containers in libPid";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void PndPidContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the Pid library.*/

	FairContainer* p1= new FairContainer("PndPidCorrPar",
														"Pid Correlation Parameters",
														"TestDefaultContext");
	p1->addContext("TestNonDefaultContext");
	
	containers->Add(p1);
}

FairParSet* PndPidContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  std::cout << " -I container name " << name << std::endl;
  FairParSet* p=NULL;
  if (strcmp(name,"PndPidCorrPar")==0) {
    p=new PndPidCorrPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  
  return p;
}
