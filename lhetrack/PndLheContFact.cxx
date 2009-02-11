/////////////////////////////////////////////////////////////
//
//  PndLheContFact
//
//  Factory for the parameter containers in libLhe
//
//  Created 14/08/06  by S.Spataro
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "PndLheContFact.h"
#include "FairRuntimeDb.h"
#include "PndLheCorrPar.h"
#include "FairParAsciiFileIo.h"
#include "TClass.h"										
#include <iostream>
#include <iomanip>

ClassImp(PndLheContFact)

static PndLheContFact gPndLheContFact;

PndLheContFact::PndLheContFact() {
  // Constructor (called when the library is loaded)
  fName="PndLheContFact";
  fTitle="Factory for parameter containers in libLHETrack";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void PndLheContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the Lhe library.*/

	FairContainer* p1= new FairContainer("PndLheCorrPar",
														"LHE Correlation Parameters",
														"TestDefaultContext");
	p1->addContext("TestNonDefaultContext");
	
	containers->Add(p1);
}

FairParSet* PndLheContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char* name=c->GetName();
  cout << " -I container name " << name << endl;
  FairParSet* p=NULL;
  if (strcmp(name,"PndLheCorrPar")==0) {
    p=new PndLheCorrPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  
  return p;
}
