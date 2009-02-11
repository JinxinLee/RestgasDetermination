/////////////////////////////////////////////////////////////
//
// PndDchContFact
//
// Factory for the parameter containers in libDch
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "PndDchContFact.h"
#include "PndDchGeoPar.h"
#include "PndDchDigiPar.h"

#include "FairRuntimeDb.h"

//#include "FairParRootFileIo.h"
//#include "FairParAsciiFileIo.h"
#include <iostream>
#include <iomanip>

ClassImp(PndDchContFact)

static PndDchContFact gPndDchContFact;

PndDchContFact::PndDchContFact() {
	// Constructor (called when the library is loaded)
	fName  = "PndDchContFact";
	fTitle = "Factory for parameter containers in libDch";
	setAllContainers();
	FairRuntimeDb::instance()->addContFactory(this);
}

void PndDchContFact::setAllContainers() {
	/** Creates the Container objects with all accepted contexts and adds them to
	 *  the list of containers for the DCH library.*/

	FairContainer* p = new FairContainer("PndDchGeoPar", "Dch Geometry Parameters", "TestDefaultContext");
	FairContainer* calib = new FairContainer("PndDchDigiParGarfield", "Dch Digitalization Parameters", "TestDefaultContextDigi");

	//   p->addContext("TestNonDefaultContext");
	//   calib->addContext("TestNonDefaultContextDigi");

	containers->Add(p);
	containers->Add(calib);
}

FairParSet* PndDchContFact::createContainer(FairContainer* c) {
	/** Calls the constructor of the corresponding parameter container.
	 * For an actual context, which is not an empty string and not the default context
	 * of this container, the name is concatinated with the context. */
	const char* name = c->GetName();
	FairParSet* p = NULL;
	if (strcmp(name, "PndDchGeoPar") == 0) {
		p = new PndDchGeoPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
	}
	if (strcmp(name, "PndDchDigiParGarfield") == 0) {
		p = new PndDchDigiPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
	}
	return p;
}

